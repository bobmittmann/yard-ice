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

_Pragma ("GCC optimize (\"O2\")")

#define __THINKOS_SYS__
#include <thinkos_sys.h>
#include <thinkos.h>

const char __xcpt_name_lut[16][12] = {
	"Thread",
	"Reset",
	"NMI",
	"HardFault",
	"MemManage",
	"BusFault",
	"UsageFault",
	"Invalid 7",
	"Invalid 8",
	"Invalid 9",
	"Invalid 10",
	"SVCall",
	"DbgMonitor",
	"Invalid 13",
	"PendSV",
	"SysTick"
};

int __scan_stack(void * stack, unsigned int size)
{
	uint32_t * ptr = (uint32_t *)stack;
	int i;

	for (i = 0; i < size / 4; ++i) {
		if (ptr[i] != 0xdeadbeef)
			break;
	}

	return i * 4;
}

int __scan_stack(void * stack, unsigned int size);
extern uint32_t thinkos_dmon_stack[];
extern const uint16_t thinkos_dmon_stack_size;


void __xdump(struct thinkos_except * xcpt)
{
#ifdef DEBUG
	uint32_t shcsr;
	uint32_t icsr;
	uint32_t ipsr;
	uint32_t xpsr;
	uint32_t sp;
	int irqmax;
	int irqbits;
	int i;
	int j;

	irqmax = CM3_ICTR;
	for (i = 0; i < irqmax ; ++i) {
		irqbits = __rbit(CM3_NVIC->iabr[i]);
		while ((j = __clz(irqbits)) < 32) {
			irqbits &= ~(0x80000000 >> j);
			DCC_LOG1(LOG_TRACE, "Active IRQ=%d", i * 32 + j); 
		}
	}

	for (i = 0; i < irqmax ; ++i) {
		irqbits = __rbit(CM3_NVIC->ispr[i]);
		while ((j = __clz(irqbits)) < 32) {
			irqbits &= ~(0x80000000 >> j);
			DCC_LOG1(LOG_TRACE, "Pending IRQ=%d", i * 32 + j); 
		}
	}

	sp = (xcpt->ret == CM3_EXC_RET_THREAD_PSP) ? xcpt->psp : xcpt->msp;

	DCC_LOG4(LOG_ERROR, "   R0=%08x  R1=%08x  R2=%08x  R3=%08x", 
			xcpt->ctx.r0, xcpt->ctx.r1, xcpt->ctx.r2, xcpt->ctx.r3);
	DCC_LOG4(LOG_ERROR, "   R4=%08x  R5=%08x  R6=%08x  R7=%08x", 
			xcpt->ctx.r4, xcpt->ctx.r5, xcpt->ctx.r6, xcpt->ctx.r7);
	DCC_LOG4(LOG_ERROR, "   R8=%08x  R9=%08x R10=%08x R11=%08x", 
			xcpt->ctx.r8, xcpt->ctx.r9, xcpt->ctx.r10, xcpt->ctx.r11);
	DCC_LOG4(LOG_ERROR, "  R12=%08x  SP=%08x  LR=%08x  PC=%08x", 
			xcpt->ctx.r12, sp, xcpt->ctx.lr, xcpt->ctx.pc);
	DCC_LOG4(LOG_ERROR, " XPSR=%08x MSP=%08x PSP=%08x RET=%08x", 
			xcpt->ctx.xpsr, xcpt->msp, xcpt->psp, xcpt->ret);
	xpsr = xcpt->ctx.xpsr;
	ipsr = xpsr & 0x1ff;
	if (ipsr < 16) { 
		DCC_LOG10(LOG_ERROR, " XPSR={ %c%c%c%c%c %c "
				 "ICI/IT=%02x GE=%1x IPSR=%d (%s) }", 
				 (xpsr & (1 << 31)) ? 'N' : '.',
				 (xpsr & (1 << 30)) ? 'Z' : '.',
				 (xpsr & (1 << 29)) ? 'C' : '.',
				 (xpsr & (1 << 28)) ? 'V' : '.',
				 (xpsr & (1 << 27)) ? 'Q' : '.',
				 (xpsr & (1 << 24)) ? 'T' : '.',
				 ((xpsr >> 19) & 0xc0) | ((xpsr >> 10) & 0x3f),
				 ((xpsr >> 16) & 0x0f),
				 ipsr, __xcpt_name_lut[ipsr]);
	} else {
		DCC_LOG10(LOG_ERROR, " XPSR={ %c%c%c%c%c %c "
				 "ICI/IT=%02x GE=%1x IPSR=%d (IRQ %d) }", 
				 (xpsr & (1 << 31)) ? 'N' : '.',
				 (xpsr & (1 << 30)) ? 'Z' : '.',
				 (xpsr & (1 << 29)) ? 'C' : '.',
				 (xpsr & (1 << 28)) ? 'V' : '.',
				 (xpsr & (1 << 27)) ? 'Q' : '.',
				 (xpsr & (1 << 24)) ? 'T' : '.',
				 ((xpsr >> 19) & 0xc0) | ((xpsr >> 10) & 0x3f),
				 ((xpsr >> 16) & 0x0f),
				 ipsr, ipsr - 16);
	}
	shcsr = CM3_SCB->shcsr;
	DCC_LOG7(LOG_ERROR, "SHCSR={%s%s%s%s%s%s%s }", 
				 (shcsr & SCB_SHCSR_SYSTICKACT) ? " SYSTICKACT" : "",
				 (shcsr & SCB_SHCSR_PENDSVACT) ? " PENDSVACT" : "",
				 (shcsr & SCB_SHCSR_MONITORACT) ? " MONITORACT" : "",
				 (shcsr & SCB_SHCSR_SVCALLACT) ? " SVCALLACT" : "",
				 (shcsr & SCB_SHCSR_USGFAULTACT) ?  " USGFAULTACT" : "",
				 (shcsr & SCB_SHCSR_BUSFAULTACT) ?  " BUSFAULTACT" : "",
				 (shcsr & SCB_SHCSR_MEMFAULTACT) ?  " MEMFAULTACT" : "");

	icsr = CM3_SCB->icsr;
	DCC_LOG8(LOG_ERROR, " ICSR={%s%s%s%s%s%s VECTPENDING=%d VECTACTIVE=%d }", 
				 (icsr & SCB_ICSR_NMIPENDSET) ? " NMIPEND" : "",
				 (icsr & SCB_ICSR_PENDSVSET) ? " PENDSV" : "",
				 (icsr & SCB_ICSR_PENDSTSET) ? " PENDST" : "",
				 (icsr & SCB_ICSR_ISRPREEMPT) ? " ISRPREEMPT" : "",
				 (icsr & SCB_ICSR_ISRPENDING) ? " ISRPENDING" : "",
				 (icsr & SCB_ICSR_RETTOBASE) ? " RETTOBASE" : "",
				 (icsr & SCB_ICSR_VECTPENDING) >> 12,
				 (icsr & SCB_ICSR_VECTACTIVE));

	DCC_LOG2(LOG_ERROR, "thread_id=%d active=%d", xcpt->thread_id,
			 thinkos_rt.active); 

#if (THINKOS_ENABLE_MONITOR)
	if (ipsr == CM3_EXCEPT_DEBUG_MONITOR) {
		DCC_LOG2(LOG_ERROR, "DMON stack free: %d/%6d", 
				 __scan_stack(thinkos_dmon_stack, thinkos_dmon_stack_size),
				 thinkos_dmon_stack_size); 
	}
#endif

#endif
}

