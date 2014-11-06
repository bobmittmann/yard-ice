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
 * @file cmd_trace.c
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
#include "trace.h"

extern FILE * monitor_stream;
extern bool monitor_auto_flush;

int cmd_trace(FILE * f, int argc, char ** argv)
{
	argc--;
	argv++;

	if (argc) {
		if ((strcmp(*argv, "flush") == 0) || (strcmp(*argv, "f") == 0)) {
			fprintf(f, "flush\n");
			trace_flush();
			return 0;
		} 

		if ((strcmp(*argv, "monitor") == 0) || (strcmp(*argv, "m") == 0)) {
			fprintf(f, "monitor\n");
			monitor_stream = f;
			return 0;
		}

		if ((strcmp(*argv, "auto") == 0) || (strcmp(*argv, "a") == 0)) {
			fprintf(f, "Auto flush\n");
			monitor_auto_flush = true;
			return 0;
		}

		if ((strcmp(*argv, "keep") == 0) || (strcmp(*argv, "k") == 0)) {
			fprintf(f, "Keep trace (don't flush)\n");
			monitor_auto_flush = false;
			return 0;
		}

		return ERR_PARM;
	}

	fprintf(f, "---------\n");
	trace_fprint(f, TRACE_ALL);

	return 0;
}

