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
uint32_t thinkos_dmon_stack[256];
const uint16_t thinkos_dmon_stack_size = sizeof(thinkos_dmon_stack);

void dmon_context_swap(void * ctx); 

/* -------------------------------------------------------------------------
 * Debug Monitor API
 * ------------------------------------------------------------------------- */

uint32_t dmon_select(uint32_t evmask)
{
	uint32_t evset;
	
	DCC_LOG1(LOG_INFO, "evmask=%08x", evmask);

	evset = thinkos_dmon_rt.events;
	if (evset & evmask) {
		DCC_LOG1(LOG_INFO, "got evset=%08x !!", evset);
		return evset & evmask;
	}

	/* umask event */
	thinkos_dmon_rt.mask |= evmask;

	do {
		DCC_LOG(LOG_INFO, "sleep...");
		dmon_context_swap(&thinkos_dmon_rt.ctx); 
		evset = thinkos_dmon_rt.events;
		DCC_LOG1(LOG_INFO, "wakeup evset=%08x.", evset);
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

	DCC_LOG1(LOG_INFO, "waiting for %d, sleeping...", ev);
	do {
		dmon_context_swap(&thinkos_dmon_rt.ctx); 
		evset = thinkos_dmon_rt.events;
		evmsk = thinkos_dmon_rt.mask;
	} while ((evset & evmsk) == 0);
	DCC_LOG(LOG_INFO, "wakeup...");

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

	DCC_LOG1(LOG_INFO, "waiting for %d, sleeping...", ev);
	do {
		dmon_context_swap(&thinkos_dmon_rt.ctx); 
		evset = thinkos_dmon_rt.events;
		evmsk = thinkos_dmon_rt.mask;
	} while ((evset & evmsk) == 0);
	DCC_LOG(LOG_INFO, "wakeup...");

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
	/* request signal */
	thinkos_dmon_rt.req |= (1 << DMON_IDLE);
	/* wait for signal */
	if ((ret = dmon_wait(DMON_IDLE)) < 0)
		return ret;

	/* clear request */
	thinkos_dmon_rt.req &= ~(1 << DMON_IDLE);
	return 0;
}

void dmon_reset(void)
{
	dmon_signal(DMON_RESET);
	dmon_context_swap(&thinkos_dmon_rt.ctx); 
}

void dmon_exec(void (* task)(struct dmon_comm *))
{
	thinkos_dmon_rt.task = task;
	dmon_signal(DMON_RESET);
	dmon_context_swap(&thinkos_dmon_rt.ctx); 
}

/* -------------------------------------------------------------------------
 * Debug Breakpoint
 * ------------------------------------------------------------------------- */

#if (THINKOS_ENABLE_DEBUG_STEP)
#define BP_DEFSZ 2
#define BP_MAX 6

bool dmon_breakpoint_set(uint32_t addr, uint32_t size)
{
	struct cm3_fpb * fbp = CM3_FPB;
	uint32_t comp;
	int i;

	for (i = 0; i < BP_MAX; ++i) {
		if ((fbp->comp[i] & COMP_ENABLE) == 0) 
			break;
	}

	if (i == BP_MAX) {
		DCC_LOG(LOG_WARNING, "no more breakpoints");
		return false;
	}

	/* use default size if zero */
	size = (size == 0) ? BP_DEFSZ : size;

	if (size == 2) {
		if (addr & 0x00000002) {
			comp = COMP_BP_HIGH | (addr & 0xfffffffc) | COMP_ENABLE;
		} else {
			comp = COMP_BP_LOW | (addr & 0xfffffffc) | COMP_ENABLE;
		}
	} else {
		comp = COMP_BP_WORD | (addr & 0xfffffffc) | COMP_ENABLE;
	}

	fbp->comp[i] = comp;

	DCC_LOG4(LOG_TRACE, "bp=%d addr=0x%08x size=%d comp=0x%08x ", i, addr, 
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
			comp = COMP_BP_HIGH | (addr & 0xfffffffc) | COMP_ENABLE;
		} else {
			comp = COMP_BP_LOW | (addr & 0xfffffffc) | COMP_ENABLE;
		}
	} else {
		comp = COMP_BP_WORD | (addr & 0xfffffffc) | COMP_ENABLE;
	}

	DCC_LOG2(LOG_TRACE, "addr=0x%08x size=%d", addr, size);

	for (i = 0; i < BP_MAX; ++i) {
		if (fbp->comp[i] == comp) {
			fbp->comp[i] = 0;
			return true;
		}
	}

	DCC_LOG(LOG_WARNING, "breakpoint not found!");
	return false;
}

