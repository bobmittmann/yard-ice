
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
 * @file cmd_run.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <yard-ice/drv.h>

#include <sys/dcclog.h>

int cmd_beep(FILE * f, int argc, char ** argv)
{
	unsigned int tone = 1;
	unsigned int time = 100;

	if (argc > 3) {
		fprintf(f, "usage: beep [tone [time]]\n");
		return -1;
	}

	if (argc > 1) {
		tone = strtoul(argv[1], NULL, 0);
		if (argc > 2) {
			time = strtoul(argv[2], NULL, 0);
		} 
	}

	tone_play(tone, time);

	return 0;
}

