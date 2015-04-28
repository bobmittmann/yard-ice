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

int dmon_print_osinfo(struct dmon_comm * comm)
{
	struct thinkos_rt * rt = &thinkos_rt;
	uint32_t * wq;
	int i;
#if THINKOS_ENABLE_PROFILING
	uint32_t cyccnt;
	int32_t delta;
	uint32_t cycdiv;
	uint32_t busy;
#endif
#if THINKOS_MUTEX_MAX > 0
	int j;
#endif

#if THINKOS_ENABLE_PROFILING
	cyccnt = CM3_DWT->cyccnt;
	delta = cyccnt - thinkos_rt.cycref;
	/* update the reference */
	thinkos_rt.cycref = cyccnt;
	/* update active thread's cycle counter */
	thinkos_rt.cyccnt[thinkos_rt.active] += delta; 
#endif

	__thinkos_memcpy32(rt, &thinkos_rt, sizeof(struct thinkos_rt));

#if THINKOS_ENABLE_PROFILING
	/* Reset cycle counters */
	for (i = 0; i <= THINKOS_THREADS_MAX; ++i)
		thinkos_rt.cyccnt[i] = 0; 
#endif

	dmprintf(comm, "\r\n------------------------------------------------"
			 "---------------------\r\n");
#if THINKOS_ENABLE_CLOCK
	dmprintf(comm, "[ Ticks = %d ]", rt->ticks);
#endif

#if THINKOS_ENABLE_PROFILING
	{
		uint32_t cycsum = 0;
		uint32_t cycbusy;
		uint32_t idle;

		cycsum = 0;
		for (i = 0; i < THINKOS_THREADS_MAX; ++i)
			cycsum += rt->cyccnt[i];
		cycbusy = cycsum;
		cycsum += rt->cyccnt[THINKOS_CYCCNT_IDLE];

		cycdiv = (cycsum + 500) / 1000;
		busy = (cycbusy + cycdiv / 2) / cycdiv;
		idle = 1000 - busy;
		dmprintf(comm, " [ %u cycles | %d.%d%% busy | %d.%d%% idle ]", 
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
	dmprintf(comm, " |  Val |  Pri"); 
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
			dmprintf(comm, " | %4d | %4d", rt->sched_val[i], rt->sched_pri[i]); 
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
			busy = (rt->cyccnt[i] + cycdiv / 2) / cycdiv;
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

	for (wq = rt->wq_lst; wq != rt->wq_end; ++wq) {
		int oid;
		int type;
		if (*wq) { 
			oid = wq - rt->wq_lst;
			type = thinkos_obj_type_get(oid);
			dmprintf(comm, "%3d %5s:", oid, thinkos_type_name_lut[type]);
			for (i = 0; i < THINKOS_THREADS_MAX; ++i) {
				if (*wq & (1 << i)) 
					dmprintf(comm, " %d", i);
			}
#if THINKOS_MUTEX_MAX > 0
			if (type == THINKOS_OBJ_MUTEX)
				dmprintf(comm, " [lock=%d]", rt->lock[oid - THINKOS_MUTEX_BASE]);
#endif 
			dmprintf(comm, "\r\n");
		}
	}

	return 0;
}

