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
 * @file vec_zeros.c
 * @brief YARD-ICE libbitvec
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdint.h>

int vec_zeros(void * vec, int len)
{
	uint8_t * dst = (uint8_t *)vec;
	int n;
	int i;

	n = (len + 7) / 8;

	for (i = 0; i < n; i++) {
		dst[i] = 0x00;
	}	

	return len;
}

