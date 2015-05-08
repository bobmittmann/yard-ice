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

_Pragma ("GCC optimize (\"Ofast\")")

#include <sys/stm32f.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arch/cortex-m3.h>
#include <sys/delay.h>

#define __THINKOS_SYS__
#include <thinkos_sys.h>
#define __THINKOS_DMON__
#include <thinkos_dmon.h>

#include <sys/dcclog.h>

#if THINKOS_ENABLE_EXCEPTIONS

#ifndef THINKOS_SYSRST_ONFAULT
#define THINKOS_SYSRST_ONFAULT 0
#endif

#ifndef THINKOS_STDERR_FAULT_DUMP
#define THINKOS_STDERR_FAULT_DUMP 0
#endif

#ifndef THINKOS_ENABLE_MPU
#define THINKOS_ENABLE_MPU 0
#endif

#ifndef THINKOS_ENABLE_BUSFAULT
#define THINKOS_ENABLE_BUSFAULT 0
#endif

#ifndef THINKOS_ENABLE_USAGEFAULT 
#define THINKOS_ENABLE_USAGEFAULT 0
#endif


#if (THINKOS_ENABLE_DEBUG_FAULT)

#undef THINKOS_ENABLE_MPU
#define THINKOS_ENABLE_MPU 1

#undef THINKOS_ENABLE_BUSFAULT
#define THINKOS_ENABLE_BUSFAULT 1

#undef THINKOS_ENABLE_USAGEFAULT 
#define THINKOS_ENABLE_USAGEFAULT 1

#endif


static inline void __attribute__((always_inline)) 
__xcpt_context_save(struct thinkos_except * xcpt)
{
	register uint32_t * ctx asm("r0");
	register uint32_t sp asm("r1");
	register uint32_t tmp asm("r2");
	register uint32_t lr asm("r3");

	ctx = (uint32_t *)&xcpt->ctx;

	asm volatile ("mov    %2, %3\n"
				  "stmia  %2, {r4-r11}\n"
				  "add    %2, %2, #32\n"
				  "tst    lr, #4\n" 
				  "ite    eq\n" 
				  "mrseq  %0, MSP\n" 
				  "mrsne  %0, PSP\n" 
				  "ldmia  %0, {r4-r11}\n"
				  "stmia  %2, {r4-r11}\n"
				  "mov    %1, lr\n"
				  : "=r" (sp), "=r" (lr), "=r" (tmp) : "r" (ctx) );

	xcpt->ret = lr;
	xcpt->ipsr = cm3_ipsr_get();
	xcpt->sp = sp - 8 * 4;

	if (lr == CM3_EXC_RET_THREAD_PSP) {
		xcpt->psp = xcpt->sp;
	} else {
		xcpt->psp = cm3_psp_get();
	}

	xcpt->msp = cm3_msp_get() - 4 * 8;
}

static inline void __attribute__((always_inline, noreturn)) 
__xcpt_context_restore(struct thinkos_except * xcpt)
{
	register uint32_t * ctx asm("r0");
	register uint32_t sp asm("r1");
	register uint32_t lr asm("r3");

	ctx = (uint32_t *)&xcpt->ctx;
	sp = xcpt->sp;
	lr = xcpt->ret;

	asm volatile ("add    %2, %2, #32\n"
				  "ldmia  %2, {r4-r11}\n"
				  "stmia  %0, {r4-r11}\n"
				  "sub    %2, %2, #32\n"
				  "ldmia  %2, {r4-r11}\n"
				  "mov    lr, %1\n"
				  "bx     lr\n"
				  : : "r" (sp), "r" (lr), "r" (ctx) );
	for(;;);
}

struct cm3_isr_sf {
	uint32_t r0;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;
	uint32_t r12;
	uint32_t lr;
	uint32_t pc;
	uint32_t xpsr;
};

