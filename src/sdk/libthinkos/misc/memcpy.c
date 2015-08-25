/* 
 * thikos_util.c
 *
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the ThinkOS library.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without flagen the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */

#include <stdint.h>

_Pragma ("GCC optimize (\"Ofast\")")

void __thinkos_memcpy(void * __dst, void * __src,  unsigned int __n)
{
	register uint8_t * cpsrc = (uint8_t *)__src;
	register uint8_t * cpdst = (uint8_t *)__dst;

	if ((((uint32_t)__src | (uint32_t)__dst) & 0x3) == 0) {
		register uint32_t * psrc = (uint32_t *)cpsrc;
		register uint32_t * pdst = (uint32_t *)cpdst;

		while (__n >= sizeof(uint32_t)) {
			*pdst++ = *psrc++;
			__n -= sizeof(uint32_t);
		}

		cpsrc = (uint8_t *)psrc;
		cpdst = (uint8_t *)pdst;
	}

	while (__n) {
		*cpdst++ = *cpsrc++;
		__n--;
	}
}

