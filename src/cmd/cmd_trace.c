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

void trace_file_set(FILE * f);
void trace_autoflush_set(bool enabled);

int cmd_trace(FILE * f, int argc, char ** argv)
{
	bool flush = false;
	bool dump = true;

	argc--;
	argv++;

	while (argc) {
		if ((strcmp(*argv, "sup") == 0) || (strcmp(*argv, "s") == 0)) {
			fprintf(f, "Supervisory trace set to this console.\n");
			trace_file_set(f);
			flush = true;
		} else if ((strcmp(*argv, "auto") == 0) || 
				   (strcmp(*argv, "a") == 0)) {
			fprintf(f, "Supervisory auto-flush enabled.\n");
			trace_autoflush_set(true);
			dump = false;
		} else if ((strcmp(*argv, "keep") == 0) || 
				   (strcmp(*argv, "k") == 0)) {
			fprintf(f, "Supervisory auto-flush disabled.\n");
			trace_autoflush_set(false);
			dump = false;
		} else if ((strcmp(*argv, "flush") == 0) || 
				   (strcmp(*argv, "f") == 0)) {
			fprintf(f, "flush\n");
			flush = true;
		} else { 
			return ERR_PARM;
		}	

		argc--;
		argv++;
	}

	if (dump || flush) {
		struct trace_iterator trace_it;
		struct trace_entry * trace;

		fprintf(f, "---------\n");

		trace_tail(&trace_it);

		while ((trace = trace_getnext(&trace_it)) != NULL) {
			struct timeval tv;
			unsigned int lvl;
			char s[80];

			trace_fmt(trace, s, sizeof(s));
			trace_ts2timeval(&tv, trace->dt);

			if ((lvl = trace->ref->lvl) <= TRACE_LVL_WARN)
				fprintf(f, "%s %2d.%06d: %s,%d: %s\n",
						trace_lvl_nm[lvl],
						(int)tv.tv_sec, (int)tv.tv_usec,
						trace->ref->func, trace->ref->line, s);
			else
				fprintf(f, "%s %2d.%06d: %s\n",
						trace_lvl_nm[lvl],
						(int)tv.tv_sec, (int)tv.tv_usec, s);
		}

		if (flush)
			trace_flush(&trace_it);
	}

	return 0;
}