void __idump(const char * s, uint32_t sp, uint32_t ret)
{
	uint32_t shcsr;
	uint32_t icsr;

	DCC_LOG1(LOG_TRACE, "%s() _________________________________", s); 

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
	DCC_LOG5(LOG_TRACE, " ICSR={%s%s%s VECTPENDING=%d VECTACTIVE=%d }", 
				 (icsr & SCB_ICSR_ISRPREEMPT) ? " ISRPREEMPT" : "",
				 (icsr & SCB_ICSR_ISRPENDING) ? " PENDSVACT" : "",
				 (icsr & SCB_ICSR_RETTOBASE) ? " RETTOBASE" : "",
				 (icsr & SCB_ICSR_VECTPENDING) >> 12,
				 (icsr & SCB_ICSR_VECTACTIVE));

	DCC_LOG2(LOG_TRACE, "   SP=%08x RET=%08x", sp, ret); 

	if (cm3_ipsr_get() != (icsr & SCB_ICSR_VECTACTIVE))
		DCC_LOG(LOG_ERROR, "IPSR != ICSR.VECTACTIVE");
}

void __attribute__((naked, noreturn))__xcpt_thread(void)
{
	__idump(__func__, cm3_sp_get(), cm3_lr_get());

	/* suspend all threads */
	__thinkos_pause_all();
	/* reset the IDLE thread */
	thinkos_rt.idle_ctx = &thinkos_idle.ctx;
	thinkos_idle.ctx.pc = (uint32_t)thinkos_idle_task,
	thinkos_idle.ctx.xpsr = 0x01000000;
	/* set the active thread to void, to flush this context */
//	thinkos_rt.active = THINKOS_THREAD_VOID;

	cm3_cpsie_f();
	cm3_cpsie_i();

	for(;;);
}


void __attribute__((naked, noreturn))__xcpt_handler(void)
{
	struct cm3_isr_sf * sf;
	uint32_t ret = cm3_lr_get();

	__idump(__func__, cm3_sp_get(), ret);

	sf = (struct cm3_isr_sf *)&thinkos_idle.ctx.r0;
	/* reset the IDLE thread */
	sf->r0 = 0x00000000;
	sf->r1 = 0x01000001;
	sf->r2 = 0x02000002;
	sf->r3 = 0x03000003;
	sf->r12 = 0xdeadbeef;
	sf->lr = 0x0badface;
	sf->pc = (uint32_t)__xcpt_thread;
	sf->xpsr = 0x01000000;
	cm3_psp_set((uint32_t)sf);

	asm volatile ("bx   %0\n" : : "r" (ret)); 

	/* return */
	for(;;);
}



void __attribute__((noreturn))__xcpt_return_to_dsr(uint32_t xpsr)
{
	struct cm3_isr_sf * sf;
	uint32_t ret;
	unsigned int ipsr;
//	uint32_t * sp;

	ipsr = xpsr & 0x1ff;

	if (ipsr != 0) {
		int irq;
		if ((irq = (ipsr - 16)) >= 0)
			cm3_irq_disable(irq);

		sf = (struct cm3_isr_sf *)&thinkos_idle.ctx;
		sf->r0 = 0x00000000;
		sf->r1 = 0x00000101;
		sf->r2 = 0x00000202;
		sf->r3 = 0x00000303;
		sf->r12 = 0x00001212;
		sf->lr = CM3_EXC_RET_THREAD_PSP;
		sf->pc = (uint32_t)__xcpt_handler;
		sf->xpsr = xpsr;
		/* Reset exception context */
		cm3_msp_set((uint32_t)sf);
		ret = CM3_EXC_RET_HANDLER;
	} else {
		sf = (struct cm3_isr_sf *)&thinkos_idle.ctx.r0;
		/* reset the IDLE thread */
		sf->r0 = 0x00000000;
		sf->r1 = 0x01010101;
		sf->r2 = 0x02020202;
		sf->r3 = 0x03030303;
		sf->r12 = 0xdeadbeef;
		sf->lr = CM3_EXC_RET_THREAD_PSP;
		sf->pc = (uint32_t)__xcpt_thread;
		sf->xpsr = xpsr;
		cm3_psp_set((uint32_t)sf);
		ret = CM3_EXC_RET_THREAD_PSP;
	}

	/* return */
	asm volatile ("bx   %0\n" : : "r" (ret)); 

	for(;;);
}

