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

#include <debug.h>

int cmd_watchpoint(FILE * f, int argc, char ** argv)
{
#if 0
	unsigned int addr;
	unsigned int mask;
	int i;
	struct debugger * dbg = &debugger;

	if (argc > 3) {
//		printf(msg_break_usage);
		return -1;
	}

	if (argc > 1) {
		addr = strtoul(argv[1], NULL, 0);

		if (argc > 2)
			mask = strtoul(argv[2], NULL, 0);
		else
			mask = 0x00000000;

		return target_set_watchpoint(addr, mask);
	}

	for (i = 0; i < DBG_WATCHPOINT_MAX; i++) {
		if (dbg->wp[i].mask != 0xffffffff) {
			fprintf(f, "Watchpoint %d at 0x%08x (mask=0x%08x)\n", 
						i, dbg->wp[i].addr, dbg->wp[i].mask);
		}
	}

	return 0;
#endif
	return 0;
}

