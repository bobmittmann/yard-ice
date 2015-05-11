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
#include <stdlib.h>
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

void usage_fault(void)
{
	void (* f)(int, int, int, int) = (void *)(0x0);

	f(1, 2, 3, 4);
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

unsigned int irq_count = 0 ;

void stm32_tim3_isr(void)
{
	struct stm32f_tim * tim = STM32F_TIM3;
	/* Clear timer interrupt flags */
	tim->sr = 0;

	irq_count++;

	if (irq_count & 1)
		__led_on(LED4);
	else
		__led_off(LED4);

	if (irq_count == 50)
		read_fault();
}

void stm32_tim2_isr(void)
{
	struct stm32f_tim * tim = STM32F_TIM2;
	int i;

	/* Clear timer interrupt flags */
	tim->sr = 0;
	for (;;) {
		__led_on(LED2);
		for (i = 0; i < 1000000; ++i)
			__NOP();
		__led_off(LED2);
		for (i = 0; i < 1000000; ++i)
			__NOP();
	}
}
#define IRQ_PRIORITY_HIGHEST   (1 << 5)
#define IRQ_PRIORITY_VERY_HIGH (2 << 5)
#define IRQ_PRIORITY_HIGH      (3 << 5)
#define IRQ_PRIORITY_REGULAR   (4 << 5)
#define IRQ_PRIORITY_LOW       (5 << 5)
#define IRQ_PRIORITY_VERY_LOW  (6 << 5)

void timer3_init(uint32_t freq)
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
	tim->cr1 = TIM_CEN; /* Enable counter */
	tim->egr = TIM_UG; /* Force update event */
	while (tim->sr == 0);
	tim->sr = 0;
	tim->dier = TIM_UIE; /* Update interrupt enable */
	/* Enable Timer interrupt */
	thinkos_irq_register(STM32F_IRQ_TIM3, IRQ_PRIORITY_HIGH, stm32_tim3_isr);
}

void timer2_init(uint32_t freq)
{
	struct stm32f_tim * tim = STM32F_TIM2;
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
	stm32_clk_enable(STM32_RCC, STM32_CLK_TIM2);
	/* Timer configuration */
	tim->psc = pre - 1;
	tim->arr = n - 1;
	tim->cnt = 0;
	tim->cr1 = TIM_CEN; /* Enable counter */
	tim->egr = TIM_UG; /* Force update event */
	while (tim->sr == 0);
	tim->sr = 0;
	tim->dier = TIM_UIE; /* Update interrupt enable */
	/* Enable Timer interrupt */
	thinkos_irq_register(STM32F_IRQ_TIM2, IRQ_PRIORITY_LOW, stm32_tim2_isr);
}

int led_task(void * arg)
{
	int i;

	for (i = 10; i > 0; --i) {
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

	usage_fault();

	return 0;
}

uint32_t led_stack[256];

int main(int argc, char ** argv)
{
	int i;

	io_init();
	stdio_init();

//	thinkos_sleep(2000);

//	timer3_init(10);
//	timer2_init(4);

#if 0
	printf("\r\n");
	printf("--------------------------------------------\r\n");
	printf("- Thread Fault Test\r\n");
	printf("--------------------------------------------\r\n");
#endif

	thinkos_thread_create((void *)led_task, (void *)NULL,
						  led_stack, sizeof(led_stack) | 
						  THINKOS_OPT_PRIORITY(30) | 
						  THINKOS_OPT_ID(30));


	for (i = 1000; i > 0; --i) {
		__led_on(LED1);
		__led_on(LED2);
		thinkos_sleep(500);
		__led_off(LED1);
		__led_off(LED2);
		thinkos_sleep(500);
	}


	return 0;
}


