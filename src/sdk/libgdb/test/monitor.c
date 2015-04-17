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

uint32_t monitor_stack[256];

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

void monitor_event_unmask(int event)
{
	monitor_rt.mask |= 1 << event;
}

void monitor_event_mask(int event)
{
	monitor_rt.mask &= ~(1 << event);
}

uint32_t monitor_wait_multiple(uint32_t watch)
{
	uint32_t ret;
	uint32_t evset;
	uint32_t mask;

	for (;;) {
		evset = monitor_rt.events;
		mask = monitor_rt.mask | watch;
		
		if ((ret = (evset & mask)) != 0)
			break;

		monitor_context_swap(&monitor_rt.ctx); 
	}

	/* clear events */
	monitor_rt.events = evset ^ ret;

	/* return events */
	return ret;
}

int monitor_wait(int event)
{
	uint32_t mask = (1 << event);
	uint32_t evset;

	DCC_LOG1(LOG_TRACE, "event=%d", event);

	monitor_rt.mask |= mask;
	monitor_context_swap(&monitor_rt.ctx); 
	if ((evset = monitor_rt.events) & mask) {
		/* clear event */
		monitor_rt.events = evset ^ mask;
		return 0;
	}

	DCC_LOG(LOG_WARNING, "unexpected event received");

	/* unexpected event received */
	return -1;
}

const char monitor_banner[] = "\r\n\r\n"
"-------------------------------------------------------------------------\r\n"
"- ThinkOS Monitor\r\n"
"-------------------------------------------------------------------------\r\n"
"\r\n";

void __attribute__((noreturn)) monitor_task(struct thinkos_monitor * mon) 
{
	char buf[64];
	int n;
	int c;

	DCC_LOG(LOG_TRACE, "Monitor start...");
	monitor_comm_connect(mon->comm.drv);

	monitor_comm_send(mon->comm.drv, monitor_banner, 
					  sizeof(monitor_banner) - 1);

	for(;;) {
		n = monitor_comm_recv(mon->comm.drv, buf, 64);
		DCC_LOG1(LOG_TRACE, "n=%d...", n);
		(void)n;
		if (n == 0)
			continue;

		monitor_comm_send(mon->comm.drv, buf, n);
		c = buf[0];
		if (c == 'i') {
			monitor_signal(MON_START);
		}
	}
}

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
	if ((evset = (monitor_rt.events & monitor_rt.mask)) != 0) {
		if (evset & (1 << MON_START)) {
			uint32_t * sp;
			sp = &monitor_stack[(sizeof(monitor_stack) / 4) - 10];
			sp[0] = 0x0100000f; /* CPSR */
			sp[9] = ((uint32_t)monitor_task) + 1; /* LR */
			monitor_rt.ctx = sp;
			monitor_rt.events = 0;
			monitor_rt.mask = (1 << MON_START);
		}

//		monitor_on_event(&monitor_rt, ctx, ev);
		monitor_context_swap(&monitor_rt.ctx); 
	}

	DCC_LOG(LOG_TRACE, "done.");
}


struct usb_cdc_class * usb_mon_init(const usb_dev_t * usb, 
									const uint8_t * const str[], 
									unsigned int strcnt);

void monitor_init(void)
{
	struct cm3_dcb * dcb = CM3_DCB;
	
	monitor_rt.events = (1 << MON_START);
	monitor_rt.mask = (1 << MON_START);

	usb_cdc_sn_set(*((uint64_t *)STM32F_UID));
	monitor_rt.comm.drv = usb_mon_init(&stm32f_otg_fs_dev, 
									   cdc_acm_def_str, 
									   cdc_acm_def_strcnt);

	/* enable monitor and send the start event */
	dcb->demcr = DCB_DEMCR_MON_EN | DCB_DEMCR_MON_PEND;
}

#if 0
static inline struct thinkos_context * __attribute__((always_inline)) 
__thread_context_save(void) {
	register void * ptr asm("r1") = (void *)&monitor_rt;
	register void * ctx asm("r0");
	asm volatile ("mrs   %0, PSP\n" 
				  "stmdb %0!, {r4-r11}\n"
				  "str   lr, [%1, #8]\n" /* thread return */
				  "str   %0, [%1, #12]\n" /* thread context */
				  : "=r" (ctx) : "r" (ptr));
	return ctx;
}


static inline void __attribute__((always_inline)) 
__monitor_context_restore(void) {
	register void * ptr asm("r0") = (void *)&monitor_rt;
	asm volatile ("str    sp, [%0, #4]\n" /* exceptions SP */
				  "ldr    sp, [%0, #0]\n" /* monitor SP */
				  "pop    {r1}\n"
				  "msr    APSR_nzcvq, r1\n"
				  "pop    {r0-r12,lr}\n"
				  "pop    {pc}\n"
				  : : "r" (ptr));
}

static inline void __attribute__((always_inline)) __wait(void) {
	asm volatile ("mov    r3, #1\n"
				  "0:\n"
				  "cbz	r3, 1f\n"
				  "b.n  0b\n"
				  "1:\n" : : : "r3"); 
}

uint32_t monitor_wait(uint32_t mask)
{
	register void * ptr asm("r0") = (void *)&monitor_rt;
	uint32_t ret;

	while ((ret = (monitor_rt.events & mask)) ==0 ) {
	
		__wait();

	/* save monitor context, restore exception stack and thread context */
	asm volatile ("sub    sp, #4\n"    /* make room for PC */
				  "push   {r0-r12}\n"
				  "mrs    r1, APSR\n"
				  "push   {r1}\n"
				  "add    r1, pc, #28\n"
				  "str    r1, [sp, #4 * 15]\n"
				  "str    sp, [%0, #0]\n" /* monitor SP */
				  "ldr    sp, [%0, #4]\n" /* exceptions SP */
				  "ldr    r1, [%0, #8]\n" /* thread return */
				  "ldr    r2, [%0, #12]\n" /* thread context */
				  "add    r3, r2, #8 * 4\n"
				  "msr    PSP, r3\n"      /* thread SP */
				  "ldmia  r0, {r4-r11}\n"
				  "bx     r1\n"
				  "0:\n"
				  : : "r" (ptr) : );
	}

	/* clear events */
	monitor_rt.events &= ~ret;

	return ret;
}

static inline void __attribute__((always_inline)) 
__thread_context_restore(void) {
	register void * ptr asm("r0") = (void *)&monitor_rt;
	asm volatile ("ldr    r1, [%0, #8]\n" /* thread return */
				  "ldr    r2, [%0, #12]\n" /* thread context */
				  "add    r3, r2, #8 * 4\n"
				  "msr    PSP, r3\n"      /* thread SP */
				  "ldmia  r0, {r4-r11}\n"
				  "bx     r1\n"
				  : : "r" (ptr) : );
}
#endif
