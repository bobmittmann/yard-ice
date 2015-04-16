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
#include <sys/serial.h>
#include <string.h>
#include <stdbool.h>
#include <sys/usb-cdc.h>

#include <sys/dcclog.h>

#include "monitor.h"

void monitor_on_event(struct thinkos_monitor * mon, 
					  struct thinkos_context * ctx, int ev)
{
	char buf[64];
	int n;

	switch (ev) {
	case MON_COMM_RCV:
		DCC_LOG(LOG_TRACE, "MON_COMM_RCV");
		n = monitor_comm_recv(mon->comm.drv, buf, 64);
		(void)n;
		break;
	case MON_COMM_EOT:
		DCC_LOG(LOG_TRACE, "MON_COMM_EOT");
		break;
	}
}

static inline struct thinkos_context * __attribute__((always_inline)) 
__save_context(void) {
	register struct thinkos_context * ctx asm("r0");
	asm volatile ("mrs   %0, PSP\n" 
				  "stmdb %0!, {r4-r11}\n"
				  "push  {lr}\n"
				  : "=r" (ctx));
	return ctx;
}

static inline void __attribute__((always_inline)) 
__restore_context(struct thinkos_context * __ctx) {
	register struct thinkos_context * r0 asm("r0") = __ctx;
	asm volatile ("add    r3, %0, #8 * 4\n"
				  "msr    PSP, r3\n"
				  "ldmia  %0, {r4-r11}\n"
				  "pop    {lr}\n"
				  "bx     lr\n"
				  : : "r" (r0) : "r3"); 
}

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
	DCC_LOG1(LOG_TRACE, "xpsr=%08x", __ctx->xpsr);
}

static inline void __attribute__((always_inline)) __wait(void) {
	asm volatile ("mov    r3, #1\n"
				  "0:\n"
				  "cbz	r3, 1f\n"
				  "b.n  0b\n"
				  "1:\n" : : : "r3"); 
}

void __attribute__((naked)) cm3_debug_mon_isr(void)
{
	struct cm3_dcb * dcb = CM3_DCB;
	struct thinkos_context * ctx;
	uint32_t msp;
	uint32_t psp;
	uint32_t lr;
	int ev;

	/* save the context */
	ctx = __save_context();
	lr = cm3_lr_get();
	msp = cm3_msp_get();
	psp = cm3_psp_get();
	(void)psp;
	(void)msp;
	(void)lr;

	DCC_LOG1(LOG_TRACE, "demcr=%08x", dcb->demcr);

	if (dcb->demcr & DCB_DEMCR_MON_REQ) {
		DCC_LOG(LOG_TRACE, "DCB_DEMCR_MON_REQ");
	} else {
		DCC_LOG(LOG_TRACE, "HW Debug Event");
	}

	/* Process events */
	while ((ev = __clz(__rbit(monitor_rt.events))) < 32) {
		__bit_mem_wr(&monitor_rt.events, ev, 0);  
		monitor_on_event(&monitor_rt, ctx, ev);
	}

	/* restore the context */
	__restore_context(ctx);
}


struct usb_cdc_class * usb_mon_init(const usb_dev_t * usb, 
									const uint8_t * const str[], 
									unsigned int strcnt);

void monitor_init(void)
{
	struct cm3_dcb * dcb = CM3_DCB;

	monitor_rt.events = 0;
	dcb->demcr = DCB_DEMCR_MON_EN;

	usb_cdc_sn_set(*((uint64_t *)STM32F_UID));
	monitor_rt.comm.drv = usb_mon_init(&stm32f_otg_fs_dev, 
									   cdc_acm_def_str, 
									   cdc_acm_def_strcnt);
}

