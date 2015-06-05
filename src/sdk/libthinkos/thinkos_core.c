/* 
 * thikos_core.c
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
#define __THINKOS_IRQ__
#include <thinkos_irq.h>
#define __THINKOS_DMON__
#include <thinkos_dmon.h>
#include <thinkos.h>
#include <thinkos_except.h>

/* -------------------------------------------------------------------------- 
 * Run Time ThinkOS block
 * --------------------------------------------------------------------------*/

struct thinkos_rt thinkos_rt;

/* This is a trick to save memory. We need an area to store
   the Idle thread context, but the Idle thread only uses 4 entries:
   xpsr, pc, lr, r12. The other registers are not used at any time. We 
   claim the space avalilable for this registers as part of the exception 
   stack. */
struct thinkos_except_and_idle __attribute__((aligned(8))) thinkos_idle;

#if THINKOS_ENABLE_SCHED_DEBUG
static inline void __attribute__((always_inline)) 
__dump_context(struct thinkos_context * __ctx) {
	DCC_LOG4(LOG_TRACE, "  r0=%08x  r1=%08x  r2=%08x  r3=%08x", 
			__ctx->r0, __ctx->r1, __ctx->r2, __ctx->r3);
	DCC_LOG4(LOG_TRACE, "  r4=%08x  r5=%08x  r6=%08x  r7=%08x", 
			__ctx->r4, __ctx->r7, __ctx->r6, __ctx->r7);
	DCC_LOG4(LOG_TRACE, "  r8=%08x  r9=%08x r10=%08x r11=%08x", 
			__ctx->r8, __ctx->r9, __ctx->r10, __ctx->r11);
	DCC_LOG4(LOG_TRACE, " r12=%08x  sp=%08x  lr=%08x  pc=%08x", 
			__ctx->r12, __ctx, __ctx->lr, __ctx->pc);
	DCC_LOG1(LOG_TRACE, "xpsr=%08x", __ctx->xpsr);
}

void test_call(struct thinkos_context * ctx)
{
	__dump_context(ctx);
}

static inline void __attribute__((always_inline)) __wait(void) {
	asm volatile ("mov    r3, #1\n"
				  "0:\n"
				  "cbz	r3, 1f\n"
				  "b.n  0b\n"
				  "1:\n" : : : "r3"); 
}
#endif

/* -------------------------------------------------------------------------- 
 * Idle task
 * --------------------------------------------------------------------------*/

void __attribute__((noreturn, naked)) thinkos_idle_task(void)
{
#if (THINKOS_ENABLE_MONITOR)
	register struct cm3_dcb * dcb asm("r3");
	register uint32_t * sigptr asm("r2");
	register uint32_t sigval asm("r1");
	register uint32_t demcr asm("r0");
#endif

	DCC_LOG(LOG_TRACE, "<<< Idle >>>");

#if (THINKOS_ENABLE_MONITOR)
	dcb = CM3_DCB;
	sigval = 1;
	sigptr = CM3_BITBAND_MEM(&thinkos_dmon_rt.events, DMON_IDLE);
#endif
	for (;;) {
#if (THINKOS_ENABLE_MONITOR)
		/* check if the monitor is requesting a notification when
		   entering IDLE state */
		asm volatile ("ldr %0, [%1, #12]\n" 
					  : "=r" (demcr) : "r"(dcb));
		if (demcr & DCB_DEMCR_MON_REQ) {
			/* Call the monitor by a pending a monitor interrupt */
			demcr = (demcr | DCB_DEMCR_MON_PEND) & ~DCB_DEMCR_MON_REQ;
			asm volatile (
						  "str %1, [%0]\n" 
						  "str %3, [%2, #12]\n" 
						  "isb\n" 
						  :  : "r"(sigptr), "r"(sigval), "r"(dcb), "r"(demcr) 
						  : );
		}
#endif
#if THINKOS_ENABLE_IDLE_WFI
		asm volatile ("wfi\n"); /* wait for interrupt */
#endif
	}
}

/* -------------------------------------------------------------------------- 
 * Scheduler
 * --------------------------------------------------------------------------*/

static inline struct thinkos_context * __attribute__((always_inline)) 
__sched_entry(void) {
	register struct thinkos_context * ctx asm("r0");
	asm volatile (
#if THINKOS_ENABLE_SCHED_DEBUG
				  "push  {lr}\n"
				  "sub   sp, #16\n"
#endif				  
				  "mrs   %0, PSP\n" 
				  "stmdb %0!, {r4-r11}\n"
#if THINKOS_ENABLE_FPU 
				  "vstmdb.64 %0!, {d0-d15}\n"
#endif
				  : "=r" (ctx));
	return ctx;
}

