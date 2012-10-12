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
 * Main stack - used for exception handling
 * --------------------------------------------------------------------------*/
uint64_t cm3rtos_except_stack[32];

void __attribute__((noreturn)) cm3_hard_fault_isr(void)
{
	struct cm3_scb * scb = CM3_SCB;
	uint32_t hfsr;

	cm3_faultmask_set(1);
//	cm3_msp_set((uint32_t)&cm3rtos_except_stack);

	printf("---\n");
	printf("Hard fault:");

	hfsr = scb->hfsr;

	if (hfsr & SCB_HFSR_DEBUGEVT)  
		printf(" DEBUGEVT");
	if (hfsr & SCB_HFSR_FORCED)  
		printf(" FORCED");
	if (hfsr & SCB_HFSR_VECTTBL)  
		printf(" VECTTBL");

	printf("\n");

	for(;;);
}

void __attribute__((noreturn)) cm3_bus_fault_isr(void)
{
	struct cm3_scb * scb = CM3_SCB;
	uint32_t bfsr;

	cm3_faultmask_set(1);
//	cm3_msp_set((uint32_t)&cm3rtos_except_stack);

	printf("---\n");
	printf("Bus fault:");

	bfsr = SCB_CFSR_BFSR_GET(scb->cfsr);

	if (bfsr & BFSR_BFARVALID)  
		printf(" BFARVALID");
	if (bfsr & BFSR_LSPERR)
		printf(" LSPERR");
	if (bfsr & BFSR_STKERR)  
		printf(" STKERR");
	if (bfsr & BFSR_UNSTKERR)  
		printf(" INVPC");
	if (bfsr & BFSR_IMPRECISERR)  
		printf(" IMPRECISERR");
	if (bfsr & BFSR_PRECISERR)
		printf(" PRECISERR");
	if (bfsr & BFSR_IBUSERR)  
		printf(" IBUSERR");
	printf("\n");

	if (bfsr & BFSR_BFARVALID)  {
		printf(" * ADDR = 0x%08x\n", scb->bfar);
	}

	for(;;);
}

void __attribute__((noreturn)) cm3_usage_fault_isr(void)
{
	struct cm3_scb * scb = CM3_SCB;
	uint32_t ufsr;

	cm3_faultmask_set(1);
//	cm3_msp_set((uint32_t)&cm3rtos_except_stack);

	printf("---\n");
	printf("Usage fault:");

	ufsr = SCB_CFSR_UFSR_GET(scb->cfsr);
	if (ufsr & UFSR_DIVBYZERO)  
		printf(" DIVBYZERO");
	if (ufsr & UFSR_UNALIGNED)  
		printf(" UNALIGNED");
	if (ufsr & UFSR_NOCP)  
		printf(" NOCP");
	if (ufsr & UFSR_INVPC)  
		printf(" INVPC");
	if (ufsr & UFSR_INVSTATE)  
		printf(" INVSTATE");
	if (ufsr & UFSR_UNDEFINSTR)  
		printf(" UNDEFINSTR");

	printf("\n");

	for(;;);
}
/* -------------------------------------------------------------------------- 
 * Run Time RTOS block 
 * --------------------------------------------------------------------------*/


struct cm3rtos_rt {
	/* Pointer to the active (current) thread */
	struct cm3rtos_context * ctx[32 + 1];
	int32_t this;
	volatile uint32_t ready;
	int32_t a;
	volatile uint32_t cpu_wait;
	int32_t sched_limit;
	struct cm3rtos_thread * th_lst[32 + 1];
	uint32_t wq_alloc;
	uint32_t wq[CM3RTOS_WAIT_QUEUE_MAX];
	uint32_t wq_sleep;
	uint32_t wq_join[CM3RTOS_THREADS_MAX];
	uint32_t th_alloc;
	struct cm3rtos_thread * active;
	uint32_t paused;
};

struct cm3rtos_rt cm3rtos_rt;

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

