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

	arg[0] = 0;

	/* avoid possible race condition on sem_val */
	/* this is only necessary in case we use the __uthread_sem_post() call
	   inside interrupt handlers */
again:
	sem_val = __ldrexw(&thinkos_rt.sem_val[sem]);
	if (sem_val > 0) {
		sem_val--;
		if (__strexw(&thinkos_rt.sem_val[sem], sem_val))
			goto again;
		return;
	}

	/* remove from the ready wait queue */
	__thinkos_suspend(self);

	/* insert into the event wait queue */
#if THINKOS_ENABLE_THREAD_STAT
	thinkos_rt.th_stat[self] = wq << 1;
#endif
	queue = __ldrexw(&thinkos_rt.wq_lst[wq]);
	queue |= (1 << self);
	if (__strexw(&thinkos_rt.wq_lst[wq], queue)) {
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
	/* XXX: save the context pointer */
	thinkos_rt.ctx[self] = (struct thinkos_context *)&arg[-8];
	__thinkos_defer_sched(); /* signal the scheduler ... */
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
		sem_val = __ldrexw(&thinkos_rt.sem_val[sem]);
		if (sem_val > 0) {
			sem_val--;
			arg[0] = 0;
		} else {
			arg[0] = THINKOS_EAGAIN;
		}
	} while (__strexw(&thinkos_rt.sem_val[sem], sem_val));
}

#if THINKOS_ENABLE_TIMED_CALLS
void thinkos_sem_timedwait_svc(int32_t * arg)
{	
	unsigned int wq = arg[0];
	unsigned int sem = wq - THINKOS_SEM_BASE;
	uint32_t ms = (uint32_t)arg[1];
	int self = thinkos_rt.active;

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
	/* TODO: study the possibility of using exclusive access instead of 
	   disabling interrupts. */

	if (thinkos_rt.sem_val[sem] > 0) {
		thinkos_rt.sem_val[sem]--;
		/* reenable interrupts ... */
		arg[0] = 0;
		return;
	}

	/* -- wait for event ---------------------------------------- */
	DCC_LOG2(LOG_INFO, "<%d> waiting on semaphore %d...", self, wq);
	/* Set the default return value to timeout. The
	   sem_post call will change this to 0 */
	arg[0] = THINKOS_ETIMEDOUT;
	/* insert into the semaphore wait queue */
	__thinkos_tmdwq_insert(wq, self, ms);
	/* remove from the ready wait queue */
	__thinkos_suspend(self);
	/* XXX: save the context pointer */
	thinkos_rt.ctx[self] = (struct thinkos_context *)&arg[-8];
	__thinkos_defer_sched(); /* signal the scheduler ... */
}
#endif

void __thinkos_sem_post(uint32_t wq)
{
	int th;

	DCC_LOG1(LOG_INFO, "wq=%d...", wq);
	if ((th = __thinkos_wq_head(wq)) != THINKOS_THREAD_NULL) {
		DCC_LOG1(LOG_TRACE, "wakeup sem=%d", wq);
		/* wakeup from the sem wait queue */
		__thinkos_wakeup(wq, th);
		/* signal the scheduler ... */
		__thinkos_defer_sched();
	} else {
		DCC_LOG1(LOG_INFO, "increment sem=%d", wq);
		/* no threads waiting on the semaphore, increment. */ 
		thinkos_rt.sem_val[wq - THINKOS_SEM_BASE]++;
	}
}

void thinkos_sem_post_svc(int32_t * arg)
{	
	unsigned int wq = arg[0];
	unsigned int sem = wq - THINKOS_SEM_BASE;

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

	DCC_LOG1(LOG_INFO, "sem %d +++++++++++++ ", wq);

	arg[0] = 0;
	__thinkos_sem_post(wq);
}

#endif /* THINKOS_SEM_MAX > 0 */

