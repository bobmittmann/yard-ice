/* 
 * File:	 usb-cdc.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2011 Bob Mittmann. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#define __THINKOS_DMON__
#include <thinkos_dmon.h>
#include <thinkos.h>
#include <sys/dcclog.h>

#if THINKOS_ENABLE_EXIT || THINKOS_ENABLE_JOIN
#define CYCCNT_MAX ((THINKOS_THREADS_MAX) + 2) /* extra slot for void thread */
#else
#define CYCCNT_MAX ((THINKOS_THREADS_MAX) + 1)
#endif

int dmon_print_osinfo(struct dmon_comm * comm)
{
	struct thinkos_rt * rt = &thinkos_rt;
#if THINKOS_ENABLE_PROFILING
	uint32_t cycbuf[CYCCNT_MAX];
	uint32_t cyccnt;
	int32_t delta;
	uint32_t cycdiv;
	uint32_t busy;
#endif
	int i;
	int j;

#if THINKOS_ENABLE_PROFILING
	cyccnt = CM3_DWT->cyccnt;
	delta = cyccnt - thinkos_rt.cycref;
	/* update the reference */
	thinkos_rt.cycref = cyccnt;
	/* update active thread's cycle counter */
	thinkos_rt.cyccnt[thinkos_rt.active] += delta; 
	/* copy the thread counters to a buffer */
	__thinkos_memcpy32(cycbuf, rt->cyccnt, sizeof(cycbuf));
	/* reset cycle counters */
	__thinkos_memset32(rt->cyccnt, 0, sizeof(cycbuf));
#endif

//	__thinkos_memcpy32(rt, &thinkos_rt, sizeof(struct thinkos_rt));

	dmprintf(comm, "\r\n------------------------------------------------"
			 "---------------------\r\n");
	dmprintf(comm, "[ Current = %d ]", rt->active);

#if THINKOS_ENABLE_CLOCK
	dmprintf(comm, "[ %d ticks ]", rt->ticks);
#endif

#if THINKOS_ENABLE_PROFILING
	dmprintf(comm, "[ cyccnt = %u ]\r\n", cyccnt);
	{
		uint32_t cycsum = 0;
		uint32_t cycbusy;
		uint32_t idle;

		cycsum = 0;
		for (i = 0; i < THINKOS_THREADS_MAX; ++i)
			cycsum += cycbuf[i];
		cycbusy = cycsum;
		cycsum += cycbuf[THINKOS_CYCCNT_IDLE];

		cycdiv = (cycsum + 500) / 1000;
		busy = (cycbusy + cycdiv / 2) / cycdiv;
		idle = 1000 - busy;
		dmprintf(comm, "[ %u cycles | %d.%d%% busy | %d.%d%% idle ]", 
				cycsum, busy / 10, busy % 10, idle / 10, idle % 10);
	}

#endif

	dmprintf(comm, "\r\n");

	dmprintf(comm, " Th"); 
#if THINKOS_ENABLE_THREAD_INFO
	dmprintf(comm, " |     Tag"); 
	dmprintf(comm, " |    Stack"); 
#endif
	dmprintf(comm, " |  Context"); 
#if THINKOS_ENABLE_THREAD_STAT
	dmprintf(comm, " |  WQ | TmW"); 
#endif
#if THINKOS_ENABLE_TIMESHARE
//	dmprintf(comm, " |  Val |  Pri"); 
#endif
#if THINKOS_ENABLE_CLOCK
	dmprintf(comm, " | Clock (ms)"); 
#endif
#if THINKOS_ENABLE_PROFILING
	dmprintf(comm, " | CPU %%"); 
#endif

#if THINKOS_MUTEX_MAX > 0
	dmprintf(comm, " | Locks\r\n"); 
#else
	dmprintf(comm, " |\r\n");
#endif

	for (i = 0; i < THINKOS_THREADS_MAX; ++i) {
		if (rt->ctx[i] != NULL) {
			dmprintf(comm, "%3d", i);
#if THINKOS_ENABLE_THREAD_INFO
			if (rt->th_inf[i] != NULL) {
				dmprintf(comm, " | %7s", rt->th_inf[i]->tag); 
				dmprintf(comm, " | %08x", (uint32_t)rt->th_inf[i]->stack_ptr); 
			} else {
				dmprintf(comm, " |     ..."); 
				dmprintf(comm, " |      ..."); 
			}
#endif
			dmprintf(comm, " | %08x", (uint32_t)rt->ctx[i]); 
#if THINKOS_ENABLE_THREAD_STAT
			dmprintf(comm, " | %3d | %s", rt->th_stat[i] >> 1, 
					rt->th_stat[i] & 1 ? "Yes" : " No"); 
#endif
#if THINKOS_ENABLE_TIMESHARE
//			dmprintf(comm, " | %4d | %4d", rt->sched_val[i], rt->sched_pri[i]); 
#endif
#if THINKOS_ENABLE_CLOCK
			{
				int32_t dt = (int32_t)(rt->clock[i] - rt->ticks);
				if (dt < 0)
					dmprintf(comm, " | <timedout>"); 
				else
					dmprintf(comm, " | %10d", dt); 
			}
#endif
#if THINKOS_ENABLE_PROFILING
			busy = (cycbuf[i] + cycdiv / 2) / cycdiv;
			dmprintf(comm, " | %3d.%d", busy / 10, busy % 10);
#endif
			dmprintf(comm, " |");
#if THINKOS_MUTEX_MAX > 0
			for (j = 0; j < THINKOS_MUTEX_MAX ; ++j) {
				if (rt->lock[j] == i)
					dmprintf(comm, " %d", j + THINKOS_MUTEX_BASE);
			}
#endif 
			dmprintf(comm, "\r\n");
		}
	}


	/* IDLE thread */
	dmprintf(comm, "%3d", i);
#if THINKOS_ENABLE_THREAD_INFO
	dmprintf(comm, " |  <IDLE>");
	dmprintf(comm, " | %08x", (uint32_t)*thinkos_idle_stack_ptr); 
#endif
	dmprintf(comm, " | %08x", (uint32_t)rt->idle_ctx); 
#if THINKOS_ENABLE_THREAD_STAT
	dmprintf(comm, " | ... | ...");
#endif
#if THINKOS_ENABLE_TIMESHARE
	//			dmprintf(comm, " | %4d | %4d", rt->sched_val[i], rt->sched_pri[i]); 
#endif
#if THINKOS_ENABLE_CLOCK
	dmprintf(comm, " |        ..."); 
#endif
#if THINKOS_ENABLE_PROFILING
	busy = (cycbuf[i] + cycdiv / 2) / cycdiv;
	dmprintf(comm, " | %3d.%d", busy / 10, busy % 10);
#endif
	dmprintf(comm, " |\r\n");

	for (j = 0; j < (rt->wq_end - rt->wq_lst); ++j) {
		uint32_t wq;
		int type;
		wq = rt->wq_lst[j];
#if (THINKOS_THREADS_MAX < 32) 
		if (wq & ~(1 << THINKOS_THREADS_MAX)) { 
#else
		if (wq) { 
#endif
			type = thinkos_obj_type_get(j);
			dmprintf(comm, "%3d %5s: {", j, thinkos_type_name_lut[type]);
			for (i = 0; i < THINKOS_THREADS_MAX; ++i) {
				if (wq & (1 << i)) 
					dmprintf(comm, " %d", i);
			}
			dmprintf(comm, " }");
#if THINKOS_MUTEX_MAX > 0
			if (type == THINKOS_OBJ_MUTEX)
				dmprintf(comm, " [lock=%d]", 
						 rt->lock[j - THINKOS_MUTEX_BASE]);
#endif 
			dmprintf(comm, "\r\n");
		}
	}

	return 0;
}

