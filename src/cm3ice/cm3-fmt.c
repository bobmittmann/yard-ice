/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the YARD-ICE.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */

/** 
 * @file cm3-fmt.c.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/delay.h>

#include "cm3ice.h"

const char * const cm3_regnames[20] = {
	"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", 
	"r8", "r9", "sl", "fp", "ip", "sp", "lr", "pc", 
	"xpsr", " msp", " psp", "ctrl"};

const char * const v7m_fpu_regnames[33] = {
	" s0", " s1", " s2", " s3", " s4", " s5", " s6", " s7", 
	" s8", " s9", "s10", "s11", "s12", "s13", "s14", "s15", 
	"s16", "s17", "s18", "s19", "s20", "s21", "s22", "s23", 
	"s24", "s25", "s26", "s27", "s28", "s29", "s30", "s31",
	"fpcsr"};

const char * cm3_reg_name(int r)
{
	return cm3_regnames[r];
}

const char * v7m_fpu_reg_name(int r)
{
	return v7m_fpu_regnames[r];
}

static const char xcpt_name_lut[16][12] = {
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


int cm3_show_xpsr(FILE * f, uint32_t xpsr)
{
	int ipsr = xpsr & 0x1ff;

	if (ipsr < 16) { 
		fprintf(f, "[ %c%c%c%c%c %c "
				 "ICI/IT=%02x GE=%1x IPSR=%d (%s) ]", 
				 (xpsr & (1 << 31)) ? 'N' : '.',
				 (xpsr & (1 << 30)) ? 'Z' : '.',
				 (xpsr & (1 << 29)) ? 'C' : '.',
				 (xpsr & (1 << 28)) ? 'V' : '.',
				 (xpsr & (1 << 27)) ? 'Q' : '.',
				 (xpsr & (1 << 24)) ? 'T' : '.',
				 ((xpsr >> 19) & 0xc0) | ((xpsr >> 10) & 0x3f),
				 ((xpsr >> 16) & 0x0f),
				 ipsr, xcpt_name_lut[ipsr]);
	} else {
		fprintf(f, "[ %c%c%c%c%c %c "
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
	return 0;
}

/* Bits [31:24] CONTROL
   Bits [23:16] FAULTMASK
   Bits [15:8] BASEPRI
   Bits [7:0] PRIMASK */

int cm3_show_ctrl(FILE * f, uint32_t ctrl)
{
	fprintf(f, "[ %s ", (ctrl & (1 << 25)) ? "PSP" : "MSP");
	fprintf(f, "%s ", (ctrl & (1 << 24)) ? "USER" : "PRIV");
	fprintf(f, "PM=%c ", ((ctrl >> 0) & 0x01) + '0');
	fprintf(f, "FM=%c ", ((ctrl >> 16) & 0x01) + '0');
	fprintf(f, "BPRI=%02x ] ", (ctrl >> 8) & 0xff);

	return 0;
}

int cm3_show_shcsr(FILE * f, uint32_t shcsr)
{
	fprintf(f, "[%s%s%s%s%s%s%s ]", 
			(shcsr & SHCSR_SYSTICKACT) ? " SYSTICKACT" : "",
			(shcsr & SHCSR_PENDSVACT) ? " PENDSVACT" : "",
			(shcsr & SHCSR_MONITORACT) ? " MONITORACT" : "",
			(shcsr & SHCSR_SVCALLACT) ? " SVCALLACT" : "",
			(shcsr & SHCSR_USGFAULTACT) ?  " USGFAULTACT" : "",
			(shcsr & SHCSR_BUSFAULTACT) ?  " BUSFAULTACT" : "",
			(shcsr & SHCSR_MEMFAULTACT) ?  " MEMFAULTACT" : "");
	return 0;
}

int cm3_show_icsr(FILE * f, uint32_t icsr)
{
	fprintf(f, "[%s%s%s%s%s%s VECTPENDING=%d VECTACTIVE=%d ]", 
			(icsr & ICSR_NMIPENDSET) ? " NMIPEND" : "",
			(icsr & ICSR_PENDSVSET) ? " PENDSV" : "",
			(icsr & ICSR_PENDSTSET) ? " PENDST" : "",
			(icsr & ICSR_ISRPREEMPT) ? " ISRPREEMPT" : "",
			(icsr & ICSR_ISRPENDING) ? " ISRPENDING" : "",
			(icsr & ICSR_RETTOBASE) ? " RETTOBASE" : "",
			(icsr & ICSR_VECTPENDING) >> 12,
			(icsr & ICSR_VECTACTIVE));
	return 0;
}

int cm3_show_regs(FILE * f, uint32_t * reg)
{
	int r; 

	for (r = 0; r < 4; r++) {
		fprintf(f, "%s= %08x  ", 
					  cm3_reg_name(r), reg[r]);
		fprintf(f, "%s= %08x  ", 
					  cm3_reg_name(r + 4), reg[r + 4]);
		fprintf(f, "%s= %08x  ", 
					  cm3_reg_name(r + 8), reg[r + 8]);
		fprintf(f, "%s= %08x  ", 
					  cm3_reg_name(r + 12), reg[r + 12]);
		fprintf(f, "%s= %08x\n", 
					  cm3_reg_name(r + 16), reg[r + 16]);
	}

	return 0;
}

int cm3_show_fpu_regs(FILE * f, uint32_t * reg)
{
	int r; 

	for (r = 0; r < 8; r++) {
		fprintf(f, "%s= %08x  ", 
					  v7m_fpu_reg_name(r), reg[r]);
		fprintf(f, "%s= %08x  ", 
					  v7m_fpu_reg_name(r + 8), reg[r + 8]);
		fprintf(f, "%s= %08x  ", 
					  v7m_fpu_reg_name(r + 16), reg[r + 16]);
		fprintf(f, "%s= %08x\n", 
					  v7m_fpu_reg_name(r + 24), reg[r + 24]);
	}

	return 0;
}

