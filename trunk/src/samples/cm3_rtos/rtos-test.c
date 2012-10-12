/* 
 * File:	 dac-test.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2011 Bob Mittmann. All Rights Reserved.
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

#include "cm3rtos.h"

struct my_arg {
	volatile uint32_t cnt;
	volatile bool enabled;
};

int busy_task(void * arg)
{
	struct my_arg * ctrl= (struct my_arg *)arg;

	ctrl->cnt = 0;

	while (ctrl->enabled) {
		ctrl->cnt++;
		ctrl->cnt++;
		ctrl->cnt++;
		ctrl->cnt++;

		ctrl->cnt++;
		ctrl->cnt++;
		ctrl->cnt++;
		ctrl->cnt++;

		ctrl->cnt++;
		ctrl->cnt++;
		ctrl->cnt++;
		ctrl->cnt++;

		ctrl->cnt++;
		ctrl->cnt++;
		ctrl->cnt++;
		ctrl->cnt++;
	}

	return ctrl->cnt;
}

int yield_task(void * arg)
{
	struct my_arg * ctrl = (struct my_arg *)arg;

	while (1) {
		cm3rtos_yield();
		cm3rtos_yield();
		cm3rtos_yield();
		cm3rtos_yield();

		cm3rtos_yield();
		cm3rtos_yield();
		cm3rtos_yield();
		cm3rtos_yield();

		cm3rtos_yield();
		cm3rtos_yield();
		cm3rtos_yield();
		cm3rtos_yield();

		cm3rtos_yield();
		cm3rtos_yield();
		cm3rtos_yield();
		cm3rtos_yield();

		ctrl->cnt += 16;

		cm3rtos_yield();
		cm3rtos_yield();
		cm3rtos_yield();
		cm3rtos_yield();

		cm3rtos_yield();
		cm3rtos_yield();
		cm3rtos_yield();
		cm3rtos_yield();

		cm3rtos_yield();
		cm3rtos_yield();
		cm3rtos_yield();
		cm3rtos_yield();

		cm3rtos_yield();
		cm3rtos_yield();
		cm3rtos_yield();
		cm3rtos_yield();

		ctrl->cnt += 16;
	}

	return 0;
}

#define STACK_SIZE 512
uint32_t stack[4][STACK_SIZE / 4];
volatile struct my_arg ctrl[4];

void sched_speed_test(void)
{
//	uint32_t stack[4][STACK_SIZE / 4];
//	volatile struct my_arg ctrl[4];
	int th[4];
	int x[4];
	int y[4];
	int d[4];
	int ret;
	int i;

	printf(" - Scheduler speed test\n");

	th[0] = cm3rtos_thread_create(yield_task, (void *)&ctrl[0], 
								  stack[0], STACK_SIZE,
								  CM3RTOS_OPT_PRIORITY(1) |
								  CM3RTOS_OPT_ID(2) | CM3RTOS_OPT_PAUSED);
								  
	th[1] = cm3rtos_thread_create(yield_task, (void *)&ctrl[1], 
								  stack[1], STACK_SIZE,
								  CM3RTOS_OPT_PRIORITY(1) |
								  CM3RTOS_OPT_ID(2) | CM3RTOS_OPT_PAUSED);

	th[2] = cm3rtos_thread_create(yield_task, (void *)&ctrl[2], 
								  stack[2], STACK_SIZE,
								  CM3RTOS_OPT_PRIORITY(2) |
								  CM3RTOS_OPT_ID(0) | CM3RTOS_OPT_PAUSED);

	th[3] = cm3rtos_thread_create(yield_task, (void *)&ctrl[3], 
								  stack[3], STACK_SIZE,
								  CM3RTOS_OPT_PRIORITY(2) |
								  CM3RTOS_OPT_ID(0) | CM3RTOS_OPT_PAUSED);

	printf("  * Threads: %d, %d, %d, %d\n", th[0], th[1], th[2], th[3]);

	/* The threads where paused on creation, start them */
	for (i = 0; i < 4; i++) {
		cm3rtos_resume(th[i]);
	}

	for (i = 0; i < 5; i++) {
		x[0] = ctrl[0].cnt;
		x[1] = ctrl[1].cnt;
		x[2] = ctrl[2].cnt;
		x[3] = ctrl[3].cnt;
		cm3rtos_sleep(1000);
		y[0] = ctrl[0].cnt;
		y[1] = ctrl[1].cnt;
		y[2] = ctrl[2].cnt;
		y[3] = ctrl[3].cnt;

		d[0] = y[0] - x[0];
		d[1] = y[1] - x[1];
		d[2] = y[2] - x[2];
		d[3] = y[3] - x[3];

		printf(" %8d + %8d + %8d + %8d = %8d (%d idle)\n", 
			   d[0], d[1], d[2], d[3], d[0] + d[1] + d[2] + d[3], 
			   cm3rtos_idle_val());
	}

	for (i = 0; i < 4; i++) {
		cm3rtos_cancel(th[i], i + 30);
		printf("  * join(%d) ...", th[i]);
		ret = cm3rtos_join(th[i]);
		printf(" %d\n", ret);
	}


	printf("\n");
};

