/* 
 * thikos.c
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
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */

#define __THINKOS_SYS__
#include <thinkos_sys.h>
#include <thinkos.h>

#if THINKOS_ENABLE_PAUSE

void thinkos_resume_svc(int32_t * arg)
{
	unsigned int th = arg[0];
	unsigned int wq;
	int stat;

#if THINKOS_ENABLE_ARG_CHECK
	if (th >= THINKOS_THREADS_MAX) {
		DCC_LOG1(LOG_ERROR, "invalid thread %d!", th);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_THREAD_ALLOC
	if (__bit_mem_rd(thinkos_rt.th_alloc, th) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid thread %d!", th);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	DCC_LOG1(LOG_TRACE, "thread=%d", th);

	arg[0] = 0;

	if (__bit_mem_rd(&thinkos_rt.wq_paused, th) == 0) {
		/* not paused */
		return;
	}

#if (THINKOS_ENABLE_THREAD_STAT == 0)
#error "thinkos_resume() depends on THINKOS_ENABLE_THREAD_STAT"	
#endif
	/* remove from the paused queue */
	__bit_mem_wr(&thinkos_rt.wq_paused, th, 0);  
	/* reinsert the thread into a waiting queue, including ready  */
	stat = thinkos_rt.th_stat[th];
	wq = stat >> 1;
	DCC_LOG2(LOG_TRACE, "stat=0x%02x wq=%d", stat, wq);
	__bit_mem_wr(&thinkos_rt.wq_lst[wq], th, 1);

#if THINKOS_ENABLE_CLOCK
	/* reenable the clock according to the thread status */
	__bit_mem_wr(&thinkos_rt.wq_clock, th, 1);
#endif
	__thinkos_defer_sched();
}

void thinkos_pause_svc(int32_t * arg)
{
	unsigned int th = arg[0];
	unsigned int wq;
	int stat;

#if THINKOS_ENABLE_ARG_CHECK
	if (th >= THINKOS_THREADS_MAX) {
		DCC_LOG1(LOG_ERROR, "invalid thread %d!", th);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_THREAD_ALLOC
	if (__bit_mem_rd(thinkos_rt.th_alloc, th) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid thread %d!", th);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

#if (THINKOS_ENABLE_THREAD_STAT == 0)
#error "thinkos_pause() depends on THINKOS_ENABLE_THREAD_STAT"	
#endif

	DCC_LOG1(LOG_TRACE, "thread=%d", th);

	arg[0] = 0;

	if (__bit_mem_rd(&thinkos_rt.wq_paused, th) != 0) {
		/* paused */
		return;
	}

	/* insert into the paused queue */
	__bit_mem_wr(&thinkos_rt.wq_paused, th, 1);

	/* remove the thread from a waiting queue, including ready  */
	stat = thinkos_rt.th_stat[th];
	wq = stat >> 1;
	DCC_LOG2(LOG_TRACE, "stat=0x%02x wq=%d", stat, wq);
	__bit_mem_wr(&thinkos_rt.wq_lst[wq], th, 0);

#if THINKOS_ENABLE_TIMESHARE
	/* possibly remove from the time share wait queue */
	__bit_mem_wr(&thinkos_rt.wq_tmshare, th, 0);
#endif

#if THINKOS_ENABLE_CLOCK
	/* disable the clock */
	__bit_mem_wr(&thinkos_rt.wq_clock, th, 0);
#endif

	__thinkos_defer_sched();
}

#endif /* THINKOS_ENABLE_PAUSE */

