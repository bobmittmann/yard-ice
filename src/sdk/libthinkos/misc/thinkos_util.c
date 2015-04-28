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

int __thinkos_thread_getnext(int th)
{
	int idx;

	idx = (th < 0) ? 0 : th + 1;
	
	for (; idx < THINKOS_THREADS_MAX; ++idx) {
		if (thinkos_rt.ctx[idx] != NULL)
			return idx;
	}

	return -1;
}

void __thinkos_memcpy(void * __dst, void * __src,  unsigned int __len)
{
	uint8_t * dst = (uint8_t *)__dst;
	uint8_t * src = (uint8_t *)__src;
	int i;

	for (i = 0; i < __len; ++i)
		dst[i] = src[i];
}

void __thinkos_memset32(void * __dst, uint32_t __val, unsigned int __len)
{
	uint32_t * dst = (uint32_t *)__dst;
	int i;

	for (i = 0; i < (__len / 4); ++i)
		dst[i] = __val;
}


#if THINKOS_ENABLE_THREAD_ALLOC | THINKOS_ENABLE_MUTEX_ALLOC | \
	THINKOS_ENABLE_COND_ALLOC | THINKOS_ENABLE_SEM_ALLOC | \
	THINKOS_ENABLE_EVENT_ALLOC | THINKOS_ENABLE_FLAG_ALLOC
int __thinkos_bmp_alloc(uint32_t bmp[], int bits) 
{
	int i;
	int j;

	for (i = 0; i < ((bits + 31) / 32); ++i) {
		/* Look for an empty bit MSB first */
		if ((j = __clz(__rbit(~(bmp[i])))) < 32) {
			/* Mark as used */
			__bit_mem_wr(&bmp[i], j, 1);  
			return 32 * i + j;;
		}
	}
	return -1;
}
#endif

#if THINKOS_ENABLE_THREAD_ALLOC | THINKOS_ENABLE_MUTEX_ALLOC | \
	THINKOS_ENABLE_COND_ALLOC | THINKOS_ENABLE_SEM_ALLOC | \
	THINKOS_ENABLE_EVENT_ALLOC | THINKOS_ENABLE_FLAG_ALLOC
void __thinkos_bmp_init(uint32_t bmp[], int bits) 
{
	int i;
	for (i = 0; i < bits / 32; ++i)
		bmp[i] = 0;
	if (bits % 32)
		bmp[i] = 0xffffffff << (bits % 32);
}
#endif

const char thinkos_obj_type_name[][8] = {
	[THINKOS_OBJ_READY] = "Ready",
	[THINKOS_OBJ_TMSHARE] = "Sched",
	[THINKOS_OBJ_CLOCK] = "Clock",
	[THINKOS_OBJ_MUTEX] = "Mutex",
	[THINKOS_OBJ_COND] = "Cond",
	[THINKOS_OBJ_SEMAPHORE] = "Sem",
	[THINKOS_OBJ_EVENT] = "EvSet",
	[THINKOS_OBJ_FLAG] = "Flag",
	[THINKOS_OBJ_JOIN] = "Join",
	[THINKOS_OBJ_PAUSED] = "Pausd",
	[THINKOS_OBJ_CANCELED] = "Cancl",
	[THINKOS_OBJ_FAULT] = "Fault",
	[THINKOS_OBJ_INVALID] = "Inv"
};

