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
 * @file cmd_osinfo.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/shell.h>
#include <thinkos.h>
#define __THINKOS_SYS__
#include <thinkos_sys.h>
#include <sys/dcclog.h>

#if THINKOS_ENABLE_THREAD_ALLOC | THINKOS_ENABLE_MUTEX_ALLOC | \
	THINKOS_ENABLE_COND_ALLOC | THINKOS_ENABLE_SEM_ALLOC | \
	THINKOS_ENABLE_EVENT_ALLOC | THINKOS_ENABLE_FLAG_ALLOC
static int bmp_bit_cnt(uint32_t bmp[], int bits)
{
	int cnt = 0;
	int j;
	int i;

	for (j = 0; j < (bits / 32); ++j)
		for (i = 0; i < 32; ++i)
			cnt += ((1 << i) & bmp[j]) ? 1: 0;

	for (i = 0; i < bits % 32; ++i)
		cnt += ((1 << i) & bmp[j]) ? 1: 0;

	return cnt;
}
#endif

static void os_alloc_dump(FILE * f, struct thinkos_rt * rt)
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
	fprintf(f, " %08x", rt->th_alloc[0]);
#endif
#if THINKOS_ENABLE_MUTEX_ALLOC
	fprintf(f, " %08x", rt->mutex_alloc[0]);
#endif
#if THINKOS_ENABLE_COND_ALLOC
	fprintf(f, " %08x", rt->cond_alloc[0]);
#endif
#if THINKOS_ENABLE_SEM_ALLOC
	fprintf(f, " %08x", rt->sem_alloc[0]);
#endif
#if THINKOS_ENABLE_EVENT_ALLOC
	fprintf(f, " %08x", rt->ev_alloc[0]);
#endif
#if THINKOS_ENABLE_FLAG_ALLOC
	fprintf(f, " %08x", rt->flag_alloc[0]);
#endif
	fprintf(f, "\n");

	fprintf(f, " Cnt:");
#if THINKOS_ENABLE_THREAD_ALLOC
	fprintf(f, "%6d/%-2d", bmp_bit_cnt(rt->th_alloc, THINKOS_THREADS_MAX), 
			THINKOS_THREADS_MAX);
#endif
#if THINKOS_ENABLE_MUTEX_ALLOC
	fprintf(f, "%6d/%-2d", bmp_bit_cnt(rt->mutex_alloc, THINKOS_MUTEX_MAX), 
			THINKOS_MUTEX_MAX);
#endif
#if THINKOS_ENABLE_COND_ALLOC
	fprintf(f, "%6d/%-2d", bmp_bit_cnt(rt->cond_alloc, THINKOS_COND_MAX),
			THINKOS_COND_MAX);
#endif
#if THINKOS_ENABLE_SEM_ALLOC
	fprintf(f, "%6d/%-2d", bmp_bit_cnt(rt->sem_alloc, THINKOS_SEMAPHORE_MAX), 
			THINKOS_SEMAPHORE_MAX);
#endif
#if THINKOS_ENABLE_EVENT_ALLOC
	fprintf(f, "%6d/%-2d", bmp_bit_cnt(rt->ev_alloc, THINKOS_EVENT_MAX),
			THINKOS_EVENT_MAX);
#endif
#if THINKOS_ENABLE_FLAG_ALLOC
	fprintf(f, "%6d/%-2d", bmp_bit_cnt(rt->flag_alloc, THINKOS_FLAG_MAX),
			THINKOS_FLAG_MAX);
#endif
	fprintf(f, "\n");

}

