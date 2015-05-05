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
#include <sys/console.h>
#include <sys/stm32f.h>
#include <sys/serial.h>
#include <sys/tty.h>

#include "board.h"

void stdio_init(void)
{
	struct serial_dev * ser;
	struct tty_dev * tty;
	FILE * f_tty;
	FILE * f_raw;

	ser = stm32f_uart5_serial_init(115200, SERIAL_8N1);
	f_raw = serial_fopen(ser);
	tty = tty_attach(f_raw);
	f_tty = tty_fopen(tty);

	stderr = console_fopen();
	stdout = f_tty;
	stdin = f_tty;
}

volatile uint32_t irq_count = 0 ;
volatile int tmr_sem;

void stm32_tim3_isr(void)
{
	struct stm32f_tim * tim = STM32F_TIM3;
	/* Clear timer interrupt flags */
	tim->sr = 0;
	irq_count++;
	thinkos_sem_post_i(tmr_sem);
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


	printf(" %s(): freq=%dHz pre=%d n=%d\n", 
		   __func__, freq, pre, n);

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

	/* Enable interrupt */
	thinkos_irq_register(STM32F_IRQ_TIM3, 0xff, stm32_tim3_isr);
}

void io_init(void)
{
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOC);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOD);
	/* USART5 TX */
	stm32_gpio_mode(UART5_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_af(UART5_TX, GPIO_AF8);
	/* USART5 RX */
	stm32_gpio_mode(UART5_RX, ALT_FUNC, PULL_UP);
	stm32_gpio_af(UART5_RX, GPIO_AF8);
}

int main(int argc, char ** argv)
{
	io_init();

	stdio_init();

	printf("\n");
	printf("--------------------------------------------\n");
	printf("- Serial Console Test\n");
	printf("--------------------------------------------\n");

	tmr_sem = thinkos_sem_alloc(0);
	timer_init(2);

	for (;;) {
		thinkos_sem_wait(tmr_sem);
		printf("IRQ count = %d\n", irq_count);
	}

	return 0;
}



