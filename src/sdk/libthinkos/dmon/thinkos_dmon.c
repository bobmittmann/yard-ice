/* 
 * File:	 usb-cdc.c
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

_Pragma ("GCC optimize (\"O2\")")

#include <sys/stm32f.h>
#include <arch/cortex-m3.h>
#include <sys/param.h>
#include <string.h>
#include <stdbool.h>

#include <sys/dcclog.h>

#define __THINKOS_DMON__
#include <thinkos_dmon.h>
#include <thinkos.h>

#if (THINKOS_ENABLE_MONITOR)

struct thinkos_dmon thinkos_dmon_rt;
uint32_t thinkos_dmon_stack[240];
const uint16_t thinkos_dmon_stack_size = sizeof(thinkos_dmon_stack);

void dmon_context_swap(void * ctx); 

/* -------------------------------------------------------------------------
 * Debug Monitor API
 * ------------------------------------------------------------------------- */

uint32_t dmon_select(uint32_t evmask)
{
	uint32_t evset;
	
	DCC_LOG1(LOG_MSG, "evmask=%08x", evmask);

	evset = thinkos_dmon_rt.events;
	if (evset & evmask) {
		DCC_LOG1(LOG_INFO, "got evset=%08x !!", evset);
		return evset & evmask;
	}

	/* umask event */
	thinkos_dmon_rt.mask |= evmask;

	do {
		DCC_LOG(LOG_MSG, "sleep...");
		dmon_context_swap(&thinkos_dmon_rt.ctx); 
		evset = thinkos_dmon_rt.events;
		DCC_LOG1(LOG_MSG, "wakeup evset=%08x.", evset);
	} while ((evset & evmask) == 0);

	thinkos_dmon_rt.mask &= ~evmask;

	return evset & evmask;
}

int dmon_wait(int ev)
{
	uint32_t evset;
	uint32_t evmsk;
	uint32_t mask = (1 << ev);
	
	evset = thinkos_dmon_rt.events;
	if (evset & mask) {
		thinkos_dmon_rt.events = evset & ~(mask);
		return 0;
	}

	/* umask event */
	thinkos_dmon_rt.mask |= mask;

	DCC_LOG1(LOG_MSG, "waiting for %d, sleeping...", ev);
	do {
		dmon_context_swap(&thinkos_dmon_rt.ctx); 
		evset = thinkos_dmon_rt.events;
		evmsk = thinkos_dmon_rt.mask;
	} while ((evset & evmsk) == 0);
	DCC_LOG(LOG_MSG, "wakeup...");

	if (evset & mask) {
		thinkos_dmon_rt.events = evset & ~mask;
		thinkos_dmon_rt.mask = evmsk & ~mask;
		return 0;
	}

	DCC_LOG1(LOG_WARNING, "unexpected event=%08x!!", 
			 evset & thinkos_dmon_rt.mask);

	/* unexpected event received */
	return -1;
}

int dmon_expect(int ev)
{
	uint32_t evset;
	uint32_t evmsk;
	uint32_t mask = (1 << ev);
	
	evset = thinkos_dmon_rt.events;
	if (evset & mask)
		return 0;

	/* umask event */
	thinkos_dmon_rt.mask |= mask;

	DCC_LOG1(LOG_MSG, "waiting for %d, sleeping...", ev);
	do {
		dmon_context_swap(&thinkos_dmon_rt.ctx); 
		evset = thinkos_dmon_rt.events;
		evmsk = thinkos_dmon_rt.mask;
	} while ((evset & evmsk) == 0);
	DCC_LOG(LOG_MSG, "wakeup...");

	if (evset & mask) {
		thinkos_dmon_rt.mask = evmsk & ~mask;
		return 0;
	}

	DCC_LOG1(LOG_INFO, "unexpected event=%08x!!", 
			 evset & thinkos_dmon_rt.mask);

	/* unexpected event received */
	return -1;
}



void dmon_unmask(int event)
{
	__bit_mem_wr((uint32_t *)&thinkos_dmon_rt.mask, event, 1);  
}

void dmon_mask(int event)
{
	__bit_mem_wr((uint32_t *)&thinkos_dmon_rt.mask, event, 0);  
}

void dmon_clear(int event)
{
	__bit_mem_wr((uint32_t *)&thinkos_dmon_rt.events, event, 0);  
}

