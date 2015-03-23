/* 
 * thikos_irq.h
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

#ifndef __THINKOS_IRQ_H__
#define __THINKOS_IRQ_H__

#ifndef __THINKOS_IRQ__
#error "Only use this file on privileged code "
#endif 

#define __THINKOS_SYS__
#include <thinkos_sys.h>

#ifndef __ASSEMBLER__

#define SYSCALL_PRIORITY       0x60
#define CLOCK_PRIORITY         0xa0
#define SCHED_PRIORITY         0xff
#define EXCEPT_PRIORITY        0x00

#define IRQ_PRIORITY_HIGHEST   (0 << 5)
#define IRQ_PRIORITY_VERY_HIGH (1 << 5)
#define IRQ_PRIORITY_HIGH      (2 << 5)
#define IRQ_PRIORITY_SYSCALL   (3 << 5)
#define IRQ_PRIORITY_REGULAR   (4 << 5)
#define IRQ_PRIORITY_CLOCK     (5 << 5)
#define IRQ_PRIORITY_SCHED     (6 << 5)
#define IRQ_PRIORITY_LOW       (7 << 5)

#ifdef __cplusplus
extern "C" {
#endif

#if (THINKOS_EVENT_MAX > 0)

static inline void __attribute__((always_inline)) 
	__thinkos_ev_raise(int wq, int ev)
{
	unsigned int no = wq - THINKOS_EVENT_BASE;
	uint32_t pri;
	int th;

	pri = cm3_primask_get();
	cm3_primask_set(1);
	if ((__bit_mem_rd(&thinkos_rt.ev[no].mask, ev)) &&  
		((th = __thinkos_wq_head(wq)) != THINKOS_THREAD_NULL)) {
		/* wakeup from the event wait queue, set the return of
		 the thread to the event */
		__thinkos_wakeup_return(wq, th, ev);
		/* signal the scheduler ... */
		__thinkos_defer_sched();
	} else {
		/* event is maksed or no thread is waiting ont hte event set
		   , set the event as pending */
		__bit_mem_wr(&thinkos_rt.ev[no].pend, ev, 1);  
	}
	cm3_primask_set(pri);
}

static inline void thinkos_ev_raise_i(int wq, int ev) {
	__thinkos_ev_raise(wq, ev);
}

#endif /* (THINKOS_EVENT_MAX > 0) */


#if (THINKOS_FLAG_MAX > 0)

static inline void __attribute__((always_inline)) 
__thinkos_flag_clr(int wq) {
	/* clear the flag bit */
	__bit_mem_wr(&thinkos_rt.flag, wq - THINKOS_FLAG_BASE, 0);  
}

/* set the flag and wakeup all threads waiting on the flag */
static inline void __attribute__((always_inline)) 
	__thinkos_flag_set(int flag) {
	uint32_t pri;
	int th;

	pri = cm3_primask_get();
	cm3_primask_set(1);
	/* set the flag bit */
	__bit_mem_wr(&thinkos_rt.flag, flag - THINKOS_FLAG_BASE, 1);  

	/* get a thread from the queue */
	if ((th = __thinkos_wq_head(flag)) != THINKOS_THREAD_NULL) {
		__thinkos_wakeup(flag, th);
		while ((th = __thinkos_wq_head(flag)) != THINKOS_THREAD_NULL) {
			__thinkos_wakeup(flag, th);
		}
		/* signal the scheduler ... */
		__thinkos_defer_sched();
	}
	cm3_primask_set(pri);
}

static inline unsigned int __attribute__((always_inline)) 
	__thinkos_flag_is_set(int wq) {
	/* get the flag state */
	return 	__bit_mem_rd(&thinkos_rt.flag, wq - THINKOS_FLAG_BASE);  
}

/* wakeup a single thread waiting on the flag 
   OR set the flag */
