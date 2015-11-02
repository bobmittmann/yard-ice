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
 * @file cmd_run.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "target.h"
#include "debugger.h"

#include <sys/dcclog.h>

int cmd_run(FILE * f, int argc, char ** argv)
{
	unsigned int addr;
	int err;
	int opt;

	if (argc > 3) {
		fprintf(f, "usage: run [ADDR [arm/thumb]]\n");
		return -1;
	}

	if (argc > 1) {
		addr = strtoul(argv[1], NULL, 0);
		if (argc > 2) {
			if (strcmp(argv[2], "arm") == 0) {
				opt = 2;
			} else {
				if (strcmp(argv[2], "thumb") == 0)
					opt = 1;
				else
					return -3;
			}
		} else {
			opt = 0;
		}
		target_goto(addr, opt);
	}

	if ((err = target_run()) < 0) {
		fprintf(f, "#ERROR: target run: %s\n", target_strerror(err));
		return err;
	}

	return 0;
}

