/* 
 * thikos_wq.c
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

#if THINKOS_WAIT_QUEUE_MAX > 0

#if THINKOS_ENABLE_WAIT_QUEUE_ALLOC
void thinkos_wq_alloc_svc(int32_t * arg)
{
	arg[0] = thinkos_alloc_lo(&thinkos_rt.wq_alloc, 0);
}

void thinkos_wq_free_svc(int32_t * arg)
{
	__bit_mem_wr(&thinkos_rt.wq_alloc, arg[0], 0);
}
#endif

void thinkos_wq_wait_svc(int32_t * arg)
{
	int wq = arg[0];
	int this = thinkos_rt.active;

#if THINKOS_ENABLE_WAIT_QUEUE_ALLOC
	if (__bit_mem_rd(&thinkos_rt.wq_alloc, wq) == 0) {
		arg[0] = -1;
		return;
	}
#endif

	/* insert into the wq wait queue */
	bmp_bit_set(&thinkos_rt.wq_event[wq], this);  
#if THINKOS_ENABLE_THREAD_STAT
	/* update status */
	thinkos_rt.th_stat[this] = __wq_idx(&thinkos_rt.wq_event[wq]) << 1;
#endif

	/* wait for event */
	__thinkos_wait();
}

#if 0
void thinkos_wq_notify_svc(int32_t * arg)
{
	int wq = arg[0];

#if THINKOS_ENABLE_WAIT_QUEUE_ALLOC
	if (__bit_mem_rd(&thinkos_rt.wq_alloc, wq) == 0) {
		arg[0] = -1;
		return;
	}
#endif

	idx = __thinkos_wq_head(&thinkos_rt.wq_wq[wq]);
	if (idx != THINKOS_IDX_NULL) {
#if THINKOS_ENABLE_THREAD_STAT
		/* update status */
		thinkos_rt.th_stat[idx] = 0;
#endif
		/* remove from the wq wait queue */
		bmp_bit_clr(&thinkos_rt.wq_wq[wq], idx);  
		/* insert the thread into ready queue */
		__bit_mem_wr(&thinkos_rt.wq_ready, idx, 1);  
		/* signal the scheduler ... */
		__thinkos_defer_sched();
	}
}
#endif

void thinkos_event_notify(int wq)
{
	int idx;

	idx = __thinkos_wq_head(&thinkos_rt.wq_event[wq]);

	if (idx != THINKOS_IDX_NULL) {
#if THINKOS_ENABLE_THREAD_STAT
		/* update status */
		thinkos_rt.th_stat[idx] = 0;
#endif
		/* remove from the wq wait queue */
		bmp_bit_clr(&thinkos_rt.wq_event[wq], idx);  
		/* insert the thread into ready queue */
		bmp_bit_set(&thinkos_rt.wq_ready, idx);  
		/* signal the scheduler ... */
		__thinkos_defer_sched();
	}
}

void thinkos_event_wait(int wq)
{
	int this = thinkos_rt.active;

	/* insert into the wq wait queue */
	bmp_bit_set(&thinkos_rt.wq_event[wq], this);  

#if THINKOS_ENABLE_THREAD_STAT
	/* update status */
	thinkos_rt.th_stat[this] = __wq_idx(&thinkos_rt.wq_event[wq]) << 1;
#endif

	/* wait for event */
	__thinkos_wait();
	__thinkos_critical_exit();

	__thinkos_critical_enter();
}


#endif /* THINKOS_WAIT_QUEUE_MAX > 0 */

