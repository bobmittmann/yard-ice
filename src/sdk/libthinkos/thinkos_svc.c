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

void thinkos_thread_create_svc(int32_t * arg);

void thinkos_pause_svc(int32_t * arg);

void thinkos_resume_svc(int32_t * arg);

void thinkos_join_svc(int32_t * arg);

void thinkos_cancel_svc(int32_t * arg);

void thinkos_exit_svc(int32_t * arg);

void thinkos_sleep_svc(int32_t * arg);

void thinkos_alarm_svc(int32_t * arg);

void thinkos_mutex_alloc_svc(int32_t * arg);

void thinkos_mutex_free_svc(int32_t * arg);

void thinkos_mutex_lock_svc(int32_t * arg);

void thinkos_mutex_trylock_svc(int32_t * arg);

void thinkos_mutex_timedlock_svc(int32_t * arg);

void thinkos_mutex_unlock_svc(int32_t * arg);


void thinkos_cond_alloc_svc(int32_t * arg);

void thinkos_cond_free_svc(int32_t * arg);

void thinkos_cond_wait_svc(int32_t * arg);

void thinkos_cond_timedwait_svc(int32_t * arg);

void thinkos_cond_signal_svc(int32_t * arg);

void thinkos_cond_broadcast_svc(int32_t * arg);


void thinkos_sem_alloc_svc(int32_t * arg);

void thinkos_sem_free_svc(int32_t * arg);

void thinkos_sem_init_svc(int32_t * arg);

void thinkos_sem_wait_svc(int32_t * arg);

void thinkos_sem_trywait_svc(int32_t * arg);

void thinkos_sem_timedwait_svc(int32_t * arg);

void thinkos_sem_post_svc(int32_t * arg);


void thinkos_ev_alloc_svc(int32_t * arg);

void thinkos_ev_free_svc(int32_t * arg);

void thinkos_ev_wait_svc(int32_t * arg);

void thinkos_ev_timedwait_svc(int32_t * arg);

void thinkos_ev_raise_svc(int32_t * arg);

void thinkos_ev_mask_svc(int32_t * arg);

void thinkos_ev_unmask_svc(int32_t * arg);


void thinkos_flag_alloc_svc(int32_t * arg);

void thinkos_flag_free_svc(int32_t * arg);


void thinkos_flag_val_svc(int32_t * arg);

void thinkos_flag_set_svc(int32_t * arg);

void thinkos_flag_clr_svc(int32_t * arg);

void thinkos_flag_watch_svc(int32_t * arg);

void thinkos_flag_timedwatch_svc(int32_t * arg);


void thinkos_flag_give_svc(int32_t * arg);

void thinkos_flag_take_svc(int32_t * arg);

void thinkos_flag_timedtake_svc(int32_t * arg);


void thinkos_flag_release_svc(int32_t * arg);

void thinkos_flag_wait_svc(int32_t * arg);

void thinkos_flag_timedwait_svc(int32_t * arg);


void thinkos_irq_wait_svc(int32_t * arg);

void thinkos_irq_register_svc(int32_t * arg);

void thinkos_rt_snapshot_svc(int32_t * arg);

void thinkos_console_svc(int32_t * arg);

void thinkos_nosys(int32_t * arg)
{
	arg[0] = THINKOS_ENOSYS;
}

void thinkos_clock_svc(int32_t * arg)
{
	arg[0] = thinkos_rt.ticks;
}

void thinkos_thread_self_svc(int32_t * arg)
{
	arg[0] = thinkos_rt.active;
}

static inline uint32_t __attribute__((always_inline)) cm3_svc_stackframe(void) {
	register uint32_t sp;
	asm volatile ("tst lr, #4\n" 
				  "ite eq\n" 
				  "mrseq %0, MSP\n" 
				  "mrsne %0, PSP\n" 
				  : "=r" (sp));
	return sp;
}

