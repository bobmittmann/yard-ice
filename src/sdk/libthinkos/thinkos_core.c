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

//#include <thinkos_svc.h>
#include <thinkos_except.h>

#include <stdio.h>
#include <string.h>

extern const char thinkos_svc_nm[];
extern const char thinkos_nmi_nm[];
extern const struct thinkos_thread_inf thinkos_main_inf;

/* -------------------------------------------------------------------------- 
 * Run Time ThinkOS block
 * --------------------------------------------------------------------------*/

struct thinkos_rt thinkos_rt;

/* This is a trick to save memory. We need an area to store
   the Idle thread context, but the Idle thread only uses 4 entries:
   xpsr, pc, lr, r12. The other registers are not used at any time. We 
   claim the space avalilable for this registers as part of the exception 
   stack. */
struct thinkos_except_and_idle thinkos_idle;

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
	DCC_LOG2(LOG_TRACE, "xpsr=%08x ret=%08x ", 
			 __ctx->xpsr, __ctx->ret);
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

void __attribute__((noreturn, naked)) thinkos_idle_task(void)
{
	/* FIXME: the profile counter does not take into 
	 account the time spent on interrupt handlers or OS (SYS).. */
//#if THINKOS_ENABLE_PROFILING
#if 0 
	volatile uint32_t * cycref = &thinkos_rt.cycref;
	volatile uint32_t * cycidle = &thinkos_rt.cyccnt[THINKOS_CYCCNT_IDLE];
	volatile uint32_t * cycsys = &thinkos_rt.cyccnt[THINKOS_CYCCNT_SYS];
	register uint32_t * dwt_cyccnt asm ("lr") = (void *)&(CM3_DWT->cyccnt);
	for (;;) {
		uint32_t cyccnt;
		uint32_t cycprev;
		int32_t delta;

		cm3_cpsie_i();
		cm3_cpsid_i();

		asm volatile ("ldr	%0, [%1, #0]" : "=r" (cyccnt) : "r" (dwt_cyccnt));
		delta = cyccnt - *cycref;
		/* update system cycle counter */
		*cycsys += delta; 
		/* save counter */
		cycprev = cyccnt; 
		asm volatile ("wfi\n"); /* wait for interrupt */

		asm volatile ("ldr	%0, [%1, #0]" : "=r" (cyccnt) : "r" (dwt_cyccnt));
		delta = cyccnt - cycprev;
		/* update the reference */
		*cycref = cyccnt;
		/* update idle cycle counter */
		*cycidle += delta; 
	}
#else
#if THINKOS_ENABLE_DEBUG_STEP
	volatile uint32_t * req_bit = CM3_BITBAND_MEM(&thinkos_dmon_rt.req, 
												  DMON_IDLE);
	volatile uint32_t * sig_bit = CM3_BITBAND_MEM(&thinkos_dmon_rt.events, 
												  DMON_IDLE);
	uint32_t val;
#endif

	for (;;) {
#if THINKOS_ENABLE_SCHED_DEBUG
		DCC_LOG(LOG_MSG, "zzz...");
//		__dump_context(&thinkos_idle.ctx);
#endif
#if THINKOS_ENABLE_DEBUG_STEP
		/* check if the monitor is requesting a notification when
		   entering IDLE state */
		if ((val = *req_bit) != 0) {
			/* Notify the monitor */
			/* Set the IDLE signal bit */
			*sig_bit = val;
			/* Call the monitor by a pending a monitor interrupt */
			CM3_DCB->demcr |= DCB_DEMCR_MON_PEND;
		}
#endif
#if THINKOS_ENABLE_IDLE_WFI
		asm volatile ("wfi\n"); /* wait for interrupt */
#endif
	}

#endif
}

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
				  "add    r3, %0, #8 * 4\n"
				  "msr    PSP, r3\n"
				  "ldmia  %0, {r4-r11}\n"
				  "bx     lr\n"
				  : : "r" (r0) : "r3"); 
}

