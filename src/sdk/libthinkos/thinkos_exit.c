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

	for (j = 0; j < THINKOS_THREADS_MAX; ++j) {
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
	{
		unsigned int wq = THINKOS_JOIN_BASE + self;
		int th;

		if ((th = __thinkos_wq_head(wq)) != THINKOS_THREAD_NULL) {
			DCC_LOG2(LOG_TRACE, "<%d> wakeup from join %d.", th, wq);
			/* wakeup from the join wait queue */
			__thinkos_wakeup(wq, th);
			thinkos_rt.ctx[th]->r0 = code;
			/* wakeup all remaining threads */
			while ((th = __thinkos_wq_head(wq)) != THINKOS_THREAD_NULL) {
				DCC_LOG2(LOG_TRACE, "<%d> wakeup from join %d.", th, wq);
				__thinkos_wakeup(wq, th);
				thinkos_rt.ctx[th]->r0 = code;
			}
			/* signal the scheduler ... */
			__thinkos_defer_sched();
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
	DCC_LOG2(LOG_WARNING, "<%d> code=%d", thinkos_rt.active, code); 
	/* pretend we are somebody else */
	thinkos_rt.active = THINKOS_THREAD_VOID;
	/* signal the scheduler ... */
	__thinkos_defer_sched();
}

#endif /* THINKOS_ENABLE_EXIT || THINKOS_ENABLE_JOIN */


#if THINKOS_ENABLE_EXIT
void thinkos_exit_svc(struct cm3_except_context * ctx)
{
	DCC_LOG2(LOG_TRACE, "<%d> exit with code %d!", 
			 thinkos_rt.active, ctx->r0); 

#if THINKOS_ENABLE_JOIN
	int self = thinkos_rt.active;

	if (thinkos_rt.wq_join[self] == 0) {
		DCC_LOG1(LOG_TRACE, "<%d> canceled...", self); 
		/* insert into the canceled wait queue and wait for a join call */ 
		__thinkos_wq_insert(THINKOS_WQ_CANCELED, self);
		cm3_cpsid_i();
		/* remove from the ready wait queue */
		__thinkos_suspend(self);
		cm3_cpsie_i();
	}
#endif /* THINKOS_ENABLE_JOIN */

	/* adjust PC to the exit continuation call */
	ctx->pc = (uint32_t)__thinkos_thread_exit;

	__thinkos_defer_sched();
}
#endif