void __attribute__((noreturn, naked)) cm3_pendsv_isr(void)
{
	register struct cm3rtos_context * ctx asm("r0");
//	struct cm3rtos_thread * th;			  
	uint32_t bmp;
	int idx;

	asm volatile ("mrs  %0, PSP\n" 
				  "stmdb %0!, {r4-r11, lr}\n" : "=r" (ctx));

	/* get the active thread pointer */
//	th = cm3rtos_rt.active;
//	th = (struct cm3rtos_thread *)cm3_ldrexw((uint32_t *)&cm3rtos_rt.active);
	idx = cm3rtos_rt.this;
//	th = cm3rtos_rt.th_lst[idx];
	cm3rtos_rt.ctx[idx] = ctx;

	/* save the active thread stack pointer*/
//	th->sp = ctx;

#if 0
	if ((bmp = cm3rtos_rt.ready) == 0) {
		bmp = cm3rtos_rt.cpu_wait;
		cm3rtos_rt.cpu_wait = 0;
		cm3rtos_rt.ready = bmp;
	}
#endif

	bmp = cm3rtos_rt.ready;

	/* get a thread from the ready bitmap */
	idx = cm3_clz(cm3_rbit(bmp));
	bmp_bit_clr((void *)&cm3rtos_rt.ready, idx);  

	cm3rtos_rt.this = idx;
	if (idx == 32)
		printf("~");
	else
		printf("%c", idx + '0');

	/* get the thread pointer from the priority list */
//	th = cm3rtos_rt.th_lst[idx];

//	cm3_strexw((uint32_t *)&cm3rtos_rt.active, (int32_t)th);

//	cm3rtos_rt.active = th;

	/* The operation of clearing the thread from 
	   the ready bitmap can have side effects if
	   the scheduler is preempted by another interrupt:
	   1. System is IDLE
	   2. IRQ insert a thread and pend Scheduler
	   3. Schduler Get thread 
	   4. Anothee IRQ preempts scheduler, before updating the active thread
	   5. IRQ considers the system IDLE and do not reinsert the thread
	   6. Scheduler run again, but ready queue is empty

	   Possible solutions:
	   1. Disable interrupts while running scheduler.
	   2. Clear pending IRQ to avoid running the scheduler again.
	 */
	/* The scheduler could have been interrupted and might be pending
	   again. As we already got a thread to run, clear
	   the pending bit. */
//	CM3_SCB->icsr = SCB_ICSR_PENDSVCLR;

	/* get the process stack pointer */
//	ctx = th->sp;

//	printf("%s(): <%d>\n", __func__, idx);

	ctx = cm3rtos_rt.ctx[idx];

	/* restore the context */
	asm volatile ( "add    r2, %0, #(9 * 4)\n"
				   "msr    PSP, r2\n"
				   "ldmia	%0, {r4-r11, pc}\n" : : "r" (ctx));
	for(;;);
}

static inline int __cm3rtos_sched(void)
{
	int idx;

	/* get a thread from the ready bitmap */
	idx = cm3_clz(cm3_rbit(cm3rtos_rt.ready));
	bmp_bit_clr((void *)&cm3rtos_rt.ready, idx);  
	cm3rtos_rt.this = idx;
	return idx;
}


//void __attribute__((section (".ramcode"), noreturn, naked)) cm3_nmi_isr(void)
void __attribute__((noreturn, naked)) cm3_nmi_isr(void)
{
	register struct cm3rtos_context * ctx asm("r0");
	int idx;

	asm volatile ("mrs  %0, PSP\n" 
				  "stmdb %0!, {r4-r11, lr}\n" : "=r" (ctx));

	cm3_cpsie_i();

	idx = cm3rtos_rt.this;
	cm3rtos_rt.ctx[idx] = ctx;

//	bmp_bit_set((void *)&cm3rtos_rt.ready, idx);  

//	idx = __cm3rtos_sched();

	idx = cm3_clz(cm3_rbit(cm3rtos_rt.ready));
	bmp_bit_clr((void *)&cm3rtos_rt.ready, idx);  
	cm3rtos_rt.this = idx;

	if (idx == 32)
		printf("~");
	else
		printf("%c", idx + '0');

	ctx = cm3rtos_rt.ctx[idx];

	/* restore the context */
	asm volatile ( "add    r2, %0, #(9 * 4)\n"
				   "msr    PSP, r2\n"
				   "ldmia	%0, {r4-r11, pc}\n" : : "r" (ctx));
	for(;;);
}

