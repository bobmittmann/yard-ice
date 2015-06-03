/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the YARD-ICE.
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

/** 
 * @file hello.c
 * @brief application test
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdio.h>
#include <thinkos.h>
#include <sys/delay.h>
#include <sys/stm32f.h>
#include <sys/dcclog.h>

volatile uint32_t irq_count = 0 ;
volatile int tmr_sem;

uint32_t xwq;

void stm32f_tim3_isr(void)
{
	struct stm32f_tim * tim = STM32F_TIM3;
//	uint32_t x;

	/* Clear timer interrupt flags */
	tim->sr = 0;
//	irq_count++;
//	__bit_mem_wr((uint32_t *)&xwq, irq_count & 0x1f, 1);

//	x = __ldrexw((uint32_t *)&xwq);
//	x |= (1 << (irq_count & 0x1f));
//	if (__strexw((uint32_t *)&xwq, x))
//		DCC_LOG1(LOG_WARNING, "strex wxq=0x%08x failed!", xwq);
//	thinkos_sem_post_i(tmr_sem);
//	DCC_LOG1(LOG_TRACE, "IRQ count = %d", irq_count);
}

void timer_init(uint32_t freq)
{
	struct stm32f_tim * tim = STM32F_TIM3;
	uint32_t div;
	uint32_t pre;
	uint32_t n;

	/* get the total divisior */
	div = ((stm32f_tim1_hz) + (freq / 2)) / freq;
	/* get the minimum pre scaler */
	pre = (div / 65536) + 1;
	/* get the reload register value */
	n = (div + pre / 2) / pre;

	/* Timer clock enable */
	stm32_clk_enable(STM32_RCC, STM32_CLK_TIM3);
	
	/* Timer configuration */
	tim->psc = pre - 1;
	tim->arr = n - 1;
	tim->cnt = 0;
	tim->egr = 0;
	tim->dier = TIM_UIE; /* Update interrupt enable */
	tim->ccmr1 = TIM_OC1M_PWM_MODE1;
	tim->ccr1 = tim->arr - 2;
	tim->cr2 = TIM_MMS_OC1REF;
	tim->cr1 = TIM_URS | TIM_CEN; /* Enable counter */

	/* enable interrupts */
	cm3_irq_enable(STM32F_IRQ_TIM3);
}

void io_init(void)
{
	DCC_LOG(LOG_MSG, "Configuring GPIO pins...");
}

int main(int argc, char ** argv)
{
	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	DCC_LOG(LOG_TRACE, "1. cm3_udelay_calibrate()");
	/* calibrate usecond delay loop */
	cm3_udelay_calibrate();

	DCC_LOG(LOG_TRACE, "2. io_init()");
	io_init();

	DCC_LOG(LOG_TRACE, "3. thinkos_init()");
//	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(32));

//	tmr_sem = thinkos_sem_alloc(0);
	DCC_LOG1(LOG_TRACE, "tmr_sem=%d", tmr_sem);
	timer_init(5);


	for (;;) {
		uint32_t x;

		xwq = 0;
//		thinkos_sem_wait(tmr_sem);
//		DCC_LOG1(LOG_TRACE, "IRQ count = %d", irq_count);
//		thinkos_sleep(1000);

		x = __ldrexw(&xwq);
		udelay(100000);
		x = 0;
		if (__strexw(&xwq, x))
			DCC_LOG1(LOG_WARNING, "strex wxq=0x%08x failed!", xwq);
		else
			DCC_LOG1(LOG_TRACE, "strex wxq=0x%08x ok.", xwq);
	}

	return 0;
}