#if 0
		/* Clear active exceptions */
		CM3_SCB->shcsr &= ~(SCB_SHCSR_SYSTICKACT |
							SCB_SHCSR_PENDSVACT |
							SCB_SHCSR_MONITORACT |
							SCB_SHCSR_SVCALLACT |
							SCB_SHCSR_USGFAULTACT |
							SCB_SHCSR_BUSFAULTACT |
							SCB_SHCSR_MEMFAULTACT);
#endif


#if THINKOS_STDERR_FAULT_DUMP
void __show_ctrl(uint32_t ctrl)
{
	fprintf(stderr, "[%s ", (ctrl & (1 << 25)) ? "PSP" : "MSP");
	fprintf(stderr, "%s ", (ctrl & (1 << 24)) ? "USER" : "PRIV");
	fprintf(stderr, "PM=%c ", ((ctrl >> 0) & 0x01) + '0');
	fprintf(stderr, "FM=%c ", ((ctrl >> 16) & 0x01) + '0');
	fprintf(stderr, "BPRI=%02x] ", (ctrl >> 8) & 0xff);
}

static void __show_xpsr(uint32_t psr)
{
	fprintf(stderr, "[N=%c ", ((psr >> 31) & 0x01) + '0');
	fprintf(stderr, "Z=%c ", ((psr >> 30) & 0x01) + '0');
	fprintf(stderr, "C=%c ", ((psr >> 29) & 0x01) + '0');
	fprintf(stderr, "V=%c ", ((psr >> 28) & 0x01) + '0');
	fprintf(stderr, "Q=%c ", ((psr >> 27) & 0x01) + '0');
	fprintf(stderr, "ICI/IT=%02x ", ((psr >> 19) & 0xc0) | 
			((psr >> 10) & 0x3f));
	fprintf(stderr, "XCP=%02x]", psr & 0xff);
}


void print_except_context(struct thinkos_except * xcpt)
{
	__show_xpsr(xcpt->ctx.xpsr);

	fprintf(stderr, "\n");

	fprintf(stderr, "   r0=%08x", xcpt->ctx.r0);
	fprintf(stderr, "   r4=%08x", xcpt->ctx.r4);
	fprintf(stderr, "   r8=%08x", xcpt->ctx.r8);
	fprintf(stderr, "  r12=%08x", xcpt->ctx.r12);
	fprintf(stderr, " xpsr=%08x\n", xcpt->ctx.xpsr);

	fprintf(stderr, "   r1=%08x", xcpt->ctx.r1);
	fprintf(stderr, "   r5=%08x", xcpt->ctx.r5);
	fprintf(stderr, "   r9=%08x", xcpt->ctx.r9);
	fprintf(stderr, "   sp=%08x", xcpt->sp);
	fprintf(stderr, "  msp=%08x\n", xcpt->msp);

	fprintf(stderr, "   r2=%08x", xcpt->ctx.r2);
	fprintf(stderr, "   r6=%08x", xcpt->ctx.r6);
	fprintf(stderr, "  r10=%08x", xcpt->ctx.r10);
	fprintf(stderr, "   lr=%08x",  xcpt->ctx.lr);
	fprintf(stderr, "  psp=%08x\n", xcpt->psp);

	fprintf(stderr, "   r3=%08x",  xcpt->ctx.r3);
	fprintf(stderr, "   r7=%08x",  xcpt->ctx.r7);
	fprintf(stderr, "  r11=%08x",  xcpt->ctx.r11);
	fprintf(stderr, "   pc=%08x\n",  xcpt->ctx.pc);
}

static void __dump_bfsr(void)
{
	struct cm3_scb * scb = CM3_SCB;
	uint32_t bfsr;

	bfsr = SCB_CFSR_BFSR_GET(scb->cfsr);

	fprintf(stderr, "BFSR=0X%08x", bfsr);

	if (bfsr & BFSR_BFARVALID)  
		fprintf(stderr, " BFARVALID");
	if (bfsr & BFSR_LSPERR)
		fprintf(stderr, " LSPERR");
	if (bfsr & BFSR_STKERR)  
		fprintf(stderr, " STKERR");
	if (bfsr & BFSR_UNSTKERR)  
		fprintf(stderr, " UNSTKERR");
	if (bfsr & BFSR_IMPRECISERR)  
		fprintf(stderr, " IMPRECISERR");
	if (bfsr & BFSR_PRECISERR)
		fprintf(stderr, " PRECISERR");
	if (bfsr & BFSR_IBUSERR)  
		fprintf(stderr, " IBUSERR");

	if (bfsr & BFSR_BFARVALID)  {
		fprintf(stderr, "\n * ADDR = 0x%08x", (int)scb->bfar);
	}
}

