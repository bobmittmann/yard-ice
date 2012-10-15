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

int cmd_select(FILE * f, int argc, char ** argv)
{
	jtag_tap_t * tap;
	value_t val;
	int n;
	
	argc--;
	argv++;

	if (argc) {
		if ((n = eval_uint32(&val, argc, argv)) < 0) {
			DCC_LOG(LOG_WARNING, "arg_eval(), addr");
			return n;
		}
		if (n != argc) {
			fprintf(f, "usage: select [POSITION]\n");
			return -3;
		}

		n = val.int32;
	}  else {
		n = 0;
	}

	if (jtag_tap_get(&tap, n) != JTAG_OK) {
		return -4;
	}

	if (jtag_tap_select(tap) != JTAG_OK) {
		return n;
	}

	return 0;
}

