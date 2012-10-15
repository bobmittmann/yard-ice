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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "target.h"
#include "debugger.h"
#include "version.h"
#include "eval.h"

#include <sys/dcclog.h>

int cmd_mem_write(FILE * f, int argc, char ** argv)
{
	uint32_t buf[16];
	uint32_t addr;
	value_t val;
	int cnt;
	int n;
	
	argc--;
	argv++;

	if (argc < 1) {
		printf("usage: write ADDR W1 [W2 ... Wn]\n");
		return -1;
	}

	if ((n = eval_uint32(&val, argc, argv)) < 0) {
		DCC_LOG(LOG_WARNING, "eval_uint32(), addr");
		return n;
	}
	argc -= n;
	argv += n;
	addr = val.uint32;
	DCC_LOG1(LOG_INFO, "addr=0x%08x", addr);

	cnt = 0;
	while (argc) {
		if ((n = eval_uint32(&val, argc, argv)) < 0) {
			DCC_LOG(LOG_WARNING, "eval_uint32(), size");
			return n;
		}

		buf[cnt++] = val.uint32;
		argc -= n;
		argv += n;
	} 

	return target_mem_write(addr, buf, cnt * 4);
}

