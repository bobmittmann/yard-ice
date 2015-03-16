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
 * @file cmd_reboot.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arch/cortex-m3.h>
#include <sys/shell.h>
#include <thinkos.h>

/*****************************************************************************
 * System reboot
 *****************************************************************************/

int cmd_reboot(FILE * f, int argc, char ** argv)
{
	if (argc > 1)
		return SHELL_ERR_EXTRA_ARGS;

	fprintf(f, "Restarting in 2 seconds...\n");

	thinkos_sleep(2000);

	cm3_sysrst();

	return 0;
}
