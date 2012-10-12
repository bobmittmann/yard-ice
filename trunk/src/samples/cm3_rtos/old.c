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

#include <cm3rtos.h>
#define __CM3RTOS_SYS__
#include <cm3rtos_sys.h>

/* -------------------------------------------------------------------------- 
 * Run Time RTOS block cancelled  
 * --------------------------------------------------------------------------*/

struct cm3rtos_rt {
	/* Pointer to the active (current) thread */
	struct cm3rtos_context * ctx[32];
	struct cm3rtos_context * idle_ctx;
	uint32_t ready;
	int32_t a;
	int32_t this;
	uint32_t cpu_wait;
	int32_t sched_limit;
	int32_t sched_pri[CM3RTOS_THREADS_MAX];
	int32_t sched_val[CM3RTOS_THREADS_MAX];
	uint32_t wq_alloc;
	uint32_t wq[CM3RTOS_WAIT_QUEUE_MAX];
	uint32_t wq_sleep;
	uint32_t wq_join[CM3RTOS_THREADS_MAX];
	uint32_t th_alloc;
	struct cm3rtos_thread * active;
	uint32_t paused;
	uint32_t canceled;
};

struct cm3rtos_rt __attribute((section(".ram_vectors"))) cm3rtos_rt;

//const uint32_t test = 0x22000000 + ((((uint32_t)&cm3rtos_rt.ready) - 0x40000000) * 32);

/* -------------------------------------------------------------------------- 
 * Main stack - used for exception handling
 * --------------------------------------------------------------------------*/
uint64_t cm3rtos_except_stack[32];

void cm3rtos_wq_insert(unsigned int wq_id, unsigned int th_id)
{
	bmp_bit_set(&cm3rtos_rt.wq[wq_id], th_id);  
}

void cm3rtos_wq_remove(unsigned int wq_id, unsigned int th_id)
{
	bmp_bit_clr(&cm3rtos_rt.wq[wq_id], th_id);  
}

int cm3rtos_wq_get_head(unsigned int wq_id)
{
	return bmp_ffs32(&cm3rtos_rt.wq[wq_id]);
}

void cm3rtos_signal(unsigned int wq_id)
{
	int idx;

	idx = cm3_clz(cm3_rbit(cm3rtos_rt.wq[wq_id]));
	bmp_bit_set(&cm3rtos_rt.wq[wq_id], idx);
	cm3rtos_defer_sched();
}

static inline int __cm3rtos_sched(void)
{
	int idx;

	/* get a thread from the ready bitmap */
	idx = cm3_clz(cm3_rbit(cm3rtos_rt.ready));
//	idx = cm3_clz(cm3rtos_rt.ready);
//	if (idx != CM3RTOS_IDLE_ID)
	bmp_bit_clr((void *)&cm3rtos_rt.ready, idx);  
	cm3rtos_rt.this = idx;
	return idx;
}

void __attribute__((noreturn, naked, aligned(16))) cm3_pendsv_isr(void)
{
	register struct cm3rtos_context * ctx asm("r0");
	int idx;

	/* save the context */
	asm volatile ("mrs  %0, PSP\n" 
				  "stmdb %0!, {r4-r11, lr}\n" : "=r" (ctx));
	idx = cm3rtos_rt.this;
	cm3rtos_rt.ctx[idx] = ctx;

	bmp_bit_set((void *)&cm3rtos_rt.ready, idx);  

	idx = __cm3rtos_sched();

/*	if (idx == 32)
		printf("~");
	else
		printf("%c", idx + '0'); */

	/* restore the context */
	ctx = cm3rtos_rt.ctx[idx];
	asm volatile ( "add    r2, %0, #(8 * 4)\n"
				   "msr    PSP, r2\n"
				   "ldmia	%0, {r4-r11, pc}\n" : : "r" (ctx));
	for(;;);
}

void __attribute__((noreturn, naked, aligned(16))) cm3_nmi_isr(void)
{
	register struct cm3rtos_context * ctx asm("r0");
	int idx;

	/* save context */
	asm volatile ("mrs  %0, PSP\n" 
				  "stmdb %0!, {r4-r11, lr}\n" : "=r" (ctx));
	idx = cm3rtos_rt.this;
	cm3rtos_rt.ctx[idx] = ctx;

	idx = __cm3rtos_sched();

/*	if (idx == 32)
		printf("~");
	else
		printf("%c", idx + '0'); */

	/* restore the context */
	ctx = cm3rtos_rt.ctx[idx];
	asm volatile ( "add    r2, %0, #(9 * 4)\n"
				   "msr    PSP, r2\n"
				   "ldmia	%0, {r4-r11, pc}\n" : : "r" (ctx));
	for(;;);
}

