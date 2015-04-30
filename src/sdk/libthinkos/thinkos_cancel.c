/* 
 * thikos_cancel.c
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
#include <sys/delay.h>

#if THINKOS_ENABLE_CANCEL
void thinkos_cancel_svc(int32_t * arg)
{
	unsigned int th = arg[0];
	int code = arg[1];
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
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

#if (THINKOS_ENABLE_THREAD_STAT == 0)
#error "thinkos_cancel() depends on THINKOS_ENABLE_THREAD_STAT"	
#endif
	stat = thinkos_rt.th_stat[th];
	/* remove from other wait queue including wq_ready */
	__bit_mem_wr(&thinkos_rt.wq_lst[stat >> 1], th, 0);

#if THINKOS_ENABLE_JOIN
	/* insert into the canceled wait queue and wait for a join call */ 
	wq = __wq_idx(&thinkos_rt.wq_canceled);
#else /* THINKOS_ENABLE_JOIN */
	/* if join is not enabled insert into the ready queue */
	wq = __wq_idx(&thinkos_rt.wq_ready);
#endif /* THINKOS_ENABLE_JOIN */

	__thinkos_wq_insert(wq, th);

#if THINKOS_ENABLE_TIMESHARE
	/* possibly remove from the time share wait queue */
	__bit_mem_wr(&thinkos_rt.wq_tmshare, th, 0); 
#endif

#if THINKOS_ENABLE_CLOCK
	/* possibly remove from the time wait queue */
	__bit_mem_wr(&thinkos_rt.wq_clock, th, 0);  
#endif

	DCC_LOG3(LOG_TRACE, "<%d> cancel %d, with code %d!", 
			 thinkos_rt.active, th, code); 

	thinkos_rt.ctx[th]->pc = (uint32_t)__thinkos_thread_exit;
	thinkos_rt.ctx[th]->r0 = code;
	arg[0] = 0;
}
#endif


