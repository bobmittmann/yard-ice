/* 
 * File:	 rtos_basic.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */


#include <sys/stm32f.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <arch/cortex-m3.h>
#include <sys/serial.h>
#include <sys/delay.h>
#include <sys/dcclog.h>

#include <thinkos.h>

struct my_arg {
	volatile uint32_t cnt;
	volatile bool enabled;
};

int yield_task(void * arg)
{
	struct my_arg * dev = (struct my_arg *)arg;

	while (1) {
		thinkos_yield();
		thinkos_yield();
		thinkos_yield();
		thinkos_yield();
		thinkos_yield();

		thinkos_yield();
		thinkos_yield();
		thinkos_yield();
		thinkos_yield();
		thinkos_yield();

		thinkos_yield();
		thinkos_yield();
		thinkos_yield();
		thinkos_yield();
		thinkos_yield();

		thinkos_yield();
		thinkos_yield();
		thinkos_yield();
		thinkos_yield();
		thinkos_yield();

		thinkos_yield();
		thinkos_yield();
		thinkos_yield();
		thinkos_yield();
		thinkos_yield();

		thinkos_yield();
		thinkos_yield();
		thinkos_yield();
		thinkos_yield();
		thinkos_yield();

		thinkos_yield();
		thinkos_yield();
		thinkos_yield();
		thinkos_yield();
		thinkos_yield();

		thinkos_yield();
		thinkos_yield();
		thinkos_yield();
		thinkos_yield();
		thinkos_yield();

		thinkos_yield();
		thinkos_yield();
		thinkos_yield();
		thinkos_yield();
		thinkos_yield();

		thinkos_yield();
		thinkos_yield();
		thinkos_yield();
		thinkos_yield();
		thinkos_yield();

		dev->cnt += 50;
	}

	return 0;
}

uint32_t thinkos_idle_val(void);

#define STACK_SIZE 512
uint32_t stack[4][STACK_SIZE / 4];
volatile struct my_arg dev[4];

void sched_speed_test(void)
{
//	uint32_t stack[4][STACK_SIZE / 4];
//	volatile struct my_arg ctrl[4];
	int th[4];
	int x[4];
	int y[4];
	int d[4];
	unsigned int sum;
	unsigned int dt;
	int ret;
	int i;

	printf("---------------------------------------------------------\n");
	printf(" - Scheduler speed test\n");

	th[0] = thinkos_thread_create(yield_task, (void *)&dev[0], 
								  stack[0], STACK_SIZE |
								  THINKOS_OPT_PRIORITY(1) |
								  THINKOS_OPT_ID(2) | THINKOS_OPT_PAUSED);
								  
	th[1] = thinkos_thread_create(yield_task, (void *)&dev[1], 
								  stack[1], STACK_SIZE |
								  THINKOS_OPT_PRIORITY(1) |
								  THINKOS_OPT_ID(2) | THINKOS_OPT_PAUSED);

	th[2] = thinkos_thread_create(yield_task, (void *)&dev[2], 
								  stack[2], STACK_SIZE |
								  THINKOS_OPT_PRIORITY(2) |
								  THINKOS_OPT_ID(0) | THINKOS_OPT_PAUSED);

	th[3] = thinkos_thread_create(yield_task, (void *)&dev[3], 
								  stack[3], STACK_SIZE |
								  THINKOS_OPT_PRIORITY(2) |
								  THINKOS_OPT_ID(0) | THINKOS_OPT_PAUSED);

	printf("  * Threads: %d, %d, %d, %d\n", th[0], th[1], th[2], th[3]);

	/* The threads where paused on creation, start them */
	for (i = 0; i < 4; i++) {
		thinkos_resume(th[i]);
	}

	for (i = 0; i < 5; i++) {
		x[0] = dev[0].cnt;
		x[1] = dev[1].cnt;
		x[2] = dev[2].cnt;
		x[3] = dev[3].cnt;
		thinkos_sleep(1000);
		y[0] = dev[0].cnt;
		y[1] = dev[1].cnt;
		y[2] = dev[2].cnt;
		y[3] = dev[3].cnt;

		d[0] = y[0] - x[0];
		d[1] = y[1] - x[1];
		d[2] = y[2] - x[2];
		d[3] = y[3] - x[3];
		sum = d[0] + d[1] + d[2] + d[3];
		dt = 1000000000 / sum;

		printf(" %7d + %7d + %8d + %7d = %8d (%d.%03d us)\n", 
			   d[0], d[1], d[2], d[3], sum, dt / 1000, dt % 1000);
	}

	for (i = 0; i < 4; i++) {
		thinkos_cancel(th[i], i + 30);
		printf("  * join(%d) ...", th[i]);
		ret = thinkos_join(th[i]);
		printf(" %d\n", ret);
	}


	printf("\n");
};

