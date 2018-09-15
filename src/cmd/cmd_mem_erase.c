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
#include <trace.h>

#include "target.h"
#include "debugger.h"
#include "eval.h"

#include <sys/dcclog.h>

int cmd_mem_erase(FILE * f, int argc, char ** argv)
{
	struct debugger * dbg = &debugger;
	value_t val;
	uint32_t addr;
	uint32_t size;
	int n;
	int ret;

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
		addr = (uint32_t)dbg->transf.base;

	if (argc) {
		if ((n = eval_uint32(&val, argc, argv)) < 0) {
			DCC_LOG(LOG_WARNING, "eval_uint32(), size");
			return n;
		}
		size = val.uint32;
		DCC_LOG2(LOG_INFO, "size=%d n=%d", size, n);
		argc -= n;
		argv += n;
	} else
		size = (dbg->transf.size + 3) & ~0x03;

	if (argc) {
		fprintf(f, "Too many arguments...\n");
		return -1;
	}

	dbg->transf.base = addr & ~0x03;
	dbg->transf.size = size;

	INF("target_mem_erase: 0x%08x, %d", addr, size);

	DCC_LOG2(LOG_TRACE, "addr=0x%08x size=%d", addr, size);

	ret = target_mem_erase(addr, size);

	INF("target_mem_erase: return %d", ret);

	return ret;
}

