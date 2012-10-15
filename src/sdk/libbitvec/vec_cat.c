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
 * @file vec_cat.c
 * @brief YARD-ICE libbitvec
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdlib.h>
#include <stdint.h>

int vec_cat(void * dst_vec, int dst_len, const void * src_vec, int src_len)
{
	uint8_t * dst = (uint8_t *)dst_vec;
	uint8_t * src = (uint8_t *)src_vec;
	uint8_t a;
	int q;
	int r;
	int n;
	int m;
	int i;

	q = dst_len / 8;
	r = dst_len % 8;

	dst += q;

	n = 8 - r;
	m = ((src_len + r) + 7) / 8;

	a = *dst & (0xff >> n);
	for (i = 0; i < m; i++) {
		a |= *src << r;
		*dst = a;
		a = *src >> n;
		src++;
		dst++;
	}

	return dst_len + src_len;
}

