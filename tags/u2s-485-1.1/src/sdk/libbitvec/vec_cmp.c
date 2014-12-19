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
 * @file vec_cmp.c
 * @brief YARD-ICE libbitvec
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdint.h>
#include <string.h>

int vec_cmp(const void * vec_a, const void * vec_b, int len)
{
	uint8_t * pa = (uint8_t *)vec_a;
	uint8_t * pb = (uint8_t *)vec_b;
	uint8_t mask;
	uint8_t x;
	int rem;
	int i;

	for (i = 0; i < (len / 8); i++) {
		if ((x = (*pa ^ *pb)) != 0) {
			return i + ffs(x);
		}
		pa++;
		pb++;
	}

	if ((rem = (len % 8)) != 0) {
		mask = (1 << rem) - 1;	
		if ((x = ((*pa & mask) ^ (*pb & mask))) != 0) {
			return i + ffs(x);
		}
	}

	return 0;
}