void __idump(const char * s, uint32_t ipsr)
{
#ifdef DEBUG
	uint32_t shcsr;
	uint32_t icsr;
	int irqmax;
	int irqbits;
	int i;
	int j;

	DCC_LOG1(LOG_TRACE, "%s() _________________________________", s); 

	irqmax = CM3_ICTR;
	for (i = 0; i < irqmax ; ++i) {
		irqbits = __rbit(CM3_NVIC->iabr[i]);
		while ((j = __clz(irqbits)) < 32) {
			irqbits &= ~(0x80000000 >> j);
			DCC_LOG1(LOG_TRACE, "Active IRQ=%d", i * 32 + j); 
		}
	}

	for (i = 0; i < irqmax ; ++i) {
		irqbits = __rbit(CM3_NVIC->ispr[i]);
		while ((j = __clz(irqbits)) < 32) {
			irqbits &= ~(0x80000000 >> j);
			DCC_LOG1(LOG_TRACE, "Pending IRQ=%d", i * 32 + j); 
		}
	}

	if (ipsr < 16) { 
		DCC_LOG2(LOG_TRACE, " IPSR=%d (%s)", ipsr, __xcpt_name_lut[ipsr]);
	} else {
		DCC_LOG2(LOG_TRACE, " IPSR=%d (IRQ %d)", ipsr, ipsr - 16);
	}
	shcsr = CM3_SCB->shcsr;
	DCC_LOG7(LOG_TRACE, "SHCSR={%s%s%s%s%s%s%s }", 
				 (shcsr & SCB_SHCSR_SYSTICKACT) ? " SYSTICKACT" : "",
				 (shcsr & SCB_SHCSR_PENDSVACT) ? " PENDSVACT" : "",
				 (shcsr & SCB_SHCSR_MONITORACT) ? " MONITORACT" : "",
				 (shcsr & SCB_SHCSR_SVCALLACT) ? " SVCALLACT" : "",
				 (shcsr & SCB_SHCSR_USGFAULTACT) ?  " USGFAULTACT" : "",
				 (shcsr & SCB_SHCSR_BUSFAULTACT) ?  " BUSFAULTACT" : "",
				 (shcsr & SCB_SHCSR_MEMFAULTACT) ?  " MEMFAULTACT" : "");

	icsr = CM3_SCB->icsr;
	DCC_LOG8(LOG_TRACE, " ICSR={%s%s%s%s%s%s VECTPENDING=%d VECTACTIVE=%d }", 
				 (icsr & SCB_ICSR_NMIPENDSET) ? " NMIPEND" : "",
				 (icsr & SCB_ICSR_PENDSVSET) ? " PENDSV" : "",
				 (icsr & SCB_ICSR_PENDSTSET) ? " PENDST" : "",
				 (icsr & SCB_ICSR_ISRPREEMPT) ? " ISRPREEMPT" : "",
				 (icsr & SCB_ICSR_ISRPENDING) ? " ISRPENDING" : "",
				 (icsr & SCB_ICSR_RETTOBASE) ? " RETTOBASE" : "",
				 (icsr & SCB_ICSR_VECTPENDING) >> 12,
				 (icsr & SCB_ICSR_VECTACTIVE));

	if (ipsr != (icsr & SCB_ICSR_VECTACTIVE))
		DCC_LOG(LOG_ERROR, "IPSR != ICSR.VECTACTIVE");
#endif
}