static void __dump_ufsr(void)
{
	struct cm3_scb * scb = CM3_SCB;
	uint32_t ufsr;

	ufsr = SCB_CFSR_UFSR_GET(scb->cfsr);

	fprintf(stderr, "UFSR=0x%08x", ufsr);

	if (ufsr & UFSR_DIVBYZERO)  
		fprintf(stderr, " DIVBYZERO");
	if (ufsr & UFSR_UNALIGNED)  
		fprintf(stderr, " UNALIGNED");
	if (ufsr & UFSR_NOCP)  
		fprintf(stderr, " NOCP");
	if (ufsr & UFSR_INVPC)  
		fprintf(stderr, " INVPC");
	if (ufsr & UFSR_INVSTATE)  
		fprintf(stderr, " INVSTATE");
	if (ufsr & UFSR_UNDEFINSTR)  
		fprintf(stderr, " UNDEFINSTR");
}
#endif

static void __xdump(struct thinkos_except * xcpt)
{
	unsigned int ipsr;
	uint32_t shcsr;
	uint32_t icsr;

	ipsr = xcpt->ctx.xpsr & 0x1ff;
	switch (ipsr) {
	case 0:
		DCC_LOG(LOG_ERROR, "Thread!");
		break;
	case CM3_EXCEPT_USAGE_FAULT:
		DCC_LOG(LOG_ERROR, "UsageFault!");
		break;
	case CM3_EXCEPT_BUS_FAULT:
		DCC_LOG(LOG_ERROR, "BusFault!");
		break;
	case CM3_EXCEPT_MEM_MANAGE:
		DCC_LOG(LOG_ERROR, "MemManage!");
		break;
	case CM3_EXCEPT_PENDSV:
		DCC_LOG(LOG_ERROR, "PendSV!");
		break;
	case CM3_EXCEPT_SVC:
		DCC_LOG(LOG_ERROR, "SVCall!");
		break;
	case CM3_EXCEPT_DEBUG_MONITOR:
		DCC_LOG(LOG_ERROR, "Monitor!");
		break;
	case CM3_EXCEPT_SYSTICK:
		DCC_LOG(LOG_ERROR, "SysTick!");
		break;
	default:
		DCC_LOG1(LOG_ERROR, "IRQ %d!", ipsr - 16);
		break;
	}

	DCC_LOG1(LOG_ERROR, " IPSR=%03x", xcpt->ipsr);
	shcsr = CM3_SCB->shcsr;
	DCC_LOG7(LOG_ERROR, "SHCSR: {%s%s%s%s%s%s%s }", 
				 (shcsr & SCB_SHCSR_SYSTICKACT) ? " SYSTICKACT" : "",
				 (shcsr & SCB_SHCSR_PENDSVACT) ? " PENDSVACT" : "",
				 (shcsr & SCB_SHCSR_MONITORACT) ? " MONITORACT" : "",
				 (shcsr & SCB_SHCSR_SVCALLACT) ? " SVCALLACT" : "",
				 (shcsr & SCB_SHCSR_USGFAULTACT) ?  " USGFAULTACT" : "",
				 (shcsr & SCB_SHCSR_BUSFAULTACT) ?  " BUSFAULTACT" : "",
				 (shcsr & SCB_SHCSR_MEMFAULTACT) ?  " MEMFAULTACT" : "");

	icsr = CM3_SCB->icsr;
	DCC_LOG5(LOG_ERROR, "ICSR: {%s%s%s VECTPENDING=%d VECTACTIVE=%d }", 
				 (icsr & SCB_ICSR_ISRPREEMPT) ? " ISRPREEMPT" : "",
				 (icsr & SCB_ICSR_ISRPENDING) ? " PENDSVACT" : "",
				 (icsr & SCB_ICSR_RETTOBASE) ? " RETTOBASE" : "",
				 (icsr & SCB_ICSR_VECTPENDING) >> 12,
				 (icsr & SCB_ICSR_VECTACTIVE));

	DCC_LOG4(LOG_ERROR, "  R0=%08x  R1=%08x  R2=%08x  R3=%08x", 
			xcpt->ctx.r0, xcpt->ctx.r1, xcpt->ctx.r2, xcpt->ctx.r3);
	DCC_LOG4(LOG_ERROR, "  R4=%08x  R5=%08x  R6=%08x  R7=%08x", 
			xcpt->ctx.r4, xcpt->ctx.r5, xcpt->ctx.r6, xcpt->ctx.r7);
	DCC_LOG4(LOG_ERROR, "  R8=%08x  R9=%08x R10=%08x R11=%08x", 
			xcpt->ctx.r8, xcpt->ctx.r9, xcpt->ctx.r10, xcpt->ctx.r11);
	DCC_LOG4(LOG_ERROR, " R12=%08x  SP=%08x  LR=%08x  PC=%08x", 
			xcpt->ctx.r12, xcpt->sp, xcpt->ctx.lr, xcpt->ctx.pc);
	DCC_LOG4(LOG_ERROR, "XPSR=%08x MSP=%08x PSP=%08x RET=%08x", 
			xcpt->ctx.xpsr, xcpt->msp, xcpt->psp, xcpt->ret);
}

