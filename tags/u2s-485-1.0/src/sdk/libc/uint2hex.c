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
 * @file uint2hex.c
 * @brief YARD-ICE libc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

const char __hextab[] = {
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

int uint2hex(char * s, unsigned int val)
{
	int n;
	int c;
	int i;

	/* value is zero ? */
	if (!val) {
		*s++ = '0';
		*s = '\0';
		return 1;
	}

	n = 0;
	for (i = 0; i < (sizeof(unsigned int) * 2); i++) {
		c = val >> ((sizeof(unsigned int) * 8) - 4);
		val <<= 4;
		if ((c != 0) || (n != 0)) {
			s[n++] = __hextab[c];
		}
	}

	s[n] = '\0';

	return n;
}