int dmon_sleep(unsigned int ms)
{
	dmon_clear(DMON_ALARM);
	/* set the clock */
	thinkos_rt.dmclock = thinkos_rt.ticks + ms;
	/* wait for signal */
	return dmon_wait(DMON_ALARM);
}

void dmon_alarm(unsigned int ms)
{
	dmon_clear(DMON_ALARM);
	dmon_unmask(DMON_ALARM);
	/* set the clock */
	thinkos_rt.dmclock = thinkos_rt.ticks + ms;
}

void dmon_alarm_stop(void)
{
	/* set the clock in the past so it won't generate a signal */
	thinkos_rt.dmclock = thinkos_rt.ticks - 1;
	/* make sure the signal is cleared */
	dmon_clear(DMON_ALARM);
	/* mask the signal */
	dmon_mask(DMON_ALARM);
}

int dmon_wait_idle(void)
{
	int ret;

	/* DEbug monitor request semaphore */
	CM3_DCB->demcr |= DCB_DEMCR_MON_REQ;

	/* wait for signal */
	if ((ret = dmon_wait(DMON_IDLE)) < 0)
		return ret;

	DCC_LOG(LOG_MSG, "[IDLE] zzz zzz zzz zzz");

	return 0;
}

void dmon_reset(void)
{
	dmon_signal(DMON_RESET);
	dmon_context_swap(&thinkos_dmon_rt.ctx); 
}

void dmon_exec(void (* task)(struct dmon_comm *))
{
	DCC_LOG1(LOG_TRACE, "task=%p", task);
	thinkos_dmon_rt.task = task;
	dmon_signal(DMON_RESET);
	dmon_context_swap(&thinkos_dmon_rt.ctx); 
}

/* -------------------------------------------------------------------------
 * Debug Breakpoint
 * ------------------------------------------------------------------------- */

#if (THINKOS_ENABLE_DEBUG_STEP)
#define BP_DEFSZ 2
/* (Flash Patch) Number of instruction address comparators */
#define CM3_FP_NUM_CODE 6
/* (Flash Patch) Number of literal address comparators */
#define CM3_FP_NUM_LIT  2

bool dmon_breakpoint_set(uint32_t addr, uint32_t size)
{
	struct cm3_fpb * fbp = CM3_FPB;
	uint32_t comp;
	int i;

	for (i = 0; i < CM3_FP_NUM_CODE; ++i) {
		if ((fbp->comp[i] & COMP_ENABLE) == 0) 
			break;
	}

	if (i == CM3_FP_NUM_CODE) {
		DCC_LOG(LOG_WARNING, "no more breakpoints");
		return false;
	}

	/* use default size if zero */
	size = (size == 0) ? BP_DEFSZ : size;

	if (size == 2) {
		if (addr & 0x00000002) {
			comp = COMP_BP_HIGH | (addr & 0x0ffffffc) | COMP_ENABLE;
		} else {
			comp = COMP_BP_LOW | (addr & 0x0ffffffc) | COMP_ENABLE;
		}
	} else {
		comp = COMP_BP_WORD | (addr & 0x0ffffffc) | COMP_ENABLE;
	}

	fbp->comp[i] = comp;

	DCC_LOG4(LOG_INFO, "bp=%d addr=0x%08x size=%d comp=0x%08x ", i, addr, 
			 size, fbp->comp[i]);

	return true;
}

bool dmon_breakpoint_clear(uint32_t addr, uint32_t size)
{
	struct cm3_fpb * fbp = CM3_FPB;
	uint32_t comp;
	int i;

	size = (size == 0) ? BP_DEFSZ : size;

	if (size == 2) {
		if (addr & 0x00000002) {
			comp = COMP_BP_HIGH | (addr & 0x0ffffffc) | COMP_ENABLE;
		} else {
			comp = COMP_BP_LOW | (addr & 0x0ffffffc) | COMP_ENABLE;
		}
	} else {
		comp = COMP_BP_WORD | (addr & 0x0ffffffc) | COMP_ENABLE;
	}

	DCC_LOG2(LOG_INFO, "addr=0x%08x size=%d", addr, size);

	for (i = 0; i < CM3_FP_NUM_CODE; ++i) {
		if ((fbp->comp[i] | COMP_ENABLE) == comp) {
			fbp->comp[i] = 0;
			return true;
		}
	}

	DCC_LOG1(LOG_WARNING, "breakpoint 0x%08x not found!", addr);

	return false;
}

