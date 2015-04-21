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

#include <sys/stm32f.h>
#include <arch/cortex-m3.h>
#include <sys/param.h>
#include <string.h>
#include <stdbool.h>

#include <sys/dcclog.h>

#define __THINKOS_DMON__
#include <thinkos_dmon.h>


struct thinkos_dmon thinkos_dmon_rt;
uint32_t thinkos_dmon_stack[256];

void __attribute__((noinline)) monitor_context_swap(void * ctx) 
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

uint32_t dmon_select(uint32_t evmask)
{
	uint32_t evset;
	
	DCC_LOG1(LOG_INFO, "evmask=%08x", evmask);

	evset = thinkos_dmon_rt.events;
	if (evset & evmask)
		return evset & evmask;

	/* umask event */
	thinkos_dmon_rt.mask |= evmask;

	do {
		DCC_LOG(LOG_INFO, "sleep...");
		monitor_context_swap(&thinkos_dmon_rt.ctx); 
		DCC_LOG(LOG_INFO, "wakeup...");
		evset = thinkos_dmon_rt.events;
	} while ((evset & evmask) == 0);

	thinkos_dmon_rt.mask &= ~evmask;

	return evset & evmask;
}

int dmon_wait(int ev)
{
	uint32_t evset;
	uint32_t mask = (1 << ev);
	
	evset = thinkos_dmon_rt.events;
	if (evset & mask) {
		thinkos_dmon_rt.events = evset & ~(mask);
		return 0;
	}

	/* umask event */
	thinkos_dmon_rt.mask |= mask;

	DCC_LOG1(LOG_INFO, "waiting for %d, sleeping...", ev);
	monitor_context_swap(&thinkos_dmon_rt.ctx); 
	DCC_LOG(LOG_INFO, "wakeup...");

	evset = thinkos_dmon_rt.events;
	if (evset & mask) {
		thinkos_dmon_rt.events = evset & ~mask;
		thinkos_dmon_rt.mask &= ~mask;
		return 0;
	}

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
	/* set the clock */
	thinkos_rt.dmclock = thinkos_rt.ticks + ms;
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

static inline void __attribute__((always_inline)) __wait(void) {
	asm volatile ("mov    r3, #1\n"
				  "0:\n"
				  "cbz	r3, 1f\n"
				  "b.n  0b\n"
				  "1:\n" : : : "r3"); 
}

void __attribute__((noreturn)) dmon_bootstrap(void)
{
	thinkos_dmon_rt.task(&thinkos_dmon_rt, thinkos_dmon_rt.comm);
	for(;;);
}

void dmon_start(struct thinkos_dmon * dmon)
{
	uint32_t * sp;
	DCC_LOG(LOG_TRACE, "DMON_START");
	sp = &thinkos_dmon_stack[(sizeof(thinkos_dmon_stack) / 4) - 10];
	sp[0] = 0x0100000f; /* CPSR */
	sp[9] = ((uint32_t)dmon_bootstrap) | 1; /* LR */
	dmon->ctx = sp;
	dmon->events = 0;
	dmon->mask = (1 << DMON_START) | (1 << DMON_COMM_CTL) | (1 << DMON_EXCEPT);
}

void dmon_except(struct thinkos_dmon * mon)
{
#if DEBUG
	struct thinkos_context * ctx = &thinkos_except_buf.ctx;
#endif
	uint32_t sp = thinkos_except_buf.sp;
	uint32_t ret = thinkos_except_buf.ret;
	bool handler_mode;
	int th;
	(void)sp;
	(void)ret;

	handler_mode = (ret & (1 << 4)) ? false : true;

	DCC_LOG1(LOG_ERROR, "Bus fault!: [%s mode]!", handler_mode ? 
			 "handler": "thread");
	DCC_LOG4(LOG_ERROR, "  R0=%08x  R1=%08x  R2=%08x  R3=%08x", 
			ctx->r0, ctx->r1, ctx->r2, ctx->r3);
	DCC_LOG4(LOG_ERROR, "  R4=%08x  R5=%08x  R6=%08x  R7=%08x", 
			ctx->r4, ctx->r7, ctx->r6, ctx->r7);
	DCC_LOG4(LOG_ERROR, "  R8=%08x  R9=%08x R10=%08x R11=%08x", 
			ctx->r8, ctx->r9, ctx->r10, ctx->r11);
	DCC_LOG4(LOG_ERROR, " R12=%08x  SP=%08x  LR=%08x  PC=%08x", 
			ctx->r12, sp, ctx->lr, ctx->pc);
	DCC_LOG2(LOG_ERROR, "XPSR=%08x RET=%08x", 
			ctx->xpsr, ret);

	if (handler_mode) {
		DCC_LOG(LOG_TRACE, "-----------------------------------------");
		for(;;);
	}

	/* get the active (current) thread */	
	th = thinkos_rt.active;
#if THINKOS_ENABLE_PAUSE
	/* insert into the paused queue */
	__bit_mem_wr(&thinkos_rt.wq_paused, th, 1);
#endif
	/* remove from the ready wait queue */
	__thinkos_suspend(th);

	for(;;);

	thinkos_except_buf.thread = th;
}


#if 1
void cm3_debug_mon_isr(void)
{
	uint32_t sigset = thinkos_dmon_rt.events;

	if (sigset & (1 << DMON_EXCEPT)) {
		sigset &= ~(1 << DMON_EXCEPT);
		sigset |= (1 << DMON_THREAD_FAULT);
		thinkos_dmon_rt.events = sigset;
		dmon_except(&thinkos_dmon_rt);
	}

	if (sigset & (1 << DMON_START)) {
		dmon_start(&thinkos_dmon_rt);
	}

	/* Process monitor events */
	if ((sigset & thinkos_dmon_rt.mask) != 0) {
		monitor_context_swap(&thinkos_dmon_rt.ctx); 
	}
}
#endif

#if 0
void thinkos_debug_monitor(void)
{
	uint32_t sigset = thinkos_dmon_rt.events;

	if (sigset & (1 << DMON_EXCEPT)) {
		sigset &= ~(1 << DMON_EXCEPT);
		sigset |= (1 << DMON_THREAD_FAULT);
		thinkos_dmon_rt.events = sigset;
		dmon_except(&thinkos_dmon_rt);
	}

	if (sigset & (1 << DMON_START)) {
		dmon_start(&thinkos_dmon_rt);
	}

	/* Process monitor events */
	if ((sigset & thinkos_dmon_rt.mask) != 0) {
		monitor_context_swap(&thinkos_dmon_rt.ctx); 
	}
}

/* THinkOS - Debug Monitor */
//void __attribute__((naked, noreturn)) cm3_debug_mon_isr(void)
void cm3_debug_mon_isr(void)
{
	register struct thinkos_context * ctx asm("r0");
//	register uint32_t lr asm("r4");
	uint32_t idx;

	/* save the context */
	asm volatile ("mrs    %0, PSP\n" 
				  "stmdb  %0!, {r4-r11}\n"
//				  "mov    %1, lr\n"
//				  : "=r" (ctx), "=r" (lr));
				  : "=r" (ctx));

	/* get the active (current) thread */	
	idx = thinkos_rt.active;
	/* save SP */
	thinkos_rt.ctx[idx] = ctx;

	DCC_LOG2(LOG_TRACE, "--> thread=%d ctx=%p", idx, ctx);

//	thinkos_debug_monitor();

	/* update the active thread */
	idx = thinkos_rt.active;
	ctx = thinkos_rt.ctx[idx];
	DCC_LOG2(LOG_TRACE, "<-- thread=%d ctx=%p", idx, ctx);

	/* restore the context */
	asm volatile ("add    r3, %0, #8 * 4\n"
				  "msr    PSP, r3\n"
				  "ldmia  %0, {r4-r11}\n"
				  : : "r" (ctx) : "r3"); 
}
#endif

void thinkos_exception_dsr(struct thinkos_except * xcpt)
{
	dmon_signal(DMON_EXCEPT);
}

void thinkos_dmon_init(void * comm, void (* task)(struct thinkos_dmon * , 
												  struct dmon_comm * ))
{
	struct cm3_dcb * dcb = CM3_DCB;
	
	thinkos_dmon_rt.events = (1 << DMON_START);
	thinkos_dmon_rt.mask = (1 << DMON_START);
	thinkos_dmon_rt.comm = comm;
	thinkos_dmon_rt.task = task;

	DCC_LOG1(LOG_TRACE, "comm=%0p", comm);

	/* enable monitor and send the start event */
	dcb->demcr |= DCB_DEMCR_MON_EN | DCB_DEMCR_MON_PEND;
}

