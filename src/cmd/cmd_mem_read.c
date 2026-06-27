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
 * @file .c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/shell.h>

#include "target.h"
#include "debugger.h"
#include "hexdump.h"
#include "eval.h"

#include <sys/dcclog.h>

int cmd_mem_read(FILE * f, int argc, char ** argv)
{
	struct mem_range * dump = target_dump_range();
	int width = HEXDUMP_32_BITS;
	value_t val;
	uint32_t addr;
	uint32_t size;
	int n;
	int c;

	argc--;
	argv++;

	c = *argv[0];
	if ((argc) && ((c == 'b') || (c == 'h'))) {
		if (c == 'b')
			width = HEXDUMP_8_BITS;
		else
			width = HEXDUMP_16_BITS;
		argc--;
		argv++;
	};

	if (argc) {
		if ((n = eval_uint32(&val, argc, argv)) < 0) {
			DCC_LOG(LOG_WARNING, "eval_uint32(), addr");
			return n;
		}
		argc -= n;
		argv += n;
		addr = val.uint32;
		DCC_LOG2(LOG_INFO, "addr=%08x n=%d", addr, n);
	} else
		addr = (uint32_t)dump->base;

	if (argc) {
		if ((n = eval_uint32(&val, argc, argv)) < 0) {
			DCC_LOG(LOG_WARNING, "eval_uint32(), size");
			return n;
		}
		size = val.uint32;
		DCC_LOG2(LOG_INFO, "size=%d n=%d", size, n);
		argc -= n;
		argv += n;
	} else
		size = (dump->size + 3) & ~0x03;

	if (argc) {
		return SHELL_ERR_EXTRA_ARGS;
	}

	if (size == 0)
		size = 64;

	dump->base = addr & ~0x03;
	dump->size = 0;
	
	size = mem_hexdump(f, dump->base, size, width);

	dump->base += size;
	dump->size = size;

	return 0;
}

