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
#include "eval.h"

#include <sys/dcclog.h>

int cmd_disassemble(FILE * f, int argc, char ** argv)
{
	struct debugger * dbg = &debugger;
	value_t val;
	uint32_t addr;
	uint32_t size;
	int n;
	int i;

	argc--;
	argv++;

	if (argc) {
		if ((n = eval_uint32(&val, argc, argv)) < 0) {
			DCC_LOG(LOG_WARNING, "eval_uint32(), addr");
			return n;
		}
		argc -= n;
		argv += n;
		addr = val.uint32;
		DCC_LOG2(LOG_INFO, "addr=%08x n=%d", addr, n);
	} else
		addr = (uint32_t)dbg->dasm.base;

	DCC_LOG2(LOG_TRACE, "addr=%08x argc=%d", addr, argc);

	if (argc) {
		if ((n = eval_uint32(&val, argc, argv)) < 0) {
			DCC_LOG(LOG_WARNING, "eval_uint32(), size");
			return n;
		}
		size = val.uint32;
		DCC_LOG2(LOG_TRACE, "size=%d n=%d", size, n);
		argc -= n;
		argv += n;
	} else
		size = (dbg->dasm.size + 3) & ~0x03;

	if (argc) {
		fprintf(f, "Too many arguments...\n");
		return -1;
	}


	if (size == 0)
		size = 64;

	dbg->dasm.size = 0;

	for (i = 0; i < size; i++) {
		n = target_print_insn(f, addr);
		if (n <= 0) 
			break;
		addr += n;
	}

	DCC_LOG2(LOG_TRACE, "addr=%08x size=%d", addr, size);

	dbg->dasm.size = i;
	dbg->dasm.base = addr;

	return 0;
}


