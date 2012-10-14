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
 * @file vec_fmt.c
 * @brief YARD-ICE libbitvec
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdint.h>

char * vec_fmt(char * buf, const void * vec, int len)
{
	uint8_t * ptr = (uint8_t *)vec;
	char * s = buf;
	int x;
	int i;

	x = 0;
	for (i = 0; i < len; i++) {
		if ((i % 8) == 0)
			x = *ptr++;
		*s++ = (x & 0x01) ? '1' : '0';
		x >>= 1;
	}

	*s = '\0';

	return buf;
}

