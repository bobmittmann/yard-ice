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
 * @file cmd_os.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <thinkos.h>
#define __THINKOS_SYS__
#include <thinkos_sys.h>

#if 0
static void print_wq(FILE * f, const char * name, uint32_t wq)
{
	int i;

	fprintf(f, "%s:", name);
	
	for (i = 0; i < THINKOS_THREADS_MAX; ++i) {
		if (wq & (1 << i)) 
			fprintf(f, " %d", i);
	}
	fprintf(f, "\n");
}

static void print_wq_lst(FILE * f, const char * name, uint32_t wq[], int len)
{
	int i;
	int j;

	fprintf(f, "%s:\n", name);
	
	for (j = 0; j < len; ++j) {
		if (wq[j] != 0) {
			fprintf(f, "        %2d:", j);
			for (i = 0; i < THINKOS_THREADS_MAX; ++i) {
				if (wq[j] & (1 << i)) 
					fprintf(f, " %d", i);
			}
			fprintf(f, "\n");
		}
	}
	fprintf(f, "\n");
}
#endif

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

#if THINKOS_ENABLE_JOIN
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

int cmd_os(FILE * f, int argc, char ** argv)
{
	struct thinkos_rt rt;
	uint32_t * wq;
	int i;
	int j;

	thinkos_rt_snapshot(&rt);
#if THINKOS_ENABLE_CLOCK
	fprintf(f, "Ticks = %d\n", rt.ticks);
#endif

	fprintf(f, "  Th |    Context"); 
#if THINKOS_ENABLE_THREAD_STAT
	fprintf(f, " |  WQ | TmW"); 
#endif
#if THINKOS_ENABLE_TIMESHARE
	fprintf(f, " |  Val |  Pri"); 
#endif
#if THINKOS_ENABLE_CLOCK
	fprintf(f, " |    Clock "); 
#endif
#if THINKOS_MUTEX_MAX > 0
	fprintf(f, " | Locks\n"); 
#else
	fprintf(f, " |\n");
#endif

	for (i = 0; i < THINKOS_THREADS_MAX; ++i) {
		if (rt.ctx[i] != NULL) {
			fprintf(f, "%4d | 0x%08x", i, (uint32_t)rt.ctx[i]); 
#if THINKOS_ENABLE_THREAD_STAT
			fprintf(f, " | %3d | %s", rt.th_stat[i] >> 1, 
					rt.th_stat[i] & 1 ? "Yes" : " No"); 
#endif
#if THINKOS_ENABLE_TIMESHARE
			fprintf(f, " | %4d | %4d", rt.sched_val[i], sched_pri[i]); 
#endif
#if THINKOS_ENABLE_CLOCK
			fprintf(f, " | %9d", (int32_t)(rt.clock[i] - rt.ticks)); 
#endif
			fprintf(f, " |");
#if THINKOS_MUTEX_MAX > 0
			for (j = 0; j < THINKOS_MUTEX_MAX ; ++j) {
				if (rt.lock[j] == i)
					fprintf(f, " %d", j + THINKOS_MUTEX_BASE);
			}
#endif 
			fprintf(f, "\n");
		}
	}

	for (wq = rt.wq_lst; wq != rt.wq_end; ++wq) {
		int oid;
		int type;
		if (*wq) { 
			oid = wq - rt.wq_lst;
			type = thinkos_obj_type_get(oid);
			fprintf(f, "%3d %5s:", oid, obj_type_name[type]);
			for (i = 0; i < THINKOS_THREADS_MAX; ++i) {
				if (*wq & (1 << i)) 
					fprintf(f, " %d", i);
			}
#if THINKOS_MUTEX_MAX > 0
			if (type == THINKOS_OBJ_MUTEX)
				fprintf(f, " [lock=%d]", rt.lock[oid - THINKOS_MUTEX_BASE]);
#endif 
			fprintf(f, "\n");
		}
	}

	return 0;
}

int cmd_thread(FILE * f, int argc, char ** argv)
{
	struct thinkos_context * ctx;
	struct thinkos_rt rt;
	unsigned int th;
	int oid;
	int type;
	int j;

	if (argc > 1) {
		th = strtoul(argv[1], NULL, 0);
		if (argc > 2)
			return -1;
	} else {
		th = thinkos_thread_self();
	}

	thinkos_rt_snapshot(&rt);

	if (th >= THINKOS_THREADS_MAX) {
		fprintf(f, "Thread %d is invalid!\n", th);
		return 0;
	}

	if (rt.ctx[th] == NULL) {
		fprintf(f, "Thread %d is invalid!\n", th);
		return 0;
	}

	oid = rt.th_stat[th] >> 1;
	type = thinkos_obj_type_get(oid);

	fprintf(f, " - Id: %d\n", th); 

#if THINKOS_ENABLE_THREAD_STAT
	fprintf(f, " - Waiting on queue: %3d %5s (time wait: %s)\n", 
			oid, obj_type_name[type], rt.th_stat[th] & 1 ? "Yes" : " No"); 
#endif

#if THINKOS_ENABLE_TIMESHARE
	fprintf(f, " - Scheduler: val=%d pri=%4d\n", 
			rt.sched_val[th], rt.sched_pri[th]); 
#endif
#if THINKOS_ENABLE_CLOCK
	fprintf(f, " - Clock: val=%d time=%d\n", rt.clock[th],
			(int32_t)(rt.clock[th] - rt.ticks)); 
#endif

#if THINKOS_MUTEX_MAX > 0
	fprintf(f, " - Mutex Locks: ");
	for (j = 0; j < THINKOS_MUTEX_MAX ; ++j) {
		if (rt.lock[j] == th)
			fprintf(f, " %d", j + THINKOS_MUTEX_BASE);
	}
	fprintf(f, "\n");
#endif 

	ctx = rt.ctx[th];

	fprintf(f, " - Context: 0x%08x\n", (uint32_t)ctx); 

	fprintf(f, "     r0=%08x  r1=%08x  r2=%08x  r3=%08x\n", 
			ctx->r0, ctx->r1, ctx->r2, ctx->r3);
	fprintf(f, "     r4=%08x  r5=%08x  r6=%08x  r7=%08x\n", 
			ctx->r4, ctx->r7, ctx->r6, ctx->r7);
	fprintf(f, "     r8=%08x  r9=%08x r10=%08x r11=%08x\n", 
			ctx->r8, ctx->r9, ctx->r10, ctx->r11);
	fprintf(f, "    r12=%08x  sp=%08x  lr=%08x  pc=%08x\n", 
			ctx->r12, (uint32_t)ctx, ctx->lr, ctx->pc);
	fprintf(f, "   xpsr=%08x\n", ctx->xpsr);

	fprintf(f, "\n");

	return 0;
}

