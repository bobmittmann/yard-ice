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

void thinkos_rt_snapshot_svc(int32_t * arg)
{
	uint32_t * dst = (uint32_t *)arg[0];
	uint32_t pri = cm3_primask_get();
	uint32_t * src;
	int i;

	cm3_primask_set(1);

	src = (uint32_t *)&thinkos_rt;

	for (i = 0; i < (sizeof(struct thinkos_rt) / 4); ++i)
		dst[i] = src[i];

	cm3_primask_set(pri);
}

#if 0

static void dump_queue(FILE * f, char * s, uint32_t q)
{
	int i;

	fprintf(f, s);
	for (i = 0; i < THINKOS_THREADS_MAX; ++i) {
		if (q & (1 << i)) {
			fprintf(f, " %2d", i);
		}
	}
	fprintf(f, "\n");
}

static void dump_obj_queue(FILE * f, const char * fmt, int obj, uint32_t q)
{
	int i;

	fprintf(f, fmt, obj);
	for (i = 0; i < THINKOS_THREADS_MAX; ++i) {
		if (q & (1 << i)) {
			fprintf(f, " %2d", i);
		}
	}
	fprintf(f, "\n");
}

void thinkos_rt_dump(FILE * f, struct thinkos_rt * rt)
{
	int i;

	fprintf(f, "Context pointers:\n");

	for (i = 0; i < THINKOS_THREADS_MAX; ++i) {
		fprintf(f, "\t%2d: %p\n", i, rt->ctx[i]);
	}
	fprintf(f, "Idle context: %p\n", rt->idle_ctx);
	fprintf(f, "Active thread: %d\n", rt->active);


	dump_queue(f, "Ready threads:", rt->wq_ready);

#if THINKOS_ENABLE_TIMESHARE
	/* Threads waiting for time share cycle */
	dump_queue(f, "Time share wait:", rt->wq_tmshare);
#endif

#if THINKOS_ENABLE_JOIN
	/* canceled threads wait queue */
	dump_queue(f, "Canceled wait:", rt->wq_canceled);
#endif

#if THINKOS_ENABLE_PAUSE
	/* canceled threads wait queue */
	dump_queue(f, "Paused threads:", rt->wq_paused);
#endif

#if THINKOS_ENABLE_CLOCK
	/* canceled threads wait queue */
	dump_queue(f, "Clock wait threads:", rt->wq_clock);
#endif

#if THINKOS_MUTEX_MAX > 0
	for (i = 0; i < THINKOS_MUTEX_MAX; ++i) {
		if (rt->wq_mutex[i]) {
			dump_obj_queue(f, "Mutex %d wait threads:", i, rt->wq_mutex[i]);
		}
	}
#endif /* THINKOS_MUTEX_MAX > 0 */

#if THINKOS_COND_MAX > 0
	for (i = 0; i < THINKOS_COND_MAX; ++i) {
		if (rt->wq_cond[i]) {
			dump_obj_queue(f, "Conditional %d wait threads:", 
						   i, rt->wq_cond[i]);
		}
	}
#endif /* THINKOS_COND_MAX > 0 */

#if THINKOS_SEMAPHORE_MAX > 0
	for (i = 0; i < THINKOS_SEMAPHORE_MAX; ++i) {
		if (rt->wq_sem[i]) {
			dump_obj_queue(f, "Semaphore %d wait threads:", i, rt->wq_sem[i]);
		}
	}
#endif /* THINKOS_SEMAPHORE_MAX > 0 */

#if THINKOS_EVENT_MAX > 0
	for (i = 0; i < THINKOS_EVENT_MAX; ++i) {
		if (rt->wq_event[i]) {
			dump_obj_queue(f, "Event %d wait threads:", i, rt->wq_event[i]);
		}
	}
#endif /* THINKOS_EVENT_MAX > 0 */

#if THINKOS_FLAG_MAX > 0
	for (i = 0; i < THINKOS_FLAG_MAX; ++i) {
		if (rt->wq_flag[i]) {
			dump_obj_queue(f, "Flag %d wait threads:", i, rt->wq_flag[i]);
		}
	}
#endif /* THINKOS_FLAG_MAX > 0 */

#if THINKOS_ENABLE_JOIN
	for (i = 0; i < THINKOS_THREADS_MAX; ++i) {
		if (rt->wq_join[i]) {
			dump_obj_queue(f, "Join %d wait threads:", i, rt->wq_join[i]);
		}
	}
#endif /* THINKOS_ENABLE_JOIN */

#if 0
#if THINKOS_ENABLE_THREAD_STAT
	uint8_t th_stat[THINKOS_THREADS_MAX]; /* Per thread status */
#endif

#if THINKOS_ENABLE_TIMESHARE
	/* This fields are used for time sharing (round robin) schedule only */
	struct {
		int8_t sched_val[32]; /* Thread schedule value */
		int8_t sched_idle_val;
		uint8_t sched_pri[32]; /* Thread schedule priority */
		int8_t sched_idle_pri;
		uint32_t sched_limit; /* Limiting time for an active thread */
	};
#endif

#if THINKOS_ENABLE_CLOCK
	struct {
		uint32_t ticks;
		/* This fields are used for time wait (e.g. sleep()) */
		uint32_t clock[THINKOS_THREADS_MAX];
	};
#endif

#if THINKOS_MUTEX_MAX > 0
	int8_t lock[THINKOS_MUTEX_MAX];
#endif /* THINKOS_MUTEX_MAX > 0 */

#if THINKOS_COND_MAX > 0
	uint8_t cond_mutex[THINKOS_COND_MAX];
#endif /* THINKOS_COND_MAX > 0 */

#if THINKOS_SEMAPHORE_MAX > 0
	uint32_t sem_val[THINKOS_SEMAPHORE_MAX];
#endif /* THINKOS_SEMAPHORE_MAX > 0 */

#if THINKOS_FLAG_MAX > 0
	uint32_t flag; /* event flags */
#endif /* THINKOS_EVENT_MAX > 0 */

#if THINKOS_IRQ_MAX > 0
	int8_t irq_th[THINKOS_IRQ_MAX];
#endif /* THINKOS_IRQ_MAX */

#if THINKOS_ENABLE_THREAD_ALLOC
	uint32_t th_alloc;
#endif

#if THINKOS_ENABLE_MUTEX_ALLOC
	uint32_t mutex_alloc;
#endif

#if THINKOS_ENABLE_COND_ALLOC
	uint32_t cond_alloc;
#endif

#if THINKOS_ENABLE_SEM_ALLOC
	uint32_t sem_alloc;
#endif

#if THINKOS_ENABLE_EVENT_ALLOC
	uint32_t ev_alloc;
#endif

#if THINKOS_ENABLE_FLAG_ALLOC
	uint32_t flag_alloc;
#endif

#if THINKOS_ENABLE_SCHED_DEBUG
	uint32_t sched_trace_req;
#endif

#endif
}