static inline void __attribute__((always_inline)) 
__sched_exit(struct thinkos_context * __ctx) {
	register struct thinkos_context * r0 asm("r0") = __ctx;
	asm volatile (
#if THINKOS_ENABLE_SCHED_DEBUG
				  "add    sp, #16\n"
				  "pop    {lr}\n"
#endif				  
#if THINKOS_ENABLE_FPU 
				  "add    r3, %0, #40 * 4\n"
				  "msr    PSP, r3\n"
				  "vldmia.64 %0!, {d0-d15}\n"
#else
				  "add    r3, %0, #8 * 4\n"
				  "msr    PSP, r3\n"
#endif
				  "ldmia  %0, {r4-r11}\n"
				  "bx     lr\n"
				  : : "r" (r0) : "r3"); 
}


/* Partially restore the context and then set the NMI pending
   to step thread. */ 
static inline void __attribute__((always_inline)) 
__sched_exit_step(unsigned int thread_id, struct thinkos_context * ctx) {
	register unsigned int r0 asm("r0") = thread_id;
	register struct thinkos_context * r1 asm("r1") = ctx;
	register unsigned int r2 asm("r2") = THINKOS_DEBUG_STEP_I;
	asm volatile (
#if THINKOS_ENABLE_SCHED_DEBUG
				  "add    sp, #16\n"
				  "pop    {lr}\n"
#endif				  
#if THINKOS_ENABLE_FPU 
				  "add    r3, %1, #40 * 4\n"
				  "msr    PSP, r3\n"
				  "vldmia.64 %1!, {d0-d15}\n"
#else
				  "add    r3, %1, #8 * 4\n"
				  "msr    PSP, r3\n"
#endif
				  "ldmia  %1, {r4-r11}\n"
				  "movw   r3, #0xed00\n"
				  "movt   r3, #0xe000\n"
				  "mov.w  r12, #0x80000000\n"
				  "str.w  r12, [r3, #4]\n" /* NMI pending */
				  "isb    sy\n"
				  : : "r" (r0), "r" (r1), "r" (r2) : "r3"); 
}

/* --------------------------------------------------------------------------
 * ThinkOS - scheduler 
 * --------------------------------------------------------------------------*/
void __attribute__((naked, aligned(16))) cm3_pendsv_isr(void)
{
	struct thinkos_context * old_ctx;
	struct thinkos_context * new_ctx;
	uint32_t old_thread_id;
	uint32_t new_thread_id;

//	DCC_LOG(LOG_TRACE, "...");
//	__wait();

	/* save the context */
	old_ctx = __sched_entry();
	/* get the active (current) thread */	
	old_thread_id = thinkos_rt.active;

	/* get a thread from the ready bitmap */
	new_thread_id = __clz(__rbit(thinkos_rt.wq_ready));
	/* update the active thread */
	thinkos_rt.active = new_thread_id;
	/* save the old context (SP) */
	thinkos_rt.ctx[old_thread_id] = old_ctx;
	/* get the new context (SP) */
	new_ctx = thinkos_rt.ctx[new_thread_id];

#if THINKOS_ENABLE_PROFILING
	{
		uint32_t cyccnt = CM3_DWT->cyccnt;
		int32_t delta = cyccnt - thinkos_rt.cycref;
		/* update the reference */
		thinkos_rt.cycref = cyccnt;
		/* update thread's cycle counter */
		thinkos_rt.cyccnt[old_thread_id] += delta; 
	}
#endif

#if THINKOS_ENABLE_SCHED_DEBUG
	if (thinkos_rt.sched_trace_req) {
		DCC_LOG1(LOG_TRACE, "active=%d", new_thread_id);
		DCC_LOG1(LOG_TRACE, "sp=%08x", cm3_sp_get());
		__dump_context(new_ctx);
		__wait();
		thinkos_rt.sched_trace_req = 0;
	}
#endif

#if THINKOS_ENABLE_DEBUG_STEP
	if ((1 << new_thread_id) & thinkos_rt.step_req) {
		__sched_exit_step(new_thread_id, new_ctx);
	} else
#endif
	/* restore the context */
	__sched_exit(new_ctx);
}

#if (THINKOS_SEMAPHORE_MAX  > 0)
void __thinkos_sem_post(uint32_t wq)
{
	int th;

	DCC_LOG1(LOG_INFO, "wq=%d...", wq);
	if ((th = __thinkos_wq_head(wq)) != THINKOS_THREAD_NULL) {
		DCC_LOG1(LOG_TRACE, "wakeup sem=%d", wq);
		/* wakeup from the sem wait queue */
		__thinkos_wakeup(wq, th);
		/* signal the scheduler ... */
		__thinkos_defer_sched();
	} else {
		DCC_LOG1(LOG_INFO, "increment sem=%d", wq);
		/* no threads waiting on the semaphore, increment. */ 
		thinkos_rt.sem_val[wq - THINKOS_SEM_BASE]++;
	}
}
#endif

