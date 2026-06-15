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
 * @file cmd_connect..c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/shell.h>

#include "config.h"
#include "target.h"
#include "debugger.h"

int cmd_connect(FILE * f, int argc, char ** argv)
{
	int err;
	int force = 0;
	int init = 0;
	int i;

	if (argc > 3) {
//		fprintf(f, msg_connect_usage);
		return SHELL_ERR_EXTRA_ARGS;
	}

	for (i = 1; i < argc; i++) {
		if ((strcmp(argv[i], "force") == 0) || 
			(strcmp(argv[i], "f") == 0)) {
			force = 1;
		} else {
			if ((strcmp(argv[i], "init") == 0) || 
				(strcmp(argv[i], "i") == 0)) {
				init = 1;
			} else {
				return SHELL_ERR_ARG_INVALID;
			}
		}
	}

	fprintf(f, "* Connecting ...");
	if ((err = target_connect(force)) < 0) {
		fprintf(f, " fail!\n");
		fprintf(f, "#ERROR: target connect: %s\n", target_strerror(err));
		return SHELL_ERR_LOW_LEVEL;
	}
	fprintf(f, " Ok.\n");

	if (init) {
		fprintf(f, "* Initializing ...");
		if ((err = target_init(f)) < 0) {
			fprintf(f, " fail!\n");
			fprintf(f, "#ERROR: target init: %s\n", target_strerror(err));
			return SHELL_ERR_LOW_LEVEL;
		}
		fprintf(f, " Ok.\n");
	}

	return SHELL_OK;
}

