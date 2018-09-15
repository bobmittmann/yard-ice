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

#include <debug.h>

int show_asm(char * s, const struct ice_drv * drv, 
			 ice_context_t * ct, ice_insn_t * insn);

int show_context(FILE * f, int which)
{
//	struct debugger * dbg = &debugger;

	return 0;
/*
	char s[256];
	struct arm7context * ct;
	struct arm7insn * insn;

	ct = (struct arm7context *)dbg->ct;
	insn = (struct arm7insn *)&ct->insn;

	if (which >= DBG_TRACE_MAX)
		which = DBG_TRACE_MAX - 1;

	if (ct->cpsr & ARM_STATE_THUMB) {
		fprintf(f, "THUMB state\n");
	} else {
		fprintf(f, "ARM state\n");
	}

	arm_fmt_cpsr(s, ct);

	fprintf(f, "* cpsr = %08x %s\n", ct->cpsr, s);

	arm_fmt_registers(s, ct);

	fprintf(f, "%s\n", s);

	show_asm(s, dbg->ice, (ice_context_t *)dbg->ct, (ice_insn_t *)insn);

	fprintf(f, "%s\n", s);

	fprintf(f, "\n"); */
}

