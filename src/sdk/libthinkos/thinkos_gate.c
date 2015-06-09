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
		__bit_mem_wr(thinkos_rt.gate, idx * 2, 0);
		__bit_mem_wr(thinkos_rt.gate, idx * 2 + 1, 0);
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

#define THINKOS_GATE_CLOSED   0
#define THINKOS_GATE_SIGNALED 1
#define THINKOS_GATE_LOCKED   2

void thinkos_gate_wait_svc(int32_t * arg)
{
	unsigned int wq = arg[0];
	int self = thinkos_rt.active;
	unsigned int idx = wq - THINKOS_GATE_BASE;
	uint32_t * gates_bmp;
	uint32_t gates;
	uint32_t queue;

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

#if THINKOS_FLAG_MAX < 16
	gates_bmp = &thinkos_rt.gate[0];
#else
	gates_bmp = &thinkos_rt.gate[idx / 16];
	idx %= 16;
#endif
	idx *= 2;

	DCC_LOG1(LOG_MSG, "gate %d", wq);

again:
	/* check whether the gate is open or not */
	gates = __ldrex(gates_bmp);
	if (((gates >> idx) & 3) == THINKOS_GATE_SIGNALED) {
		/* close and lock the gate */
		gates = (gates & ~(3 << idx)) | (THINKOS_GATE_LOCKED << idx);
		if (__strex(gates_bmp, gates))
			goto again;
		DCC_LOG2(LOG_INFO, "<%d> enter gate %d.", self, wq);
		arg[0] = 0;
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
	/* insert into the flag wait queue */
	queue = __ldrex(&thinkos_rt.wq_lst[wq]);
	queue |= (1 << self);
	/* The gate may have been signaled while suspending (1).
	 If this is the case roll back and restart. */
	if ((((volatile uint32_t)*gates_bmp >> idx) & 3) == THINKOS_GATE_OPEN ||
		__strex(&thinkos_rt.wq_lst[wq], queue)) {
		/* roll back */
#if THINKOS_ENABLE_THREAD_STAT
		thinkos_rt.th_stat[self] = 0;
#endif
		/* insert into the ready wait queue */
		__bit_mem_wr(&thinkos_rt.wq_ready, self, 1);  
		goto again;
	}

	/* -- wait for event ---------------------------------------- */
	DCC_LOG2(LOG_INFO, "<%d> waiting at gate %d...", self, wq);
	/* signal the scheduler ... */
	__thinkos_defer_sched(); 
}

#if THINKOS_ENABLE_TIMED_CALLS
void thinkos_gate_timedwait_svc(int32_t * arg)
{
	unsigned int wq = arg[0];
	uint32_t ms = (uint32_t)arg[1];
	int self = thinkos_rt.active;
	unsigned int idx = wq - THINKOS_GATE_BASE;
	uint32_t * gates_bmp;
	uint32_t gates;
	uint32_t queue;

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

#if THINKOS_FLAG_MAX < 16
	gates_bmp = &thinkos_rt.gate[0];
#else
	gates_bmp = &thinkos_rt.gate[idx / 16];
	idx %= 16;
#endif
	idx *= 2;

again:
	/* check whether the gate is open or not */
	gates = __ldrex(gates_bmp);
	if (((gates >> idx) & 3) == THINKOS_GATE_SIGNALED) {
		/* close and lock the gate */
		gates = (gates & ~(3 << idx)) | (THINKOS_GATE_LOCKED << idx);
		if (__strex(gates_bmp, gates))
			goto again;
		DCC_LOG2(LOG_INFO, "<%d> enter gate %d.", self, wq);
		arg[0] = 0;
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
	/* update status, mark the thread clock enable bit */
	thinkos_rt.th_stat[self] = (wq << 1) + 1;
#endif
	/* insert into the flag wait queue */
	queue = __ldrex(&thinkos_rt.wq_lst[wq]);
	queue |= (1 << self);
	/* The gate may have been signaled while suspending (1).
	 If this is the case roll back and restart. */
	if ((((volatile uint32_t)*gates_bmp >> idx) & 3) == THINKOS_GATE_OPEN ||
		__strex(&thinkos_rt.wq_lst[wq], queue)) {
		/* roll back */
#if THINKOS_ENABLE_THREAD_STAT
		thinkos_rt.th_stat[self] = 0;
#endif
		/* insert into the ready wait queue */
		__bit_mem_wr(&thinkos_rt.wq_ready, self, 1);  
		goto again;
	}

	/* -- wait for event ---------------------------------------- */
	DCC_LOG2(LOG_INFO, "<%d> waiting at gate %d...", self, wq);
	/* set the clock */
	thinkos_rt.clock[self] = thinkos_rt.ticks + ms;
	/* insert into the clock wait queue */
	__bit_mem_wr(&thinkos_rt.wq_clock, self, 1);  
	/* signal the scheduler ... */
	__thinkos_defer_sched(); 
}
#endif

void thinkos_gate_exit_svc(int32_t * arg)
{

	unsigned int wq = arg[0];
	unsigned int sig = arg[1];
	unsigned int idx = wq - THINKOS_GATE_BASE;
	uint32_t queue;
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

#if THINKOS_ENABLE_SANITY_CHECK
	if (!__bit_mem_rd(thinkos_rt.gate, idx * 2 + 1)) {
		DCC_LOG2(LOG_ERROR, "<%d> gate %d is not locked!", 
				 thinkos_rt.active, wq);
		arg[0] = THINKOS_EPERM;
		return;
	}
#endif

	/* The gate is locked, this will prevent any interrupt
	   handler from messing up with the waiting queue.
	 */

	arg[0] = 0;

	if (sig) {
		DCC_LOG2(LOG_INFO, "<%d> exit gate %d, leave open.", 
				 thinkos_rt.active, wq);
		/* signal the gate acoording to the call argument */
		__bit_mem_wr(thinkos_rt.gate, idx * 2, 1);
	} else {
		if (!__bit_mem_rd(thinkos_rt.gate, idx * 2)) {
			/* unlock the gate */
			__bit_mem_wr(thinkos_rt.gate, idx * 2 + 1, 0);
			DCC_LOG2(LOG_INFO, "<%d> exit gate %d, leave closed.", 
					 thinkos_rt.active, wq);
			return;
		}
		DCC_LOG2(LOG_INFO, "<%d> exit gate %d, leave open due to signal.", 
				 thinkos_rt.active, wq);
	}

	do {
		/* get the gate wait queue bitmap */
		queue = __ldrex(&thinkos_rt.wq_lst[wq]);
		/* get a thread from the queue bitmap */
		if ((th = __clz(__rbit(queue))) == THINKOS_THREAD_NULL) {
			/* no threads waiting */
			__clrex();
			/* unlock the gate */
			__bit_mem_wr(thinkos_rt.gate, idx * 2 + 1, 0);
			return;
		} 
		/* remove from the wait queue */
		queue &= ~(1 << th);
	} while (__strex(&thinkos_rt.wq_lst[wq], queue));

	DCC_LOG2(LOG_INFO, "<%d> enter gate %d.", th, wq);

	/* close the gate */
	__bit_mem_wr(thinkos_rt.gate, idx * 2, 0);

	/* insert the thread into ready queue */
	__bit_mem_wr(&thinkos_rt.wq_ready, th, 1);
#if THINKOS_ENABLE_TIMED_CALLS
	/* possibly remove from the time wait queue */
	__bit_mem_wr(&thinkos_rt.wq_clock, th, 0);  
#endif
	/* set the thread's return value */
	thinkos_rt.ctx[th]->r0 = 0;
#if THINKOS_ENABLE_THREAD_STAT
	/* update status */
	thinkos_rt.th_stat[th] = 0;
#endif
	/* signal the scheduler ... */
	__thinkos_defer_sched();
}

/* void __thinkos_gate_open_i(uint32_t wq) */
void cm3_except13_isr(uint32_t wq)
{
	unsigned int idx = wq - THINKOS_GATE_BASE;
	uint32_t * gates_bmp;
	uint32_t gates;
	uint32_t queue;
	int th;

#if THINKOS_FLAG_MAX < 16
	gates_bmp = &thinkos_rt.gate[0];
#else
	gates_bmp = &thinkos_rt.gate[idx / 16];
	idx %= 16;
#endif
	idx *= 2;

again:
	/* check whether the gate is locked or not */
	gates = __ldrex(gates_bmp);
	if (gates & (THINKOS_GATE_LOCKED << idx)) {
		/* gate is locked, signal for open */
		gates |= THINKOS_GATE_SIGNALED << idx;
		if (__strex(gates_bmp, gates))
			goto again;
		DCC_LOG1(LOG_INFO, "gate %d is locked, signaling!", wq);
		return;
	}
	/* lock the gate to avoid race conditions */
	gates |= (THINKOS_GATE_LOCKED | THINKOS_GATE_SIGNALED) << idx;
	if (__strex(gates_bmp, gates))
		goto again;

	do {
		/* get the gate wait queue bitmap */
		queue = __ldrex(&thinkos_rt.wq_lst[wq]);
		/* get a thread from the queue bitmap */
		if ((th = __clz(__rbit(queue))) == THINKOS_THREAD_NULL) {
			/* no threads waiting */
			__clrex();
			/* unlock the gate */
			__bit_mem_wr(thinkos_rt.gate, idx + 1, 0);
			DCC_LOG1(LOG_INFO, "opening gate %d.", wq);
			return;
		} 
		/* remove from the wait queue */
		queue &= ~(1 << th);
	} while (__strex(&thinkos_rt.wq_lst[wq], queue));

	DCC_LOG2(LOG_INFO, "<%d> enter gate %d.", th, wq);

	/* close the gate */
	__bit_mem_wr(thinkos_rt.gate, idx, 0);

	/* insert the thread into ready queue */
	__bit_mem_wr(&thinkos_rt.wq_ready, th, 1);
#if THINKOS_ENABLE_TIMED_CALLS
	/* possibly remove from the time wait queue */
	__bit_mem_wr(&thinkos_rt.wq_clock, th, 0);  
#endif
	/* set the thread's return value */
	thinkos_rt.ctx[th]->r0 = 0;
#if THINKOS_ENABLE_THREAD_STAT
	/* update status */
	thinkos_rt.th_stat[th] = 0;
#endif
	/* signal the scheduler ... */
	__thinkos_defer_sched();
}

void __thinkos_gate_open_i(uint32_t wq) 
	__attribute__((weak, alias("cm3_except13_isr")));

void thinkos_gate_open_svc(int32_t * arg)
{
	unsigned int wq = arg[0];

#if THINKOS_ENABLE_ARG_CHECK
	unsigned int idx = wq - THINKOS_GATE_BASE;

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

	DCC_LOG1(LOG_INFO, "gate %d", wq);

	__thinkos_gate_open_i(wq);
}

#endif /* THINKOS_GATE_MAX > 0 */