int cmd_osinfo(FILE * f, int argc, char ** argv)
{
	struct thinkos_rt rt;
	uint32_t * wq;
	int i;
#if THINKOS_ENABLE_PROFILING
	uint32_t cycdiv;
	uint32_t busy;
#endif
#if THINKOS_MUTEX_MAX > 0
	int j;
#endif

	if (argc > 1)
		return SHELL_ERR_EXTRA_ARGS;

	if (thinkos_rt_snapshot(&rt) < 0) {
		fprintf(f, "#ERROR: Thinkos RT_DEBUG not enabled!\n");
		return SHELL_ERR_GENERAL;
	}

#if THINKOS_ENABLE_CLOCK
	fprintf(f, "[ Ticks = %d ]", rt.ticks);
#endif

#if THINKOS_ENABLE_PROFILING
	{
		uint32_t cycsum = 0;
		uint32_t cycbusy;
		uint32_t idle;
//		uint32_t sys;

		cycsum = 0;
		for (i = 0; i < THINKOS_THREADS_MAX; ++i)
			cycsum += rt.cyccnt[i];
		cycbusy = cycsum;
		cycsum += rt.cyccnt[THINKOS_CYCCNT_IDLE];
//		cycsum += rt.cyccnt[THINKOS_CYCCNT_SYS];

		cycdiv = (cycsum + 500) / 1000;
		busy = (cycbusy + cycdiv / 2) / cycdiv;
#if 0
		sys = (rt.cyccnt[THINKOS_CYCCNT_SYS] + cycdiv / 2) / cycdiv;
		//	idle = (rt.cyccnt[THINKOS_CYCCNT_IDLE] + cycdiv / 2) / cycdiv;
		idle = 1000 - busy - sys;
		fprintf(f, " [ %u cycles | %d.%d%% busy | %d.%d%% sys | %d.%d%% idle ]", 
				cycsum, busy / 10, busy % 10, 
				sys / 10, sys % 10,
				idle / 10, idle % 10);
#else
		idle = 1000 - busy;
		fprintf(f, " [ %u cycles | %d.%d%% busy | %d.%d%% idle ]", 
				cycsum, busy / 10, busy % 10, idle / 10, idle % 10);
	}
#endif

#endif

	fprintf(f, "\n");

	fprintf(f, " Th"); 
#if THINKOS_ENABLE_THREAD_INFO
	fprintf(f, " |     Tag"); 
	fprintf(f, " |    Stack"); 
#endif
	fprintf(f, " |  Context"); 
#if THINKOS_ENABLE_THREAD_STAT
	fprintf(f, " |  WQ | TmW"); 
#endif
#if THINKOS_ENABLE_TIMESHARE
	fprintf(f, " |  Val |  Pri"); 
#endif
#if THINKOS_ENABLE_CLOCK
	fprintf(f, " | Clock (ms)"); 
#endif
#if THINKOS_ENABLE_PROFILING
	fprintf(f, " | CPU %%"); 
#endif

#if THINKOS_MUTEX_MAX > 0
	fprintf(f, " | Locks\n"); 
#else
	fprintf(f, " |\n");
#endif

	for (i = 0; i < THINKOS_THREADS_MAX; ++i) {
		if (rt.ctx[i] != NULL) {
			fprintf(f, "%3d", i);
#if THINKOS_ENABLE_THREAD_INFO
			if (rt.th_inf[i] != NULL) {
				fprintf(f, " | %7s", rt.th_inf[i]->tag); 
				fprintf(f, " | %08x", (uint32_t)rt.th_inf[i]->stack_ptr); 
			} else {
				fprintf(f, " |     ..."); 
				fprintf(f, " |      ..."); 
			}
#endif
			fprintf(f, " | %08x", (uint32_t)rt.ctx[i]); 
#if THINKOS_ENABLE_THREAD_STAT
			fprintf(f, " | %3d | %s", rt.th_stat[i] >> 1, 
					rt.th_stat[i] & 1 ? "Yes" : " No"); 
#endif
#if THINKOS_ENABLE_TIMESHARE
			fprintf(f, " | %4d | %4d", rt.sched_val[i], rt.sched_pri[i]); 
#endif
#if THINKOS_ENABLE_CLOCK
			{
				int32_t dt = (int32_t)(rt.clock[i] - rt.ticks);
				if (dt < 0)
					fprintf(f, " | <timedout>"); 
				else
					fprintf(f, " | %10d", dt); 
			}
#endif
#if THINKOS_ENABLE_PROFILING
			busy = (rt.cyccnt[i] + cycdiv / 2) / cycdiv;
			fprintf(f, " | %3d.%d", busy / 10, busy % 10);
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
			fprintf(f, "%3d %5s:", oid, thinkos_type_name_lut[type]);
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

	os_alloc_dump(f, &rt);

	return 0;
}

int cmd_thread(FILE * f, int argc, char ** argv)
{
	uint8_t lst[THINKOS_THREADS_MAX];
	struct thinkos_context * ctx;
	struct thinkos_rt rt;
	unsigned int th;
#if THINKOS_ENABLE_THREAD_STAT
	int oid;
	int type;
#endif
	int cnt;
#if THINKOS_MUTEX_MAX > 0
	int j;
#endif
	int i;

	thinkos_rt_snapshot(&rt);

	if (argc == 1) {
		// no arguments (dump the current thread)
		lst[0] = thinkos_thread_self();
		cnt = 1;
	} else {
		// read all arguments from command line
		cnt = (argc - 1);
		for (i = 0; i < cnt; ++i) {
			lst[i] = strtoul(argv[i + 1], NULL, 0);
		}
	}

	for (i = 0; i < cnt; ++i) {
		th = lst[i];
		if ((th >= THINKOS_THREADS_MAX) || (rt.ctx[th] == NULL)) {
			fprintf(f, "Thread %d is invalid!\n", th);
			return SHELL_ERR_ARG_INVALID;
		}

		fprintf(f, " - Id: %d", th); 

#if THINKOS_ENABLE_THREAD_INFO
		if (rt.th_inf[th] != NULL)
			fprintf(f, ", %s", rt.th_inf[th]->tag); 
		else
			fprintf(f, ", %s", "..."); 
#endif
		fprintf(f, "\n"); 

#if THINKOS_ENABLE_THREAD_STAT
		oid = rt.th_stat[th] >> 1;
		type = thinkos_obj_type_get(oid);

		fprintf(f, " - Waiting on queue: %3d %5s (time wait: %s)\n", 
				oid, thinkos_type_name_lut[type], rt.th_stat[th] & 1 ? "Yes" : " No"); 
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
	}

	return 0;
}

#if THINKOS_ENABLE_THREAD_INFO
static int scan_stack(uint32_t * ptr, unsigned int size)
{
	int i;

	for (i = 0; i < size / 4; ++i) {
		if (ptr[i] != 0xdeadbeef)
			break;
	}

	return i * 4;
}
#endif

int cmd_oscheck(FILE * f, int argc, char ** argv)
{
	struct thinkos_rt rt;
	int i;

	if (argc > 1)
		return SHELL_ERR_EXTRA_ARGS;

	if (thinkos_rt_snapshot(&rt) < 0) {
		fprintf(f, "#ERROR: Thinkos RT_DEBUG not enabled!\n");
		return SHELL_ERR_GENERAL;
	}

	fprintf(f, "\n");

	fprintf(f, " Th"); 
#if THINKOS_ENABLE_THREAD_INFO
	fprintf(f, " |     Tag"); 
	fprintf(f, " |    Stack"); 
#endif
	fprintf(f, " |  Context"); 
#if THINKOS_ENABLE_THREAD_INFO
	fprintf(f, " |   Size"); 
	fprintf(f, " |   Free"); 
#endif
	fprintf(f, "\n");

	for (i = 0; i < THINKOS_THREADS_MAX; ++i) {
		if (rt.ctx[i] != NULL) {
			fprintf(f, "%3d", i);
#if THINKOS_ENABLE_THREAD_INFO
			if (rt.th_inf[i] != NULL) {
				fprintf(f, " | %7s", rt.th_inf[i]->tag); 
				fprintf(f, " | %08x", (uint32_t)rt.th_inf[i]->stack_ptr); 
			} else {
				fprintf(f, " |     ..."); 
				fprintf(f, " |      ..."); 
			}
#endif
			fprintf(f, " | %08x", (uint32_t)rt.ctx[i]); 
#if THINKOS_ENABLE_THREAD_INFO
			if (rt.th_inf[i] != NULL) {
				fprintf(f, " | %6d", rt.th_inf[i]->stack_size); 
				fprintf(f, " | %6d", scan_stack(rt.th_inf[i]->stack_ptr, 
												rt.th_inf[i]->stack_size));
			} else {
				fprintf(f, " |    ..."); 
				fprintf(f, " |    ..."); 
			}
#endif
			fprintf(f, "\n");
		}
	}

	return 0;
}

