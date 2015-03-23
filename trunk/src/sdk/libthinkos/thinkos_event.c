/* 
 * thikos_event.c
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
#include <stdio.h>

#define __THINKOS_SYS__
#include <thinkos_sys.h>

#define __THINKOS_IRQ__
#include <thinkos_irq.h>

#include <thinkos.h>

#if THINKOS_EVENT_MAX > 0

#if THINKOS_ENABLE_EVENT_ALLOC
void thinkos_ev_alloc_svc(int32_t * arg)
{
	unsigned int wq;
	int idx;

	if ((idx = thinkos_alloc_lo(thinkos_rt.ev_alloc, 0)) >= 0) {
		thinkos_rt.ev[idx].mask = 0xffffffff;
		thinkos_rt.ev[idx].pend = 0;
		wq = idx + THINKOS_EVENT_BASE;
		DCC_LOG2(LOG_MSG, "event set=%d wq=%d", idx, wq);
		arg[0] = wq;
	} else 
		arg[0] = idx;
}

void thinkos_ev_free_svc(int32_t * arg)
{
	unsigned int wq = arg[0];

#if THINKOS_ENABLE_ARG_CHECK
	if ((wq < THINKOS_EVENT_BASE) || 
		(wq >= (THINKOS_EVENT_BASE + THINKOS_EVENT_MAX))) {
		DCC_LOG1(LOG_ERROR, "object %d is not an event set!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif

	DCC_LOG1(LOG_MSG, "event wq=%d", wq);
	__bit_mem_wr(&thinkos_rt.ev_alloc, wq - THINKOS_EVENT_BASE, 0);
}
#endif

void thinkos_ev_wait_svc(int32_t * arg)
{
	unsigned int wq = arg[0];
	unsigned int no = wq - THINKOS_EVENT_BASE;
	int self = thinkos_rt.active;
	unsigned int ev;

#if THINKOS_ENABLE_ARG_CHECK

	if (no >= THINKOS_EVENT_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not an event set!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_EVENT_ALLOC
	if (__bit_mem_rd(&thinkos_rt.ev_alloc, no) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid event set %d!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	cm3_cpsid_i();

	/* check for any pending unmasked event */
	if ((ev = __clz(__rbit(thinkos_rt.ev[no].pend & 
						   thinkos_rt.ev[no].mask))) < 32) {
		DCC_LOG2(LOG_MSG, "set=0x%08x msk=0x%08x", 
				 thinkos_rt.ev[no].pend, thinkos_rt.ev[no].mask);
		__bit_mem_wr(&thinkos_rt.ev[no].pend, ev, 0);  
		DCC_LOG2(LOG_INFO, "pending event %d.%d!", wq, ev);
		arg[0] = ev;
		cm3_cpsie_i();
		return;
	} 

	/* insert into the wait queue */
	__thinkos_wq_insert(wq, self);

	/* wait for event */
	/* remove from the ready wait queue */
	__bit_mem_wr(&thinkos_rt.wq_ready, thinkos_rt.active, 0);  
#if THINKOS_ENABLE_TIMESHARE
	/* if the ready queue is empty, collect
	 the threads from the CPU wait queue */
	if (thinkos_rt.wq_ready == 0) {
		thinkos_rt.wq_ready = thinkos_rt.wq_tmshare;
		thinkos_rt.wq_tmshare = 0;
	}
#endif

	cm3_cpsie_i();

	DCC_LOG2(LOG_INFO, "<%d> waiting for event %d.xx ...", self, wq);

	/* signal the scheduler ... */
	__thinkos_defer_sched();

}

