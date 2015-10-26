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

void __thinkos_thread_abort(int thread_id)
{
	DCC_LOG1(LOG_INFO, "<%d> ....", thread_id); 

#if THINKOS_ENABLE_TIMESHARE
	{
		int j;

		for (j = 0; j < THINKOS_THREADS_MAX; ++j) {
			if (j == thread_id)
				continue;
			if (thinkos_rt.ctx[j] == NULL)
				continue;
			/* schedule limit reevaluation */
			if (thinkos_rt.sched_limit < thinkos_rt.sched_pri[j])
				thinkos_rt.sched_limit = thinkos_rt.sched_pri[j];
		}
	}
#endif

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

	if (thread_id == thinkos_rt.active) {
#if THINKOS_ENABLE_THREAD_VOID 
		DCC_LOG(LOG_INFO, "set active thread to void!"); 
		/* pretend we are somebody else */
		thinkos_rt.active = THINKOS_THREAD_VOID;
#else
		DCC_LOG(LOG_PANIC, "abort current thread won't clear context!"); 
#endif
	} else {
		DCC_LOG1(LOG_INFO, "active thread=%d", thinkos_rt.active); 
	}

	/* clear context. */
	thinkos_rt.ctx[thread_id] = NULL;

	__thinkos_suspend(thread_id);

	DCC_LOG1(LOG_INFO, "ready=%08x", thinkos_rt.wq_ready);

	/* signal the scheduler ... */
	__thinkos_defer_sched();
}

#if THINKOS_ENABLE_TERMINATE
/* Terminate the target thread */
void thinkos_terminate_svc(struct cm3_except_context * ctx)
{
	int thread_id = ctx->r1;
	int code = ctx->r0;

	(void)code;

#if THINKOS_ENABLE_ARG_CHECK
	if (thread_id >= THINKOS_THREADS_MAX) {
		DCC_LOG1(LOG_ERROR, "invalid thread %d!", thread_id);
		ctx->r0 = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_THREAD_ALLOC
	if (__bit_mem_rd(thinkos_rt.th_alloc, thread_id) == 0) {
		ctx->r0 = THINKOS_EINVAL;
		return;
	}
#endif
#endif
	DCC_LOG2(LOG_TRACE, "<%d> code=%d", thread_id, code); 

#if THINKOS_ENABLE_TIMESHARE
	/* possibly remove from the time share wait queue */
	__bit_mem_wr((uint32_t *)&thinkos_rt.wq_tmshare, thread_id, 0);  
#endif

#if THINKOS_ENABLE_CANCEL
#if THINKOS_ENABLE_THREAD_STAT
	{
		int stat;
		/* update the thread status */
		stat = thinkos_rt.th_stat[thread_id];
		DCC_LOG1(LOG_INFO, "wq=%d", stat >> 1); 
		thinkos_rt.th_stat[thread_id] = 0;
		/* remove from other wait queue, if any */
		__bit_mem_wr(&thinkos_rt.wq_lst[stat >> 1], thread_id, 0);  
	}
#else
	{
		int i;
		/* remove from other wait queues, if any */
		for (i = 0; i < __wq_idx(thinkos_rt.wq_end); ++i)
			__bit_mem_wr(&thinkos_rt.wq_lst[i], thread_id, 0);  
	}
#endif
#endif /* THINKOS_ENABLE_CANCEL */

#if THINKOS_ENABLE_JOIN
	{
		unsigned int wq = THINKOS_JOIN_BASE + thread_id;
		int th;

		if ((th = __thinkos_wq_head(wq)) != THINKOS_THREAD_NULL) {
			DCC_LOG2(LOG_INFO, "<%d> wakeup from join %d.", th, wq);
			/* wakeup from the join wait queue */
			__thinkos_wakeup(wq, th);
			thinkos_rt.ctx[th]->r0 = code;
			/* wakeup all remaining threads */
			while ((th = __thinkos_wq_head(wq)) != THINKOS_THREAD_NULL) {
				DCC_LOG2(LOG_INFO, "<%d> wakeup from join %d.", th, wq);
				__thinkos_wakeup(wq, th);
				thinkos_rt.ctx[th]->r0 = code;
			}
			/* signal the scheduler ... */
			__thinkos_defer_sched();
		}
	}
#endif

	__thinkos_thread_abort(thread_id);
}
#endif /* THINKOS_ENABLE_TERMINATE */

void __attribute__((noreturn)) __thinkos_thread_exit(int code)
{
	int self = thinkos_rt.active;

	thinkos_terminate(code, self);

	for(;;);
}

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
		/* remove from the ready wait queue */
		__thinkos_suspend(self);
		/* signal the scheduler ... */
		__thinkos_defer_sched();
	}
#endif /* THINKOS_ENABLE_JOIN */

	/* adjust PC to the exit continuation call */
	ctx->pc = (uint32_t)__thinkos_thread_exit;
}
#endif /* THINKOS_ENABLE_EXIT */