//void cm3_systick_isr(void)
void __attribute__((noreturn, naked)) cm3_systick_isr(void)
{
	register struct cm3rtos_context * ctx asm("r0");
	struct cm3rtos_thread * th;
	int idx;
	int j;

	asm volatile ("mrs  %0, PSP\n" 
				  "stmdb %0!, {r4-r11, lr}\n" : "=r" (ctx));

	idx = cm3rtos_rt.this;
	cm3rtos_rt.ctx[idx] = ctx;

	for (j = 0; j < CM3RTOS_THREADS_MAX; j++) {
		if (bmp_bit_rd(&cm3rtos_rt.wq_sleep, j) == 0)
			continue;

		th = cm3rtos_rt.th_lst[j];

		if (--th->ticks <= 0) {
			bmp_bit_clr((void *)&cm3rtos_rt.wq_sleep, j);  
			bmp_bit_set((void *)&cm3rtos_rt.ready, j);  
			printf("%s(): wakeup <%d> ready=%08x\n", __func__, 
				   j, cm3rtos_rt.ready);
			asm volatile ("nop\n");
			break;
		}
	}

	/* insert active thread into READY queue */

//	th = cm3rtos_rt.active;
//	if (th != &cm3rtos_idle_thread) {

	if (idx != 32) {
//		idx = th->idx;
#if 0
		th = cm3rtos_rt.th_lst[idx];
		th->ticks -= th->priority;
		if (th->ticks < 0) {
			th->ticks += cm3rtos_rt.sched_limit;
			/* insert into the CPU wait queue */
			bmp_bit_set((void *)&cm3rtos_rt.cpu_wait, idx);  
			if (cm3rtos_rt.ready == 0) {
				cm3rtos_rt.ready = cm3rtos_rt.cpu_wait;
				cm3rtos_rt.cpu_wait = 0;
			}
//			printf("?%x", cm3rtos_rt.ready);
		} else {
			bmp_bit_set((void *)&cm3rtos_rt.ready, idx);
//			printf(">%x", cm3rtos_rt.ready);
		}
#endif
		bmp_bit_set((void *)&cm3rtos_rt.ready, idx);
	}

//	cm3rtos_sched();
//	cm3rtos_defer_sched();

	idx = __cm3rtos_sched();


	if (idx == 32)
		printf("!");
	else
		printf("%c", idx + 'A');


	ctx = cm3rtos_rt.ctx[idx];
	/* restore the context */
	asm volatile ( "add    r2, %0, #(9 * 4)\n"
				   "msr    PSP, r2\n"
				   "ldmia	%0, {r4-r11, pc}\n" : : "r" (ctx));
	for(;;);
}

