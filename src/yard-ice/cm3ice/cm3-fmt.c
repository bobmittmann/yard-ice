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
 * @file .c
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
	"xpsr", " msp", " psp", "ctrl" };

const char * cm3_reg_name(int r)
{
	return cm3_regnames[r];
}

int cm3_show_xpsr(FILE * f, uint32_t psr)
{
	fprintf(f, "[N=%c ", ((psr >> 31) & 0x01) + '0');
	fprintf(f, "Z=%c ", ((psr >> 30) & 0x01) + '0');
	fprintf(f, "C=%c ", ((psr >> 29) & 0x01) + '0');
	fprintf(f, "V=%c ", ((psr >> 28) & 0x01) + '0');
	fprintf(f, "Q=%c ", ((psr >> 27) & 0x01) + '0');
	fprintf(f, "ICI/IT=%02x ", ((psr >> 19) & 0xc0) | ((psr >> 10) & 0x3f));
	fprintf(f, "XCP=%02x]", psr & 0xff);
	
	return 0;
}

/* Bits [31:24] CONTROL
   Bits [23:16] FAULTMASK
   Bits [15:8] BASEPRI
   Bits [7:0] PRIMASK */

int cm3_show_ctrl(FILE * f, uint32_t ctrl)
{
	fprintf(f, "[%s ", (ctrl & (1 << 25)) ? "PSP" : "MSP");
	fprintf(f, "%s ", (ctrl & (1 << 24)) ? "USER" : "PRIV");
	fprintf(f, "PM=%c ", ((ctrl >> 0) & 0x01) + '0');
	fprintf(f, "FM=%c ", ((ctrl >> 16) & 0x01) + '0');
	fprintf(f, "BPRI=%02x] ", (ctrl >> 8) & 0xff);

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

