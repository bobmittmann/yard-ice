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
 * @file memset.c
 * @brief YARD-ICE libc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdint.h>
#include <string.h>

/*
void * memset(void * s, int c, size_t n)
{
	register uint8_t * cp = (uint8_t *)s;

	while (n) {
		*cp++ = c;
		n--;
	}

	return s;
}
*/

void * memset(void * __dst, int __c, size_t __n)
{
	register uint8_t * cpdst = (uint8_t *)__dst;
	register uint32_t align;
	register uint32_t data;

	data = __c & 0xff;
	data |= data << 8;
	data |= data << 16;

	align = (uint32_t)__dst & 0x3;

	if (align == 0) {
		register uint32_t * pdst = (uint32_t *)__dst;

		while (__n >= (8 * sizeof(uint32_t))) {
			*pdst++ = data;
			*pdst++ = data;
			*pdst++ = data;
			*pdst++ = data;
			*pdst++ = data;
			*pdst++ = data;
			*pdst++ = data;
			*pdst++ = data;
			__n -= (8 * sizeof(uint32_t));
		}

		if (__n >= (4 * sizeof(uint32_t))) {
			*pdst++ = data;
			*pdst++ = data;
			*pdst++ = data;
			*pdst++ = data;
			__n -= (4 * sizeof(uint32_t));
		}

		if (__n >= (2 * sizeof(uint32_t))) {
			*pdst++ = data;
			*pdst++ = data;
			__n -= (2 * sizeof(uint32_t));
		}

		cpdst = (uint8_t *)pdst;
	} else if (align == 2) {
		register uint16_t * pdst = (uint16_t *)__dst;

		while (__n >= (8 * sizeof(uint16_t))) {
			*pdst++ = data;
			*pdst++ = data;
			*pdst++ = data;
			*pdst++ = data;
			*pdst++ = data;
			*pdst++ = data;
			*pdst++ = data;
			*pdst++ = data;
			__n -= (8 * sizeof(uint16_t));
		}

		if (__n >= (4 * sizeof(uint16_t))) {
			*pdst++ = data;
			*pdst++ = data;
			*pdst++ = data;
			*pdst++ = data;
			__n -= (4 * sizeof(uint16_t));
		}
		cpdst = (uint8_t *)pdst;
	}

	while (__n >= 4) {
		cpdst[0] = data;
		cpdst[1] = data;
		cpdst[2] = data;
		cpdst[3] = data;
		cpdst += 4;
		__n -= 4;
	}

	if (__n >= 2) {
		cpdst[0] = data;
		cpdst[1] = data;
		cpdst += 2;
		__n -= 2;
	}

	if (__n) {
		*cpdst = data;
	}

	return __dst;
}

