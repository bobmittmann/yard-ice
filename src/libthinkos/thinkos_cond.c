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
	unsigned int wq;
	int cond;

	cond = thinkos_alloc_lo(&thinkos_rt.cond_alloc, 0);
	if (cond >= 0)
		thinkos_rt.lock[cond] = -1;

	wq = cond + THINKOS_COND_BASE;

	DCC_LOG2(LOG_TRACE, "cond=%d wq=%d", cond, wq);
	arg[0] = wq;
}

void thinkos_cond_free_svc(int32_t * arg)
{
	unsigned int wq = arg[0];
	unsigned int cond = wq - THINKOS_COND_BASE;

#if THINKOS_ENABLE_ARG_CHECK
	if (cond >= THINKOS_COND_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is conditional variable!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif

	DCC_LOG2(LOG_TRACE, "cond=%d wq=%d", cond, wq);
	__bit_mem_wr(&thinkos_rt.cond_alloc, cond, 0);
}
#endif

void thinkos_cond_wait_svc(int32_t * arg)
{
	unsigned int cwq = arg[0];
	unsigned int mwq = arg[1];
	unsigned int cond = cwq - THINKOS_COND_BASE;
	unsigned int mutex = mwq - THINKOS_MUTEX_BASE;
	int self = thinkos_rt.active;
	int th;

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

	/* assign the mutex to be locked on wakeup */
	thinkos_rt.cond_mutex[cond] = mwq;
	/* insert into the cond wait queue */
	__thinkos_wq_insert(cwq, self);
	DCC_LOG3(LOG_INFO, "<%d> mutex %d unlocked, waiting on cond %d...", 
			 self, mwq, cwq);

	/* check for threads wating on the mutex wait queue */
	if ((th = __thinkos_wq_head(mwq)) == THINKOS_THREAD_NULL) {
		/* no threads waiting on the lock, just release
		   the lock */
		thinkos_rt.lock[mutex] = -1;
	} else {
		/* set the mutex ownership to the new thread */
		thinkos_rt.lock[mutex] = th;
		DCC_LOG2(LOG_INFO, "<%d> mutex %d locked", th, mwq);
		/* wakeup from the event wait queue */
		__thinkos_wakeup(mwq, th);
	}

	arg[0] = 0;

	/* wait for event */
	__thinkos_wait();
}

#if THINKOS_ENABLE_TIMED_CALLS
void thinkos_cond_timedwait_svc(int32_t * arg)
{
	unsigned int cwq = arg[0];
	unsigned int mwq = arg[1];
	unsigned int cond = cwq - THINKOS_COND_BASE;
	unsigned int mutex = mwq - THINKOS_MUTEX_BASE;
	uint32_t ms = (uint32_t)arg[2];
	int self = thinkos_rt.active;
	int th;

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

	/* assign the mutex to be locked on wakeup */
	thinkos_rt.cond_mutex[cond] = mwq;
	/* insert into the cond wait queue */
	__thinkos_tmdwq_insert(cwq, self, ms);
	DCC_LOG3(LOG_INFO, "<%d> mutex %d unlocked, waiting on cond %d...", 
			 self, mwq, cwq);

	/* check for threads wating on the mutex wait queue */
	if ((th = __thinkos_wq_head(mwq)) == THINKOS_THREAD_NULL) {
		/* no threads waiting on the lock, just release
		   the lock */
		thinkos_rt.lock[mutex] = -1;
	} else {
		/* set the mutex ownership to the new thread */
		thinkos_rt.lock[mutex] = th;
		DCC_LOG2(LOG_INFO, "<%d> mutex %d locked", th, mwq);
		/* wakeup from the event wait queue */
		__thinkos_wakeup(mwq, th);
	}

	arg[0] = 0;

	/* wait for event */
	__thinkos_wait();
}
#endif

void thinkos_cond_signal_svc(int32_t * arg)
{	
	unsigned int cwq = arg[0];
	unsigned int cond = cwq - THINKOS_COND_BASE;
	unsigned int mwq;
	unsigned int mutex;
	int th;

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

	if ((th = __thinkos_wq_head(cwq)) == THINKOS_THREAD_NULL) {
		/* no threads waiting on the conditional variable. */ 
	} else {
		DCC_LOG2(LOG_TRACE, "<%d> wakeup from cond %d.", th, cwq);

		/* remove from the conditional variable wait queue */
		__thinkos_wq_remove(cwq, th);

		/* get the mutex to be locked */
		mwq = thinkos_rt.cond_mutex[cond];
		mutex = mwq - THINKOS_MUTEX_BASE;

		/* check whether the mutex is locked or not */
		if (thinkos_rt.lock[mutex] == -1) {
			/* no threads waiting on the lock... */
			/* set the mutex ownership to the new thread */
			thinkos_rt.lock[mutex] = th;
			DCC_LOG2(LOG_INFO, "<%d> mutex %d locked.", th, mwq);
#if THINKOS_ENABLE_THREAD_STAT
			/* update status */
			thinkos_rt.th_stat[th] = 0;
#endif
			/* set the return value */
			thinkos_rt.ctx[th]->r0 = 0;
			/* insert the thread into ready queue */
			bmp_bit_set(&thinkos_rt.wq_ready, th);

			/* signal the scheduler ... */
			__thinkos_defer_sched();
		} else { 
			/* insert into the mutex wait queue */
			__thinkos_wq_insert(mwq, th);
			DCC_LOG2(LOG_INFO, "<%d> waiting on mutex %d...", th, mwq);
		}
	}
	arg[0] = 0;
}

void thinkos_cond_broadcast_svc(int32_t * arg)
{	
	unsigned int cwq = arg[0];
	unsigned int cond = cwq - THINKOS_COND_BASE;
	unsigned int mwq;
	unsigned int mutex;
	int th;

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

	if ((th = __thinkos_wq_head(cwq)) == THINKOS_THREAD_NULL) {
		/* no threads waiting on the conditional variable. */ 
	} else {
		DCC_LOG2(LOG_TRACE, "<%d> wakeup from cond %d.", th, cwq);

		/* remove from the conditional variable wait queue */
		__thinkos_wq_remove(cwq, th);

		/* get the mutex to be locked */
		mwq = thinkos_rt.cond_mutex[cond];
		mutex = mwq - THINKOS_MUTEX_BASE;

		/* check whether the mutex is locked or not */
		if (thinkos_rt.lock[mutex] == -1) {
			/* no threads waiting on the lock... */
			/* set the mutex ownership to the new thread */
			thinkos_rt.lock[mutex] = th;
			DCC_LOG2(LOG_INFO, "<%d> mutex %d locked.", th, mwq);
#if THINKOS_ENABLE_THREAD_STAT
			/* update status */
			thinkos_rt.th_stat[th] = 0;
#endif
			/* set the return value */
			thinkos_rt.ctx[th]->r0 = 0;
			/* insert the thread into ready queue */
			bmp_bit_set(&thinkos_rt.wq_ready, th);

			/* signal the scheduler ... */
			__thinkos_defer_sched();
		} else { 
			/* insert into the mutex wait queue */
			__thinkos_wq_insert(mwq, th);
			DCC_LOG2(LOG_INFO, "<%d> waiting on mutex %d...", th, mwq);
		}

		/* insert all remaining threads into mutex wait queue */
		while ((th = __thinkos_wq_head(cwq)) == THINKOS_THREAD_NULL) {
			__thinkos_wq_insert(mwq, th);
			DCC_LOG2(LOG_INFO, "<%d> waiting on mutex %d...", th, mwq);
		}
	}

	arg[0] = 0;
}

#endif /* THINKOS_COND_MAX > 0 */

