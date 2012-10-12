/* 
 * File:	 dac-test.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2011 Bob Mittmann. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */


#include <sys/stm32f.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arch/cortex-m3.h>
#include <sys/serial.h>
#include <sys/delay.h>

#include <cm3rtos_svc.h>
#define __CM3RTOS_SYS__
#include <cm3rtos_sys.h>

#define CM3RTOS_IDLE_IDX 32 

#define ENABLE_SYSTICK_SCHED 0
#define CM3_IRQ_MAX 80


/* -------------------------------------------------------------------------- 
 * Run Time RTOS block
 * --------------------------------------------------------------------------*/

struct cm3rtos_rt cm3rtos_rt;

#if 0
/* -------------------------------------------------------------------------- 
 * Main stack - used for exception handling
 * --------------------------------------------------------------------------*/
uint64_t cm3rtos_except_stack[32];

/* -------------------------------------------------------------------------- 
 * Idle thread context strucure
 * --------------------------------------------------------------------------*/
struct cm3rtos_idle cm3rtos_idle;

#else

/* This is a trick to save memory. We need an area to store
   the Idle thread context, but the Idle thread only uses 4 entries:
   xpsr, pc, lr, r12. The other registers are not used at any time. We 
   claim the space avalibale for this registers as part of the exception 
   stack. */
struct cm3rtos_except_and_idle cm3rtos_idle;
#endif

void __attribute__((noreturn, naked)) cm3rtos_idle_task(void)
{
	for (;;) {
		asm volatile ("ldr  r12, [lr, #0]\n"); /* update the snapshot value */
#if CM3RTOS_ENABLE_WFI
		asm volatile ("wfi\n"); /* wait for interrupt */
#endif
	}
}

static inline int __cm3rtos_sched(void)
{
	int idx;
	int act;

	/* get a thread from the ready bitmap */
	idx = cm3_clz(cm3_rbit(cm3rtos_rt.wq_ready));
	/* this will ensure the MSB is 0 */
	act = idx << 1;
	/* clear the corresponding bit */
	bmp_bit_wr(&cm3rtos_rt.wq_ready, idx, act);  
	/* update the current thread and schedule bit */
	cm3rtos_rt.active = act + 1;
	return act;
}

void __attribute__((noreturn, naked, aligned(16))) cm3_pendsv_isr(void)
{
	register struct cm3rtos_context * ctx asm("r0");
	int act;
	int idx;

	/* save the context */
	asm volatile ("mrs   %0, PSP\n" 
				  "stmdb %0!, {r4-r11}\n" : "=r" (ctx));
	act = cm3rtos_rt.active;
	idx = act >> 1;
	cm3rtos_rt.ctx[idx] = ctx;

	bmp_bit_wr(&cm3rtos_rt.wq_ready, idx, act);  

	act = __cm3rtos_sched();
	idx = act >> 1;

#if 0
	printf("%c", (idx == 32) ? '^' : idx + '0');
	if (idx == 32)
		printf("+");
#endif

	/* restore the context */
	ctx = cm3rtos_rt.ctx[idx];
	asm volatile ( "add    r2, %0, #(8 * 4)\n"
				   "msr    PSP, r2\n"
				   "ldmia  %0, {r4-r11}\n"
				   "bx     lr\n" : : "r" (ctx));
	for(;;);
}

