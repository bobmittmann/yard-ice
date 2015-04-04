/* 
 * File:	 sem_test.c
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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <sys/stm32f.h>
#include <sys/serial.h>
#include <sys/delay.h>

#include <thinkos.h>
#define __THINKOS_SYS__
#include <thinkos_sys.h>

#include <sys/dcclog.h>

void stdio_init(void);

struct my_arg {
	volatile uint32_t cnt;
	volatile bool enabled;
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
	}

	return dev->cnt;
}

uint32_t thinkos_idle_val(void);

#define STACK_SIZE 512
uint32_t stack[4][STACK_SIZE / 4];
volatile struct my_arg dev[4];

void busy_test(void)
{
	int th[4];
	uint32_t x[4];
	uint32_t y[4];
	uint32_t d[4];
	uint32_t sum;
	uint32_t ref;
	uint32_t wrk;
	struct thinkos_rt rt;
	uint32_t cycsum;
	int ret;
	int i;
	int j;

	printf("---------------------------------------------------------\n");
	printf(" - Scheduler test\n");

	thinkos_rt_snapshot(&rt);
	thinkos_sleep(1000);
	thinkos_rt_snapshot(&rt);
	cycsum = 0;

	for (j = 0; j < THINKOS_THREADS_MAX + 1; ++j) {
		uint32_t cyc = rt.cyccnt[j];
		cycsum += cyc;
		if (cyc > 0) {
			printf("  * Thread %d: %9d cycles.\n", j, cyc);
		}
	}
	printf("  *    Total: %9d cycles.\n", cycsum);

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

	printf("  * Thread: %8d   %8d   %8d   %8d\n", th[0], th[1], th[2], th[3]);

	for (i = 0; i < 8; i++) {
		x[0] = dev[0].cnt;
		x[1] = dev[1].cnt;
		x[2] = dev[2].cnt;
		x[3] = dev[3].cnt;
		thinkos_rt_snapshot(&rt);
		thinkos_sleep(1000);
		thinkos_rt_snapshot(&rt);
		y[0] = dev[0].cnt;
		y[1] = dev[1].cnt;
		y[2] = dev[2].cnt;
		y[3] = dev[3].cnt;

		d[0] = y[0] - x[0];
		d[1] = y[1] - x[1];
		d[2] = y[2] - x[2];
		d[3] = y[3] - x[3];
		sum = d[0] + d[1] + d[2] + d[3];

		/* XXX: reusing x, to scale down the deltas.
		 This is to avoid overflowing the percentage calculations */
		x[0] = (d[0] + 50) / 100;
		x[1] = (d[1] + 50) / 100;
		x[2] = (d[2] + 50) / 100;
		x[3] = (d[3] + 50) / 100;
		/* scale down the total sum */
		wrk = sum / 100;

		ref = d[3];
		printf("%d.\n", i);
		printf("  *   Oper: %8d + %8d + %8d + %8d = %8d (%d idle)\n", 
			   d[0], d[1], d[2], d[3], sum, thinkos_idle_val());
		printf("  *   Work: %7d%%   %7d%%   %7d%%   %7d%% = %7d%%\n", 
			   (d[0] + wrk / 2) / wrk, (d[1] + wrk / 2) / wrk,
			   (d[2] + wrk / 2) / wrk, (d[3] + wrk / 2) / wrk,
			   (sum + wrk / 2) / wrk);
		printf("  *   Nice: %7d%%   %7d%%   %7d%%   %7d%%\n", 
			   (ref + x[0] / 2) / x[0], (ref + x[1] / 2) / x[1], 
			   (ref + x[2] / 2) / x[2], (ref + x[3] / 2) / x[3]); 

		cycsum = 0;
		for (j = 0; j < THINKOS_THREADS_MAX + 1; ++j)
			cycsum += rt.cyccnt[j];

		printf("  * Cycles: %8d + %8d + %8d + %8d = %8d (%d idle)\n", 
			   rt.cyccnt[th[0]], rt.cyccnt[th[1]], 
			   rt.cyccnt[th[2]], rt.cyccnt[th[3]], 
			   cycsum, rt.cyccnt[THINKOS_THREADS_MAX]); 

	}
	printf("\n");



	for (i = 0; i < 4; i++) {
		dev[i].enabled = false;
		printf("  * join(%d) ...", th[i]);
		ret = thinkos_join(th[i]);
		printf(" %d\n", ret);
	}
	printf("---------------------------------------------------------\n");
	printf("\n");
}

int main(int argc, char ** argv)
{
	cm3_udelay_calibrate();

	stdio_init();

	printf("\n");
	printf("---------------------------------------------------------\n");
	printf(" ThinkOS (Cortex-M RTOS Profiling Test)\n");
	printf("---------------------------------------------------------\n");
	printf("\n");
	fflush(stdout);

	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	busy_test();

	delay(10);

	return 0;
}

