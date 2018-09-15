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
 * @file cmd_step.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "target.h"
#include "debugger.h"

int cmd_step(FILE * f, int argc, char ** argv)
{
#if 0
	struct debugger * dbg = &debugger;
	struct arm7context * ct_old;
	struct arm7context * ct;
	struct arm7insn * insn;
	char s[128];
#endif
	uint32_t addr;
//	uint32_t insn[4];
	int quiet = 0;
	int err;
	int n = 1;
	int i;

	if (argc > 3)
		return -1;

	if (argc > 1) {
		if (*argv[1] == 'q')
			quiet = 1;
		else
			n = strtoul(argv[1], NULL, 0);
	}

	if (argc > 2) {
		if (*argv[2] == 'q')
			quiet = 1;
		else
			return -1;
	}

	for (i = 0; i < n; i++) {

		if ((err = target_step()) < 0) {
			fprintf(f, "#ERROR: target step: %s\n", target_strerror(err));
			return err;
		}

		if (!quiet) {
			/* Get the Instruction Fetch Address */
			target_ifa_get(&addr);
			target_print_insn(f, addr);
		}
	}

#if 0
	ct = (struct arm7context *)dbg->ct;

	for (i = 0; i < n; i++) {

		ct_old = ct;

		if ((err = target_step()) < 0) {
			fprintf(f, "%s", "ERROR: step!\n");
			return err;
		}

		ct = (struct arm7context *)dbg->ct;
		insn = (struct arm7insn *)&ct->insn;

		if (quiet) {

//			if (ct_old->cpsr != ct->cpsr) {
//				char tmp[64];
//				arm_fmt_cpsr(tmp, ct);
//				fprintf(f, "* cpsr = %08x %s\n", ct->cpsr, tmp);
//			}

			if ((i & 0x1f) == 0) {
				unsigned int addr;
				insn = (struct arm7insn *)&ct->insn;
				addr = insn->pc & ~1;
//				if (i == 0) {
					fprintf(f, "%08x (%d)\n", addr, i);
////				} else {
////					fprintf(f, "\r%08x", addr);
////				}
			}
		} else {
	//		unsigned int old;
	//		unsigned int val;
			int r;

			for (r = 0; r < 14; r++) {
//				context_register_get((struct ice_context *)ct_old, r, &old);
//				context_register_get((struct ice_context *)ct, r, &val);

//				if (val != old) {
//					fprintf(f, "* %s = 0x%08x (%d) \t{0x%08x (%d)}\n", 
//						   arm_reg_name(r), val, val, old, old);
//				}
			}

			if (ct_old->cpsr != ct->cpsr) {
//				arm_fmt_cpsr(s, ct);

				fprintf(f, "* cpsr = %08x %s\n", ct->cpsr, s);

//				if (ct->insn.thumb && !ct_old->insn.thumb)
//					fprintf(f, "\t.thumb\n");
//				else {
//					if (!ct->insn.thumb && ct_old->insn.thumb)
//						fprintf(f, "\t.arm\n");
//					}
			}

//			show_asm(s, dbg->ice, (ice_context_t *)dbg->ct, 
//					 (ice_insn_t *)&ct->insn);

			fprintf(f, "%s\n", s);
		}
	}
	if (quiet) {
//		show_asm(s, dbg->ice, (ice_context_t *)dbg->ct, 
//				 (ice_insn_t *)&ct->insn);

		fprintf(f, "%s\n", s);
	}
#endif

	return 0;
}