void __attribute__((noreturn, naked, aligned(16))) cm3_systick_isr(void)
{
	register struct cm3rtos_context * ctx asm("r0");
	uint32_t wq;
	int idx;
	int j;

	/* save the context */
	asm volatile ("mrs  %0, PSP\n" 
				  "stmdb %0!, {r4-r11, lr}\n" : "=r" (ctx));
	idx = cm3rtos_rt.this;
	cm3rtos_rt.ctx[idx] = ctx;

	wq = cm3_rbit(cm3rtos_rt.wq_sleep);
	while ((j = cm3_clz(wq)) < 32) {
		wq &= ~(0x80000000 >> j);  
		if (--cm3rtos_rt.sched_val[j] <= 0) {
			bmp_bit_clr((void *)&cm3rtos_rt.wq_sleep, j);  
			bmp_bit_set((void *)&cm3rtos_rt.ready, j);  
		}
	}

	/* insert active thread into READY queue */

//	th = cm3rtos_rt.active;
//	if (th != &cm3rtos_idle_thread) {

	if (idx != CM3RTOS_IDLE_ID) {
//		idx = th->idx;
		cm3rtos_rt.sched_val[idx] -= cm3rtos_rt.sched_pri[idx];
		if (cm3rtos_rt.sched_val[idx] < 0) {
			cm3rtos_rt.sched_val[idx] += cm3rtos_rt.sched_limit;
			/* insert into the CPU wait queue */
			bmp_bit_set((void *)&cm3rtos_rt.cpu_wait, idx);  
	//		printf("?%x", cm3rtos_rt.ready);
		} else {
			bmp_bit_set((void *)&cm3rtos_rt.ready, idx);
	//		printf(">%x", cm3rtos_rt.ready);
		}
	}

	if (cm3rtos_rt.ready == 0) {
		cm3rtos_rt.ready = cm3rtos_rt.cpu_wait;
		cm3rtos_rt.cpu_wait = 0;
	}

//	cm3rtos_defer_sched();
	idx = __cm3rtos_sched();

#if 0
	if (idx == CM3RTOS_IDLE_ID)
		printf("^");
	else
		printf("%c", idx + '0');
#endif

	/* restore the context */
	ctx = cm3rtos_rt.ctx[idx];
	asm volatile ( "add    r2, %0, #(9 * 4)\n"
				   "msr    PSP, r2\n"
				   "ldmia	%0, {r4-r11, pc}\n" : : "r" (ctx));
	for(;;);
}

static void __attribute__((noreturn)) cm3rtos_thread_exit(int code)
{
//	struct cm3rtos_thread * this;
	int idx;
	int j;

	idx = cm3rtos_rt.this;

	cm3_cpsid_i();

#if 0
	printf("%s(): <%d> th=%08x code=%d\n", 
		   __func__, idx, (int)cm3rtos_rt.th_lst[idx], code); 
#endif

	/* Releases the thread block */
	bmp_bit_clr(&cm3rtos_rt.th_alloc, idx);

	for (j = 0; j < CM3RTOS_THREADS_MAX; j++) {
		if (bmp_bit_rd(&cm3rtos_rt.th_alloc, j) == 0)
			continue;

		if (cm3rtos_rt.sched_limit < cm3rtos_rt.sched_pri[j]) {
			cm3rtos_rt.sched_limit = cm3rtos_rt.sched_pri[j];
		}

		if (bmp_bit_rd(&cm3rtos_rt.wq_join[idx], j) != 0) {
#if 0
			printf("%s(): wakeup <%d>\n", __func__, j);
#endif
			bmp_bit_set((void *)&cm3rtos_rt.ready, j);  
			bmp_bit_clr((void *)&cm3rtos_rt.wq_join[idx], j);  
			cm3rtos_rt.ctx[j]->r0 = code;
//			th->sp->r0 = code;
		}
	}
	cm3rtos_sched();
	cm3_cpsie_i();
	for(;;);
}

