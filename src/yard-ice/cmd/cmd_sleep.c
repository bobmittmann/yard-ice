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
#include "lookup.h"
#include "eval.h"

int cmd_sleep(FILE * f, int argc, char ** argv)
{
	value_t val;
	int ms;
	int n;

	argc--;
	argv++;

	if (argc == 0)
		return ERR_PARM;


	if ((n = eval_uint32(&val, argc, argv)) < 0)
		return n;

	argc -= n;
	argv += n;

	if (argc)
		return ERR_PARM;
	ms = val.uint32;

	__os_sleep(ms);

	return 0;
}