/* THinkOS - scheduler */
void __attribute__((naked, aligned(16))) cm3_pendsv_isr(void)
{
	struct thinkos_context * ctx;
	uint32_t idx;

//	DCC_LOG(LOG_TRACE, "...");

	/* save the context */
	ctx = __sched_entry();

	/* get the active (current) thread */	
	idx = thinkos_rt.active;

	/* save SP */
	thinkos_rt.ctx[idx] = ctx;

#if THINKOS_ENABLE_PROFILING
	{
		uint32_t cyccnt = CM3_DWT->cyccnt;
		int32_t delta = cyccnt - thinkos_rt.cycref;
		/* update the reference */
		thinkos_rt.cycref = cyccnt;
		/* update thread's cycle counter */
		thinkos_rt.cyccnt[idx] += delta; 
	}
#endif

	/* get a thread from the ready bitmap */
	idx = __clz(__rbit(thinkos_rt.wq_ready));

	/* update the active thread */
	thinkos_rt.active = idx;

	ctx = thinkos_rt.ctx[idx];

#if THINKOS_ENABLE_SCHED_DEBUG
	if (thinkos_rt.sched_trace_req) {
		DCC_LOG1(LOG_TRACE, "active=%d", idx);
		DCC_LOG1(LOG_TRACE, "sp=%08x", cm3_sp_get());
		__dump_context(ctx);
		__wait();
		thinkos_rt.sched_trace_req = 0;
	}
#endif

#if THINKOS_ENABLE_DEBUG_STEP
	if (idx == thinkos_rt.step_id) {
		struct cm3_dcb * dcb = CM3_DCB;
		/* rise the BASEPRI to stop the scheduler and interrupts */
//		cm3_basepri_set(EXCEPT_PRIORITY); 
		cm3_basepri_set(MONITOR_PRIORITY); 
		dcb->demcr |= DCB_DEMCR_MON_STEP; /* step the processor */
	}
#endif
	/* restore the context */
	__sched_exit(ctx);
}

#if THINKOS_ENABLE_CLOCK || THINKOS_ENABLE_TIMESHARE

void __attribute__((aligned(16))) cm3_systick_isr(void)
{
	int sched = 0;
#if THINKOS_ENABLE_CLOCK
	uint32_t ticks;
	uint32_t wq;
	int j;

	ticks = thinkos_rt.ticks; 
	ticks++;
	thinkos_rt.ticks = ticks; 

#if THINKOS_ENABLE_MONITOR
	if ((int32_t)(thinkos_rt.dmclock - ticks) == 0) {
		dmon_signal(DMON_ALARM);
	}
#endif

	wq = __rbit(thinkos_rt.wq_clock);
	while ((j = __clz(wq)) < 32) {
		wq &= ~(0x80000000 >> j);  
		if ((int32_t)(thinkos_rt.clock[j] - ticks) <= 0) {
#if THINKOS_ENABLE_THREAD_STAT
			int stat;
			/* update the thread status */
			stat = thinkos_rt.th_stat[j];
			thinkos_rt.th_stat[j] = 0;
			/* remove from other wait queue, if any */
			__bit_mem_wr(&thinkos_rt.wq_lst[stat >> 1], j, 0);  
#endif
			/* remove from the time wait queue */
			__bit_mem_wr(&thinkos_rt.wq_clock, j, 0);  
			DCC_LOG1(LOG_MSG, "Wakeup %d...", j);
#if THINKOS_ENABLE_SCHED_DEBUG
			thinkos_rt.sched_trace_req = 1;
#endif
			/* insert into the ready wait queue */
			__bit_mem_wr(&thinkos_rt.wq_ready, j, 1);  
			sched++;
//			printf("^%d", j);
		} else {
//			printf("t");
		}
	}

//	printf(".");

#endif /* THINKOS_ENABLE_CLOCK */

#if THINKOS_ENABLE_TIMESHARE
	int32_t idx;

	idx = thinkos_rt.active;

	/* write the schedule bit into the ready bitmap */
//	if (idx != THINKOS_THREAD_IDLE) {
		thinkos_rt.sched_val[idx] -= thinkos_rt.sched_pri[idx];
		if (thinkos_rt.sched_val[idx] < 0) {
			thinkos_rt.sched_val[idx] += thinkos_rt.sched_limit;
			/* set the non schedule flag for the active thread */
//			__bit_mem_wr(&thinkos_rt.active.flags, THINKOS_NON_SCHED, 1);  
			/* insert into the CPU wait queue */
			__bit_mem_wr(&thinkos_rt.wq_tmshare, idx, 1);  
//			printf(" w%x", thinkos_rt.wq_tmshare);
			__bit_mem_wr(&thinkos_rt.wq_ready, idx, 0);
			cm3_cpsid_i();
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
			cm3_cpsie_i();
			sched++;
		} else {
//			__bit_mem_wr(&thinkos_rt.wq_ready, idx, 1);
//			printf(" r%x", thinkos_rt.wq_ready);
		}
//	} else {
//		printf("i");
//	}

#endif /* THINKOS_ENABLE_TIMESHARE */

	if (sched)
		__thinkos_defer_sched();
}
#endif /* THINKOS_ENABLE_CLOCK || THINKOS_ENABLE_TIMESHARE */

