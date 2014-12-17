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
 * @file fgets.c
 * @brief YARD-ICE libc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/file.h>

#define IN_BS      '\x8'
#define IN_DEL      0x7F
#define IN_EOL      '\r'
#define IN_SKIP     '\3'
#define IN_EOF      '\x1A'
#define IN_ESC      '\033'

#define OUT_DEL     "\x8 \x8"
#define OUT_EOL     "\r\n"
#define OUT_SKIP    "^C\r\n"
#define OUT_EOF     "^Z"
#define OUT_BEL     "\7"

char * fgets(char * s, int size, FILE * f)
{
	char * cp;
	char * end;
	int len;

#if 0
	char buf[1];
	int c;
	int pos;

	/* left room to '\0' */
	size--;
	pos = 0;

	do {
		if (f->op->read(f->data, buf, sizeof(char)) <= 0) {
			return NULL;
		}

		c = buf[0];

		if (c == '\n') {
			break;
		}

		s[pos++] = c;

		if (pos == size)
			break;
	}

	s[pos] = '\0';

#endif

	if ((len = f->op->read(f->data, s, size - 1)) <= 0) {
		return NULL;
	}

	for (cp = s, end = s + len; cp < end ; ++cp) {
		if (*cp == '\n') {
			++cp;
			break;
		}
	}

	*cp = '\0';

	return s;
}

