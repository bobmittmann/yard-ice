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
	unsigned int wq;
	int mutex;

	mutex = thinkos_alloc_lo(&thinkos_rt.mutex_alloc, 0);
	if (mutex >= 0)
		thinkos_rt.lock[mutex] = -1;
	wq = mutex + THINKOS_MUTEX_BASE;

	DCC_LOG2(LOG_MSG, "mutex=%d wq=%d", mutex, wq);
	arg[0] = wq;
}

void thinkos_mutex_free_svc(int32_t * arg)
{
	unsigned int wq = arg[0];
	unsigned int mutex = wq - THINKOS_MUTEX_BASE;

#if THINKOS_ENABLE_ARG_CHECK
	if (mutex >= THINKOS_MUTEX_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not a mutex!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif

	DCC_LOG2(LOG_MSG, "mutex=%d wq=%d", mutex, wq);
	__bit_mem_wr(&thinkos_rt.mutex_alloc, mutex, 0);
}
#endif

void thinkos_mutex_lock_svc(int32_t * arg)
{
	unsigned int wq = arg[0];
	unsigned int mutex = wq - THINKOS_MUTEX_BASE;
	int self = thinkos_rt.active;

#if THINKOS_ENABLE_ARG_CHECK
	if (mutex >= THINKOS_MUTEX_MAX) {
		DCC_LOG1(LOG_ERROR, "invalid mutex %d!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_MUTEX_ALLOC
	if (__bit_mem_rd(&thinkos_rt.mutex_alloc, mutex) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid mutex %d!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	if (thinkos_rt.lock[mutex] == -1) {
		thinkos_rt.lock[mutex] = self;
		DCC_LOG2(LOG_TRACE, "<%d> mutex %d locked", self, wq);
		arg[0] = 0;
		return;
	}

#if THINKOS_ENABLE_DEADLOCK_CHECK
	/* Sanity check: the current thread already owns the lock */
	if (thinkos_rt.lock[mutex] == self) {
		DCC_LOG2(LOG_WARNING, "<%d> mutex %d, possible deadlock!", self, wq);
		arg[0] = THINKOS_EDEADLK;
		return;
	}
#endif
	/* insert into the mutex wait queue */
	__thinkos_wq_insert(wq, self);
	DCC_LOG2(LOG_TRACE , "<%d> waiting on mutex %d...", self, wq);

	arg[0] = 0;

	/* wait for event */
	__thinkos_wait();
}

void thinkos_mutex_trylock_svc(int32_t * arg)
{
	unsigned int wq = arg[0];
	unsigned int mutex = wq - THINKOS_MUTEX_BASE;
	int self = thinkos_rt.active;

#if THINKOS_ENABLE_ARG_CHECK
	if (mutex >= THINKOS_MUTEX_MAX) {
		DCC_LOG1(LOG_ERROR, "invalid mutex %d!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_MUTEX_ALLOC
	if (__bit_mem_rd(&thinkos_rt.mutex_alloc, mutex) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid mutex %d!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	if (thinkos_rt.lock[mutex] == -1) {
		DCC_LOG2(LOG_INFO, "<%d> mutex %d locked.", self, wq);
		thinkos_rt.lock[mutex] = self;
		arg[0] = 0;
	} else {
#if THINKOS_ENABLE_DEADLOCK_CHECK
		if (thinkos_rt.lock[mutex] == self) {
			DCC_LOG2(LOG_INFO, "<%d> mutex %d deadlock.", self, wq);
			arg[0] = THINKOS_EDEADLK;
		} else
#endif
		{
			DCC_LOG2(LOG_INFO, "<%d> mutex %d busy...", self, wq);
			arg[0] = THINKOS_EAGAIN;
		}
	}
}

#if THINKOS_ENABLE_TIMED_CALLS
void thinkos_mutex_timedlock_svc(int32_t * arg)
{
	unsigned int wq = arg[0];
	uint32_t ms = (uint32_t)arg[1];
	unsigned int mutex = wq - THINKOS_MUTEX_BASE;
	int self = thinkos_rt.active;

#if THINKOS_ENABLE_ARG_CHECK
	if (mutex >= THINKOS_MUTEX_MAX) {
		DCC_LOG1(LOG_ERROR, "invalid mutex %d!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_MUTEX_ALLOC
	if (__bit_mem_rd(&thinkos_rt.mutex_alloc, mutex) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid mutex %d!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	if (thinkos_rt.lock[mutex] == -1) {
		thinkos_rt.lock[mutex] = self;
		DCC_LOG2(LOG_MSG, "<%d> mutex %d locked.", self, wq);
		arg[0] = 0;
		return;
	}

#if THINKOS_ENABLE_DEADLOCK_CHECK
	/* Sanity check: the current thread already owns the lock */
	if (thinkos_rt.lock[mutex] == self) {
		arg[0] = THINKOS_EDEADLK;
		return;
	}
#endif

	/* insert into the mutex wait queue */
	__thinkos_tmdwq_insert(wq, self, ms);
	DCC_LOG2(LOG_MSG, "<%d> waiting on mutex %d...", self, wq);

	/* Set the default return value to timeout. The
	   mutex_unlock() call will change this to 0 */
	arg[0] = THINKOS_ETIMEDOUT;

	/* wait for event */
	__thinkos_wait();

}
#endif


void thinkos_mutex_unlock_svc(int32_t * arg)
{
	unsigned int wq = arg[0];
	unsigned int mutex = wq - THINKOS_MUTEX_BASE;
	int self = thinkos_rt.active;
	int th;

#if THINKOS_ENABLE_ARG_CHECK
	if (mutex >= THINKOS_MUTEX_MAX) {
		DCC_LOG1(LOG_ERROR, "invalid mutex %d!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_MUTEX_ALLOC
	if (__bit_mem_rd(&thinkos_rt.mutex_alloc, mutex) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid mutex %d!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	/* sanity check: avoid unlock the mutex by a thread that 
	   does not own the lock */
	if (thinkos_rt.lock[mutex] != self) {
		DCC_LOG3(LOG_ERROR, "<%d> mutex %d is locked by <%d>!", 
				 self, wq, thinkos_rt.lock[mutex]);
		arg[0] = THINKOS_EPERM;
		return;
	}

	DCC_LOG2(LOG_TRACE, "<%d> mutex %d unlocked.", self, wq);

	if ((th = __thinkos_wq_head(wq)) == THINKOS_THREAD_NULL) {
		/* no threads waiting on the lock, just release
		   the lock */
		thinkos_rt.lock[mutex] = -1;
	} else {
		/* set the mutex ownership to the new thread */
		thinkos_rt.lock[mutex] = th;
		DCC_LOG2(LOG_TRACE, "<%d> mutex %d locked.", th, wq);
		/* wakeup from the mutex wait queue */
		__thinkos_wakeup(wq, th);
		/* signal the scheduler ... */
		__thinkos_defer_sched();
	}

	arg[0] = 0;
}

#endif /* THINKOS_MUTEX_MAX > 0 */

