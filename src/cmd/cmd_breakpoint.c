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
#include <sys/shell.h>

#include <sys/dcclog.h>
#include "target.h"
#include "debugger.h"
#include "eval.h"
#include "lookup.h"

int bp_usage(FILE * f, char * msg, char * cmd)
{
	if (msg)
		fprintf(f, "%s\n", msg); 
	fprintf(f, "Usage: %s [set | clr | en | dis] [all | <ADDR [SIZE]>]\n", 
			cmd);
	return -1;
}

void bp_print_header(FILE * f)
{
	fprintf(f, "  # | BP | E | A | ID |       Addr |     Size |\n");
}

const char type_tag[][4] = {
	[ICE_BKPT_EXEC_HW]   = "HW",
	[ICE_BKPT_READ_HW]   = "RD",
	[ICE_BKPT_WRITE_HW]  = "WR",
	[ICE_BKPT_ACCESS_HW] = "WP",
	[ICE_BKPT_EXEC_SW]   = "SW"
};

void bp_print_entry(FILE * f, struct bkpt_ctrl * bp, int n)
{
	fprintf(f, " %2d | %s | %c | %c | %2d | 0x%08x | %8d |\n", 
			n, type_tag[bp->hdr.type],
			bp->hdr.enabled ? '*' : ' ', 
			bp->hdr.active ? '*' : ' ', 
			bp->hdr.id, bp->hdr.addr, bp->hdr.size);
}

int bp_print_table(FILE * f, int type, int n) 
{
	struct bkpt_ctrl * bp = NULL;
	while ((bp = target_bkpt_next(bp, type)) != NULL) {
		bp_print_entry(f, bp, n++);
	}

	return n;
}

enum args {
	ARG_SET = 0,
	ARG_CLR = 1,
	ARG_DIS = 2,
	ARG_EN = 3,
	ARG_ALL = 4,
	ARG_HW = 5,
	ARG_SW = 6,
	ARG_WP = 7,
	ARGS_CNT = 8
};

#define TYPE 0x200
#define ACTION 0x100
#define ARG_MASK 0xff

static const struct dict_entry bkpt_args[] = {
	{ "a",       ARG_ALL },
	{ "all",     ARG_ALL },
	{ "c",       ARG_CLR + ACTION },
	{ "clear",   ARG_CLR + ACTION },
	{ "clr",     ARG_CLR + ACTION },
	{ "d",       ARG_DIS + ACTION },
	{ "dis",     ARG_DIS + ACTION },
	{ "disable", ARG_DIS + ACTION },
	{ "e",       ARG_EN + ACTION },
	{ "en",      ARG_EN + ACTION },
	{ "enable",  ARG_EN + ACTION },
	{ "h",       ARG_HW + TYPE },
	{ "hw",      ARG_HW + TYPE },
	{ "s",       ARG_SET + ACTION },
	{ "set",     ARG_SET + ACTION },
	{ "soft",    ARG_SW + TYPE },
	{ "sw",      ARG_SW + TYPE },
	{ "w",       ARG_WP + TYPE },
	{ "wp",      ARG_WP + TYPE }
};

#define VALUES_MAX 2

int cmd_breakpoint(FILE * f, int argc, char ** argv)
{
	struct bkpt_ctrl * bp = NULL;
	char * cmd = argv[0];
	value_t val;
	bool apply_to_all = false;
	int (* action)(uint32_t, uint32_t, unsigned int) = NULL;
	int nval = 0;
	uint32_t addr = 0;
	uint32_t size = 0;
	int type = -1;
	int n;

	argc--;
	argv++;

	while (argc > 0) {
		int v;
		/* lookup for string arguments */
		v = dict_bsearch(bkpt_args, DICT_LEN(bkpt_args), argv[0]);
		if (v >= 0) {
			/* remove the action flag */
			if (v & ACTION) {
				if (action != NULL) {
					fprintf(f, "Multiple actions not supported.\n");
					return SHELL_ERR_ARG_INVALID;
				}
				switch (v & ARG_MASK) { 
				case ARG_SET:
					action = target_bkpt_set;
					break;
				case ARG_CLR:
					action = target_bkpt_clear;
					break;
				case ARG_EN:
					action = target_bkpt_enable;
					break;
				case ARG_DIS:
					action = target_bkpt_disable;
					break;
				}
				argc--;
				argv++;
			} else if (v & TYPE) {
				if (type >= 0) {
					fprintf(f, "Multiple types not allowed.\n");
					return SHELL_ERR_ARG_INVALID;
				}
				switch (v & ARG_MASK) {
				case ARG_HW:
					type = ICE_BKPT_EXEC_HW;
					break;
				case ARG_SW:
					type = ICE_BKPT_EXEC_SW;
					break;
				case ARG_WP:
					type = ICE_BKPT_ACCESS_HW;
					break;
				}
				argc--;
				argv++;
			} else if (v == ARG_ALL) {
				apply_to_all = true;
				argc--;
				argv++;
			} else {
				DCC_LOG(LOG_WARNING, "should not reach here!!!");
			}
			/* try to evaluate a numeric expression  */
		} else if ((n = eval_uint32(&val, argc, argv)) > 0) {
			argc -= n;
			argv += n;
			if (nval == VALUES_MAX) {
				return SHELL_ERR_EXTRA_ARGS;
			}
			if (nval == 0)
				addr = val.uint32;
			else
				size = val.uint32;
			nval++;
		} else {
			/* declare an argument invalid */
			fprintf(f, "Invalid argument %s", *argv);
			return SHELL_ERR_ARG_INVALID;
		}
	}

	if ((action == NULL) && (nval > 0)) /* default action */
		action = target_bkpt_set;

	if (nval == 0)
		apply_to_all = true;

	if (action == NULL) { 	
		/* no action specified then show the table(s) */
		bp_print_header(f);
		if (type < 0) {
			n = bp_print_table(f, ICE_BKPT_EXEC_HW, 1); 
			n = bp_print_table(f, ICE_BKPT_EXEC_SW, n); 
			n = bp_print_table(f, ICE_BKPT_ACCESS_HW, n); 
		} else {
			n = bp_print_table(f, type, 1); 
		}
		return SHELL_OK;
	}

	if (type < 0) /* default type */
		type = ICE_BKPT_EXEC_HW;

	if (apply_to_all) {

		if (action == target_bkpt_set) {
			/* cannot set all */
			bp_usage(f, "cannot set all", cmd);
			return SHELL_ERR_ARG_INVALID;
		}
		if (action == target_bkpt_clear) {
			/* the 'bkpt_clear' action deletes the head of the list. We cannot use
			 * bkpt_next() in this case. */
			while ((bp = target_bkpt_head(type)) != NULL)
				action(bp->hdr.addr, bp->hdr.size, bp->hdr.type);
		} else {
			while ((bp = target_bkpt_next(bp, type)) != NULL)
				action(bp->hdr.addr, bp->hdr.size, bp->hdr.type);
		}
		return SHELL_OK;
	}

	if (action(addr, size, type) < 0) {
		return SHELL_ERR_LOW_LEVEL;
	}

	return SHELL_OK;
}

