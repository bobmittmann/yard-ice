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

#if THINKOS_ENABLE_EXIT
static void __exit_stub(int code)
{
	thinkos_exit(code);
}
#endif

void __thinkos_thread_init(unsigned int thread_id, uint32_t sp, 
						   void * task, void * arg)
{
	struct thinkos_context * ctx;
	uint32_t pc;

	pc = (uint32_t)task;
	sp &= 0xfffffff8; /* 64bits alignemnt */

	DCC_LOG3(LOG_TRACE, "thread_id=%d pc=%08x sp=%08x", thread_id, pc, sp);

	sp -= sizeof(struct thinkos_context);
	ctx = (struct thinkos_context *)sp;

	__thinkos_memset32(ctx, 0, sizeof(struct thinkos_context));

	ctx->r0 = (uint32_t)arg;
#if THINKOS_ENABLE_EXIT
	ctx->lr = (uint32_t)__exit_stub;
#else
	ctx->lr = (uint32_t)__thinkos_thread_exit;
#endif
	ctx->pc = pc;
	ctx->xpsr = CM_EPSR_T; /* set the thumb bit */
	thinkos_rt.ctx[thread_id] = ctx;

#if THINKOS_ENABLE_PAUSE
	/* insert into the paused list */
	__bit_mem_wr(&thinkos_rt.wq_paused, thread_id, 1);  
#endif
}

/* initialize a thread context */
void thinkos_thread_create_svc(int32_t * arg)
{
	struct thinkos_thread_init * init = (struct thinkos_thread_init *)arg;
	uint32_t sp;
	int thread_id;

#if THINKOS_ENABLE_THREAD_ALLOC
	DCC_LOG1(LOG_INFO, "thinkos_rt.th_alloc=0x%08x", thinkos_rt.th_alloc[0]);

	if (init->opt.id >= THINKOS_THREADS_MAX) {
		thread_id = thinkos_alloc_hi(thinkos_rt.th_alloc, THINKOS_THREADS_MAX);
		DCC_LOG2(LOG_INFO, "thinkos_alloc_hi() %d -> %d.", init->opt.id, 
				 thread_id);
	} else {
		/* Look for the next available slot */
		thread_id = thinkos_alloc_lo(thinkos_rt.th_alloc, init->opt.id);
		DCC_LOG2(LOG_INFO, "thinkos_alloc_lo() %d -> %d.", init->opt.id, 
				 thread_id);
		if (thread_id < 0) {
			thread_id = thinkos_alloc_hi(thinkos_rt.th_alloc, init->opt.id);
			DCC_LOG2(LOG_INFO, "thinkos_alloc_hi() %d -> %d.", 
					init->opt.id, thread_id);
		}
	}

	if (thread_id < 0) {
		arg[0] = THINKOS_EINVAL;
		return;
	}
#else
	thread_id = init->opt.id;
	if (thread_id >= THINKOS_THREADS_MAX) {
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif

	sp = (uint32_t)init->stack_ptr + init->opt.stack_size;

	if (init->opt.stack_size < sizeof(struct thinkos_context)) {
		DCC_LOG1(LOG_INFO, "stack too small. size=%d", init->opt.stack_size);
		arg[0] = THINKOS_EINVAL;
		return;
	}

#if THINKOS_ENABLE_STACK_INIT
	/* initialize stack */
	__thinkos_memset32(init->stack_ptr, 0xdeadbeef, init->opt.stack_size);
#endif

#if THINKOS_ENABLE_THREAD_INFO
	thinkos_rt.th_inf[thread_id] = init->inf;
#endif

#if THINKOS_ENABLE_TIMESHARE
	thinkos_rt.sched_pri[thread_id] = init->opt.priority;
	if (thinkos_rt.sched_pri[thread_id] > THINKOS_SCHED_LIMIT_MAX)
		thinkos_rt.sched_pri[thread_id] = THINKOS_SCHED_LIMIT_MAX;

	/* update schedule limit */
	if (thinkos_rt.sched_limit < thinkos_rt.sched_pri[thread_id]) {
		thinkos_rt.sched_limit = thinkos_rt.sched_pri[thread_id];
	}
	thinkos_rt.sched_val[thread_id] = thinkos_rt.sched_limit / 2;
#endif

	__thinkos_thread_init(thread_id, sp, init->task, init->arg);

#if THINKOS_ENABLE_PAUSE
	if (!init->opt.paused)
#endif
	{
		DCC_LOG(LOG_JABBER, "__thinkos_thread_resume()");
		__thinkos_thread_resume(thread_id);
		DCC_LOG(LOG_JABBER, "__thinkos_defer_sched()");
		__thinkos_defer_sched();
	}

	arg[0] = thread_id;
}


#if THINKOS_ENABLE_THREAD_INFO
extern uint32_t _stack;

#ifndef THINKOS_MAIN_STACK_SIZE
#define THINKOS_MAIN_STACK_SIZE 4096
#endif

/* FIXME: move this definition elsewere, or allow it 
   to be configured by the user ... */
const struct thinkos_thread_inf thinkos_main_inf = {
	.tag = "MAIN",
	.stack_ptr = (void *)((uintptr_t)&_stack - THINKOS_MAIN_STACK_SIZE),
	.stack_size = THINKOS_MAIN_STACK_SIZE,
	.priority = 0,
	.thread_id = 0,
	.paused = 0
};
#endif