#if ENABLE_SYSTICK_SCHED
void __attribute__((noreturn, naked, aligned(16))) cm3_systick_isr(void)
#else
void __attribute__((aligned(16))) cm3_systick_isr(void)
#endif
{
	int idx;
#if ENABLE_SYSTICK_SCHED
	register struct cm3rtos_context * ctx asm("r0");
	/* save the context */
	asm volatile ("mrs   %0, PSP\n" 
				  "stmdb %0!, {r4-r11}\n" : "=r" (ctx));
	idx = cm3rtos_rt.active >> 1;
	cm3rtos_rt.ctx[idx] = ctx;
#else
	idx = cm3rtos_rt.active >> 1;
#endif

#if CM3RTOS_ENABLE_CLOCK
	uint32_t ticks;
	int j;

	ticks = cm3rtos_rt.ticks; 
	ticks++;
	cm3rtos_rt.ticks = ticks; 

#if 0
	uint32_t wq;

	wq = cm3_rbit(cm3rtos_rt.wq_clock);
	while ((j = cm3_clz(wq)) < 32) {
		wq &= ~(0x80000000 >> j);  
		if ((int32_t)(cm3rtos_rt.clock[j] - ticks) <= 0) {
			bmp_bit_clr(&cm3rtos_rt.wq_clock, j);  
			bmp_bit_set(&cm3rtos_rt.wq_ready, j);  
//			printf("^%d", j);
		}
	}
#else
	int stat;

	for (j = 0; (stat = cm3rtos_rt.th_stat[j]) & 1; j++) {
		if ((int32_t)(cm3rtos_rt.clock[j] - ticks) <= 0) {
			cm3rtos_rt.th_stat[j] = 0;
			bmp_bit_clr(&cm3rtos_rt.q_lst[stat >> 1], j);  
			bmp_bit_set(&cm3rtos_rt.wq_ready, j);  
//			printf("^%d", j);
		}
#endif
	}
#endif

#if CM3RTOS_ENABLE_TIMESHARE
	if (idx != CM3RTOS_IDLE_IDX) {
		cm3rtos_rt.sched_val[idx] -= cm3rtos_rt.sched_pri[idx];
		if (cm3rtos_rt.sched_val[idx] < 0) {
			cm3rtos_rt.sched_val[idx] += cm3rtos_rt.sched_limit;
			/* insert into the CPU wait queue */
			bmp_bit_set(&cm3rtos_rt.wq_sched, idx);  
//			printf(" w%x", cm3rtos_rt.wq_sched);
		} else {
			bmp_bit_set(&cm3rtos_rt.wq_ready, idx);
//			printf(" r%x", cm3rtos_rt.wq_ready);
		}
	}

	if (cm3rtos_rt.wq_ready == 0) {
		cm3rtos_rt.wq_ready = cm3rtos_rt.wq_sched;
		cm3rtos_rt.wq_sched = 0;
//		printf(" R%x", cm3rtos_rt.wq_ready);
	}
#endif

//	printf(".%d", idx);

#if ENABLE_SYSTICK_SCHED
	idx = __cm3rtos_sched() >> 1;
	/* restore the context */
	ctx = cm3rtos_rt.ctx[idx];
	asm volatile ( "add    r2, %0, #(8 * 4)\n"
				   "msr    PSP, r2\n"
				   "ldmia  %0, {r4-r11}\n"
				   "mov    r0, #0xfffffffd\n"
				   "bx     r0\n" : : "r" (ctx));
	for(;;);
#else
	bmp_bit_wr(&cm3rtos_rt.active, 0, cm3rtos_rt.wq_ready >> idx);
	__cm3rtos_defer_sched();
#endif
}

void __attribute__((noreturn)) cm3rtos_thread_exit(int code)
{
	int idx = cm3rtos_rt.active >> 1;
	int j;

	cm3_cpsid_i();

#if 0
	printf("%s(): <%d> code=%d\n", __func__, idx, code); 
#endif

#if CM3RTOS_ENABLE_THREAD_ALLOC
	/* Releases the thread block */
	bmp_bit_clr(&cm3rtos_rt.th_alloc, idx);
#endif

	for (j = 0; j < CM3RTOS_THREADS_MAX; j++) {
#if CM3RTOS_ENABLE_THREAD_ALLOC
		if (bmp_bit_rd(&cm3rtos_rt.th_alloc, j) == 0)
			continue;
#endif

#if CM3RTOS_ENABLE_TIMESHARE
		/* schedule limit reevaluation */
		if (cm3rtos_rt.sched_limit < cm3rtos_rt.sched_pri[j]) {
			cm3rtos_rt.sched_limit = cm3rtos_rt.sched_pri[j];
		}
#endif

#if CM3RTOS_ENABLE_JOIN
		if (bmp_bit_rd(&cm3rtos_rt.wq_join[idx], j) != 0) {
#if 0
			printf("%s(): wakeup <%d>\n", __func__, j);
#endif
			bmp_bit_set((void *)&cm3rtos_rt.wq_ready, j);  
			bmp_bit_clr((void *)&cm3rtos_rt.wq_join[idx], j);  
			cm3rtos_rt.ctx[j]->r0 = code;
		}
#endif
	}
#if !CM3RTOS_ENABLE_THREAD_ALLOC && CM3RTOS_ENABLE_TIMESHARE
	/* clear the schedule priority. In case the thread allocations
	 is disabled, the schedule limit reevalution may produce inconsistent
	 results ... */
	cm3rtos_rt.sched_pri[idx] = 0;
#endif
	/* wait forever */
	__cm3rtos_wait();
	cm3_cpsie_i();

	for(;;);

	idx = idx;
}

