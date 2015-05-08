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
	FILE * f;

	f = console_fopen();
	/* initialize STDIO */
	stderr = f;
	stdout = f;
	stdin = f;
}

void io_init(void)
{
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOG);
	stm32_gpio_mode(LED1, OUTPUT, OPEN_DRAIN | SPEED_MED);
	stm32_gpio_mode(LED2, OUTPUT, OPEN_DRAIN | SPEED_MED);
	stm32_gpio_mode(LED3, OUTPUT, OPEN_DRAIN | SPEED_MED);
	stm32_gpio_mode(LED4, OUTPUT, OPEN_DRAIN | SPEED_MED);

	stm32_gpio_set(LED1);
	stm32_gpio_set(LED2);
	stm32_gpio_set(LED3);
	stm32_gpio_set(LED4);
}

void read_fault(void)
{
	volatile uint32_t * ptr = (uint32_t *)(0x0);
	uint32_t x;
	int i;

//	printf("Invalid read access\r\n");

	for (i = 0; i < (16 << 4); ++i) {
//		printf("0x%08x\r\n", (uintptr_t)ptr);
		x = *ptr;
		(void)x;
		ptr += 0x10000000 / (2 << 4);
	}
}

void write_fault(void)
{
	volatile uint32_t * ptr = (uint32_t *)(0x0);
	uint32_t x = 0;
	int i;

	printf("Invalid write access\r\n");
	for (i = 0; i < (16 << 4); ++i) {
		printf("0x%08x\r\n", (uintptr_t)ptr);
		fflush(stdout);
		*ptr = x;
		ptr += 0x10000000 / (2 << 4);
	}
}

volatile uint32_t irq_count = 0 ;

void stm32_tim3_isr(void)
{
	struct stm32f_tim * tim = STM32F_TIM3;
	/* Clear timer interrupt flags */
	tim->sr = 0;
	irq_count++;
	if (irq_count == 100)
		read_fault();
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

	/* Enable Timer interrupt */
	thinkos_irq_register(STM32F_IRQ_TIM3, 0xff, stm32_tim3_isr);

	tim->cr1 = TIM_URS | TIM_CEN; /* Enable counter */
}

int main(int argc, char ** argv)
{
	int i;

	io_init();
	stdio_init();
	timer_init(10);

	printf("\r\n");
	printf("--------------------------------------------\r\n");
	printf("- Thread Fault Test\r\n");
	printf("--------------------------------------------\r\n");

	for (i = 10; i > 0; --i) {
		printf(" %d", i);
		__led_on(LED1);
		thinkos_sleep(167);
		__led_on(LED2);
		thinkos_sleep(166);
		__led_off(LED1);
		__led_on(LED3);
		thinkos_sleep(166);
		__led_off(LED2);
		__led_on(LED4);
		thinkos_sleep(166);
		__led_off(LED3);
		thinkos_sleep(166);
		__led_off(LED4);
		thinkos_sleep(166);
	}

	printf(" ^^^\r\n");

	read_fault();

	return 0;
}


