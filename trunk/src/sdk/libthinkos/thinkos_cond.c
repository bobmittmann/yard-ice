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
	int idx;

	if ((idx = thinkos_bmp_alloc(thinkos_rt.cond_alloc, 
								 THINKOS_COND_MAX)) >= 0) {
		wq = idx + THINKOS_COND_BASE;
		DCC_LOG2(LOG_INFO, "cond=%d wq=%d", idx, wq);
		arg[0] = wq;
	} else
		arg[0] = idx;
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

	DCC_LOG2(LOG_INFO, "cond=%d wq=%d", cond, wq);
	__bit_mem_wr(thinkos_rt.cond_alloc, cond, 0);
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
	if (cond >= THINKOS_COND_MAX) {
		DCC_LOG1(LOG_ERROR, "invalid conditional variable %d!", cwq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
	if (mutex >= THINKOS_MUTEX_MAX) {
		DCC_LOG1(LOG_ERROR, "invalid mutex %d!", mwq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_COND_ALLOC
	if (__bit_mem_rd(thinkos_rt.cond_alloc, cond) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid conditional variable %d!", cwq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#if THINKOS_ENABLE_MUTEX_ALLOC
	if (__bit_mem_rd(thinkos_rt.mutex_alloc, mutex) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid mutex %d!", mwq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	/* sanity check: avoid unlock the mutex by a thread that 
	   does not own the lock */
	if (thinkos_rt.lock[mutex] != self) {
		DCC_LOG3(LOG_WARNING, "<%d> mutex %d is locked by <%d>", 
				 self, mwq, thinkos_rt.lock[mutex]);
		arg[0] = THINKOS_EPERM;
		return;
	}
#if 0
	/* assign the mutex to be locked on wakeup */
	thinkos_rt.cond_mutex[cond] = mwq;
#endif
	/* insert into the cond wait queue */
	__thinkos_wq_insert(cwq, self);
	DCC_LOG3(LOG_INFO, "<%d> mutex %d unlocked, waiting on cond %d...", 
			 self, mwq, cwq);

	/* check for threads wating on the mutex wait queue */
	if ((th = __thinkos_wq_head(mwq)) == THINKOS_THREAD_NULL) {
		/* no threads waiting on the lock, just release
		   the lock */
		DCC_LOG2(LOG_INFO, "<%d> mutex %d released", self, mwq);
		thinkos_rt.lock[mutex] = -1;
	} else {
		/* set the mutex ownership to the new thread */
		thinkos_rt.lock[mutex] = th;
		DCC_LOG2(LOG_INFO, "<%d> mutex %d locked", th, mwq);
		/* wakeup from the mutex wait queue */
		__thinkos_wakeup(mwq, th);
	}

	arg[0] = 0;

	/* wait for event */
	__thinkos_wait(self);
}

#if 0
#if THINKOS_ENABLE_TIMED_CALLS
void thinkos_cond_hook(void)
{
	int32_t * arg = (int32_t *)cm3_sp_get();
	unsigned int ret = arg[0];
	unsigned int mwq = arg[1];
	unsigned int mutex;
	uint32_t lr = cm3_lr_get();
	int self = thinkos_rt.active;
	int th = self;

	mutex = mwq - THINKOS_MUTEX_BASE;

	(void)lr;
	(void)ret;
	(void)mwq;
	(void)mutex;

	DCC_LOG3(LOG_TRACE, "<%d>  mutex=%d lr=0x%08x...", th, mwq, lr);

	for(;;);

	if (thinkos_rt.lock[mutex] == -1) {
		thinkos_rt.lock[mutex] = th;
		DCC_LOG2(LOG_TRACE, "<%d> mutex %d locked", th, mwq);
		return;
	}

	/* insert into the mutex wait queue */
	__thinkos_wq_insert(mwq, th);
	DCC_LOG2(LOG_TRACE , "<%d> waiting on mutex %d...", th, mwq);

}
#endif
#endif


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
		DCC_LOG1(LOG_ERROR, "invalid mutex %d!", mwq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
	if (cond >= THINKOS_COND_MAX) {
		DCC_LOG1(LOG_ERROR, "invalid conditional variable %d!", cwq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_MUTEX_ALLOC
	if (__bit_mem_rd(thinkos_rt.mutex_alloc, mutex) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid mutex %d!", mwq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#if THINKOS_ENABLE_COND_ALLOC
	if (__bit_mem_rd(thinkos_rt.cond_alloc, cond) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid conditional variable %d!", cwq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	/* sanity check: avoid unlock the mutex by a thread that 
	   does not own the lock */
	if (thinkos_rt.lock[mutex] != self) {
		DCC_LOG3(LOG_WARNING, "<%d> mutex %d is locked by <%d>", 
				 self, mwq, thinkos_rt.lock[mutex]);
		arg[0] = THINKOS_EPERM;
		return;
	}

#if 0
	/* assign the mutex to be locked on wakeup */
	thinkos_rt.cond_mutex[cond] = mwq;
#endif
	/* insert into the cond wait queue */
	__thinkos_tmdwq_insert(cwq, self, ms);
	DCC_LOG3(LOG_INFO, "<%d> mutex %d unlocked, waiting on cond %d...", 
			 self, mwq, cwq);

	/* check for threads wating on the mutex wait queue */
	if ((th = __thinkos_wq_head(mwq)) == THINKOS_THREAD_NULL) {
		/* no threads waiting on the lock, just release
		   the lock */
		DCC_LOG2(LOG_INFO, "<%d> mutex %d released", self, mwq);
		thinkos_rt.lock[mutex] = -1;
	} else {
		/* set the mutex ownership to the new thread */
		thinkos_rt.lock[mutex] = th;
		DCC_LOG2(LOG_INFO, "<%d> mutex %d locked", th, mwq);
		/* wakeup from the mutex wait queue */
		__thinkos_wakeup(mwq, th);
	}

	/* Set the default return value to timeout. The
	   sem_post call will change this to 0 */
	arg[0] = THINKOS_ETIMEDOUT;

	/* wait for event */
	__thinkos_wait(self);
}
#endif

void thinkos_cond_signal_svc(int32_t * arg)
{	
	unsigned int cwq = arg[0];
	unsigned int cond = cwq - THINKOS_COND_BASE;
	int th;

#if THINKOS_ENABLE_ARG_CHECK
	if (cond >= THINKOS_COND_MAX) {
		DCC_LOG1(LOG_ERROR, "invalid conditional variable %d!", cwq);
		arg[0] = THINKOS_EINVAL;
		return;
	}

#if THINKOS_ENABLE_COND_ALLOC
	if (__bit_mem_rd(thinkos_rt.cond_alloc, cond) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid conditional variable %d!", cwq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	/* XXX: NEW experimental implementation:
	   the cond_wait() and cond_timedwait() user
	   calls invoque the mutex_lock() before returning */

	/* insert all remaining threads into mutex wait queue */
	if ((th = __thinkos_wq_head(cwq)) != THINKOS_THREAD_NULL) {
		DCC_LOG2(LOG_INFO, "<%d> wakeup from cond %d.", th, cwq);
		/* wakeup from the mutex wait queue */
		__thinkos_wakeup(cwq, th);
		/* signal the scheduler ... */
		__thinkos_defer_sched();
	}

#if 0
	unsigned int mwq;
	unsigned int mutex;
	if ((th = __thinkos_wq_head(cwq)) == THINKOS_THREAD_NULL) {
		/* no threads waiting on the conditional variable. */ 
	} else {
		DCC_LOG2(LOG_INFO, "<%d> wakeup from cond %d.", th, cwq);

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
			/* insert the thread into ready queue */
			__bit_mem_wr(&thinkos_rt.wq_ready, th, 1);

			/* signal the scheduler ... */
			__thinkos_defer_sched();
		} else { 
			/* insert into the mutex wait queue */
			__thinkos_wq_insert(mwq, th);
			DCC_LOG2(LOG_INFO, "<%d> waiting on mutex %d...", th, mwq);
		}
	}
#endif
	arg[0] = 0;
}

void thinkos_cond_broadcast_svc(int32_t * arg)
{	
	unsigned int cwq = arg[0];
	unsigned int cond = cwq - THINKOS_COND_BASE;
	int th;

#if THINKOS_ENABLE_ARG_CHECK
	if (cond >= THINKOS_COND_MAX) {
		DCC_LOG1(LOG_ERROR, "invalid conditional variable %d!", cwq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_COND_ALLOC
	if (__bit_mem_rd(thinkos_rt.cond_alloc, cond) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid conditional variable %d!", cwq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif
	/* XXX: NEW experimental implementation:
	   the cond_wait() and cond_timedwait() user
	   calls invoque the mutex_lock() before returning */

	/* insert all remaining threads into mutex wait queue */
	if ((th = __thinkos_wq_head(cwq)) != THINKOS_THREAD_NULL) {
		DCC_LOG2(LOG_INFO, "<%d> wakeup from cond %d.", th, cwq);
		/* wakeup from the mutex wait queue */
		__thinkos_wakeup(cwq, th);
		/* insert all remaining threads into mutex wait queue */
		while ((th = __thinkos_wq_head(cwq)) != THINKOS_THREAD_NULL) {
			DCC_LOG2(LOG_INFO, "<%d> wakeup from cond %d.", th, cwq);
			__thinkos_wakeup(cwq, th);
		}
		/* signal the scheduler ... */
		__thinkos_defer_sched();
	}

#if 0
	unsigned int mwq;
	unsigned int mutex;

		/* get the mutex associated with the conditional variable */
	if ((th = __thinkos_wq_head(cwq)) == THINKOS_THREAD_NULL) {
		/* no threads waiting on the conditional variable. */ 
	} else {
		DCC_LOG2(LOG_INFO, "<%d> wakeup from cond %d.", th, cwq);

		/* remove from the conditional variable wait queue */
		__thinkos_wq_remove(cwq, th);

		/* get the mutex associated with the conditional variable */
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
			/* insert the thread into ready queue */
			__bit_mem_wr(&thinkos_rt.wq_ready, th, 1);

			/* signal the scheduler ... */
			__thinkos_defer_sched();
		} else { 
			/* insert into the mutex wait queue */
			__thinkos_wq_insert(mwq, th);
			DCC_LOG2(LOG_INFO, "<%d> waiting on mutex %d...", th, mwq);
		}

		/* insert all remaining threads into mutex wait queue */
		while ((th = __thinkos_wq_head(cwq)) != THINKOS_THREAD_NULL) {
			/* remove from the conditional variable wait queue */
			__thinkos_wq_remove(cwq, th);
			/* insert into mutex wait queue */
			__thinkos_wq_insert(mwq, th);
			DCC_LOG2(LOG_INFO, "<%d> waiting on mutex %d...", th, mwq);
		}
	}
#endif
	arg[0] = 0;
}

#endif /* THINKOS_COND_MAX > 0 */

