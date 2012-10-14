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

#include <stdio.h>


#if THINKOS_ENABLE_JOIN
void thinkos_join_svc(int32_t * arg)
{
	int idx = arg[0];

#if THINKOS_ENABLE_THREAD_ALLOC
	if (__bit_mem_rd(&thinkos_rt.th_alloc, idx) == 0) {
		arg[0] = -1;
		return;
	}
#endif
	
	bmp_bit_set(&thinkos_rt.wq_join[idx], thinkos_rt.active); 

#if THINKOS_ENABLE_CANCEL
	if (__bit_mem_rd(&thinkos_rt.wq_canceled, idx)) {
		bmp_bit_clr(&thinkos_rt.wq_canceled, idx);  
#if THINKOS_ENABLE_THREAD_STAT
		thinkos_rt.th_stat[idx] = 0;
#endif
		bmp_bit_set((void *)&thinkos_rt.wq_ready, idx);  
	}
#endif /* THINKOS_ENABLE_CANCEL */

	/* wait for event */
	__thinkos_wait();

	/* set the return to ERROR as a default value. The
	   exit function of the joining thread will set this to the 
	   appropriate return code */
	arg[0] = -1;
}
#endif

#if THINKOS_ENABLE_CANCEL
void thinkos_cancel_svc(int32_t * arg)
{
	int idx = arg[0];
	int code = arg[1];

#if THINKOS_ENABLE_THREAD_ALLOC
	if (__bit_mem_rd(&thinkos_rt.th_alloc, idx) == 0) {
		arg[0] = -1;
		return;
	}
#endif

#if THINKOS_ENABLE_JOIN
	/* insert into the canceled wait queue and
	 wait for a join call */ 
	bmp_bit_set(&thinkos_rt.wq_canceled, idx);  
#if THINKOS_ENABLE_THREAD_STAT
	int stat;
	/* update the thread status */
	stat = thinkos_rt.th_stat[idx];
	/* remove from other wait queue including wq_ready */
	bmp_bit_clr(&thinkos_rt.wq_lst[stat >> 1], idx);  
	thinkos_rt.th_stat[idx] = __wq_idx(&thinkos_rt.wq_canceled) << 1;
#else /* THINKOS_ENABLE_THREAD_STAT */
	bmp_bit_clr(&thinkos_rt.wq_ready, idx);  
#endif /* THINKOS_ENABLE_THREAD_STAT */

#else /* THINKOS_ENABLE_JOIN */
	/* if join is not enabled insert into the ready queue */
	bmp_bit_set(&thinkos_rt.wq_ready, idx);  
#endif /* THINKOS_ENABLE_JOIN */

#if THINKOS_ENABLE_TIMESHARE
	/* possibly remove from the time share wait queue */
	bmp_bit_clr(&thinkos_rt.wq_tmshare, idx);  
#endif

#if THINKOS_ENABLE_CLOCK
	/* possibly remove from the time wait queue */
	bmp_bit_clr(&thinkos_rt.wq_clock, idx);  
#endif

#if 0
	printf("%s(): <%d> cancel %d, with code %d!\n", 
		   __func__, thinkos_rt.active, idx, code); 
#endif
	thinkos_rt.ctx[idx]->pc = (uint32_t)thinkos_thread_exit;
	thinkos_rt.ctx[idx]->r0 = code;
	arg[0] = 0;
}
#endif

#if THINKOS_ENABLE_PAUSE
void thinkos_resume_svc(int32_t * arg)
{
	int idx = arg[0];
	int stat;
#if THINKOS_ENABLE_THREAD_ALLOC
	if (__bit_mem_rd(&thinkos_rt.th_alloc, idx) == 0) {
		arg[0] = -1;
		return;
	}
#endif

	arg[0] = 0;

	if (__bit_mem_rd(&thinkos_rt.wq_paused, idx) == 0) {
		/* not paused */
		return;
	}
	/* remove from the paused queue */
	bmp_bit_clr(&thinkos_rt.wq_paused, idx);  
	/* reinsert the thread into a waiting queue, including ready  */
	stat = thinkos_rt.th_stat[idx];
	bmp_bit_set(&thinkos_rt.wq_lst[stat >> 1], idx);  
#ifndef THINKOS_ENABLE_CLOCK
	/* reenable the clock according to the thread status */
	bmp_bit_set(&thinkos_rt.wq_clock, stat);
#endif
	__thinkos_defer_sched();
}

