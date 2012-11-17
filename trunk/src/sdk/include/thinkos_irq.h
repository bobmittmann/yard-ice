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
#define SCHED_PRIORITY         0xc0

#define IRQ_PRIORITY_HIGHEST   0x00
#define IRQ_PRIORITY_VERY_HIGH 0x20
#define IRQ_PRIORITY_HIGH      0x40
#define IRQ_PRIORITY_REGULAR   0x80
#define IRQ_PRIORITY_LOW       0xe0

#ifdef __cplusplus
extern "C" {
#endif

static inline void __thinkos_critical_enter(void)  {
	/* rise the BASEPRI to stop the scheduler */
	cm3_basepri_set(SCHED_PRIORITY); 
}

static inline void __thinkos_critical_exit(void)  {
	/* return the BASEPRI to the default to reenable the scheduler. */
	cm3_basepri_set(0x00);
}

#if THINKOS_ENABLE_EVENT_ALLOC
static inline int __thinkos_ev_alloc(void) {
	return thinkos_alloc_lo(&thinkos_rt.ev_alloc, 0);
}

static inline void __thinkos_ev_free(int ev)
{
	__bit_mem_wr(&thinkos_rt.ev_alloc, ev, 0);
}
#endif

static inline void __thinkos_ev_wait(int ev) {
	int self = thinkos_rt.active;
	__thinkos_wq_insert(ev, self);
	/* prepare to wait ... */
	__thinkos_wait();
	__thinkos_critical_exit();
	/* the scheduler will run at this point */
	__thinkos_critical_enter();
}

static inline void __thinkos_ev_raise(int ev) {
	int th;

	if ((th = __thinkos_wq_head(ev)) != THINKOS_THREAD_NULL) {
#if THINKOS_ENABLE_THREAD_STAT
		/* update status */
		thinkos_rt.th_stat[th] = 0;
#endif
		/* remove from the ev wait queue */
		__bit_mem_wr(&thinkos_rt.wq_lst[ev], th, 0);  
		/* insert the thread into ready queue */
		__bit_mem_wr(&thinkos_rt.wq_ready, th, 1);  
		/* signal the scheduler ... */
		__thinkos_defer_sched();
	}
}

/* set the interrupt priority */
static inline void __thinkos_irq_pri_set(unsigned int irq, 
										 unsigned int priority) {
	cm3_irq_pri_set(irq, priority);
}

/* enable interrupts */
static inline void __thinkos_irq_enable(unsigned int irq) {
	cm3_irq_enable(irq);
}

static inline void __thinkos_irq_wait(int irq) {
	int32_t self = thinkos_rt.active;
	/* store the thread info */
	thinkos_rt.irq_th[irq] = self;
	__thinkos_critical_enter();
	/* prepare to wait ... */
	__thinkos_wait();
	/* enable this interrupt source */
	cm3_irq_enable(irq);
	__thinkos_critical_exit();
	/* the scheduler will run at this point */
}

#ifdef __cplusplus
}
#endif

#endif /* __ASSEMBLER__ */

#endif /* __THINKOS_IRQ_H__ */