void cm3_svc_isr(void)
{
	int32_t * arg;
	uint8_t * pc;
	int svc;

	/* get a pointer to the caller's stack */
#if THINKOS_ENABLE_IRQ_SVC_CALL
	arg = (int32_t * )cm3_svc_stackframe();
#else
	arg = (int32_t * )cm3_psp_get();
#endif

	/* get PC value */
	pc = (uint8_t *)arg[6];
	/* get the immediate data from instruction */
	svc = pc[-2];

	DCC_LOG1(LOG_MSG, "pc=0x%08x", pc);

	switch (svc) {
	case THINKOS_THREAD_SELF:
		thinkos_thread_self_svc(arg);
		break;

	case THINKOS_THREAD_CREATE:
		thinkos_thread_create_svc(arg);
		break;

/* ----------------------------------------------
 * Clock related
 * --------------------------------------------- */

	case THINKOS_CLOCK:
#if THINKOS_ENABLE_CLOCK
		thinkos_clock_svc(arg);
#else
		thinkos_nosys(arg);
#endif
		break;

	case THINKOS_SLEEP:
#if THINKOS_ENABLE_SLEEP
		thinkos_sleep_svc(arg);
#else
		thinkos_nosys(arg);
#endif
		break;

	case THINKOS_ALARM:
#if THINKOS_ENABLE_ALARM
		thinkos_alarm_svc(arg);
#else
		thinkos_nosys(arg);
#endif
		break;
		
/* ----------------------------------------------
 * Mutex
 * --------------------------------------------- */

	case THINKOS_MUTEX_LOCK:
#if THINKOS_MUTEX_MAX > 0
		thinkos_mutex_lock_svc(arg);
#else
		thinkos_nosys(arg);
#endif
		break;

	case THINKOS_MUTEX_TRYLOCK:
#if THINKOS_MUTEX_MAX > 0
		thinkos_mutex_trylock_svc(arg);
#else
		thinkos_nosys(arg);
#endif
		break;

	case THINKOS_MUTEX_TIMEDLOCK:
#if THINKOS_MUTEX_MAX > 0 && THINKOS_ENABLE_TIMED_CALLS
		thinkos_mutex_timedlock_svc(arg);
#else
		thinkos_nosys(arg);
#endif
		break;

	case THINKOS_MUTEX_UNLOCK:
#if THINKOS_MUTEX_MAX > 0
		thinkos_mutex_unlock_svc(arg);
#else
		thinkos_nosys(arg);
#endif
		break;

/* ----------------------------------------------
 * Semaphores
 * --------------------------------------------- */

	case THINKOS_SEM_INIT:
#if THINKOS_SEMAPHORE_MAX > 0
		thinkos_sem_init_svc(arg);
#else
		thinkos_nosys(arg);
#endif
		break;

	case THINKOS_SEM_WAIT:
#if THINKOS_SEMAPHORE_MAX > 0
		thinkos_sem_wait_svc(arg);
#else
		thinkos_nosys(arg);
#endif
		break;

	case THINKOS_SEM_TRYWAIT:
#if THINKOS_SEMAPHORE_MAX > 0
		thinkos_sem_trywait_svc(arg);
#else
		thinkos_nosys(arg);
#endif
		break;

	case THINKOS_SEM_TIMEDWAIT:
#if THINKOS_SEMAPHORE_MAX > 0 && THINKOS_ENABLE_TIMED_CALLS
		thinkos_sem_timedwait_svc(arg);
#else
		thinkos_nosys(arg);
#endif
		break;

	case THINKOS_SEM_POST:
#if THINKOS_SEMAPHORE_MAX > 0
		thinkos_sem_post_svc(arg);
#else
		thinkos_nosys(arg);
#endif
		break;


/* ----------------------------------------------
 * Conditional Variables
 * --------------------------------------------- */

	case THINKOS_COND_WAIT:
#if THINKOS_COND_MAX > 0
		thinkos_cond_wait_svc(arg);
#else
		thinkos_nosys(arg);
#endif
		break;

	case THINKOS_COND_TIMEDWAIT:
#if THINKOS_COND_MAX > 0 && THINKOS_ENABLE_TIMED_CALLS
		thinkos_cond_timedwait_svc(arg);
#else
		thinkos_nosys(arg);
#endif
		break;

	case THINKOS_COND_SIGNAL:
#if THINKOS_COND_MAX > 0
		thinkos_cond_signal_svc(arg);
#else
		thinkos_nosys(arg);
#endif
		break;

	case THINKOS_COND_BROADCAST:
#if THINKOS_COND_MAX > 0
		thinkos_cond_broadcast_svc(arg);
#else
		thinkos_nosys(arg);
#endif
		break;

/* ----------------------------------------------
 * Single (simple) flags
 * --------------------------------------------- */

	case THINKOS_FLAG_TAKE:
#if (THINKOS_FLAG_MAX > 0)
		thinkos_flag_take_svc(arg);
#else
		thinkos_nosys(arg);
#endif
		break;

	case THINKOS_FLAG_TIMEDTAKE:
#if (THINKOS_FLAG_MAX > 0) && THINKOS_ENABLE_TIMED_CALLS
		thinkos_flag_timedtake_svc(arg);
#else
		thinkos_nosys(arg);
#endif
		break;

	case THINKOS_FLAG_GIVE:
#if (THINKOS_FLAG_MAX > 0)
		thinkos_flag_give_svc(arg);
#else
		thinkos_nosys(arg);
#endif
		break;



	case THINKOS_FLAG_WAIT:
#if (THINKOS_FLAG_MAX > 0) && THINKOS_ENABLE_FLAG_LOCK
		thinkos_flag_wait_svc(arg);
#else
		thinkos_nosys(arg);
#endif
		break;

	case THINKOS_FLAG_TIMEDWAIT:
#if (THINKOS_FLAG_MAX > 0) && THINKOS_ENABLE_FLAG_LOCK && \
		THINKOS_ENABLE_TIMED_CALLS
		thinkos_flag_timedwait_svc(arg);
#else
		thinkos_nosys(arg);
#endif
		break;

	case THINKOS_FLAG_RELEASE:
#if (THINKOS_FLAG_MAX > 0) && THINKOS_ENABLE_FLAG_LOCK
		thinkos_flag_release_svc(arg);
#else
		thinkos_nosys(arg);
#endif
		break;


	case THINKOS_FLAG_VAL:
#if (THINKOS_FLAG_MAX > 0) && THINKOS_ENABLE_FLAG_WATCH
		thinkos_flag_val_svc(arg);
#else
		thinkos_nosys(arg);
#endif
		break;

	case THINKOS_FLAG_CLR:
#if (THINKOS_FLAG_MAX > 0) && THINKOS_ENABLE_FLAG_WATCH
		thinkos_flag_clr_svc(arg);
#else
		thinkos_nosys(arg);
#endif
		break;

	case THINKOS_FLAG_SET:
#if (THINKOS_FLAG_MAX > 0) && THINKOS_ENABLE_FLAG_WATCH
		thinkos_flag_set_svc(arg);
#else
		thinkos_nosys(arg);
#endif
		break;

	case THINKOS_FLAG_WATCH:
#if (THINKOS_FLAG_MAX > 0) && THINKOS_ENABLE_FLAG_WATCH
		thinkos_flag_watch_svc(arg);
#else
		thinkos_nosys(arg);
#endif
		break;

	case THINKOS_FLAG_TIMEDWATCH:
#if (THINKOS_FLAG_MAX > 0) && THINKOS_ENABLE_FLAG_WATCH && \
		THINKOS_ENABLE_TIMED_CALLS
		thinkos_flag_timedwatch_svc(arg);
#else
		thinkos_nosys(arg);
#endif
		break;

/* ----------------------------------------------
 * Event sets (multiple flags)
 * --------------------------------------------- */

	case THINKOS_EVENT_WAIT:
#if (THINKOS_EVENT_MAX > 0)
		thinkos_ev_wait_svc(arg);
#else
		thinkos_nosys(arg);
#endif
		break;

	case THINKOS_EVENT_TIMEDWAIT:
#if (THINKOS_EVENT_MAX > 0) && THINKOS_ENABLE_TIMED_CALLS
		thinkos_ev_timedwait_svc(arg);
#else
		thinkos_nosys(arg);
#endif
		break;

	case THINKOS_EVENT_RAISE:
#if (THINKOS_EVENT_MAX > 0)
		thinkos_ev_raise_svc(arg);
#else
		thinkos_nosys(arg);
#endif
		break;

	case THINKOS_EVENT_MASK:
#if (THINKOS_EVENT_MAX > 0)
		thinkos_ev_mask_svc(arg);
#else
		thinkos_nosys(arg);
#endif
		break;

	case THINKOS_EVENT_UNMASK:
#if (THINKOS_EVENT_MAX > 0)
		thinkos_ev_unmask_svc(arg);
#else
		thinkos_nosys(arg);
#endif
		break;

/* ----------------------------------------------
 * Interrupts
 * --------------------------------------------- */

	case THINKOS_IRQ_WAIT:
#if THINKOS_IRQ_MAX > 0
		thinkos_irq_wait_svc(arg);
#else
		thinkos_nosys(arg);
#endif /* THINKOS_IRQ_MAX > 0 */
		break;

	case THINKOS_IRQ_REGISTER:
#if CM3_RAM_VECTORS
		thinkos_irq_register_svc(arg);
#else
		thinkos_nosys(arg);
#endif /* CM3_RAM_VECTORS */
		break;

/* ----------------------------------------------
 * Dynamic Resource Allocation 
 * --------------------------------------------- */

#if THINKOS_MUTEX_MAX > 0
#if THINKOS_ENABLE_MUTEX_ALLOC
	case THINKOS_MUTEX_ALLOC:
		thinkos_mutex_alloc_svc(arg);
		break;

	case THINKOS_MUTEX_FREE:
		thinkos_mutex_free_svc(arg);
		break;
#endif /* THINKOS_MUTEX_ALLOC */
#endif /* THINKOS_MUTEX_MAX > 0 */

#if THINKOS_SEMAPHORE_MAX > 0
#if THINKOS_ENABLE_SEM_ALLOC
	case THINKOS_SEM_ALLOC:
		thinkos_sem_alloc_svc(arg);
		break;

	case THINKOS_SEM_FREE:
		thinkos_sem_free_svc(arg);
		break;
#endif /* THINKOS_ENABLE_SEM_ALLOC */
#endif /* THINKOS_SEMAPHORE_MAX > 0 */

#if (THINKOS_EVENT_MAX > 0)
#if THINKOS_ENABLE_EVENT_ALLOC
	case THINKOS_EVENT_ALLOC:
		thinkos_ev_alloc_svc(arg);
		break;

	case THINKOS_EVENT_FREE:
		thinkos_ev_free_svc(arg);
		break;
#endif
#endif /* (THINKOS_EVENT_MAX > 0) */

#if (THINKOS_FLAG_MAX > 0)
#if THINKOS_ENABLE_FLAG_ALLOC
	case THINKOS_FLAG_ALLOC:
		thinkos_flag_alloc_svc(arg);
		break;

	case THINKOS_FLAG_FREE:
		thinkos_flag_free_svc(arg);
		break;
#endif
#endif /* (THINKOS_FLAG_MAX > 0) */



#if THINKOS_COND_MAX > 0
#if THINKOS_ENABLE_COND_ALLOC
	case THINKOS_COND_ALLOC:
		thinkos_cond_alloc_svc(arg);
		break;

	case THINKOS_COND_FREE:
		thinkos_cond_free_svc(arg);
		break;
#endif /* THINKOS_COND_ALLOC */
#endif /* THINKOS_COND_MAX > 0 */


	case THINKOS_EXIT:
#if THINKOS_ENABLE_EXIT
		thinkos_exit_svc(arg);
#else
		thinkos_nosys(arg);
#endif
		break;

	case THINKOS_RT_SNAPSHOT:
#if THINKOS_ENABLE_RT_DEBUG
		thinkos_rt_snapshot_svc(arg);
#else
		thinkos_nosys(arg);
#endif
		break;

	case THINKOS_PAUSE:
#if THINKOS_ENABLE_PAUSE
		thinkos_pause_svc(arg);
#else
		thinkos_nosys(arg);
#endif
		break;

	case THINKOS_RESUME:
#if THINKOS_ENABLE_PAUSE
		thinkos_resume_svc(arg);
#else
		thinkos_nosys(arg);
#endif
		break;

	case THINKOS_CANCEL:
#if THINKOS_ENABLE_CANCEL
		thinkos_cancel_svc(arg);
#else
		thinkos_nosys(arg);
#endif
		break;

	case THINKOS_JOIN:
#if THINKOS_ENABLE_JOIN
		thinkos_join_svc(arg);
#else
		thinkos_nosys(arg);
#endif
		break;

	case THINKOS_CONSOLE:
#if THINKOS_ENABLE_CONSOLE
		thinkos_console_svc(arg);
#else
		thinkos_nosys(arg);
#endif
		break;
	default:
		thinkos_nosys(arg);
		break;
	}
}

/* FIXME: this is a hack to force linking this file. 
 The linker then will override the weak alias for the cm3_svc_isr() */
const char thinkos_svc_nm[] = "SVC";