static void __attribute__((noreturn)) cm3rtos_thread_exit(int code)
{
//	struct cm3rtos_thread * this;
	struct cm3rtos_thread * th;
	int idx;
	int j;

	idx = cm3rtos_rt.this;
//	this = cm3rtos_rt.th_lst[idx];
//	this = cm3rtos_rt.active;
//	idx = this->idx;

	bmp_bit_clr((void *)&cm3rtos_rt.cpu_wait, idx);  
	bmp_bit_clr((void *)&cm3rtos_rt.wq_sleep, idx);  

	cm3_cpsid_i();

#if 1
	printf("%s(): <%d> th=%08x code=%d\n", 
		   __func__, idx, (int)cm3rtos_rt.th_lst[idx], code); 
#endif

	/* Releases the thread block */
	bmp_bit_clr(&cm3rtos_rt.th_alloc, idx);

	printf("%s(): bit=%d\n", __func__, bmp_bit_rd(&cm3rtos_rt.wq_join[idx], 0));

	for (j = 0; j < CM3RTOS_THREADS_MAX; j++) {
		if (bmp_bit_rd(&cm3rtos_rt.th_alloc, j) == 0)
			continue;

		th = cm3rtos_rt.th_lst[j];

		if (cm3rtos_rt.sched_limit < th->priority) {
			cm3rtos_rt.sched_limit = th->priority;
		}


		if (bmp_bit_rd(&cm3rtos_rt.wq_join[idx], j) != 0) {
			printf("%s(): wakeup <%d>\n", __func__, j);
			bmp_bit_set((void *)&cm3rtos_rt.ready, j);  
			bmp_bit_clr((void *)&cm3rtos_rt.wq_join[idx], j);  
			th->sp->r0 = code;
//			cm3rtos_rt.ctx[j]->r0 = code;
		}
	}
	cm3_cpsie_i();
	cm3rtos_sched();
	for(;;);
}

int cm3rtos_join(unsigned int idx)
{
	int ret;

	cm3_cpsid_i();
	if (bmp_bit_rd(&cm3rtos_rt.th_alloc, idx) == 0) {
#if 1
		printf("%s(): <%d> thread=%d failed!\n", __func__, 
			   cm3rtos_rt.this, idx);
#endif
		ret = -1;
	} else {
#if 1
		printf("%s(): <%d> thread=%d waiting...\n", __func__, 
			   cm3rtos_rt.this, idx);
#endif
		bmp_bit_set((void *)&cm3rtos_rt.wq_join[idx], cm3rtos_rt.this);  
		cm3rtos_sched();
	}
	cm3_cpsie_i();

	return ret;
}


int cm3rtos_cancel(unsigned int idx, int code)
{
//	struct cm3rtos_thread * th;
	int ret;

	cm3_cpsid_i();
	if (bmp_bit_rd(&cm3rtos_rt.th_alloc, idx) == 0) {
		ret = -1;
	} else {
		/* XXX: sanity check */
//		th = cm3rtos_rt.th_lst[idx];
//		th->sp->pc = (uint32_t)cm3rtos_thread_exit;
//		th->sp->r0 = code;
		cm3rtos_rt.ctx[idx]->pc = (uint32_t)cm3rtos_thread_exit;
		cm3rtos_rt.ctx[idx]->r0 = code;
		bmp_bit_set((void *)&cm3rtos_rt.ready, idx);  
		ret = 0;
#if 0
		printf("%s(): <%d> th=%08x code=%d\n", 
			   __func__, idx, (int)cm3rtos_rt.th_lst[idx], code); 
#endif
	}
	cm3_cpsie_i();

	return ret;
}

void cm3rtos_yield(void)
{
	cm3_cpsid_i();
	bmp_bit_set((void *)&cm3rtos_rt.ready, cm3rtos_rt.this);  
	cm3rtos_sched();
}

void cm3rtos_sleep(unsigned int ms)
{
	/* insert into the sleep wait queue */
//	cm3rtos_rt.active->ticks = arg[0] + 1;
//		bmp_bit_set(&cm3rtos_rt.wq_sleep, cm3rtos_rt.active->idx);  
	//	printf("%s(): <%d> sleep...\n", __func__, cm3rtos_rt.this);
	
	cm3_cpsid_i();

	cm3rtos_rt.th_lst[cm3rtos_rt.this]->ticks = (ms + 1) / 100;
	bmp_bit_set(&cm3rtos_rt.wq_sleep, cm3rtos_rt.this);  
	cm3rtos_sched();
}

