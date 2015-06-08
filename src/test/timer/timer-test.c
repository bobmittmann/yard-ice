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
volatile uint32_t tmr_count = 0 ;
int tmr_sem;


void stm32f_tim4_isr(void)
{
	struct stm32f_tim * tim = STM32F_TIM4;
	/* Clear timer interrupt flags */
	tim->sr = 0;
	tmr_count++;
	thinkos_sem_post_i(tmr_sem);
//	DCC_LOG(LOG_TRACE, "++++");
}

void oneshot_timer(unsigned int usec)
{
	struct stm32f_tim * tim = STM32F_TIM4;

	tim->arr = usec; 
	tim->cr1 = TIM_CMS_EDGE | TIM_OPM | TIM_URS | TIM_CEN; 
}

void oneshot_timer_init(uint32_t freq)
{
	struct stm32f_tim * tim = STM32F_TIM4;
	unsigned int div;

	/* Timer clock enable */
	stm32_clk_enable(STM32_RCC, STM32_CLK_TIM4);
	/* get the total divisior */
	div = (stm32f_tim2_hz + (freq / 2)) / freq;
	DCC_LOG1(LOG_TRACE, "div=%d", div);
	/* Timer configuration */
	tim->psc = div - 1;
	tim->arr = 0;
	tim->cnt = 0;
	tim->dier = TIM_UIE; /* Update interrupt enable */
	tim->cr1 = TIM_CMS_EDGE | TIM_OPM | TIM_URS; 
	cm3_irq_enable(STM32F_IRQ_TIM4);
}


void stm32f_tim3_isr(void)
{
	struct stm32f_tim * tim = STM32F_TIM3;
#if 0
	oneshot_timer(20); /* time to signal the semaphore after the 
						  first wakeup, before going to sleep again. */
#endif
//	oneshot_timer(41); 
//	oneshot_timer(66); /* rollback 1 */
	oneshot_timer(69); /* rollback 2 */
//	oneshot_timer(80); /* sleep */

	/* Clear timer interrupt flags */
	tim->sr = 0;
	irq_count++;

	thinkos_sem_post_i(tmr_sem);
	DCC_LOG1(LOG_INFO, "IRQ count = %d", irq_count);
}


void periodic_timer_init(uint32_t freq)
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
	tim->cnt = n - 1;
	tim->cr1 = TIM_CEN; /* Enable counter */
	tim->egr = TIM_UG; /* Force an update */
	while (tim->sr == 0);
	tim->sr = 0;
	tim->dier = TIM_UIE; /* Update interrupt enable */

	/* enable interrupts */
	cm3_irq_enable(STM32F_IRQ_TIM3);
}

void io_init(void)
{
	DCC_LOG(LOG_MSG, "Configuring GPIO pins...");
}

int main(int argc, char ** argv)
{
	int i;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	DCC_LOG(LOG_TRACE, "1. cm3_udelay_calibrate()");
	/* calibrate usecond delay loop */
	cm3_udelay_calibrate();

	DCC_LOG(LOG_TRACE, "2. io_init()");
	io_init();

	DCC_LOG(LOG_TRACE, "3. thinkos_init()");
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(32));

	tmr_sem = thinkos_sem_alloc(0);
	DCC_LOG1(LOG_TRACE, "tmr_sem=%d", tmr_sem);

	oneshot_timer_init(168000000 / 2);
	periodic_timer_init(2);

	for (i = 0; ; ++i) {
		if ((i & 0x7) == 0)
			DCC_LOG3(LOG_TRACE, "i=%d IRQ=%d TMR=%d", i, irq_count, tmr_count);
		thinkos_sem_wait(tmr_sem);
		thinkos_sem_wait(tmr_sem);
//		if (irq_count != tmr_count)
//			DCC_LOG2(LOG_TRACE, "IRQ=%d TMR=%d", irq_count, tmr_count);
//		thinkos_sleep((1 + (i % 9)) * 100);
	}

	return 0;
}



