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

#if THINKOS_ENABLE_EXCEPTIONS

void __show_ctrl(uint32_t ctrl)
{
	printf("[%s ", (ctrl & (1 << 25)) ? "PSP" : "MSP");
	printf("%s ", (ctrl & (1 << 24)) ? "USER" : "PRIV");
	printf("PM=%c ", ((ctrl >> 0) & 0x01) + '0');
	printf("FM=%c ", ((ctrl >> 16) & 0x01) + '0');
	printf("BPRI=%02x] ", (ctrl >> 8) & 0xff);
}

static void __show_xpsr(uint32_t psr)
{
	printf("[N=%c ", ((psr >> 31) & 0x01) + '0');
	printf("Z=%c ", ((psr >> 30) & 0x01) + '0');
	printf("C=%c ", ((psr >> 29) & 0x01) + '0');
	printf("V=%c ", ((psr >> 28) & 0x01) + '0');
	printf("Q=%c ", ((psr >> 27) & 0x01) + '0');
	printf("ICI/IT=%02x ", ((psr >> 19) & 0xc0) | ((psr >> 10) & 0x3f));
	printf("XCP=%02x]", psr & 0xff);
}


void thinkos_context_show(const struct thinkos_context * ctx, 
						  uint32_t sp, uint32_t msp, uint32_t psp)
{
	__show_xpsr(ctx->xpsr);

	printf("\n");

	printf("   r0=%08x", ctx->r0);
	printf("   r4=%08x", ctx->r4);
	printf("   r8=%08x", ctx->r8);
	printf("  r12=%08x", ctx->r12);
	printf(" xpsr=%08x\n", ctx->r12);

	printf("   r1=%08x", ctx->r0);
	printf("   r5=%08x", ctx->r5);
	printf("   r9=%08x", ctx->r9);
	printf("   sp=%08x", sp);
	printf("  msp=%08x\n", msp);

	printf("   r2=%08x", ctx->r2);
	printf("   r6=%08x", ctx->r6);
	printf("  r10=%08x", ctx->r10);
	printf("   lr=%08x",  ctx->lr);
	printf("  psp=%08x\n", psp);

	printf("   r3=%08x",  ctx->r3);
	printf("   r7=%08x",  ctx->r7);
	printf("  r11=%08x",  ctx->r11);
	printf("   pc=%08x\n",  ctx->pc);
}

static inline struct thinkos_context * __attribute__((always_inline)) __get_context(void) {
	register struct thinkos_context * ctx;
	asm volatile ("push {r4-r11}\n"
				  "mov  %0, sp\n" : "=r" (ctx));
	return ctx;
}

static inline uint32_t __attribute__((always_inline)) __get_stack(void) {
	register uint32_t sp;
	asm volatile ("tst lr, #4\n" 
				  "ite eq\n" 
				  "mrseq r0, MSP\n" 
				  "mrsne r0, PSP\n" 
				  : "=r" (sp));
	return sp;
}

void __attribute__((naked, noreturn)) cm3_hard_fault_isr(void)
{
	struct thinkos_context * ctx;
	struct cm3_scb * scb = CM3_SCB;
	uint32_t hfsr;
	uint32_t sp;
	uint32_t msp;
	uint32_t psp;
	uint32_t lr;

	/* save the context */
	ctx = __get_context();
	lr = cm3_lr_get();
	msp = cm3_msp_get();
	psp = cm3_psp_get();

	if (lr & (1 << 4))
		sp = psp;
	else
		sp = msp;

	cm3_faultmask_set(1);

	printf("---\n");
	printf("Hard fault:");

	hfsr = scb->hfsr;

	if (hfsr & SCB_HFSR_DEBUGEVT)  
		printf(" DEBUGEVT");
	if (hfsr & SCB_HFSR_FORCED)  
		printf(" FORCED");
	if (hfsr & SCB_HFSR_VECTTBL)  
		printf(" VECTTBL");

	printf("\n");

	thinkos_context_show(ctx, sp, msp, psp);

	for(;;);
}

void __attribute__((naked, noreturn)) cm3_bus_fault_isr(void)
{
	struct cm3_scb * scb = CM3_SCB;
	uint32_t bfsr;

	cm3_faultmask_set(1);

	printf("---\n");
	printf("Bus fault:");

	bfsr = SCB_CFSR_BFSR_GET(scb->cfsr);

	if (bfsr & BFSR_BFARVALID)  
		printf(" BFARVALID");
	if (bfsr & BFSR_LSPERR)
		printf(" LSPERR");
	if (bfsr & BFSR_STKERR)  
		printf(" STKERR");
	if (bfsr & BFSR_UNSTKERR)  
		printf(" INVPC");
	if (bfsr & BFSR_IMPRECISERR)  
		printf(" IMPRECISERR");
	if (bfsr & BFSR_PRECISERR)
		printf(" PRECISERR");
	if (bfsr & BFSR_IBUSERR)  
		printf(" IBUSERR");
	printf("\n");

	if (bfsr & BFSR_BFARVALID)  {
		printf(" * ADDR = 0x%08x\n", (int)scb->bfar);
	}

	for(;;);
}

void __attribute__((naked, noreturn)) cm3_usage_fault_isr(void)
{
	struct cm3_scb * scb = CM3_SCB;
	uint32_t ufsr;

	cm3_faultmask_set(1);

	printf("---\n");
	printf("Usage fault:");

	ufsr = SCB_CFSR_UFSR_GET(scb->cfsr);
	if (ufsr & UFSR_DIVBYZERO)  
		printf(" DIVBYZERO");
	if (ufsr & UFSR_UNALIGNED)  
		printf(" UNALIGNED");
	if (ufsr & UFSR_NOCP)  
		printf(" NOCP");
	if (ufsr & UFSR_INVPC)  
		printf(" INVPC");
	if (ufsr & UFSR_INVSTATE)  
		printf(" INVSTATE");
	if (ufsr & UFSR_UNDEFINSTR)  
		printf(" UNDEFINSTR");

	printf("\n");

	for(;;);
}

const char thinkos_except_nm[] = "EXCEPT";

#endif /* THINKOS_ENABLE_EXCEPT */

