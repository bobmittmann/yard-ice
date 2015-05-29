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
#include <sys/delay.h>

#include <stdio.h>
#include <stdlib.h>
#include <thinkos.h>

#include "board.h"
#include "lcd20x4.h"

void io_init(void)
{
	thinkos_sysinfo_udelay_factor(&udelay_factor);

	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOD);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOJ);
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

		lcd_at_puts(lcd, 0, 12, buf);
	}

	return 0;
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

void lcd_test1(struct lcd_dev * lcd)
{
	int x;
	int y;
	int c;

	for (x = 0; x < 20; ++x) {
		for (y = 0; y < 4; ++y) {
			thinkos_sleep(50);
			lcd_set_pos(lcd, y, x);
			if (x < 10)
				c = x + '0';
			else
				c = x - 10 + 'A';
			lcd_putc(lcd, c);
		}
	}
}

int main(int argc, char ** argv)
{
	struct lcd_dev * lcd;
	int x;

	io_init();

	lcd = lcd20x4_init();

	lcd_test1(lcd);

	thinkos_sleep(5);

	lcd_puts(lcd, "= ThinkOS = 00:00:00");
	lcd_puts(lcd, " Zigbee Coordinator ");
	lcd_puts(lcd, "      Firelink      ");
	lcd_puts(lcd, "====================");

	init_clock_task(lcd);

	thinkos_sleep(2000);
	lcd_at_puts(lcd, 3, 0, "                    ");

	for (;;) {
		thinkos_sleep(100);
		lcd_at_puts(lcd, 3, 0, "=");
		thinkos_sleep(100);
		lcd_at_puts(lcd, 3, 0, "==");
		for (x = 0; x < 18; ++x) {
			thinkos_sleep(100);
			lcd_at_puts(lcd, 3, x, " ==");
		}
		thinkos_sleep(100);
		lcd_at_puts(lcd, 3, x, " =");
		thinkos_sleep(100);
		lcd_at_puts(lcd, 3, x + 1, " ");
	}

	return 0;
}