#if THINKOS_ENABLE_TIMED_CALLS
void thinkos_ev_timedwait_svc(int32_t * arg)
{
	unsigned int wq = arg[0];
	uint32_t ms = (uint32_t)arg[1];
	unsigned int no = wq - THINKOS_EVENT_BASE;
	int self = thinkos_rt.active;
	unsigned int ev;

#if THINKOS_ENABLE_ARG_CHECK

	if (no >= THINKOS_EVENT_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not an event set!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_EVENT_ALLOC
	if (__bit_mem_rd(&thinkos_rt.ev_alloc, no) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid event set %d!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	cm3_cpsid_i();

	/* check for any pending unmasked event */
	if ((ev = __clz(__rbit(thinkos_rt.ev[no].pend & 
						   thinkos_rt.ev[no].mask))) < 32) {
		__bit_mem_wr(&thinkos_rt.ev[no].pend, ev, 0);  
		arg[0] = ev;
		cm3_cpsie_i();
		return;
	} 

	/* insert into the mutex wait queue */
	__thinkos_tmdwq_insert(wq, self, ms);

	/* wait for event */
	/* remove from the ready wait queue */
	__bit_mem_wr(&thinkos_rt.wq_ready, thinkos_rt.active, 0);  
#if THINKOS_ENABLE_TIMESHARE
	/* if the ready queue is empty, collect
	 the threads from the CPU wait queue */
	if (thinkos_rt.wq_ready == 0) {
		thinkos_rt.wq_ready = thinkos_rt.wq_tmshare;
		thinkos_rt.wq_tmshare = 0;
	}
#endif
	/* Set the default return value to timeout. The
	   ev_rise() call will change it to the active event */
	arg[0] = THINKOS_ETIMEDOUT;

	cm3_cpsie_i();

	DCC_LOG2(LOG_INFO, "<%d> waiting for event %d...", self, wq);

	/* signal the scheduler ... */
	__thinkos_defer_sched();

}
#endif

void thinkos_ev_raise_svc(int32_t * arg)
{
	unsigned int wq = arg[0];
	unsigned int ev = arg[1];
	unsigned int no = wq - THINKOS_EVENT_BASE;
	int th;

#if THINKOS_ENABLE_ARG_CHECK
	if (ev > 31) {
		DCC_LOG1(LOG_ERROR, "event %d is invalid!", ev);
		arg[0] = THINKOS_EINVAL;
		return;
	}
	if (no >= THINKOS_EVENT_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not an event set!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_EVENT_ALLOC
	if (__bit_mem_rd(&thinkos_rt.ev_alloc, no) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid event set %d!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	if ((__bit_mem_rd(&thinkos_rt.ev[no].mask, ev)) &&  
		((th = __thinkos_wq_head(wq)) != THINKOS_THREAD_NULL)) {
		/* wakeup from the event wait queue, set the return of
		 the thread to the event */
		__thinkos_wakeup_return(wq, th, ev);
		DCC_LOG3(LOG_INFO, "<%d> waked up with event %d.%d", th, wq, ev);
		/* signal the scheduler ... */
		__thinkos_defer_sched();
	} else {
		/* event is maksed or no thread is waiting ont hte event set
		   , set the event as pending */
		__bit_mem_wr(&thinkos_rt.ev[no].pend, ev, 1);  
		DCC_LOG2(LOG_INFO, "event %d.%d pendig...", wq, ev);
		DCC_LOG2(LOG_MSG, "set=0x%08x msk=0x%08x", 
				 thinkos_rt.ev[no].pend, thinkos_rt.ev[no].mask);
	}
}

void thinkos_ev_mask_svc(int32_t * arg)
{
	unsigned int wq = arg[0];
	uint32_t mask = arg[1];
	unsigned int no = wq - THINKOS_EVENT_BASE;

#if THINKOS_ENABLE_ARG_CHECK
	if (no >= THINKOS_EVENT_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not an event set!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_EVENT_ALLOC
	if (__bit_mem_rd(&thinkos_rt.ev_alloc, no) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid event set %d!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	/* mask the events on the mask bitmap */
	thinkos_rt.ev[no].mask &= ~mask;
}

void thinkos_ev_unmask_svc(int32_t * arg)
{
	unsigned int wq = arg[0];
	uint32_t mask = arg[1];
	unsigned int no = wq - THINKOS_EVENT_BASE;
	unsigned int ev;
	int th;

#if THINKOS_ENABLE_ARG_CHECK
	if (no >= THINKOS_EVENT_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not an event set!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_EVENT_ALLOC
	if (__bit_mem_rd(&thinkos_rt.ev_alloc, no) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid event set %d!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	cm3_cpsid_i();

	/* unmask the events on the mask bitmap */
	thinkos_rt.ev[no].mask |= mask;

	/* wake up the first unmasked thread if any. */
	if ((ev = __clz(__rbit(thinkos_rt.ev[no].pend & mask))) < 32) {
		if ((th = __thinkos_wq_head(wq)) != THINKOS_THREAD_NULL) {
			/* a pending event was unmaksed and there is a thread waiting on 
			   the queue, clear the event pending flag and 
			   wakes up the thread. */
			__bit_mem_wr(&thinkos_rt.ev[no].pend, ev, 0);  
			/* wakeup from the event wait queue, set the return of
			   the thread to the event */
			__thinkos_wakeup_return(wq, th, ev);
			DCC_LOG3(LOG_TRACE, "<%d> waked up with event %d.%d", th, wq, ev);
			/* signal the scheduler ... */
			__thinkos_defer_sched();
		} else {
			/* no threads waiting */
			cm3_cpsie_i();
			return;
		}
	}

	/* wake up as many other threads as possible */
	while ((ev = __clz(__rbit(thinkos_rt.ev[no].pend & mask))) < 32) {
		if ((th = __thinkos_wq_head(wq)) != THINKOS_THREAD_NULL) {
			/* a pending event was unmaksed and there is a thread waiting on 
			   the queue, clear the event pending flag and 
			   wakes up the thread. */
			__bit_mem_wr(&thinkos_rt.ev[no].pend, ev, 0);  
			/* wakeup from the event wait queue, set the return of
			   the thread to the event */
			__thinkos_wakeup_return(wq, th, ev);
			DCC_LOG3(LOG_TRACE, "<%d> waked up with event %d.%d", th, wq, ev);
		} else {
			/* no more threads waiting */
			break;
		}
	}

	cm3_cpsie_i();
}


#endif /* THINKOS_EVENT_MAX > 0 */

