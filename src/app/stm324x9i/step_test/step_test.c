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

#include <stdlib.h>
#include <thinkos.h>
#include <sys/console.h>
#include <sys/stm32f.h>

#include "board.h"

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

int led_task(void * arg)
{
	int i;

	for (i = 1000000; i > 0; --i) {
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

	return 0;
}

uint32_t led_stack[256];

int step_task(void * arg)
{
	asm volatile (
				  "1:\r\n"
				  "mov r0, #0\r\n" 
				  "mov r0, #1\r\n" 
				  "mov r0, #2\r\n" 
				  "mov r0, #3\r\n" 
				  "mov r0, #4\r\n" 
				  "mov r0, #5\r\n" 
				  "mov r0, #6\r\n" 
				  "mov r0, #7\r\n" 
				  "mov r0, #8\r\n" 
				  "mov r0, #9\r\n" 
				  "b 1b\r\n" 
				  : : );

	return 0;
}


uint32_t step_stack[128];

const struct thinkos_thread_inf step_inf = {
	.stack_ptr = step_stack, 
	.stack_size = sizeof(step_stack), 
	.priority = 32,
	.thread_id = 31, 
	.paused = 0,
	.tag = "STEP"
};


int main(int argc, char ** argv)
{
	int i;

	io_init();

	thinkos_thread_create((void *)led_task, (void *)NULL,
						  led_stack, sizeof(led_stack) | 
						  THINKOS_OPT_PRIORITY(30) | 
						  THINKOS_OPT_ID(30));

	thinkos_thread_create_inf(step_task, NULL, &step_inf);


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


