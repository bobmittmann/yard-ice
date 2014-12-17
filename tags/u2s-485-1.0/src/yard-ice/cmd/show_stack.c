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

#include "config.h"
#include "target.h"
#include "debugger.h"
#include "hexdump.h"

#include <debug.h>

int show_stack(FILE * f)
{
	struct debugger * dbg = &debugger;
	unsigned int addr;
	unsigned int * sp = 0;
	unsigned int base;
	int cnt;
	int n;

	if (target_stack_refresh() < 0)
		return -1;

	addr = dbg->stack.base;
	cnt = dbg->stack.size;
//	sp = (unsigned int *)dbg->stack_buf;

	addr &= ~(sizeof(uint32_t) - 1);
	base = addr & ~(16 - 1);
	if (base < addr) {
		n = (base + 16) - addr;
	} else {
		n = 16;
	}

	do {
		if (cnt < n)
			n = cnt;

		show_hex32(f, addr, sp, n);

		fprintf(f, "\n");

		addr += n;
		sp += 16;
		cnt -= n;
		n = 16;
	} while (cnt > 0);

	return 0;
}