void __attribute__((noreturn)) thinkos_thread_exit(int code)
{
	int self = thinkos_rt.active;
	int j;

	DCC_LOG2(LOG_TRACE, "<%d> code=%d", self, code); 

#if THINKOS_ENABLE_TIMESHARE
	/* possibly remove from the time share wait queue */
	__bit_mem_wr(&thinkos_rt.wq_tmshare, self, 0);  
#endif

	/* disable interrupts */
	cm3_cpsid_i();

#ifdef THINKOS_ENABLE_CANCEL
#if THINKOS_ENABLE_THREAD_STAT
	int stat;
	/* update the thread status */
	stat = thinkos_rt.th_stat[self];
	thinkos_rt.th_stat[self] = 0;
	/* remove from other wait queue, if any */
	__bit_mem_wr(&thinkos_rt.wq_lst[stat >> 1], self, 0);  
#else
	int i;
	/* remove from other wait queue, if any */
	for (i = 0; i < __wq_idx(thinkos_rt.wq_end); i++) {
		__bit_mem_wr(&thinkos_rt.wq_lst[i], self, 0);  
	}
#endif
#endif

	for (j = 0; j < THINKOS_THREADS_MAX; j++) {
#if THINKOS_ENABLE_THREAD_ALLOC
		if (__bit_mem_rd(&thinkos_rt.th_alloc, j) == 0)
			continue;
#endif

#if THINKOS_ENABLE_TIMESHARE
		/* schedule limit reevaluation */
		if (thinkos_rt.sched_limit < thinkos_rt.sched_pri[j]) {
			thinkos_rt.sched_limit = thinkos_rt.sched_pri[j];
		}
#endif

#if THINKOS_ENABLE_JOIN
		if (__bit_mem_rd(&thinkos_rt.wq_join[self], j) != 0) {
			DCC_LOG1(LOG_TRACE, "wakeup <%d>", j);
			__bit_mem_wr((void *)&thinkos_rt.wq_ready, j, 1); 
			__bit_mem_wr((void *)&thinkos_rt.wq_join[self], j, 0);  
			thinkos_rt.ctx[j]->r0 = code;
		}
#endif
	}

	cm3_cpsie_i();

#if !THINKOS_ENABLE_THREAD_ALLOC && THINKOS_ENABLE_TIMESHARE
	/* clear the schedule priority. In case the thread allocations
	 is disabled, the schedule limit reevaluation may produce inconsistent
	 results ... */
	thinkos_rt.sched_pri[self] = 0;
#endif

#if THINKOS_ENABLE_THREAD_ALLOC
	/* Releases the thread block */
	__bit_mem_wr(&thinkos_rt.th_alloc, self, 0);
#endif


	/* FIXME: clear context. The way is implemented th scheduler will 
	   override this value...  */
	thinkos_rt.ctx[self] = 0;

	/* wait forever */
	__thinkos_wait(self);

	for(;;);
}