bool dmon_breakpoint_disable(uint32_t addr)
{
	struct cm3_fpb * fbp = CM3_FPB;
	int i;

	for (i = 0; i < CM3_FP_NUM_CODE; ++i) {
		if ((fbp->comp[i] & 0x0ffffffc) == (addr & 0x0ffffffc)) {
			fbp->comp[i] &= ~COMP_ENABLE;
			return true;
		}
	}

	DCC_LOG1(LOG_WARNING, "breakpoint 0x%08x not found!", addr);

	return false;
}

void dmon_breakpoint_clear_all(void)
{
	struct cm3_fpb * fbp = CM3_FPB;
	int i;

	for (i = 0; i < CM3_FP_NUM_CODE + CM3_FP_NUM_LIT; ++i)
		fbp->comp[i] = 0;
}

#endif

/* -------------------------------------------------------------------------
 * Thread stepping
 * ------------------------------------------------------------------------- */

#if (THINKOS_ENABLE_DEBUG_STEP)
int dmon_thread_step(unsigned int thread_id, bool sync)
{
	int ret;

	DCC_LOG2(LOG_INFO, "step_req=%08x thread_id=%d", 
			 thinkos_rt.step_req, thread_id);

	if (CM3_DCB->dhcsr & DCB_DHCSR_C_DEBUGEN) {
		DCC_LOG(LOG_ERROR, "can't step: DCB_DHCSR_C_DEBUGEN !!");
		return -1;
	}

	if (thread_id >= THINKOS_THREADS_MAX) {
		DCC_LOG1(LOG_ERROR, "thread %d is invalid!", thread_id);
		return -1;
	}

	if (__bit_mem_rd(&thinkos_rt.step_req, thread_id)) {
		DCC_LOG1(LOG_WARNING, "thread %d is step waiting already!", thread_id);
		return -1;
	}

	/* request stepping the thread  */
	__bit_mem_wr(&thinkos_rt.step_req, thread_id, 1);

	/* resume the thread */
	__thinkos_thread_resume(thread_id);

	if (__bit_mem_rd(&thinkos_rt.wq_ready, thread_id)) {
		uint32_t insn;
		uint16_t * pc;

		/* get the PC value */
		pc = (uint16_t *)thinkos_rt.ctx[thread_id]->pc;
		/* get the next instruction */
		insn = pc[0];

		DCC_LOG3(LOG_TRACE, "thread_id=%d PC=%08x INSN=%04x +++++", 
				 thread_id, pc, insn);

		/* if the thread is running, and it is about to invoke 
		   a system call then we need to step twice. */
		if ((insn & 0xdf00) == 0xdf00) {
			DCC_LOG1(LOG_TRACE, "SVC %d", insn & 0xff); 
			/* the thread is stepping into a system call */
			__bit_mem_wr(&thinkos_rt.step_svc, thread_id, 1);
		} 
	} else {
		/* if the thread is not in the ready queue, 
		   it should be waiting on a system call. Don't really
		   step, stop right after the syscall return */
		__bit_mem_wr(&thinkos_rt.step_brk, thread_id, 1);
	}

	/* make sure to run the scheduler */
	__thinkos_defer_sched();

	if (sync) {
		if ((ret = dmon_wait(DMON_THREAD_STEP)) < 0)
			return ret;
	}

	return 0;
}
#endif

/* -------------------------------------------------------------------------
 * Debug Monitor Core
 * ------------------------------------------------------------------------- */

void __attribute__((noinline)) dmon_context_swap(void * ctx) 
{
	register void * ptr0 asm("r0") = ctx;
	asm volatile ("push   {r4-r11,lr}\n"
				  "mrs    r1, APSR\n"
				  "push   {r1}\n"
				  "mov    r1, sp\n"
				  "ldr    sp, [%0]\n" /* restore context */
				  "str    r1, [%0]\n" /* save context */
				  "pop    {r1}\n"
				  "msr    APSR_nzcvq, r1\n"
				  "pop    {r4-r11,lr}\n"
				  : : "r" (ptr0) : "r1");
}

static void __attribute__((naked)) dmon_bootstrap(void)
{
	/* set the clock in the past so it won't generate signals in 
	 the near future */
	thinkos_rt.dmclock = thinkos_rt.ticks - 1;
	thinkos_dmon_rt.task(thinkos_dmon_rt.comm);
	dmon_context_swap(&thinkos_dmon_rt.ctx); 
}

