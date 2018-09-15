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

int cmd_relay(FILE * f, int argc, char ** argv)
{
	int err;
	int ms;

	if ((argc < 2) || (argc > 3)) {
//		printf(msg_reset_usage);
		return -1;
	}

	if ((strcmp(argv[1], "on") == 0) || (strcmp(argv[1], "1") == 0)) {
		fprintf(f, "Target relay on...\n");
		err = target_relay(true);
	} else {
		if ((strcmp(argv[1], "off") == 0) || (strcmp(argv[1], "0") == 0)) {
			fprintf(f, "Target relay off...\n");
			err = target_relay(false);
		} else {
			if ((strcmp(argv[1], "cycle") == 0) || 
				(strcmp(argv[1], "cyc") == 0)) {
				if (argc == 3) {
					ms = strtoul(argv[2], NULL, 0);
				} else {
					ms = 250;
				}
				fprintf(f, "Target relay cycle...\n");
				if ((err = target_relay(false)) == 0) {
					thinkos_sleep(ms);
					err = target_relay(true);
				}
			} else {
				printf("ERROR: invalid argument %s\n", argv[1]);
				return -1;
			}
		}
	}

	if (err < 0) {
		printf("ERROR: relay: %s.\n", target_strerror(err));
	}

	return err;
}