int thinkos_init(struct thinkos_thread_opt opt)
{
	struct cm3_systick * systick = CM3_SYSTICK;
	int self;
	uint32_t msp;
#if	(THINKOS_IRQ_MAX > 0)
	int irq;
#endif
	int i;

	/* disable interrupts */
	cm3_cpsid_i();

#if THINKOS_ENABLE_EXCEPTIONS
	thinkos_exception_init();
#endif

	/* adjust exception priorities */
	/*
	 *  0x00 - low latency interrupts
     *
	 *  0x20 - high priority interrupts
	 *  0x40   .
	 *
	 *  0x60 - SVC
	 *  0x80 - regular priority interrupts
	 * 
	 *  0xa0 - SysTick
	 *  0xc0 - PendSV (scheduler)
	 *
	 *  0xe0 - very low priority interrupts
	 */

	/* SVC shuld not be preempted by the scheduler, thus it runs 
	   at higher priority. In order for the regular priority
	   interrupts to call SVC, they shuld run at a lower priority
	   then SVC.*/
	cm3_except_pri_set(CM3_EXCEPT_SVC, SYSCALL_PRIORITY);
	/* SysTick interrupt has to have a lowr priority then SVC */
	cm3_except_pri_set(CM3_EXCEPT_SYSTICK, CLOCK_PRIORITY);
	/* PendSV interrupt has to have the lowest priority among
	   regular interrupts (higher number) */
	cm3_except_pri_set(CM3_EXCEPT_PENDSV, SCHED_PRIORITY);

	cm3_except_pri_set(CM3_EXCEPT_MEM_MANAGE, EXCEPT_PRIORITY);
	cm3_except_pri_set(CM3_EXCEPT_BUS_FAULT, EXCEPT_PRIORITY);
	cm3_except_pri_set(CM3_EXCEPT_USAGE_FAULT, EXCEPT_PRIORITY);
	cm3_except_pri_set(CM3_EXCEPT_DEBUG_MONITOR, MONITOR_PRIORITY);

#if THINKOS_ENABLE_DEBUG_STEP
	thinkos_rt.step_id = -1;
	thinkos_rt.step_cnt = 0;
#endif

#if	(THINKOS_IRQ_MAX > 0)
	/* adjust IRQ priorities to regular (above SysTick and bellow SVC) */
	for (irq = 0; irq < THINKOS_IRQ_MAX; irq++) {
		cm3_irq_pri_set(irq, IRQ_PRIORITY_REGULAR);
		thinkos_rt.irq_th[irq] = -1;
	}
#endif

	/* configure the thread stack */
	cm3_psp_set(cm3_sp_get());
	/* configure the main stack */
#if 0
	msp = (uint32_t)&thinkos_except_stack + sizeof(thinkos_except_stack);
#endif
	msp = (uint32_t)&thinkos_idle.stack.r0;
	cm3_msp_set(msp);

	DCC_LOG2(LOG_TRACE, "msp=0x%08x idle=0x%08x", msp, &thinkos_idle);

	/* configure the use of PSP in thread mode */
	cm3_control_set(CONTROL_THREAD_PSP | CONTROL_THREAD_PRIV);

	/* initialize exception stack */
	for (i = 0; i < (THINKOS_EXCEPT_STACK_SIZE / 4 - IDLE_UNUSED_REGS); ++i)
		thinkos_idle.except_stack[i] = 0xdeadbeef;

	/* initialize the idle thread */
	thinkos_rt.idle_ctx = &thinkos_idle.ctx;
#if THINKOS_ENABLE_TIMESHARE
	thinkos_rt.sched_idle_val = 0;
	thinkos_rt.sched_idle_pri = 0;
#endif
//	thinkos_idle.ctx.ret = CM3_EXC_RET_THREAD_PSP;


	thinkos_idle.ctx.pc = (uint32_t)thinkos_idle_task,
	thinkos_idle.ctx.xpsr = 0x01000000;

#if THINKOS_ENABLE_MUTEX_ALLOC
	{	/* initialize the mutex allocation bitmap */ 
		int i;
		for (i = 0; i < (THINKOS_MUTEX_MAX / 32); ++i)
			thinkos_rt.mutex_alloc[i] = 0;
		if (THINKOS_MUTEX_MAX % 32)
			thinkos_rt.mutex_alloc[i] = 0xffffffff << (THINKOS_MUTEX_MAX % 32);
	}
#endif

#if THINKOS_ENABLE_SEM_ALLOC
	{	/* initialize the semaphore allocation bitmap */ 
		int i;
		for (i = 0; i < (THINKOS_SEMAPHORE_MAX / 32); ++i)
			thinkos_rt.sem_alloc[i] = 0;
		if (THINKOS_SEMAPHORE_MAX % 32)
			thinkos_rt.sem_alloc[i] = 0xffffffff << (THINKOS_SEMAPHORE_MAX % 32);
	}
#endif

#if THINKOS_ENABLE_COND_ALLOC
	{	/* initialize the conditional variable allocation bitmap */ 
		int i;
		for (i = 0; i < (THINKOS_COND_MAX / 32); ++i)
			thinkos_rt.cond_alloc[i] = 0;
		if (THINKOS_COND_MAX % 32)
			thinkos_rt.cond_alloc[i] = 0xffffffff << (THINKOS_COND_MAX % 32);
	}
#endif

#if THINKOS_ENABLE_FLAG_ALLOC
	{	/* initialize the flag allocation bitmap */ 
		int i;
		for (i = 0; i < (THINKOS_FLAG_MAX / 32); ++i)
			thinkos_rt.flag_alloc[i] = 0;
		if (THINKOS_FLAG_MAX % 32)
			thinkos_rt.flag_alloc[i] = 0xffffffff << (THINKOS_FLAG_MAX % 32);
	}
#endif

#if THINKOS_ENABLE_EVENT_ALLOC
	{	/* initialize the event set allocation bitmap */ 
		int i;
		for (i = 0; i < (THINKOS_EVENT_MAX / 32); ++i)
			thinkos_rt.ev_alloc[i] = 0;
		if (THINKOS_EVENT_MAX % 32)
			thinkos_rt.ev_alloc[i] = 0xffffffff << (THINKOS_EVENT_MAX % 32);
	}
#endif

#if (THINKOS_MUTEX_MAX > 0)
	{
		int i;
		/* initialize the mutex locks */
		for (i = 0; i < THINKOS_MUTEX_MAX; i++) 
			thinkos_rt.lock[i] = -1;
	}
#endif

#if THINKOS_FLAG_MAX > 0
	{
		/* initialize the flags */
		int i;
		for (i = 0; i < ((THINKOS_FLAG_MAX + 31) / 32); ++i) {
			thinkos_rt.flag.sig[i] = 0;
#if THINKOS_ENABLE_FLAG_LOCK
			thinkos_rt.flag.lock[i] = 0;
#endif 
		}
	}
#endif /* THINKOS_EVENT_MAX > 0 */

#if THINKOS_EVENT_MAX > 0
	{
		int i;
		/* initialize the event sets */
		for (i = 0; i < THINKOS_EVENT_MAX; i++) {
			thinkos_rt.ev[i].pend = 0; /* no pending events */
			thinkos_rt.ev[i].mask = 0xffffffff; /* all events a are enabled */
		}
	}
#endif /* THINKOS_EVENT_MAX > 0 */

	/* initialize the main thread */ 
	/* alloc main thread */
	if (opt.id >= THINKOS_THREADS_MAX)
		opt.id = THINKOS_THREADS_MAX - 1;
#if THINKOS_ENABLE_THREAD_ALLOC
	/* initialize the thread allocation bitmap */ 
	thinkos_rt.th_alloc[0] = (uint32_t)(0xffffffffLL << THINKOS_THREADS_MAX);
	self = thinkos_alloc_lo(thinkos_rt.th_alloc, opt.id);
#else
	self = opt.id;
#endif

#if THINKOS_ENABLE_THREAD_INFO
	thinkos_rt.th_inf[self] = (struct thinkos_thread_inf *)&thinkos_main_inf;
#endif

#if THINKOS_ENABLE_TIMESHARE

#if THINKOS_SCHED_LIMIT_MIN < 1
#error "THINKOS_SCHED_LIMIT_MIN must be at least 1"
#endif

#if THINKOS_SCHED_LIMIT_MAX < THINKOS_SCHED_LIMIT_MIN
#error "THINKOS_SCHED_LIMIT_MAX < THINKOS_SCHED_LIMIT_MIN !!!"
#endif

	if (opt.priority > THINKOS_SCHED_LIMIT_MAX)
		opt.priority = THINKOS_SCHED_LIMIT_MAX;

	thinkos_rt.sched_pri[self] = opt.priority;
	thinkos_rt.sched_val[self] = opt.priority / 2;

	/* set the initial schedule limit */
	thinkos_rt.sched_limit = opt.priority;
	if (thinkos_rt.sched_limit < THINKOS_SCHED_LIMIT_MIN)
		thinkos_rt.sched_limit = THINKOS_SCHED_LIMIT_MIN;
#endif /* THINKOS_ENABLE_TIMESHARE */

	/* set the active thread */
	thinkos_rt.active = self;
	__bit_mem_wr(&thinkos_rt.wq_ready, self, 1);

#if (THINKOS_THREADS_MAX < 32) 
	/* put the IDLE thread in the ready queue */
	__bit_mem_wr(&thinkos_rt.wq_ready, THINKOS_THREADS_MAX, 1);
#endif

	DCC_LOG2(LOG_TRACE, "threads_max=%d ready=%08x", 
			 THINKOS_THREADS_MAX, thinkos_rt.wq_ready);

	/* initialize the SysTick module */
	systick->load = cm3_systick_load_1ms; /* 1ms tick period */
	systick->val = 0;
#if THINKOS_ENABLE_CLOCK || THINKOS_ENABLE_TIMESHARE
	systick->ctrl = SYSTICK_CTRL_ENABLE | SYSTICK_CTRL_TICKINT;
#else
	systick->ctrl = SYSTICK_CTRL_ENABLE;
#endif

#if THINKOS_ENABLE_PAUSE
	if (opt.paused) {
		/* insert into the paused list */
		__bit_mem_wr(&thinkos_rt.wq_paused, self, 1);
		/* Invoke the scheduler */
		__thinkos_defer_sched();
	} else
#endif

#if THINKOS_ENABLE_PROFILING
	{
		int i;
		/* initialize cycle counters */
		for (i = 0; i < THINKOS_THREADS_MAX + 2; i++)
			thinkos_rt.cyccnt[i] = 0; 

		/* Enable trace */
		CM3_DCB->demcr |= DCB_DEMCR_TRCENA;
		/* Enable cycle counter */
		CM3_DWT->ctrl |= DWT_CTRL_CYCCNTENA;

		/* set the reference to now */
		thinkos_rt.cycref = CM3_DWT->cyccnt;
	}
#endif

#if (THINKOS_MUTEX_MAX > 0)
	DCC_LOG3(LOG_TRACE, "    mutex: %2d (%2d .. %2d)", THINKOS_MUTEX_MAX,
			 THINKOS_MUTEX_BASE,
			 THINKOS_MUTEX_BASE + THINKOS_MUTEX_MAX - 1);
#endif
#if (THINKOS_COND_MAX > 0)
	DCC_LOG3(LOG_TRACE, "  condvar: %2d (%2d .. %2d)", THINKOS_COND_MAX,
			 THINKOS_COND_BASE,
			 THINKOS_COND_BASE + THINKOS_COND_MAX - 1);
#endif
#if (THINKOS_SEMAPHORE_MAX > 0)
	DCC_LOG3(LOG_TRACE, "semaphore: %2d (%2d .. %2d)", THINKOS_SEMAPHORE_MAX,
			 THINKOS_SEM_BASE,
			 THINKOS_SEM_BASE + THINKOS_SEMAPHORE_MAX - 1);
#endif
#if (THINKOS_EVENT_MAX > 0)
	DCC_LOG3(LOG_TRACE, "    evset: %2d (%2d .. %2d)", THINKOS_EVENT_MAX,
			 THINKOS_EVENT_BASE,
			 THINKOS_EVENT_BASE + THINKOS_EVENT_MAX - 1);
#endif
#if (THINKOS_FLAG_MAX > 0)
	DCC_LOG3(LOG_TRACE, "     flag: %2d (%2d .. %2d)", THINKOS_FLAG_MAX,
			 THINKOS_FLAG_BASE,
			 THINKOS_FLAG_BASE + THINKOS_FLAG_MAX - 1);
#endif
#if THINKOS_ENABLE_JOIN
	DCC_LOG3(LOG_TRACE, "     join: %2d (%2d .. %2d)", THINKOS_THREADS_MAX,
			 THINKOS_JOIN_BASE,
			 THINKOS_JOIN_BASE + THINKOS_THREADS_MAX- 1);
#endif

	DCC_LOG(LOG_TRACE, "enabling interrupts!");

	/* enable interrupts */
	cm3_cpsie_i();

	DCC_LOG4(LOG_TRACE, "<%d> msp=%08x psp=%08x ctrl=%08x", 
			 self, cm3_msp_get(), cm3_psp_get(), cm3_control_get());

	return self;
}

#if THINKOS_ENABLE_THREAD_ALLOC | THINKOS_ENABLE_MUTEX_ALLOC | \
	THINKOS_ENABLE_COND_ALLOC | THINKOS_ENABLE_SEM_ALLOC | \
	THINKOS_ENABLE_EVENT_ALLOC | THINKOS_ENABLE_FLAG_ALLOC
int thinkos_bmp_alloc(uint32_t bmp[], int bits) 
{
	int i;
	int j;

	for (i = 0; i < ((bits + 31) / 32); ++i) {
		/* Look for an empty bit MSB first */
		if ((j = __clz(__rbit(~(bmp[i])))) < 32) {
			/* Mark as used */
			__bit_mem_wr(&bmp[i], j, 1);  
			return 32 * i + j;;
		}
	}
	return -1;
}
#endif

const char * const thinkos_svc_link = thinkos_svc_nm;
const char * const thinkos_nmic_link = thinkos_nmi_nm;