int cm3rtos_join_svc(unsigned int idx)
{
	int ret;

	if (bmp_bit_rd(&cm3rtos_rt.th_alloc, idx) == 0) {
#if 0
		printf("%s(): <%d> thread=%d failed!\n", __func__, 
			   cm3rtos_rt.this, idx);
#endif
		ret = -1;
	} else {
#if 0
		printf("%s(): <%d> thread=%d waiting...\n", __func__, 
			   cm3rtos_rt.this, idx);
#endif
		bmp_bit_set(&cm3rtos_rt.wq_join[idx], cm3rtos_rt.this);  
		if (bmp_bit_rd(&cm3rtos_rt.canceled, idx)) {
			bmp_bit_clr(&cm3rtos_rt.canceled, idx);  
			bmp_bit_set((void *)&cm3rtos_rt.ready, idx);  
		}
		cm3rtos_sched();
	}

	return ret;
}

int cm3rtos_cancel_svc(unsigned int idx, int code)
{
//	struct cm3rtos_thread * th;
	int ret;

	if (bmp_bit_rd(&cm3rtos_rt.th_alloc, idx) == 0) {
		printf("%s(): <%d> cancel %d failed, not alloc!\n", 
			   __func__, cm3rtos_rt.this, idx); 
		ret = -1;
	} else {
		bmp_bit_clr((void *)&cm3rtos_rt.cpu_wait, idx);  
		bmp_bit_clr((void *)&cm3rtos_rt.wq_sleep, idx);  
		bmp_bit_clr((void *)&cm3rtos_rt.ready, idx);  
#if 0
		printf("%s(): <%d> cancel %d, with code %d!\n", 
			   __func__, cm3rtos_rt.this, idx, code); 
#endif
		cm3rtos_rt.ctx[idx]->pc = (uint32_t)cm3rtos_thread_exit;
		cm3rtos_rt.ctx[idx]->r0 = code;

		bmp_bit_set(&cm3rtos_rt.canceled, idx);  
		ret = 0;
	}

	return ret;
}

void cm3rtos_sleep_svc(unsigned int ms)
{
	cm3rtos_rt.sched_val[cm3rtos_rt.this] = (ms + 1) / 100;
	/* insert into the sleep wait queue */
	bmp_bit_set(&cm3rtos_rt.wq_sleep, cm3rtos_rt.this);  
	cm3rtos_sched();
}

/* initialize a thread context */
int cm3rtos_thread_create_svc(struct cm3rtos_thread_init * init)
{
	struct cm3rtos_context * ctx;
	uint32_t sp;
	int idx;

	/* Alloc a thread */
	if (init->priority == 0) 
		idx = cm3rtos_alloc_lo(&cm3rtos_rt.th_alloc);
	else
		idx = cm3rtos_alloc_hi(&cm3rtos_rt.th_alloc);

	if (idx < 0) {
#if 0
		printf("%s(): error!\n", __func__);
#endif
		return idx;
	}

	sp = (uint32_t)init->stack_ptr + init->stack_size;
	sp &= 0xfffffff8; /* 64bits alignemnt */
	sp -= sizeof(struct cm3rtos_context);

	ctx = (struct cm3rtos_context *)sp;

	ctx->r4 = 0xdecaf004;
	ctx->r5 = 0xdecaf005;
	ctx->r6 = 0xdecaf006;
	ctx->r7 = 0xdecaf007;
	ctx->r8 = 0xdecaf008;
	ctx->r9 = 0xdecaf009;
	ctx->r10 = 0xdecaf00a;
	ctx->r11 = 0xdecaf00b;
	ctx->exc_return = 0xfffffffd;
	ctx->r0 = (uint32_t)init->arg;
	ctx->r1 = idx;
	ctx->r2 = 0xdecaf002;
	ctx->r3 = 0xdecaf003;
	ctx->r12 = 0xdecaf00c;
	ctx->lr = (uint32_t)cm3rtos_thread_exit;
	ctx->pc = (uint32_t)init->task;
	ctx->xpsr = 0x01000000;

	cm3rtos_rt.ctx[idx] = ctx;
	cm3rtos_rt.sched_pri[idx] = init->priority;

	/* update schedule limit */
	if (cm3rtos_rt.sched_limit < cm3rtos_rt.sched_pri[idx]) {
		cm3rtos_rt.sched_limit = cm3rtos_rt.sched_pri[idx];
	}

	cm3rtos_rt.sched_val[idx] = cm3rtos_rt.sched_limit / 2;

	/* insert into the ready list */
	bmp_bit_set((void *)&cm3rtos_rt.ready, idx);  

#if 0
	printf("%s(): active=%08x\n", __func__, (int)cm3rtos_rt.active);
				
	printf("%s(): <%d> ready=%08x priority=%d/%d task=%08x th=%08x sp=%08x\n", 
		   __func__, th->idx, cm3rtos_rt.ready, init->priority, 
		   cm3rtos_rt.sched_limit,
		   (int)init->task, (int)th, (int)ctx);
#endif

	return idx;
}

