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

	DCC_LOG1(LOG_MSG, "IRQ %d", irq);

	/* store the thread info */
	thinkos_rt.irq_th[irq] = self;

	/* clear pending interrupt */
	cm3_irq_pend_clr(irq);

	/* enable this interrupt source */
	cm3_irq_enable(irq);

	/* prepare to wait ... */
	__thinkos_wait(self);
}

#endif

#ifdef CM3_RAM_VECTORS
extern void * __ram_vectors[];
extern int __sizeof_rom_vectors;

void thinkos_irq_register_svc(int32_t * arg)
{
	unsigned int irq = arg[0];
	unsigned int priority = arg[1];
	void * isr = (void *)arg[2];

#if THINKOS_ENABLE_ARG_CHECK
	int irq_max = ((uintptr_t)&__sizeof_rom_vectors / sizeof(void *)) - 16;

	if (irq >= irq_max) {
		DCC_LOG1(LOG_ERROR, "invalid IRQ %d!", irq);
		arg[0] = THINKOS_EINVAL;
		return;
	}
#endif

	/* disable this interrupt source */
	cm3_irq_disable(irq);

	/* adjust the priority level */
	cm3_irq_disable(irq);
	if (priority > IRQ_PRIORITY_VERY_LOW)
		priority = IRQ_PRIORITY_VERY_LOW;
	else if (priority < IRQ_PRIORITY_VERY_HIGH)
		priority = IRQ_PRIORITY_VERY_HIGH;

	/* set the interrupt priority */
	cm3_irq_pri_set(irq, priority);

	/* clear pending interrupt */
	cm3_irq_pend_clr(irq);

	/* set the vector */
	__ram_vectors[irq + 16] = isr;

	/* enable this interrupt source */
	cm3_irq_enable(irq);
}

#endif

