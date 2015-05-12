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

void led_on(struct stm32_gpio *__gpio, int __pin) {
	stm32_gpio_clr(__gpio, __pin);
}

void led_off(struct stm32_gpio *__gpio, int __pin) {
	stm32_gpio_set(__gpio, __pin);
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

int led_task(void * arg)
{
	int i;

	for (i = 1000000; i > 0; --i) {
		led_on(LED1);
		thinkos_sleep(167);
		led_on(LED2);
		thinkos_sleep(166);
		led_off(LED1);
		led_on(LED3);
		thinkos_sleep(166);
		led_off(LED2);
		led_on(LED4);
		thinkos_sleep(166);
		led_off(LED3);
		thinkos_sleep(166);
		led_off(LED4);
		thinkos_sleep(166);
	}

	return 0;
}

uint32_t led_stack[256];

const struct thinkos_thread_inf led_inf = {
	.stack_ptr = led_stack,
	.stack_size = sizeof(led_stack),
	.priority = 8,
	.thread_id = 2,
	.paused = 0,
	.tag = "LED"
};


int step_task(void * arg)
{
	for (;;) {
		asm volatile ("mov r0, #0\r\n" : : : "r0");
		asm volatile ("mov r0, #1\r\n" : : : "r0");
		asm volatile ("mov r0, #2\r\n" : : : "r0");
		asm volatile ("mov r0, #3\r\n" : : : "r0");
		asm volatile ("mov r0, #4\r\n" : : : "r0");
		asm volatile ("mov r0, #5\r\n" : : : "r0");
		asm volatile ("mov r0, #6\r\n" : : : "r0");
		asm volatile ("mov r0, #7\r\n" : : : "r0");
		asm volatile ("mov r0, #8\r\n" : : : "r0");
		asm volatile ("mov r0, #9\r\n" : : : "r0");
	}
	return 0;
}


uint32_t step_stack[128];

const struct thinkos_thread_inf step_inf = {
	.stack_ptr = step_stack, 
	.stack_size = sizeof(step_stack), 
	.priority = 8,
	.thread_id = 31, 
	.paused = 0,
	.tag = "STEP"
};

uint32_t clk;
uint8_t seconds;
uint8_t minutes;
uint8_t hours;

void init_led_task(void)
{
	thinkos_thread_create_inf(led_task, NULL, &led_inf);
}

void init_step_task(void)
{
	thinkos_thread_create_inf(step_task, NULL, &step_inf);
}

int main(int argc, char ** argv)
{
	io_init();

	init_led_task();

	init_step_task();

	clk = thinkos_clock();
	seconds = 0;
	minutes = 0;
	hours = 0;

	for (; ; ) {
		clk += 1000;
		thinkos_alarm(clk);
		if (++seconds == 60) {
			seconds = 0;
			if (++minutes == 60) {
				minutes = 0;
				hours++;
			}
		}

	}

	return 0;
}


void _exit(int status)
{
	for (;;);
}

