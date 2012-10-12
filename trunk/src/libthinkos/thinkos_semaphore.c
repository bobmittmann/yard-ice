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
	uint32_t value = (uint32_t)arg[0];
	unsigned int sem;

	if ((sem = thinkos_alloc_lo(&thinkos_rt.sem_alloc, 0)) >= 0)
		thinkos_rt.sem_val[sem] = value;

	arg[0] = sem;
}

void thinkos_sem_free_svc(int32_t * arg)
{	
	unsigned int sem = arg[0];

#if THINKOS_ENABLE_ARG_CHECK
	if (sem >= THINKOS_SEMAPHORE_MAX) {
		DCC_LOG1(LOG_ERROR, "invalid semaphore %d!", sem);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif

	__bit_mem_wr(&thinkos_rt.sem_alloc, sem, 0);
}
#endif

void thinkos_sem_init_svc(int32_t * arg)
{	
	unsigned int sem = arg[0];
	uint32_t value = (uint32_t)arg[1];

#if THINKOS_ENABLE_ARG_CHECK
	if (sem >= THINKOS_SEMAPHORE_MAX) {
		DCC_LOG1(LOG_ERROR, "invalid semaphore %d!", sem);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_SEM_ALLOC
	if (__bit_mem_rd(&thinkos_rt.sem_alloc, sem) == 0) {
		arg[0] = -1;
		return;
	}
#endif
#endif

	thinkos_rt.sem_val[sem] = value;
	arg[0] = 0;
}

void thinkos_sem_wait_svc(int32_t * arg)
{	
	unsigned int sem = arg[0];

#if THINKOS_ENABLE_ARG_CHECK
	if (sem >= THINKOS_SEMAPHORE_MAX) {
		DCC_LOG1(LOG_ERROR, "invalid semaphore %d!", sem);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_SEM_ALLOC
	if (__bit_mem_rd(&thinkos_rt.sem_alloc, sem) == 0) {
		arg[0] = -1;
		return;
	}
#endif
#endif

	if (thinkos_rt.sem_val[sem] > 0) {
		thinkos_rt.sem_val[sem]--;
	//	printf("%s(): <%d> ...\n", __func__, thinkos_rt.active);
	} else {
		int this = thinkos_rt.active;

		bmp_bit_set(&thinkos_rt.wq_sem[sem], this);  
#if THINKOS_ENABLE_THREAD_STAT
		/* update status */
		thinkos_rt.th_stat[this] = __wq_idx(&thinkos_rt.wq_sem[sem]) << 1;
#endif

//		printf("%s(): <%d> wait...\n", __func__, this);

		/* wait for event */
		__thinkos_wait();
	}

	arg[0] = 0;
}

void thinkos_sem_trywait_svc(int32_t * arg)
{	
	unsigned int sem = arg[0];

#if THINKOS_ENABLE_ARG_CHECK
	if (sem >= THINKOS_SEMAPHORE_MAX) {
		DCC_LOG1(LOG_ERROR, "invalid semaphore %d!", sem);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_SEM_ALLOC
	if (__bit_mem_rd(&thinkos_rt.sem_alloc, sem) == 0) {
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	if (thinkos_rt.sem_val[sem] > 0) {
		thinkos_rt.sem_val[sem]--;
		arg[0] = 0;
	} else {
		arg[0] = THINKOS_EAGAIN;
	}

	arg[0] = 0;
}

#if THINKOS_ENABLE_TIMED_CALLS
void thinkos_sem_timedwait_svc(int32_t * arg)
{	
	unsigned int sem = arg[0];
	uint32_t ms = (uint32_t)arg[1];
	int this = thinkos_rt.active;

#if THINKOS_ENABLE_ARG_CHECK
	if (sem >= THINKOS_SEMAPHORE_MAX) {
		DCC_LOG1(LOG_ERROR, "invalid semaphore %d!", sem);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_SEM_ALLOC
	if (__bit_mem_rd(&thinkos_rt.sem_alloc, sem) == 0) {
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	if (thinkos_rt.sem_val[sem] > 0) {
		thinkos_rt.sem_val[sem]--;
		arg[0] = 0;
		return;
	}

	/* insert into the sem wait queue */
	bmp_bit_set(&thinkos_rt.wq_sem[sem], this);  

	/* set the clock */
	thinkos_rt.clock[this] = thinkos_rt.ticks + (ms / 1);
	/* insert into the clock wait queue */
	bmp_bit_set(&thinkos_rt.wq_clock, this);  

#if THINKOS_ENABLE_THREAD_STAT
	/* update status, mark the thread clock enable bit */
	thinkos_rt.th_stat[this] = (__wq_idx(&thinkos_rt.wq_sem[sem]) << 1) + 1;
#endif

	/* Set the default return value to timeout. The
	   sem_post call will change this to 0 */
	arg[0] = THINKOS_ETIMEDOUT;

	/* wait for event */
	__thinkos_wait();
}
#endif

void thinkos_sem_post_svc(int32_t * arg)
{	
	unsigned int sem = arg[0];
	int idx;

#if THINKOS_ENABLE_ARG_CHECK
	if (sem >= THINKOS_SEMAPHORE_MAX) {
		DCC_LOG1(LOG_ERROR, "invalid semaphore %d!", sem);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_SEM_ALLOC
	if (__bit_mem_rd(&thinkos_rt.sem_alloc, sem) == 0) {
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	idx = __thinkos_wq_head(&thinkos_rt.wq_sem[sem]);
	if (idx == THINKOS_IDX_NULL) {
		/* no threads waiting on the semaphore, increment. */ 
		thinkos_rt.sem_val[sem]++;
	} else {
//		printf("%s(): <%d> wakeup ...\n", __func__, idx);

		/* remove from the sem wait queue */
		bmp_bit_clr(&thinkos_rt.wq_sem[sem], idx);  
#if THINKOS_ENABLE_TIMED_CALLS
		/* possibly remove from the time wait queue */
		bmp_bit_clr(&thinkos_rt.wq_clock, idx);  
#endif
#if THINKOS_ENABLE_THREAD_STAT
		/* update status */
		thinkos_rt.th_stat[idx] = 0;
#endif
		/* set the thread's return value */
		thinkos_rt.ctx[idx]->r0 = 0;
		/* insert the thread into ready queue */
		bmp_bit_set(&thinkos_rt.wq_ready, idx);

		/* signal the scheduler ... */
		__thinkos_defer_sched();
	}
	arg[0] = 0;
}

#endif /* THINKOS_SEM_MAX > 0 */