#if (THINKOS_EVENT_MAX > 0)
void __thinkos_ev_raise(uint32_t wq, int ev)
{
	unsigned int no = wq - THINKOS_EVENT_BASE;
	int th;

	if ((__bit_mem_rd(&thinkos_rt.ev[no].mask, ev)) &&  
		((th = __thinkos_wq_head(wq)) != THINKOS_THREAD_NULL)) {
		/* wakeup from the event wait queue, set the return of
		   the thread to event */
		DCC_LOG2(LOG_INFO, "wakeup ev=%d.%d", wq, ev);
		__thinkos_wakeup_return(wq, th, ev);
		/* signal the scheduler ... */
		__thinkos_defer_sched();
	} else {
		DCC_LOG2(LOG_INFO, "pending ev=%d.%d", wq, ev);
		/* event is masked or no thread is waiting on the 
		   event set, mark the event as pending */
		__bit_mem_wr(&thinkos_rt.ev[no].pend, ev, 1);  
	}
}
#endif

#if (THINKOS_FLAG_MAX > 0)
void __thinkos_flag_give(uint32_t wq)
{
	unsigned int flag = wq - THINKOS_FLAG_BASE;
	int th;

	DCC_LOG1(LOG_INFO, "wq=%d...", wq);

	/* flag_give(): wakeup a single thread waiting on the flag 
	   OR set the flag */
	/* get the flag state */
	if (__bit_mem_rd(thinkos_rt.flag.sig, flag) == 0) {
		/* get a thread from the queue */
		if ((th = __thinkos_wq_head(wq)) != THINKOS_THREAD_NULL) {
			__thinkos_wakeup(wq, th);
			/* signal the scheduler ... */
			__thinkos_defer_sched();
		} else {
			/* set the flag bit */
			__bit_mem_wr(thinkos_rt.flag.sig, flag, 1);  
		}
	}
}
#endif

#if (THINKOS_FLAG_MAX > 0)
#if THINKOS_ENABLE_FLAG_LOCK
void __thinkos_flag_signal(uint32_t wq)
{
	unsigned int flag = wq - THINKOS_FLAG_BASE;
	int th;

	DCC_LOG1(LOG_INFO, "wq=%d...", wq);
	/* flag_signal() wakeup a single thread waiting on the flag 
	   OR set the flag */
	/* set the flag bit */
	__bit_mem_wr(thinkos_rt.flag.sig, flag, 1);  
	if (!__bit_mem_rd(thinkos_rt.flag.lock, flag)) {
		/* get a thread from the queue */
		if ((th = __thinkos_wq_head(wq)) != THINKOS_THREAD_NULL) {
			/* lock the flag */
			__bit_mem_wr(thinkos_rt.flag.lock, flag, 1);
			__thinkos_wakeup(wq, th);

			/* clear the flag bit */
			__bit_mem_wr(thinkos_rt.flag.sig, flag, 0);
			/* signal the scheduler ... */
			__thinkos_defer_sched();
		} 
	}
}
#endif /* THINKOS_ENABLE_FLAG_LOCK */
#endif

#if (THINKOS_FLAG_MAX > 0)
void __thinkos_flag_clr(uint32_t wq)
{
	unsigned int flag = wq - THINKOS_FLAG_BASE;

	/* clear the flag signal bit */
	__bit_mem_wr(thinkos_rt.flag.sig, flag, 0);  
}
#endif

#if (THINKOS_FLAG_MAX > 0)
void __thinkos_flag_set(uint32_t wq)
{
	unsigned int flag = wq - THINKOS_FLAG_BASE;
	int th;

	DCC_LOG1(LOG_INFO, "wq=%d...", wq);
	/* set the flag and wakeup all threads waiting on the flag */

	/* set the flag bit */
	__bit_mem_wr(thinkos_rt.flag.sig, flag, 1);  

	/* get a thread from the queue */
	if ((th = __thinkos_wq_head(wq)) != THINKOS_THREAD_NULL) {
		__thinkos_wakeup(wq, th);
		/* get the remaining threads from the queue */
		while ((th = __thinkos_wq_head(wq)) != 
			   THINKOS_THREAD_NULL) {
			__thinkos_wakeup(wq, th);
		}
		/* signal the scheduler ... */
		__thinkos_defer_sched();
	}
}
#endif


