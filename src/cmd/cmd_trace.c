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

extern volatile FILE * spv_fout;
extern volatile bool spv_auto_flush;

int cmd_trace(FILE * f, int argc, char ** argv)
{
	struct trace_entry trace;
	struct timeval tv;
	bool flush = false;
	char s[80];

	argc--;
	argv++;

	if (argc) {
		if ((strcmp(*argv, "sup") == 0) || (strcmp(*argv, "s") == 0)) {
			fprintf(f, "Supervisory trace set to this console.\n");
			spv_fout = f;
			return 0;
		}

		if ((strcmp(*argv, "auto") == 0) || (strcmp(*argv, "a") == 0)) {
			fprintf(f, "Supervisory auto-flush enabled.\n");
			spv_auto_flush = true;
			return 0;
		}

		if ((strcmp(*argv, "keep") == 0) || (strcmp(*argv, "k") == 0)) {
			fprintf(f, "Supervisory auto-flush disabled.\n");
			spv_auto_flush = false;
			return 0;
		}

		if ((strcmp(*argv, "flush") == 0) || (strcmp(*argv, "f") == 0)) {
			fprintf(f, "flush\n");
			flush = true;
		} else { 
			return ERR_PARM;
		}	
	}

	fprintf(f, "---------\n");

	trace_tail(&trace);

	while (trace_getnext(&trace, s, sizeof(s)) >= 0) {
		trace_ts2timeval(&tv, trace.dt);
		if (trace.ref->lvl <= TRACE_LVL_WARN)
			fprintf(f, "%s %2d.%06d: %s,%d: %s\n",
					trace_lvl_nm[trace.ref->lvl],
					(int)tv.tv_sec, (int)tv.tv_usec,
					trace.ref->func, trace.ref->line, s);
		else
			fprintf(f, "%s %2d.%06d: %s\n",
					trace_lvl_nm[trace.ref->lvl],
					(int)tv.tv_sec, (int)tv.tv_usec, s);
	}

	if (flush)
		trace_flush(&trace);

	return 0;
}

