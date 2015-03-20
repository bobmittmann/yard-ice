/* 
 * Copyright(c) 2003-2012 BORESTE (www.boreste.com). All Rights Reserved.
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
 * @file memmove.c
 * @brief YARD-ICE libc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdint.h>
#include <string.h>

void * memmove(void * __dst, const void * __src, size_t __n)
{
	char *dst = __dst;
	const char *src = __src;

	if (src < dst && dst < src + __n) {
		/* Have to copy backwards */
		src += __n;
		dst += __n;
		while (__n--) {
			*--dst = *--src;
		}
	} else {
		while (__n--) {
			*dst++ = *src++;
		}
	}

	return dst;
}

