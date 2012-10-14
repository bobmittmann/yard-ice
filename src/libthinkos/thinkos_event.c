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

#define __THINKOS_SYS__
#include <thinkos_sys.h>

#define __THINKOS_IRQ__
#include <thinkos_irq.h>

#include <thinkos.h>

#if THINKOS_EVENT_MAX > 0

#if THINKOS_ENABLE_EVENT_ALLOC
void thinkos_ev_alloc_svc(int32_t * arg)
{
	arg[0] = __thinkos_ev_alloc();
}

void thinkos_ev_free_svc(int32_t * arg)
{
	unsigned int ev = arg[0];
#if THINKOS_ENABLE_ARG_CHECK
	if (ev >= THINKOS_EVENT_MAX) {
		DCC_LOG1(LOG_ERROR, "invalid event %d!", ev);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
	__thinkos_ev_free(ev);
}
#endif

void thinkos_ev_wait_svc(int32_t * arg)
{
	unsigned int ev = arg[0];
	int self = thinkos_rt.active;

#if THINKOS_ENABLE_ARG_CHECK
	if (ev >= THINKOS_EVENT_MAX) {
		DCC_LOG1(LOG_ERROR, "invalid event %d!", ev);
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

	/* insert into the ev wait queue */
	bmp_bit_set(&thinkos_rt.wq_event[ev], self);  
#if THINKOS_ENABLE_THREAD_STAT
	/* update status */
	thinkos_rt.th_stat[self] = __wq_idx(&thinkos_rt.wq_event[ev]) << 1;
#endif
	DCC_LOG2(LOG_TRACE, "<%d> wait for event %d ", 
			 thinkos_rt.active, ev);

	/* wait for event */
	__thinkos_wait();
}

void thinkos_ev_timedwait_svc(int32_t * arg)
{
	unsigned int ev = arg[0];
	uint32_t ms = (uint32_t)arg[1];
	int self = thinkos_rt.active;

#if THINKOS_ENABLE_ARG_CHECK
	if (ev >= THINKOS_EVENT_MAX) {
		DCC_LOG1(LOG_ERROR, "invalid event %d!", ev);
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

	/* insert into the ev wait queue */
	bmp_bit_set(&thinkos_rt.wq_event[ev], self);  
#if THINKOS_ENABLE_THREAD_STAT
	/* update status */
	thinkos_rt.th_stat[self] = __wq_idx(&thinkos_rt.wq_event[ev]) << 1;
#endif
	/* insert into the event wait queue */
	bmp_bit_set(&thinkos_rt.wq_event[ev], self);  
	/* set the clock */
	thinkos_rt.clock[self] = thinkos_rt.ticks + (ms / 1);
	/* insert into the clock wait queue */
	bmp_bit_set(&thinkos_rt.wq_clock, self);  
#if THINKOS_ENABLE_THREAD_STAT
	/* update status, mark the thread clock enable bit */
	thinkos_rt.th_stat[self] = (__wq_idx(&thinkos_rt.wq_event[ev]) << 1) + 1;
#endif
	/* Set the default return value to timeout. The
	   ev_rise() call will change self to 0 */
	arg[0] = THINKOS_ETIMEDOUT;

	DCC_LOG2(LOG_TRACE, "<%d> wait for event %d ", thinkos_rt.active, ev);
	/* wait for event */
	__thinkos_wait();
}

void thinkos_ev_raise_svc(int32_t * arg)
{
	unsigned int ev = arg[0];
	int idx;

#if THINKOS_ENABLE_ARG_CHECK
	if (ev >= THINKOS_EVENT_MAX) {
		DCC_LOG1(LOG_ERROR, "invalid event %d!", ev);
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

	idx = __thinkos_wq_head(&thinkos_rt.wq_event[ev]);
	if (idx != THINKOS_IDX_NULL) {
		/* remove from the ev wait queue */
		bmp_bit_clr(&thinkos_rt.wq_event[ev], idx);  
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
		DCC_LOG2(LOG_TRACE, "<%d> event %d ", idx, ev);
		/* signal the scheduler ... */
		__thinkos_defer_sched();
	}
}

#endif /* THINKOS_EVENT_MAX > 0 */

