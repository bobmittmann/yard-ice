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
	xcpt->icsr = CM3_SCB->icsr;
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

void __attribute__((naked)) __xcpt_thread(struct thinkos_except * xcpt)
{
	uint32_t icsr;
	int ipsr;

	__idump(__func__, cm3_ipsr_get());
	DCC_LOG(LOG_TRACE, "end of exception...");

	/* suspend all threads */
	__thinkos_pause_all();

	ipsr = xcpt->ctx.xpsr & 0x1ff;
	icsr = xcpt->icsr;
	if ((icsr & SCB_ICSR_RETTOBASE) || (ipsr == CM3_EXCEPT_SVC)) {
		if ((uint32_t)thinkos_rt.active < THINKOS_THREADS_MAX) {
			/* record the current thread */
			xcpt->thread_id = thinkos_rt.active;
#if THINKOS_ENABLE_DEBUG_FAULT
			/* flag the thread as faulty */
			__bit_mem_wr(&thinkos_rt.wq_fault, thinkos_rt.active, 1);
#endif
		} else {
			DCC_LOG(LOG_ERROR, "invalid active thread ...");
			xcpt->thread_id = -1;
		}
	} else {
		xcpt->thread_id = -1;
	}

	/* set the active thread to void */
	thinkos_rt.active = THINKOS_THREAD_VOID;
	/* reset the IDLE thread */
	thinkos_rt.idle_ctx = &thinkos_idle.ctx;
	thinkos_idle.ctx.pc = (uint32_t)thinkos_idle_task,
	thinkos_idle.ctx.xpsr = 0x01000000;

	if (thinkos_rt.wq_ready != 0) {
		DCC_LOG1(LOG_ERROR, "wq_ready=%08x, ready queue not empty !!!!!!!", 
				 thinkos_rt.wq_ready);
		thinkos_rt.wq_ready = 0;
	}

#if THINKOS_ENABLE_TIMESHARE
	if (thinkos_rt.wq_tmshare != 0) {
		DCC_LOG1(LOG_ERROR, "wq_tmshare=%08x, timeshare queue not empty !!!!!", 
				 thinkos_rt.wq_tmshare);
		thinkos_rt.wq_tmshare = 0;
	}
#endif

	thinkos_exception_dsr(xcpt);

	__tdump();

	cm3_cpsie_i();
	
	for(;;);
}