void __hard_fault(struct thinkos_except * xcpt)
{
	struct cm3_scb * scb = CM3_SCB;
	uint32_t hfsr;

	hfsr = scb->hfsr;

	DCC_LOG3(LOG_ERROR, "Hard fault:%s%s%s", 
			 (hfsr & SCB_HFSR_DEBUGEVT) ? " DEBUGEVT" : "",
			 (hfsr & SCB_HFSR_FORCED) ?  " FORCED" : "",
			 (hfsr & SCB_HFSR_VECTTBL) ? " VECTTBL" : "");
	__xdump(xcpt);
	DCC_LOG1(LOG_ERROR, "HFSR=%08x", scb->hfsr);
	DCC_LOG1(LOG_ERROR, "CFSR=%08x", scb->cfsr);
	DCC_LOG1(LOG_ERROR, "BFAR=%08x", scb->bfar);

	if (hfsr & SCB_HFSR_FORCED) {
		uint32_t bfsr;
		uint32_t ufsr;

		bfsr = SCB_CFSR_BFSR_GET(scb->cfsr);
		ufsr = SCB_CFSR_UFSR_GET(scb->cfsr);
		(void)bfsr;
		(void)ufsr;

		DCC_LOG1(LOG_ERROR, "BFSR=%08X", bfsr);
		if (bfsr) {
			DCC_LOG7(LOG_ERROR, "    %s%s%s%s%s%s%s", 
					 (bfsr & BFSR_BFARVALID) ? " BFARVALID" : "",
					 (bfsr & BFSR_LSPERR) ? " LSPERR" : "",
					 (bfsr & BFSR_STKERR) ? " STKERR" : "",
					 (bfsr & BFSR_UNSTKERR) ?  " UNSTKERR" : "",
					 (bfsr & BFSR_IMPRECISERR) ?  " IMPRECISERR" : "",
					 (bfsr & BFSR_PRECISERR) ?  " PRECISERR" : "",
					 (bfsr & BFSR_IBUSERR)  ?  " IBUSERR" : "");
		}

		DCC_LOG1(LOG_ERROR, "UFSR=%08X", ufsr);
		if (ufsr) {
			DCC_LOG6(LOG_ERROR, "    %s%s%s%s%s%s", 
					 (ufsr & UFSR_DIVBYZERO)  ? " DIVBYZERO" : "",
					 (ufsr & UFSR_UNALIGNED)  ? " UNALIGNED" : "",
					 (ufsr & UFSR_NOCP)  ? " NOCP" : "",
					 (ufsr & UFSR_INVPC)  ? " INVPC" : "",
					 (ufsr & UFSR_INVSTATE)  ? " INVSTATE" : "",
					 (ufsr & UFSR_UNDEFINSTR)  ? " UNDEFINSTR" : "");
		}
	}

#if THINKOS_STDERR_FAULT_DUMP
	fprintf(stderr, "\n---\n");
	fprintf(stderr, "Hard fault:");

	if (hfsr & SCB_HFSR_DEBUGEVT)  
		fprintf(stderr, " DEBUGEVT");
	if (hfsr & SCB_HFSR_FORCED)  
		fprintf(stderr, " FORCED");
	if (hfsr & SCB_HFSR_VECTTBL)  
		fprintf(stderr, " VECTTBL");

	if (hfsr & SCB_HFSR_DEBUGEVT)  
		fprintf(stderr, " DEBUGEVT");

	fprintf(stderr, "\n");

	if (hfsr & SCB_HFSR_FORCED) {
		__dump_bfsr();
		fprintf(stderr, "\n");
		__dump_ufsr();
		fprintf(stderr, "\n");
	}

	print_except_context(xcpt);
	fprintf(stderr, "\n");
	fflush(stderr);
#endif
}

