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

extern const uint8_t thinkos_obj_type_lut[];

#if (THINKOS_ENABLE_PAUSE && THINKOS_ENABLE_THREAD_STAT)

static void ready_resume(unsigned int th, unsigned int wq, bool tmw) 
{
#if THINKOS_IRQ_MAX > 0
	{
		int irq;
		for (irq = 0; irq < THINKOS_IRQ_MAX; ++irq) {
			if (thinkos_rt.irq_th[irq] == th) {
				DCC_LOG2(LOG_INFO, "PC=%08x IRQ=%d ......", 
						 thinkos_rt.ctx[th]->pc, irq); 
				/* disable this interrupt source */
				cm3_irq_enable(irq);
				return;
			}
		}
	}
#endif
	DCC_LOG2(LOG_INFO, "thread_id=%d PC=%08x +++++", 
			 th, thinkos_rt.ctx[th]->pc); 
	__bit_mem_wr(&thinkos_rt.wq_ready, th, 1);
}
#if THINKOS_ENABLE_TIMESHARE
static void tmshare_resume(unsigned int th, unsigned int wq, bool tmw) 
{
	DCC_LOG2(LOG_INFO, "thread_id=%d PC=%08x +++++", 
			 th, thinkos_rt.ctx[th]->pc); 
	__bit_mem_wr(&thinkos_rt.wq_lst[wq], th, 1);

}
#endif

#if THINKOS_ENABLE_CLOCK
static void clock_resume(unsigned int th, unsigned int wq, bool tmw) 
{
//	if ((int32_t)(thinkos_rt.clock[th] - thinkos_rt.ticks) <= 0) {
		/* thread's clock is in the past, wakeup now. */
//		DCC_LOG1(LOG_INFO, "timeout PC=%08x .......", thinkos_rt.ctx[th]->pc); 
		/* update the thread status */
//		thinkos_rt.th_stat[th] = 0;
		/* insert into the ready wait queue */
//		__bit_mem_wr(&thinkos_rt.wq_ready, th, 1);  
//	} else {
	DCC_LOG2(LOG_INFO, "thread_id=%d PC=%08x +++++", 
			 th, thinkos_rt.ctx[th]->pc); 
		__bit_mem_wr(&thinkos_rt.wq_clock, th, 1);
//	}
}
#endif

#if THINKOS_MUTEX_MAX > 0
static void mutex_resume(unsigned int th, unsigned int wq, bool tmw) 
{
	unsigned int mutex = wq - THINKOS_MUTEX_BASE;

	DCC_LOG1(LOG_INFO, "PC=%08x ...........", thinkos_rt.ctx[th]->pc); 

	if (thinkos_rt.lock[mutex] == -1) {
		thinkos_rt.lock[mutex] = th;
		/* insert the thread into ready queue */
		__bit_mem_wr(&thinkos_rt.wq_ready, th, 1);
#if THINKOS_ENABLE_TIMED_CALLS
		/* set the thread's return value */
		thinkos_rt.ctx[th]->r0 = 0;
#endif
		/* update status */
		thinkos_rt.th_stat[th] = 0;
	} else {
		__bit_mem_wr(&thinkos_rt.wq_lst[wq], th, 1);
		__bit_mem_wr(&thinkos_rt.wq_clock, th, tmw);
	}
}
#endif

#if THINKOS_COND_MAX > 0
static void cond_resume(unsigned int th, unsigned int wq, bool tmw) 
{
	DCC_LOG1(LOG_INFO, "PC=%08x ...........", thinkos_rt.ctx[th]->pc); 

	__bit_mem_wr(&thinkos_rt.wq_lst[wq], th, 1);
	__bit_mem_wr(&thinkos_rt.wq_clock, th, tmw);
}
#endif

#if THINKOS_SEMAPHORE_MAX > 0
static void semaphore_resume(unsigned int th, unsigned int wq, bool tmw) 
{
	unsigned int sem = wq - THINKOS_SEM_BASE;

	DCC_LOG1(LOG_INFO, "PC=%08x ...........", thinkos_rt.ctx[th]->pc); 

	if (thinkos_rt.sem_val[sem] > 0) {
		thinkos_rt.sem_val[sem]--;
		/* insert the thread into ready queue */
		__bit_mem_wr(&thinkos_rt.wq_ready, th, 1);
#if THINKOS_ENABLE_TIMED_CALLS
		/* set the thread's return value */
		thinkos_rt.ctx[th]->r0 = 0;
#endif
		/* update status */
		thinkos_rt.th_stat[th] = 0;
	} else {
		__bit_mem_wr(&thinkos_rt.wq_lst[wq], th, 1);
#if THINKOS_ENABLE_CLOCK
		__bit_mem_wr(&thinkos_rt.wq_clock, th, tmw);
#endif
	}
}
#endif

