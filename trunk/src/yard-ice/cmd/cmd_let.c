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
#include "eval.h"

int cmd_let(FILE * f, int argc, char ** argv)
{
	char buf[64];
	var_def_t * var;
	value_t val;
	int err;
	int n;

	if (argc < 3) {
//		fprintf(f, msg_init_usage);
		return -1;
	}

	argc--;
	argv++;

	if ((var = var_global_lookup(*argv)) < 0) {
		fprintf(f, "Variable '%s' not found!\n", *argv);
		return -2;
	}

	argc--;
	argv++;

	switch (var->type) {
	case TYPE_UINT32: 
	case TYPE_INT32: 
	case TYPE_BOOL: 
		if ((n = eval_uint32(&val, argc, argv)) < 0) {
			fprintf(f, "Can't evaluate\n");
			return n;
		}
		break;
	default:
		fprintf(f, "Unsupported type: %d\n", var->type);
		return -3;
	}


	if (n != argc) {
		fprintf(f, "Syntax error\n");
		return -4;
	}

	if ((err = var_set(var, &val)) < 0) {
		fprintf(f, "Can't assign variable\n");
		return err;
	}

	if ((err = var_get(var, &val)) < 0) {
		fprintf(f, "Can't get variable value\n");
		return err;
	}

	fprintf(f, "%s\n", val_decode(def_of(var->type), &val, buf));

	return 0;
}