static void dmon_on_reset(struct thinkos_dmon * dmon)
{
	uint32_t * sp;

	DCC_LOG(LOG_TRACE, "DMON_RESET");
	sp = &thinkos_dmon_stack[(sizeof(thinkos_dmon_stack) / 4) - 10];
	sp[0] = 0x0100000f; /* CPSR */
	sp[1] = 0; /* R4 */
	sp[2] = 0; /* R5 */
	sp[9] = ((uint32_t)dmon_bootstrap) | 1; /* LR */
	dmon->ctx = sp;
	dmon->events &= ~(1 << DMON_RESET);
	dmon->mask |= (1 << DMON_RESET) | (1 << DMON_COMM_CTL) | (1 << DMON_EXCEPT);
}

void __attribute__((noinline)) dbgmon_isr(struct cm3_except_context * ctx)
{
	uint32_t sigset = thinkos_dmon_rt.events;
	uint32_t sigmsk = thinkos_dmon_rt.mask;

#if (THINKOS_ENABLE_DEBUG_STEP)
	uint32_t dfsr;

	/* read SCB Debug Fault Status Register */
	if ((dfsr = CM3_SCB->dfsr) != 0) {
		uint32_t demcr;

		/* clear the fault */
		CM3_SCB->dfsr = dfsr;

		DCC_LOG5(LOG_INFO, "DFSR=(EXT=%c)(VCATCH=%c)"
				 "(DWTRAP=%c)(BKPT=%c)(HALT=%c)", 
				 dfsr & SCB_DFSR_EXTERNAL ? '1' : '0',
				 dfsr & SCB_DFSR_VCATCH ? '1' : '0',
				 dfsr & SCB_DFSR_DWTTRAP ? '1' : '0',
				 dfsr & SCB_DFSR_BKPT ? '1' : '0',
				 dfsr & SCB_DFSR_HALTED ? '1' : '0');
	
		demcr = CM3_DCB->demcr;

		DCC_LOG3(LOG_INFO, "DEMCR=(REQ=%c)(PEND=%c)(STEP=%c)", 
				 demcr & DCB_DEMCR_MON_REQ ? '1' : '0',
				 demcr & DCB_DEMCR_MON_PEND ? '1' : '0',
				 demcr & DCB_DEMCR_MON_STEP ? '1' : '0');

		if (dfsr & SCB_DFSR_BKPT) {
			if ((CM3_SCB->icsr & SCB_ICSR_RETTOBASE) == 0) {
				DCC_LOG2(LOG_ERROR, "<<BREAKPOINT>>: exception=%d pc=%08x", 
						 ctx->xpsr & 0x1ff, ctx->pc);
				DCC_LOG(LOG_ERROR, "invalid breakpoint on exception!!!");
				sigset |= (1 << DMON_BREAKPOINT);
				sigmsk |= (1 << DMON_BREAKPOINT);
				thinkos_dmon_rt.events = sigset;
				/* FIXME: add support for breakpoints on IRQ */
				/* record the break thread id */
				thinkos_rt.break_id = thinkos_rt.active;
				__thinkos_pause_all();
				/* diasble all breakpoints */
				dmon_breakpoint_clear_all();
			} else if ((uint32_t)thinkos_rt.active < THINKOS_THREADS_MAX) {
				sigset |= (1 << DMON_BREAKPOINT);
				sigmsk |= (1 << DMON_BREAKPOINT);
				thinkos_dmon_rt.events = sigset;
				DCC_LOG2(LOG_TRACE, "<<BREAKPOINT>>: thread_id=%d pc=%08x ---", 
						 thinkos_rt.active, ctx->pc);
				/* suspend the current thread */
				__thinkos_thread_pause(thinkos_rt.active);
				/* record the break thread id */
				thinkos_rt.break_id = thinkos_rt.active;
				__thinkos_defer_sched();
				/* diasble this breakpoint */
				dmon_breakpoint_disable(ctx->pc);
			} else {
				DCC_LOG2(LOG_ERROR, "<<BREAKPOINT>>: thread_id=%d pc=%08x ---", 
						 thinkos_rt.active, ctx->pc);
				DCC_LOG(LOG_ERROR, "invalid active thread!!!");
				sigset |= (1 << DMON_BREAKPOINT);
				sigmsk |= (1 << DMON_BREAKPOINT);
				thinkos_dmon_rt.events = sigset;
				/* record the break thread id */
				thinkos_rt.break_id = thinkos_rt.active;
				__thinkos_pause_all();
				/* diasble all breakpoints */
				dmon_breakpoint_clear_all();
			}
		}

		if (dfsr & SCB_DFSR_HALTED) {
			if (demcr & DCB_DEMCR_MON_STEP) {
				int thread_id = thinkos_rt.step_id;
				if ((unsigned int)thread_id < THINKOS_THREADS_MAX) {
					int ipsr = (ctx->xpsr & 0x1ff);

					DCC_LOG3(LOG_TRACE, "<<STEP>> thread_id=%d pc=%08x" 
							 " ipsr=%d ------", thread_id, ctx->pc, ipsr);

					if (ipsr == 0) {
						if (__bit_mem_rd(&thinkos_rt.step_svc, thread_id)) {
							DCC_LOG(LOG_TRACE, "wait for SVC");
							return;
						}
					} else if (ipsr == CM3_EXCEPT_SVC) {
						if (__bit_mem_rd(&thinkos_rt.step_svc, thread_id)) {
							DCC_LOG(LOG_TRACE, "got SVC!!!!");
							__bit_mem_wr(&thinkos_rt.step_svc, thread_id, 0);
							/* mark the thread to stop on service return */
							__bit_mem_wr(&thinkos_rt.step_brk, thread_id, 1);
							/* make sure to run the scheduler */
							__thinkos_defer_sched();
							thinkos_rt.step_id = -1;
							goto step_done;
						} else {
							DCC_LOG(LOG_ERROR, "unexpected SVC mode !!!");
							goto step_done;
						}	
					} else {
						DCC_LOG(LOG_ERROR, "unexpected exception !!!");
						goto step_done;
					}

					/* suspend the thread, this will clear the 
					   step request flag */
					__thinkos_thread_pause(thread_id);
					/* signal the monitor */
					sigset |= (1 << DMON_THREAD_STEP);
					sigmsk |= (1 << DMON_THREAD_STEP);
					thinkos_dmon_rt.events = sigset;
					__thinkos_defer_sched();
				} else {
					DCC_LOG1(LOG_ERROR, "invalid stepping thread %d !!!", 
							 thread_id);
				}
				thinkos_rt.break_id = thread_id;
step_done:
				CM3_DCB->demcr = demcr & ~DCB_DEMCR_MON_STEP;
			} else {
				DCC_LOG(LOG_TRACE, "SCB_DFSR_HALTED !!!");
			}
		}
	}
#endif

	DCC_LOG1(LOG_INFO, "<%08x>", sigset);

	if (sigset & (1 << DMON_RESET)) {
		dmon_on_reset(&thinkos_dmon_rt);
	}

	/* Process monitor events */
	if ((sigset & sigmsk) != 0) {
		dmon_context_swap(&thinkos_dmon_rt.ctx); 
	}
}

