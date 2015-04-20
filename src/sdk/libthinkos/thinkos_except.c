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
	fprintf(stderr, "ICI/IT=%02x ", ((psr >> 19) & 0xc0) | ((psr >> 10) & 0x3f));
	fprintf(stderr, "XCP=%02x]", psr & 0xff);
}


void thinkos_context_show(const struct thinkos_context * ctx, 
						  uint32_t sp, uint32_t msp, uint32_t psp)
{
	__show_xpsr(ctx->xpsr);

	fprintf(stderr, "\n");

	fprintf(stderr, "   r0=%08x", ctx->r0);
	fprintf(stderr, "   r4=%08x", ctx->r4);
	fprintf(stderr, "   r8=%08x", ctx->r8);
	fprintf(stderr, "  r12=%08x", ctx->r12);
	fprintf(stderr, " xpsr=%08x\n", ctx->xpsr);

	fprintf(stderr, "   r1=%08x", ctx->r1);
	fprintf(stderr, "   r5=%08x", ctx->r5);
	fprintf(stderr, "   r9=%08x", ctx->r9);
	fprintf(stderr, "   sp=%08x", sp);
	fprintf(stderr, "  msp=%08x\n", msp);

	fprintf(stderr, "   r2=%08x", ctx->r2);
	fprintf(stderr, "   r6=%08x", ctx->r6);
	fprintf(stderr, "  r10=%08x", ctx->r10);
	fprintf(stderr, "   lr=%08x",  ctx->lr);
	fprintf(stderr, "  psp=%08x\n", psp);

	fprintf(stderr, "   r3=%08x",  ctx->r3);
	fprintf(stderr, "   r7=%08x",  ctx->r7);
	fprintf(stderr, "  r11=%08x",  ctx->r11);
	fprintf(stderr, "   pc=%08x\n",  ctx->pc);
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
		fprintf(stderr, " INVPC");
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

#if 0
static inline uint32_t __attribute__((always_inline)) __get_stack(void) {
	register uint32_t sp;
	asm volatile ("tst lr, #4\n" 
				  "ite eq\n" 
				  "mrseq r0, MSP\n" 
				  "mrsne r0, PSP\n" 
				  : "=r" (sp));
	return sp;
}
#endif

static inline struct thinkos_context * 
	__attribute__((always_inline)) __get_context(void) {
	register struct thinkos_context * ctx;
	asm volatile ("tst   lr, #0x4\n" /* Test EXC_RETURN bit 2 */
				  "it    ne\n"
				  "subne sp, #32\n"  /* Make room in the stack... */ 
				  "push  {r4-r11}\n"
				  "tst   lr, #0x4\n"
				  "beq   0f\n"
				  "mrs   r0, PSP\n" 
				  "add   r1, sp, #32\n" 
				  "ldmia r0, {r2-r9}\n"
				  "stmia r1, {r2-r9}\n"
				  "0:\n"
				  "mov  %0, sp\n" : "=r" (ctx));
	return ctx;
}

void __attribute__((noreturn)) 
	thinkos_exception_dsr(struct thinkos_context * ctx);

void hard_fault(struct thinkos_context * ctx, uint32_t msp, 
				uint32_t psp, uint32_t lr)
{
	struct cm3_scb * scb = CM3_SCB;
	uint32_t hfsr;
	uint32_t sp;
	(void)sp;

	if (lr & (1 << 4))
		sp = psp;
	else
		sp = msp;

	hfsr = scb->hfsr;

	DCC_LOG3(LOG_ERROR, "Hard fault:%s%s%s", 
			 (hfsr & SCB_HFSR_DEBUGEVT) ? " DEBUGEVT" : "",
			 (hfsr & SCB_HFSR_FORCED) ?  " FORCED" : "",
			 (hfsr & SCB_HFSR_VECTTBL) ? " VECTTBL" : "");

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
					 (bfsr & BFSR_UNSTKERR) ?  " INVPC" : "",
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

	thinkos_context_show(ctx, sp, msp, psp);
	fprintf(stderr, "\n");
	fflush(stderr);
#endif
}

void __bus_fault(struct thinkos_context * ctx, uint32_t msp, 
			   uint32_t psp, uint32_t lr)
{
	uint32_t sp;
	(void)sp;

	if (lr & (1 << 4))
		sp = psp;
	else
		sp = msp;

	DCC_LOG(LOG_ERROR, "Bus fault!");

	DCC_LOG4(LOG_ERROR, "  R0=%08x  R1=%08x  R2=%08x  R3=%08x", 
			ctx->r0, ctx->r1, ctx->r2, ctx->r3);
	DCC_LOG4(LOG_ERROR, "  R4=%08x  R5=%08x  R6=%08x  R7=%08x", 
			ctx->r4, ctx->r7, ctx->r6, ctx->r7);
	DCC_LOG4(LOG_ERROR, "  R8=%08x  R9=%08x R10=%08x R11=%08x", 
			ctx->r8, ctx->r9, ctx->r10, ctx->r11);
	DCC_LOG4(LOG_ERROR, " R12=%08x  SP=%08x  LR=%08x  PC=%08x", 
			ctx->r12, sp, ctx->lr, ctx->pc);
	DCC_LOG4(LOG_ERROR, "XPSR=%08x MSP=%08x PSP=%08x RET=%08x", 
			ctx->xpsr, msp, psp, lr);

#if THINKOS_STDERR_FAULT_DUMP
	fprintf(stderr, "\n---\n");
	fprintf(stderr, "Bus fault:");

	__dump_bfsr();

	thinkos_context_show(ctx, sp, msp, psp);
	fprintf(stderr, "\n");
	fflush(stderr);
#endif
}

void usage_fault(struct thinkos_context * ctx, uint32_t msp, 
				 uint32_t psp, uint32_t lr)
{
	uint32_t sp;
	(void)sp;

	if (lr & (1 << 4))
		sp = psp;
	else
		sp = msp;

	DCC_LOG(LOG_ERROR, "Usage fault!");

	DCC_LOG4(LOG_ERROR, "  R0=%08x  R1=%08x  R2=%08x  R3=%08x", 
			ctx->r0, ctx->r1, ctx->r2, ctx->r3);
	DCC_LOG4(LOG_ERROR, "  R4=%08x  R5=%08x  R6=%08x  R7=%08x", 
			ctx->r4, ctx->r7, ctx->r6, ctx->r7);
	DCC_LOG4(LOG_ERROR, "  R8=%08x  R9=%08x R10=%08x R11=%08x", 
			ctx->r8, ctx->r9, ctx->r10, ctx->r11);
	DCC_LOG4(LOG_ERROR, " R12=%08x  SP=%08x  LR=%08x  PC=%08x", 
			ctx->r12, sp, ctx->lr, ctx->pc);
	DCC_LOG4(LOG_ERROR, "XPSR=%08x MSP=%08x PSP=%08x RET=%08x", 
			ctx->xpsr, msp, psp, lr);

#if THINKOS_STDERR_FAULT_DUMP
	fprintf(stderr, "\n---\n");
	fprintf(stderr, "Usage fault:");

	__dump_ufsr();

	thinkos_context_show(ctx, sp, msp, psp);
	fprintf(stderr, "\n");
	fflush(stderr);
#endif
}

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
	thinkos_context_show(ctx, sp, msp, psp);
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
	thinkos_context_show(ctx, sp, msp, psp);
	fprintf(stderr, "\n");
	fflush(stderr);
#endif
	for(;;);
}

