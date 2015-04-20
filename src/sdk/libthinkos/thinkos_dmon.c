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
	
	DCC_LOG1(LOG_TRACE, "evmask=%08x", evmask);

	evset = thinkos_dmon_rt.events;
	if (evset & evmask)
		return evset & evmask;

	/* umask event */
	thinkos_dmon_rt.mask |= evmask;

	do {
		DCC_LOG(LOG_TRACE, "sleep...");
		monitor_context_swap(&thinkos_dmon_rt.ctx); 
		DCC_LOG(LOG_TRACE, "wakeup...");
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

	DCC_LOG1(LOG_TRACE, "waiting for %d, sleeping...", ev);
	monitor_context_swap(&thinkos_dmon_rt.ctx); 
	DCC_LOG(LOG_TRACE, "wakeup...");

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
		/* insert into the paused list */
	/* insert into the wait queue */
	__thinkos_wq_insert(wq, th);
#endif
	/* remove from the ready wait queue */
	__thinkos_suspend(th);
	/* signal the scheduler ... */
	__thinkos_defer_sched();

	thinkos_except_buf.thread = th;
}

void cm3_debug_mon_isr(void)
{
	struct cm3_dcb * dcb = CM3_DCB;
	uint32_t evset;

	DCC_LOG1(LOG_INFO, "demcr=%08x", dcb->demcr);

	if (dcb->demcr & DCB_DEMCR_MON_REQ) {
		DCC_LOG(LOG_TRACE, "DCB_DEMCR_MON_REQ");
	} else {
		DCC_LOG(LOG_TRACE, "HW Debug Event");
	}

	if (thinkos_dmon_rt.events & (1 << DMON_EXCEPT)) {
		thinkos_dmon_rt.events &= ~(1 << DMON_EXCEPT);
		thinkos_dmon_rt.events |= (1 << DMON_THREAD_FAULT);
		dmon_except(&thinkos_dmon_rt);
	}

	DCC_LOG2(LOG_TRACE, "events=%08x mask=%08x", 
			 thinkos_dmon_rt.events, thinkos_dmon_rt.mask);

	/* Process events */
	if ((evset = (thinkos_dmon_rt.events & thinkos_dmon_rt.mask)) != 0) {
		if (evset & (1 << DMON_START)) {
			uint32_t * sp;
			DCC_LOG(LOG_TRACE, "DMON_START");
			sp = &thinkos_dmon_stack[(sizeof(thinkos_dmon_stack) / 4) - 10];
			sp[0] = 0x0100000f; /* CPSR */
			sp[9] = ((uint32_t)dmon_bootstrap) | 1; /* LR */
			thinkos_dmon_rt.ctx = sp;
			thinkos_dmon_rt.events = 0;
			thinkos_dmon_rt.mask = (1 << DMON_START) | (1 << DMON_COMM_CTL) |
				(1 << DMON_EXCEPT);
		} else {
			DCC_LOG1(LOG_TRACE, "evset=%08x", evset);
		}

		monitor_context_swap(&thinkos_dmon_rt.ctx); 
	}

	DCC_LOG(LOG_INFO, "done.");
}

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
	dcb->demcr = DCB_DEMCR_MON_EN | DCB_DEMCR_MON_PEND;
}