#if (THINKOS_ENABLE_DEBUG_STEP)
void __attribute__((noinline)) dbgmon_stop_isr(struct cm3_except_context * ctx, 
											   unsigned int thread_id)
{
	DCC_LOG2(LOG_TRACE, "thread_id=%d PC=%08x", thread_id, ctx->pc);

	/* suspend the thread, this will clear the 
	   step request flag */
	__thinkos_thread_pause(thread_id);
	__thinkos_defer_sched();
	/* clear the break signal */
	__bit_mem_wr(&thinkos_rt.step_brk, thread_id, 0);
	/* ser the break thread */
	thinkos_rt.break_id = thread_id;
	/* signal the monitor */
	thinkos_dmon_rt.events |= (1 << DMON_THREAD_STEP);

	dmon_context_swap(&thinkos_dmon_rt.ctx); 
}

void __attribute__((naked)) dbgmon_step_isr(struct cm3_except_context * ctx, 
											unsigned int thread_id)
{
//	DCC_LOG2(LOG_TRACE, "PC=%08x REQ=%08x", ctx->pc, &thinkos_step_req);
	if (__bit_mem_rd(&thinkos_rt.step_brk, thread_id)) {
		/* signal the monitor */
		dbgmon_stop_isr(ctx, thread_id);
		return;
	}	

	/* save the current stepping thread */
	thinkos_rt.step_id = thread_id;

	/* Clear PendSV active */
	CM3_SCB->shcsr &= ~SCB_SHCSR_PENDSVACT; 

	/* Remove DebugMon context */
	asm volatile ("pop    {r0-r3,r12,lr}\n"
				  "add    sp, sp, #2 * 4\n"
				  : : : ); 
	/* Step and return */
	/* CM3_DCB->demcr |= DCB_DEMCR_MON_STEP */
	asm volatile ("movw   r3, #0xedf0\n"
				  "movt   r3, #0xe000\n"
				  "ldr    r2, [r3, #12]\n"
				  "orr.w  r2, r2, #(1 << 18)\n"
				  "str    r2, [r3, #12]\n"
				  "bx     lr\n"
				  : :  : "r3", "r2"); 
}