#if	THINKOS_ENABLE_BUSFAULT 
void __bus_fault(struct thinkos_except * xcpt)
{
	struct cm3_scb * scb = CM3_SCB;
	uint32_t bfsr = SCB_CFSR_BFSR_GET(scb->cfsr);
	(void)bfsr;

	DCC_LOG(LOG_ERROR, "Bus fault!");
	DCC_LOG1(LOG_ERROR, "BFSR=%08X", bfsr);
	if (bfsr) {
		DCC_LOG7(LOG_ERROR, "    %s%s%s%s%s%s%s", 
				 (bfsr & BFSR_BFARVALID) ? " BFARVALID" : "",
				 (bfsr & BFSR_LSPERR) ? " LSPERR" : "",
				 (bfsr & BFSR_STKERR) ? " STKERR" : "",
				 (bfsr & BFSR_UNSTKERR) ?  " UNSTKERR" : "",
				 (bfsr & BFSR_IMPRECISERR) ?  " IMPRECISERR" : "",
				 (bfsr & BFSR_PRECISERR) ?  " PRECISERR" : "",
				 (bfsr & BFSR_IBUSERR)  ?  " IBUSERR" : "");
	}
	DCC_LOG1(LOG_ERROR, "BFAR=%08x", scb->bfar);
	__xdump(xcpt);

#if THINKOS_STDERR_FAULT_DUMP
	fprintf(stderr, "\n---\n");
	fprintf(stderr, "Bus fault:");

	__dump_bfsr();

	print_except_context(xcpt);
	fprintf(stderr, "\n");
	fflush(stderr);
#endif
}
#endif /* THINKOS_ENABLE_BUSFAULT  */

#if	THINKOS_ENABLE_USAGEFAULT 
void __usage_fault(struct thinkos_except * xcpt)
{
	struct cm3_scb * scb = CM3_SCB;
	uint32_t ufsr = SCB_CFSR_UFSR_GET(scb->cfsr);
	(void)ufsr;

	DCC_LOG(LOG_ERROR, "Usage fault!");
	DCC_LOG1(LOG_ERROR, "UFSR=%08X", ufsr);
	if (ufsr) {
		DCC_LOG6(LOG_ERROR, "    %s%s%s%s%s%s", 
				 (ufsr & UFSR_DIVBYZERO)  ? " DIVBYZERO" : "",
				 (ufsr & UFSR_UNALIGNED)  ? " UNALIGNED" : "",
				 (ufsr & UFSR_NOCP)  ? " NOCP" : "",
				 (ufsr & UFSR_INVPC)  ? " INVPC" : "",
				 (ufsr & UFSR_INVSTATE)  ? " INVSTATE" : "",
				 (ufsr & UFSR_UNDEFINSTR)  ? " UNDEFINSTR" : "");
	}
	__xdump(xcpt);

#if THINKOS_STDERR_FAULT_DUMP
	fprintf(stderr, "\n---\n");
	fprintf(stderr, "Usage fault:");

	__dump_ufsr();

	print_except_context(xcpt);
	fprintf(stderr, "\n");
	fflush(stderr);
#endif
}
#endif /* THINKOS_ENABLE_USAGEFAULT  */

