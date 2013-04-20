/* 
 * thikos_irq.c
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

#define __THINKOS_IRQ__
#include <thinkos_irq.h>
#include <thinkos.h>

#include <stdio.h>

#if THINKOS_IRQ_MAX > 0

void cm3_default_isr(int irq) 
{
	int th;

	/* disable this interrupt source */
	cm3_irq_disable(irq);

	th = thinkos_rt.irq_th[irq];

	/* insert the thread into ready queue */
	__bit_mem_wr(&thinkos_rt.wq_ready, th, 1);  

	/* signal the scheduler ... */
	__thinkos_defer_sched();
}

void thinkos_irq_wait_svc(int32_t * arg)
{
	unsigned int irq = arg[0];
	int32_t self = thinkos_rt.active;

#if THINKOS_ENABLE_ARG_CHECK
	if (irq >= THINKOS_IRQ_MAX) {
		DCC_LOG1(LOG_ERROR, "invalid IRQ %d!", irq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif

	DCC_LOG1(LOG_TRACE, "IRQ %d", irq);

	/* store the thread info */
	thinkos_rt.irq_th[irq] = self;

	/* clear pending interrupt */
	cm3_irq_pend_clr(irq);

	/* enable this interrupt source */
	cm3_irq_enable(irq);

	/* prepare to wait ... */
	__thinkos_wait();
}

#endif