#if 0
int __cmd_os(FILE * f, int argc, char ** argv)
{
	struct thinkos_rt rt;
	int i;

	thinkos_rt_snapshot(&rt);
#if THINKOS_ENABLE_CLOCK
	fprintf(f, "Ticks = %d\n", rt.ticks);
#endif

	fprintf(f, "  Th |    Context"); 
#if THINKOS_ENABLE_THREAD_STAT
	fprintf(f, " |  WQ | TmW"); 
#endif
#if THINKOS_ENABLE_TIMESHARE
	fprintf(f, " |  Val |  Pri"); 
#endif
#if THINKOS_ENABLE_CLOCK
	fprintf(f, " |    Clock "); 
#endif
	fprintf(f, " |\n");

	for (i = 0; i < THINKOS_THREADS_MAX; ++i) {
		if (rt.ctx[i] != NULL) {
			fprintf(f, "%4d | 0x%08x", i, (uint32_t)rt.ctx[i]); 
#if THINKOS_ENABLE_THREAD_STAT
			fprintf(f, " | %3d | %s", rt.th_stat[i] >> 1, 
					rt.th_stat[i] & 1 ? "Yes" : " No"); 
#endif
#if THINKOS_ENABLE_TIMESHARE
			fprintf(f, " | %4d | %4d", rt.sched_val[i], sched_pri[i]); 
#endif
#if THINKOS_ENABLE_CLOCK
			fprintf(f, " | %9d", (int32_t)(rt.clock[i] - rt.ticks)); 
#endif
			fprintf(f, " |\n");
		}
	}

	fprintf(f, "Idle | 0x%08x |\n", (uint32_t)rt.idle_ctx); 

	fprintf(f, "Active Thread: %d\n", rt.active); 
	
	print_wq(f, "Ready", rt.wq_ready);


#if THINKOS_ENABLE_TIMESHARE
	print_wq(f, "TmShare", rt.wq_tmshare);
#endif
#if THINKOS_ENABLE_JOIN
	print_wq(f, "Canceled", rt.wq_canceled);
#endif
#if THINKOS_ENABLE_PAUSE
	print_wq(f, "Paused", rt.wq_paused);
#endif
#if THINKOS_ENABLE_JOIN
	print_wq(f, "Join", rt.wq_join);
#endif 
#if THINKOS_ENABLE_CLOCK
	print_wq(f, "Clock", rt.wq_clock);
#endif
#if THINKOS_MUTEX_MAX > 0
	print_wq_lst(f, "Mutex", rt.wq_mutex, THINKOS_MUTEX_MAX);
	fprintf(f, " Locks:");
	for (i = 0; i < THINKOS_MUTEX_MAX; ++i) {
		if (rt.lock[i] >= 0) 
			fprintf(f, " (%d, %d)", i, rt.lock[i]);
	}
	fprintf(f, "\n");
#endif /* THINKOS_MUTEX_MAX > 0 */
#if THINKOS_COND_MAX > 0
	print_wq_lst(f, "Cond", rt.wq_cond, THINKOS_COND_MAX);
	fprintf(f, " Cond Mutex :");
	for (i = 0; i < THINKOS_COND_MAX; ++i) {
		if (rt.wq_cond[i]) 
			fprintf(f, " (%d, %d)", i, rt.cond_mutex[i]);
	}
	fprintf(f, "\n");
#endif /* THINKOS_COND_MAX > 0 */
#if THINKOS_SEMAPHORE_MAX > 0
	print_wq_lst(f, "Sem", rt.wq_sem, THINKOS_SEMAPHORE_MAX);
	fprintf(f, " Semaphores:");
	for (i = 0; i < THINKOS_SEMAPHORE_MAX; ++i) {
		if (rt.sem_val[i] >= 0) 
			fprintf(f, " (%d, %d)", i, rt.sem_val[i]);
	}
	fprintf(f, "\n");
#endif /* THINKOS_SEMAPHORE_MAX > 0 */
#if THINKOS_EVENT_MAX > 0
	print_wq_lst(f, "Event", rt.wq_event, THINKOS_EVENT_MAX);
#endif /* THINKOS_EVENT_MAX > 0 */
#if THINKOS_FLAG_MAX > 0
	print_wq_lst(f, "Flag", rt.wq_flag, THINKOS_FLAG_MAX);
#endif /* THINKOS_FLAG_MAX > 0 */

#if THINKOS_ENABLE_CLOCK
	print_wq(f, "Flag", rt.flag);
#endif

#if 0
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

	return 0;
}

#endif
