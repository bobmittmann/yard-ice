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
#include "command.h"
#include "eval.h"

#include <debug.h>

int cmd_goto(FILE * f, int argc, char ** argv)
{
//	uint32_t insn[4];
	uint32_t addr;
	value_t val;
	int err;
	int n;

	argc--;
	argv++;

	if ((n = eval_uint32(&val, argc, argv)) < 0) {
		fprintf(f, "Can't evaluate\n");
		return n;
	}

	addr = val.uint32;

	if (n != argc) {
		fprintf(f, "usage: goto [ADDR]\n");
		return -3;
	}

	if ((err = target_goto(addr, 0)) < 0) {
		fprintf(f, "#ERROR: target goto: %s\n", target_strerror(err));
		return err;
	}

//	target_insn_fetch(addr, insn);
//	target_insn_show(f, addr, insn);

//	show_context(f, 0);

	return 0;
}

