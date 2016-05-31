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

#include <sys/dcclog.h>
#include "target.h"
#include "debugger.h"
#include "eval.h"

int bp_usage(FILE * f, char * msg, char * cmd)
{
	if (msg)
		fprintf(f, "%s\n", msg); 
	fprintf(f, "Usage: %s [set | clr | en | dis] [all | <ADDR [SIZE]>]\n", 
			cmd);
	return -1;
}

int cmd_breakpoint(FILE * f, int argc, char ** argv)
{
	struct dbg_bp * bp = NULL;
	value_t val;
	char * cmd = argv[0];
	int (* action)(uint32_t, uint32_t);
	uint32_t addr;
	uint32_t size;
	int n;

	argc--;
	argv++;

	if (argc == 0) {
		n = 1;
		fprintf(f, "  # | E | A | HW |       Addr |     Size |\n");
		while (target_breakpoint_next(bp, &bp) == 0) {
			fprintf(f, " %2d | %c | %c | %2d | 0x%08x | %8d |\n", 
					n++, 
					bp->enabled ? '*' : ' ', 
					bp->active ? '*' : ' ', 
					bp->hw_id, bp->addr, bp->size);
		}
		return 0;
	}

	do {
		if (strcmp(*argv, "set") == 0) {
			action = target_breakpoint_set;
		} else if (strcmp(*argv, "clr") == 0) {
			action = target_breakpoint_clear;
		} else if (strcmp(*argv, "en") == 0) {
			action = target_breakpoint_enable;
		} else if (strcmp(*argv, "dis") == 0) {
			action = target_breakpoint_disable;
		} else {
			action = target_breakpoint_set;
			break;
		}
		argc--;
		argv++;
	} while (0);

	if (!argc ||  (strcmp(*argv, "all") == 0)) {
		if (action == target_breakpoint_set) {
			return bp_usage(f, "invalid argument", cmd);
		}
		argc--;
		argv++;

		if (argc > 0)
			return bp_usage(f, "too many arguments", cmd);

		while (target_breakpoint_next(bp, &bp) == 0)
			action(bp->addr, bp->size);

		return 0;
	}

	if (argc) {
		if ((n = eval_uint32(&val, argc, argv)) < 0) {
			DCC_LOG(LOG_WARNING, "eval_uint32(), addr");
			bp_usage(f, "invalid argument", cmd);
			return n;
		}
		argc -= n;
		argv += n;
		addr = val.uint32;
		DCC_LOG2(LOG_INFO, "addr=%08x n=%d", addr, n);
	} else {
		bp_usage(f, "missing address", cmd);
		return -1;
	}

	if (argc) {
		if ((n = eval_uint32(&val, argc, argv)) < 0) {
			DCC_LOG(LOG_WARNING, "eval_uint32(), size");
			bp_usage(f, "invalid argument", cmd);
			return n;
		}
		size = val.uint32;
		DCC_LOG2(LOG_TRACE, "size=%d n=%d", size, n);
		argc -= n;
		argv += n;
	} else
		size = 0;

	if (argc) {
		bp_usage(f, "too many arguments", cmd);
		return -1;
	}

	return action(addr, size);
}