int cm3rtos_thread_id(void)
{
	return cm3rtos_rt.this;
}

void cm3_svc_handler(void)
{
	uint32_t * arg;
	uint8_t * pc;
	int svc;

	/* get a pointer to the process' stack */
	arg = (uint32_t * )cm3_psp_get();
	/* get PC value */
	pc = (uint8_t *)arg[6];
	/* get the immediate data from instruction */
	svc = pc[-2];

	switch (svc) {
	case CM3RTOS_THREAD_ID:
	//	arg[0] = cm3rtos_rt.active->idx;
		arg[0] = cm3rtos_rt.this;
		return;

	case CM3RTOS_YIELD:
//		bmp_bit_set((void *)&cm3rtos_rt.ready, cm3rtos_rt.active->idx);  
//		bmp_bit_set((void *)&cm3rtos_rt.ready, cm3rtos_rt.this);  
//		cm3rtos_defer_sched();
		return;

	case CM3RTOS_THREAD_CREATE:
		arg[0] = cm3rtos_thread_create_svc((struct cm3rtos_thread_init *)arg);
		return;

	case CM3RTOS_CANCEL:
		arg[0] = cm3rtos_cancel_svc(arg[0], arg[1]);
		return;

	case CM3RTOS_JOIN:
		arg[0] = cm3rtos_join_svc(arg[0]);
		return;

	case CM3RTOS_SLEEP:
		cm3rtos_sleep_svc(arg[0]);
		break;

	case CM3RTOS_WAIT:
		/* insert into the wait queue */
//		bmp_bit_set(&cm3rtos_rt.wq[arg[0]], cm3rtos_rt.active->idx);  
		bmp_bit_set(&cm3rtos_rt.wq[arg[0]], cm3rtos_rt.this);  
		cm3rtos_defer_sched();
		break;

//	case CM3RTOS_SIGNAL:
//		break;
	}

//	arg[0] = -1;
}

#define CM3_IRQ_MAX 80

int cm3rtos_init(void)
{
	struct cm3_systick * systick = CM3_SYSTICK;
	int idx;
	int irq;

	/* adjust exception priorities */

	/* SVC and SysTick must not be preempted
	   thus they run at highest possible priority */
	cm3_except_pri_set(CM3_EXCEPT_SVC, 0);
	cm3_except_pri_set(CM3_EXCEPT_SYSTICK, 0);

	/* PendSV interrupt has to have the lowest priority (higher number) */
	cm3_except_pri_set(CM3_EXCEPT_PENDSV, 255);

	for (irq = 0; irq < CM3_IRQ_MAX; irq++)
		cm3_irq_pri_set(irq, 128);

	/* configure the thread stack */
	cm3_psp_set(cm3_sp_get());
	/* configure the main stack */
	cm3_msp_set((uint32_t)&cm3rtos_except_stack + 
				sizeof(cm3rtos_except_stack));

	/* configure to use of PSP in thread mode */
	cm3_control_set(CONTROL_THREAD_PSP | CONTROL_THREAD_PRIV);

	/* initialize the runtime block */ 
	cm3rtos_rt.th_alloc = 0xffffffff << CM3RTOS_THREADS_MAX;

	/* initialize the main thread */ 

	/* alloc main thread */
	idx = cm3rtos_alloc_lo(&cm3rtos_rt.th_alloc);
	cm3rtos_rt.sched_pri[idx] = 1;
	cm3rtos_rt.sched_val[idx] = 0;

	/* set the active thread */
	cm3rtos_rt.this = idx;
	cm3rtos_rt.sched_limit = cm3rtos_rt.sched_pri[idx] = 1;

	/* 1ms tick period */
	systick->load = (CM3_SYSTICK_CLK_HZ / 10) - 1;
	systick->val = 0;
	systick->ctrl = SYSTICK_CTRL_ENABLE | SYSTICK_CTRL_TICKINT;

	/* instantiate the idle thread */
	cm3rtos_rt.idle_ctx = &cm3rtos_idle.ctx;

	return 0;
}

