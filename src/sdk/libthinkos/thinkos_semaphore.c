/* 
 * thikos_semaphore.c
 *
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the ThinkOS library.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */


_Pragma ("GCC optimize (\"Ofast\")")

#define __THINKOS_SYS__
#include <thinkos_sys.h>
#include <thinkos.h>

#if THINKOS_SEMAPHORE_MAX > 0

#if THINKOS_ENABLE_SEM_ALLOC
void thinkos_sem_alloc_svc(int32_t * arg)
{	
	unsigned int wq;
	uint32_t value = (uint32_t)arg[0];
	int idx;

	if ((idx = __thinkos_bmp_alloc(thinkos_rt.sem_alloc, 
								   THINKOS_SEMAPHORE_MAX )) >= 0) {
		thinkos_rt.sem_val[idx] = value;
		wq = idx + THINKOS_SEM_BASE;
		DCC_LOG2(LOG_INFO, "sem=%d wq=%d", idx, wq);
		arg[0] = wq;
	} else {
		DCC_LOG(LOG_WARNING, "__thinkos_bmp_alloc() failed!");
		arg[0] = idx;
	}	
}

void thinkos_sem_free_svc(int32_t * arg)
{	
	unsigned int wq = arg[0];
	unsigned int idx = wq - THINKOS_SEM_BASE;

#if THINKOS_ENABLE_ARG_CHECK
	if (idx >= THINKOS_SEMAPHORE_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not a semaphore!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
	__bit_mem_wr(thinkos_rt.sem_alloc, idx, 0);
}
#endif

void thinkos_sem_init_svc(int32_t * arg)
{	
	unsigned int wq = arg[0];
	unsigned int sem = wq - THINKOS_SEM_BASE;
	uint32_t value = (uint32_t)arg[1];

#if THINKOS_ENABLE_ARG_CHECK
	if (sem >= THINKOS_SEMAPHORE_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not a semaphore!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_SEM_ALLOC
	if (__bit_mem_rd(thinkos_rt.sem_alloc, sem) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid semaphore %d!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	DCC_LOG2(LOG_INFO, "sem[%d] <= %d", sem, value);

	thinkos_rt.sem_val[sem] = value;
	arg[0] = 0;
}

void thinkos_sem_trywait_svc(int32_t * arg)
{	
	unsigned int wq = arg[0];
	unsigned int sem = wq - THINKOS_SEM_BASE;
	uint32_t sem_val;

#if THINKOS_ENABLE_ARG_CHECK
	if (sem >= THINKOS_SEMAPHORE_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not a semaphore!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_SEM_ALLOC
	if (__bit_mem_rd(thinkos_rt.sem_alloc, sem) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid semaphore %d!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	/* avoid possible race condition on sem_val */
	/* this is only necessary in case we use the __uthread_sem_post() call
	   inside interrupt handlers */
	do {
		sem_val = __ldrex(&thinkos_rt.sem_val[sem]);
		if (sem_val > 0) {
			sem_val--;
			arg[0] = 0;
		} else {
			arg[0] = THINKOS_EAGAIN;
		}
	} while (__strex(&thinkos_rt.sem_val[sem], sem_val));
}

void thinkos_sem_wait_svc(int32_t * arg)
{	
	unsigned int wq = arg[0];
	unsigned int sem = wq - THINKOS_SEM_BASE;
	int self = thinkos_rt.active;
	uint32_t sem_val;
	uint32_t queue;

#if THINKOS_ENABLE_ARG_CHECK
	if (sem >= THINKOS_SEMAPHORE_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not a semaphore!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_SEM_ALLOC
	if (__bit_mem_rd(thinkos_rt.sem_alloc, sem) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid semaphore %d!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	/* avoid possible race condition on sem_val */
	/* this is only necessary in case we use the __uthread_sem_post() call
	   inside interrupt handlers */
again:
	sem_val = __ldrex(&thinkos_rt.sem_val[sem]);
	if (sem_val > 0) {
		DCC_LOG2(LOG_INFO, "<%d> signaled %d...", self, wq);
		sem_val--;
		if (__strex(&thinkos_rt.sem_val[sem], sem_val))
			goto again;
		arg[0] = 0;
		return;
	}

	/* (1) suspend the thread by removing it from the
	   ready wait queue. The __thinkos_suspend() call cannot be nested
	   inside a LDREX/STREX pair as it may use the exclusive access itself,
	   in case we have anabled the time sharing option.
	   It is not a problem having a thread not contained in any waiting
	   queue inside a system call. 
	 */
	__thinkos_suspend(self);
	/* update the thread status in preparation for event wait */
#if THINKOS_ENABLE_THREAD_STAT
	thinkos_rt.th_stat[self] = wq << 1;
#endif
	/* (2) Save the context pointer. In case an interrupt wakes up
	   this thread before the scheduler is called, this will allow
	   the interrupt handler to locate the return value (r0) address. */
	thinkos_rt.ctx[self] = (struct thinkos_context *)&arg[-CTX_R0];

	/* insert into the event wait queue */
	queue = __ldrex(&thinkos_rt.wq_lst[wq]);

	/* The semaphore may have been signaled while suspending (1).
	 If this is the case roll back and restart. */
	if ((volatile uint32_t)thinkos_rt.sem_val[sem] > 0) {
		/* roll back */
#if THINKOS_ENABLE_THREAD_STAT
		thinkos_rt.th_stat[self] = 0;
#endif
		/* insert into the ready wait queue */
		__bit_mem_wr(&thinkos_rt.wq_ready, self, 1);  
		DCC_LOG2(LOG_WARNING, "<%d> rollback 1 %d...", self, wq);
		goto again;
	}

	queue |= (1 << self);
	if (__strex(&thinkos_rt.wq_lst[wq], queue)) {
		/* roll back */
#if THINKOS_ENABLE_THREAD_STAT
		thinkos_rt.th_stat[self] = 0;
#endif
		/* insert into the ready wait queue */
		__bit_mem_wr(&thinkos_rt.wq_ready, self, 1);  
		DCC_LOG2(LOG_WARNING, "<%d> rollback 2 %d...", self, wq);
		goto again;
	}

	/* -- wait for event ---------------------------------------- */
	DCC_LOG2(LOG_INFO, "<%d> waiting on semaphore %d...", self, wq);
	/* signal the scheduler ... */
	__thinkos_defer_sched(); 
}

#if THINKOS_ENABLE_TIMED_CALLS
void thinkos_sem_timedwait_svc(int32_t * arg)
{	
	unsigned int wq = arg[0];
	unsigned int sem = wq - THINKOS_SEM_BASE;
	uint32_t ms = (uint32_t)arg[1];
	int self = thinkos_rt.active;
	uint32_t sem_val;
	uint32_t queue;

#if THINKOS_ENABLE_ARG_CHECK
	if (sem >= THINKOS_SEMAPHORE_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not a semaphore!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_SEM_ALLOC
	if (__bit_mem_rd(thinkos_rt.sem_alloc, sem) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid semaphore %d!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

again:
	sem_val = __ldrex(&thinkos_rt.sem_val[sem]);
	if (sem_val > 0) {
		DCC_LOG2(LOG_INFO, "<%d> signaled %d...", self, wq);
		sem_val--;
		if (__strex(&thinkos_rt.sem_val[sem], sem_val))
			goto again;
		arg[0] = 0;
		return;
	}

	__thinkos_suspend(self);
#if THINKOS_ENABLE_THREAD_STAT
	/* update status, mark the thread clock enable bit */
	thinkos_rt.th_stat[self] = (wq << 1) + 1;
#endif
	thinkos_rt.ctx[self] = (struct thinkos_context *)&arg[-CTX_R0];
	queue = __ldrex(&thinkos_rt.wq_lst[wq]);
	queue |= (1 << self);
	if (((volatile uint32_t)thinkos_rt.sem_val[sem] > 0) ||
		__strex(&thinkos_rt.wq_lst[wq], queue)) {
		/* roll back */
#if THINKOS_ENABLE_THREAD_STAT
		thinkos_rt.th_stat[self] = 0;
#endif
		/* insert into the ready wait queue */
		__bit_mem_wr(&thinkos_rt.wq_ready, self, 1);  
		goto again;
	}

	/* -- wait for event ---------------------------------------- */
	DCC_LOG2(LOG_INFO, "<%d> waiting on semaphore %d...", self, wq);
	/* set the clock */
	thinkos_rt.clock[self] = thinkos_rt.ticks + ms;
	/* insert into the clock wait queue */
	__bit_mem_wr(&thinkos_rt.wq_clock, self, 1);  
	/* Set the default return value to timeout. The
	   sem_post call will change this to 0 */
	arg[0] = THINKOS_ETIMEDOUT;
	/* signal the scheduler ... */
	__thinkos_defer_sched(); 
}
#endif

//void __thinkos_sem_post_i(uint32_t wq)
void cm3_except7_isr(uint32_t wq)
{
	uint32_t queue;
	int th;

	do {
		/* insert into the event wait queue */
		queue = __ldrex(&thinkos_rt.wq_lst[wq]);
		/* get a thread from the queue bitmap */
		if ((th = __clz(__rbit(queue))) == THINKOS_THREAD_NULL) {
			int sem = wq - THINKOS_SEM_BASE;
			uint32_t sem_val;

			/* no threads waiting on the semaphore, increment. */ 
			do {
				sem_val = __ldrex(&thinkos_rt.sem_val[sem]);
				sem_val++;
			} while (__strex(&thinkos_rt.sem_val[sem], sem_val));

			return;
		} 
	
		/* remove from the wait queue */
		queue &= ~(1 << th);
	} while (__strex(&thinkos_rt.wq_lst[wq], queue));

	/* insert the thread into ready queue */
	__bit_mem_wr(&thinkos_rt.wq_ready, th, 1);
#if THINKOS_ENABLE_TIMED_CALLS
	/* possibly remove from the time wait queue */
	__bit_mem_wr(&thinkos_rt.wq_clock, th, 0);  
	/* set the thread's return value */
	thinkos_rt.ctx[th]->r0 = 0;
#endif
#if THINKOS_ENABLE_THREAD_STAT
	/* update status */
	thinkos_rt.th_stat[th] = 0;
#endif
	/* signal the scheduler ... */
	__thinkos_defer_sched();
}

void __thinkos_sem_post_i(uint32_t wq) 
	__attribute__((weak, alias("cm3_except7_isr")));


void thinkos_sem_post_svc(int32_t * arg)
{	
	unsigned int wq = arg[0];

#if THINKOS_ENABLE_ARG_CHECK
	unsigned int sem = wq - THINKOS_SEM_BASE;

	if (sem >= THINKOS_SEMAPHORE_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not a semaphore!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_SEM_ALLOC
	if (__bit_mem_rd(thinkos_rt.sem_alloc, sem) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid semaphore %d!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	DCC_LOG1(LOG_INFO, "sem %d +++++++++++++ ", wq);

	arg[0] = 0;
	__thinkos_sem_post_i(wq);
}

#endif /* THINKOS_SEM_MAX > 0 */

