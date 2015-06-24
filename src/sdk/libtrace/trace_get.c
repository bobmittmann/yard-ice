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

int trace_getfirst(struct trace_entry * entry, char * s, int len)
{
	uint32_t tail;

	if (entry == NULL)
		return -1;

	tail = trace_ring.tail;

	entry->dt = 0;
	entry->idx = tail;

	if ((int32_t)(trace_ring.head - tail) < 2) {
		entry->ref = NULL;
		entry->tm = trace_ring.tm;
		return -1;
	}

	entry->ref = trace_ring.buf[tail++ & (TRACE_RING_SIZE - 1)].ref;
	entry->tm = trace_ring.buf[tail++ & (TRACE_RING_SIZE - 1)].ts;

	if (s == NULL)
		len = 0;

	return trace_fmt(entry, s, len);
}

int trace_getnext(struct trace_entry * entry, char * s, int len)
{
	uint32_t tail;
	uint32_t ts;
	uint32_t dt;
	int ret;

	if (entry == NULL)
		return -1;

	thinkos_mutex_lock(trace_ring.mutex);

	tail = trace_ring.tail;
	if ((int32_t)(entry->idx - tail) > 0)
		tail = entry->idx;

	if ((int32_t)(trace_ring.head - tail) < 2) {
		ret = -1;
	} else {
		entry->idx = tail;
		entry->ref = trace_ring.buf[tail++ & (TRACE_RING_SIZE - 1)].ref;
		ts = trace_ring.buf[tail++ & (TRACE_RING_SIZE - 1)].ts;
		dt = ts - (entry->tm & 0xffffffff);
		entry->dt = dt;
		entry->tm += dt;

		if (s == NULL)
			len = 0;

		ret = trace_fmt(entry, s, len);
	}

	thinkos_mutex_unlock(trace_ring.mutex);

	return ret;
}

int trace_tail(struct trace_entry * entry)
{
	uint32_t tail;
	uint32_t ts;
	uint32_t dt;

	if (entry == NULL)
		return -1;

	thinkos_mutex_lock(trace_ring.mutex);

	tail = trace_ring.tail;

	entry->dt = 0;
	entry->idx = tail;

	if ((int32_t)(trace_ring.head - tail) < 2) {
		entry->ref = NULL;
		entry->tm = trace_ring.tm;
	} else {
		entry->ref = trace_ring.buf[tail++ & (TRACE_RING_SIZE - 1)].ref;
		ts = trace_ring.buf[tail++ & (TRACE_RING_SIZE - 1)].ts;
		dt = ts - (trace_ring.tm & 0xffffffff);
		entry->tm = trace_ring.tm + dt;
	}

	thinkos_mutex_unlock(trace_ring.mutex);

	return 0;
}


void trace_flush(struct trace_entry * entry)
{
	uint32_t head;
	uint32_t tail;
	uint32_t ts;
	uint32_t dt;

	thinkos_mutex_lock(trace_ring.mutex);

	head = trace_ring.head;
	tail = trace_ring.tail;

	if ((int32_t)(head - tail) < 2) {
		ts = (trace_ring.tm & 0xffffffff);
	} else {
		ts = trace_ring.buf[(tail + 1) & (TRACE_RING_SIZE - 1)].ts;
	}

	if (entry == NULL) {
		tail = head;
	} else {
		if ((int32_t)(head - entry->idx) < 0)
			entry->idx = head;
		tail = entry->idx;
	}

	/* update the time at tail */
	dt = ts - (trace_ring.tm & 0xffffffff);
	trace_ring.tm += dt;
	/* set the new tail */
	trace_ring.tail = tail;

	thinkos_mutex_unlock(trace_ring.mutex);
}
