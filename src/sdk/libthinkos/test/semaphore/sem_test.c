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

volatile uint64_t buffer; /* production buffer */
volatile bool prod_enabled; /* producer control flag */

int sem_empty; /* semaphore to signal an empty buffer */
int sem_full; /* semaphore to signal a full buffer */


int producer_task(void * arg)
{
	uint64_t y;
	unsigned int i = 0;
	uint64_t x0 = 0;
	uint64_t x1 = 0;

	printf(" %s(): [%d] started...\n", __func__, thinkos_thread_self());
	thinkos_sleep(100);

	while (prod_enabled) {
		/* let's spend some time thinking */
		thinkos_sleep(500);

		/* working */
		if (i == 0)
			y = 0;
		else if (i == 1)
			y = 1;
		else
			y = x1 + x0;

		x0 = x1;
		x1 = y;
		i++;

		/* waiting for room to insert a new item */
		thinkos_sem_wait(sem_empty);

//		printf(" %s(): [%d] produced\n", __func__, thinkos_thread_self());

		/* insert the produced item in the buffer */
		buffer = y;

		/* signall a full buffer */
		thinkos_sem_post(sem_full);
	}

	return i;
}

#define STACK_SIZE 512
uint32_t prod_stack[STACK_SIZE / 4];

void semaphore_test(void)
{
	int prod_th;
	int ret;
	int i = 0;

	/* allocate the empty signal semaphores */
	sem_empty = thinkos_sem_alloc(1); /* initialize the empty as 1
										 so we can insert an item
										 immediately */

	/* allocate the full signal semaphores */
	sem_full = thinkos_sem_alloc(0); /* initialize the full as 0
										as we don't haven't produced
										anything yet. */

	/* set the production enable flag */
	prod_enabled = true;

	/* create the producer thread */
	prod_th = thinkos_thread_create(producer_task, NULL, 
							   prod_stack, STACK_SIZE, 0);

	printf(" * Empty semaphore: %d\n", sem_empty);
	printf(" * Full semaphore: %d\n", sem_full);
	printf(" * Producer thread: %d\n", prod_th);
	printf("\n");
	thinkos_sleep(100);

	for (i = 0; i < 100; i++) {
		printf(" %3d ", i);
		/* wait for an item to be produced */
		while (thinkos_sem_timedwait(sem_full, 50) == THINKOS_ETIMEDOUT) {
			printf(".");
		}

//		thinkos_sem_wait(sem_full);
			   
		/* unload the buffer */
		printf(" %016llx %llu\n", buffer, buffer);
		/* signal the empty buffer */
		thinkos_sem_post(sem_empty);
	}

	/* clear the enable flag to stop production */
	prod_enabled = false;

	/* wait for the production thread to finish */
	ret = thinkos_join(prod_th);
	printf(" * production return = %d\n", ret);

	/* get the last produced item, if any */
	if (thinkos_sem_timedwait(sem_full, 0) == 0) {
		thinkos_sem_post(sem_empty);
		printf(" %lld\n", buffer);
	}

	/* release the semaphores */
	thinkos_sem_free(sem_empty);
	thinkos_sem_free(sem_full);

	printf("\n");
};

int main(int argc, char ** argv)
{
	cm3_udelay_calibrate();
	stdout = stm32f_usart_open(STM32F_UART5, 115200, SERIAL_8N1);

	printf("\n");
	printf("---------------------------------------------------------\n");
	printf(" ThinkOS - Semaphore example\n");
	printf("---------------------------------------------------------\n");
	printf("\n");

	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

#if 0
	printf("1.\n");
	thinkos_sleep(1000);
	printf("2.\n");
	thinkos_sleep(1000);
	printf("3.\n");
	thinkos_sleep(1000);
#endif

	semaphore_test();

//	thinkos_sleep(10000);
	delay(10);

	return 0;
}

