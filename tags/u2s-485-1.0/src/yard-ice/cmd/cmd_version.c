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

#include "version.h"

void version_fprint(FILE * f)
{
	fprintf(f, "\nYARD-ICE " VERSION_NUM " - " VERSION_DATE "\n" 
			"(c) Copyright 2013-" VERSION_YEAR 
			",  Bob Mittmann (bobmittmann@gmail.com)\n\n");
}

int cmd_version(FILE *f, int argc, char ** argv)
{
	version_fprint(f);
#if 0
	int n;
	uint8_t ver[4];
	struct esn_info esn;

	sysinfo(SYSINFO_ESN, (void *)&esn);
	n = sprintf(s, "ESN: %02x%02x-%02x%02x-%02x%02x\n",
			   esn.code, esn.version, esn.number[2], esn.number[1],
			   esn.number[0], esn.crc);
	printf(s);

	sysinfo(SYSINFO_VERSION, (void *)ver);
	n = sprintf(s, "uBoot: %d.%d\n\n", ver[0], ver[1]);
	printf(s);
#endif

	return 0;
}

