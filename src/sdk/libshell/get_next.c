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
 * @file shell.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/shell.h>

#define __SHELL_I__
#include "shell-i.h"

#include <sys/dcclog.h>

char * cmd_get_next(char ** linep)
{
	char * cp = *linep;
	char * cmd;
	int c;
	
	/* remove leading spaces */
	for (; isspace(c = *cp); cp++);

	if (c == '\0')
		return NULL;

	cmd = cp;

	do {
		if (c == ';') {
			*cp = '\0';
			cp++;
			break;
		}

		cp++;

		/* Quotes */
		if ((c == '\'') || (c == '\"')) {
			int qt = c;
			for (; ((c = *cp) != '\0'); cp++) {
				if (c == qt) {
					cp++;
					break;
				}	
			}
		}

		c = *cp;
	} while (c != '\0');

	*linep = cp;

	return cmd;
}

