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

_Pragma ("GCC optimize (\"Ofast\")")

#define __THINKOS_SYS__
#include <thinkos_sys.h>
#include <thinkos.h>

#if THINKOS_ENABLE_DEBUG_STEP
void __attribute__((naked)) __thinkos_debug_step_i(unsigned int thread_id) 
{
	unsigned int xpsr;
	unsigned int ipsr;

	DCC_LOG(LOG_MSG, "??????? ?????????");

	/* save the current stepping thread */
	thinkos_rt.step_id = thread_id;

	/* get the XPSR from the stack */
	asm volatile ("ldr   %0, [sp, #7 * 4]\n"
				  : "=r" (xpsr) : : ); 
	ipsr = xpsr & 0x1ff;

	if (ipsr == CM3_EXCEPT_PENDSV) {
		/* Clear PendSV active */
		CM3_SCB->shcsr &= ~SCB_SHCSR_PENDSVACT; 
		/* Remove NMI context */
		asm volatile ("pop    {r0-r3,r12,lr}\n"
					  "add    sp, sp, #2 * 4\n"
					  : : : ); 
	}
	/* Step and return */
	asm volatile ("movw   r3, #0xedf0\n"
				  "movt   r3, #0xe000\n"
				  "ldr    r2, [r3, #12]\n"
				  "orr.w  r2, r2, #(1 << 18)\n"
				  "str    r2, [r3, #12]\n"
				  "bx     lr\n"
				  : :  : "r3", "r2"); 
}
#else
void __thinkos_debug_step_i(unsigned int thread_id)
{
}
#endif /* THINKOS_ENABLE_DEBUG_STEP */