void thinkos_pause_svc(int32_t * arg)
{
	unsigned int idx = arg[0];

#if THINKOS_ENABLE_THREAD_ALLOC
	if (__bit_mem_rd(&thinkos_rt.th_alloc, idx) == 0) {
		arg[0] = -1;
		return;
	}
#endif
	/* insert into the paused queue */
	bmp_bit_set(&thinkos_rt.wq_paused, idx);  
	/* possibly remove from the ready queue */
	bmp_bit_clr(&thinkos_rt.wq_ready, idx);  
#if THINKOS_ENABLE_TIMESHARE
	/* possibly remove from the time share wait queue */
	bmp_bit_clr(&thinkos_rt.wq_tmshare, idx);  
#endif
#ifndef THINKOS_ENABLE_CLOCK
	/* disable the clock */
	bmp_bit_clr(&thinkos_rt.wq_clock, idx);  
#endif

	__thinkos_defer_sched();

	arg[0] = 0;
}
#endif /* THINKOS_ENABLE_PAUSE */

/* initialize a thread context */
void thinkos_thread_create_svc(int32_t * arg)
{
	struct thinkos_thread_init * init = (struct thinkos_thread_init *)arg;
	struct thinkos_context * ctx;
	uint32_t sp;
	int idx;

#if THINKOS_ENABLE_THREAD_ALLOC
	if (init->opt.id >= THINKOS_THREADS_MAX) {
		idx = thinkos_alloc_hi(&thinkos_rt.th_alloc, THINKOS_THREADS_MAX);
		printf("%s(): thinkos_alloc_hi() %d -> %d.\n", 
			   __func__, init->opt.id, idx);
	} else {
		/* Look for the next available slot */
		idx = thinkos_alloc_lo(&thinkos_rt.th_alloc, init->opt.id);
		printf("%s(): thinkos_alloc_lo() %d -> %d.\n", 
			   __func__, init->opt.id, idx);
		if (idx < 0) {
			idx = thinkos_alloc_hi(&thinkos_rt.th_alloc, init->opt.id);
			printf("%s(): thinkos_alloc_hi() %d -> %d.\n", 
				   __func__, init->opt.id, idx);
		}
	}

	if (idx < 0) {
		arg[0] = idx;
		return;
	}
#else
	idx = init->opt.id;
	if (idx >= THINKOS_THREADS_MAX)
		idx = THINKOS_THREADS_MAX - 1;
#endif

	sp = (uint32_t)init->stack_ptr + init->stack_size;
	sp &= 0xfffffff8; /* 64bits alignemnt */
	sp -= sizeof(struct thinkos_context);

	ctx = (struct thinkos_context *)sp;

	ctx->r0 = (uint32_t)init->arg;
	ctx->lr = (uint32_t)thinkos_thread_exit;
	ctx->pc = (uint32_t)init->task;
	ctx->xpsr = 0x01000000;

	thinkos_rt.ctx[idx] = ctx;

#if THINKOS_ENABLE_TIMESHARE
	thinkos_rt.sched_pri[idx] = init->opt.priority;
	if (thinkos_rt.sched_pri[idx] > THINKOS_SCHED_LIMIT_MAX)
		thinkos_rt.sched_pri[idx] = THINKOS_SCHED_LIMIT_MAX;

	/* update schedule limit */
	if (thinkos_rt.sched_limit < thinkos_rt.sched_pri[idx]) {
		thinkos_rt.sched_limit = thinkos_rt.sched_pri[idx];
	}
	thinkos_rt.sched_val[idx] = thinkos_rt.sched_limit / 2;
#endif

#if THINKOS_ENABLE_PAUSE
	if (init->opt.f_paused) 
		/* insert into the paused list */
		bmp_bit_set(&thinkos_rt.wq_paused, idx);  
	else
#endif
	{
		/* insert into the ready list */
		bmp_bit_set(&thinkos_rt.wq_ready, idx);  
		__thinkos_defer_sched();
	}

	DCC_LOG5(LOG_TRACE, "<%d> pri=%d/%d task=%08x sp=%08x\n", 
			 idx, thinkos_rt.sched_pri[idx], 
			 thinkos_rt.sched_limit, init->task, ctx);

	arg[0] = idx;
}

void thinkos_sleep_svc(int32_t * arg)
{
#if THINKOS_ENABLE_CLOCK
	uint32_t ms = (uint32_t)arg[0];
	int this = thinkos_rt.active;

	/* set the clock */
	thinkos_rt.clock[this] = thinkos_rt.ticks + (ms / 1);
	/* insert into the clock wait queue */
	bmp_bit_set(&thinkos_rt.wq_clock, this);  

#if THINKOS_ENABLE_THREAD_STAT
	/* mark the thread clock enable bit */
	thinkos_rt.th_stat[this] = 1;
#endif

//	printf("%s(): <%d> wait... %02x\n", __func__, this, thinkos_rt.wq_clock);

	/* wait for event */
	__thinkos_wait();
#endif
}

uint32_t thinkos_idle_val(void)
{
	return thinkos_idle.snapshot.val;
}

