/* 
 * thikos_xdmon.c
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
 * but WITHOUT ANY WARRANTY; without xdmonen the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */
#include <stdio.h>

#define __THINKOS_SYS__
#include <thinkos_sys.h>

#define __THINKOS_IRQ__
#include <thinkos_irq.h>

#include <thinkos.h>

#if THINKOS_XDMON_MAX > 0

#if THINKOS_ENABLE_XDMON_ALLOC

void thinkos_xdmon_alloc_svc(int32_t * arg)
{
	int idx;

	if ((idx = thinkos_alloc_lo(thinkos_rt.sem_alloc, 0)) >= 0) {
		__bit_mem_wr(thinkos_rt.xdmon_lock, idx, 0);
		__bit_mem_wr(thinkos_rt.xdmon_sig, idx, 0);
		thinkos_rt.sem_val[idx] = value;
		arg[0] = idx + THINKOS_XDMON_BASE;
		DCC_LOG2(LOG_TRACE, "wq=%d", arg[0]);
	} else {
		arg[0] = idx;
	}
}

void thinkos_xdmon_free_svc(int32_t * arg)
{
	unsigned int wq = arg[0];
	unsigned int idx = wq - THINKOS_XDMON_BASE;

#if THINKOS_ENABLE_ARG_CHECK
	if (idx >= THINKOS_XDMON_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not a xdmon!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
	__bit_mem_wr(thinkos_rt.flag_alloc, wq - THINKOS_XDMON_BASE, 0);
}

#endif

void thinkos_xdmon_wait_svc(int32_t * arg)
{
	unsigned int wq = arg[0];
	int self = thinkos_rt.active;
	unsigned int idx = wq - THINKOS_XDMON_BASE;

#if THINKOS_ENABLE_ARG_CHECK
	if (idx >= THINKOS_XDMON_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not a xdmon!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_XDMON_ALLOC
	if (__bit_mem_rd(thinkos_rt.xdmon_alloc, idx) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid xdmon %d!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	arg[0] = 0;

	cm3_cpsid_i();
	if (!__bit_mem_rd(thinkos_rt.xdmon_lock, idx) && 
		__bit_mem_rd(thinkos_rt.xdmon_sig, idx)) {
		__bit_mem_wr(thinkos_rt.xdmon_lock, idx, 1);
		__bit_mem_wr(thinkos_rt.xdmon_sig, idx, 0);
		cm3_cpsie_i();
		return;
	}

	/* insert into the wait queue */
	__thinkos_wq_insert(wq, self);
	DCC_LOG2(LOG_INFO, "<%d> waiting for xdmon %d...", self, wq);

	/* wait for event */

	/* remove from the ready wait queue */
	__bit_mem_wr(&thinkos_rt.wq_ready, self, 0);  

#if THINKOS_ENABLE_TIMESHARE
	/* if the ready queue is empty, collect
	 the threads from the CPU wait queue */
#if (THINKOS_THREADS_MAX < 32) 
	if (thinkos_rt.wq_ready == (1 << THINKOS_THREADS_MAX)) {
		/* No more threads into the ready queue,
		 move the timeshare queue to the ready queue.
		 Keep the IDLE bit set */
		thinkos_rt.wq_ready |= thinkos_rt.wq_tmshare;
		thinkos_rt.wq_tmshare = 0;
	} 
#else
	if (thinkos_rt.wq_ready == 0) {
		/* no more threads into the ready queue,
		 move the timeshare queue to the ready queue */
		thinkos_rt.wq_ready = thinkos_rt.wq_tmshare;
		thinkos_rt.wq_tmshare = 0;
	} 
#endif
#endif
	cm3_cpsie_i();

	/* signal the scheduler ... */
	__thinkos_defer_sched();
}

#if THINKOS_ENABLE_TIMED_CALLS
void thinkos_xdmon_timedwait_svc(int32_t * arg)
{
	unsigned int wq = arg[0];
	uint32_t ms = (uint32_t)arg[1];
	int self = thinkos_rt.active;

#if THINKOS_ENABLE_ARG_CHECK
	unsigned int xdmon = wq - THINKOS_XDMON_BASE;

	if (xdmon >= THINKOS_XDMON_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not a xdmon!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_XDMON_ALLOC
	if (__bit_mem_rd(&thinkos_rt.xdmon_alloc, xdmon) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid xdmon %d!", xdmon);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	cm3_cpsid_i();
	if (!__bit_mem_rd(thinkos_rt.xdmon_lock, idx) && 
		__bit_mem_rd(thinkos_rt.xdmon_sig, idx)) {
		__bit_mem_wr(thinkos_rt.xdmon_lock, idx, 1);
		__bit_mem_wr(thinkos_rt.xdmon_sig, idx, 0);
		cm3_cpsie_i();
		return;
	}

	/* insert into the xdmon wait queue */
	__thinkos_tmdwq_insert(wq, self, ms);

	DCC_LOG2(LOG_INFO, "<%d> waiting for xdmon %d...", self, wq);

	/* Set the default return value to timeout. The
	   xdmon_rise() call will change it to 0 */
	arg[0] = THINKOS_ETIMEDOUT;

	/* wait for event */

	/* remove from the ready wait queue */
	__bit_mem_wr(&thinkos_rt.wq_ready, self, 0);  

#if THINKOS_ENABLE_TIMESHARE
	/* if the ready queue is empty, collect
	 the threads from the CPU wait queue */
#if (THINKOS_THREADS_MAX < 32) 
	if (thinkos_rt.wq_ready == (1 << THINKOS_THREADS_MAX)) {
		/* No more threads into the ready queue,
		 move the timeshare queue to the ready queue.
		 Keep the IDLE bit set */
		thinkos_rt.wq_ready |= thinkos_rt.wq_tmshare;
		thinkos_rt.wq_tmshare = 0;
	} 
#else
	if (thinkos_rt.wq_ready == 0) {
		/* no more threads into the ready queue,
		 move the timeshare queue to the ready queue */
		thinkos_rt.wq_ready = thinkos_rt.wq_tmshare;
		thinkos_rt.wq_tmshare = 0;
	} 
#endif
#endif
	cm3_cpsie_i();
	/* signal the scheduler ... */
	__thinkos_defer_sched();
}
#endif

void thinkos_xdmon_signal_svc(int32_t * arg)
{
	unsigned int wq = arg[0];
	unsigned int xdmon = wq - THINKOS_XDMON_BASE;
	int th;

#if THINKOS_ENABLE_ARG_CHECK
	if (xdmon >= THINKOS_XDMON_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not a xdmon!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_XDMON_ALLOC
	if (__bit_mem_rd(&thinkos_rt.xdmon_alloc, xdmon) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid xdmon %d!", xdmon);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	arg[0] = 0;

	cm3_cpsid_i();
	if (__bit_mem_rd(thinkos_rt.xdmon_lock, idx)) {
		__bit_mem_wr(thinkos_rt.xdmon_sig, idx, 1);
		cm3_cpsie_i();
		DCC_LOG1(LOG_INFO, "xdmon %d is locked!", wq);
		return;
	}

	if ((th = __thinkos_wq_head(wq)) != THINKOS_THREAD_NULL) {
		/* wakeup from the xdmon wait queue */
		__thinkos_wakeup(wq, th);
		DCC_LOG2(LOG_MSG, "<%d> waked up with xdmon %d", th, wq);
		/* clear the xdmon signal bit */
		__bit_mem_wr(thinkos_rt.xdmon_sig, idx, 0);  
		/* set the xdmon lock bit */
		__bit_mem_wr(thinkos_rt.xdmon_lock, idx, 1);  
		cm3_cpsie_i();
		/* signal the scheduler ... */
		__thinkos_defer_sched();
	} else {
		cm3_cpsie_i();
		/* set the xdmon signal bit */
		__bit_mem_wr(thinkos_rt.xdmon_sig, idx, 1);  
	}
}


void thinkos_xdmon_unlock_svc(int32_t * arg)
{
	unsigned int wq = arg[0];
	unsigned int sig = arg[1];
	unsigned int xdmon = wq - THINKOS_XDMON_BASE;
	int th;

#if THINKOS_ENABLE_ARG_CHECK
	if (xdmon >= THINKOS_XDMON_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not a xdmon!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_XDMON_ALLOC
	if (__bit_mem_rd(&thinkos_rt.xdmon_alloc, xdmon) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid xdmon %d!", xdmon);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	arg[0] = 0;

	cm3_cpsid_i();

	/* unlock */
	__bit_mem_wr(thinkos_rt.xdmon_lock, idx, 0);

	if (sig)
		__bit_mem_wr(thinkos_rt.xdmon_sig, idx, 1);

	if (!__bit_mem_rd(thinkos_rt.xdmon_sig, idx)) {
		cm3_cpsie_i();
		DCC_LOG1(LOG_INFO, "xdmon %d is not signaled !", wq);
		return;
	}

	cm3_cpsie_i();

	if ((th = __thinkos_wq_head(wq)) != THINKOS_THREAD_NULL) {
		/* wakeup from the xdmon wait queue */
		__thinkos_wakeup(wq, th);
		DCC_LOG2(LOG_MSG, "<%d> waked up with xdmon %d", th, wq);
		/* clear the xdmon signal bit */
		__bit_mem_wr(thinkos_rt.xdmon_sig, idx, 0);  
		/* set the xdmon lock bit */
		__bit_mem_wr(thinkos_rt.xdmon_lock, idx, 1);  
		/* signal the scheduler ... */
		__thinkos_defer_sched();
	}
}

#endif /* THINKOS_XDMON_MAX > 0 */