static void thinkos_signal_queue(void)
{
#if (THINKOS_SEMAPHORE_MAX > 0) || (THINKOS_EVENT_MAX > 0) || \
	(HINKOS_FLAG_MAX > 0)
	uint32_t tail = thinkos_rt.sig.tail;
	uint32_t opc;
	uint32_t wq;

	/* ----------------------------------------------------------------------
	 * Process the signal queue 
	 * ----------------------------------------------------------------------*/
	while (tail != thinkos_rt.sig.head) {
		opc = thinkos_rt.sig.queue[tail % THINKOS_SIG_QUEUE_LEN];
		thinkos_rt.sig.tail = tail + 1;
		wq = opc & 0x3ff;
		if (opc & 0x8000) {
			/* Event sets */
#if (THINKOS_EVENT_MAX > 0)
			__thinkos_ev_raise(wq, (opc & 0x7c00) >> 10);
#endif
		} else if (opc & 0x4000) {
			/* Flags */
#if (THINKOS_FLAG_MAX > 0)
			switch ((opc & 0x0c00) >> 10) {
			case 0: 
				__thinkos_flag_give(wq);
			case 1: 
#if THINKOS_ENABLE_FLAG_LOCK
				__thinkos_flag_signal(wq);
#endif /* THINKOS_ENABLE_FLAG_LOCK */
				break;
			case 2: 
				__thinkos_flag_clr(wq);
				break;
			case 3: 
				__thinkos_flag_set(wq);
				break;
			}
#endif /* (THINKOS_FLAG_MAX > 0) */
		} else {
			/* Semaphores */
#if (THINKOS_SEMAPHORE_MAX  > 0)
			__thinkos_sem_post(wq);
#endif
		}
	}
#endif
}

#if THINKOS_ENABLE_CLOCK
static void thinkos_time_wakeup(int thread_id) 
{
#if THINKOS_ENABLE_THREAD_STAT
	int stat;
	/* update the thread status */
	stat = thinkos_rt.th_stat[thread_id];
	thinkos_rt.th_stat[thread_id] = 0;
	/* remove from other wait queue, if any */
	__bit_mem_wr(&thinkos_rt.wq_lst[stat >> 1], thread_id, 0);  
#endif
	/* remove from the time wait queue */
	__bit_mem_wr(&thinkos_rt.wq_clock, thread_id, 0);  
	DCC_LOG1(LOG_MSG, "Wakeup %d...", thread_id);
	/* insert into the ready wait queue */
	__bit_mem_wr(&thinkos_rt.wq_ready, thread_id, 1);  
	__thinkos_defer_sched();
#if THINKOS_ENABLE_SCHED_DEBUG
	thinkos_rt.sched_trace_req = 1;
#endif
}
#endif /* THINKOS_ENABLE_CLOCK */

#if THINKOS_ENABLE_TIMESHARE
static void thinkos_timeshare(void) 
{
	int32_t idx;

	idx = thinkos_rt.active;

	/*  */
	thinkos_rt.sched_val[idx] -= thinkos_rt.sched_pri[idx];
	if (thinkos_rt.sched_val[idx] < 0) {
		thinkos_rt.sched_val[idx] += thinkos_rt.sched_limit;
		
		if (__bit_mem_rd(&thinkos_rt.wq_ready, idx) == 0) {
			DCC_LOG1(LOG_WARNING, "thread %d is active but not ready!!!", idx);
		} else {
			/* insert into the CPU wait queue */
			__bit_mem_wr(&thinkos_rt.wq_tmshare, idx, 1);  
			__thinkos_suspend(idx);
			__thinkos_defer_sched();
		}
	}
}
#endif /* THINKOS_ENABLE_TIMESHARE */


/* --------------------------------------------------------------------------
 * ThinkOS - defered services
 * --------------------------------------------------------------------------*/

void __attribute__((aligned(16))) cm3_systick_isr(void)
{
#if THINKOS_ENABLE_CLOCK
	uint32_t ticks;
	uint32_t wq;
	int j;
#endif

	thinkos_signal_queue();

#if THINKOS_ENABLE_CLOCK
	if ((CM3_SYSTICK->csr & SYSTICK_CSR_COUNTFLAG) == 0) {
		DCC_LOG(LOG_INFO, "systick COUNTFLAG not set!!!");
		return;
	}

	ticks = thinkos_rt.ticks; 
	ticks++;
	thinkos_rt.ticks = ticks; 

	wq = __rbit(thinkos_rt.wq_clock);
	while ((j = __clz(wq)) < 32) {
		wq &= ~(0x80000000 >> j);  
		if ((int32_t)(thinkos_rt.clock[j] - ticks) <= 0) {
			thinkos_time_wakeup(j); 
		}
	}

#if THINKOS_ENABLE_MONITOR
	if ((int32_t)(thinkos_rt.dmclock - ticks) == 0) {
		dmon_signal(DMON_ALARM);
	}
#endif

#if THINKOS_ENABLE_TIMESHARE
	thinkos_timeshare(); 
#endif /* THINKOS_ENABLE_TIMESHARE */

#endif /* THINKOS_ENABLE_CLOCK */
}

