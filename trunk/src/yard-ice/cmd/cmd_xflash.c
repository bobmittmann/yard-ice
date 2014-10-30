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
 * @file cmd_let.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "target.h"
#include "debugger.h"
#include "eval.h"

int usb_xflash(uint32_t offs, uint32_t len);

int cmd_xflash(FILE * f, int argc, char ** argv)
{
	uint32_t offs = 0x00000;
	uint32_t pri;
	value_t val;
	int ret;
	int n;

	argc--;
	argv++;

	if (argc > 1) {
		if ((n = eval_uint32(&val, argc, argv)) < 0) {
			fprintf(f, "Can't evaluate\n");
			return n;
		}
		argc -= n;
		if (argc > 1) {
			//		fprintf(f, msg_init_usage);
			//		return SHELL_ERR_EXTRA_ARGS;
			return -1;
		}
		offs = val.uint32;
	}

	fprintf(f, "Firmware update...\n");
	fflush(f);

	pri = cm3_primask_get();
	cm3_primask_set(1);

	ret = usb_xflash(offs, 256 * 1024);

	cm3_primask_set(pri);

	return ret;
}