void sleep_test(void)
{
	printf(" - sleep(1000)\n");
	cm3rtos_sleep(1000);
	printf(" - wakeup\n");
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

	printf(" - Scheduler test\n");

	for (i = 0; i < 4; i++) {
		ctrl[i].enabled = true;
		ctrl[i].cnt = 0;
	}

	th[0] = cm3rtos_thread_create(busy_task, (void *)&ctrl[0], 
								  stack[0], STACK_SIZE, 
								  CM3RTOS_OPT_PRIORITY(2) |
								  CM3RTOS_OPT_ID(8));

	th[1] = cm3rtos_thread_create(busy_task, (void *)&ctrl[1], 
								  stack[1], STACK_SIZE,
								  CM3RTOS_OPT_PRIORITY(4) |
								  CM3RTOS_OPT_ID(8));

	th[2] = cm3rtos_thread_create(busy_task, (void *)&ctrl[2], 
								  stack[2], STACK_SIZE,
								  CM3RTOS_OPT_PRIORITY(6) |
								  CM3RTOS_OPT_ID(8));

	th[3] = cm3rtos_thread_create(busy_task, (void *)&ctrl[3], 
								  stack[3], STACK_SIZE,
								  CM3RTOS_OPT_PRIORITY(8) |
								  CM3RTOS_OPT_ID(8));

	printf("  * Threads: %d, %d, %d, %d\n", th[0], th[1], th[2], th[3]);

	for (i = 0; i < 8; i++) {
		x[0] = ctrl[0].cnt;
		x[1] = ctrl[1].cnt;
		x[2] = ctrl[2].cnt;
		x[3] = ctrl[3].cnt;
		cm3rtos_sleep(1000);
		y[0] = ctrl[0].cnt;
		y[1] = ctrl[1].cnt;
		y[2] = ctrl[2].cnt;
		y[3] = ctrl[3].cnt;

		d[0] = y[0] - x[0];
		d[1] = y[1] - x[1];
		d[2] = y[2] - x[2];
		d[3] = y[3] - x[3];
		sum = d[0] + d[1] + d[2] + d[3];

		ref = 100 * d[3];

		printf(" %8d + %8d + %8d + %8d = %8d (%d idle)\n", 
			   d[0], d[1], d[2], d[3], sum, cm3rtos_idle_val());
		printf(" %d%% %d%% %d%% %d%%\n", 
			   ref / d[0], ref / d[1], ref / d[2], ref / d[3]);

	}

	for (i = 0; i < 4; i++) {
		ctrl[i].enabled = false;
		printf("  * join(%d) ...", i);
		ret = cm3rtos_join(th[i]);
		printf(" %d\n", ret);
	}

	cm3rtos_sleep(1);

	printf("\n");
}

int main(int argc, char ** argv)
{
	cm3_udelay_calibrate();
	stdout = stm32f_usart_open(STM32F_UART5, 115200, SERIAL_8N1);

	printf("\n");
	printf("---------------------------------------------------------\n");
	printf(" Cortex M3 RTOS Test\n");
	printf("---------------------------------------------------------\n");
	printf("\n");
/*
	bmp_bit_wr(stack, 1, 0);
	printf("0 -> %d : %08x\n", bmp_bit_rd(stack, 1), stack[0][0]);

	bmp_bit_wr(stack, 1, 14);
	printf("14 -> %d : %08x\n", bmp_bit_rd(stack, 1), stack[0][0]);

	bmp_bit_wr(stack, 1, 15);
	printf("15 -> %d : %08x\n", bmp_bit_rd(stack, 1), stack[0][0]);
*/
	cm3rtos_init(CM3RTOS_OPT_PRIORITY(0) | CM3RTOS_OPT_ID(0));

//	sleep_test();
	busy_test();
	sched_speed_test();

	cm3rtos_sleep(10000);

	return 0;
}

