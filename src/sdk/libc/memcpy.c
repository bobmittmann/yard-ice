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
 * @file memcpy.c
 * @brief YARD-ICE libc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdint.h>
#include <string.h>

#include <sys/dcclog.h>

#if 0
void * memcpy(void * __dst, const void * __src, size_t __n)
{
	register uint8_t * cpsrc = (uint8_t *)__src;
	register uint8_t * cpdst = (uint8_t *)__dst;
	register uint32_t align;

	align = ((uint32_t)__src | (uint32_t)__dst) & 0x3;

	if (align == 0) {
		register uint32_t * psrc = (uint32_t *)__src;
		register uint32_t * pdst = (uint32_t *)__dst;

		while (__n >= (8 * sizeof(uint32_t))) {
			*pdst++ = *psrc++;
			*pdst++ = *psrc++;
			*pdst++ = *psrc++;
			*pdst++ = *psrc++;
			*pdst++ = *psrc++;
			*pdst++ = *psrc++;
			*pdst++ = *psrc++;
			*pdst++ = *psrc++;
			__n -= (8 * sizeof(uint32_t));
		}

		if (__n >= (4 * sizeof(uint32_t))) {
			*pdst++ = *psrc++;
			*pdst++ = *psrc++;
			*pdst++ = *psrc++;
			*pdst++ = *psrc++;
			__n -= (4 * sizeof(uint32_t));
		}

		if (__n >= (2 * sizeof(uint32_t))) {
			*pdst++ = *psrc++;
			*pdst++ = *psrc++;
			__n -= (2 * sizeof(uint32_t));
		}

		cpsrc = (uint8_t *)psrc;
		cpdst = (uint8_t *)pdst;
	} else {
		if (align == 2) {
			register uint16_t * psrc = (uint16_t *)__src;
			register uint16_t * pdst = (uint16_t *)__dst;

			while (__n >= (8 * sizeof(uint16_t))) {
				*pdst++ = *psrc++;
				*pdst++ = *psrc++;
				*pdst++ = *psrc++;
				*pdst++ = *psrc++;
				*pdst++ = *psrc++;
				*pdst++ = *psrc++;
				*pdst++ = *psrc++;
				*pdst++ = *psrc++;
				__n -= (8 * sizeof(uint16_t));
			}

			if (__n >= (4 * sizeof(uint16_t))) {
				*pdst++ = *psrc++;
				*pdst++ = *psrc++;
				*pdst++ = *psrc++;
				*pdst++ = *psrc++;
				__n -= (4 * sizeof(uint16_t));
			}

			cpsrc = (uint8_t *)psrc;
			cpdst = (uint8_t *)pdst;
		}
	}

	while (__n >= 4) {
		cpdst[0] = cpsrc[0];
		cpdst[1] = cpsrc[1];
		cpdst[2] = cpsrc[2];
		cpdst[3] = cpsrc[3];
		cpdst += 4;
		cpsrc += 4;
		__n -= 4;
	}

	if (__n >= 2) {
		cpdst[0] = cpsrc[0];
		cpdst[1] = cpsrc[1];
		cpdst += 2;
		cpsrc += 2;
		__n -= 2;
	}

	if (__n) {
		*cpdst = *cpsrc;
	}

	return __dst;
}
#endif

void * memcpy(void * __dst, const void * __src, size_t __n)
{
	register uint8_t * cpsrc = (uint8_t *)__src;
	register uint8_t * cpdst = (uint8_t *)__dst;
	register uint32_t align;

	align = ((uint32_t)__src | (uint32_t)__dst) & 0x3;

	if (align == 0) {
		register uint32_t * psrc = (uint32_t *)__src;
		register uint32_t * pdst = (uint32_t *)__dst;

		while (__n >= sizeof(uint32_t)) {
			*pdst++ = *psrc++;
			__n -= sizeof(uint32_t);
		}

		cpsrc = (uint8_t *)psrc;
		cpdst = (uint8_t *)pdst;
	} else {
		if (align == 2) {
			register uint16_t * psrc = (uint16_t *)__src;
			register uint16_t * pdst = (uint16_t *)__dst;

			if (__n >= sizeof(uint16_t)) {
				*pdst++ = *psrc++;
				__n -= sizeof(uint16_t);
			}

			cpsrc = (uint8_t *)psrc;
			cpdst = (uint8_t *)pdst;
		}
	}

	while (__n) {
		*cpdst++ = *cpsrc++;
		__n--;
	}

	return __dst;
}

#if 0
void * memcpy(void * __dst, const void * __src, size_t __n)
{
	register uint8_t * cpsrc = (uint8_t *)__src;
	register uint8_t * cpdst = (uint8_t *)__dst;

	while (__n) {
		*cpdst++ = *cpsrc++;
		__n--;
	}

	return __dst;
}
#endif
