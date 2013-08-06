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
 * @file show_line_hex8.c
 * @brief YARD-ICE libhexdump
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdint.h>
#include <stdio.h>

void show_line_hex8(FILE * f, uint32_t  addr, const uint8_t * data, int count)
{
	uint32_t base;
	uint32_t val;
	uint8_t * cp;
	int i;
	int j;
	int n;

	/* 16 bytes base alignment */
	base = addr & ~(16 - 1);
	j = (base < addr) ? (addr - base) : 0;
	n = count + j;

	cp = (uint8_t *)data; 
	for (i = 0; i < n; i += 1) {
		if (i < j) {
			fprintf(f, "__ ");
		} else {

			val = cp[0];
			fprintf(f, "%02x ", val);
			cp += 1;
		}
	}

	for (; i < 16; i += 1)
		fprintf(f, "   ");
}

