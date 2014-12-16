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
__thinkos_ev_wait(int ev) {
	int self = thinkos_rt.active;
	__thinkos_wq_insert(ev, self);
	/* prepare to wait ... */
	__thinkos_wait(self);
	__thinkos_critical_exit();
	/* the scheduler will run at this point */
	__thinkos_critical_enter();
}

static inline void __attribute__((always_inline)) 
__thinkos_critical_ev_wait(int ev, int lvl) {
	int self = thinkos_rt.active;
	__thinkos_wq_insert(ev, self);
	/* prepare to wait ... */
	__thinkos_wait(self);
	__thinkos_critical_exit();
	/* the scheduler will run at this point */
	__thinkos_critical_level(lvl);
}

static inline void __attribute__((always_inline)) __thinkos_ev_raise(int ev) {
	int th;

	if ((th = __thinkos_wq_head(ev)) != THINKOS_THREAD_NULL) {
		/* remove from the ev wait queue */
		__bit_mem_wr(&thinkos_rt.wq_lst[ev], th, 0);
		/* insert the thread into ready queue */
		__bit_mem_wr(&thinkos_rt.wq_ready, th, 1);  
		/* signal the scheduler ... */
		__thinkos_defer_sched();
	}
}

#if THINKOS_ENABLE_TIMED_CALLS
static inline void __attribute__((always_inline)) 
__thinkos_ev_timed_raise(int ev) {
	int th;

	if ((th = __thinkos_wq_head(ev)) != THINKOS_THREAD_NULL) {
		__thinkos_wakeup(ev, th);
		/* signal the scheduler ... */
		__thinkos_defer_sched();
	}
}
#endif

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
	__thinkos_critical_enter();
	/* prepare to wait ... */
	__thinkos_wait(self);
	/* clear pending interrupt */
	cm3_irq_pend_clr(irq);
	/* enable this interrupt source */
	cm3_irq_enable(irq);
	__thinkos_critical_exit();
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

