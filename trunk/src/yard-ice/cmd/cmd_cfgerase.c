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
#include <ctype.h>
#include <nvconfig.h>

#include "target.h"
#include "debugger.h"

#include <debug.h>

int cmd_cfgerase(FILE *f, int argc, char ** argv)
{
	char s[64];

	fprintf(f, "\nErase the configuration [y/n]? ");

	fgets(s, 32, f);

	if (tolower(s[0]) == 'y') {
		if (config_erase() < 0) {
			fprintf(f, "\nERROR erasing configuration.\n");
			return -1;
		}
		fprintf(f, "\nFactory defaults restored.\n");
	}

	return 0;
}
