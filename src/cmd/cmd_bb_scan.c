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

#include "target.h"
#include "debugger.h"
#include "hexdump.h"
#include "eval.h"

#include <sys/dcclog.h>

int cmd_bb_scan(FILE * f, int argc, char ** argv)
{
//	struct debugger * dbg = &debugger;
	nand_chip_t * chip;
	int ret;
	int i;
	int bb_cnt = 0;

	argc--;
	argv++;

	if (argc) {
		fprintf(f, "Too many arguments...\n");
		return -1;
	}

	if ((ret = target_nand_chip_get(0, 0, &chip)) < 0) {
		fprintf(f, "# target_nand_chip_get() fail!\n");
		return -1;
	}

	fprintf(f, "- blocks: %d\n", chip->num_blocks);

	for (i = 0; i < chip->num_blocks; i++) {
		if (target_nand_bb_check(i) < 0) {
			if (bb_cnt)
				fprintf(f, ", ");
			fprintf(f, "%d", i);
			bb_cnt++;
		} else {
			fprintf(f, ".");
		}
	}

	if (bb_cnt) {
		fprintf(f, "\n- %d bad blocks found\n", bb_cnt);
	} else {
		fprintf(f, "\n- No bad blocks found\n");
	}

	return 0;
}

