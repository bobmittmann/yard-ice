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
 * @file cmd_reset.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "target.h"
#include "debugger.h"
#include "lookup.h"

#include <sys/shell.h>

enum args {
	ARG_CLR = 1,
};

#define WITH_PARAM 0x100

static const struct dict_entry dbg_args[] = {
	{ "c",     ARG_CLR },
	{ "clear", ARG_CLR },
	{ "clr",   ARG_CLR }
};

int cmd_debugger(FILE * f, int argc, char ** argv)
{
	bool flag[2] = { false, false };

	if (argc < 2) {
		return SHELL_ERR_ARG_MISSING;
	}

	if (argc > 2) {
		return SHELL_ERR_EXTRA_ARGS;
	}

	if (argc > 1) {
		int val;
		val = dict_bsearch(dbg_args, DICT_LEN(dbg_args), argv[1]);
		if (val > 0) {
			flag[val] = true;
		} else {
			return SHELL_ERR_ARG_INVALID;
		}
	}

	if (flag[ARG_CLR]) {
		fprintf(f, "Debugger fault clear\n");
		target_fault_clr();
	}

	return SHELL_OK;
}

