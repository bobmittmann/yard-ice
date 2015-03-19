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
 * @file cmd_cfgerase.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/shell.h>

int cmd_cfgerase(FILE *f, int argc, char ** argv)
{
	char s[64];

	if (argc > 1)
		return SHELL_ERR_EXTRA_ARGS;

	fprintf(f, "\nErase the configuration [y/n]? ");

	fgets(s, 32, f);

	if (tolower(s[0]) == 'y') {
		clearenv();
		fprintf(f, "\nFactory defaults restored.\n");
	}

	return 0;
}

