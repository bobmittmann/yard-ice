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
 * @file cmd_eint.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "target.h"
#include "debugger.h"

int cmd_eint(FILE * f, int argc, char ** argv)
{
	int err;

	if (argc > 1) {
		return -1;
	}

	if ((err = target_int_enable()) < 0) {
		fprintf(f, "#ICE error: %s\n", target_strerror(err));
		return -1;
	}

	fprintf(f, "Interrupts enabled\n");

	return 0;
}

