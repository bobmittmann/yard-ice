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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/dcclog.h>

#include "debugger.h"

int cmd_meminfo(FILE * f, int argc, char ** argv)
{
	struct debugger * dbg = &debugger;
	ice_mem_entry_t * e;
	uint32_t size;
	uint32_t start;
	uint32_t end;

	if (argc != 1)
//		return show_cmd_usage(argv[0]);
		return -1;

	fprintf(f, "MEM:       FROM:    TO:      SIZE:     ACCESS:\n");

	for (e = (ice_mem_entry_t *)dbg->mem; e->op != NULL; ++e) {

		if ((size = e->blk.count * e->blk.size) == 0) {
			/* skip empty blocks */
			continue;
		}

		start = e->addr.base + e->addr.offs;
		end = e->addr.base + e->addr.offs + size - 1;

		DCC_LOG3(LOG_TRACE, "%s %08x %08x", e->name, start, end);

		fprintf(f, "%-10s %08x %08x", e->name, start, end);
		if (size >= 10000) {
			if (size > (10000 * 1024)) {
				fprintf(f, " %4d MiB", size / (1024 * 1024));
			} else {
				fprintf(f, " %4d KiB", size / 1024);
			}
		} else {
			fprintf(f, " %4d B  ", size);
		}

		fprintf(f, "  %2d bits", 8 << (e->flags & MEM_BUS_MASK));
		fprintf(f, " %s\n", (e->flags & MEM_RO) ? "RO" : "R/W");

	}

	return 0;
}