int busy_task(void * arg)
{
	struct my_arg * dev= (struct my_arg *)arg;

	dev->cnt = 0;

	while (dev->enabled) {
		dev->cnt++;
		dev->cnt++;
		dev->cnt++;
		dev->cnt++;
		dev->cnt++;

		dev->cnt++;
		dev->cnt++;
		dev->cnt++;
		dev->cnt++;
		dev->cnt++;

		dev->cnt++;
		dev->cnt++;
		dev->cnt++;
		dev->cnt++;
		dev->cnt++;

		dev->cnt++;
		dev->cnt++;
		dev->cnt++;
		dev->cnt++;
		dev->cnt++;

		dev->cnt++;
		dev->cnt++;
		dev->cnt++;
		dev->cnt++;
		dev->cnt++;

		dev->cnt++;
		dev->cnt++;
		dev->cnt++;
		dev->cnt++;
		dev->cnt++;

		dev->cnt++;
		dev->cnt++;
		dev->cnt++;
		dev->cnt++;
		dev->cnt++;

		dev->cnt++;
		dev->cnt++;
		dev->cnt++;
		dev->cnt++;
		dev->cnt++;

		dev->cnt++;
		dev->cnt++;
		dev->cnt++;
		dev->cnt++;
		dev->cnt++;

		dev->cnt++;
		dev->cnt++;
		dev->cnt++;
		dev->cnt++;
		dev->cnt++;
	}

	return dev->cnt;
}

void busy_test(void)
{
	int th[4];
	int x[4];
	int y[4];
	int d[4];
	int sum;
	int ref;
	int ret;
	int i;

	printf("---------------------------------------------------------\n");
	printf(" - Scheduler test\n");

	for (i = 0; i < 4; i++) {
		dev[i].enabled = true;
		dev[i].cnt = 0;
	}

	th[0] = thinkos_thread_create(busy_task, (void *)&dev[0], 
								  stack[0], STACK_SIZE | 
								  THINKOS_OPT_PRIORITY(2) |
								  THINKOS_OPT_ID(8));

	th[1] = thinkos_thread_create(busy_task, (void *)&dev[1], 
								  stack[1], STACK_SIZE |
								  THINKOS_OPT_PRIORITY(4) |
								  THINKOS_OPT_ID(8));

	th[2] = thinkos_thread_create(busy_task, (void *)&dev[2], 
								  stack[2], STACK_SIZE |
								  THINKOS_OPT_PRIORITY(6) |
								  THINKOS_OPT_ID(8));

	th[3] = thinkos_thread_create(busy_task, (void *)&dev[3], 
								  stack[3], STACK_SIZE |
								  THINKOS_OPT_PRIORITY(8) |
								  THINKOS_OPT_ID(8));

	printf("  * Threads: %d, %d, %d, %d\n", th[0], th[1], th[2], th[3]);

	for (i = 0; i < 8; i++) {
		x[0] = dev[0].cnt;
		x[1] = dev[1].cnt;
		x[2] = dev[2].cnt;
		x[3] = dev[3].cnt;
		thinkos_sleep(1000);
		y[0] = dev[0].cnt;
		y[1] = dev[1].cnt;
		y[2] = dev[2].cnt;
		y[3] = dev[3].cnt;

		d[0] = y[0] - x[0];
		d[1] = y[1] - x[1];
		d[2] = y[2] - x[2];
		d[3] = y[3] - x[3];
		sum = d[0] + d[1] + d[2] + d[3];

		ref = 100 * d[3];

		printf("  Ops: %8d + %8d + %8d + %8d = %8d\n", 
			   d[0], d[1], d[2], d[3], sum);
		printf(" Nice: %7d%%   %7d%%   %7d%%   %7d%%\n", 
			   (ref + d[0] / 2) / d[0], (ref + d[1] / 2) / d[1], 
			   (ref + d[2] / 2) / d[2], (ref + d[3] / 2) / d[3]);

	}

	for (i = 0; i < 4; i++) {
		dev[i].enabled = false;
		printf("  * join(%d) ...", th[i]);
		ret = thinkos_join(th[i]);
		printf(" %d\n", ret);
	}

	thinkos_sleep(1);

	printf("\n");
}

void stdio_init(void);

int main(int argc, char ** argv)
{
	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	cm3_udelay_calibrate();

	stdio_init();

	printf("\n");
	printf("---------------------------------------------------------\n");
	printf(" ThinkOS (Cortex-M RTOS Basic Test)\n");
	printf("---------------------------------------------------------\n");
	printf("\n");

	delay(1);
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	sched_speed_test();
	busy_test();
//	sleep_test();

	delay(10);

	return 0;
}

