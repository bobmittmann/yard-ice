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

#if THINKOS_ENABLE_PAUSE

extern const uint8_t thinkos_obj_type_lut[];


static void ready_resume(unsigned int th, unsigned int wq, bool tmw) 
{
	__bit_mem_wr(&thinkos_rt.wq_lst[wq], th, 1);
}
#if THINKOS_ENABLE_TIMESHARE
static void tmshare_resume(unsigned int th, unsigned int wq, bool tmw) 
{
	__bit_mem_wr(&thinkos_rt.wq_lst[wq], th, 1);

}
#endif
#if THINKOS_ENABLE_CLOCK
static void clock_resume(unsigned int th, unsigned int wq, bool tmw) 
{
	__bit_mem_wr(&thinkos_rt.wq_lst[wq], th, 1);
}
#endif
#if THINKOS_MUTEX_MAX > 0
static void mutex_resume(unsigned int th, unsigned int wq, bool tmw) 
{
	unsigned int mutex = wq - THINKOS_MUTEX_BASE;

	if (thinkos_rt.lock[mutex] == -1) {
		thinkos_rt.lock[mutex] = th;
		__bit_mem_wr(&thinkos_rt.wq_ready, th, 1);
	} else {
		__bit_mem_wr(&thinkos_rt.wq_lst[wq], th, 1);
		__bit_mem_wr(&thinkos_rt.wq_clock, th, tmw);
	}
}
#endif
#if THINKOS_COND_MAX > 0
static void cond_resume(unsigned int th, unsigned int wq, bool tmw) 
{
	__bit_mem_wr(&thinkos_rt.wq_lst[wq], th, 1);
	__bit_mem_wr(&thinkos_rt.wq_clock, th, tmw);
}
#endif
#if THINKOS_SEMAPHORE_MAX > 0
static void semaphore_resume(unsigned int th, unsigned int wq, bool tmw) 
{
	unsigned int sem = wq - THINKOS_SEM_BASE;

	if (thinkos_rt.sem_val[sem] > 0) {
		thinkos_rt.sem_val[sem]--;
		__bit_mem_wr(&thinkos_rt.wq_ready, th, 1);
	} else {
		__bit_mem_wr(&thinkos_rt.wq_lst[wq], th, 1);
		__bit_mem_wr(&thinkos_rt.wq_clock, th, tmw);
	}
}
#endif
#if THINKOS_EVENT_MAX > 0
static void evset_resume(unsigned int th, unsigned int wq, bool tmw) 
{
	unsigned int no = wq - THINKOS_EVENT_BASE;
	unsigned int ev;

	/* check for any pending unmasked event */
	if ((ev = __clz(__rbit(thinkos_rt.ev[no].pend & 
						   thinkos_rt.ev[no].mask))) < 32) {
		__bit_mem_wr(&thinkos_rt.ev[no].pend, ev, 0);  
		__bit_mem_wr(&thinkos_rt.wq_ready, th, 1);
	} else {
		__bit_mem_wr(&thinkos_rt.wq_lst[wq], th, 1);
		__bit_mem_wr(&thinkos_rt.wq_clock, th, tmw);
	}
}
#endif
#if THINKOS_FLAG_MAX > 0
static void flag_resume(unsigned int th, unsigned int wq, bool tmw) 
{
	unsigned int idx = wq - THINKOS_FLAG_BASE;

#if THINKOS_ENABLE_FLAG_LOCK
	if ((__bit_mem_rd(thinkos_rt.flag.sig, idx)) && 
		(!__bit_mem_rd(thinkos_rt.flag.lock, idx))) {
			/* lock the flag */
			__bit_mem_wr(thinkos_rt.flag.lock, idx, 1);
			/* clear the signal */
			__bit_mem_wr(thinkos_rt.flag.sig, idx, 0);
			__bit_mem_wr(&thinkos_rt.wq_ready, th, 1);
#else
	if (__bit_mem_rd(thinkos_rt.flag.sig, idx)) {
		/* clear the signal */
		__bit_mem_wr(thinkos_rt.flag.sig, idx, 0);
		__bit_mem_wr(&thinkos_rt.wq_ready, th, 1);
#endif
	} else { 
		__bit_mem_wr(&thinkos_rt.wq_lst[wq], th, 1);
		__bit_mem_wr(&thinkos_rt.wq_clock, th, tmw);
	}
}
#endif
#if THINKOS_ENABLE_JOIN
static void join_resume(unsigned int th, unsigned int wq, bool tmw) 
{
	__bit_mem_wr(&thinkos_rt.wq_lst[wq], th, 1);
}
#endif

const void * resume_ltu[] = {
	[THINKOS_OBJ_READY] = ready_resume,
#if THINKOS_ENABLE_TIMESHARE
	[THINKOS_OBJ_TMSHARE] = tmshare_resume,
#endif
#if THINKOS_ENABLE_CLOCK
	[THINKOS_OBJ_CLOCK] = clock_resume,
#endif
#if THINKOS_MUTEX_MAX > 0
	[THINKOS_OBJ_MUTEX] = mutex_resume,
#endif
#if THINKOS_COND_MAX > 0
	[THINKOS_OBJ_COND] = cond_resume,
#endif
#if THINKOS_SEMAPHORE_MAX > 0
	[THINKOS_OBJ_SEMAPHORE] = semaphore_resume,
#endif
#if THINKOS_EVENT_MAX > 0
	[THINKOS_OBJ_EVENT] = evset_resume,
#endif
#if THINKOS_FLAG_MAX > 0
	[THINKOS_OBJ_FLAG] = flag_resume,
#endif
#if THINKOS_ENABLE_JOIN
	[THINKOS_OBJ_JOIN] = join_resume,
#endif
};

bool __thinkos_thread_resume(unsigned int th)
{
	void (* resume)(unsigned int, unsigned int, bool);
	unsigned int wq;
	bool tmw;
	int stat;
	int type;

	if (__bit_mem_rd(&thinkos_rt.wq_paused, th) == 0) {
		DCC_LOG1(LOG_WARNING, "thread=%d is not paused!", th);
		/* not paused, this is not an error condition. */
		return false;
	}

	/* remove from the paused queue */
	__bit_mem_wr(&thinkos_rt.wq_paused, th, 0);  

	/* reinsert the thread into a waiting queue, including ready  */
	stat = thinkos_rt.th_stat[th];
	wq = stat >> 1;
	tmw = stat & 1;
	DCC_LOG3(LOG_TRACE, "thread=%d wq=%d clk=%d", th, wq, tmw);

	type = thinkos_obj_type_lut[wq];
	resume = (void (*)(unsigned int, unsigned int, bool))resume_ltu[type];
	resume(th, wq, tmw);

	return true;
}

#if (THINKOS_ENABLE_THREAD_STAT == 0)
#error "thinkos_resume() depends on THINKOS_ENABLE_THREAD_STAT"	
#endif

void thinkos_resume_svc(int32_t * arg)
{
	unsigned int th = arg[0];

#if THINKOS_ENABLE_ARG_CHECK
	if (th >= THINKOS_THREADS_MAX) {
		DCC_LOG1(LOG_ERROR, "invalid thread %d!", th);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_THREAD_ALLOC
	if (__bit_mem_rd(thinkos_rt.th_alloc, th) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid thread %d!", th);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	if (thinkos_rt.ctx[th] == NULL) {
		DCC_LOG1(LOG_ERROR, "invalid thread %d!", th);
		arg[0] = THINKOS_EINVAL;
		return;
	}

	arg[0] = 0;

	if (__thinkos_thread_resume(th))
		__thinkos_defer_sched();
}

bool __thinkos_thread_pause(unsigned int th)
{
	unsigned int wq;
	int stat;

	if (__bit_mem_rd(&thinkos_rt.wq_paused, th) != 0) {
		DCC_LOG1(LOG_WARNING, "thread=%d is paused already!", th);
		/* paused */
		return false;
	}

	/* insert into the paused queue */
	__bit_mem_wr(&thinkos_rt.wq_paused, th, 1);

	/* remove the thread from a waiting queue, including ready  */
	stat = thinkos_rt.th_stat[th];
	wq = stat >> 1;
	DCC_LOG4(LOG_TRACE, "thread=%d stat=0x%02x wq=%d clk=%d", 
			 th, stat, wq, (stat & 1));
	__bit_mem_wr(&thinkos_rt.wq_lst[wq], th, 0);

#if THINKOS_ENABLE_TIMESHARE
	/* possibly remove from the time share wait queue */
	__bit_mem_wr(&thinkos_rt.wq_tmshare, th, 0);
#endif

#if THINKOS_ENABLE_CLOCK
	/* disable the clock */
	__bit_mem_wr(&thinkos_rt.wq_clock, th, 0);
#endif

	return true;
}

void thinkos_pause_svc(int32_t * arg)
{
	unsigned int th = arg[0];

#if THINKOS_ENABLE_ARG_CHECK
	if (th >= THINKOS_THREADS_MAX) {
		DCC_LOG1(LOG_ERROR, "invalid thread %d!", th);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_THREAD_ALLOC
	if (__bit_mem_rd(thinkos_rt.th_alloc, th) == 0) {
		DCC_LOG1(LOG_ERROR, "invalid thread %d!", th);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

#if (THINKOS_ENABLE_THREAD_STAT == 0)
#error "thinkos_pause() depends on THINKOS_ENABLE_THREAD_STAT"	
#endif

	if (thinkos_rt.ctx[th] == NULL) {
		DCC_LOG1(LOG_ERROR, "invalid thread %d!", th);
		arg[0] = THINKOS_EINVAL;
		return;
	}

	arg[0] = 0;

	if (__thinkos_thread_pause(th))
		__thinkos_defer_sched();
}

#endif /* THINKOS_ENABLE_PAUSE */