extern uintptr_t thinkos_thread_step_call;
#endif

void __attribute__((naked)) cm3_debug_mon_isr(void)
{
	register struct cm3_except_context * ctx asm("r0");
	register uint32_t thread_id asm("r1");

	/* select the context stack according to the content of LR */
	asm volatile ("tst lr, #4\n" 
				  "ite eq\n" 
				  "mrseq %0, MSP\n" 
				  "mrsne %0, PSP\n" 
				  : "=r"(ctx) : : );

#if (THINKOS_ENABLE_DEBUG_STEP)
	/* was this a thread step request ? */
	if (ctx->pc == (uint32_t)&thinkos_thread_step_call)
		dbgmon_step_isr(ctx, thread_id);
	else
#endif
		dbgmon_isr(ctx);
}

void __thinkos_irq_disable_all(void);

void thinkos_exception_dsr(struct thinkos_except * xcpt)
{
	if (xcpt->thread_id >= 0) {
		DCC_LOG1(LOG_WARNING, "Fault at thread %d !!!!!!!!!!!!!", 
				 xcpt->thread_id);
#if THINKOS_ENABLE_DEBUG_STEP
		thinkos_rt.break_id = thinkos_rt.active;
#endif
		dmon_signal(DMON_THREAD_FAULT);
	} else {
#if THINKOS_ENABLE_DEBUG_STEP
		int ipsr;

		ipsr = (xcpt->ctx.xpsr & 0x1ff);
		DCC_LOG1(LOG_ERROR, "Exception at IRQ: %d !!!", 
				 ipsr - 16);
		/* FIXME: add support for exceptions on IRQ */
		thinkos_rt.break_id = -ipsr;

		if (ipsr == CM3_EXCEPT_DEBUG_MONITOR) {
			DCC_LOG(LOG_TRACE, "1. disable all interrupts"); 
			__thinkos_irq_disable_all();
			DCC_LOG(LOG_TRACE, "2. ThinkOS reset...");
			__thinkos_reset();
#if THINKOS_ENABLE_CONSOLE
			DCC_LOG(LOG_TRACE, "3. console reset...");
			__console_reset();
#endif
			DCC_LOG(LOG_TRACE, "4. exception reset...");
			__exception_reset();
#if (THINKOS_ENABLE_DEBUG_STEP)
			DCC_LOG(LOG_TRACE, "6. clear all breakpoints...");
			dmon_breakpoint_clear_all();
#endif
			DCC_LOG(LOG_TRACE, "7. restore Comm interrupts...");
			dmon_comm_irq_config(thinkos_dmon_rt.comm);
			DCC_LOG(LOG_TRACE, "8. reset.");
			dmon_signal(DMON_RESET);
		}
#endif
		dmon_signal(DMON_EXCEPT);
	}
}

/* -------------------------------------------------------------------------
 * ThinkOS thread level API
 * ------------------------------------------------------------------------- */

void thinkos_dmon_init(void * comm, void (* task)(struct dmon_comm * ))
{
	struct cm3_dcb * dcb = CM3_DCB;
	uint32_t demcr; 
	
	thinkos_dmon_rt.events = (1 << DMON_RESET);
	thinkos_dmon_rt.mask = (1 << DMON_RESET);
	thinkos_dmon_rt.comm = comm;
	thinkos_dmon_rt.task = task;

	__thinkos_memset32(thinkos_dmon_stack, 0xdeadbeef, 
					   sizeof(thinkos_dmon_stack));

	DCC_LOG1(LOG_TRACE, "comm=%0p", comm);

	/* clear the step request */
	demcr = dcb->demcr;
	
	demcr &= ~DCB_DEMCR_MON_STEP;
	/* enable monitor and send the reset event */
	demcr |= DCB_DEMCR_MON_EN | DCB_DEMCR_MON_PEND;

	dcb->demcr = demcr;
}

#endif /* THINKOS_ENABLE_MONITOR */