void dmon_breakpoint_clear_all(void)
{
	struct cm3_fpb * fbp = CM3_FPB;
	int i;

	for (i = 0; i < BP_MAX; ++i)
		fbp->comp[i] = 0;
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


static void __attribute__((noreturn, naked)) dmon_bootstrap(void)
{
	/* set the clock in the past so it won't generate signals in 
	 the near future */
	thinkos_rt.dmclock = thinkos_rt.ticks - 1;
	thinkos_dmon_rt.task(thinkos_dmon_rt.comm);
	DCC_LOG(LOG_WARNING, "task exit.");
	for (;;) {
		dmon_context_swap(&thinkos_dmon_rt.ctx); 
	}
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

#if (THINKOS_ENABLE_DEBUG_STEP)
int dmon_thread_step(unsigned int id, bool sync)
{
	int ret;

	if (CM3_DCB->dhcsr & DCB_DHCSR_C_DEBUGEN) {
		DCC_LOG(LOG_ERROR, "can't step: DCB_DHCSR_C_DEBUGEN !!");
		return -1;
	}

	if (__bit_mem_rd(&thinkos_rt.step_req, id)) {
		DCC_LOG1(LOG_WARNING, "thread %d is step waiting already!", id);
		return -1;
	}

	/* request stepping the thread  */
	__bit_mem_wr(&thinkos_rt.step_req, id, 1);
	/* resume the thread */
	__thinkos_thread_resume(id);
	/* make sure to run the scheduler */
	__thinkos_defer_sched();

	DCC_LOG1(LOG_TRACE, "thread_id=%d +++++++++++++++++++++", id);

	if (sync) {
		if ((ret = dmon_wait(DMON_THREAD_STEP)) < 0)
			return ret;
	}

	return 0;
}
#endif

#if 0
void thinkos_suspend_all(void)
{
	int32_t th;

	for (th = 0; th < THINKOS_THREADS_MAX; ++th) {
		__thinkos_thread_pause(th);
	}

	__thinkos_defer_sched();
}
#endif

static inline uint32_t __attribute__((always_inline)) cm3_svc_stackframe(void) {
	register uint32_t sp;
	asm volatile ("tst lr, #4\n" 
				  "ite eq\n" 
				  "mrseq %0, MSP\n" 
				  "mrsne %0, PSP\n" 
				  : "=r" (sp));
	return sp;
}

#if 1
void cm3_debug_mon_isr(void)
{
	uint32_t sigset = thinkos_dmon_rt.events;
	uint32_t sigmsk = thinkos_dmon_rt.mask;

#if 0
	uint32_t demcr;

	demcr = CM3_DCB->demcr;
	(void)demcr; 

	DCC_LOG3(LOG_INFO, "DEMCR=(REQ=%c)(PEND=%c)(STEP=%c)", 
			demcr & DCB_DEMCR_MON_REQ ? '1' : '0',
			demcr & DCB_DEMCR_MON_PEND ? '1' : '0',
			demcr & DCB_DEMCR_MON_STEP ? '1' : '0');
#endif
#if (THINKOS_ENABLE_DEBUG_STEP)
	uint32_t lr = cm3_lr_get();
	uint32_t dfsr;

	/* read SCB Debug Fault Status Register */
	if ((dfsr = CM3_SCB->dfsr) != 0) {
		/* clear the fault */
		CM3_SCB->dfsr = dfsr;
		DCC_LOG5(LOG_TRACE, "DFSR=(EXT=%c)(VCATCH=%c)"
				 "(DWTRAP=%c)(BKPT=%c)(HALT=%c)", 
				 dfsr & SCB_DFSR_EXTERNAL ? '1' : '0',
				 dfsr & SCB_DFSR_VCATCH ? '1' : '0',
				 dfsr & SCB_DFSR_DWTTRAP ? '1' : '0',
				 dfsr & SCB_DFSR_BKPT ? '1' : '0',
				 dfsr & SCB_DFSR_HALTED ? '1' : '0');

		if (dfsr & SCB_DFSR_BKPT) {
			if (lr & 0x4) {
				DCC_LOG(LOG_ERROR, "invalid breakpoint on exception!!!");
				sigset |= (1 << DMON_BREAKPOINT);
				sigmsk |= (1 << DMON_BREAKPOINT);
				thinkos_dmon_rt.events = sigset;
				__thinkos_pause_all();
				/* diasble all breakpoints */
				dmon_breakpoint_clear_all();
			} else if ((uint32_t)thinkos_rt.active < THINKOS_THREADS_MAX) {
				sigset |= (1 << DMON_BREAKPOINT);
				sigmsk |= (1 << DMON_BREAKPOINT);
				thinkos_dmon_rt.events = sigset;
				/* suspend the current thread */
				__thinkos_thread_pause(thinkos_rt.active);
				__thinkos_defer_sched();
				DCC_LOG1(LOG_TRACE, "thread_id=%d --------------------", 
						 thinkos_rt.active);
			} else {
				DCC_LOG1(LOG_ERROR, "invalid active thread: %d !!!",
						 thinkos_rt.active);
				sigset |= (1 << DMON_BREAKPOINT);
				sigmsk |= (1 << DMON_BREAKPOINT);
				thinkos_dmon_rt.events = sigset;
				__thinkos_pause_all();
				/* diasble all breakpoints */
				dmon_breakpoint_clear_all();
			}
		}

		if (dfsr & SCB_DFSR_HALTED) {
			/* clear the step request */
			CM3_DCB->demcr &= ~DCB_DEMCR_MON_STEP;
			if ((uint32_t)thinkos_rt.step_id < THINKOS_THREADS_MAX) {
				/* suspend the thread, this will clear the step request flag */
				__thinkos_thread_pause(thinkos_rt.step_id);
				/* signal the monitor */
				sigset |= (1 << DMON_THREAD_STEP);
				sigmsk |= (1 << DMON_THREAD_STEP);
				thinkos_dmon_rt.events = sigset;
				__thinkos_defer_sched();
				DCC_LOG1(LOG_TRACE, "thread_id=%d --------------------", 
						 thinkos_rt.step_id);
			} else {
				DCC_LOG1(LOG_ERROR, "invalid stepping thread %d !!!", 
						 thinkos_rt.step_id);
			}
		}
	}
#endif

	DCC_LOG1(LOG_INFO, "<%08x>", sigset );

	if (sigset & (1 << DMON_RESET)) {
		dmon_on_reset(&thinkos_dmon_rt);
	}

	/* Process monitor events */
	if ((sigset & sigmsk) != 0) {
		dmon_context_swap(&thinkos_dmon_rt.ctx); 
	}

}
#endif

#if 0

static inline void __attribute__((always_inline)) __wait(void) {
	asm volatile ("mov    r3, #1\n"
				  "0:\n"
				  "cbz	r3, 1f\n"
				  "b.n  0b\n"
				  "1:\n" : : : "r3"); 
}

void thinkos_debug_monitor(void)
{
	struct cm3_dcb * dcb = CM3_DCB;
	uint32_t sigset = thinkos_dmon_rt.events;


	if (dcb->demcr & DCB_DEMCR_MON_REQ) {
		DCC_LOG(LOG_TRACE, "DCB_DEMCR_MON_REQ +++++++++++++++");
		dcb->demcr &= ~DCB_DEMCR_MON_REQ;
		thinkos_suspend_all();
	}

	if (sigset & (1 << DMON_EXCEPT)) {
		sigset &= ~(1 << DMON_EXCEPT);
		sigset |= (1 << DMON_THREAD_FAULT);
		thinkos_dmon_rt.events = sigset;
		dmon_on_except(&thinkos_dmon_rt);
	}

	if (sigset & (1 << DMON_RESET)) {
		dmon_on_reset(&thinkos_dmon_rt);
	}

	/* Process monitor events */
	if ((sigset & thinkos_dmon_rt.mask) != 0) {
		dmon_context_swap(&thinkos_dmon_rt.ctx); 
	}

}

/* THinkOS - Debug Monitor */
void __attribute__((naked, noreturn)) cm3_debug_mon_isr(void)
//void cm3_debug_mon_isr(void)
{
	register struct thinkos_context * ctx asm("r0");
	register uint32_t lr asm("r4");
	uint32_t idx;

	/* save the context */
	asm volatile ("mrs    %0, PSP\n" 
				  "stmdb  %0!, {r4-r11}\n"
				  "mov    %1, lr\n"
				  : "=r" (ctx), "=r" (lr));

	/* disable interrupts */
	cm3_cpsid_i();

	/* get the active (current) thread */	
	idx = thinkos_rt.active;
	/* save SP */
	thinkos_rt.ctx[idx] = ctx;

	thinkos_debug_monitor();

	/* update the active thread */
	idx = thinkos_rt.active;
	ctx = thinkos_rt.ctx[idx];

	/* enable interrupts */
	cm3_cpsie_i();

	/* restore the context */
	asm volatile (
				  "mov    lr, %1\n"
				  "add    r3, %0, #8 * 4\n"
				  "msr    PSP, r3\n"
				  "ldmia  %0, {r4-r11}\n"
				  "bx     lr\n"
				  : : "r" (ctx), "r" (lr): "r3"); 
	for(;;);
}
#endif


/* -------------------------------------------------------------------------
 * ThinkOS thread level API
 * ------------------------------------------------------------------------- */

void thinkos_exception_dsr(struct thinkos_except * xcpt)
{
	unsigned int isr_no;

	isr_no = xcpt->ctx.xpsr & 0xff;

	if (isr_no == 0) {
		if ((uint32_t)thinkos_rt.active < THINKOS_THREADS_MAX) {
			/* record the  */
			xcpt->thread_id = thinkos_rt.active;
			/* suspend the current thread */
			__thinkos_thread_pause(thinkos_rt.active);
			__thinkos_defer_sched();
			DCC_LOG1(LOG_WARNING, "Fault at thread %d !!!!!!!!!!!!!",
					 thinkos_rt.active);
			dmon_signal(DMON_THREAD_FAULT);
		} else {
			DCC_LOG1(LOG_ERROR, "invalid active thread: %d !!!",
					 thinkos_rt.active);
			dmon_signal(DMON_EXCEPT);
		}
	} else {
		/* suspend all threads */
		__thinkos_pause_all();
		dmon_signal(DMON_EXCEPT);
	}
}

void thinkos_dmon_init(void * comm, void (* task)(struct dmon_comm * ))
{
	struct cm3_dcb * dcb = CM3_DCB;
	
	thinkos_dmon_rt.events = (1 << DMON_RESET);
	thinkos_dmon_rt.mask = (1 << DMON_RESET);
	thinkos_dmon_rt.req = 0;
	thinkos_dmon_rt.comm = comm;
	thinkos_dmon_rt.task = task;

	__thinkos_memset32(thinkos_dmon_stack, 0xdeadbeef, 
					   sizeof(thinkos_dmon_stack));

	DCC_LOG1(LOG_TRACE, "comm=%0p", comm);

	/* enable monitor and send the start event */
	dcb->demcr |= DCB_DEMCR_MON_EN | DCB_DEMCR_MON_PEND;
}



#endif /* THINKOS_ENABLE_MONITOR */