#if (THINKOS_SEMAPHORE_MAX  > 0)
#if 0
void ___thinkos_sem_post_i(int sem) 
{
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
#endif
void __thinkos_sem_post_i(int wq) 
{
	unsigned int opc = 0x0000 | wq;
	thinkos_rt.sig.queue[thinkos_rt.sig.head++ % THINKOS_SIG_QUEUE_LEN] = opc;
	__thinkos_defer_svc();
}
#else
void __thinkos_sem_post_i(int sem) 
{
}
#endif /* (THINKOS_SEMAPHORE_MAX > 0) */

#if (THINKOS_EVENT_MAX > 0)
#if 0
void __thinkos_ev_raise_i(int wq, int ev)
{
	unsigned int no = wq - THINKOS_EVENT_BASE;
	int th;

	if ((__bit_mem_rd(&thinkos_rt.ev[no].mask, ev)) &&  
		((th = __thinkos_wq_head(wq)) != THINKOS_THREAD_NULL)) {
		/* wakeup from the event wait queue, set the return of
		 the thread to event */
		DCC_LOG2(LOG_INFO, "wakeup ev=%d.%d", wq, ev);
		__thinkos_wakeup_return(wq, th, ev);
		/* signal the scheduler ... */
		__thinkos_defer_sched();
	} else {
		DCC_LOG2(LOG_INFO, "pending ev=%d.%d", wq, ev);
		/* event is masked or no thread is waiting on the event set, 
		   mark the event as pending */
		__bit_mem_wr(&thinkos_rt.ev[no].pend, ev, 1);  
	}
}
#endif
void __thinkos_ev_raise_i(int wq, int ev)
{
	unsigned int opc = 0x8000 | (ev << 10) | wq;
	thinkos_rt.sig.queue[thinkos_rt.sig.head++ % THINKOS_SIG_QUEUE_LEN] = opc;
	__thinkos_defer_svc();
}

#else
void __thinkos_ev_raise_i(int wq, int ev)
{
}
#endif /* (THINKOS_EVENT_MAX > 0) */

#if (THINKOS_FLAG_MAX > 0)
#if 0
/* wakeup a single thread waiting on the flag 
   OR set the flag */
void __thinkos_flag_give_i(int wq) 
{
	unsigned int flag = wq - THINKOS_FLAG_BASE;
	int th;

	/* get the flag state */
	DCC_LOG1(LOG_INFO, "wq=%d...", wq);
	if (__bit_mem_rd(thinkos_rt.flag.sig, flag) == 0) {
		/* get a thread from the queue */
		if ((th = __thinkos_wq_head(wq)) != THINKOS_THREAD_NULL) {
			__thinkos_wakeup(wq, th);
			/* signal the scheduler ... */
			__thinkos_defer_sched();
		} else {
			/* set the flag bit */
			__bit_mem_wr(thinkos_rt.flag.sig, flag, 1);  
		}
	}
}
#endif
void __thinkos_flag_give_i(int wq) 
{
	unsigned int opc = 0x4000 | (0 << 10) | wq;
	thinkos_rt.sig.queue[thinkos_rt.sig.head++ % THINKOS_SIG_QUEUE_LEN] = opc;
	__thinkos_defer_svc();
}

/* wakeup a single thread waiting on the flag 
   OR set the flag */
#if 0
void __thinkos_flag_signal_i(int wq) 
{
	unsigned int flag = wq - THINKOS_FLAG_BASE;
	int th;
	/* set the flag bit */
	__bit_mem_wr(thinkos_rt.flag.sig, flag, 1);  
#if THINKOS_ENABLE_FLAG_LOCK
	if (!__bit_mem_rd(thinkos_rt.flag.lock, flag)) {
#endif
		/* get a thread from the queue */
		if ((th = __thinkos_wq_head(wq)) != THINKOS_THREAD_NULL) {
#if THINKOS_ENABLE_FLAG_LOCK
			/* lock the flag */
			__bit_mem_wr(thinkos_rt.flag.lock, flag, 1);
#endif
			__thinkos_wakeup(wq, th);

			/* clear the flag bit */
			__bit_mem_wr(thinkos_rt.flag.sig, flag, 0);
			/* signal the scheduler ... */
			__thinkos_defer_sched();
			return;
		} 
#if THINKOS_ENABLE_FLAG_LOCK
	}
#endif
}
#endif
void __thinkos_gate_open_i(int wq) 
{
	unsigned int opc = 0x4000 | (1 << 10) | wq;
	thinkos_rt.sig.queue[thinkos_rt.sig.head++ % THINKOS_SIG_QUEUE_LEN] = opc;
	__thinkos_defer_svc();
}

#if 0
void __thinkos_flag_clr_i(int wq) 
{
	unsigned int flag = wq - THINKOS_FLAG_BASE;
	/* clear the flag signal bit */
	__bit_mem_wr(thinkos_rt.flag.sig, flag, 0);  
}
#endif
void __thinkos_flag_clr_i(int wq) 
{
	unsigned int opc = 0x4000 | (2 << 10) | wq;
	thinkos_rt.sig.queue[thinkos_rt.sig.head++ % THINKOS_SIG_QUEUE_LEN] = opc;
	__thinkos_defer_svc();
}

#if 0
/* set the flag and wakeup all threads waiting on the flag */
void __thinkos_flag_set_i(int wq) 
{
	unsigned int flag = wq - THINKOS_FLAG_BASE;
	int th;

	/* set the flag bit */
	__bit_mem_wr(thinkos_rt.flag.sig, flag, 1);  

	/* get a thread from the queue */
	if ((th = __thinkos_wq_head(wq)) != THINKOS_THREAD_NULL) {
		__thinkos_wakeup(wq, th);
		/* get the remaining threads from the queue */
		while ((th = __thinkos_wq_head(wq)) != THINKOS_THREAD_NULL) {
			__thinkos_wakeup(wq, th);
		}
		/* signal the scheduler ... */
		__thinkos_defer_sched();
	}
}
#endif
void __thinkos_flag_set_i(int wq) 
{
	unsigned int opc = 0x4000 | (3 << 10) | wq;
	thinkos_rt.sig.queue[thinkos_rt.sig.head++ % THINKOS_SIG_QUEUE_LEN] = opc;
	__thinkos_defer_svc();
}

#else
void __thinkos_flag_give_i(int wq) 
{
}

void __thinkos_gate_open_i(int wq) 
{
}

void __thinkos_flag_clr_i(int wq) 
{
}

void __thinkos_flag_set_i(int wq) 
{
}
#endif /* (THINKOS_FLAG_MAX > 0) */

void __attribute__((naked)) cm3_nmi_isr(int arg1, int arg2, int arg3, int arg4)
{
	asm volatile ("add    r12, r12\n"
				  "add    r12, r12, pc\n"
				  "ldr    pc, [r12, #2]\n"
				  ".word  __thinkos_debug_step_i\n"
				  ".word  __thinkos_sem_post_i\n"
				  ".word  __thinkos_ev_raise_i\n"
				  ".word  __thinkos_flag_give_i\n"
				  ".word  __thinkos_gate_open_i\n"
				  ".word  __thinkos_flag_clr_i\n"
				  ".word  __thinkos_flag_set_i\n"
				  : : "r" (arg1), "r" (arg2), "r" (arg3), "r" (arg4) : ); 

}

/* FIXME: this is a hack to force linking this file. 
 The linker then will override the weak alias for the cm3_svc_isr() */
const char thinkos_nmi_nm[] = "NMI";

