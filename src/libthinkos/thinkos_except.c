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

void __attribute__((naked, noreturn)) cm3_hard_fault_isr(void)
{
	struct cm3_scb * scb = CM3_SCB;
	uint32_t hfsr;

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
		printf(" * ADDR = 0x%08x\n", scb->bfar);
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