#endif

void thinkos_mem(struct thinkos_context * ctx, uint32_t msp, 
				 uint32_t psp, uint32_t lr)
{
	uint32_t sp;
	(void)sp;

	if (lr & (1 << 4))
		sp = psp;
	else
		sp = msp;

	DCC_LOG(LOG_ERROR, "Mem Manager");
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
	fprintf(stderr, "Mem Manager:");
	thinkos_context_show(ctx, sp, msp, psp);
	fprintf(stderr, "\n");
	fflush(stderr);
#endif
	for(;;);
}

static inline struct thinkos_context * 
	__attribute__((always_inline)) __save_context(struct thinkos_context * ctx) {
	return ctx;
}

void __attribute__((naked, noreturn)) cm3_bus_fault_isr(void)
{
	struct thinkos_context * ctx = &thinkos_dmon_rt.except.ctx;
	uint32_t sp;
	uint32_t tmp;
	uint32_t lr;
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

	thinkos_dmon_rt.except.ret = lr;
	thinkos_dmon_rt.except.sp = sp;

	DCC_LOG(LOG_ERROR, "DMON_BUSFAULT");
	dmon_signal(DMON_BUSFAULT);

	lr = thinkos_dmon_rt.except.ret;
	sp = thinkos_dmon_rt.except.sp;

	asm volatile ("add    %2, %2, #32\n"
				  "ldmia  %2, {r4-r11}\n"
				  "stmia  %0, {r4-r11}\n"
				  "sub    %2, %2, #32\n"
				  "ldmia  %2, {r4-r11}\n"
				  "mov    lr, %1\n"
				  "bx     lr\n"
				  : : "r" (sp), "r" (lr), "r" (ctx) );
}

void __attribute__((naked, noreturn)) cm3_usage_fault_isr(void)
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

	usage_fault(ctx, msp, psp, lr);

	thinkos_exception_dsr(ctx);
}


void __attribute__((naked, noreturn)) cm3_hard_fault_isr(void)
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

	hard_fault(ctx, msp, psp, lr);

	thinkos_exception_dsr(ctx);
}

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

void __attribute__((naked, noreturn)) cm3_mem_manage_isr(void)
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

	thinkos_mem(ctx, msp, psp, lr);

	thinkos_exception_dsr(ctx);
}


void __attribute__((noreturn)) 
	thinkos_default_exception_dsr(struct thinkos_context * ctx)
{
#if THINKOS_SYSRST_ONFAULT
	cm3_sysrst();
#else
	for(;;);
#endif
}

void thinkos_exception_dsr(struct thinkos_context *) 
	__attribute__((weak, alias("thinkos_default_exception_dsr")));

void thinkos_except_init(void)
{
	struct cm3_scb * scb = CM3_SCB;
	scb->shcsr = SCB_SHCSR_USGFAULTENA | SCB_SHCSR_BUSFAULTENA | 
		SCB_SHCSR_MEMFAULTENA;
}

#endif /* THINKOS_ENABLE_EXCEPTIONS */