void __mpudump(void)
{
#ifdef DEBUG
	struct cm3_mpu * mpu = CM3_MPU;
	uint32_t rbar;
	uint32_t rasr;
	int i;

	DCC_LOG(LOG_TRACE, "MPU _________________________________"); 
	DCC_LOG1(LOG_TRACE, "ctrl=%08x", mpu->ctrl);

	for (i = 0; i < 8; ++i) {
		uint32_t size;

		mpu->rnr = i;
		rasr = mpu->rasr;
		rbar = mpu->rbar;
		DCC_LOG4(LOG_TRACE, "%d rbar=%08x rasr=%08x ADDR=%08x", i, 
				 rbar, rasr, rbar & 0xffffffe0);

		size = (1 << (((rasr >> 1) & 0x1f) + 1)) / 1024;
		if (size > 1024)  
			DCC_LOG9(LOG_TRACE, "  %sAP=%d TEX=%d %s%s%sSRD=%02x %dMB %s",
					 (rasr & (1 << 28)) ? "XN " : "",
					 (rasr >> 24) & 7,
					 (rasr >> 19) & 7,
					 (rasr & (1 << 18)) ? "S " : "",
					 (rasr & (1 << 17)) ? "C " : "",
					 (rasr & (1 << 16)) ? "B " : "",
					 (rasr >> 8) & 0xff,
					 size / 1024,
					 (rasr & 1) ? "enabled" : "disabled");
		else
			DCC_LOG9(LOG_TRACE, "  %sAP=%d TEX=%d %s%s%sSRD=%02x %dKB %s",
					 (rasr & (1 << 28)) ? "XN " : "",
					 (rasr >> 24) & 7,
					 (rasr >> 19) & 7,
					 (rasr & (1 << 18)) ? "S " : "",
					 (rasr & (1 << 17)) ? "C " : "",
					 (rasr & (1 << 16)) ? "B " : "",
					 (rasr >> 8) & 0xff,
					 size,
					 (rasr & 1) ? "enabled" : "disabled");
	}
#endif
}

/* Return the an active interrupt wich is different from the 
   interrupt in the current irq. Otherwise returns -16  */

int __xcpt_next_active_irq(int this_irq)
{
	int irqmax;
	int irqbits;
	int irq;
	int i;
	int j;

	irqmax = CM3_ICTR;
	for (i = 0; i < irqmax ; ++i) {
		irqbits = __rbit(CM3_NVIC->iabr[i]);
		while ((j = __clz(irqbits)) < 32) {
			irq = i * 32 + j;
			if (irq != this_irq)
				return irq;
			irqbits &= ~(0x80000000 >> j);
		}
	}

	return -16;
}

