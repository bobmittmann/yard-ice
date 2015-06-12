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
#include "lcd20x4.h"
#include "fzc_io.h"
#include "lattice.h"

struct rs585_dev;

struct rs585_dev * rs485_init(void);

int rc485_pkt_recv(struct rs585_dev * rs485, uint8_t * pkt,
		unsigned int tmo);

int rc485_pkt_send(struct rs585_dev * rs485, uint8_t * pkt,
		unsigned int len);

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
		.thread_id = 30, 
		.paused = 0,
		.tag = "CLOCK"
	};

	thinkos_thread_create_inf((void *)clock_task, lcd, &clock_inf);
}

volatile unsigned int zap_tm = 100;

int zap_task(struct lcd_dev * lcd)
{
	int x;

	thinkos_sleep(zap_tm);
	lcd_at_puts(lcd, 3, 0, "==");

	for (;;) {
		for (x = 0; x < 18; ++x) {
			thinkos_sleep(zap_tm);
			lcd_at_puts(lcd, 3, x, " ==");
		}
		for (; x >= 0; --x) {
			thinkos_sleep(zap_tm);
			lcd_at_puts(lcd, 3, x, "== ");
		}
	}

	return 0;
}

void init_zap_task(struct lcd_dev * lcd)
{
	static uint32_t zap_stack[128];

	static const struct thinkos_thread_inf zap_inf = {
		.stack_ptr = zap_stack,
		.stack_size = sizeof(zap_stack),
		.priority = 8,
		.thread_id = 10,
		.paused = 0,
		.tag = "ZAP"
	};

	thinkos_thread_create_inf((void *)zap_task, lcd, &zap_inf);
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
	struct rs585_dev * rs485;
	int c;
	int rate = RATE_OFF;

	io_init();
	stdio_init();

	lcd = lcd20x4_init();

	lcd_puts(lcd, "= ThinkOS = 00:00:00");
	lcd_puts(lcd, " Zigbee Coordinator ");
	lcd_puts(lcd, "      Firelink      ");
	lcd_puts(lcd, "====================");

	rs485 = rs485_init();

	init_clock_task(lcd);
	init_zap_task(lcd);

	led_set_rate(LED_AC_PWR, RATE_1BPS);

	for (;;) {
		c = keypad_read();
		switch (c) {
		case KEY_UP:
			led_set_rate(LED_ALRM, RATE_2BPS);
			break;
		case KEY_DOWN:
			led_off(LED_ALRM);
			break;
		case KEY_LEFT:
			zap_tm += 10;
			break;
		case KEY_RIGHT:
			if (zap_tm > 10) {
				zap_tm -= 10;
			}
			break;
		case KEY_ENTER:
			switch (rate) {
			case RATE_OFF:
				rate = RATE_1BPS;
				break;
			case RATE_1BPS:
				rate = RATE_2BPS;
				break;
			case RATE_2BPS:
				rate = RATE_4BPS;
				break;
			case RATE_4BPS:
				rate = RATE_STEADY;
				break;
			case RATE_STEADY:
				rate = RATE_OFF;
				break;
			}
			led_set_rate(LED_SLNC, rate);
			led_set_rate(LED_TRBL, rate);
			led_set_rate(LED_SPR, rate);
			break;

		};
		lcd_set_pos(lcd, 0, 12);
	}

	return 0;
}

