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

#define __THINKOS_SYS__
#include <thinkos_sys.h>

#include <thinkos.h>

#include <sys/dcclog.h>

#if THINKOS_SEMAPHORE_MAX > 0

#if THINKOS_ENABLE_SEM_ALLOC
void thinkos_sem_alloc_svc(int32_t * arg)
{	
	unsigned int wq;
	uint32_t value = (uint32_t)arg[0];
	int sem;

	if ((sem = thinkos_alloc_lo(thinkos_rt.sem_alloc, 0)) >= 0) {
		thinkos_rt.sem_val[sem] = value;
		wq = sem + THINKOS_SEM_BASE;
		DCC_LOG2(LOG_TRACE, "sem=%d wq=%d", sem, wq);
		arg[0] = wq;
	} else {
		arg[0] = sem;
	}
}

void thinkos_sem_free_svc(int32_t * arg)
{	
	unsigned int wq = arg[0];
	unsigned int sem = wq - THINKOS_SEM_BASE;

#if THINKOS_ENABLE_ARG_CHECK
	if (sem >= THINKOS_SEMAPHORE_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not a semaphore!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif

	DCC_LOG2(LOG_TRACE, "sem=%d wq=%d", sem, wq);
	__bit_mem_wr(&thinkos_rt.sem_alloc, sem, 0);
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
	if (__bit_mem_rd(&thinkos_rt.sem_alloc, sem) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid semaphore %d!", sem);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	DCC_LOG2(LOG_TRACE, "sem[%d] <= %d", sem, value);

	thinkos_rt.sem_val[sem] = value;
	arg[0] = 0;
}

void thinkos_sem_wait_svc(int32_t * arg)
{	
	unsigned int wq = arg[0];
	unsigned int sem = wq - THINKOS_SEM_BASE;
	int self = thinkos_rt.active;

#if THINKOS_ENABLE_ARG_CHECK
	if (sem >= THINKOS_SEMAPHORE_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not a semaphore!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_SEM_ALLOC
	if (__bit_mem_rd(&thinkos_rt.sem_alloc, sem) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid semaphore %d!", sem);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	arg[0] = 0;

	/* avoid possible race condition on sem_val */
	/* this is only necessary in case we use the __uthread_sem_post() call
	   inside interrupt handlers */
	/* TODO: study the possibility of using exclusive access instead of 
	   disabling interrupts. */
	cm3_cpsid_i();

	if (thinkos_rt.sem_val[sem] > 0) {
		thinkos_rt.sem_val[sem]--;
	} else {
		/* insert into the semaphore wait queue */
		__thinkos_wq_insert(wq, self);
		DCC_LOG2(LOG_INFO, "<%d> waiting on semaphore %d...", self, wq);
		/* wait for event */
		/* remove from the ready wait queue */
		__bit_mem_wr(&thinkos_rt.wq_ready, self, 0);  
#if THINKOS_ENABLE_TIMESHARE
		/* if the ready queue is empty, collect
		   the threads from the CPU wait queue */
		if (thinkos_rt.wq_ready == 0) {
			thinkos_rt.wq_ready = thinkos_rt.wq_tmshare;
			thinkos_rt.wq_tmshare = 0;
		}
#endif
	}

	/* reenable interrupts ... */
	cm3_cpsie_i();

	/* signal the scheduler ... */
	__thinkos_defer_sched();
}

void thinkos_sem_trywait_svc(int32_t * arg)
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
	if (__bit_mem_rd(&thinkos_rt.sem_alloc, sem) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid semaphore %d!", sem);
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
	cm3_cpsid_i();

	if (thinkos_rt.sem_val[sem] > 0) {
		thinkos_rt.sem_val[sem]--;
		arg[0] = 0;
	} else {
		arg[0] = THINKOS_EAGAIN;
	}

	cm3_cpsie_i();
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
	if (__bit_mem_rd(&thinkos_rt.sem_alloc, sem) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid semaphore %d!", sem);
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
	cm3_cpsid_i();

	if (thinkos_rt.sem_val[sem] > 0) {
		thinkos_rt.sem_val[sem]--;
		arg[0] = 0;
	} else {
		/* insert into the semaphore wait queue */
		__thinkos_tmdwq_insert(wq, self, ms);
		DCC_LOG2(LOG_INFO, "<%d> waiting on semaphore %d...", self, wq);
		/* wait for event */
		/* remove from the ready wait queue */
		__bit_mem_wr(&thinkos_rt.wq_ready, self, 0);  
#if THINKOS_ENABLE_TIMESHARE
		/* if the ready queue is empty, collect
		   the threads from the CPU wait queue */
		if (thinkos_rt.wq_ready == 0) {
			thinkos_rt.wq_ready = thinkos_rt.wq_tmshare;
			thinkos_rt.wq_tmshare = 0;
		}
#endif
		/* Set the default return value to timeout. The
		   sem_post call will change this to 0 */
		arg[0] = THINKOS_ETIMEDOUT;
	}

	/* reenable interrupts ... */
	cm3_cpsie_i();

	/* signal the scheduler ... */
	__thinkos_defer_sched();
}
#endif

void thinkos_sem_post_svc(int32_t * arg)
{	
	unsigned int wq = arg[0];
	unsigned int sem = wq - THINKOS_SEM_BASE;
	int th;

#if THINKOS_ENABLE_ARG_CHECK
	if (sem >= THINKOS_SEMAPHORE_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not a semaphore!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_SEM_ALLOC
	if (__bit_mem_rd(&thinkos_rt.sem_alloc, sem) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid semaphore %d!", sem);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	arg[0] = 0;

	cm3_cpsid_i();
	if ((th = __thinkos_wq_head(wq)) == THINKOS_THREAD_NULL) {
		/* no threads waiting on the semaphore, increment. */ 
		thinkos_rt.sem_val[sem]++;
	} else {
		/* wakeup from the sem wait queue */
		__thinkos_wakeup(wq, th);
		DCC_LOG2(LOG_INFO, "<%d> wakeup from sem %d ", th, wq);
		/* signal the scheduler ... */
		__thinkos_defer_sched();
	}

	cm3_cpsie_i();

}

#endif /* THINKOS_SEM_MAX > 0 */