#if THINKOS_ENABLE_MPU
void __mem_manag(struct thinkos_except * xcpt)
{
	DCC_LOG(LOG_ERROR, "Mem Management!");
	__xdump(xcpt);

#if THINKOS_STDERR_FAULT_DUMP
	fprintf(stderr, "\n---\n");
	fprintf(stderr, "Mem Manager:");
	print_except_context(xcpt);
	fprintf(stderr, "\n");
	fflush(stderr);
#endif
}
#endif

/* -------------------------------------------------------------------------
   Fault handlers 
   ------------------------------------------------------------------------- */

void thinkos_default_exception_dsr(struct thinkos_except * xcpt);

struct thinkos_except thinkos_except_buf;

#if	THINKOS_ENABLE_BUSFAULT 
void __attribute__((naked, noreturn)) cm3_bus_fault_isr(void)
{
	cm3_cpsid_f();
	cm3_cpsid_i();
	__xcpt_context_save(&thinkos_except_buf);
	thinkos_except_buf.type = CM3_EXCEPT_BUS_FAULT;
	__bus_fault(&thinkos_except_buf);
	__xcpt_return_to_dsr(thinkos_except_buf.ctx.xpsr);
//	thinkos_exception_dsr(&thinkos_except_buf);
//	__xcpt_context_restore(&thinkos_except_buf);
}
#endif

#if	THINKOS_ENABLE_USAGEFAULT 
void __attribute__((naked, noreturn)) cm3_usage_fault_isr(void)
{
	__xcpt_context_save(&thinkos_except_buf);
	thinkos_except_buf.type = CM3_EXCEPT_USAGE_FAULT;
	__usage_fault(&thinkos_except_buf);

//	__xcpt_return_to_dsr(thinkos_except_buf.xpsr);
//	thinkos_exception_dsr(&thinkos_except_buf);

	for(;;);
//	__xcpt_context_restore(&thinkos_except_buf);
}
#endif

#if THINKOS_ENABLE_MPU
void __attribute__((naked, noreturn)) cm3_mem_manage_isr(void)
{
	__xcpt_context_save(&thinkos_except_buf);
	thinkos_except_buf.type = CM3_EXCEPT_MEM_MANAGE;
	__mem_manag(&thinkos_except_buf);

	__xcpt_return_to_dsr(thinkos_except_buf.ctx.xpsr);
//	thinkos_exception_dsr(&thinkos_except_buf);
//	__xcpt_context_restore(&thinkos_except_buf);
}
#endif

void __attribute__((naked, noreturn)) cm3_hard_fault_isr(void)
{
	__xcpt_context_save(&thinkos_except_buf);
	thinkos_except_buf.type = CM3_EXCEPT_HARD_FAULT;
	__hard_fault(&thinkos_except_buf);
//	thinkos_exception_dsr(&thinkos_except_buf);
#if THINKOS_SYSRST_ONFAULT
	cm3_sysrst();
#else
	for(;;);
#endif
}

/* -------------------------------------------------------------------------
   Application fault defered handler 
   ------------------------------------------------------------------------- */

void thinkos_default_exception_dsr(struct thinkos_except * xcpt)
{
#if THINKOS_SYSRST_ONFAULT
	cm3_sysrst();
#else
	for(;;);
#endif
}

void thinkos_exception_dsr(struct thinkos_except *) 
	__attribute__((weak, alias("thinkos_default_exception_dsr")));


void __exception_reset(void)
{
	__thinkos_memset32(&thinkos_except_buf, 0x00000000,
					   sizeof(struct thinkos_except));
	thinkos_except_buf.thread_id = -1;
}

void thinkos_exception_init(void)
{
	struct cm3_scb * scb = CM3_SCB;
	scb->shcsr = 0

#if	THINKOS_ENABLE_USAGEFAULT 
	| SCB_SHCSR_USGFAULTENA 
#endif
#if	THINKOS_ENABLE_BUSFAULT 
	| SCB_SHCSR_BUSFAULTENA
#endif
#if THINKOS_ENABLE_MPU
	| SCB_SHCSR_MEMFAULTENA;
#endif
	;
	__exception_reset();
}

#endif /* THINKOS_ENABLE_EXCEPTIONS */

