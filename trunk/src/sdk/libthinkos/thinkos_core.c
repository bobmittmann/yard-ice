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


#define __THINKOS_SYS__
#include <thinkos_sys.h>
#define __THINKOS_IRQ__
#include <thinkos_irq.h>

#include <thinkos_svc.h>
#include <thinkos_except.h>

#include <stdio.h>
#include <string.h>

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

#if THINKOS_ENABLE_IDLE_SNAPSHOT
uint32_t thinkos_idle_val(void)
{
	return thinkos_idle.snapshot.val;
}
#endif

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
#endif

void __attribute__((noreturn, naked)) thinkos_idle_task(void)
{
	for (;;) {
#if THINKOS_ENABLE_SCHED_DEBUG
		DCC_LOG(LOG_MSG, "zzz...");
//		__dump_context(&thinkos_idle.ctx);
#endif
#if THINKOS_ENABLE_IDLE_SNAPSHOT
		asm volatile ("ldr  r12, [lr, #0]\n"); /* update the snapshot value */
#endif
#if THINKOS_ENABLE_IDLE_WFI
		asm volatile ("wfi\n"); /* wait for interrupt */
#endif
	}
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

static inline void __attribute__((always_inline)) __wait(void) {
	asm volatile ("mov    r3, #1\n"
				  "0:\n"
				  "cbz	r3, 1f\n"
				  "b.n  0b\n"
				  "1:\n" : : : "r3"); 
}

#include <sys/delay.h>

int sched_busy = 0;

/* THinkOS - scheduler */
void __attribute__((naked, aligned(16))) cm3_pendsv_isr(void)
{
	struct thinkos_context * ctx;
	uint32_t idx;

//	sched_busy = 1;

//	DCC_LOG(LOG_TRACE, "...");

	/* save the context */
	ctx = __sched_entry();

//	udelay(2000);

	/* get the active thread and schedule bit */	
	idx = thinkos_rt.active;

	/* save SP */
	thinkos_rt.ctx[idx] = ctx;

	/* get a thread from the ready bitmap */
	idx = __clz(__rbit(thinkos_rt.wq_ready));

	/* update the current thread */
	thinkos_rt.active = idx;

	ctx = thinkos_rt.ctx[idx];

#if THINKOS_ENABLE_SCHED_DEBUG
	if (thinkos_rt.sched_trace_req) {
//		DCC_LOG1(LOG_TRACE, "active=%d", idx);
//		DCC_LOG1(LOG_TRACE, "sp=%08x", cm3_sp_get());
//		__dump_context(ctx);
//		__wait();
		thinkos_rt.sched_trace_req = 0;
	}
#endif

//	sched_busy = 0;

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

//	if (sched_busy)
//		__wait();

	ticks = thinkos_rt.ticks; 
	ticks++;
	thinkos_rt.ticks = ticks; 

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
			bmp_bit_clr(&thinkos_rt.wq_lst[stat >> 1], j);  
#endif
			/* remove from the time wait queue */
			bmp_bit_clr(&thinkos_rt.wq_clock, j);  
			DCC_LOG1(LOG_MSG, "Wakeup %d...", j);
#if THINKOS_ENABLE_SCHED_DEBUG
			thinkos_rt.sched_trace_req = 1;
#endif
			/* insert into the ready wait queue */
			bmp_bit_set(&thinkos_rt.wq_ready, j);  
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
//			bmp_bit_set(&thinkos_rt.active.flags, THINKOS_NON_SCHED);  
			/* insert into the CPU wait queue */
			bmp_bit_set(&thinkos_rt.wq_tmshare, idx);  
//			printf(" w%x", thinkos_rt.wq_tmshare);
			bmp_bit_clr(&thinkos_rt.wq_ready, idx);
			cm3_cpsid_i();
			if (thinkos_rt.wq_ready == 0) {
				/* no more threads into the ready queue,
				 move the timeshare queue to the ready queue */
				thinkos_rt.wq_ready = thinkos_rt.wq_tmshare;
				thinkos_rt.wq_tmshare = 0;
			} 
			cm3_cpsie_i();
			sched++;
		} else {
//			bmp_bit_set(&thinkos_rt.wq_ready, idx);
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
	int idx = thinkos_rt.active;
	int j;

	DCC_LOG2(LOG_TRACE, "<%d> code=%d", idx, code); 

#if THINKOS_ENABLE_TIMESHARE
	/* possibly remove from the time share wait queue */
	bmp_bit_clr(&thinkos_rt.wq_tmshare, idx);  
#endif

	/* disable interrupts */
	cm3_cpsid_i();

#ifdef THINKOS_ENABLE_CANCEL
#if THINKOS_ENABLE_THREAD_STAT
	int stat;
	/* update the thread status */
	stat = thinkos_rt.th_stat[idx];
	thinkos_rt.th_stat[idx] = 0;
	/* remove from other wait queue, if any */
	bmp_bit_clr(&thinkos_rt.wq_lst[stat >> 1], idx);  
#else
	int i;
	/* remove from other wait queue, if any */
	for (i = 0; i < __wq_idx(thinkos_rt.wq_end); i++) {
		bmp_bit_clr(&thinkos_rt.wq_lst[i], idx);  
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
		if (__bit_mem_rd(&thinkos_rt.wq_join[idx], j) != 0) {
			DCC_LOG1(LOG_TRACE, "wakeup <%d>", j);
			bmp_bit_set((void *)&thinkos_rt.wq_ready, j);  
			bmp_bit_clr((void *)&thinkos_rt.wq_join[idx], j);  
			thinkos_rt.ctx[j]->r0 = code;
		}
#endif
	}

#if !THINKOS_ENABLE_THREAD_ALLOC && THINKOS_ENABLE_TIMESHARE
	/* clear the schedule priority. In case the thread allocations
	 is disabled, the schedule limit reevaluation may produce inconsistent
	 results ... */
	thinkos_rt.sched_pri[idx] = 0;
#endif

#if THINKOS_ENABLE_THREAD_ALLOC
	/* Releases the thread block */
	bmp_bit_clr(&thinkos_rt.th_alloc, idx);
#endif

	/* FIXME: clear context. The way is implemented th scheduler will 
	   override this value...  */
	thinkos_rt.ctx[idx] = 0;

	/* wait forever */
	__thinkos_wait();
	cm3_cpsie_i();

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
	msp = (uint32_t)&thinkos_idle.stack.r12;
	cm3_msp_set(msp);

	DCC_LOG2(LOG_TRACE, "msp=0x%08x idle=0x%08x", msp, &thinkos_idle);

	/* configure the use of PSP in thread mode */
	cm3_control_set(CONTROL_THREAD_PSP | CONTROL_THREAD_PRIV);

	/* initialize exception stack */
	for (i = 0; i < (THINKOS_EXCEPT_STACK_SIZE / 4 - IDLE_UNUSED_REGS); ++i)
		thinkos_idle.except_stack[i] = 0xfacade44;

	/* initialize the idle thread */
	thinkos_rt.idle_ctx = &thinkos_idle.ctx;
#if THINKOS_ENABLE_TIMESHARE
	thinkos_rt.sched_idle_val = 0;
	thinkos_rt.sched_idle_pri = 0;
#endif
//	thinkos_idle.ctx.ret = CM3_EXC_RET_THREAD_PSP;


	thinkos_idle.ctx.pc = (uint32_t)thinkos_idle_task,
	thinkos_idle.ctx.xpsr = 0x01000000;
#if THINKOS_ENABLE_CLOCK
	thinkos_idle.snapshot.ptr = &thinkos_rt.ticks;
#else
	thinkos_idle.snapshot.ptr = (void *)&thinkos_rt.active;
#endif
	thinkos_idle.snapshot.val = 0;

#if THINKOS_ENABLE_MUTEX_ALLOC
	/* initialize the thread allocation bitmap */ 
	thinkos_rt.mutex_alloc = (uint32_t)(0xffffffffLL << THINKOS_MUTEX_MAX);
#endif

#if THINKOS_ENABLE_SEM_ALLOC
	/* initialize the semaphore allocation bitmap */ 
	thinkos_rt.sem_alloc = (uint32_t)(0xffffffffLL << THINKOS_SEMAPHORE_MAX);
#endif

#if (THINKOS_MUTEX_MAX > 0)
	{
		int i;
		/* initialize the mutex locks */
		for (i = 0; i < THINKOS_MUTEX_MAX; i++) 
			thinkos_rt.lock[i] = -1;
	}
#endif

	/* initialize the main thread */ 
	/* alloc main thread */
	if (opt.id >= THINKOS_THREADS_MAX)
		opt.id = THINKOS_THREADS_MAX - 1;
#if THINKOS_ENABLE_THREAD_ALLOC
	/* initialize the thread allocation bitmap */ 
	thinkos_rt.th_alloc = (uint32_t)(0xffffffffLL << THINKOS_THREADS_MAX);
	self = thinkos_alloc_lo(&thinkos_rt.th_alloc, opt.id);
#else
	self = opt.id;
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
	bmp_bit_set(&thinkos_rt.wq_ready, self);

	/* initialize the SysTick module */
	systick->load = cm3_systick_load_1ms; /* 1ms tick period */
	systick->val = 0;
#if THINKOS_ENABLE_CLOCK || THINKOS_ENABLE_TIMESHARE
	systick->ctrl = SYSTICK_CTRL_ENABLE | SYSTICK_CTRL_TICKINT;
#else
	systick->ctrl = SYSTICK_CTRL_ENABLE;
#endif

#if THINKOS_ENABLE_PAUSE
	if (opt.f_paused) {
		/* insert into the paused list */
		bmp_bit_set(&thinkos_rt.wq_paused, self);  
		/* Invoke the scheduler */
		__thinkos_defer_sched();
	} else
#endif

	DCC_LOG(LOG_TRACE, "enabling interrupts!");

	/* enable interrupts */
	cm3_cpsie_i();

	DCC_LOG4(LOG_TRACE, "<%d> msp=%08x psp=%08x ctrl=%08x", 
			 self, cm3_msp_get(), cm3_psp_get(), cm3_control_get());

	return self;
}

const char * const thinkos_svc_link = thinkos_svc_nm;

#if THINKOS_ENABLE_EXCEPTIONS
const char * const thinkos_execpt_link = thinkos_except_nm;
#endif

