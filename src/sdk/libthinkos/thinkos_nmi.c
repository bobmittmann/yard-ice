/* 
 * thikos_svc.c
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
#include <thinkos_svc.h>

#if (THINKOS_SEMAPHORE_MAX  > 0)
void __thinkos_sem_post_i(int sem) {
	int th;

	if ((th = __thinkos_wq_head(sem)) == THINKOS_THREAD_NULL) {
		/* no threads waiting on the semaphore, increment. */ 
		thinkos_rt.sem_val[sem - THINKOS_SEM_BASE]++;
	} else {
		/* wakeup from the sem wait queue */
		__thinkos_wakeup(sem, th);
		/* signal the scheduler ... */
		__thinkos_defer_sched();
	}
}
#endif /* (THINKOS_SEMAPHORE_MAX > 0) */

#if (THINKOS_EVENT_MAX > 0)
void __thinkos_ev_raise_i(int wq, int ev)
{
	unsigned int no = wq - THINKOS_EVENT_BASE;
	int th;

	if ((__bit_mem_rd(&thinkos_rt.ev[no].mask, ev)) &&  
		((th = __thinkos_wq_head(wq)) != THINKOS_THREAD_NULL)) {
		/* wakeup from the event wait queue, set the return of
		 the thread to event */
		__thinkos_wakeup_return(wq, th, ev);
		/* signal the scheduler ... */
		__thinkos_defer_sched();
	} else {
		/* event is masked or no thread is waiting ont the event set, 
		   mark the event as pending */
		__bit_mem_wr(&thinkos_rt.ev[no].pend, ev, 1);  
	}
}
#endif /* (THINKOS_EVENT_MAX > 0) */

#if (THINKOS_FLAG_MAX > 0)
/* wakeup a single thread waiting on the flag 
   OR set the flag */
void __thinkos_flag_give_i(int flag) {
	int th;
	/* get the flag state */
	if (__bit_mem_rd(thinkos_rt.flag.sig, flag - THINKOS_FLAG_BASE) == 0) {
		/* get a thread from the queue */
		if ((th = __thinkos_wq_head(flag)) != THINKOS_THREAD_NULL) {
			__thinkos_wakeup(flag, th);
			/* signal the scheduler ... */
			__thinkos_defer_sched();
		} else {
			/* set the flag bit */
			__bit_mem_wr(thinkos_rt.flag.sig, flag - THINKOS_FLAG_BASE, 1);  
		}
	}
}

/* wakeup a single thread waiting on the flag 
   OR set the flag */
void __thinkos_flag_signal_i(int flag) {
	int th;
	/* set the flag bit */
	__bit_mem_wr(thinkos_rt.flag.sig, flag - THINKOS_FLAG_BASE, 1);  
#if THINKOS_ENABLE_FLAG_LOCK
	if (!__bit_mem_rd(thinkos_rt.flag.lock, flag - THINKOS_FLAG_BASE)) {
#endif
		/* get a thread from the queue */
		if ((th = __thinkos_wq_head(flag)) != THINKOS_THREAD_NULL) {
#if THINKOS_ENABLE_FLAG_LOCK
			/* lock the flag */
			__bit_mem_wr(thinkos_rt.flag.lock, flag - THINKOS_FLAG_BASE, 1);
#endif
			__thinkos_wakeup(flag, th);

			/* clear the flag bit */
			__bit_mem_wr(thinkos_rt.flag.sig, flag - THINKOS_FLAG_BASE, 0);
			/* signal the scheduler ... */
			__thinkos_defer_sched();
			return;
		} 
#if THINKOS_ENABLE_FLAG_LOCK
	}
#endif
}

void __thinkos_flag_clr_i(int flag) {
	/* clear the flag signal bit */
	__bit_mem_wr(thinkos_rt.flag.sig, flag - THINKOS_FLAG_BASE, 0);  
}

/* set the flag and wakeup all threads waiting on the flag */
void __thinkos_flag_set_i(int flag) {
	int th;

	/* set the flag bit */
	__bit_mem_wr(thinkos_rt.flag.sig, flag - THINKOS_FLAG_BASE, 1);  

	/* get a thread from the queue */
	if ((th = __thinkos_wq_head(flag)) != THINKOS_THREAD_NULL) {
		__thinkos_wakeup(flag, th);
		while ((th = __thinkos_wq_head(flag)) != THINKOS_THREAD_NULL) {
			__thinkos_wakeup(flag, th);
		}
		/* signal the scheduler ... */
		__thinkos_defer_sched();
	}
}

#endif /* (THINKOS_FLAG_MAX > 0) */

void cm3_nmi_isr(int arg1, int arg2, int svc)
{
	switch (svc) {
	case THINKOS_SEM_POST_I:
#if THINKOS_SEMAPHORE_MAX > 0
		__thinkos_sem_post_i(arg1);
#endif /* THINKOS_SEMAPHORE_MAX > 0 */
		break;

	case THINKOS_EV_RAISE_I:
#if (THINKOS_EVENT_MAX > 0)
		__thinkos_ev_raise_i(arg1, arg2);
#endif /* (THINKOS_EVENT_MAX > 0) */
		break;

	case THINKOS_FLAG_GIVE_I:
#if (THINKOS_FLAG_MAX > 0)
		__thinkos_flag_give_i(arg1);
#endif /* (THINKOS_FLAG_MAX > 0) */
		break;

	case THINKOS_FLAG_SIGNAL_I:
#if (THINKOS_FLAG_MAX > 0)
		__thinkos_flag_signal_i(arg1);
#endif /* (THINKOS_FLAG_MAX > 0) */
		break;

	case THINKOS_FLAG_CLR_I:
#if (THINKOS_FLAG_MAX > 0)
		__thinkos_flag_clr_i(arg1);
#endif /* (THINKOS_FLAG_MAX > 0) */
		break;

	case THINKOS_FLAG_SET_I:
#if (THINKOS_FLAG_MAX > 0)
		__thinkos_flag_set_i(arg1);
#endif /* (THINKOS_FLAG_MAX > 0) */
		break;
	}
}

/* FIXME: this is a hack to force linking this file. 
 The linker then will override the weak alias for the cm3_svc_isr() */
const char thinkos_nmi_nm[] = "NMI";

