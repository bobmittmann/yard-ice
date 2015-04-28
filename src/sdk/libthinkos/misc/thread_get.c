/* 
 * thikos_util.c
 *
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the ThinkOS library.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without flagen the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */

#define __THINKOS_SYS__
#include <thinkos_sys.h>

int __thinkos_thread_get(struct thinkos_rt * rt, struct thinkos_thread * st, 
						 unsigned int th)
{
	uint32_t * src;
	uint32_t * dst;
	int i;

	if ((th >= THINKOS_THREADS_MAX) || (rt->ctx[th] == NULL)) {
		return -1;
	}

	st->idx = th;

#if THINKOS_ENABLE_THREAD_STAT
	st->wq = rt->th_stat[th] >> 1;
	st->tmw = rt->th_stat[th] & 1;
#else
	for (i = 0; i < THINKOS_WQ_LST_END; ++i) {
		if (rt->wq_lst[i] & (1 << th))
			break;
	}
	if (i == THINKOS_WQ_LST_END)
		return -1; /* not found */
	st->wq = i;
 #if THINKOS_ENABLE_CLOCK
	st->tmw = rt->wq_clock & (1 << th) ? 1 : 0;
 #else
	st->tmw = 0;
 #endif
#endif /* THINKOS_ENABLE_THREAD_STAT */

#if THINKOS_ENABLE_THREAD_ALLOC
	st->alloc = rt->th_alloc[0] & (1 << th) ? 1 : 0;
#else
	st->alloc = 0;
#endif

#if THINKOS_ENABLE_TIMESHARE
	st->sched_val = rt->sched_val[th];
	st->sched_pri = rt->sched_pri[th]; 
#else
	st->sched_val = 0;
	st->sched_pri = 0;
#endif

#if THINKOS_ENABLE_CLOCK
	st->timeout = (int32_t)(rt->clock[th] - rt->ticks); 
#else
	st->timeout = -1;
#endif

#if THINKOS_ENABLE_PROFILING
	st->cyccnt = rt->cyccnt[th];
#else
	st->cyccnt = 0;
#endif

#if THINKOS_ENABLE_THREAD_INFO
	st->th_inf = rt->th_inf[th];
#else
	st->th_inf = NULL;
#endif

	st->sp = (uint32_t)rt->ctx[th];
	src = (uint32_t *)rt->ctx[th];
	dst = (uint32_t *)&st->ctx;
	for (i = 0; i < 16; ++i)
		dst[i] = src[i];

	return 0;
}