#endif


#if 0
static inline void __attribute__((always_inline)) 
__dump_context(struct thinkos_context * __ctx) {
	DCC_LOG4(LOG_TRACE, "  r0=%08x  r1=%08x  r2=%08x  r3=%08x", 
			__ctx->r0, __ctx->r1, __ctx->r2, __ctx->r3);
	DCC_LOG4(LOG_TRACE, "  r4=%08x  r5=%08x  r6=%08x  r7=%08x", 
			__ctx->r4, __ctx->r7, __ctx->r6, __ctx->r7);
	DCC_LOG4(LOG_TRACE, "  r8=%08x  r9=%08x r10=%08x r11=%08x", 
			__ctx->r8, __ctx->r9, __ctx->r10, __ctx->r11);
	DCC_LOG4(LOG_TRACE, " r12=%08x  sp=%08x  lr=%08x  pc=%08x", 
			__ctx->r12, __ctx, __ctx->lr, __ctx->pc);
	DCC_LOG2(LOG_TRACE, "xpsr=%08x ret=%08x ", 
			 __ctx->xpsr, __ctx->ret);
}

#endif

#if 0
void thinkos_rt_trace(struct thinkos_rt * rt)
{
	int i;

	for (i = 0; i < THINKOS_THREADS_MAX; ++i) {
		if (rt->ctx[i] != NULL) {
			DCC_LOG2(LOG_TRACE, "Context %2d: %p\n", i, rt->ctx[i]);
		}
	}
	DCC_LOG1(LOG_TRACE, "Idle context: %p\n", rt->idle_ctx);
	DCC_LOG1(LOG_TRACE, "Active thread: %d\n", rt->active);

	dump_queue(f, "Ready threads:", rt->wq_ready);

#if THINKOS_ENABLE_TIMESHARE
	/* Threads waiting for time share cycle */
	dump_queue(f, "Time share wait:", rt->wq_tmshare);
#endif

#if THINKOS_ENABLE_JOIN
	/* canceled threads wait queue */
	dump_queue(f, "Canceled wait:", rt->wq_canceled);
#endif

#if THINKOS_ENABLE_PAUSE
	/* canceled threads wait queue */
	dump_queue(f, "Paused threads:", rt->wq_paused);
#endif

#if THINKOS_ENABLE_CLOCK
	/* canceled threads wait queue */
	dump_queue(f, "Clock wait threads:", rt->wq_clock);
#endif

#if THINKOS_MUTEX_MAX > 0
	for (i = 0; i < THINKOS_MUTEX_MAX; ++i) {
		if (rt->wq_mutex[i]) {
			dump_obj_queue(f, "Mutex %d wait threads:", i, rt->wq_mutex[i]);
		}
	}
#endif /* THINKOS_MUTEX_MAX > 0 */

#if THINKOS_COND_MAX > 0
	for (i = 0; i < THINKOS_COND_MAX; ++i) {
		if (rt->wq_cond[i]) {
			dump_obj_queue(f, "Conditional %d wait threads:", 
						   i, rt->wq_cond[i]);
		}
	}
#endif /* THINKOS_COND_MAX > 0 */

#if THINKOS_SEMAPHORE_MAX > 0
	for (i = 0; i < THINKOS_SEMAPHORE_MAX; ++i) {
		if (rt->wq_sem[i]) {
			dump_obj_queue(f, "Semaphore %d wait threads:", i, rt->wq_sem[i]);
		}
	}
#endif /* THINKOS_SEMAPHORE_MAX > 0 */

#if THINKOS_EVENT_MAX > 0
	for (i = 0; i < THINKOS_EVENT_MAX; ++i) {
		if (rt->wq_event[i]) {
			dump_obj_queue(f, "Event %d wait threads:", i, rt->wq_event[i]);
		}
	}
#endif /* THINKOS_EVENT_MAX > 0 */

#if THINKOS_FLAG_MAX > 0
	for (i = 0; i < THINKOS_FLAG_MAX; ++i) {
		if (rt->wq_flag[i]) {
			dump_obj_queue(f, "Flag %d wait threads:", i, rt->wq_flag[i]);
		}
	}
#endif /* THINKOS_FLAG_MAX > 0 */

#if THINKOS_ENABLE_JOIN
	for (i = 0; i < THINKOS_THREADS_MAX; ++i) {
		if (rt->wq_join[i]) {
			dump_obj_queue(f, "Join %d wait threads:", i, rt->wq_join[i]);
		}
	}
#endif /* THINKOS_ENABLE_JOIN */

#if THINKOS_ENABLE_THREAD_STAT
	uint8_t th_stat[THINKOS_THREADS_MAX]; /* Per thread status */
#endif

#if THINKOS_ENABLE_TIMESHARE
	/* This fields are used for time sharing (round robin) schedule only */
	struct {
		int8_t sched_val[32]; /* Thread schedule value */
		int8_t sched_idle_val;
		uint8_t sched_pri[32]; /* Thread schedule priority */
		int8_t sched_idle_pri;
		uint32_t sched_limit; /* Limiting time for an active thread */
	};
#endif

#if THINKOS_ENABLE_CLOCK
	struct {
		uint32_t ticks;
		/* This fields are used for time wait (e.g. sleep()) */
		uint32_t clock[THINKOS_THREADS_MAX];
	};
#endif

#if THINKOS_MUTEX_MAX > 0
	int8_t lock[THINKOS_MUTEX_MAX];
#endif /* THINKOS_MUTEX_MAX > 0 */

#if THINKOS_COND_MAX > 0
	uint8_t cond_mutex[THINKOS_COND_MAX];
#endif /* THINKOS_COND_MAX > 0 */

#if THINKOS_SEMAPHORE_MAX > 0
	uint32_t sem_val[THINKOS_SEMAPHORE_MAX];
#endif /* THINKOS_SEMAPHORE_MAX > 0 */

#if THINKOS_FLAG_MAX > 0
	uint32_t flag; /* event flags */
#endif /* THINKOS_EVENT_MAX > 0 */

#if THINKOS_IRQ_MAX > 0
	int8_t irq_th[THINKOS_IRQ_MAX];
#endif /* THINKOS_IRQ_MAX */

#if THINKOS_ENABLE_THREAD_ALLOC
	uint32_t th_alloc;
#endif

#if THINKOS_ENABLE_MUTEX_ALLOC
	uint32_t mutex_alloc;
#endif

#if THINKOS_ENABLE_COND_ALLOC
	uint32_t cond_alloc;
#endif

#if THINKOS_ENABLE_SEM_ALLOC
	uint32_t sem_alloc;
#endif

#if THINKOS_ENABLE_EVENT_ALLOC
	uint32_t ev_alloc;
#endif

#if THINKOS_ENABLE_FLAG_ALLOC
	uint32_t flag_alloc;
#endif

#if THINKOS_ENABLE_SCHED_DEBUG
	uint32_t sched_trace_req;
#endif

}
#endif

#if 0
static inline void __attribute__((always_inline)) 
__dump_context(struct thinkos_context * __ctx) {
	DCC_LOG4(LOG_TRACE, "  r0=%08x  r1=%08x  r2=%08x  r3=%08x", 
			__ctx->r0, __ctx->r1, __ctx->r2, __ctx->r3);
	DCC_LOG4(LOG_TRACE, "  r4=%08x  r5=%08x  r6=%08x  r7=%08x", 
			__ctx->r4, __ctx->r7, __ctx->r6, __ctx->r7);
	DCC_LOG4(LOG_TRACE, "  r8=%08x  r9=%08x r10=%08x r11=%08x", 
			__ctx->r8, __ctx->r9, __ctx->r10, __ctx->r11);
	DCC_LOG4(LOG_TRACE, " r12=%08x  sp=%08x  lr=%08x  pc=%08x", 
			__ctx->r12, __ctx, __ctx->lr, __ctx->pc);
	DCC_LOG2(LOG_TRACE, "xpsr=%08x ret=%08x ", 
			 __ctx->xpsr, __ctx->ret);
}

#endif




