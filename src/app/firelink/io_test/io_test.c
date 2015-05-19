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
#include "fzc_io.h"
#include "zigbee.h"

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

	return 0;
}

void init_clock_task(struct lcd_dev * lcd)
{
	static uint32_t clock_stack[128];

	static const struct thinkos_thread_inf clock_inf = {
		.stack_ptr = clock_stack, 
		.stack_size = sizeof(clock_stack), 
		.priority = 8,
		.thread_id = 30, 
		.paused = 0,
		.tag = "CLOCK"
	};

	thinkos_thread_create_inf((void *)clock_task, lcd, &clock_inf);
}

void stdio_init(void)
{
	FILE * f;

	f = console_fopen();
	/* initialize STDIO */
	stderr = f;
	stdout = f;
	stdin = f;
}

int main(int argc, char ** argv)
{
	struct lcd_dev * lcd;
	struct zigbee_dev * zigb;
	int c;

	io_init();
	stdio_init();

	lcd = lcd20x4_init();
	zigb = zigbee_init();

	lcd_puts(lcd, "= ThinkOS = 00:00:00");
	lcd_puts(lcd, " Zigbee Coordinator ");
	lcd_puts(lcd, "      Firelink      ");
	lcd_puts(lcd, "====================");

	init_clock_task(lcd);

	led_set_rate(LED_AC_PWR, RATE_1BPS);

	for (;;) {
		c = keypad_read();
		switch (c) {
		case KEY_UP:
			led_set_rate(LED_ALRM, RATE_2BPS);
			zigbee_nac_on(zigb);
			break;
		case KEY_DOWN:
			zigbee_nac_off(zigb);
			led_off(LED_ALRM);
			break;
		case KEY_LEFT:
			led_set_rate(LED_TRBL, RATE_2BPS);
			break;
		case KEY_RIGHT:
			led_set_rate(LED_SPR, RATE_2BPS);
			break;
		case KEY_ENTER:
			led_set_rate(LED_SLNC, RATE_2BPS);
			break;

		};
		lcd_set_pos(lcd, 0, 12);
	}

	return 0;
}

