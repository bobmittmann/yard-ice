/* 
 * thikos_flag.c
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
 * but WITHOUT ANY WARRANTY; without flagen the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */

_Pragma ("GCC optimize (\"Ofast\")")

#define __THINKOS_SYS__
#include <thinkos_sys.h>
#include <thinkos.h>

#if THINKOS_FLAG_MAX > 0

#if THINKOS_ENABLE_FLAG_ALLOC

void thinkos_flag_alloc_svc(int32_t * arg)
{
	int idx;

	if ((idx = __thinkos_bmp_alloc(thinkos_rt.flag_alloc, 
								   THINKOS_FLAG_MAX)) >= 0) {
		__bit_mem_wr(thinkos_rt.flag, idx, 0);
		arg[0] = idx + THINKOS_FLAG_BASE;
		DCC_LOG1(LOG_TRACE, "wq=%d", arg[0]);
	} else {
		arg[0] = idx;
	}
}

void thinkos_flag_free_svc(int32_t * arg)
{
	unsigned int wq = arg[0];

#if THINKOS_ENABLE_ARG_CHECK
	unsigned int idx = wq - THINKOS_FLAG_BASE;

	if (idx >= THINKOS_FLAG_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not a xdmon!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
	__bit_mem_wr(thinkos_rt.flag_alloc, wq - THINKOS_FLAG_BASE, 0);
}

#endif


/* --------------------------------------------------------------------------
 * Flag give/take family 
 * -------------------------------------------------------------------------- */

void thinkos_flag_take_svc(int32_t * arg)
{
	unsigned int wq = arg[0];
	int self = thinkos_rt.active;
	unsigned int idx = wq - THINKOS_FLAG_BASE;

#if THINKOS_ENABLE_ARG_CHECK
	if (idx >= THINKOS_FLAG_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not a flag!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_FLAG_ALLOC
	if (__bit_mem_rd(thinkos_rt.flag_alloc, idx) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid flag %d!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	if (__bit_mem_rd(thinkos_rt.flag, idx)) {
		/* clear the signal */
		__bit_mem_wr(thinkos_rt.flag, idx, 0);
		arg[0] = 0;
		return;
	} 

	/* -- wait for event ---------------------------------------- */
	DCC_LOG2(LOG_INFO, "<%d> waiting for flag %d...", self, wq);
	/* insert into the wait queue */
	__thinkos_wq_insert(wq, self);
	/* remove from the ready wait queue */
	__thinkos_suspend(self);
	/* XXX: save the context pointer */
	thinkos_rt.ctx[self] = (struct thinkos_context *)&arg[-8];
	__thinkos_defer_sched(); /* signal the scheduler ... */
}

#if THINKOS_ENABLE_TIMED_CALLS
void thinkos_flag_timedtake_svc(int32_t * arg)
{
	unsigned int wq = arg[0];
	uint32_t ms = (uint32_t)arg[1];
	int self = thinkos_rt.active;
	unsigned int idx = wq - THINKOS_FLAG_BASE;

#if THINKOS_ENABLE_ARG_CHECK
	if (idx >= THINKOS_FLAG_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not a flag!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_FLAG_ALLOC
	if (__bit_mem_rd(thinkos_rt.flag_alloc, idx) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid flag %d!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	if (__bit_mem_rd(thinkos_rt.flag, idx)) {
		/* clear the signal */
		__bit_mem_wr(thinkos_rt.flag, idx, 0);
		arg[0] = 0;
		return;
	} 

	/* -- wait for event ---------------------------------------- */
	DCC_LOG2(LOG_INFO, "<%d> waiting for flag %d...", self, wq);
	/* Set the default return value to timeout. The
	   flag_rise() call will change it to 0 */
	arg[0] = THINKOS_ETIMEDOUT;
	/* insert into the flag wait queue */
	__thinkos_tmdwq_insert(wq, self, ms);
	/* remove from the ready wait queue */
	__thinkos_suspend(self);
	/* XXX: save the context pointer */
	thinkos_rt.ctx[self] = (struct thinkos_context *)&arg[-8];
	__thinkos_defer_sched(); /* signal the scheduler ... */
}
#endif


void __thinkos_flag_give_i(uint32_t wq)
{
	unsigned int flag = wq - THINKOS_FLAG_BASE;
	int th;

	/* flag_give(): wakeup a single thread waiting on the flag 
	   OR set the flag */
	/* get the flag state */
	if (__bit_mem_rd(thinkos_rt.flag, flag) == 0) {
		/* get a thread from the queue */
		if ((th = __thinkos_wq_head(wq)) != THINKOS_THREAD_NULL) {
			DCC_LOG2(LOG_MSG, "<%d> waked up with flag %d", th, wq);
			__thinkos_wakeup(wq, th);
			/* signal the scheduler ... */
			__thinkos_defer_sched();
		} else {
			/* set the flag bit */
			__bit_mem_wr(thinkos_rt.flag, flag, 1);  
		}
	}
}

void thinkos_flag_give_svc(int32_t * arg)
{
	unsigned int wq = arg[0];
	unsigned int idx = wq - THINKOS_FLAG_BASE;

#if THINKOS_ENABLE_ARG_CHECK
	if (idx >= THINKOS_FLAG_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not a flag!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_FLAG_ALLOC
	if (__bit_mem_rd(thinkos_rt.flag_alloc, idx) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid flag %d!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	arg[0] = 0;

	__thinkos_flag_give_i(wq);
}



/* --------------------------------------------------------------------------
 * Flag watch family 
 * -------------------------------------------------------------------------- */

#if THINKOS_ENABLE_FLAG_WATCH

void thinkos_flag_val_svc(int32_t * arg)
{
	unsigned int wq = arg[0];
	unsigned int idx = wq - THINKOS_FLAG_BASE;

#if THINKOS_ENABLE_ARG_CHECK
	if (idx >= THINKOS_FLAG_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not a flag!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_FLAG_ALLOC
	if (__bit_mem_rd(thinkos_rt.flag_alloc, idx) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid flag %d!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif
	arg[0] = __bit_mem_rd(thinkos_rt.flag, idx);
}

void __thinkos_flag_clr(uint32_t wq)
{
	unsigned int flag = wq - THINKOS_FLAG_BASE;

	/* clear the flag signal bit */
	__bit_mem_wr(thinkos_rt.flag, flag, 0);  
}

void thinkos_flag_clr_svc(int32_t * arg)
{
	unsigned int wq = arg[0];
	unsigned int idx = wq - THINKOS_FLAG_BASE;

#if THINKOS_ENABLE_ARG_CHECK
	if (idx >= THINKOS_FLAG_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not a flag!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_FLAG_ALLOC
	if (__bit_mem_rd(thinkos_rt.flag_alloc, idx) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid flag %d!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif
	arg[0] = 0;
	__thinkos_flag_clr(wq);
}

void __thinkos_flag_set(uint32_t wq)
{
	unsigned int flag = wq - THINKOS_FLAG_BASE;
	int th;

	DCC_LOG1(LOG_INFO, "wq=%d...", wq);
	/* set the flag and wakeup all threads waiting on the flag */

	/* set the flag bit */
	__bit_mem_wr(thinkos_rt.flag, flag, 1);  

	/* get a thread from the queue */
	if ((th = __thinkos_wq_head(wq)) != THINKOS_THREAD_NULL) {
		__thinkos_wakeup(wq, th);
		/* get the remaining threads from the queue */
		while ((th = __thinkos_wq_head(wq)) != 
			   THINKOS_THREAD_NULL) {
			__thinkos_wakeup(wq, th);
		}
		/* signal the scheduler ... */
		__thinkos_defer_sched();
	}
}

void thinkos_flag_set_svc(int32_t * arg)
{
	unsigned int wq = arg[0];
	unsigned int idx = wq - THINKOS_FLAG_BASE;

#if THINKOS_ENABLE_ARG_CHECK
	if (idx >= THINKOS_FLAG_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not a flag!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_FLAG_ALLOC
	if (__bit_mem_rd(thinkos_rt.flag_alloc, idx) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid flag %d!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif
	arg[0] = 0;

	__thinkos_flag_set(wq);
}

void thinkos_flag_watch_svc(int32_t * arg)
{
	unsigned int wq = arg[0];
	int self = thinkos_rt.active;
	unsigned int idx = wq - THINKOS_FLAG_BASE;

#if THINKOS_ENABLE_ARG_CHECK
	if (idx >= THINKOS_FLAG_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not a flag!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_FLAG_ALLOC
	if (__bit_mem_rd(thinkos_rt.flag_alloc, idx) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid flag %d!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif
	/* set the return value */
	arg[0] = 0;
	/* flag is set just return */
	if (__bit_mem_rd(thinkos_rt.flag, idx))
		return;
	/* -- wait for event ---------------------------------------- */
	DCC_LOG2(LOG_INFO, "<%d> waiting for flag %d...", self, wq);
	/* insert into the wait queue */
	__thinkos_wq_insert(wq, self);
	/* remove from the ready wait queue */
	__thinkos_suspend(self);
	/* XXX: save the context pointer */
	thinkos_rt.ctx[self] = (struct thinkos_context *)&arg[-8];
	__thinkos_defer_sched(); /* signal the scheduler ... */
}


#if THINKOS_ENABLE_TIMED_CALLS
void thinkos_flag_timedwatch_svc(int32_t * arg)
{
	unsigned int wq = arg[0];
	uint32_t ms = (uint32_t)arg[1];
	int self = thinkos_rt.active;
	unsigned int idx = wq - THINKOS_FLAG_BASE;

#if THINKOS_ENABLE_ARG_CHECK
	if (idx >= THINKOS_FLAG_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not a flag!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_FLAG_ALLOC
	if (__bit_mem_rd(thinkos_rt.flag_alloc, idx) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid flag %d!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	arg[0] = 0;
	if (__bit_mem_rd(thinkos_rt.flag, idx))
		return;

	/* -- wait for event ---------------------------------------- */
	DCC_LOG2(LOG_INFO, "<%d> waiting for flag %d...", self, wq);
	/* Set the default return value to timeout. The
	   flag_rise() call will change it to 0 */
	arg[0] = THINKOS_ETIMEDOUT;
	/* insert into the flag wait queue */
	__thinkos_tmdwq_insert(wq, self, ms);
	/* remove from the ready wait queue */
	__thinkos_suspend(self);
	/* XXX: save the context pointer */
	thinkos_rt.ctx[self] = (struct thinkos_context *)&arg[-8];
	__thinkos_defer_sched(); /* signal the scheduler ... */
}
#endif

#endif /* THINKOS_ENABLE_FLAG_WATCH */

#endif /* THINKOS_FLAG_MAX > 0 */

