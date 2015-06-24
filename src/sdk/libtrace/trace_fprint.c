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
 * @file trace.c
 * @brief Real-time trace
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include "trace-i.h"
#include <stdarg.h>

int trace_fprint(FILE * f, unsigned int opt)
{
	char s[TRACE_STRING_MAX + 1];
	const struct trace_ref * ref;
	uint32_t ts_prev;
	uint32_t ts;
	int32_t dt;
	int32_t sec;
	uint32_t ms;
	uint32_t us;
	uint32_t tail;
	int ret = 0;

	if ((opt & TRACE_UNSAFE) == 0)
		thinkos_mutex_lock(trace_ring.mutex);

	if (opt & TRACE_ALL) {
		tail = trace_ring.tail;
		ts_prev = trace_ring.tm;
	} else {
		tail = trace_ring.print_pos;
		ts_prev = trace_ring.print_tm;
	}

	for (; tail != trace_ring.head; tail++) {
		ref = trace_ring.buf[tail++ & (TRACE_RING_SIZE - 1)].ref;
		ts = trace_ring.buf[tail++ & (TRACE_RING_SIZE - 1)].ts;
		if (opt & TRACE_ABSTIME) {
			us = __timer_ts2us(ts);
			sec = us / 1000000;
			us -= (sec * 1000000);
			ms = us / 1000;
			us = us - (ms * 1000);
		} else {
			dt = __timer_ts2us((int32_t)(ts - ts_prev));
			sec = dt / 1000000;
			dt -= (sec * 1000000);
			ms = dt / 1000;
			us = dt - (ms * 1000);
		}
		ts_prev = ts;

		ret =  trace_fmt(s, sizeof(s), ref, &tail);

		if (opt & TRACE_COUNT)
			ret = fprintf(f, "%5d %4d,%03d.%03d: %s\n", tail, sec, ms, us, s);
		else
			ret = fprintf(f, "%4d,%03d.%03d: %s\n", sec, ms, us, s);

		if (ret < 0)
			goto done;

		if (ret < 0)
			goto done;
	}

	if ((opt & TRACE_ALL) == 0)
		trace_ring.print_pos = tail;

	trace_ring.print_tm = ts_prev;

	if (opt & TRACE_FLUSH) {
		trace_ring.tail = tail;
		trace_ring.print_pos = tail;
		trace_ring.tm = ts_prev;
	}

done:
	if ((opt & TRACE_UNSAFE) == 0)
		thinkos_mutex_unlock(trace_ring.mutex);

	if (ret > 0)
		ret = 0;

	return ret;
}
