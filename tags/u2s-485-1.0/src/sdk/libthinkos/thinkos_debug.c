/* 
 * thikos_debug.c
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

enum {
	THINKOS_OBJ_WQ_READY = 0,
	THINKOS_OBJ_WQ_TMSHARE,
	THINKOS_OBJ_WQ_CANCELED,
	THINKOS_OBJ_WQ_PAUSED,
	THINKOS_OBJ_WQ_CLOCK,
	THINKOS_OBJ_MUTEX,
	THINKOS_OBJ_COND,
	THINKOS_OBJ_SEMAPHORE,
	THINKOS_OBJ_EVENT,
	THINKOS_OBJ_FLAG,
	THINKOS_OBJ_JOIN,
	THINKOS_OBJ_INVALID
};

static int thinkos_obj_type_get(int oid)
{
	if (oid == 0)
		return THINKOS_OBJ_WQ_READY;

#if THINKOS_ENABLE_TIMESHARE
	if (oid == THINKOS_WQ_TMSHARE)
		return THINKOS_OBJ_WQ_TMSHARE;
#endif

#if THINKOS_ENABLE_JOIN
	if (oid == THINKOS_WQ_CANCELED)
		return THINKOS_OBJ_WQ_CANCELED;
#endif

#if THINKOS_ENABLE_PAUSE
	if (oid == THINKOS_WQ_PAUSED)
		return THINKOS_OBJ_WQ_PAUSED;
#endif

#if THINKOS_ENABLE_CLOCK
	if (oid == THINKOS_WQ_CLOCK)
		return THINKOS_OBJ_WQ_CLOCK;
#endif

	if (oid >= THINKOS_WQ_LST_END)
		return THINKOS_OBJ_INVALID;

#if THINKOS_ENABLE_JOIN
	if (oid >= THINKOS_JOIN_BASE)
		return THINKOS_OBJ_JOIN;
#endif 

#if THINKOS_FLAG_MAX > 0
	if (oid >= THINKOS_FLAG_BASE)
		return THINKOS_OBJ_FLAG;
#endif 

#if THINKOS_EVENT_MAX > 0
	if (oid >= THINKOS_EVENT_BASE)
		return THINKOS_OBJ_EVENT;
#endif 

#if THINKOS_SEMAPHORE_MAX > 0
	if (oid >= THINKOS_SEM_BASE)
		return THINKOS_OBJ_SEMAPHORE;
#endif 

#if THINKOS_COND_MAX > 0
	if (oid >= THINKOS_COND_BASE)
		return THINKOS_OBJ_COND;
#endif 

#if THINKOS_MUTEX_MAX > 0
	if (oid >= THINKOS_MUTEX_BASE)
		return THINKOS_OBJ_MUTEX;
#endif 

	return THINKOS_OBJ_INVALID;
}

void thinkos_trace_rt(struct thinkos_rt * rt)
{
	uint32_t * wq;
	int i;

	for (wq = rt->wq_lst; wq != rt->wq_end; ++wq) {
		int oid;
		int type;
		if (*wq) { 
			oid = wq - rt->wq_lst;
			type = thinkos_obj_type_get(oid);
			switch (type) {
			case THINKOS_OBJ_WQ_READY:
				DCC_LOG2(LOG_TRACE, "READY %d: 0x%08x", oid, *wq);
				break;
			case THINKOS_OBJ_WQ_TMSHARE:
				DCC_LOG2(LOG_TRACE, "TMSHARE %d: 0x%08x", oid, *wq);
				break;
			case THINKOS_OBJ_WQ_CANCELED:
				DCC_LOG2(LOG_TRACE, "CANCELED %d: 0x%08x", oid, *wq);
				break;
			case THINKOS_OBJ_WQ_PAUSED:
				DCC_LOG2(LOG_TRACE, "PAUSED %d: 0x%08x", oid, *wq);
				break;
			case THINKOS_OBJ_WQ_CLOCK:
				DCC_LOG2(LOG_TRACE, "CLOCK %d: 0x%08x", oid, *wq);
				break;
			case THINKOS_OBJ_MUTEX:
				DCC_LOG2(LOG_TRACE, "MUTEX %d: 0x%08x", oid, *wq);
				break;
			case THINKOS_OBJ_COND:
				DCC_LOG2(LOG_TRACE, "COND %d: 0x%08x", oid, *wq);
				break;
			case THINKOS_OBJ_SEMAPHORE:
				DCC_LOG2(LOG_TRACE, "SEMAPHORE %d: 0x%08x", oid, *wq);
				break;
			case THINKOS_OBJ_EVENT:
				DCC_LOG2(LOG_TRACE, "EVENT %d: 0x%08x", oid, *wq);
				break;
			case THINKOS_OBJ_FLAG:
				DCC_LOG2(LOG_TRACE, "FLAG %d: 0x%08x", oid, *wq);
				break;
			case THINKOS_OBJ_JOIN:
				DCC_LOG2(LOG_TRACE, "JOIN %d: 0x%08x", oid, *wq);
				break;
			case THINKOS_OBJ_INVALID:
				DCC_LOG2(LOG_TRACE, "INVALID %d: 0x%08x", oid, *wq);
				break;
			}
		}
	}

#if THINKOS_ENABLE_CLOCK
	DCC_LOG1(LOG_TRACE, "Ticks = %d", rt->ticks);
#endif

	for (i = 0; i < THINKOS_THREADS_MAX; ++i) {
		struct thinkos_context * ctx;

		if ((ctx = rt->ctx[i]) != NULL) {
#if THINKOS_ENABLE_THREAD_STAT
			DCC_LOG5(LOG_TRACE, "%2d %3d sp=%08x lr=%08x pc=%08x", 
					 i, rt->th_stat[i] >> 1, (uint32_t)ctx, ctx->lr, ctx->pc);
#else
			DCC_LOG4(LOG_TRACE, "%2d sp=%08x lr=%08x pc=%08x", 
					 i, (uint32_t)ctx, ctx->lr, ctx->pc);
#endif
		}
	}


}

