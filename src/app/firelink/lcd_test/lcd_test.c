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

#include <sys/stm32f.h>
#include <sys/console.h>
#include <stdio.h>
#include <stdlib.h>
#include <thinkos.h>

#include "board.h"

void io_init(void)
{
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOD);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOJ);

	stm32_gpio_mode(LED1, OUTPUT, OPEN_DRAIN | SPEED_MED);
	stm32_gpio_mode(LED2, OUTPUT, OPEN_DRAIN | SPEED_MED);

	stm32_gpio_set(LED1);
	stm32_gpio_set(LED2);
}

int led_task(void * arg)
{
	for (; ;) {
		__led_on(LED1);
		thinkos_sleep(100);
		__led_on(LED2);
		thinkos_sleep(100);
		__led_off(LED1);
		thinkos_sleep(100);
		__led_off(LED2);
		thinkos_sleep(100);
	}

	return 0;
}

void init_led_task(void)
{
	static uint32_t led_stack[256];

	static const struct thinkos_thread_inf led_inf = {
		.stack_ptr = led_stack,
		.stack_size = sizeof(led_stack),
		.priority = 8,
		.thread_id = 2,
		.paused = 0,
		.tag = "LED"
	};


	thinkos_thread_create_inf(led_task, NULL, &led_inf);
}

int clock_task(struct lcd_dev * lcd)
{
	uint32_t clk;
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	char buf[16];

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
				if (++hours == 24)
					hours = 0;
			}
		}

		sprintf(buf, "%02d:%02d:%02d", hours, minutes, seconds);

		lcd_set_pos(lcd, 0, 12);
		lcd_puts(lcd, buf);
	}
}

void init_clock_task(struct lcd_dev * lcd)
{
	static uint32_t clock_stack[128];

	static const struct thinkos_thread_inf clock_inf = {
		.stack_ptr = clock_stack, 
		.stack_size = sizeof(clock_stack), 
		.priority = 8,
		.thread_id = 31, 
		.paused = 0,
		.tag = "CLOCK"
	};

	thinkos_thread_create_inf((void *)clock_task, lcd, &clock_inf);
}

int main(int argc, char ** argv)
{
	struct lcd_dev * lcd;

	io_init();

	lcd = lcd20x4_init();

	lcd_puts(lcd, "= ThinkOS = 00:00:00");
	lcd_puts(lcd, " Zigbee Coordinator ");
	lcd_puts(lcd, "      Firelink      ");
	lcd_puts(lcd, "====================");

	init_led_task();
	init_clock_task(lcd);

	for (;;) {
		thinkos_sleep(1000);
	}

	return 0;
}

