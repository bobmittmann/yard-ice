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

_Pragma ("GCC optimize (\"Ofast\")")

#define __THINKOS_SYS__
#include <thinkos_sys.h>
#include <thinkos.h>

#if THINKOS_EVENT_MAX > 0

#if THINKOS_ENABLE_EVENT_ALLOC
void thinkos_ev_alloc_svc(int32_t * arg)
{
	unsigned int wq;
	int idx;

	if ((idx = __thinkos_bmp_alloc(thinkos_rt.ev_alloc, 
								   THINKOS_EVENT_MAX)) >= 0) {
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
	uint32_t mask;
	uint32_t pend;
	uint32_t queue;

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

	mask = thinkos_rt.ev[no].mask;
again:
	/* check for any pending unmasked event */
	pend = __ldrexw(&thinkos_rt.ev[no].pend);
	if ((ev = __clz(__rbit(pend & mask))) < 32) {
		pend &= ~(1 << ev);
		arg[0] = ev;
		DCC_LOG2(LOG_MSG, "set=0x%08x msk=0x%08x", 
				 thinkos_rt.ev[no].pend, thinkos_rt.ev[no].mask);
		DCC_LOG2(LOG_INFO, "pending event %d.%d!", wq, ev);
		if (__strexw(&thinkos_rt.ev[no].pend, pend))
			goto again;
		return;
	}

	/* (1) - suspend the thread by removing it from the
	   ready wait queue. The __thinkos_suspend() call cannot be nested
	   inside a LDREX/STREX pair as it may use the exclusive access itself,
	   in case we have anabled the time sharing option.
	   It is not a problem having a thread not contained in any waiting
	   queue inside a system call. 
	 */
	__thinkos_suspend(self);
	/* update the thread status in preparation for event wait */
#if THINKOS_ENABLE_THREAD_STAT
	thinkos_rt.th_stat[self] = wq << 1;
#endif

	/* insert into the event wait queue */
	queue = __ldrexw(&thinkos_rt.wq_lst[wq]);

	/* The event set may have been signaled while suspending (1).
	 If this is the case roll back and restart. */
	pend = (volatile uint32_t)thinkos_rt.ev[no].pend;
	if ((ev = __clz(__rbit(pend & mask))) < 32) {
		/* roll back */
#if THINKOS_ENABLE_THREAD_STAT
		thinkos_rt.th_stat[self] = 0;
#endif
		/* insert into the ready wait queue */
		__bit_mem_wr(&thinkos_rt.wq_ready, self, 1);  
		DCC_LOG2(LOG_WARNING, "<%d> rollback 1 %d...", self, wq);
		goto again;
	}

	/* insert into the event wait queue */
	queue |= (1 << self);
	if (__strexw(&thinkos_rt.wq_lst[wq], queue)) {
		/* roll back */
#if THINKOS_ENABLE_THREAD_STAT
		thinkos_rt.th_stat[self] = 0;
#endif
		/* insert into the ready wait queue */
		__bit_mem_wr(&thinkos_rt.wq_ready, self, 1);  
		DCC_LOG2(LOG_WARNING, "<%d> rollback 2 %d...", self, wq);
		goto again;
	}

	/* -- wait for event ---------------------------------------- */
	DCC_LOG2(LOG_INFO, "<%d> waiting for event on %d", self, wq);
	DCC_LOG3(LOG_INFO, "<%d> sp=%08x ctx=%p", 
			 self, cm3_psp_get(), thinkos_rt.ctx[self]);
	DCC_LOG3(LOG_INFO, "<%d> ctx=%p pc=%p", 
			 self, thinkos_rt.ctx[self], arg[6]);
	arg[0] = THINKOS_EFAULT;
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
	uint32_t mask;
	uint32_t pend;
	uint32_t queue;

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

	mask = thinkos_rt.ev[no].mask;
again:
	/* check for any pending unmasked event */
	pend = __ldrexw(&thinkos_rt.ev[no].pend);
	if ((ev = __clz(__rbit(pend & mask))) < 32) {
		pend &= ~(1 << ev);
		arg[0] = ev;
		DCC_LOG2(LOG_MSG, "set=0x%08x msk=0x%08x", 
				 thinkos_rt.ev[no].pend, thinkos_rt.ev[no].mask);
		DCC_LOG2(LOG_INFO, "pending event %d.%d!", wq, ev);
		if (__strexw(&thinkos_rt.ev[no].pend, pend))
			goto again;
		return;
	}

	__thinkos_suspend(self);
#if THINKOS_ENABLE_THREAD_STAT
	/* update status, mark the thread clock enable bit */
	thinkos_rt.th_stat[self] = (wq << 1) + 1;
#endif
	queue = __ldrexw(&thinkos_rt.wq_lst[wq]);
	queue |= (1 << self);
	pend = (volatile uint32_t)thinkos_rt.ev[no].pend;
	if (((ev = __clz(__rbit(pend & mask))) < 32) || 
		(__strexw(&thinkos_rt.wq_lst[wq], queue))) {
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
	/* set the clock */
	thinkos_rt.clock[self] = thinkos_rt.ticks + ms;
	/* insert into the clock wait queue */
	__bit_mem_wr(&thinkos_rt.wq_clock, self, 1);  
	/* Set the default return value to timeout. The
	   event_rise call will change this to 0 */
	arg[0] = THINKOS_ETIMEDOUT;
	/* signal the scheduler ... */
	__thinkos_defer_sched(); 
}
#endif

void cm3_except9_isr(uint32_t wq, int ev)
//void __thinkos_ev_raise_i(uint32_t wq, int ev)
{
	unsigned int no = wq - THINKOS_EVENT_BASE;
	uint32_t queue;
	int th;

	if (__bit_mem_rd(&thinkos_rt.ev[no].mask, ev)) {
		DCC_LOG2(LOG_INFO, "pending ev=%d.%d", wq, ev);
		/* event is masked, set the event as pending */
		__bit_mem_wr(&thinkos_rt.ev[no].pend, ev, 1);  
		return;
	}

	do {
		/* insert into the event wait queue */
		queue = __ldrexw(&thinkos_rt.wq_lst[wq]);
		/* get a thread from the queue bitmap */
		if ((th = __clz(__rbit(queue))) == THINKOS_THREAD_NULL) {
			/* no threads waiting on the event set, mark the event as pending */
			__bit_mem_wr(&thinkos_rt.ev[no].pend, ev, 1);  
			return;
		} 
		/* remove from the wait queue */
		queue &= ~(1 << th);
	} while (__strexw(&thinkos_rt.wq_lst[wq], queue));

	/* insert the thread into ready queue */
	__bit_mem_wr(&thinkos_rt.wq_ready, th, 1);
#if THINKOS_ENABLE_TIMED_CALLS
	/* possibly remove from the time wait queue */
	__bit_mem_wr(&thinkos_rt.wq_clock, th, 0);  
#endif
	/* set the thread's return value */
	thinkos_rt.ctx[th]->r0 = ev;
#if THINKOS_ENABLE_THREAD_STAT
	/* update status */
	thinkos_rt.th_stat[th] = 0;
#endif
	/* signal the scheduler ... */
	__thinkos_defer_sched();
}

void __thinkos_ev_raise_i(uint32_t wq, int ev)
	__attribute__((weak, alias("cm3_except9_isr")));

void thinkos_ev_raise_svc(int32_t * arg)
{
	unsigned int wq = arg[0];
	unsigned int ev = arg[1];
	unsigned int no = wq - THINKOS_EVENT_BASE;

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

	arg[0] = 0;
	__thinkos_ev_raise_i(wq, ev);
}

void thinkos_ev_mask_svc(int32_t * arg)
{
	unsigned int wq = arg[0];
	unsigned int ev = arg[1];
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

	arg[0] = 0;
	/* mask the events on the mask bitmap */
	__bit_mem_wr(&thinkos_rt.ev[no].mask, ev, 0);  
}

void thinkos_ev_unmask_svc(int32_t * arg)
{
	unsigned int wq = arg[0];
	unsigned int ev = arg[1];
	unsigned int no = wq - THINKOS_EVENT_BASE;
	uint32_t queue;
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

	arg[0] = 0;
	/* unmask the events on the mask bitmap */
	__bit_mem_wr(&thinkos_rt.ev[no].mask, ev, 1);  

	if (__bit_mem_rd(&thinkos_rt.ev[no].pend, ev)) {
		/* event is not pneding, return */
		return;
	}

	do {
		/* insert into the event wait queue */
		queue = __ldrexw(&thinkos_rt.wq_lst[wq]);
		/* get a thread from the queue bitmap */
		if ((th = __clz(__rbit(queue))) == THINKOS_THREAD_NULL) {
			/* no threads waiting on the event set, mark the event as pending */
			__bit_mem_wr(&thinkos_rt.ev[no].pend, ev, 1);  
			return;
		} 
		/* remove from the wait queue */
		queue &= ~(1 << th);
	} while (__strexw(&thinkos_rt.wq_lst[wq], queue));

	/* insert the thread into ready queue */
	__bit_mem_wr(&thinkos_rt.wq_ready, th, 1);
#if THINKOS_ENABLE_TIMED_CALLS
	/* possibly remove from the time wait queue */
	__bit_mem_wr(&thinkos_rt.wq_clock, th, 0);  
#endif
	/* set the thread's return value */
	thinkos_rt.ctx[th]->r0 = ev;
#if THINKOS_ENABLE_THREAD_STAT
	/* update status */
	thinkos_rt.th_stat[th] = 0;
#endif
	/* signal the scheduler ... */
	__thinkos_defer_sched();


}


#endif /* THINKOS_EVENT_MAX > 0 */

