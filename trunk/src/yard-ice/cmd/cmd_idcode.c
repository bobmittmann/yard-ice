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
#include <ctype.h>

#include "config.h"
#include "jtag.h"
#include "eval.h"
#include "dbglog.h"

int cmd_idcode(FILE * f, int argc, char ** argv)
{
	value_t val;
	uint32_t buf[8];
	uint32_t ret[8];
	int len;
	int n;
	
	argc--;
	argv++;

	if (argc) {
		if ((n = eval_uint32(&val, argc, argv)) < 0) {
			DCC_LOG(LOG_WARNING, "arg_eval(), addr");
			return n;
		}
		argc -= n;
		argv += n;
		buf[0] = val.uint32;
		len = 32;
	} 

	if (argc) {
		if ((n = eval_uint32(&val, argc, argv)) < 0) {
			DCC_LOG(LOG_WARNING, "arg_eval(), addr");
			return n;
		}
		len = val.uint32;
		argc -= n;
		argv += n;
	} 

	if ((n = jtag_ir_scan(buf, ret, len, JTAG_TAP_IDLE)) < 0) {
		return n;
	}
	
	fprintf(f, "%08x\n", ret[0]);

	return 0;
}

