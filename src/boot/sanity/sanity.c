/* 
 * Copyright(C) 2013 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the usb-serial converter.
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
 * @file usbamp.c
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <fixpt.h>

#include "board.h"

#include <string.h>
#include <stdint.h>
#include <sys/param.h>
#include <sys/delay.h>
#include <sys/console.h>
#include <thinkos.h>
#include <stdio.h>
#include <trace.h>

const unsigned int tracelevel = TRACE_LVL_DBG;
const bool showtrace = true;
const bool showcpu = false;
const bool showdetail = false;
const bool flushtrace = true;
unsigned int second;
unsigned int minute;
unsigned int hour;


int app_task(FILE * f)
{
	uint32_t clk;

	clk = thinkos_clock();
	second = 0;
	minute = 0;
	hour = 0;
	for (;;) {
		int i;

		for (i = 0; i < 5; ++i) {
			clk += 150;
			thinkos_alarm(clk);
			clk += 50;
			thinkos_alarm(clk);
		}
		if (++second == 60) {
			second = 0;
			if (++minute == 60) {
				minute = 0;
				hour++;
			}
		}
		printf("\r%02d:%02d:%02d", hour, minute, second);
	}

	return 0;
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

int timer_test_init(uint32_t freq);

uint32_t app_stack[1024] __attribute__ ((aligned(8), section(".stack")));

const struct thinkos_thread_inf app_thread_inf = {
	.stack_ptr = app_stack,
	.stack_size = sizeof(app_stack),
	.priority = 16,
	.thread_id = 16,
	.paused = false,
	.tag = "APP"
};

int main(int argc, char ** argv)
{
	stdio_init();

	trace_init();

	/* Start the main application thread */
	thinkos_thread_create_inf((int (*)(void *))app_task, (void *)stdout,
							&app_thread_inf);

	/* cancel the current thread */
	thinkos_thread_abort(0);


	return 0;
}





