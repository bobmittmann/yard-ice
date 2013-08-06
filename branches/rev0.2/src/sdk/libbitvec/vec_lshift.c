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
 * @file vec_lshift.c
 * @brief YARD-ICE libbitvec
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdint.h>

int vec_lshift(uint32_t * dst, const uint32_t * src, int len, int bits)
{
	uint32_t x0;
	uint32_t x1;
	int i;
	int n;

	n = (len + 31) / 32;

	x0 = 0;
	for (i = 0; i < n; i++) {
		x1 = src[i];
		dst[i] = (x1 << bits) | (x0 >> (32 - bits));
		x0 = x1;
	}

	return 0;
}

