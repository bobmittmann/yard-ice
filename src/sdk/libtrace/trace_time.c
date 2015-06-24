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
 * @file trace_time.c
 * @brief Real-time trace
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include "trace-i.h"

int trace_ts2time(struct timeval * tv, uint32_t ts)
{
	uint32_t us;

	if (tv == NULL)
		return -1;

	us = trace_ts2us(ts);

	tv->tv_sec = us / 1000000;
	tv->tv_usec = us - (tv->tv_sec * 1000000);

	return 0;
}

int trace_tm2time(struct timeval * tv, uint64_t tm)
{
	uint64_t us;

	if (tv == NULL)
		return -1;

	us = trace_tm2us(tm);

	tv->tv_sec = us / 1000000;
	tv->tv_usec = us - ((uint64_t)tv->tv_sec * 1000000LL);

	return 0;
}