int cm3rtos_init(struct cm3rtos_thread_opt opt)
{
	struct cm3_systick * systick = CM3_SYSTICK;
	int idx;
	int irq;

	/* disable interrupts */
	cm3_cpsid_i();

	/* adjust exception priorities */

	/* SVC must not be preempted thus it runs at highest possible priority */
	cm3_except_pri_set(CM3_EXCEPT_SVC, 0);
	/* SysTick interrupt has to have the same priority as PendSV  */
	cm3_except_pri_set(CM3_EXCEPT_SYSTICK, 128);
	/* PendSV interrupt has to have the lowest priority (higher number) */
	cm3_except_pri_set(CM3_EXCEPT_PENDSV, 128);

	/* adjust IRQ priorities */
	for (irq = 0; irq < CM3_IRQ_MAX; irq++)
		cm3_irq_pri_set(irq, 128);

	/* configure the thread stack */
	cm3_psp_set(cm3_sp_get());
	/* configure the main stack */
#if 0
	cm3_msp_set((uint32_t)&cm3rtos_except_stack + 
				sizeof(cm3rtos_except_stack));
#endif
	cm3_msp_set((uint32_t)&cm3rtos_idle.snapshot.val);
	/* configure to use of PSP in thread mode */
	cm3_control_set(CONTROL_THREAD_PSP | CONTROL_THREAD_PRIV);

	/* initialize the idle thread */
	cm3rtos_rt.idle_ctx = &cm3rtos_idle.ctx;
	cm3rtos_idle.ctx.pc = (uint32_t)cm3rtos_idle_task,
	cm3rtos_idle.ctx.xpsr = 0x01000000;
#if CM3RTOS_ENABLE_CLOCK
	cm3rtos_idle.snapshot.ptr = &cm3rtos_rt.ticks;
#else
	cm3rtos_idle.snapshot.ptr = (void *)&cm3rtos_rt.active;
#endif
	cm3rtos_idle.snapshot.val = 0;

#if CM3RTOS_ENABLE_MUTEX_ALLOC
	/* initialize the thread allocation bitmap */ 
	cm3rtos_rt.mutex_alloc = 0xffffffff << CM3RTOS_MUTEX_MAX;
#endif

	/* initialize the main thread */ 
	/* alloc main thread */
	if (opt.id >= CM3RTOS_THREADS_MAX)
		opt.id = CM3RTOS_THREADS_MAX - 1;
#if CM3RTOS_ENABLE_THREAD_ALLOC
	/* initialize the thread allocation bitmap */ 
	cm3rtos_rt.th_alloc = 0xffffffff << CM3RTOS_THREADS_MAX;

	idx = cm3rtos_alloc_lo(&cm3rtos_rt.th_alloc, opt.id);
	printf("%s(): alloc lo: %d -> %d\n", __func__, opt.id, idx);
#else
	idx = opt.id;
#endif

#if CM3RTOS_ENABLE_TIMESHARE

#if CM3RTOS_SCHED_LIMIT_MIN < 1
#error "CM3RTOS_SCHED_LIMIT_MIN must be at least 1"
#endif

#if CM3RTOS_SCHED_LIMIT_MAX < CM3RTOS_SCHED_LIMIT_MIN
#error "CM3RTOS_SCHED_LIMIT_MAX < CM3RTOS_SCHED_LIMIT_MIN !!!"
#endif

	if (opt.priority > CM3RTOS_SCHED_LIMIT_MAX)
		opt.priority = CM3RTOS_SCHED_LIMIT_MAX;

	cm3rtos_rt.sched_pri[idx] = opt.priority;
	cm3rtos_rt.sched_val[idx] = opt.priority / 2;

	/* set the initial schedule limit */
	cm3rtos_rt.sched_limit = opt.priority;
	if (cm3rtos_rt.sched_limit < CM3RTOS_SCHED_LIMIT_MIN)
		cm3rtos_rt.sched_limit = CM3RTOS_SCHED_LIMIT_MIN;
#endif /* CM3RTOS_ENABLE_TIMESHARE */

	/* set the active thread */
	cm3rtos_rt.active = (idx << 1);

	/* initialize the SysTick module */
	systick->load = (CM3_SYSTICK_CLK_HZ / 1000) - 1; /* 1ms tick period */
	systick->val = 0;
	systick->ctrl = SYSTICK_CTRL_ENABLE | SYSTICK_CTRL_TICKINT;

#if CM3RTOS_ENABLE_PAUSE
	if (opt.f_paused) {
		/* insert into the paused list */
		bmp_bit_set(&cm3rtos_rt.wq_paused, idx);  
		cm3rtos_rt.th_stat[idx] = __cm3rtos_wq_idx(&cm3rtos_rt.wq_paused);
		/* invoque the scheduler */
		__cm3rtos_defer_sched();
	} else
#endif
		/* set the active thread schedule flag */
		bmp_bit_set(&cm3rtos_rt.active, 0);  

	/* enable interrupts */
	cm3_cpsie_i();

	return idx;
}

