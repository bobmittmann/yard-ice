/* 
 * thikos_gate.c
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
 * but WITHOUT ANY WARRANTY; without gateen the implied warranty of
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

#if THINKOS_GATE_MAX > 0

#if THINKOS_ENABLE_GATE_ALLOC

void thinkos_gate_alloc_svc(int32_t * arg)
{
	int idx;

	if ((idx = __thinkos_bmp_alloc(thinkos_rt.gate_alloc, 
								   THINKOS_GATE_MAX)) >= 0) {
		__bit_mem_wr(thinkos_rt.gate.sig, idx, 0);
		__bit_mem_wr(thinkos_rt.gate.lock, idx, 0);
		arg[0] = idx + THINKOS_GATE_BASE;
		DCC_LOG1(LOG_TRACE, "wq=%d", arg[0]);
	} else {
		arg[0] = idx;
	}
}

void thinkos_gate_free_svc(int32_t * arg)
{
	unsigned int wq = arg[0];

#if THINKOS_ENABLE_ARG_CHECK
	unsigned int idx = wq - THINKOS_GATE_BASE;

	if (idx >= THINKOS_GATE_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not a xdmon!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
	__bit_mem_wr(thinkos_rt.gate_alloc, wq - THINKOS_GATE_BASE, 0);
}

#endif


/* --------------------------------------------------------------------------
 * 
 * -------------------------------------------------------------------------- */

void thinkos_gate_wait_svc(int32_t * arg)
{
	unsigned int wq = arg[0];
	int self = thinkos_rt.active;
	unsigned int idx = wq - THINKOS_GATE_BASE;

#if THINKOS_ENABLE_ARG_CHECK
	if (idx >= THINKOS_GATE_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not a gate!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_GATE_ALLOC
	if (__bit_mem_rd(thinkos_rt.gate_alloc, idx) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid gate %d!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	if (__bit_mem_rd(thinkos_rt.gate.sig, idx)) {
		if (!__bit_mem_rd(thinkos_rt.gate.lock, idx)) {
			/* lock the gate */
			__bit_mem_wr(thinkos_rt.gate.lock, idx, 1);
			/* clear the signal */
			__bit_mem_wr(thinkos_rt.gate.sig, idx, 0);
			arg[0] = 0;
			return;
		}
	}

	/* -- wait for event ---------------------------------------- */
	DCC_LOG2(LOG_INFO, "<%d> waiting for gate %d...", self, wq);
	/* insert into the wait queue */
	__thinkos_wq_insert(wq, self);
	/* remove from the ready wait queue */
	__thinkos_suspend(self);
	/* XXX: save the context pointer */
	thinkos_rt.ctx[self] = (struct thinkos_context *)&arg[-8];
	__thinkos_defer_sched(); /* signal the scheduler ... */
}

#if THINKOS_ENABLE_TIMED_CALLS
void thinkos_gate_timedwait_svc(int32_t * arg)
{
	unsigned int wq = arg[0];
	uint32_t ms = (uint32_t)arg[1];
	int self = thinkos_rt.active;
	unsigned int idx = wq - THINKOS_GATE_BASE;

#if THINKOS_ENABLE_ARG_CHECK
	if (idx >= THINKOS_GATE_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not a gate!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_GATE_ALLOC
	if (__bit_mem_rd(thinkos_rt.gate_alloc, idx) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid gate %d!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	if (__bit_mem_rd(thinkos_rt.gate.sig, idx)) {
		if (!__bit_mem_rd(thinkos_rt.gate.lock, idx)) {
			/* lock the gate */
			__bit_mem_wr(thinkos_rt.gate.lock, idx, 1);
			/* clear the signal */
			__bit_mem_wr(thinkos_rt.gate.sig, idx, 0);
			arg[0] = 0;
			return;
		}
	}

	/* -- wait for event ---------------------------------------- */
	DCC_LOG2(LOG_INFO, "<%d> waiting for gate %d...", self, wq);

	/* Set the default return value to timeout. The
	   gate_rise() call will change it to 0 */
	arg[0] = THINKOS_ETIMEDOUT;
	/* insert into the gate wait queue */
	__thinkos_tmdwq_insert(wq, self, ms);
	/* remove from the ready wait queue */
	__thinkos_suspend(self);
	/* XXX: save the context pointer */
	thinkos_rt.ctx[self] = (struct thinkos_context *)&arg[-8];
	__thinkos_defer_sched(); /* signal the scheduler ... */
}
#endif

void thinkos_gate_exit_svc(int32_t * arg)
{

	unsigned int wq = arg[0];
	unsigned int sig = arg[1];
	unsigned int idx = wq - THINKOS_GATE_BASE;
	int th;

#if THINKOS_ENABLE_ARG_CHECK
	if (idx >= THINKOS_GATE_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not a gate!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_GATE_ALLOC
	if (__bit_mem_rd(thinkos_rt.gate_alloc, idx) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid gate %d!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	arg[0] = 0;

	/* unlock the gate */
	__bit_mem_wr(thinkos_rt.gate.lock, idx, 0);

	if (sig) {
		/* signal the gate acoording to the call argument */
		__bit_mem_wr(thinkos_rt.gate.sig, idx, 1);
	}

	if (!__bit_mem_rd(thinkos_rt.gate.sig, idx))
		return;

	if ((th = __thinkos_wq_head(wq)) == THINKOS_THREAD_NULL) {
		/* no pending threads, just return */
		return;
	}

	/* wakeup from the xdmon wait queue */
	__thinkos_wakeup(wq, th);
	DCC_LOG2(LOG_MSG, "<%d> waked up with xdmon %d", th, wq);
	/* clear the xdmon signal bit */
	__bit_mem_wr(thinkos_rt.gate.sig, idx, 0);  
	/* set the xdmon lock bit */
	__bit_mem_wr(thinkos_rt.gate.lock, idx, 1);  
	/* signal the scheduler ... */
	__thinkos_defer_sched();
}

/* void __thinkos_gate_open_i(uint32_t wq) */
void cm3_except13_isr(uint32_t wq)
{
	unsigned int gate = wq - THINKOS_GATE_BASE;
	int th;

	DCC_LOG1(LOG_INFO, "wq=%d...", wq);
	/* gate_signal() wakeup a single thread waiting on the gate 
	   OR set the gate */
	/* set the gate bit */
	__bit_mem_wr(thinkos_rt.gate.sig, gate, 1);  
	if (!__bit_mem_rd(thinkos_rt.gate.lock, gate)) {
		/* get a thread from the queue */
		if ((th = __thinkos_wq_head(wq)) != THINKOS_THREAD_NULL) {
			/* lock the gate */
			__bit_mem_wr(thinkos_rt.gate.lock, gate, 1);
			__thinkos_wakeup(wq, th);

			/* clear the gate bit */
			__bit_mem_wr(thinkos_rt.gate.sig, gate, 0);
			/* signal the scheduler ... */
			__thinkos_defer_sched();
		} 
	}
}

void __thinkos_gate_open_i(uint32_t wq) 
	__attribute__((weak, alias("cm3_except13_isr")));

void thinkos_gate_open_svc(int32_t * arg)
{
	unsigned int wq = arg[0];
	unsigned int idx = wq - THINKOS_GATE_BASE;

#if THINKOS_ENABLE_ARG_CHECK
	if (idx >= THINKOS_GATE_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not a gate!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_GATE_ALLOC
	if (__bit_mem_rd(thinkos_rt.gate_alloc, idx) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid gate %d!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif
	arg[0] = 0;

	__thinkos_gate_open_i(wq);
}


#endif /* THINKOS_GATE_MAX > 0 */

