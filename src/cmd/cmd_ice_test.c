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
#include <sys/dcclog.h>

#include "debugger.h"
#include "eval.h"

#define ICET_ARGMAX 16

int cmd_ice_test(FILE *f, int argc, char ** argv)
{
	uint32_t parm[ICET_ARGMAX];
	value_t val;
	uint32_t req;
	int cnt;
	int n;

	argc--;
	argv++;

	if (argc < 1) {
		fprintf(f, "usage: icetst REQ [ARG1 ... ARGn]\n");
		return -1;
	}

	if ((n = eval_uint32(&val, argc, argv)) < 0) {
		DCC_LOG(LOG_WARNING, "eval_uint32(), addr");
		return n;
	}
	argc -= n;
	argv += n;
	req = val.uint32;
	DCC_LOG1(LOG_INFO, "req=%d", req);

	for (cnt = 0; cnt < ICET_ARGMAX ; ++cnt)
		parm[cnt] = 0;

	cnt = 0;
	while (argc) {
		if ((n = eval_uint32(&val, argc, argv)) < 0) {
			DCC_LOG(LOG_WARNING, "eval_uint32(), size");
			return n;
		}

		DCC_LOG2(LOG_INFO, "arg[%d]=0x%08x", cnt, val.uint32);
		parm[cnt++] = val.uint32;
		argc -= n;
		argv += n;
	} 

	return target_ice_test(f, req, cnt, parm);
}

