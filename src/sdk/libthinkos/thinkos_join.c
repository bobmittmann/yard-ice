/* 
 * thikos_join.c
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

#if THINKOS_ENABLE_JOIN
void thinkos_join_svc(int32_t * arg)
{
	int self = thinkos_rt.active;
	unsigned int th = arg[0];
	unsigned int wq;

#if THINKOS_ENABLE_ARG_CHECK
	if (th >= THINKOS_THREADS_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not a thread!", th);
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

	/* remove thread from the canceled wait queue */
	if (__bit_mem_rd(&thinkos_rt.wq_canceled, th)) {
		__bit_mem_wr(&thinkos_rt.wq_canceled, th, 0);  
		__bit_mem_wr(&thinkos_rt.wq_ready, th, 1);  
	}

	/* insert the current thread (self) into the joining thread wait queue */
	wq = THINKOS_JOIN_BASE + th;
	__thinkos_wq_insert(wq, self);

	DCC_LOG2(LOG_TRACE, "<%d> waiting to join at %d.", self, wq);

	/* wait for event */
	__thinkos_suspend(self);
	/* signal the scheduler ... */
	__thinkos_defer_sched();

	/* set the return to ERROR as a default value. The
	   exit function of the joining thread will set this to the 
	   appropriate return code */
	arg[0] = -1;
}
#endif

