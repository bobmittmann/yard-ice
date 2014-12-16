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
#include <stdio.h>

#define __THINKOS_SYS__
#include <thinkos_sys.h>

#define __THINKOS_IRQ__
#include <thinkos_irq.h>

#include <thinkos.h>

#if THINKOS_FLAG_MAX > 0

#if THINKOS_ENABLE_FLAG_ALLOC
void thinkos_flag_alloc_svc(int32_t * arg)
{
	unsigned int flag;

	flag = __thinkos_flag_alloc();

	DCC_LOG1(LOG_INFO, "flag=%d", flag);
	arg[0] = flag;
}

void thinkos_flag_free_svc(int32_t * arg)
{
	unsigned int flag = arg[0];

#if THINKOS_ENABLE_ARG_CHECK
	if ((flag < THINKOS_FLAG_BASE) || 
		(flag >= (THINKOS_FLAG_BASE + THINKOS_FLAG_MAX))) {
		DCC_LOG1(LOG_ERROR, "object %d is not a flag!", flag);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif

	DCC_LOG1(LOG_INFO, "flag=%d", flag);
	__thinkos_flag_free(flag);
}
#endif

void thinkos_flag_val_svc(int32_t * arg)
{
	unsigned int wq = arg[0];

#if THINKOS_ENABLE_ARG_CHECK
	unsigned int flag = wq - THINKOS_FLAG_BASE;

	if (flag >= THINKOS_FLAG_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not a flag!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_FLAG_ALLOC
	if (__bit_mem_rd(&thinkos_rt.flag_alloc, flag) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid flag %d!", flag);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	arg[0] = __thinkos_flag_is_set(wq);
}

void thinkos_flag_wait_svc(int32_t * arg)
{
	unsigned int wq = arg[0];
	int self = thinkos_rt.active;

#if THINKOS_ENABLE_ARG_CHECK
	unsigned int flag = wq - THINKOS_FLAG_BASE;

	if (flag >= THINKOS_FLAG_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not a flag!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_FLAG_ALLOC
	if (__bit_mem_rd(&thinkos_rt.flag_alloc, flag) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid flag %d!", flag);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	arg[0] = 0;

	cm3_cpsid_i();
	if (__thinkos_flag_is_set(wq)) {
		cm3_cpsie_i();
		DCC_LOG1(LOG_INFO, "flag %d is set!", wq);
		return;
	} 

	/* insert into the wait queue */
	__thinkos_wq_insert(wq, self);
	DCC_LOG2(LOG_INFO, "<%d> waiting for flag %d...", self, wq);

	/* wait for event */

	/* remove from the ready wait queue */
	__bit_mem_wr(&thinkos_rt.wq_ready, self, 0);  

#if THINKOS_ENABLE_TIMESHARE
	/* if the ready queue is empty, collect
	 the threads from the CPU wait queue */
#if (THINKOS_THREADS_MAX < 32) 
	if (thinkos_rt.wq_ready == (1 << THINKOS_THREADS_MAX)) {
		/* No more threads into the ready queue,
		 move the timeshare queue to the ready queue.
		 Keep the IDLE bit set */
		thinkos_rt.wq_ready |= thinkos_rt.wq_tmshare;
		thinkos_rt.wq_tmshare = 0;
	} 
#else
	if (thinkos_rt.wq_ready == 0) {
		/* no more threads into the ready queue,
		 move the timeshare queue to the ready queue */
		thinkos_rt.wq_ready = thinkos_rt.wq_tmshare;
		thinkos_rt.wq_tmshare = 0;
	} 
#endif
#endif
	cm3_cpsie_i();
	/* signal the scheduler ... */
	__thinkos_defer_sched();
}

#if THINKOS_ENABLE_TIMED_CALLS
void thinkos_flag_timedwait_svc(int32_t * arg)
{
	unsigned int wq = arg[0];
	uint32_t ms = (uint32_t)arg[1];
	int self = thinkos_rt.active;

#if THINKOS_ENABLE_ARG_CHECK
	unsigned int flag = wq - THINKOS_FLAG_BASE;

	if (flag >= THINKOS_FLAG_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not a flag!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_FLAG_ALLOC
	if (__bit_mem_rd(&thinkos_rt.flag_alloc, flag) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid flag %d!", flag);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	cm3_cpsid_i();
	if (__thinkos_flag_is_set(wq)) {
		cm3_cpsie_i();
		arg[0] = 0;
		DCC_LOG1(LOG_INFO, "flag %d is set!", wq);
		return;
	} 

	/* insert into the flag wait queue */
	__thinkos_tmdwq_insert(wq, self, ms);

	DCC_LOG2(LOG_INFO, "<%d> waiting for flag %d...", self, wq);

	/* Set the default return value to timeout. The
	   flag_rise() call will change it to 0 */
	arg[0] = THINKOS_ETIMEDOUT;

	/* wait for event */

	/* remove from the ready wait queue */
	__bit_mem_wr(&thinkos_rt.wq_ready, self, 0);  

#if THINKOS_ENABLE_TIMESHARE
	/* if the ready queue is empty, collect
	 the threads from the CPU wait queue */
#if (THINKOS_THREADS_MAX < 32) 
	if (thinkos_rt.wq_ready == (1 << THINKOS_THREADS_MAX)) {
		/* No more threads into the ready queue,
		 move the timeshare queue to the ready queue.
		 Keep the IDLE bit set */
		thinkos_rt.wq_ready |= thinkos_rt.wq_tmshare;
		thinkos_rt.wq_tmshare = 0;
	} 
#else
	if (thinkos_rt.wq_ready == 0) {
		/* no more threads into the ready queue,
		 move the timeshare queue to the ready queue */
		thinkos_rt.wq_ready = thinkos_rt.wq_tmshare;
		thinkos_rt.wq_tmshare = 0;
	} 
#endif
#endif
	cm3_cpsie_i();
	/* signal the scheduler ... */
	__thinkos_defer_sched();
}
#endif


void thinkos_flag_clr_svc(int32_t * arg)
{
	unsigned int wq = arg[0];

#if THINKOS_ENABLE_ARG_CHECK
	unsigned int flag = wq - THINKOS_FLAG_BASE;

	if (flag >= THINKOS_FLAG_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not a flag!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_FLAG_ALLOC
	if (__bit_mem_rd(&thinkos_rt.flag_alloc, flag) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid flag %d!", flag);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	arg[0] = __thinkos_flag_is_set(wq);

	__thinkos_flag_clr(wq);
}

void thinkos_flag_set_svc(int32_t * arg)
{
	unsigned int wq = arg[0];

#if THINKOS_ENABLE_ARG_CHECK
	unsigned int flag = wq - THINKOS_FLAG_BASE;

	if (flag >= THINKOS_FLAG_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not a flag!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_FLAG_ALLOC
	if (__bit_mem_rd(&thinkos_rt.flag_alloc, flag) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid flag %d!", flag);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	arg[0] = __thinkos_flag_is_set(wq);

	__thinkos_flag_set(wq);
}

void thinkos_flag_give_svc(int32_t * arg)
{
	unsigned int wq = arg[0];
	unsigned int flag = wq - THINKOS_FLAG_BASE;
	int th;

#if THINKOS_ENABLE_ARG_CHECK
	if (flag >= THINKOS_FLAG_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not a flag!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_FLAG_ALLOC
	if (__bit_mem_rd(&thinkos_rt.flag_alloc, flag) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid flag %d!", flag);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	arg[0] = 0;

	cm3_cpsid_i();
	/* get the flag state */
	if (__bit_mem_rd(&thinkos_rt.flag, flag)) {
		cm3_cpsie_i();
		DCC_LOG1(LOG_INFO, "flag %d is already set!", wq);
		return;
	}

	/* set the flag bit */
	__bit_mem_wr(&thinkos_rt.flag, flag, 1);  
	cm3_cpsie_i();

	if ((th = __thinkos_wq_head(wq)) != THINKOS_THREAD_NULL) {
		/* wakeup from the flag wait queue */
		__thinkos_wakeup(wq, th);
		DCC_LOG2(LOG_MSG, "<%d> waked up with flag %d", th, wq);
		/* clear the flag bit */
		__bit_mem_wr(&thinkos_rt.flag, flag, 0);  
		/* signal the scheduler ... */
		__thinkos_defer_sched();
	}
}

void thinkos_flag_take_svc(int32_t * arg)
{
	unsigned int wq = arg[0];
	int self = thinkos_rt.active;

#if THINKOS_ENABLE_ARG_CHECK
	unsigned int flag = wq - THINKOS_FLAG_BASE;

	if (flag >= THINKOS_FLAG_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not a flag!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_FLAG_ALLOC
	if (__bit_mem_rd(&thinkos_rt.flag_alloc, flag) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid flag %d!", flag);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	arg[0] = 0;

	cm3_cpsid_i();

	if (__thinkos_flag_is_set(wq)) {
		__thinkos_flag_clr(wq);
		cm3_cpsie_i();
		DCC_LOG2(LOG_INFO, "<%d> flag %d was set.", self, wq);
		return;
	} 

	/* insert into the wait queue */
	__thinkos_wq_insert(wq, self);

	DCC_LOG2(LOG_INFO, "<%d> waiting for flag %d...", self, wq);

	/* wait for event */

	/* remove from the ready wait queue */
	__bit_mem_wr(&thinkos_rt.wq_ready, self, 0);  

#if THINKOS_ENABLE_TIMESHARE
	/* if the ready queue is empty, collect
	 the threads from the CPU wait queue */
#if (THINKOS_THREADS_MAX < 32) 
	if (thinkos_rt.wq_ready == (1 << THINKOS_THREADS_MAX)) {
		/* No more threads into the ready queue,
		 move the timeshare queue to the ready queue.
		 Keep the IDLE bit set */
		thinkos_rt.wq_ready |= thinkos_rt.wq_tmshare;
		thinkos_rt.wq_tmshare = 0;
	} 
#else
	if (thinkos_rt.wq_ready == 0) {
		/* no more threads into the ready queue,
		 move the timeshare queue to the ready queue */
		thinkos_rt.wq_ready = thinkos_rt.wq_tmshare;
		thinkos_rt.wq_tmshare = 0;
	} 
#endif
#endif
	cm3_cpsie_i();

	/* signal the scheduler ... */
	__thinkos_defer_sched();
}

#if THINKOS_ENABLE_TIMED_CALLS
void thinkos_flag_timedtake_svc(int32_t * arg)
{
	unsigned int wq = arg[0];
	uint32_t ms = (uint32_t)arg[1];
	int self = thinkos_rt.active;

#if THINKOS_ENABLE_ARG_CHECK
	unsigned int flag = wq - THINKOS_FLAG_BASE;

	if (flag >= THINKOS_FLAG_MAX) {
		DCC_LOG1(LOG_ERROR, "object %d is not a flag!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_FLAG_ALLOC
	if (__bit_mem_rd(&thinkos_rt.flag_alloc, flag) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid flag %d!", flag);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	cm3_cpsid_i();

	if (__thinkos_flag_is_set(wq)) {
		__thinkos_flag_clr(wq);
		cm3_cpsie_i();
		arg[0] = 0;
		DCC_LOG1(LOG_INFO, "flag %d is set!", wq);
		return;
	} 

	/* insert into the flag wait queue */
	__thinkos_tmdwq_insert(wq, self, ms);

	DCC_LOG2(LOG_INFO, "<%d> waiting for flag %d...", self, wq);
	/* Set the default return value to timeout. The
	   flag_rise() call will change it to 0 */
	arg[0] = THINKOS_ETIMEDOUT;

	/* wait for event */

	/* remove from the ready wait queue */
	__bit_mem_wr(&thinkos_rt.wq_ready, self, 0);  

#if THINKOS_ENABLE_TIMESHARE
	/* if the ready queue is empty, collect
	 the threads from the CPU wait queue */
#if (THINKOS_THREADS_MAX < 32) 
	if (thinkos_rt.wq_ready == (1 << THINKOS_THREADS_MAX)) {
		/* No more threads into the ready queue,
		 move the timeshare queue to the ready queue.
		 Keep the IDLE bit set */
		thinkos_rt.wq_ready |= thinkos_rt.wq_tmshare;
		thinkos_rt.wq_tmshare = 0;
	} 
#else
	if (thinkos_rt.wq_ready == 0) {
		/* no more threads into the ready queue,
		 move the timeshare queue to the ready queue */
		thinkos_rt.wq_ready = thinkos_rt.wq_tmshare;
		thinkos_rt.wq_tmshare = 0;
	} 
#endif
#endif
	cm3_cpsie_i();
	/* signal the scheduler ... */
	__thinkos_defer_sched();
}
#endif


#if 0
/* wakeup a single thread waiting on the flag 
   OR set the flag */
void thinkos_flag_give_i(int wq) 
{
	uint32_t pri;
	int th;

	pri = cm3_primask_get();
	cm3_primask_set(1);
	/* get the flag state */
	if (__bit_mem_rd(&thinkos_rt.flag, wq - THINKOS_FLAG_BASE) == 0) {
		/* get a thread from the queue */
		if ((th = __thinkos_wq_head(wq)) != THINKOS_THREAD_NULL) {
			DCC_LOG3(LOG_INFO, "flag %d, wakeup %d, pri=%d...", wq, th, pri);
			__thinkos_wakeup(wq, th);
			/* signal the scheduler ... */
			__thinkos_defer_sched();
		} else {
			DCC_LOG1(LOG_INFO, "set flag %d...", wq);
			/* set the flag bit */
			__bit_mem_wr(&thinkos_rt.flag, wq - THINKOS_FLAG_BASE, 1);  
		}
	}
	cm3_primask_set(pri);
}
#endif

#endif /* THINKOS_FLAG_MAX > 0 */

