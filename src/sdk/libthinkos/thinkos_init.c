/* 
 * thikos_core.c
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

_Pragma ("GCC optimize (\"Os\")")

#define __THINKOS_SYS__
#include <thinkos_sys.h>
#define __THINKOS_IRQ__
#include <thinkos_irq.h>
#define __THINKOS_DMON__
#include <thinkos_dmon.h>

#include <thinkos_except.h>

#include <stdio.h>
#include <string.h>

extern const char thinkos_svc_nm[];
extern const char thinkos_nmi_nm[];
extern const struct thinkos_thread_inf thinkos_main_inf;

void __thinkos_reset(void)
{
	struct cm3_systick * systick = CM3_SYSTICK;

	/* adjust exception priorities */
	/*
	 *  0x00 - low latency interrupts
     *
	 *  0x20 - high priority interrupts
	 *  0x40   .
	 *
	 *  0x60 - SVC
	 *  0x80 - regular priority interrupts
	 * 
	 *  0xa0 - SysTick
	 *  0xc0 - PendSV (scheduler)
	 *
	 *  0xe0 - very low priority interrupts
	 */

	/* SVC shuld not be preempted by the scheduler, thus it runs 
	   at higher priority. In order for the regular priority
	   interrupts to call SVC, they shuld run at a lower priority
	   then SVC.*/
	cm3_except_pri_set(CM3_EXCEPT_SVC, SYSCALL_PRIORITY);
//	cm3_except_pri_set(CM3_EXCEPT_SVC, MONITOR_PRIORITY);

	/* SysTick interrupt has to have a lower priority then SVC,
	 to not preempt SVC */
	cm3_except_pri_set(CM3_EXCEPT_SYSTICK, CLOCK_PRIORITY);
	/* PendSV interrupt has to have the lowest priority among
	   regular interrupts (higher number) */
	cm3_except_pri_set(CM3_EXCEPT_PENDSV, SCHED_PRIORITY);

	cm3_except_pri_set(CM3_EXCEPT_MEM_MANAGE, EXCEPT_PRIORITY);
	cm3_except_pri_set(CM3_EXCEPT_BUS_FAULT, EXCEPT_PRIORITY);
	cm3_except_pri_set(CM3_EXCEPT_USAGE_FAULT, EXCEPT_PRIORITY);
	cm3_except_pri_set(CM3_EXCEPT_DEBUG_MONITOR, MONITOR_PRIORITY);

#if THINKOS_ENABLE_FPU 
	CM3_SCB->cpacr |= CP11_SET(3) | CP10_SET(3);
#endif

	/* clear the ThinkOS runtime structure */
	__thinkos_memset32(&thinkos_rt, 0, sizeof(struct thinkos_rt));  

#if	(THINKOS_IRQ_MAX > 0)
	{
		int irq;
		/* adjust IRQ priorities to regular (above SysTick and bellow SVC) */
		for (irq = 0; irq < THINKOS_IRQ_MAX; irq++) {
			cm3_irq_pri_set(irq, IRQ_DEF_PRIORITY);
			thinkos_rt.irq_th[irq] = -1;
		}
	}
#endif

	/* initialize the idle thread */
	thinkos_rt.idle_ctx = &thinkos_idle.ctx;
	thinkos_idle.ctx.pc = (uint32_t)thinkos_idle_task,
	thinkos_idle.ctx.xpsr = 0x01000000;
#if (THINKOS_THREADS_MAX < 32) 
	/* put the IDLE thread in the ready queue */
	__bit_mem_wr(&thinkos_rt.wq_ready, THINKOS_THREADS_MAX, 1);
#endif

#if THINKOS_ENABLE_MUTEX_ALLOC
	/* initialize the mutex allocation bitmap */ 
	__thinkos_bmp_init(thinkos_rt.mutex_alloc, THINKOS_MUTEX_MAX); 
#endif

#if THINKOS_ENABLE_SEM_ALLOC
	/* initialize the semaphore allocation bitmap */ 
	__thinkos_bmp_init(thinkos_rt.sem_alloc, THINKOS_SEMAPHORE_MAX); 
#endif

#if THINKOS_ENABLE_COND_ALLOC
	/* initialize the conditional variable allocation bitmap */ 
	__thinkos_bmp_init(thinkos_rt.cond_alloc, THINKOS_COND_MAX); 
#endif

#if THINKOS_ENABLE_FLAG_ALLOC
	/* initialize the flag allocation bitmap */ 
	__thinkos_bmp_init(thinkos_rt.flag_alloc, THINKOS_FLAG_MAX); 