static inline void __attribute__((always_inline)) 
__thinkos_flag_give(int flag) {
	uint32_t pri;
	int th;

	pri = cm3_primask_get();
	cm3_primask_set(1);
	/* get the flag state */
	if (__bit_mem_rd(&thinkos_rt.flag, flag - THINKOS_FLAG_BASE) == 0) {
		/* get a thread from the queue */
		if ((th = __thinkos_wq_head(flag)) != THINKOS_THREAD_NULL) {
			__thinkos_wakeup(flag, th);
			/* signal the scheduler ... */
			__thinkos_defer_sched();
		} else {
			/* set the flag bit */
			__bit_mem_wr(&thinkos_rt.flag, flag - THINKOS_FLAG_BASE, 1);  
		}
	}
	cm3_primask_set(pri);
}

static inline void thinkos_flag_clr_i(int flag) {
	__thinkos_flag_clr(flag);
}

static inline void thinkos_flag_set_i(int flag) {
	__thinkos_flag_set(flag);
}

static inline void thinkos_flag_give_i(int flag) {
	__thinkos_flag_give(flag);
}


#endif /* (THINKOS_FLAG_MAX > 0) */


#if (THINKOS_SEMAPHORE_MAX  > 0)

static inline void __attribute__((always_inline)) 
__thinkos_sem_post(int sem) {
	uint32_t pri;
	int th;

	pri = cm3_primask_get();
	cm3_primask_set(1);
	if ((th = __thinkos_wq_head(sem)) == THINKOS_THREAD_NULL) {
		/* no threads waiting on the semaphore, increment. */ 
		thinkos_rt.sem_val[sem - THINKOS_SEM_BASE]++;
		cm3_primask_set(pri);
	} else {
		/* wakeup from the sem wait queue */
		__thinkos_wakeup(sem, th);
		/* signal the scheduler ... */
		__thinkos_defer_sched();
	}
	cm3_primask_set(pri);
}


static inline void thinkos_sem_post_i(int sem) {
	__thinkos_sem_post(sem);
}

#endif /* (THINKOS_SEMAPHORE_MAX > 0) */


/* set the interrupt priority */
static inline void __attribute__((always_inline)) 
__thinkos_irq_pri_set(unsigned int irq, unsigned int priority) {
	cm3_irq_pri_set(irq, priority);
}

/* enable interrupts */
static inline void __attribute__((always_inline)) 
__thinkos_irq_enable(unsigned int irq) {
	cm3_irq_enable(irq);
}

#if (THINKOS_IRQ_MAX  > 0)

static inline void __attribute__((always_inline)) 
__thinkos_irq_wait(int irq) {
	int32_t self = thinkos_rt.active;
	/* store the thread info */
	thinkos_rt.irq_th[irq] = self;
	/* rise the BASEPRI to stop the scheduler */
	cm3_basepri_set(SCHED_PRIORITY); 
	/* prepare to wait ... */
	__thinkos_wait(self);
	/* clear pending interrupt */
	cm3_irq_pend_clr(irq);
	/* enable this interrupt source */
	cm3_irq_enable(irq);
	/* return the BASEPRI to the default to reenable the scheduler. */
	cm3_basepri_set(0x00);
	/* the scheduler will run at this point */
}

#endif /* THINKOS_IRQ_MAX  > 0 */

static inline void 
__attribute__((always_inline)) thinkos_critical_enter(void)  {
	/* rise the BASEPRI to stop the scheduler */
	cm3_basepri_set(SCHED_PRIORITY); 
}

static inline void 
__attribute__((always_inline)) thinkos_critical_level(int lvl)  {
	/* set the BASEPRI */
	cm3_basepri_set(lvl); 
}

static inline void 
__attribute__((always_inline)) thinkos_critical_exit(void)  {
	/* return the BASEPRI to the default to reenable the scheduler. */
	cm3_basepri_set(0x00);
}

void thinkos_flag_give_i(int flag);
void thinkos_flag_set_i(int flag); 
void thinkos_flag_clr_i(int flag); 

void thinkos_sem_post_i(int sem);

#ifdef __cplusplus
}
#endif

#endif /* __ASSEMBLER__ */

#endif /* __THINKOS_IRQ_H__ */

