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


#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "target.h"
#include "debugger.h"
#include "hexdump.h"
#include "eval.h"

#include <sys/dcclog.h>

int cmd_mem_read(FILE * f, int argc, char ** argv)
{
	struct debugger * dbg = &debugger;
	value_t val;
	uint32_t addr;
	uint32_t size;
	int n;

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
		addr = (uint32_t)dbg->dump.base;

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
		size = (dbg->dump.size + 3) & ~0x03;

	if (argc) {
		fprintf(f, "Too many arguments...\n");
		return -1;
	}


	if (size == 0)
		size = 64;

	dbg->dump.base = addr & ~0x03;
	dbg->dump.size = 0;

	size = mem_hexdump(f, dbg->dump.base, size);

	dbg->dump.base += size;
	dbg->dump.size = size;

	return 0;
}

