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
 * @file mem_hexdump.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "debugger.h"
#include "config.h"
#include "dbglog.h"
#include "hexdump.h"

int mem_hexdump(FILE * f, uint32_t addr, int size)
{
	uint8_t buf[265];
	unsigned int base;
	int count = 0; 
	int n;
	
	addr &= ~(sizeof(uint32_t) - 1);
	base = addr & ~(256 - 1);
	if (base < addr)
		n = (base + 256) - addr;
	else
		n = 256;

	while (size > 0) {
		n = (n < size) ? n : size;

		DCC_LOG1(LOG_MSG, "n=%d", n); 

		if ((n = target_mem_read(addr, &buf, n)) < 0) {
			DCC_LOG1(LOG_WARNING, "target_mem_read() %d fail!", n); 
			return n;
		}

		if (n == 0)
			break;

		DCC_LOG2(LOG_MSG, "addr:%08x n:%d", addr, n); 
		show_hex32(f, addr, buf, n);

		addr += n;
		size -= n;
		count += n;
		n = 256;
	}

	DCC_LOG1(LOG_MSG, "count=%d", count); 

	return count;
}

