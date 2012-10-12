/* 
 * thikos_mutex.c
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

#if THINKOS_MUTEX_MAX > 0

#if THINKOS_ENABLE_MUTEX_ALLOC
void thinkos_mutex_alloc_svc(int32_t * arg)
{
	int mutex;

	mutex = thinkos_alloc_lo(&thinkos_rt.mutex_alloc, 0);
	if (mutex >= 0)
		thinkos_rt.lock[mutex] = -1;
	arg[0] = mutex;

	DCC_LOG1(LOG_TRACE, "mutex=%d", mutex);
}

void thinkos_mutex_free_svc(int32_t * arg)
{
	int mutex = arg[0];

#if THINKOS_ENABLE_ARG_CHECK
	if (mutex >= THINKOS_MUTEX_MAX) {
		DCC_LOG1(LOG_ERROR, "invalid mutex %d!", mutex);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif

	DCC_LOG1(LOG_TRACE, "mutex=%d", mutex);
	__bit_mem_wr(&thinkos_rt.mutex_alloc, mutex, 0);
}
#endif

void thinkos_mutex_lock_svc(int32_t * arg)
{
	unsigned int mutex = arg[0];
	int this = thinkos_rt.active;

#if THINKOS_ENABLE_ARG_CHECK
	if (mutex >= THINKOS_MUTEX_MAX) {
		DCC_LOG1(LOG_ERROR, "invalid mutex %d!", mutex);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_MUTEX_ALLOC
	if (__bit_mem_rd(&thinkos_rt.mutex_alloc, mutex) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid mutex %d!", mutex);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	if (thinkos_rt.lock[mutex] == -1) {
		thinkos_rt.lock[mutex] = this;
		DCC_LOG2(LOG_TRACE, "<%d> mutex lock %d ", this, mutex);
	} else {
		/* Sanity check: the current thread already owns the lock */
		if (thinkos_rt.lock[mutex] == this) {
			arg[0] = THINKOS_EDEADLK;
			return;
		}
		/* insert into the mutex wait queue */
		bmp_bit_set(&thinkos_rt.wq_mutex[mutex], this);  
#if THINKOS_ENABLE_THREAD_STAT
		/* update status */
		thinkos_rt.th_stat[this] = __wq_idx(&thinkos_rt.wq_mutex[mutex]) << 1;
#endif
		DCC_LOG2(LOG_TRACE, "<%d> wait on mutex %d ", thinkos_rt.active, mutex);
		/* wait for event */
		__thinkos_wait();
	}

	arg[0] = 0;
}

void thinkos_mutex_trylock_svc(int32_t * arg)
{
	unsigned int mutex = arg[0];
	int this = thinkos_rt.active;

#if THINKOS_ENABLE_ARG_CHECK
	if (mutex >= THINKOS_MUTEX_MAX) {
		DCC_LOG1(LOG_ERROR, "invalid mutex %d!", mutex);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_MUTEX_ALLOC
	if (__bit_mem_rd(&thinkos_rt.mutex_alloc, mutex) == 0) {
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	if (thinkos_rt.lock[mutex] == -1) {
		thinkos_rt.lock[mutex] = this;
		arg[0] = 0;
	} else {
		if (thinkos_rt.lock[mutex] == this)
			arg[0] = THINKOS_EDEADLK;
		else
			arg[0] = THINKOS_EAGAIN;
	}
}

#if THINKOS_ENABLE_TIMED_CALLS
void thinkos_mutex_timedlock_svc(int32_t * arg)
{
	unsigned int mutex = arg[0]; 
	uint32_t ms = (uint32_t)arg[1];
	int this = thinkos_rt.active;

#if THINKOS_ENABLE_ARG_CHECK
	if (mutex >= THINKOS_MUTEX_MAX) {
		DCC_LOG1(LOG_ERROR, "invalid mutex %d!", mutex);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_MUTEX_ALLOC
	if (__bit_mem_rd(&thinkos_rt.mutex_alloc, mutex) == 0) {
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	if (thinkos_rt.lock[mutex] == -1) {
		thinkos_rt.lock[mutex] = this;
		arg[0] = 0;
		return;
	}

	/* Sanity check: the current thread already owns the lock */
	if (thinkos_rt.lock[mutex] == this) {
		arg[0] = THINKOS_EDEADLK;
		return;
	}

	/* insert into the mutex wait queue */
	bmp_bit_set(&thinkos_rt.wq_mutex[mutex], this);  

	/* set the clock */
	thinkos_rt.clock[this] = thinkos_rt.ticks + (ms / 1);
	/* insert into the clock wait queue */
	bmp_bit_set(&thinkos_rt.wq_clock, this);  

#if THINKOS_ENABLE_THREAD_STAT
	/* update status, mark the thread clock enable bit */
	thinkos_rt.th_stat[this] = (__wq_idx(&thinkos_rt.wq_mutex[mutex]) << 1) + 1;
#endif

	/* Set the default return value to timeout. The
	   sem_post call will change this to 0 */
	arg[0] = THINKOS_ETIMEDOUT;

	/* wait for event */
	__thinkos_wait();

}
#endif


void thinkos_mutex_unlock_svc(int32_t * arg)
{
	unsigned int mutex = arg[0];
	int idx;

#if THINKOS_ENABLE_ARG_CHECK
	if (mutex >= THINKOS_MUTEX_MAX) {
		DCC_LOG1(LOG_ERROR, "invalid mutex %d!", mutex);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_MUTEX_ALLOC
	if (__bit_mem_rd(&thinkos_rt.mutex_alloc, mutex) == 0) {
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

	DCC_LOG2(LOG_TRACE, "<%d> mutex unlock %d ", thinkos_rt.active, mutex);

	idx = __thinkos_wq_head(&thinkos_rt.wq_mutex[mutex]);
	if (idx == THINKOS_IDX_NULL) {
		/* no threads waiting on the lock, just release
		   the lock */
		thinkos_rt.lock[mutex] = -1;
	} else {
		/* set the mutex ownership to the new thread */
		thinkos_rt.lock[mutex] = idx;
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

		DCC_LOG2(LOG_TRACE, "<%d> mutex lock %d ", idx, mutex);

		/* signal the scheduler ... */
		__thinkos_defer_sched();
	}

	arg[0] = 0;
}

#endif /* THINKOS_MUTEX_MAX > 0 */

