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
 * @file strtol.c
 * @brief YARD-ICE libc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdlib.h>

long int strtol(const char * __s, char ** __endp, int __base)
{
	char c;
	char * cp = (char *)__s;
	int val = 0;
	int inv = 0;

	for (; ((c = *cp) == ' '); cp++);

	if (c == '-') {
		inv = 1;
		cp++;
	} 

	val = strtoul(cp, __endp, __base);

	if (inv)
		val = -val;
	
	return val;
}