void __attribute__((naked)) __xcpt_return(struct thinkos_except * xcpt)
{
	struct cm3_except_context * sf;
	uint32_t icsr;
	uint32_t ret;
	uint32_t xpsr;
	uint32_t shcsr;
	int ipsr;
	int irq;

	ipsr = cm3_ipsr_get();
	__idump(__func__, ipsr);

	if ((irq = __xcpt_next_active_irq(ipsr - 16)) >= 0) {
		xpsr = 0x01000000 + __xcpt_next_active_irq(ipsr - 16) + 16;
	} else if ((shcsr = CM3_SCB->shcsr) & (SCB_SHCSR_SYSTICKACT | 
										   SCB_SHCSR_PENDSVACT | 
										   SCB_SHCSR_MONITORACT | 
										   SCB_SHCSR_SVCALLACT |
										   SCB_SHCSR_USGFAULTACT |  
										   SCB_SHCSR_BUSFAULTACT |
										   SCB_SHCSR_MEMFAULTACT)) {
		if (shcsr & SCB_SHCSR_MEMFAULTACT) {
			xpsr = 0x01000000 + CM3_EXCEPT_MEM_MANAGE;
		} else if (shcsr & SCB_SHCSR_BUSFAULTACT) {
			xpsr = 0x01000000 + CM3_EXCEPT_BUS_FAULT;
		} else if (shcsr & SCB_SHCSR_USGFAULTACT) {
			xpsr = 0x01000000 + CM3_EXCEPT_USAGE_FAULT;
		} else if (shcsr & SCB_SHCSR_SVCALLACT) {
			xpsr = 0x01000000 + CM3_EXCEPT_SVC;
		} else if (shcsr & SCB_SHCSR_MONITORACT) {
			xpsr = 0x01000000 + CM3_EXCEPT_DEBUG_MONITOR;
		} else if (shcsr & SCB_SHCSR_PENDSVACT) {
			xpsr = 0x01000000 + CM3_EXCEPT_PENDSV;
		} else if (shcsr & SCB_SHCSR_SYSTICKACT) {
			xpsr = 0x01000000 + CM3_EXCEPT_SYSTICK;
		} else 
			xpsr = 0x01000000;
	} else {
		xpsr = 0x01000000;
	}


	sf = (struct cm3_except_context *)&thinkos_idle.ctx;
	sf->r0 = (uint32_t)xcpt;
	sf->xpsr = xpsr;

	icsr = CM3_SCB->icsr;
	if (icsr & SCB_ICSR_RETTOBASE) {
		sf->pc = (uint32_t)__xcpt_thread;
		cm3_psp_set((uint32_t)sf);
		ret = CM3_EXC_RET_THREAD_PSP;
	} else {
		sf->pc = (uint32_t)__xcpt_return;
		cm3_msp_set((uint32_t)sf);
		ret = CM3_EXC_RET_HANDLER;
	} 

	/* return */
	asm volatile ("bx   %0\n" : : "r" (ret)); 
}

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

	DCC_LOG(LOG_ERROR, "!!! Bus fault !!!");
	DCC_LOG2(LOG_ERROR, "BFSR=%08X BFAR=%08x", bfsr, scb->bfar);
	if (bfsr) {
		DCC_LOG7(LOG_ERROR, "BFSR={%s%s%s%s%s%s%s }", 
				 (bfsr & BFSR_BFARVALID) ? " BFARVALID" : "",
				 (bfsr & BFSR_LSPERR) ? " LSPERR" : "",
				 (bfsr & BFSR_STKERR) ? " STKERR" : "",
				 (bfsr & BFSR_UNSTKERR) ?  " UNSTKERR" : "",
				 (bfsr & BFSR_IMPRECISERR) ?  " IMPRECISERR" : "",
				 (bfsr & BFSR_PRECISERR) ?  " PRECISERR" : "",
				 (bfsr & BFSR_IBUSERR)  ?  " IBUSERR" : "");
	}
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
void __attribute__((naked)) cm3_bus_fault_isr(void)
{
	cm3_cpsid_i();
	__xcpt_context_save(&thinkos_except_buf);
	thinkos_except_buf.type = CM3_EXCEPT_BUS_FAULT;
	__bus_fault(&thinkos_except_buf);
	__xcpt_irq_disable_all();
	__xcpt_return(&thinkos_except_buf);
}
#endif

#if	THINKOS_ENABLE_USAGEFAULT 
void __attribute__((naked)) cm3_usage_fault_isr(void)
{
	cm3_cpsid_i();
	__xcpt_context_save(&thinkos_except_buf);
	thinkos_except_buf.type = CM3_EXCEPT_USAGE_FAULT;
	__usage_fault(&thinkos_except_buf);
	__xcpt_irq_disable_all();
	__xcpt_return(&thinkos_except_buf);
}
#endif

#if THINKOS_ENABLE_MPU
void __attribute__((naked)) cm3_mem_manage_isr(void)
{
	cm3_cpsid_i();
	__xcpt_context_save(&thinkos_except_buf);
	thinkos_except_buf.type = CM3_EXCEPT_MEM_MANAGE;
	__mem_manag(&thinkos_except_buf);
	__xcpt_irq_disable_all();
	__xcpt_return(&thinkos_except_buf);
}
#endif

void __attribute__((naked)) cm3_hard_fault_isr(void)
{
	__xcpt_context_save(&thinkos_except_buf);
	thinkos_except_buf.type = CM3_EXCEPT_HARD_FAULT;
	__hard_fault(&thinkos_except_buf);
	__xcpt_irq_disable_all();
	__xcpt_return(&thinkos_except_buf);
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
void __attribute__((naked)) cm3_nmi_isr(void)
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

void __attribute__((naked)) cm3_debug_mon_isr(void)
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
