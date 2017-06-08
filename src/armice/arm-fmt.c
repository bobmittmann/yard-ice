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
 * @file arm-fmt.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/delay.h>

#include "arm_opc.h"
#include "armice.h"

#include "dis-asm.h"

#if DEBUG
static const char * const except_tab[8] = {
	"UNKNOWN EXCEPTION", "UNDEFINED INSTRUCTION", "UNHANDLED SWI", 
	"PREFETCH ABORT", "DATA ABORT", "INVALID EXCEPTION",
	"UNHANDLED IRQ", "UNHANDLED FIQ" };
#endif

static const char * const arm_mode[16] = { 
	"USR", "FIQ", "IRQ", "SVC",
	"0x4", "0x5", "0x6", "ABT",
	"0x8", "0x9", "0xA", "UND",
	"0xC", "0xD", "0xE", "SYS" };

int arm_show_cpsr(FILE * f, unsigned int cpsr)
{
	fprintf(f, "* cpsr = %08x ", cpsr);
	fprintf(f, "[N=%c ", ((cpsr >> 31) & 0x01) + '0');
	fprintf(f, "Z=%c ", ((cpsr >> 30) & 0x01) + '0');
	fprintf(f, "C=%c ", ((cpsr >> 29) & 0x01) + '0');
	fprintf(f, "V=%c] ", ((cpsr >> 28) & 0x01) + '0');
	fprintf(f, "[I=%c ", ((cpsr >> 7) & 0x01) + '0');
	fprintf(f, "F=%c] ", ((cpsr >> 6) & 0x01) + '0');
	fprintf(f, "[T=%c] ", ((cpsr >> 5) & 0x01) + '0');
	fprintf(f, "[M=%s]\n", arm_mode[(cpsr & 0x0f)]);
	
	return 0;
}

int arm_show_regs(FILE * f, uint32_t * reg)
{
	int r; 

	for (r = 0; r < 4; r++) {
		fprintf(f, "%s= %08x  ", 
					  arm_reg_name(r), reg[r]);
		fprintf(f, "%s= %08x  ", 
					  arm_reg_name(r + 4), reg[r + 4]);
		fprintf(f, "%s= %08x  ", 
					  arm_reg_name(r + 8), reg[r + 8]);
		fprintf(f, "%s= %08x\n", 
					  arm_reg_name(r + 12), reg[r + 12]);
	}

	return 0;
}

#if 0
int arm_show_asm(FILE * f, uint32_t addr, uint32_t opc)
{
	char s[80];

	fprintf(f, "%08x:\t%08x\t", addr, opc);
	arm_fmt_insn(s, addr, opc);
	return 0;
}


int thumb_show_asm(FILE * f, uint32_t addr, uint32_t opc)
{
	char s[80];

	if ((opc & 0xf800) == 0xf000) {
		fprintf(f, "%08x:\t%08x\t", addr & ~1, opc);
		thumb_fmt_insn(s, addr & ~0x01, opc);
	} else {
		fprintf(f, "%08x:\t%04x\t\t", addr & ~1, opc & 0xffff);
		thumb_fmt_insn(s, addr & ~0x01, opc & 0xffff);
	}

	return 0;
}

#endif
