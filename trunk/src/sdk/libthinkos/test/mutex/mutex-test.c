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

#define STACK_SIZE 512
uint32_t stack[4][STACK_SIZE / 4];
volatile struct my_arg dev[4];

void sleep_test(void)
{
	printf(" - sleep(1000)\n");
	thinkos_sleep(1000);
	printf(" - wakeup\n");
}


int mutex;

static unsigned long long fib_i = 0;

unsigned long long fibonnaci(void) 
{
	static unsigned long long x0 = 0;
	static unsigned long long x1 = 0;
	unsigned long long y;

	/* thinking */
	thinkos_sleep(rand() % 100);
	/* working */
	if (fib_i == 0)
		y = 0;
	else if (fib_i == 1)
		y = 1;
	else
		y = x1 + x0;
	
	x0 = x1;
	x1 = y;
	fib_i++;
	
	return y;
}

int sync = 0;
int end = 0;

int fibonnaci_task(void * arg)
{
	int self = thinkos_thread_self();
	unsigned long long y;
	int i;

	thinkos_mutex_lock(mutex);
	printf("[%d] started...\n", self);
	thinkos_mutex_unlock(mutex);
	thinkos_sleep(100);

	for (i = 0; i < 20; i++) {
		thinkos_mutex_lock(mutex);
		y = fibonnaci();
		printf("[%d] %llu\n", self, y);
		thinkos_mutex_unlock(mutex);
	}

	end++;
	thinkos_sleep(1000);

	thinkos_mutex_lock(mutex);
	sync++;
	thinkos_mutex_unlock(mutex);

	for (i = 0; i < 20; i++) {
		y = fibonnaci();
		printf("[%d]:%llu \n", self, y);
	}

	end++;

	printf("[%d] end.\n", self);

	return 0;
}

void mutex_test(void)
{
	int th[3];
	int ret;
	int i;

	printf("  * thinkos_mutex_alloc()\n");
	mutex = thinkos_mutex_alloc();

	printf("  * thinkos_mutex_lock(%d)\n", mutex);
	thinkos_mutex_lock(mutex);

	printf("  * thinkos_thread_create()\n");
	th[0] = thinkos_thread_create(fibonnaci_task, NULL, 
								  stack[0], STACK_SIZE,
								  THINKOS_OPT_PRIORITY(1) |
								  THINKOS_OPT_ID(1));

	printf("  * thinkos_thread_create()\n");
	th[1] = thinkos_thread_create(fibonnaci_task, NULL, 
								  stack[1], STACK_SIZE,
								  THINKOS_OPT_PRIORITY(1) |
								  THINKOS_OPT_ID(1));

	printf("  * thinkos_thread_create()\n");
	th[2] = thinkos_thread_create(fibonnaci_task, NULL, 
								  stack[2], STACK_SIZE,
								  THINKOS_OPT_PRIORITY(1) |
								  THINKOS_OPT_ID(1));

	printf("  * Mutex: %d; Threads: %d, %d, %d;\n", mutex, th[0], th[1], th[2]);

	sync = 0;
	thinkos_mutex_unlock(mutex);
	while (end < 3)
		thinkos_sleep(10);
	end = 0;

	thinkos_mutex_lock(mutex);
	printf("\n - First round done.\n");
	fib_i = 0;

	thinkos_mutex_unlock(mutex);
	while (sync < 3)
		thinkos_sleep(10);

	while (end < 3)
		thinkos_sleep(10);
	printf("\n - Second round done.\n");

	for (i = 0; i < 3; i++) {
		printf("  * join(%d) ...", th[i]);
		ret = thinkos_join(th[i]);
		printf(" %d\n", ret);
	}

	thinkos_mutex_free(mutex);

	printf("\n");
};

int main(int argc, char ** argv)
{
	DCC_LOG(LOG_TRACE, "cm3_udelay_calibrate()");
	cm3_udelay_calibrate();
	udelay(100000);
	DCC_LOG(LOG_TRACE, "stm32f_usart_open().");
	stdout = stm32f_usart_open(STM32F_UART5, 115200, SERIAL_8N1);
	udelay(100000);

	DCC_LOG(LOG_TRACE, "thinkos_init().");
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	printf("\n");
	printf("---------------------------------------------------------\n");
	printf(" ThinkOS (Cortex-M RTOS Mutex Test)\n");
	printf("---------------------------------------------------------\n");
	printf("\n");

	sleep_test();
	mutex_test();

	thinkos_sleep(10000);

	return 0;
}

