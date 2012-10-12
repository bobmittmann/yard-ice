/* 
 * thikos_cond.c
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

#if THINKOS_COND_MAX > 0

#if THINKOS_ENABLE_COND_ALLOC
void thinkos_cond_alloc_svc(int32_t * arg)
{
	int cond;

	cond = thinkos_alloc_lo(&thinkos_rt.cond_alloc, 0);
	if (cond >= 0)
		thinkos_rt.lock[cond] = -1;
	arg[0] = cond;

	DCC_LOG1(LOG_TRACE, "cond=%d", cond);
}

void thinkos_cond_free_svc(int32_t * arg)
{
	unsigned int cond = arg[0];

#if THINKOS_ENABLE_ARG_CHECK
	if (cond >= THINKOS_COND_MAX) {
		DCC_LOG1(LOG_ERROR, "invalid conditional variable %d!", cond);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif

	DCC_LOG1(LOG_TRACE, "cond=%d", cond);
	__bit_mem_wr(&thinkos_rt.cond_alloc, cond, 0);
}
#endif

void thinkos_cond_wait_svc(int32_t * arg)
{
	unsigned int cond = arg[0];
	unsigned int mutex = arg[1];
	int this = thinkos_rt.active;
	int idx;

#if THINKOS_ENABLE_ARG_CHECK
	if (mutex >= THINKOS_MUTEX_MAX) {
		DCC_LOG1(LOG_ERROR, "invalid mutex %d!", mutex);
		arg[0] = THINKOS_EINVAL;
		return;
	}
	if (cond >= THINKOS_COND_MAX) {
		DCC_LOG1(LOG_ERROR, "invalid conditional variable %d!", cond);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_MUTEX_ALLOC
	if (__bit_mem_rd(&thinkos_rt.mutex_alloc, mutex) == 0) {
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#if THINKOS_ENABLE_COND_ALLOC
	if (__bit_mem_rd(&thinkos_rt.cond_alloc, cond) == 0) {
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	/* sanity check: avoid unlock the mutex by a thread that 
	   does not own the lock */
	if (thinkos_rt.lock[mutex] != thinkos_rt.active) {
		arg[0] = THINKOS_EPERM;
		return;
	}

	DCC_LOG3(LOG_TRACE, "<%d> wait on condition %d with mutex %d", 
			 this, cond, mutex);

	/* assign the mutex to be locked on wakeup */
	thinkos_rt.cond_mutex[cond] = mutex;
	/* insert into wait queue */
	bmp_bit_set(&thinkos_rt.wq_cond[cond], this);  
#if THINKOS_ENABLE_THREAD_STAT
	/* update status */
	thinkos_rt.th_stat[this] = __wq_idx(&thinkos_rt.wq_cond[cond]) << 1;
#endif

	idx = __thinkos_wq_head(&thinkos_rt.wq_mutex[mutex]);
	if (idx == THINKOS_IDX_NULL) {
		/* no threads waiting on the lock, just release
		   the lock */
		thinkos_rt.lock[mutex] = -1;
	} else {
		/* set the mutex ownership to the new thread */
		thinkos_rt.lock[mutex] = (idx << 1) + 1;
		/* remove from the mutex wait queue */
		bmp_bit_clr(&thinkos_rt.wq_mutex[mutex], idx);  
#if THINKOS_ENABLE_TIMED_CALLS
		/* possibly remove from the time wait queue */
		bmp_bit_clr(&thinkos_rt.wq_clock, idx);  
#endif

#if THINKOS_ENABLE_THREAD_STAT
		/* update status */
		thinkos_rt.th_stat[idx] = 0;
#endif
		/* set the return value */
		thinkos_rt.ctx[idx]->r0 = 0;
		/* insert the thread into ready queue */
		bmp_bit_set(&thinkos_rt.wq_ready, idx);

		/* signal the scheduler ... */
		__thinkos_defer_sched();
	}

	/* wait for event */
	__thinkos_wait();

	arg[0] = 0;
}

