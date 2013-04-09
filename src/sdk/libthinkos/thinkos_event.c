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
	unsigned int ev;

	ev = __thinkos_ev_alloc();

	DCC_LOG1(LOG_TRACE, "event=%d", ev);
	arg[0] = ev;
}

void thinkos_ev_free_svc(int32_t * arg)
{
	unsigned int ev = arg[0];

#if THINKOS_ENABLE_ARG_CHECK
	if ((ev < THINKOS_EVENT_BASE) || 
		(ev >= (THINKOS_EVENT_BASE + THINKOS_EVENT_MAX))) {
		DCC_LOG1(LOG_ERROR, "object %d is not an event!", ev);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif

	DCC_LOG1(LOG_TRACE, "event=%d", ev);
	__thinkos_ev_free(ev);
}
#endif


void thinkos_ev_wait_svc(int32_t * arg)
{
	unsigned int wq = arg[0];
	int self = thinkos_rt.active;

#if THINKOS_ENABLE_ARG_CHECK
	unsigned int ev = wq - THINKOS_EVENT_BASE;

	if (ev >= THINKOS_EVENT_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not an event!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_EVENT_ALLOC
	if (__bit_mem_rd(&thinkos_rt.ev_alloc, ev) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid event %d!", ev);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	cm3_cpsid_i();

	/* insert into the wait queue */
	__thinkos_wq_insert(wq, self);

	/* set the non schedule flag */
	__bit_mem_wr(&thinkos_rt.wq_ready, thinkos_rt.active, 0);  
#if THINKOS_ENABLE_TIMESHARE
	/* if the ready queue is empty, collect
	 the threads from the CPU wait queue */
	if (thinkos_rt.wq_ready == 0) {
		thinkos_rt.wq_ready = thinkos_rt.wq_tmshare;
		thinkos_rt.wq_tmshare = 0;
	}
#endif
	/* signal the scheduler ... */
	__thinkos_defer_sched();

	DCC_LOG2(LOG_TRACE, "<%d> waiting for event %d...", self, wq);

	cm3_cpsie_i();
}

void thinkos_ev_timedwait_svc(int32_t * arg)
{
	unsigned int wq = arg[0];
	uint32_t ms = (uint32_t)arg[1];
	int self = thinkos_rt.active;

#if THINKOS_ENABLE_ARG_CHECK
	unsigned int ev = wq - THINKOS_EVENT_BASE;

	if (ev >= THINKOS_EVENT_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not an event!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_EVENT_ALLOC
	if (__bit_mem_rd(&thinkos_rt.ev_alloc, ev) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid event %d!", ev);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	cm3_cpsid_i();

	/* insert into the mutex wait queue */
	__thinkos_tmdwq_insert(wq, self, ms);

	/* set the non schedule flag */
	__bit_mem_wr(&thinkos_rt.wq_ready, thinkos_rt.active, 0);  
#if THINKOS_ENABLE_TIMESHARE
	/* if the ready queue is empty, collect
	 the threads from the CPU wait queue */
	if (thinkos_rt.wq_ready == 0) {
		thinkos_rt.wq_ready = thinkos_rt.wq_tmshare;
		thinkos_rt.wq_tmshare = 0;
	}
#endif
	/* signal the scheduler ... */
	__thinkos_defer_sched();

	DCC_LOG2(LOG_TRACE, "<%d> waiting for event %d...", self, wq);

	/* Set the default return value to timeout. The
	   ev_rise() call will change it to 0 */
	arg[0] = THINKOS_ETIMEDOUT;

	cm3_cpsie_i();
}

void thinkos_ev_raise_svc(int32_t * arg)
{
	unsigned int wq = arg[0];
	int th;

#if THINKOS_ENABLE_ARG_CHECK
	unsigned int ev = wq - THINKOS_EVENT_BASE;

	if (ev >= THINKOS_EVENT_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not an event!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_EVENT_ALLOC
	if (__bit_mem_rd(&thinkos_rt.ev_alloc, ev) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid event %d!", ev);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	if ((th = __thinkos_wq_head(wq)) != THINKOS_THREAD_NULL) {
		/* wakeup from the event wait queue */
		__thinkos_wakeup(wq, th);
		DCC_LOG2(LOG_TRACE, "<%d> waked up with event %d", th, wq);
		/* signal the scheduler ... */
		__thinkos_defer_sched();
	}
}

#endif /* THINKOS_EVENT_MAX > 0 */