#if 0
void __attribute__((naked, noreturn)) cm3_nmi_isr(void)
{
	struct thinkos_context * ctx;
	uint32_t msp;
	uint32_t psp;
	uint32_t lr;

	/* save the context */
	ctx = __get_context();
	
	lr = cm3_lr_get();
	msp = cm3_msp_get();
	psp = cm3_psp_get();

	cm3_faultmask_set(1);

	thinkos_nmi(ctx, msp, psp, lr);
}

void __attribute__((naked, noreturn)) cm3_debug_mon_isr(void)
{
	struct thinkos_context * ctx;
	uint32_t msp;
	uint32_t psp;
	uint32_t lr;

	/* save the context */
	ctx = __get_context();
	
	lr = cm3_lr_get();
	msp = cm3_msp_get();
	psp = cm3_psp_get();

	cm3_faultmask_set(1);

	thinkos_mon(ctx, msp, psp, lr);

	thinkos_exception_dsr(ctx);
}
#endif

#if 0
void thinkos_nmi(struct thinkos_context * ctx, uint32_t msp, 
				 uint32_t psp, uint32_t lr)
{
	uint32_t sp;
	(void)sp;

	if (lr & (1 << 4))
		sp = psp;
	else
		sp = msp;

	DCC_LOG(LOG_ERROR, "NMI");
	DCC_LOG4(LOG_ERROR, "  R0=%08x  R1=%08x  R2=%08x  R3=%08x", 
			 ctx->r0, ctx->r1, ctx->r2, ctx->r3);
	DCC_LOG4(LOG_ERROR, "  R4=%08x  R5=%08x  R6=%08x  R7=%08x", 
			 ctx->r4, ctx->r5, ctx->r6, ctx->r7);
	DCC_LOG4(LOG_ERROR, "  R8=%08x  R9=%08x R10=%08x R11=%08x", 
			 ctx->r8, ctx->r9, ctx->r10, ctx->r11);
	DCC_LOG4(LOG_ERROR, " R12=%08x  SP=%08x  LR=%08x  PC=%08x", 
			 ctx->r12, sp, ctx->lr, ctx->pc);
	DCC_LOG4(LOG_ERROR, "XPSR=%08x MSP=%08x PSP=%08x RET=%08x", 
			 ctx->xpsr, msp, psp, lr);

#if THINKOS_STDERR_FAULT_DUMP
	fprintf(stderr, "\n---\n");
	fprintf(stderr, "NMI:");
	print_except_context(xcpt);
	fprintf(stderr, "\n");
	fflush(stderr);
#endif
	for(;;);
}

void thinkos_mon(struct thinkos_context * ctx, uint32_t msp, 
				 uint32_t psp, uint32_t lr)
{
	uint32_t sp;
	(void)sp;

	if (lr & (1 << 4))
		sp = psp;
	else
		sp = msp;

	DCC_LOG(LOG_ERROR, "Debug Monitor");
	DCC_LOG4(LOG_ERROR, "  R0=%08x  R1=%08x  R2=%08x  R3=%08x", 
			ctx->r0, ctx->r1, ctx->r2, ctx->r3);
	DCC_LOG4(LOG_ERROR, "  R4=%08x  R5=%08x  R6=%08x  R7=%08x", 
			ctx->r4, ctx->r5, ctx->r6, ctx->r7);
	DCC_LOG4(LOG_ERROR, "  R8=%08x  R9=%08x R10=%08x R11=%08x", 
			ctx->r8, ctx->r9, ctx->r10, ctx->r11);
	DCC_LOG4(LOG_ERROR, " R12=%08x  SP=%08x  LR=%08x  PC=%08x", 
			ctx->r12, sp, ctx->lr, ctx->pc);
	DCC_LOG4(LOG_ERROR, "XPSR=%08x MSP=%08x PSP=%08x RET=%08x", 
			ctx->xpsr, msp, psp, lr);

#if THINKOS_STDERR_FAULT_DUMP
	fprintf(stderr, "\n---\n");
	fprintf(stderr, "Debug Monitor:");
	print_except_context(xcpt);
	fprintf(stderr, "\n");
	fflush(stderr);
#endif
	for(;;);
}

#endif
