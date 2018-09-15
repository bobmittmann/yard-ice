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

int cmd_power(FILE * f, int argc, char ** argv)
{
	int err;
	int ms;

	if ((argc < 1) || (argc > 3)) {
//		printf(msg_reset_usage);
		return -1;
	}

	if (argc == 1) {
		return target_power_stat(f);
	}

	if ((strcmp(argv[1], "on") == 0) || (strcmp(argv[1], "1") == 0)) {
		fprintf(f, "Target power on...\n");
		err = target_power_ctl(1);
	} else {
		if ((strcmp(argv[1], "off") == 0) || (strcmp(argv[1], "0") == 0)) {
			fprintf(f, "Target power off...\n");
			err = target_power_ctl(0);
		} else {
			if ((strcmp(argv[1], "cycle") == 0) || 
				(strcmp(argv[1], "cyc") == 0)) {
				if (argc == 3) {
					ms = strtoul(argv[2], NULL, 0);
				} else {
					ms = 250;
				}
				fprintf(f, "Target power cycle...\n");
				if ((err = target_power_ctl(0)) == 0) {
					thinkos_sleep(ms);
					err = target_power_ctl(1);
				}
			} else {
				printf("ERROR: invalid argument %s\n", argv[1]);
				return -1;
			}
		}
	}

	if (err < 0) {
		printf("ERROR: power: %s.\n", target_strerror(err));
	}

	return err;
}

