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
 * @file show_hex16.c
 * @brief YARD-ICE libhexdump
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdint.h>
#include <stdio.h>
#include <hexdump.h>

int show_hex16(FILE * f, uint32_t  addr, const void * buf, int size)
{
	uint8_t * cp = (uint8_t *)buf;
	uint32_t base;
	int rem = size;
	int n;

	/* the base address is allways a 16 bytes multiple */
	base = addr & ~(16 - 1);
	n = (base < addr) ? (base + 16) - addr : 16;
	n = (n < rem) ? n : rem;

	while (n > 0) {
		fprintf(f, "%08x: ", base);
		show_line_hex16(f, addr, cp, n);
		fprintf(f, ": ");
		show_line_char(f, addr, cp, n);
		fprintf(f, "\n");
		cp += n;
		addr += n;
		rem -= n;
		base += 16;
		n = (rem < 16) ? rem : 16;
	} 

	return size;
}

