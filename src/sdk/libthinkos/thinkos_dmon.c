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

uint32_t dmon_select(uint32_t watch)
{
	uint32_t ret;
	uint32_t evset;
	uint32_t mask;

	for (;;) {
		evset = thinkos_dmon_rt.events;
		mask = thinkos_dmon_rt.mask | watch;
		
		if ((ret = (evset & mask)) != 0)
			break;

		monitor_context_swap(&thinkos_dmon_rt.ctx); 
	}

	/* clear events */
	thinkos_dmon_rt.events = evset ^ ret;

	/* return events */
	return ret;
}

int dmon_wait(int event)
{
	uint32_t mask = (1 << event);
	uint32_t evset;

	DCC_LOG1(LOG_TRACE, "event=%d", event);

	thinkos_dmon_rt.mask |= mask;
	monitor_context_swap(&thinkos_dmon_rt.ctx); 
	if ((evset = thinkos_dmon_rt.events) & mask) {
		/* clear event */
		thinkos_dmon_rt.events = evset ^ mask;
		return 0;
	}

	DCC_LOG(LOG_WARNING, "unexpected event received");

	/* unexpected event received */
	return -1;
}


void dmon_unmask(int event)
{
	__bit_mem_wr((uint32_t *)&thinkos_dmon_rt.mask, event, 0);  
}

void dmon_mask(int event)
{
	__bit_mem_wr((uint32_t *)&thinkos_dmon_rt.mask, event, 1);  
}

void cm3_debug_mon_isr(void)
{
	struct cm3_dcb * dcb = CM3_DCB;
	uint32_t evset;

	DCC_LOG1(LOG_TRACE, "demcr=%08x", dcb->demcr);

	if (dcb->demcr & DCB_DEMCR_MON_REQ) {
		DCC_LOG(LOG_TRACE, "DCB_DEMCR_MON_REQ");
	} else {
		DCC_LOG(LOG_TRACE, "HW Debug Event");
	}

	/* Process events */
	if ((evset = (thinkos_dmon_rt.events & thinkos_dmon_rt.mask)) != 0) {
		if (evset & (1 << DMON_START)) {
			uint32_t * sp;
			sp = &thinkos_dmon_stack[(sizeof(thinkos_dmon_stack) / 4) - 10];
			sp[0] = 0x0100000f; /* CPSR */
			sp[9] = ((uint32_t)thinkos_dmon_rt.task) + 1; /* LR */
			thinkos_dmon_rt.ctx = sp;
			thinkos_dmon_rt.events = 0;
			thinkos_dmon_rt.mask = (1 << DMON_START);
		}

//		monitor_on_event(&thinkos_dmon_rt, ctx, ev);
		monitor_context_swap(&thinkos_dmon_rt.ctx); 
	}

	DCC_LOG(LOG_TRACE, "done.");
}


void thinkos_dmon_init(void * comm, void (* task)(struct thinkos_dmon * dmon))
{
	struct cm3_dcb * dcb = CM3_DCB;
	
	thinkos_dmon_rt.events = (1 << DMON_START);
	thinkos_dmon_rt.mask = (1 << DMON_START);
	thinkos_dmon_rt.comm = comm;
	thinkos_dmon_rt.task = task;

	/* enable monitor and send the start event */
	dcb->demcr = DCB_DEMCR_MON_EN | DCB_DEMCR_MON_PEND;
}

