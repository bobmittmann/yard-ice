/* 
 * File:	 usb-cdc.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2011 Bob Mittmann. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#define __THINKOS_DMON__
#include <thinkos_dmon.h>
#include <thinkos.h>
#include <sys/dcclog.h>

void __thinkos_irq_disable_all(void)
{
	int irq;

	/* adjust IRQ priorities to regular (above SysTick and bellow SVC) */
	for (irq = 0; irq < THINKOS_IRQ_MAX; irq++) {
		cm3_irq_disable(irq);
		thinkos_rt.irq_th[irq] = -1;
	}
}

void __thinkos_kill_all(void) 
{
	int wq;

	/* clear all wait queues */
	for (wq = 0; wq < THINKOS_WQ_LST_END; ++wq) 
		thinkos_rt.wq_lst[wq] = 0;
#if (THINKOS_THREADS_MAX < 32) 
	/* put the IDLE thread in the ready queue */
	__bit_mem_wr(&thinkos_rt.wq_ready, THINKOS_THREADS_MAX, 1);
#endif
	/* discard current thread context */
	thinkos_rt.active = THINKOS_THREAD_VOID;
	/* signal the scheduler ... */
	__thinkos_defer_sched();
}

void dmon_soft_reset(struct dmon_comm * comm)
{
	DCC_LOG(LOG_TRACE, "1. disable all interrupts"); 
	__thinkos_irq_disable_all();
	DCC_LOG(LOG_TRACE, "2. kill all threads...");
	__thinkos_kill_all(); 
	DCC_LOG(LOG_TRACE, "3. wait idle..."); 
	dmon_wait_idle();
	DCC_LOG(LOG_TRACE, "4. ThinkOS reset...");
	__thinkos_reset();
//	DCC_LOG(LOG_TRACE, "5. Reload idle ...");
	/* signal the scheduler ... */
//	__thinkos_defer_sched();
//	dmon_wait_idle();
	DCC_LOG(LOG_TRACE, "6. restore Comm interrupts...");
	dmon_comm_irq_config(comm);
}

