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

int cmd_bp_clear(FILE * f, int argc, char ** argv)
{
	value_t val;
	uint32_t addr;
	uint32_t size = 0;
	int n;

	argc--;
	argv++;

	if (!argc) {
		struct dbg_bp * bp;
		/* clear all */
		while (target_breakpoint_get(NULL, &bp) == 0)
			target_breakpoint_delete(bp);
		return 0;
	}

	if ((n = eval_uint32(&val, argc, argv)) < 0) {
		DCC_LOG(LOG_WARNING, "target_eval(), addr");
		return n;
	}
	addr = val.uint32;
	DCC_LOG2(LOG_TRACE, "addr=%d n=%d", addr, n);
	argc -= n;
	argv += n;

	if (argc) {
		if ((n = eval_uint32(&val, argc, argv)) < 0) {
			DCC_LOG(LOG_WARNING, "target_eval(), size");
			return n;
		}
		size = val.uint32;
		DCC_LOG2(LOG_TRACE, "size=%d n=%d", size, n);
		argc -= n;
		argv += n;
	}

	if (argc) {
		fprintf(f, "Too many arguments...\n");
		return -1;
	}

	target_breakpoint_clear(addr, size);

	target_watchpoint_clear(addr, size);

	return 0;
}

