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
 * @file cmd_en_dis.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debugger.h"

int cmd_enable(FILE * f, int argc, char ** argv)
{
	if (argc != 2) {
		return -1;
	}

	argv++;
	if ((strcmp(*argv, "poll") == 0) || (strcmp(*argv, "p") == 0))
		return target_enable_ice_poll(true);

	if ((strcmp(*argv, "comm") == 0) || (strcmp(*argv, "c") == 0))
		return target_enable_comm(true);

	if ((strcmp(*argv, "debug") == 0) || (strcmp(*argv, "d") == 0))
		return target_enable_debug(true);

	return -1;
}

int cmd_disable(FILE * f, int argc, char ** argv)
{
	if (argc != 2) {
		return -1;
	}

	argv++;
	if ((strcmp(*argv, "poll") == 0) || (strcmp(*argv, "p") == 0))
		return target_enable_ice_poll(false);

	if ((strcmp(*argv, "comm") == 0) || (strcmp(*argv, "c") == 0))
		return target_enable_comm(false);

	if ((strcmp(*argv, "debug") == 0) || (strcmp(*argv, "d") == 0))
		return target_enable_debug(false);

	return -1;
}