#endif

#if THINKOS_ENABLE_EVENT_ALLOC
	/* initialize the event set allocation bitmap */ 
	__thinkos_bmp_init(thinkos_rt.ev_alloc, THINKOS_EVENT_MAX); 
#endif

#if (THINKOS_MUTEX_MAX > 0)
	{
		int i;
		/* initialize the mutex locks */
		for (i = 0; i < THINKOS_MUTEX_MAX; i++) 
			thinkos_rt.lock[i] = -1;
	}
#endif

#if THINKOS_EVENT_MAX > 0
	{
		int i;
		/* initialize the event sets */
		for (i = 0; i < THINKOS_EVENT_MAX; i++) {
			thinkos_rt.ev[i].pend = 0; /* no pending events */
			thinkos_rt.ev[i].mask = 0xffffffff; /* all events a are enabled */
		}
	}
#endif /* THINKOS_EVENT_MAX > 0 */

#if THINKOS_ENABLE_DEBUG_STEP
	thinkos_rt.step_id = -1;
	thinkos_rt.break_id = -1;
	thinkos_rt.step_req = 0;
#endif

#if THINKOS_ENABLE_PROFILING
	/* Enable trace */
	CM3_DCB->demcr |= DCB_DEMCR_TRCENA;
	/* Enable cycle counter */
	CM3_DWT->ctrl |= DWT_CTRL_CYCCNTENA;

	/* set the reference to now */
	thinkos_rt.cycref = CM3_DWT->cyccnt;
#endif

	/* initialize the SysTick module */
	systick->rvr = cm3_systick_load_1ms; /* 1ms tick period */
	systick->cvr = 0;
#if THINKOS_ENABLE_CLOCK || THINKOS_ENABLE_TIMESHARE
	systick->csr = SYSTICK_CSR_ENABLE | SYSTICK_CSR_TICKINT;
#else
	systick->csr = SYSTICK_CSR_ENABLE;
#endif

	/* Set the initial thread as an invalid thread,
	   to allow for the scheduler to save the current contex somewre */
	thinkos_rt.active = THINKOS_THREAD_VOID;
}

int __thinkos_init_main(struct thinkos_thread_opt opt)
{
	int self;

	/* initialize the main thread */ 
	/* alloc main thread */
	if (opt.id >= THINKOS_THREADS_MAX)
		opt.id = THINKOS_THREADS_MAX - 1;

#if THINKOS_ENABLE_THREAD_ALLOC
	/* initialize the thread allocation bitmap */ 
	thinkos_rt.th_alloc[0] = (uint32_t)(0xffffffffLL << THINKOS_THREADS_MAX);
	self = thinkos_alloc_lo(thinkos_rt.th_alloc, opt.id);
#else
	self = opt.id;
#endif

#if THINKOS_ENABLE_THREAD_INFO
	thinkos_rt.th_inf[self] = (struct thinkos_thread_inf *)&thinkos_main_inf;
#endif

#if THINKOS_ENABLE_TIMESHARE

#if THINKOS_SCHED_LIMIT_MIN < 1
#error "THINKOS_SCHED_LIMIT_MIN must be at least 1"
#endif

#if THINKOS_SCHED_LIMIT_MAX < THINKOS_SCHED_LIMIT_MIN
#error "THINKOS_SCHED_LIMIT_MAX < THINKOS_SCHED_LIMIT_MIN !!!"
#endif

	if (opt.priority > THINKOS_SCHED_LIMIT_MAX)
		opt.priority = THINKOS_SCHED_LIMIT_MAX;

	thinkos_rt.sched_pri[self] = opt.priority;
	thinkos_rt.sched_val[self] = opt.priority / 2;

	/* set the initial schedule limit */
	thinkos_rt.sched_limit = opt.priority;
	if (thinkos_rt.sched_limit < THINKOS_SCHED_LIMIT_MIN)
		thinkos_rt.sched_limit = THINKOS_SCHED_LIMIT_MIN;
#endif /* THINKOS_ENABLE_TIMESHARE */

	/* set the active thread */
	thinkos_rt.active = self;
	__bit_mem_wr(&thinkos_rt.wq_ready, self, 1);

	DCC_LOG2(LOG_TRACE, "threads_max=%d ready=%08x", 
			 THINKOS_THREADS_MAX, thinkos_rt.wq_ready);

#if THINKOS_ENABLE_PAUSE
	if (opt.paused) {
		/* insert into the paused list */
		__bit_mem_wr(&thinkos_rt.wq_paused, self, 1);
	} 
#endif

	/* Invoke the scheduler */
	__thinkos_defer_sched();

	return self;
}

