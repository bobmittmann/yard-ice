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

void dmon_print_alloc(struct dmon_comm * comm)
{
	struct thinkos_rt * rt = &thinkos_rt;

	dmprintf(comm, "     ");
#if THINKOS_ENABLE_MUTEX_ALLOC
	dmprintf(comm, "   Thread");
#endif
#if THINKOS_ENABLE_MUTEX_ALLOC
	dmprintf(comm, "    Mutex");
#endif
#if THINKOS_ENABLE_COND_ALLOC
	dmprintf(comm, "     Cond");
#endif
#if THINKOS_ENABLE_SEM_ALLOC
	dmprintf(comm, "  Semaphr");
#endif
#if THINKOS_ENABLE_EVENT_ALLOC
	dmprintf(comm, "    Event");
#endif
#if THINKOS_ENABLE_FLAG_ALLOC
	dmprintf(comm, "     Flag");
#endif
	dmprintf(comm, "\r\n");

	dmprintf(comm, " Cnt:");
#if THINKOS_ENABLE_THREAD_ALLOC
	dmprintf(comm, "%6d/%-2d", bmp_bit_cnt(rt->th_alloc, THINKOS_THREADS_MAX), 
			THINKOS_THREADS_MAX);
#endif
#if THINKOS_ENABLE_MUTEX_ALLOC
	dmprintf(comm, "%6d/%-2d", bmp_bit_cnt(rt->mutex_alloc, THINKOS_MUTEX_MAX), 
			THINKOS_MUTEX_MAX);
#endif
#if THINKOS_ENABLE_COND_ALLOC
	dmprintf(comm, "%6d/%-2d", bmp_bit_cnt(rt->cond_alloc, THINKOS_COND_MAX),
			THINKOS_COND_MAX);
#endif
#if THINKOS_ENABLE_SEM_ALLOC
	dmprintf(comm, "%6d/%-2d", bmp_bit_cnt(rt->sem_alloc, THINKOS_SEMAPHORE_MAX), 
			THINKOS_SEMAPHORE_MAX);
#endif
#if THINKOS_ENABLE_EVENT_ALLOC
	dmprintf(comm, "%6d/%-2d", bmp_bit_cnt(rt->ev_alloc, THINKOS_EVENT_MAX),
			THINKOS_EVENT_MAX);
#endif
#if THINKOS_ENABLE_FLAG_ALLOC
	dmprintf(comm, "%6d/%-2d", bmp_bit_cnt(rt->flag_alloc, THINKOS_FLAG_MAX),
			THINKOS_FLAG_MAX);
#endif
	dmprintf(comm, "\r\n");

}

#endif

