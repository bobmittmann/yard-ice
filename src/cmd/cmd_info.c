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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debugger.h"

static int info_target(FILE * f, const ice_drv_t * ice, 
					   const target_info_t * target)
{
	if (target == NULL) {
		printf("Invalid target!\n");
		return -1;
	}

	if (ice == NULL) {
		printf("Invalid ICE driver!\n");
		return -1;
	}

	fprintf(f, "\nTarget: %s\n\n", target->name);
	fprintf(f, "  Arch: %-15s   model: %-15s vendor: %s\n", 
			target->arch->name, target->arch->model, target->arch->vendor);

	fprintf(f, "   CPU: %-15s   model: %-15s vendor: %s\n", 
			target->arch->cpu->family, target->arch->cpu->model, 
			target->arch->cpu->vendor);

	fprintf(f, "   ICE: %-15s version: %-15s vendor: %s\n", 
			ice->info->name, ice->info->version, ice->info->vendor);

	fprintf(f, "\n");

	return 0;
}

int cmd_info(FILE * f, int argc, char ** argv)
{
	struct debugger * dbg = &debugger;

	if (argc != 1) {
//		return show_cmd_usage(argv[0]);
		return -1;
	}

	return info_target(f, &dbg->ice, dbg->target);
}