int thinkos_init(struct thinkos_thread_opt opt)
{
	uint32_t msp;
	int self;

	/* disable interrupts */
	cm3_cpsid_i();

	/* initialize exception stack */
	__thinkos_memset32(thinkos_idle.except_stack, 0xdeadbeef, 
					   sizeof(thinkos_idle.except_stack));

	/* configure the thread stack */
	cm3_psp_set(cm3_sp_get());

	/* configure the main stack */
	msp = (uint32_t)&thinkos_idle.ctx.r0;
	cm3_msp_set(msp);

	/* configure the use of PSP in thread mode */
	cm3_control_set(CONTROL_THREAD_PSP | CONTROL_THREAD_PRIV);

	DCC_LOG2(LOG_TRACE, "msp=0x%08x idle=0x%08x", msp, &thinkos_idle);

#if THINKOS_ENABLE_EXCEPTIONS
	thinkos_exception_init();
#endif

	__thinkos_reset();

	self = __thinkos_init_main(opt);


#if (THINKOS_MUTEX_MAX > 0)
	DCC_LOG3(LOG_TRACE, "    mutex: %2d (%2d .. %2d)", THINKOS_MUTEX_MAX,
			 THINKOS_MUTEX_BASE,
			 THINKOS_MUTEX_BASE + THINKOS_MUTEX_MAX - 1);
#endif
#if (THINKOS_COND_MAX > 0)
	DCC_LOG3(LOG_TRACE, "  condvar: %2d (%2d .. %2d)", THINKOS_COND_MAX,
			 THINKOS_COND_BASE,
			 THINKOS_COND_BASE + THINKOS_COND_MAX - 1);
#endif
#if (THINKOS_SEMAPHORE_MAX > 0)
	DCC_LOG3(LOG_TRACE, "semaphore: %2d (%2d .. %2d)", THINKOS_SEMAPHORE_MAX,
			 THINKOS_SEM_BASE,
			 THINKOS_SEM_BASE + THINKOS_SEMAPHORE_MAX - 1);
#endif
#if (THINKOS_EVENT_MAX > 0)
	DCC_LOG3(LOG_TRACE, "    evset: %2d (%2d .. %2d)", THINKOS_EVENT_MAX,
			 THINKOS_EVENT_BASE,
			 THINKOS_EVENT_BASE + THINKOS_EVENT_MAX - 1);
#endif
#if (THINKOS_FLAG_MAX > 0)
	DCC_LOG3(LOG_TRACE, "     flag: %2d (%2d .. %2d)", THINKOS_FLAG_MAX,
			 THINKOS_FLAG_BASE,
			 THINKOS_FLAG_BASE + THINKOS_FLAG_MAX - 1);
#endif
#if THINKOS_ENABLE_JOIN
	DCC_LOG3(LOG_TRACE, "     join: %2d (%2d .. %2d)", THINKOS_THREADS_MAX,
			 THINKOS_JOIN_BASE,
			 THINKOS_JOIN_BASE + THINKOS_THREADS_MAX - 1);
#endif
#if THINKOS_ENABLE_CONSOLE
	DCC_LOG2(LOG_TRACE, "  console: (wr:%2d rd:%2d)", 
			THINKOS_WQ_CONSOLE_WR,  THINKOS_WQ_CONSOLE_RD); 
#endif
#if THINKOS_ENABLE_PAUSE
	DCC_LOG1(LOG_TRACE, "   paused: (%2d)", THINKOS_WQ_PAUSED); 
#endif
#if THINKOS_ENABLE_JOIN
	DCC_LOG1(LOG_TRACE, " canceled: (%2d)", THINKOS_WQ_CANCELED); 
#endif
#if THINKOS_ENABLE_DEBUG_FAULT
	DCC_LOG1(LOG_TRACE, "    fault: (%2d)", THINKOS_WQ_FAULT); 
#endif

	DCC_LOG(LOG_TRACE, "enabling interrupts!");

	/* enable interrupts */
	cm3_cpsie_i();

	DCC_LOG4(LOG_TRACE, "<%d> msp=%08x psp=%08x ctrl=%08x", 
			 self, cm3_msp_get(), cm3_psp_get(), cm3_control_get());

	return self;
}


const char * const thinkos_svc_link = thinkos_svc_nm;
const char * const thinkos_nmic_link = thinkos_nmi_nm;

