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


#if CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <target.h>
#include <debugger.h>
#include <command.h>

#include <debug.h>

int cmd_probe(FILE * f, int argc, char ** argv)
{
	struct target_info * target;
	unsigned int i;
	int err = 0;

	if (argc != 1) {
//		fprintf(f, msg_init_usage);
		return -1;
	}

	target = target_first();
	for (i = 0; (target != NULL); i++) {

		fprintf(f, " %2d - '%s' ... ", i, target->name);

		if (target_configure(f, target, 0) < 0) {
			fprintf(f, "config error\n");
		} else {
			if (target_connect(1) < 0) {
				fprintf(f, "connect error\n");
			} else {
				if ((err = target_probe(f)) < 0) {
					fprintf(f, "probe error\n");
				} else {
					if(err == 0) {
						fprintf(f, "fail\n");
					} else {
						fprintf(f, "match\n");
						break;
					}
				}
			}
		}

		target = target_next(target);
	}

	if (target != NULL) {
		fprintf(f, " New target: '%s'\n", target->name);
		setenv("TARGET", target->name, 1);
	} else {
		/* fallback to NULL */
		target = target_first();
	}

	if ((err = target_configure(f, target, 0)) < 0) {
		fprintf(f, "ERROR: target_configure()!\n");
	}

	return err;
}

