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

int cmd_set(FILE * f, int argc, char ** argv)
{
	char * val;
	char * env;
	int i;

	if (argc > 1) {
		if (argc < 3) {
			fprintf(f, "Argument invalid or missing!\n");
			fprintf(f, "usage: %s [VAR VALUE]\n", argv[0]);
			return -1;
		}

		val = argv[2];

		fprintf(f, "%s=%s\n", argv[1], val);
	
		if (setenv(argv[1], val, 1) < 0) {
			fprintf(f, "setenv(): fail!\n");
			return -1;
		};

		return 0;
	}

	for (i = 0; (env =  environ[i]) != NULL; i++) {
		fprintf(f, "%s\n", env);
	};

	return 0;
}