#if THINKOS_EVENT_MAX > 0
static void evset_resume(unsigned int th, unsigned int wq, bool tmw) 
{
	unsigned int no = wq - THINKOS_EVENT_BASE;
	unsigned int ev;

	DCC_LOG1(LOG_INFO, "PC=%08x ...........", thinkos_rt.ctx[th]->pc); 

	/* check for any pending unmasked event */
	if ((ev = __clz(__rbit(thinkos_rt.ev[no].pend & 
						   thinkos_rt.ev[no].mask))) < 32) {
		__bit_mem_wr(&thinkos_rt.ev[no].pend, ev, 0);  
		/* insert the thread into ready queue */
		__bit_mem_wr(&thinkos_rt.wq_ready, th, 1);
#if THINKOS_ENABLE_TIMED_CALLS
		/* set the thread's return value */
		thinkos_rt.ctx[th]->r0 = 0;
#endif
		/* update status */
		thinkos_rt.th_stat[th] = 0;
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

	DCC_LOG1(LOG_INFO, "PC=%08x ...........", thinkos_rt.ctx[th]->pc); 

	if (__bit_mem_rd(thinkos_rt.flag, idx)) {
		/* clear the signal */
		__bit_mem_wr(thinkos_rt.flag, idx, 0);
		/* insert the thread into ready queue */
		__bit_mem_wr(&thinkos_rt.wq_ready, th, 1);
#if THINKOS_ENABLE_TIMED_CALLS
		/* set the thread's return value */
		thinkos_rt.ctx[th]->r0 = 0;
#endif
		/* update status */
		thinkos_rt.th_stat[th] = 0;
	} else { 
		__bit_mem_wr(&thinkos_rt.wq_lst[wq], th, 1);
		__bit_mem_wr(&thinkos_rt.wq_clock, th, tmw);
	}
}
#endif

#if THINKOS_GATE_MAX > 0
void __thinkos_gate_resume(unsigned int th, unsigned int wq, bool tmw) 
{
	unsigned int idx = wq - THINKOS_GATE_BASE;
	int open;
	int lock;
	
	open = __bit_mem_rd(thinkos_rt.gate, idx * 2);
	lock = __bit_mem_rd(thinkos_rt.gate, idx * 2 + 1); 

	DCC_LOG3(LOG_INFO, "PC=%08x open=%d lock=%d...........", 
			 thinkos_rt.ctx[th]->pc, open, lock);

	/* THINKOS_GATE_SIGNALED */
	if (open && !lock) {
		/* close the gate */
		__bit_mem_wr(thinkos_rt.gate, idx * 2, 0);
		/* lock the gate */
		__bit_mem_wr(thinkos_rt.gate, idx * 2 + 1, 1);
		/* insert the thread into ready queue */
		__bit_mem_wr(&thinkos_rt.wq_ready, th, 1);
#if THINKOS_ENABLE_TIMED_CALLS
		/* set the thread's return value */
		thinkos_rt.ctx[th]->r0 = 0;
#endif
		/* update status */
		thinkos_rt.th_stat[th] = 0;
	} else { 
		__bit_mem_wr(&thinkos_rt.wq_lst[wq], th, 1);
		__bit_mem_wr(&thinkos_rt.wq_clock, th, tmw);
	}
}
#endif

#if THINKOS_ENABLE_JOIN
static void join_resume(unsigned int th, unsigned int wq, bool tmw) 
{
	DCC_LOG1(LOG_INFO, "PC=%08x ...........", thinkos_rt.ctx[th]->pc); 

	__bit_mem_wr(&thinkos_rt.wq_lst[wq], th, 1);
}
#endif

#if THINKOS_ENABLE_CONSOLE
void __console_rd_resume(unsigned int th, unsigned int wq, bool tmw);
void __console_wr_resume(unsigned int th, unsigned int wq, bool tmw);
#endif

#if THINKOS_ENABLE_COMM
static void comm_recv_resume(unsigned int th, unsigned int wq, bool tmw) 
{
	DCC_LOG1(LOG_INFO, "PC=%08x ...........", thinkos_rt.ctx[th]->pc); 
	/* wakeup from the comm recv wait queue setting the return value to 0.
	   The calling thread should retry the operation. */
	__thinkos_wakeup_return(wq, th, 0);
}

static void comm_send_resume(unsigned int th, unsigned int wq, bool tmw) 
{
	DCC_LOG1(LOG_INFO, "PC=%08x ...........", thinkos_rt.ctx[th]->pc); 
	/* wakeup from the comm send wait queue setting the return value to 0.
	   The calling thread should retry the operation. */
	__thinkos_wakeup_return(wq, th, 0);
}
#endif

#if THINKOS_ENABLE_JOIN
static void canceled_resume(unsigned int th, unsigned int wq, bool tmw) 
{
	DCC_LOG1(LOG_INFO, "PC=%08x ...........", thinkos_rt.ctx[th]->pc); 

	__bit_mem_wr(&thinkos_rt.wq_lst[wq], th, 1);
}
#endif

#if THINKOS_ENABLE_PAUSE
static void paused_resume(unsigned int th, unsigned int wq, bool tmw) 
{
	DCC_LOG2(LOG_INFO, "invalid state: thread_id=%d PC=%08x !!!!!!",
			 th, thinkos_rt.ctx[th]->pc); 
}
#endif

#if THINKOS_ENABLE_FAULT
static void fault_resume(unsigned int th, unsigned int wq, bool tmw) 
{
	DCC_LOG1(LOG_INFO, "PC=%08x ...........", thinkos_rt.ctx[th]->pc); 

	__bit_mem_wr(&thinkos_rt.wq_lst[wq], th, 1);
}
#endif

static const void * const thread_resume_lut[] = {
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
#if THINKOS_GATE_MAX > 0
	[THINKOS_OBJ_GATE] = __thinkos_gate_resume,
#endif
#if THINKOS_ENABLE_JOIN
	[THINKOS_OBJ_JOIN] = join_resume,
#endif
#if THINKOS_ENABLE_CONSOLE
	[THINKOS_OBJ_CONREAD] = __console_rd_resume,
	[THINKOS_OBJ_CONWRITE] = __console_wr_resume,
#endif
#if THINKOS_ENABLE_PAUSE
	[THINKOS_OBJ_PAUSED] = paused_resume,
#endif
#if THINKOS_ENABLE_JOIN
	[THINKOS_OBJ_CANCELED] = canceled_resume,
#endif
#if THINKOS_ENABLE_FAULT
	[THINKOS_OBJ_FAULT] = fault_resume,
#endif
#if THINKOS_ENABLE_COMM
	[THINKOS_OBJ_COMMSEND] = comm_send_resume,
	[THINKOS_OBJ_COMMRECV] = comm_recv_resume
#endif
};

#endif /* (THINKOS_ENABLE_PAUSE && THINKOS_ENABLE_THREAD_STAT) */

bool __thinkos_thread_pause(unsigned int th)
{
	unsigned int wq;
#if THINKOS_ENABLE_THREAD_STAT
#endif

#if THINKOS_ENABLE_PAUSE
	if (__bit_mem_rd(&thinkos_rt.wq_paused, th) != 0) {
		DCC_LOG1(LOG_INFO, "thread=%d is paused already!", th);
		/* paused */
		return false;
	}

	/* insert into the paused queue */
	__bit_mem_wr(&thinkos_rt.wq_paused, th, 1);
#endif

#if THINKOS_ENABLE_THREAD_STAT
	{
		int stat;
		/* remove the thread from a waiting queue, including ready  */
		stat = thinkos_rt.th_stat[th];
		wq = stat >> 1;
		DCC_LOG4(LOG_INFO, "thread=%d stat=0x%02x wq=%d clk=%d", 
				 th, stat, wq, (stat & 1));
		__bit_mem_wr(&thinkos_rt.wq_lst[wq], th, 0);
#if THINKOS_ENABLE_TIMESHARE
		/* possibly remove from the time share wait queue */
		__bit_mem_wr((uint32_t *)&thinkos_rt.wq_tmshare, th, 0);
#endif
	}
#else
	/* clear all bits on all queues */
	for (wq = 0; wq < THINKOS_WQ_LST_END; ++wq) 
		__bit_mem_wr(&thinkos_rt.wq_lst[wq], th, 0);
#endif /* THINKOS_ENABLE_THREAD_STAT */

#if THINKOS_IRQ_MAX > 0
	{
		int irq;
		for (irq = 0; irq < THINKOS_IRQ_MAX; ++irq) {
			if (thinkos_rt.irq_th[irq] == th) {
				DCC_LOG2(LOG_INFO, "thread=%d IRQ=%d", th, irq);
				/* disable this interrupt source */
				cm3_irq_disable(irq);
				break;
			}
		}
	}
#endif

#if (THINKOS_ENABLE_DEBUG_STEP)
	/* posibly clear the step request */
	__bit_mem_wr(&thinkos_rt.step_req, th, 0);
#endif

#if THINKOS_ENABLE_CLOCK
	/* disable the clock */
	__bit_mem_wr(&thinkos_rt.wq_clock, th, 0);
#endif

	return true;
}

bool __thinkos_thread_resume(unsigned int th)
{
#if THINKOS_ENABLE_PAUSE
	if (__bit_mem_rd(&thinkos_rt.wq_paused, th) == 0) {
		DCC_LOG1(LOG_INFO, "thread=%d is not paused!", th);
		/* not paused, this is not an error condition. */
		return false;
	}

	/* remove from the paused queue */
	__bit_mem_wr(&thinkos_rt.wq_paused, th, 0);  
#endif

#if (THINKOS_ENABLE_PAUSE && THINKOS_ENABLE_THREAD_STAT)
	{
		void (* resume)(unsigned int, unsigned int, bool);
		unsigned int wq;
		bool tmw;
		int stat;
		int type;

		/* reinsert the thread into a waiting queue, including ready  */
		stat = thinkos_rt.th_stat[th];
		wq = stat >> 1;
		tmw = stat & 1;
		type = thinkos_obj_type_lut[wq];
		DCC_LOG4(LOG_INFO, "thread=%d wq=%d clk=%d type=%d", 
				 th, wq, tmw, type);
		resume = thread_resume_lut[type];
		resume(th, wq, tmw);
	}
#else
	DCC_LOG1(LOG_INFO, "thread=%d [ready]", th);
	__bit_mem_wr(&thinkos_rt.wq_ready, th, 1);
#endif /* (THINKOS_ENABLE_PAUSE && THINKOS_ENABLE_THREAD_STAT) */

	return true;
}

#if THINKOS_ENABLE_PAUSE

void thinkos_resume_svc(int32_t * arg)
{
	unsigned int th = arg[0];

#if THINKOS_ENABLE_ARG_CHECK
	if (th >= THINKOS_THREADS_MAX) {
		DCC_LOG1(LOG_INFO, "invalid thread %d!", th);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_THREAD_ALLOC
	if (__bit_mem_rd(thinkos_rt.th_alloc, th) == 0) {
		DCC_LOG1(LOG_INFO, "invalid thread %d!", th);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	if (thinkos_rt.ctx[th] == NULL) {
		DCC_LOG1(LOG_INFO, "invalid thread %d!", th);
		arg[0] = THINKOS_EINVAL;
		return;
	}

	arg[0] = 0;

	if (__thinkos_thread_resume(th))
		__thinkos_defer_sched();
}

void thinkos_pause_svc(int32_t * arg)
{
	unsigned int th = arg[0];

#if THINKOS_ENABLE_ARG_CHECK
	if (th >= THINKOS_THREADS_MAX) {
		DCC_LOG1(LOG_INFO, "invalid thread %d!", th);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#if THINKOS_ENABLE_THREAD_ALLOC
	if (__bit_mem_rd(thinkos_rt.th_alloc, th) == 0) {
		DCC_LOG1(LOG_INFO, "invalid thread %d!", th);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif
#endif

	if (thinkos_rt.ctx[th] == NULL) {
		DCC_LOG1(LOG_INFO, "invalid thread %d!", th);
		arg[0] = THINKOS_EINVAL;
		return;
	}

	arg[0] = 0;

	if (__thinkos_thread_pause(th))
		__thinkos_defer_sched();
}

#endif /* THINKOS_ENABLE_PAUSE */

