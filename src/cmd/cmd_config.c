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
 * @file cmd_config.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "config.h"
#include "target.h"
#include "debugger.h"


int cmd_config(FILE *f, int argc, char ** argv)
{
	char s[128];
	char * env;
	char * cp;
	int i;
/*
	if (argc > 2) {
		n = strlen(argv[2]);

		if ((n = config_set(argv[1], argv[2], n)) < 0) {
			fprintf(f, "Config set error!\n");
			return n;
		};
	}

	if (argc > 1) {
		if ((n = config_get(argv[1], s, 63)) < 0) {
			fprintf(f, "File not found!\n");
			return n;
		};

		s[n] = '\0';
		fprintf(f, "'%s'\n", s);

		return 0;
	}
*/

//	return config_list(f);

	if (argc > 2) {

		strcpy(s, argv[2]);

		for (i = 3; i < argc; i++) {
			strcat(s, " ");
			strcat(s, argv[i]);
		}

		if (setenv(argv[1], s, 1) < 0) {
			fprintf(f, "setenv() error!\n");
			return -1;
		};
	}

	if (argc > 1) {
		if ((cp = getenv(argv[1])) == NULL) {
			fprintf(f, "getenv(): not set!\n");
			return -1;
		};

		fprintf(f, "'%s'\n", cp);

		return 0;
	}


#if 0
	config_list(f);
#else
	for (i = 0; (env =  environ[i]) != NULL; i++) {
		printf("%s\n", env);
	};
#endif

	return 0;
}