/* initialize a thread context */
int svc_cm3rtos_thread_create(struct cm3rtos_thread_init * init)
{
	struct cm3rtos_thread * th;
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

	th = (struct cm3rtos_thread *)init->stack_ptr;

	sp = (uint32_t)init->stack_ptr + init->stack_size;
	sp &= 0xfffffff8; /* 64bits alignemnt */
	sp -= sizeof(struct cm3rtos_context);

	ctx = (struct cm3rtos_context *)sp;

//	ctx->impure = NULL;
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

	th->sp = ctx;
	th->idx = idx;
	th->priority = init->priority;

	/* insert the new item in the list */
	cm3rtos_rt.th_lst[idx] = th;

	cm3rtos_rt.ctx[idx] = ctx;

	/* insert into the ready list */
	bmp_bit_set((void *)&cm3rtos_rt.ready, idx);  

	if (cm3rtos_rt.sched_limit < th->priority) {
		cm3rtos_rt.sched_limit = th->priority;
	}

	th->ticks =  cm3rtos_rt.sched_limit / 2;
#if 0
	printf("%s(): active=%08x\n", __func__, (int)cm3rtos_rt.active);
				
	printf("%s(): <%d> ready=%08x priority=%d/%d task=%08x th=%08x sp=%08x\n", 
		   __func__, th->idx, cm3rtos_rt.ready, init->priority, 
		   cm3rtos_rt.sched_limit,
		   (int)init->task, (int)th, (int)ctx);
#endif

	return th->idx;
}

int cm3rtos_thread_id(void)
{
	return cm3rtos_rt.this;
}

int cm3rtos_thread_create(int (* task)(void *), 
						  void * arg, void * stack_ptr,
						  unsigned int stack_size,
						  int priority)
{
	struct cm3rtos_thread_init init;

	init.task = task;
	init.arg = arg;
	init.stack_ptr = stack_ptr;
	init.stack_size = stack_size;
	init.priority = priority;
	return svc_cm3rtos_thread_create(&init);
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
		arg[0] = svc_cm3rtos_thread_create((struct cm3rtos_thread_init *)arg);
		return;

	case CM3RTOS_CANCEL:
//		svc_cm3rtos_thread_cancel(arg[0], arg[1]);
		return;

	case CM3RTOS_JOIN:
//		arg[0] = svc_cm3rtos_thread_join(arg[0]);
		return;

	case CM3RTOS_SLEEP:
//		svc_cm3rtos_thread_sleep(arg);
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

struct cm3rtos_thread cm3rtos_main_thread;

int cm3rtos_init(void)
{
	struct cm3_systick * systick = CM3_SYSTICK;
	struct cm3rtos_thread * th;
	int idx;
	int irq;

	/* adjust exception priorities */
	cm3_except_pri_set(CM3_EXCEPT_SVC, 0);
	/* the PendSV interrupt has to have the lowest priority (higher number) */
	cm3_except_pri_set(CM3_EXCEPT_PENDSV, 0);
	cm3_except_pri_set(CM3_EXCEPT_SYSTICK, 0);

	for (irq = 0; irq < CM3_IRQ_MAX; irq++)
		cm3_irq_pri_set(irq, 32);

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
	th = &cm3rtos_main_thread;

	/* alloc main thread */
	idx = cm3rtos_alloc_lo(&cm3rtos_rt.th_alloc);
	cm3rtos_rt.th_lst[idx] = th;
	th->priority = 1;	
	th->ticks = 0;	
	th->idx = idx;

	/* set the active thread */
//	cm3rtos_rt.active = th;
	cm3rtos_rt.this = idx;
	cm3rtos_rt.sched_limit = th->priority;

	/* 1ms tick period */
	systick->load = (CM3_SYSTICK_CLK_HZ / 10) - 1;
	systick->val = 0;
	systick->ctrl = SYSTICK_CTRL_ENABLE | SYSTICK_CTRL_TICKINT;

//	cm3rtos_rt.th_lst[32] = &cm3rtos_idle_thread;
	cm3rtos_rt.ctx[32] = &cm3rtos_idle.ctx;

	return 0;
}