#if THINKOS_ENABLE_TIMED_CALLS
void thinkos_cond_timedwait_svc(int32_t * arg)
{
	int this = thinkos_rt.active;
	unsigned int cond = arg[0];
	unsigned int mutex = arg[1];
	uint32_t ms = (uint32_t)arg[2];
	int idx;

#if THINKOS_ENABLE_ARG_CHECK
	if (mutex >= THINKOS_MUTEX_MAX) {
		DCC_LOG1(LOG_ERROR, "invalid mutex %d!", mutex);
		arg[0] = THINKOS_EINVAL;
		return;
	}
	if (cond >= THINKOS_COND_MAX) {
		DCC_LOG1(LOG_ERROR, "invalid conditional variable %d!", cond);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_MUTEX_ALLOC
	if (__bit_mem_rd(&thinkos_rt.mutex_alloc, mutex) == 0) {
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#if THINKOS_ENABLE_COND_ALLOC
	if (__bit_mem_rd(&thinkos_rt.cond_alloc, cond) == 0) {
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	/* sanity check: avoid unlock the mutex by a thread that 
	   does not own the lock */
	if (thinkos_rt.lock[mutex] != thinkos_rt.active) {
		arg[0] = THINKOS_EPERM;
		return;
	}

	DCC_LOG3(LOG_TRACE, "<%d> wait on condition %d with mutex %d", 
			 this, cond, mutex);

	/* assign the mutex to be locked on wakeup */
	thinkos_rt.cond_mutex[cond] = mutex;
	/* insert into cond wait queue */
	bmp_bit_set(&thinkos_rt.wq_cond[cond], this);  
	/* set the clock */
	thinkos_rt.clock[this] = thinkos_rt.ticks + (ms / 1);
	/* insert into the clock wait queue */
	bmp_bit_set(&thinkos_rt.wq_clock, this);  
#if THINKOS_ENABLE_THREAD_STAT
	/* update status */
	thinkos_rt.th_stat[this] = (__wq_idx(&thinkos_rt.wq_cond[cond]) << 1) + 1;
#endif
	thinkos_rt.ctx[this]->r0 = THINKOS_ETIMEDOUT;

	idx = __thinkos_wq_head(&thinkos_rt.wq_mutex[mutex]);
	if (idx == THINKOS_IDX_NULL) {
		/* no threads waiting on the lock, just release
		   the lock */
		thinkos_rt.lock[mutex] = -1;
	} else {
		/* set the mutex ownership to the new thread */
		thinkos_rt.lock[mutex] = (idx << 1) + 1;
		/* remove from the mutex wait queue */
		bmp_bit_clr(&thinkos_rt.wq_mutex[mutex], idx);  
#if THINKOS_ENABLE_TIMED_CALLS
		/* possibly remove from the time wait queue */
		bmp_bit_clr(&thinkos_rt.wq_clock, idx);  
#endif

#if THINKOS_ENABLE_THREAD_STAT
		/* update status */
		thinkos_rt.th_stat[idx] = 0;
#endif
		/* set the return value */
		thinkos_rt.ctx[idx]->r0 = 0;
		/* insert the thread into ready queue */
		bmp_bit_set(&thinkos_rt.wq_ready, idx);

		/* signal the scheduler ... */
		__thinkos_defer_sched();
	}

	/* wait for event */
	__thinkos_wait();

	arg[0] = 0;
}
#endif

void thinkos_cond_signal_svc(int32_t * arg)
{	
	int cond = arg[0];
	int mutex;
	int idx;

#if THINKOS_ENABLE_ARG_CHECK
	if (cond >= THINKOS_COND_MAX) {
		DCC_LOG1(LOG_ERROR, "invalid conditional variable %d!", cond);
		arg[0] = THINKOS_EINVAL;
		return;
	}

#if THINKOS_ENABLE_COND_ALLOC
	if (__bit_mem_rd(&thinkos_rt.cond_alloc, cond) == 0) {
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	idx = __thinkos_wq_head(&thinkos_rt.wq_cond[cond]);
	if (idx == THINKOS_IDX_NULL) {
		/* no threads waiting on the conditional variable. */ 
		DCC_LOG2(LOG_TRACE, "<%d> no thread waiting on condition %d", 
				 thinkos_rt.active, cond);
	} else {
		/* remove from the cond wait queue */
		bmp_bit_clr(&thinkos_rt.wq_cond[cond], idx);  
		/* get the mutex to be locked */
		mutex = thinkos_rt.cond_mutex[cond];

		DCC_LOG3(LOG_TRACE, "<%d> wakeup, condition %d with mutex %d", 
				 idx, cond, mutex);

		/* check whether the mutex is locked or not */
		if (thinkos_rt.lock[mutex] == -1) {
			/* no threads waiting on the lock, get the lock */
			thinkos_rt.lock[mutex] = idx + 1;
#if THINKOS_ENABLE_TIMED_CALLS
			/* possibly remove from the time wait queue */
			bmp_bit_clr(&thinkos_rt.wq_clock, idx);  
#endif

#if THINKOS_ENABLE_THREAD_STAT
			/* update status */
			thinkos_rt.th_stat[idx] = 0;
#endif
			/* set the return value */
			thinkos_rt.ctx[idx]->r0 = 0;
			/* insert the thread into ready queue */
			bmp_bit_set(&thinkos_rt.wq_ready, idx);

			/* signal the scheduler ... */
			__thinkos_defer_sched();
		} else { 
			/* insert into the mutex wait queue */
			bmp_bit_set(&thinkos_rt.wq_mutex[mutex], idx);  
#if THINKOS_ENABLE_THREAD_STAT
			/* update status */
			thinkos_rt.th_stat[idx] = __wq_idx(&thinkos_rt.wq_mutex[mutex]) << 1;
#endif
		}
	}
	arg[0] = 0;
}

void thinkos_cond_broadcast_svc(int32_t * arg)
{	
	int cond = arg[0];
	int mutex;
	int idx;

#if THINKOS_ENABLE_ARG_CHECK
	if (cond >= THINKOS_COND_MAX) {
		DCC_LOG1(LOG_ERROR, "invalid conditional variable %d!", cond);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_COND_ALLOC
	if (__bit_mem_rd(&thinkos_rt.cond_alloc, cond) == 0) {
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	idx = __thinkos_wq_head(&thinkos_rt.wq_cond[cond]);
	if (idx == THINKOS_IDX_NULL) {
		/* no threads waiting on the conditional variable. */ 
	} else {
		/* remove from the cond wait queue */
		bmp_bit_clr(&thinkos_rt.wq_cond[cond], idx);  
		/* get the mutex to be locked */
		mutex = thinkos_rt.cond_mutex[cond];

		if (thinkos_rt.lock[mutex] == -1) {
			/* no threads waiting on the lock, get the lock */
			thinkos_rt.lock[mutex] = idx;
#if THINKOS_ENABLE_TIMED_CALLS
			/* possibly remove from the time wait queue */
			bmp_bit_clr(&thinkos_rt.wq_clock, idx);  
#endif
#if THINKOS_ENABLE_THREAD_STAT
			/* update status */
			thinkos_rt.th_stat[idx] = 0;
#endif
			/* set the return value */
			thinkos_rt.ctx[idx]->r0 = 0;
			/* insert the thread into ready queue */
			bmp_bit_set(&thinkos_rt.wq_ready, idx);

			/* signal the scheduler ... */
			__thinkos_defer_sched();
		} else { 
			/* insert into the mutex wait queue */
			bmp_bit_set(&thinkos_rt.wq_mutex[mutex], idx);  
#if THINKOS_ENABLE_THREAD_STAT
			/* update status */
			thinkos_rt.th_stat[idx] = __wq_idx(&thinkos_rt.wq_mutex[mutex]) << 1;
#endif
		}

		while ((idx = __thinkos_wq_head(&thinkos_rt.wq_cond[cond])) 
			   != THINKOS_IDX_NULL) {
			/* insert into the mutex wait queue */
			bmp_bit_set(&thinkos_rt.wq_mutex[mutex], idx);  
#if THINKOS_ENABLE_THREAD_STAT
			/* update status */
			thinkos_rt.th_stat[idx] = __wq_idx(&thinkos_rt.wq_mutex[mutex]) << 1;
#endif
		}
	}

	arg[0] = 0;
}

#endif /* THINKOS_COND_MAX > 0 */

