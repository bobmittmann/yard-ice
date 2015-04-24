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
#include <stdio.h>

#define __THINKOS_SYS__
#include <thinkos_sys.h>

#define __THINKOS_IRQ__
#include <thinkos_irq.h>

#include <thinkos.h>

extern const uint8_t thinkos_obj_type_lut[];

void thinkos_rt_snapshot_svc(int32_t * arg)
{
	uint32_t * dst = (uint32_t *)arg[0];
	uint32_t pri = cm3_primask_get();
	uint32_t * src;
	int i;

	cm3_primask_set(1);
#if THINKOS_ENABLE_PROFILING
	{
		int self = thinkos_rt.active;
		uint32_t cyccnt = CM3_DWT->cyccnt;
		int32_t delta = cyccnt - thinkos_rt.cycref;
		/* update the reference */
		thinkos_rt.cycref = cyccnt;
		/* update thread's cycle counter */
		thinkos_rt.cyccnt[self] += delta; 
	}
#endif

	src = (uint32_t *)&thinkos_rt;

	for (i = 0; i < (sizeof(struct thinkos_rt) / 4); ++i)
		dst[i] = src[i];

#if THINKOS_ENABLE_PROFILING
	/* Reset cycle counters */
	for (i = 0; i < THINKOS_THREADS_MAX + 1; i++)
		thinkos_rt.cyccnt[i] = 0; 
#endif

	cm3_primask_set(pri);
}

int thinkos_obj_type_get(unsigned int oid)
{
	if (oid >= THINKOS_WQ_LST_END)
		return THINKOS_EINVAL;

	return thinkos_obj_type_lut[oid];
}

uint32_t * thinkos_alloc_bmp_get(unsigned int type)
{
	return thinkos_obj_alloc_lut[type];
}

#if THINKOS_ENABLE_JOIN || THINKOS_ENABLE_DEBUG_FAULT
bool __thinkos_thread_isalive(unsigned int th)
{
	bool dead;

	if (th >= THINKOS_THREADS_MAX)
		return false;

	if (thinkos_rt.ctx[th] == NULL)
		return false;

	dead = false;
#if THINKOS_ENABLE_JOIN
	dead |= __bit_mem_rd(&thinkos_rt.wq_canceled, th);
#endif
#if THINKOS_ENABLE_DEBUG_FAULT
	dead |= __bit_mem_rd(&thinkos_rt.wq_fault, th);
#endif

	return !dead;
}
#endif

#if THINKOS_ENABLE_PAUSE
bool __thinkos_thread_ispaused(unsigned int th)
{
	if (th >= THINKOS_THREADS_MAX)
		return false;

	if (thinkos_rt.ctx[th] == NULL)
		return false;

	return __bit_mem_rd(&thinkos_rt.wq_paused, th);
}
#endif

#if THINKOS_ENABLE_DEBUG_FAULT
bool __thinkos_thread_isfaulty(unsigned int th)
{
	if (th >= THINKOS_THREADS_MAX)
		return false;

	if (thinkos_rt.ctx[th] == NULL)
		return false;

	return __bit_mem_rd(&thinkos_rt.wq_fault, th);
}
#endif

bool __thinkos_suspended(void)
{
	unsigned int th;

	for (th = 0; th < THINKOS_THREADS_MAX; ++th) {
		if (thinkos_rt.ctx[th] != NULL) {
			bool suspended = false;
#if THINKOS_ENABLE_JOIN
			suspended |= __bit_mem_rd(&thinkos_rt.wq_canceled, th);
#endif
#if THINKOS_ENABLE_DEBUG_FAULT
			suspended |= __bit_mem_rd(&thinkos_rt.wq_fault, th);
#endif
#if THINKOS_ENABLE_PAUSE
			suspended |= __bit_mem_rd(&thinkos_rt.wq_paused, th);
#endif
			if (!suspended)
				return false;
		}
	}

	return true;
}

