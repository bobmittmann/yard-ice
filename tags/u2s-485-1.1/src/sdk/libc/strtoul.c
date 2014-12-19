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
 * @file strtoul.c
 * @brief YARD-ICE libc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdlib.h>

unsigned long strtoul(const char * __s, char ** __endp, int base)
{
	unsigned long val = 0;
	char * cp = (char *)__s;
	char c;

	for (; ((c = *cp) == ' '); cp++);

	if ((base == 0 || base == 16) && (c == '0')) {
		cp++;
		if ((*cp & ~('a' - 'A')) == 'X') {
			base = 16;
			cp++;
		}
	}

	if (base == 0)
		base = c == '0' ? 8 : 10;

	switch (base) {
		while ((c = *cp) != '\0') {
			if ((c < '0') || (c > '7'))
				break;
			val <<= 3;
			val += c - '0';
			cp++;
		}
		break;
	case 10:
		while ((c = *cp) != '\0') {
			if ((c < '0') || (c > '9'))
				break;
			val = ((val << 2) + val) << 1;
			val += c - '0';
			cp++;
		}
		break;
	case 16:
		while ((c = *cp) != '\0') {

			if (c >= 'a')
				c &= ~('a' - 'A');

			if ((c >= 'A') && (c <= 'F'))
				c -= ('A' - 10);
			else {
				if ((c >= '0') && (c <= '9'))
					c -= '0';
				else
					break;
			}

			val = val << 4;
			val += c;
			cp++;
		}
		break;
	}
	
	if (__endp)
		*__endp = cp;

	return val;
}

