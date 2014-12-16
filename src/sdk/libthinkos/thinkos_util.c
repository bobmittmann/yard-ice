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

const char obj_type_name[][8] = {
	"Ready",
	"Sched",
	"Cancl",
	"Pausd",
	"Clock",
	"Mutex",
	"Cond",
	"Sem",
	"Event",
	"Flag",
	"Join",
	"Inv"
};

static int bmp_bit_cnt(uint32_t bmp)
{
	int i;
	int cnt = 0;

	for (i = 0; i < 32; ++i)
		cnt += ((1 << i) & bmp) ? 1: 0;

	return cnt;
}

int thinkos_dump_rt(FILE * f, struct thinkos_rt * rt)
{
	uint32_t * wq;
	int i;
	int j;

#if THINKOS_ENABLE_CLOCK
	fprintf(f, "Ticks = %d\n", rt->ticks);
#endif

	fprintf(f, "  Th"); 
#if THINKOS_ENABLE_THREAD_INFO
	fprintf(f, " |     Tag"); 
#endif
	fprintf(f, " |    Context"); 
#if THINKOS_ENABLE_THREAD_STAT
	fprintf(f, " |  WQ | TmW"); 
#endif
#if THINKOS_ENABLE_TIMESHARE
	fprintf(f, " |  Val |  Pri"); 
#endif
#if THINKOS_ENABLE_CLOCK
	fprintf(f, " |     Clock"); 
#endif
#if THINKOS_MUTEX_MAX > 0
	fprintf(f, " | Locks\n"); 
#else
	fprintf(f, " |\n");
#endif

	for (i = 0; i < THINKOS_THREADS_MAX; ++i) {
		if (rt->ctx[i] != NULL) {
			fprintf(f, "%4d", i);
#if THINKOS_ENABLE_THREAD_INFO
			if (rt->th_inf[i] != NULL)
				fprintf(f, " | %7s", rt->th_inf[i]->tag); 
			else
				fprintf(f, " | %7s", "..."); 
#endif
			fprintf(f, " | 0x%08x", (uint32_t)rt->ctx[i]); 
#if THINKOS_ENABLE_THREAD_STAT
			fprintf(f, " | %3d | %s", rt->th_stat[i] >> 1, 
					rt->th_stat[i] & 1 ? "Yes" : " No"); 
#endif
#if THINKOS_ENABLE_TIMESHARE
			fprintf(f, " | %4d | %4d", rt->sched_val[i], sched_pri[i]); 
#endif
#if THINKOS_ENABLE_CLOCK
			fprintf(f, " | %9d", (int32_t)(rt->clock[i] - rt->ticks)); 
#endif
			fprintf(f, " |");
#if THINKOS_MUTEX_MAX > 0
			for (j = 0; j < THINKOS_MUTEX_MAX ; ++j) {
				if (rt->lock[j] == i)
					fprintf(f, " %d", j + THINKOS_MUTEX_BASE);
			}
#endif 
			fprintf(f, "\n");
		}
	}

	for (wq = rt->wq_lst; wq != rt->wq_end; ++wq) {
		int oid;
		int type;
		if (*wq) { 
			oid = wq - rt->wq_lst;
			type = thinkos_obj_type_get(oid);
			fprintf(f, "%3d %5s:", oid, obj_type_name[type]);
			for (i = 0; i < THINKOS_THREADS_MAX; ++i) {
				if (*wq & (1 << i)) 
					fprintf(f, " %d", i);
			}
#if THINKOS_MUTEX_MAX > 0
			if (type == THINKOS_OBJ_MUTEX)
				fprintf(f, " [lock=%d]", rt->lock[oid - THINKOS_MUTEX_BASE]);
#endif 
			fprintf(f, "\n");
		}
	}

	return 0;
}

void thinkos_alloc_dump(FILE * f, struct thinkos_rt * rt)
{
	fprintf(f, "     ");
#if THINKOS_ENABLE_MUTEX_ALLOC
	fprintf(f, "   Thread");
#endif
#if THINKOS_ENABLE_MUTEX_ALLOC
	fprintf(f, "    Mutex");
#endif
#if THINKOS_ENABLE_COND_ALLOC
	fprintf(f, "     Cond");
#endif
#if THINKOS_ENABLE_SEM_ALLOC
	fprintf(f, "  Semaphr");
#endif
#if THINKOS_ENABLE_EVENT_ALLOC
	fprintf(f, "    Event");
#endif
#if THINKOS_ENABLE_FLAG_ALLOC
	fprintf(f, "     Flag");
#endif
	fprintf(f, "\n");

	fprintf(f, " Bmp:");
#if THINKOS_ENABLE_THREAD_ALLOC
	fprintf(f, " %08x", rt->th_alloc);
#endif
#if THINKOS_ENABLE_MUTEX_ALLOC
	fprintf(f, " %08x", rt->mutex_alloc);
#endif
#if THINKOS_ENABLE_COND_ALLOC
	fprintf(f, " %08x", rt->cond_alloc);
#endif
#if THINKOS_ENABLE_SEM_ALLOC
	fprintf(f, " %08x", rt->sem_alloc);
#endif
#if THINKOS_ENABLE_EVENT_ALLOC
	fprintf(f, " %08x", rt->ev_alloc);
#endif
#if THINKOS_ENABLE_FLAG_ALLOC
	fprintf(f, " %08x", rt->flag_alloc);
#endif
	fprintf(f, "\n");

	fprintf(f, " Cnt:");
#if THINKOS_ENABLE_THREAD_ALLOC
	fprintf(f, "%6d/%-2d", bmp_bit_cnt(rt->th_alloc & 
		~(0xffffffff << THINKOS_THREADS_MAX)), THINKOS_THREADS_MAX);
#endif
#if THINKOS_ENABLE_MUTEX_ALLOC
	fprintf(f, "%6d/%-2d", bmp_bit_cnt(rt->mutex_alloc & 
		~(0xffffffff << THINKOS_MUTEX_MAX)), THINKOS_MUTEX_MAX);
#endif
#if THINKOS_ENABLE_COND_ALLOC
	fprintf(f, "%6d/%-2d", bmp_bit_cnt(rt->cond_alloc & 
		~(0xffffffff << THINKOS_COND_MAX)), THINKOS_COND_MAX);
#endif
#if THINKOS_ENABLE_SEM_ALLOC
	fprintf(f, "%6d/%-2d", bmp_bit_cnt(rt->sem_alloc & 
		~(0xffffffff << THINKOS_SEMAPHORE_MAX)), THINKOS_SEMAPHORE_MAX);
#endif
#if THINKOS_ENABLE_EVENT_ALLOC
	fprintf(f, "%6d/%-2d", bmp_bit_cnt(rt->event_alloc & 
		~(0xffffffff << THINKOS_EVENT_MAX)), THINKOS_EVENT_MAX);
#endif
#if THINKOS_ENABLE_FLAG_ALLOC
	fprintf(f, "%6d/%-2d", bmp_bit_cnt(rt->flag_alloc & 
		~(0xffffffff << THINKOS_COND_MAX)), THINKOS_COND_MAX);
#endif
	fprintf(f, "\n");
}

#endif










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




