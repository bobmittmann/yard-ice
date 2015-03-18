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
	if (__bit_mem_rd(&thinkos_rt.th_alloc, th) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid thread %d!", th);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

#if THINKOS_ENABLE_CANCEL
	/* remove thread from the canceled wait queue */
	if (__bit_mem_rd(&thinkos_rt.wq_canceled, th)) {
		__bit_mem_wr(&thinkos_rt.wq_canceled, th, 0);  
		__bit_mem_wr(&thinkos_rt.wq_ready, th, 1);  
	}
#endif /* THINKOS_ENABLE_CANCEL */

	/* insert the current thread (self) into the joining thread wait queue */
	wq = __wq_idx(&thinkos_rt.wq_join[th]);
	__thinkos_wq_insert(wq, self);

	/* wait for event */
	__thinkos_wait(self);

	/* set the return to ERROR as a default value. The
	   exit function of the joining thread will set this to the 
	   appropriate return code */
	arg[0] = -1;
}
#endif

#if THINKOS_ENABLE_CANCEL
void thinkos_cancel_svc(int32_t * arg)
{
	unsigned int th = arg[0];
	int code = arg[1];
	unsigned int wq;
	int stat;

#if THINKOS_ENABLE_ARG_CHECK
	if (th >= THINKOS_THREADS_MAX) {
		DCC_LOG1(LOG_ERROR, "invalid thread %d!", th);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_THREAD_ALLOC
	if (__bit_mem_rd(&thinkos_rt.th_alloc, th) == 0) {
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

#if (THINKOS_ENABLE_THREAD_STAT == 0)
#error "thinkos_cancel() depends on THINKOS_ENABLE_THREAD_STAT"	
#endif
	stat = thinkos_rt.th_stat[th];
	/* remove from other wait queue including wq_ready */
	__bit_mem_wr(&thinkos_rt.wq_lst[stat >> 1], th, 0);

#if THINKOS_ENABLE_JOIN
	/* insert into the canceled wait queue and wait for a join call */ 
	wq = __wq_idx(&thinkos_rt.wq_canceled);
#else /* THINKOS_ENABLE_JOIN */
	/* if join is not enabled insert into the ready queue */
	wq = __wq_idx(&thinkos_rt.wq_ready);
#endif /* THINKOS_ENABLE_JOIN */

	__thinkos_wq_insert(wq, th);

#if THINKOS_ENABLE_TIMESHARE
	/* possibly remove from the time share wait queue */
	__bit_mem_wr(&thinkos_rt.wq_tmshare, th, 0); 
#endif

#if THINKOS_ENABLE_CLOCK
	/* possibly remove from the time wait queue */
	__bit_mem_wr(&thinkos_rt.wq_clock, th, 0);  
#endif

	DCC_LOG3(LOG_TRACE, "<%d> cancel %d, with code %d!", 
			 thinkos_rt.active, th, code); 

	thinkos_rt.ctx[th]->pc = (uint32_t)thinkos_thread_exit;
	thinkos_rt.ctx[th]->r0 = code;
	arg[0] = 0;
}
#endif


#if THINKOS_ENABLE_EXIT
void thinkos_exit_svc(int32_t * arg)
{
	int self = thinkos_rt.active;
	int code = arg[0];
	unsigned int wq;

#if THINKOS_ENABLE_JOIN
	/* insert into the canceled wait queue and wait for a join call */ 
	wq = __wq_idx(&thinkos_rt.wq_canceled);
#else /* THINKOS_ENABLE_JOIN */
	/* if join is not enabled insert into the ready queue */
	wq = __wq_idx(&thinkos_rt.wq_ready);
#endif /* THINKOS_ENABLE_JOIN */

	__thinkos_wq_insert(wq, self);

	DCC_LOG2(LOG_TRACE, "<%d> exit with code %d!", self, code); 

	/* adjust PC */
	arg[6] = (uint32_t)thinkos_thread_exit;
	/* set the return code at R0 */
	arg[0] = code;

	__thinkos_defer_sched();
}
#endif


#if THINKOS_ENABLE_PAUSE
void thinkos_resume_svc(int32_t * arg)
{
	unsigned int th = arg[0];
	unsigned int wq;
	int stat;

#if THINKOS_ENABLE_ARG_CHECK
	if (th >= THINKOS_THREADS_MAX) {
		DCC_LOG1(LOG_ERROR, "invalid thread %d!", th);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_THREAD_ALLOC
	if (__bit_mem_rd(&thinkos_rt.th_alloc, th) == 0) {
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	DCC_LOG1(LOG_TRACE, "thread=%d", th);

	arg[0] = 0;

	if (__bit_mem_rd(&thinkos_rt.wq_paused, th) == 0) {
		/* not paused */
		return;
	}

#if (THINKOS_ENABLE_THREAD_STAT == 0)
#error "thinkos_resume() depends on THINKOS_ENABLE_THREAD_STAT"	
#endif
	/* remove from the paused queue */
	__bit_mem_wr(&thinkos_rt.wq_paused, th, 0);  
	/* reinsert the thread into a waiting queue, including ready  */
	stat = thinkos_rt.th_stat[th];
	wq = stat >> 1;
	DCC_LOG2(LOG_TRACE, "stat=0x%02x wq=%d", stat, wq);
	__bit_mem_wr(&thinkos_rt.wq_lst[wq], th, 1);

#if THINKOS_ENABLE_CLOCK
	/* reenable the clock according to the thread status */
	__bit_mem_wr(&thinkos_rt.wq_clock, th, 1);
#endif
	__thinkos_defer_sched();
}

void thinkos_pause_svc(int32_t * arg)
{
	unsigned int th = arg[0];
	unsigned int wq;
	int stat;

#if THINKOS_ENABLE_ARG_CHECK
	if (th >= THINKOS_THREADS_MAX) {
		DCC_LOG1(LOG_ERROR, "invalid thread %d!", th);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_THREAD_ALLOC
	if (__bit_mem_rd(&thinkos_rt.th_alloc, th) == 0) {
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

#if (THINKOS_ENABLE_THREAD_STAT == 0)
#error "thinkos_pause() depends on THINKOS_ENABLE_THREAD_STAT"	
#endif

	DCC_LOG1(LOG_TRACE, "thread=%d", th);

	arg[0] = 0;

	if (__bit_mem_rd(&thinkos_rt.wq_paused, th) != 0) {
		/* paused */
		return;
	}

	/* insert into the paused queue */
	__bit_mem_wr(&thinkos_rt.wq_paused, th, 1);

	/* remove the thread from a waiting queue, including ready  */
	stat = thinkos_rt.th_stat[th];
	wq = stat >> 1;
	DCC_LOG2(LOG_TRACE, "stat=0x%02x wq=%d", stat, wq);
	__bit_mem_wr(&thinkos_rt.wq_lst[wq], th, 0);

#if THINKOS_ENABLE_TIMESHARE
	/* possibly remove from the time share wait queue */
	__bit_mem_wr(&thinkos_rt.wq_tmshare, th, 0);
#endif

#if THINKOS_ENABLE_CLOCK
	/* disable the clock */
	__bit_mem_wr(&thinkos_rt.wq_clock, th, 0);
#endif

	__thinkos_defer_sched();
}
#endif /* THINKOS_ENABLE_PAUSE */

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
	if (th >= THINKOS_THREADS_MAX)
		th = THINKOS_THREADS_MAX - 1;
#endif

	sp = (uint32_t)init->stack_ptr + init->opt.stack_size;

	if (init->opt.stack_size < sizeof(struct thinkos_context)) {
		DCC_LOG1(LOG_ERROR, "stack too small. size=%d", init->opt.stack_size);
		arg[0] = th;
		return;
	}

	sp &= 0xfffffff8; /* 64bits alignemnt */
	DCC_LOG3(LOG_INFO, "stack ptr=%08x top=%08x size=%d", 
			 init->stack_ptr, sp, init->opt.stack_size);

	sp -= sizeof(struct thinkos_context);

	DCC_LOG1(LOG_TRACE, "sp=%08x", sp);

	/* initialize stack */
	{
		uint32_t * ptr;
		for (ptr = init->stack_ptr; ptr < (uint32_t *)sp; ++ptr)
			*ptr = 0xdeadbeef;
	}

	ctx = (struct thinkos_context *)sp;

//	ctx->ret = CM3_EXC_RET_THREAD_PSP;
	ctx->r0 = (uint32_t)init->arg;
	ctx->r1 = 0;
	ctx->r2 = 0;
	ctx->r3 = 0;
	ctx->r4 = 0;
	ctx->r5 = 0;
	ctx->r6 = 0;
	ctx->r7 = 0;
	ctx->r8 = 0;
	ctx->r9 = 0;
	ctx->r10 = 0;
	ctx->r11 = 0;
	ctx->r12 = 0;
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
	if (init->opt.f_paused) 
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
	DCC_LOG5(LOG_TRACE, "<%d> pri=%d/%d task=%08x sp=%08x", 
			 th, thinkos_rt.sched_pri[th], 
			 thinkos_rt.sched_limit, init->task, ctx);
#endif

	arg[0] = th;
}

void thinkos_sleep_svc(int32_t * arg)
{
	uint32_t ms = (uint32_t)arg[0];
#if THINKOS_ENABLE_CLOCK
	int self = thinkos_rt.active;

	/* set the clock */
	thinkos_rt.clock[self] = thinkos_rt.ticks + ms;
	/* insert into the clock wait queue */
	__bit_mem_wr(&thinkos_rt.wq_clock, self, 1);

#if THINKOS_ENABLE_THREAD_STAT
	/* mark the thread clock enable bit */
	thinkos_rt.th_stat[self] = (THINKOS_WQ_CLOCK << 1) + 1;
#endif

	DCC_LOG2(LOG_MSG, "<%d> waiting %d milliseconds...", self, ms);

	/* wait for event */
	__thinkos_wait(self);
#else
	DCC_LOG1(LOG_TRACE, "busy wait: %d milliseconds...", ms);
	udelay(1000 * ms);
#endif
}

#if THINKOS_ENABLE_ALARM
void thinkos_alarm_svc(int32_t * arg)
{
	uint32_t ms = (uint32_t)arg[0];
	int self = thinkos_rt.active;

	/* set the clock */
	thinkos_rt.clock[self] = ms;
	/* insert into the clock wait queue */
	__bit_mem_wr(&thinkos_rt.wq_clock, self, 1);
#if THINKOS_ENABLE_THREAD_STAT
	/* mark the thread clock enable bit */
	thinkos_rt.th_stat[self] = (THINKOS_WQ_CLOCK << 1) + 1;
#endif
	/* wait for event */
	__thinkos_wait(self);
}
#endif

const struct {
	uint8_t wq[0]; /* placeholder */
	uint8_t wq_ready; /* ready threads queue */
#if THINKOS_ENABLE_TIMESHARE
	uint8_t wq_tmshare; /* Threads waiting for time share cycle */
#endif
#if THINKOS_ENABLE_JOIN
	uint8_t wq_canceled; /* canceled threads wait queue */
#endif
#if THINKOS_ENABLE_PAUSE
	uint8_t wq_paused;
#endif
#if THINKOS_ENABLE_CLOCK
	uint8_t wq_clock;
#endif
#if THINKOS_MUTEX_MAX > 0
	uint8_t wq_mutex[THINKOS_MUTEX_MAX];
#endif
#if THINKOS_COND_MAX > 0
	uint8_t wq_cond[THINKOS_COND_MAX];
#endif
#if THINKOS_SEMAPHORE_MAX > 0
	uint8_t wq_sem[THINKOS_SEMAPHORE_MAX];
#endif
#if THINKOS_EVENT_MAX > 0
	uint8_t wq_event[THINKOS_EVENT_MAX];
#endif
#if THINKOS_FLAG_MAX > 0
	uint8_t wq_flag[THINKOS_FLAG_MAX];
#endif
#if THINKOS_ENABLE_JOIN
	uint8_t wq_join[THINKOS_THREADS_MAX];
#endif
} thinkos_obj_type_lut = {
	.wq_ready = THINKOS_OBJ_READY,
#if THINKOS_ENABLE_TIMESHARE
	.wq_tmshare = THINKOS_OBJ_TMSHARE,
#endif
#if THINKOS_ENABLE_JOIN
	.wq_canceled = THINKOS_OBJ_CANCELED,
#endif
#if THINKOS_ENABLE_PAUSE
	.wq_paused = THINKOS_OBJ_PAUSED,
#endif
#if THINKOS_ENABLE_CLOCK
	.wq_clock = THINKOS_OBJ_CLOCK,
#endif
#if THINKOS_MUTEX_MAX > 0
	.wq_mutex = { [0 ... (THINKOS_MUTEX_MAX - 1)] = THINKOS_OBJ_MUTEX },
#endif 
#if THINKOS_COND_MAX > 0
	.wq_cond = { [0 ... (THINKOS_COND_MAX - 1)] = THINKOS_OBJ_COND },
#endif
#if THINKOS_SEMAPHORE_MAX > 0
	.wq_sem = { [0 ... (THINKOS_SEMAPHORE_MAX - 1)] = THINKOS_OBJ_SEMAPHORE },
#endif
#if THINKOS_EVENT_MAX > 0
	.wq_event = { [0 ... (THINKOS_EVENT_MAX - 1)] = THINKOS_OBJ_EVENT },
#endif
#if THINKOS_FLAG_MAX > 0
	.wq_flag = { [0 ... (THINKOS_FLAG_MAX - 1)] = THINKOS_OBJ_FLAG }
#endif
#if THINKOS_ENABLE_JOIN
	.wq_join = { [0 ... (THINKOS_THREADS_MAX - 1)] = THINKOS_OBJ_JOIN }
#endif
};

int thinkos_obj_type_get(unsigned int oid)
{
	if (oid >= THINKOS_WQ_LST_END)
		return THINKOS_EINVAL;

	return thinkos_obj_type_lut.wq[oid];
}

uint32_t * const thinkos_obj_alloc_lut[] = {
	[THINKOS_OBJ_READY] = NULL,
	[THINKOS_OBJ_TMSHARE] = NULL,
	[THINKOS_OBJ_CANCELED] = NULL,
	[THINKOS_OBJ_PAUSED] = NULL,
	[THINKOS_OBJ_CLOCK] = NULL,
#if THINKOS_ENABLE_MUTEX_ALLOC
	[THINKOS_OBJ_MUTEX] = NULL,
#else
#endif
#if THINKOS_ENABLE_COND_ALLOC
	[THINKOS_OBJ_COND] = thinkos_rt.cond_alloc,
#else
	[THINKOS_OBJ_COND] = NULL
#endif
#if THINKOS_ENABLE_SEM_ALLOC
	[THINKOS_OBJ_SEMAPHORE] = thinkos_rt.sem_alloc,
#else
	[THINKOS_OBJ_SEMAPHORE] = NULL,
#endif
#if THINKOS_ENABLE_EVENT_ALLOC
	[THINKOS_OBJ_EVENT] = thinkos_rt.ev_alloc,
#else
	[THINKOS_OBJ_EVENT] = NULL,
#endif
#if THINKOS_ENABLE_FLAG_ALLOC
	[THINKOS_OBJ_FLAG] = thinkos_rt.flag_alloc,
#else
	[THINKOS_OBJ_FLAG] = NULL,
#endif
#if THINKOS_ENABLE_THREAD_ALLOC
	[THINKOS_OBJ_JOIN] = thinkos_rt.th_alloc,
#else
	[THINKOS_OBJ_JOIN] = NULL,
#endif
	[THINKOS_OBJ_INVALID] = NULL
};

const uint8_t thinkos_wq_base_lut[] = {
	[THINKOS_OBJ_READY] = THINKOS_WQ_READY,
#if THINKOS_ENABLE_TIMESHARE
	[THINKOS_OBJ_TMSHARE] = THINKOS_WQ_TMSHARE,
#endif
#if THINKOS_ENABLE_JOIN
	[THINKOS_OBJ_CANCELED] = THINKOS_WQ_CANCELED,
#endif
#if THINKOS_ENABLE_PAUSE
	[THINKOS_OBJ_PAUSED] = THINKOS_WQ_PAUSED,
#endif
#if THINKOS_ENABLE_CLOCK
	[THINKOS_OBJ_CLOCK] = THINKOS_WQ_CLOCK,
#endif
#if THINKOS_MUTEX_MAX > 0
	[THINKOS_OBJ_MUTEX] = THINKOS_MUTEX_BASE,
#endif
#if THINKOS_COND_MAX > 0
	[THINKOS_OBJ_COND] = THINKOS_COND_BASE,
#endif
#if THINKOS_SEMAPHORE_MAX > 0
	[THINKOS_OBJ_SEMAPHORE] = THINKOS_SEM_BASE,
#endif
#if THINKOS_EVENT_MAX > 0
	[THINKOS_OBJ_EVENT] = THINKOS_EVENT_BASE,
#endif
#if THINKOS_FLAG_MAX > 0
	[THINKOS_OBJ_FLAG] = THINKOS_FLAG_BASE,
#endif
#if THINKOS_ENABLE_JOIN
	[THINKOS_OBJ_JOIN] = THINKOS_JOIN_BASE,
#endif
	[THINKOS_OBJ_INVALID] = 0 
};

#if THINKOS_ENABLE_BREAK
void thinkos_break_svc(int32_t * arg)
{	
	unsigned int wq = arg[0];
	int th;

#if THINKOS_ENABLE_ARG_CHECK
	uint32_t * alloc;
	unsigned int idx;
	int type;

	if (wq >= THINKOS_WQ_LST_END) {
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif 

#if THINKOS_ENABLE_ARG_CHECK
	type = thinkos_obj_type_lut.wq[wq];
	alloc = thinkos_obj_alloc_lut[type];
	idx = wq - thinkos_wq_base_lut[type];

	if ((alloc != NULL) && __bit_mem_rd(alloc, idx) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid object %d!", wq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif 

	cm3_cpsid_i();

	/* remove all threads from the wait queue */
	if ((th = __thinkos_wq_head(wq)) != THINKOS_THREAD_NULL) {
		do {
			/* break (wakeup) from the wait queue and set
			 the return value to EINTR */
			__thinkos_wakeup_return(wq, th, THINKOS_EINTR);
			DCC_LOG2(LOG_INFO, "<%d> wakeup from %d.", th, wq);
		} while  ((th = __thinkos_wq_head(wq)) != THINKOS_THREAD_NULL);
		/* signal the scheduler ... */
		__thinkos_defer_sched();
	}

	cm3_cpsie_i();

	arg[0] = 0;
}

#endif /* THINKOS_ENABLE_BREAK */


#if THINKOS_ENABLE_THREAD_INFO
/* FIXME: move this definition elsewere ... */
const struct thinkos_thread_info thinkos_main_inf = {
	.tag = "MAIN"
};
#endif