void __xcpt_irq_disable_all(void)
{
	int i;

	for (i = 0; i < CM3_ICTR; ++i)
		CM3_NVIC->icer[i] = 0xffffffff; /* disable all interrupts */
}

void __xcpt_systick_int_disable(void)
{
#if THINKOS_ENABLE_CLOCK || THINKOS_ENABLE_TIMESHARE
	struct cm3_systick * systick = CM3_SYSTICK;

	systick->csr &= ~SYSTICK_CSR_TICKINT;
	CM3_SCB->icsr = SCB_ICSR_PENDSTCLR;
#endif
}

void __xcpt_systick_int_enable(void)
{
#if THINKOS_ENABLE_CLOCK || THINKOS_ENABLE_TIMESHARE
	struct cm3_systick * systick = CM3_SYSTICK;
	systick->csr |= SYSTICK_CSR_TICKINT;
#endif
}


void __tdump(void)
{
#ifdef DEBUG

	int i;

	DCC_LOG1(LOG_TRACE, "Active=%d", thinkos_rt.active);
	for (i = 0; i < THINKOS_THREADS_MAX; ++i) {
		if (thinkos_rt.ctx[i] == NULL)
			continue;
#if THINKOS_ENABLE_THREAD_INFO
		if (thinkos_rt.th_inf[i] != NULL) {
#if THINKOS_ENABLE_THREAD_STAT
			DCC_LOG8(LOG_TRACE, "%7s (%2d %3d) SP=%08x PC=%08x LR=%08x %d/%d", 
					 thinkos_rt.th_inf[i]->tag,
					 i, thinkos_rt.th_stat[i] >> 1,
					 thinkos_rt.ctx[i], 
					 thinkos_rt.ctx[i]->pc, 
					 thinkos_rt.ctx[i]->lr,
					 __scan_stack(thinkos_rt.th_inf[i]->stack_ptr, 
								  thinkos_rt.th_inf[i]->stack_size),
					 thinkos_rt.th_inf[i]->stack_size);
#else
			DCC_LOG7(LOG_TRACE, "%7s (%2d) SP=%08x PC=%08x LR=%08x %d/%d", 
					 thinkos_rt.th_inf[i]->tag,
					 i, 
					 thinkos_rt.ctx[i], 
					 thinkos_rt.ctx[i]->pc, 
					 thinkos_rt.ctx[i]->lr,
					 __scan_stack(thinkos_rt.th_inf[i]->stack_ptr, 
								  thinkos_rt.th_inf[i]->stack_size),
					 thinkos_rt.th_inf[i]->stack_size);
#endif
		} else
#endif
#if THINKOS_ENABLE_THREAD_STAT
		DCC_LOG5(LOG_TRACE, "....... (%2d %3d) SP=%08x PC=%08x LR=%08x", 
				 i, thinkos_rt.th_stat[i] >> 1,
				 thinkos_rt.ctx[i], 
				 thinkos_rt.ctx[i]->pc, 
				 thinkos_rt.ctx[i]->lr);
#else
		DCC_LOG4(LOG_TRACE, "....... (%2d) SP=%08x PC=%08x LR=%08x", 
				 i, thinkos_rt.ctx[i], 
				 thinkos_rt.ctx[i]->pc, 
				 thinkos_rt.ctx[i]->lr);
#endif


	}
	DCC_LOG2(LOG_TRACE, "<IDLE>  (%2d) SP=%08x", i, 
			 thinkos_rt.idle_ctx);
#if THINKOS_ENABLE_EXIT || THINKOS_ENABLE_JOIN
	DCC_LOG2(LOG_TRACE, "<VOID>  (%2d) SP=%08x", i + 1, 
			 thinkos_rt.idle_ctx);
#endif
	DCC_LOG1(LOG_TRACE, "wq_ready=%08x", thinkos_rt.wq_ready);
#if THINKOS_ENABLE_TIMESHARE
	DCC_LOG1(LOG_TRACE, "wq_tmshare=%08x", thinkos_rt.wq_tmshare);
#endif
#if THINKOS_ENABLE_CLOCK
	DCC_LOG1(LOG_TRACE, "wq_clock=%08x", thinkos_rt.wq_clock);
#endif

#endif /* DEBUG */
}

