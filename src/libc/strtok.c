/* 
 * Copyright(c) 2004-2012 BORESTE (www.boreste.com). All Rights Reserved.
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
 * @file strtok.c
 * @brief YARD-ICE libc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdlib.h>
#include <string.h>

char * strtok(char * str, const char * delim)
{
	static char * rem;
	char * tok;
	char * dp;
	int d;
	int c;

	if ((str == NULL) && ((str = rem) == NULL)) {
		return NULL;
	}

	/* Skip leading delimiters */
	c = *str;
	dp = (char *)delim;
	while ((d = *dp) != '\0') {
		if (c == d) {
			c = *++str;
			dp = (char *)delim;
		} else {
			dp++;
		}
	}

	if (c == '\0') {
		rem = NULL;
		return rem;
	}

	tok = str;

	for (;;) {
		c = *str;
		dp = (char *)delim;
		for (;;) {	
			if ((d = *dp) == c) {
				if (c == '\0') {
					rem = NULL;
				} else {
					rem = str + 1;
					*str = '\0';
				}
				return tok;
			}

			if (d == '\0')
				break;
			dp++;
		}
		str++;
	}
}

