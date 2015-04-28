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

_Pragma ("GCC optimize (\"Ofast\")")

#define __THINKOS_SYS__
#include <thinkos_sys.h>
#include <thinkos.h>
#include <sys/delay.h>

/* initialize a thread context */
void thinkos_thread_create_svc(int32_t * arg)
{
	struct thinkos_thread_init * init = (struct thinkos_thread_init *)arg;
	struct thinkos_context * ctx;
	uint32_t sp;
	int th;

#if THINKOS_ENABLE_THREAD_ALLOC
	if (init->opt.id >= THINKOS_THREADS_MAX) {
		th = thinkos_alloc_hi(thinkos_rt.th_alloc, THINKOS_THREADS_MAX);
		DCC_LOG2(LOG_INFO, "thinkos_alloc_hi() %d -> %d.", init->opt.id, th);
		DCC_LOG1(LOG_INFO, "thinkos_rt.th_alloc=0x%08x", thinkos_rt.th_alloc);
	} else {
		/* Look for the next available slot */
		th = thinkos_alloc_lo(thinkos_rt.th_alloc, init->opt.id);
		DCC_LOG2(LOG_INFO, "thinkos_alloc_lo() %d -> %d.", init->opt.id, th);
		if (th < 0) {
			th = thinkos_alloc_hi(thinkos_rt.th_alloc, init->opt.id);
			DCC_LOG2(LOG_INFO, "thinkos_alloc_hi() %d -> %d.", 
					init->opt.id, th);
		}
	}

	if (th < 0) {
		arg[0] = THINKOS_EINVAL;
		return;
	}
#else
	th = init->opt.id;
	if (th >= THINKOS_THREADS_MAX) {
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif

	sp = (uint32_t)init->stack_ptr + init->opt.stack_size;

	if (init->opt.stack_size < sizeof(struct thinkos_context)) {
		DCC_LOG1(LOG_ERROR, "stack too small. size=%d", init->opt.stack_size);
		arg[0] = THINKOS_EINVAL;
		return;
	}

	/* initialize stack */
	__thinkos_memset32(init->stack_ptr, 0xdeafbeef, init->opt.stack_size);

	sp &= 0xfffffff8; /* 64bits alignemnt */
	DCC_LOG3(LOG_INFO, "stack ptr=%08x top=%08x size=%d", 
			 init->stack_ptr, sp, init->opt.stack_size);

	sp -= sizeof(struct thinkos_context);
	DCC_LOG1(LOG_INFO, "sp=%08x", sp);

	ctx = (struct thinkos_context *)sp;
	__thinkos_memset32(ctx, 0, sizeof(struct thinkos_context));
	ctx->r0 = (uint32_t)init->arg;
	ctx->lr = (uint32_t)thinkos_thread_exit;
	ctx->pc = (uint32_t)init->task;
	ctx->xpsr = 0x01000000;
	thinkos_rt.ctx[th] = ctx;


#if THINKOS_ENABLE_THREAD_INFO
	thinkos_rt.th_inf[th] = init->inf;
#endif

#if THINKOS_ENABLE_TIMESHARE
	thinkos_rt.sched_pri[th] = init->opt.priority;
	if (thinkos_rt.sched_pri[th] > THINKOS_SCHED_LIMIT_MAX)
		thinkos_rt.sched_pri[th] = THINKOS_SCHED_LIMIT_MAX;

	/* update schedule limit */
	if (thinkos_rt.sched_limit < thinkos_rt.sched_pri[th]) {
		thinkos_rt.sched_limit = thinkos_rt.sched_pri[th];
	}
	thinkos_rt.sched_val[th] = thinkos_rt.sched_limit / 2;
#endif

#if THINKOS_ENABLE_PAUSE
	if (init->opt.paused) 
		/* insert into the paused list */
		__bit_mem_wr(&thinkos_rt.wq_paused, th, 1);  
	else
#endif
	{
		/* insert into the ready list */
		__bit_mem_wr(&thinkos_rt.wq_ready, th, 1);  
		__thinkos_defer_sched();
	}

#if THINKOS_ENABLE_TIMESHARE
	DCC_LOG5(LOG_INFO, "<%d> pri=%d/%d task=%08x sp=%08x", 
			 th, thinkos_rt.sched_pri[th], 
			 thinkos_rt.sched_limit, init->task, ctx);
#endif

	arg[0] = th;
}

#if THINKOS_ENABLE_THREAD_INFO
/* FIXME: move this definition elsewere, or allow it 
   to be configured by the user ... */
const struct thinkos_thread_inf thinkos_main_inf = {
	.tag = "MAIN"
};
#endif


