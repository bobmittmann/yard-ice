/* 
 * thikos_exit.c
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

#if THINKOS_ENABLE_EXIT || THINKOS_ENABLE_JOIN
void __thinkos_thread_abort(int thread_id)
{
	int j;

	DCC_LOG1(LOG_WARNING, "<%d> ....", thread_id); 

	for (j = 0; j < THINKOS_THREADS_MAX; j++) {
		if (j == thread_id)
			continue;
		if (thinkos_rt.ctx[j] == NULL)
			continue;
#if THINKOS_ENABLE_TIMESHARE
		/* schedule limit reevaluation */
		if (thinkos_rt.sched_limit < thinkos_rt.sched_pri[j])
			thinkos_rt.sched_limit = thinkos_rt.sched_pri[j];
#endif
	}

#if !THINKOS_ENABLE_THREAD_ALLOC && THINKOS_ENABLE_TIMESHARE
	/* clear the schedule priority. In case the thread allocation
	 is disabled, the schedule limit reevaluation may produce inconsistent
	 results ... */
	thinkos_rt.sched_pri[thread_id] = 0;
#endif

#if THINKOS_ENABLE_THREAD_ALLOC
	/* Releases the thread block */
	__bit_mem_wr(&thinkos_rt.th_alloc, thread_id, 0);
#endif

	/* clear context. */
	thinkos_rt.ctx[thread_id] = NULL;

	/* remove from the ready wait queue */
	__bit_mem_wr(&thinkos_rt.wq_ready, thread_id, 0);  
#if THINKOS_ENABLE_TIMESHARE
	/* if the ready queue is empty, collect
	 the threads from the CPU wait queue */
	__thinkos_tmshare();
#endif

	if (thread_id == thinkos_rt.active) {
		DCC_LOG(LOG_TRACE, "set active thread to void!"); 
		/* pretend we are somebody else */
		thinkos_rt.active = THINKOS_THREAD_VOID;
	}

	/* signal the scheduler ... */
	__thinkos_defer_sched();
}

void __attribute__((noreturn)) __thinkos_thread_exit(int code)
{
	int self = thinkos_rt.active;
	int j;

	DCC_LOG2(LOG_TRACE, "<%d> code=%d", self, code); 

	/* disable interrupts */
	cm3_cpsid_i();

#if THINKOS_ENABLE_TIMESHARE
	/* possibly remove from the time share wait queue */
	__bit_mem_wr(&thinkos_rt.wq_tmshare, self, 0);  
#endif

#if THINKOS_ENABLE_CANCEL
#if THINKOS_ENABLE_THREAD_STAT
	{
		int stat;
		/* update the thread status */
		stat = thinkos_rt.th_stat[self];
		DCC_LOG1(LOG_TRACE, "wq=%d", stat >> 1); 
		thinkos_rt.th_stat[self] = 0;
		/* remove from other wait queue, if any */
		__bit_mem_wr(&thinkos_rt.wq_lst[stat >> 1], self, 0);  
	}
#else
	{
		int i;
		/* remove from other wait queue, if any */
		for (i = 0; i < __wq_idx(thinkos_rt.wq_end); ++i)
			__bit_mem_wr(&thinkos_rt.wq_lst[i], self, 0);  
	}
#endif
#endif /* THINKOS_ENABLE_CANCEL */

#if THINKOS_ENABLE_JOIN
	for (j = 0; j < THINKOS_THREADS_MAX; j++) {
		if (thinkos_rt.ctx[j] == NULL)
			continue;
		if (__bit_mem_rd(&thinkos_rt.wq_join[self], j) != 0) {
			DCC_LOG1(LOG_TRACE, "wakeup <%d>", j);
			__bit_mem_wr((void *)&thinkos_rt.wq_ready, j, 1); 
			__bit_mem_wr((void *)&thinkos_rt.wq_join[self], j, 0);  
			thinkos_rt.ctx[j]->r0 = code;
		}
	}
#endif

	__thinkos_thread_abort(self);

	cm3_cpsie_i();

	for(;;);
}

#else

void __attribute__((noreturn)) __thinkos_thread_exit(int code)
{
	DCC_LOG2(LOG_WARNING, "<%d> code=%d", self, code); 
	for(;;);
}

#endif /* THINKOS_ENABLE_EXIT || THINKOS_ENABLE_JOIN */


#if THINKOS_ENABLE_EXIT
void thinkos_exit_svc(int32_t * arg)
{
	int self = thinkos_rt.active;
	int code = arg[0];

#if THINKOS_ENABLE_JOIN
	if (thinkos_rt.wq_join[self] == 0) {
		/* insert into the canceled wait queue and wait for a join call */ 
		__thinkos_wq_insert(THINKOS_WQ_CANCELED, self);
		/* remove from the ready wait queue */
		__bit_mem_wr(&thinkos_rt.wq_ready, self, 0);  
#if THINKOS_ENABLE_TIMESHARE
		/* if the ready queue is empty, collect
		   the threads from the CPU wait queue */
		__thinkos_tmshare();
#endif
	}
#endif /* THINKOS_ENABLE_JOIN */

	DCC_LOG2(LOG_TRACE, "<%d> exit with code %d!", self, code); 

	/* adjust PC */
	arg[6] = (uint32_t)__thinkos_thread_exit;
	/* set the return code at R0 */
	arg[0] = code;

	__thinkos_defer_sched();
}
#endif

