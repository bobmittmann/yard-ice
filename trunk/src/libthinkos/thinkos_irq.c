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

#define __THINKOS_SYS__
#include <thinkos_sys.h>

#include <thinkos.h>

#include <stdio.h>

void cm3_default_isr(int irq) 
{
	int idx;

	/* disable this interrupt source */
	cm3_irq_disable(irq);

	idx = cm3_irq_pri_get(irq) >> 4;

//	printf("[%d > %d]", irq, idx);

	/* insert the thread into ready queue */
	bmp_bit_set(&thinkos_rt.wq_ready, idx);  

	/* signal the scheduler ... */
	__thinkos_defer_sched();
}

void thinkos_irq_wait(int irq)
{
	int32_t self = thinkos_rt.active;

	cm3_irq_pri_set(irq, self << 4);

//	printf("%s(): <%d> wait @ irq %d...\n", __func__, self, irq);

	__thinkos_critical_enter();
	/* wait for event */
	__thinkos_wait();
	/* enable this interrupt source */
	cm3_irq_enable(irq);
	__thinkos_critical_exit();
}


