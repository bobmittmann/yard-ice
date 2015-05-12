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

int sem_empty; /* semaphore to signal an empty buffer */
int sem_full; /* semaphore to signal a full buffer */

int prod_count;
volatile bool prod_done; /* production control flag */

int producer_task(void * arg)
{
	uint64_t y;
	unsigned int i = 0;
	uint64_t x0 = 0;
	uint64_t x1 = 0;

	prod_done = false;

	printf(" %s(): [%d] started...\n", __func__, thinkos_thread_self());
	thinkos_sleep(100);

	for (i = 0; i < prod_count; i++) {
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

		/* waiting for room to insert a new item */
		thinkos_sem_wait(sem_empty);

		/* insert the produced item in the buffer */
		buffer = y;

		/* signal a full buffer */
		thinkos_sem_post(sem_full);
	}

	prod_done = true;

	return i;
}

int consumer_task(void * arg)
{
	int i = 0;

	printf(" %s(): [%d] started...\n", __func__, thinkos_thread_self());
	thinkos_sleep(100);

	/* set the production enable flag to start production */
	do {
		printf(" %3d ", i);
		/* wait for an item to be produced */
		while (thinkos_sem_timedwait(sem_full, 50) == THINKOS_ETIMEDOUT) {
			printf(".");
		}

		/* unload the buffer */
		printf(" %016llx %llu\n", buffer, buffer);
		i++;
		/* signal the empty buffer */
		thinkos_sem_post(sem_empty);
	} while (!prod_done);

	/* get the last produced item, if any */
	if (thinkos_sem_timedwait(sem_full, 0) == 0) {
		printf(" %3d ", i);
		printf(" %016llx %llu\n", buffer, buffer);
		i++;
		thinkos_sem_post(sem_empty);
	}

	return i;
};

uint32_t producer_stack[128];
uint32_t consumer_stack[128];

void semaphore_test(void)
{
	int producer_th;
	int consumer_th;
	int producer_ret;
	int consumer_ret;

	/* allocate the empty signal semaphores */
	/* initialize the empty as 1 so we can insert an item immediately. */
	sem_empty = thinkos_sem_alloc(1); 

	/* allocate the full signal semaphores */
	/* initialize the full as 0 as we don't have produced anything yet. */
	sem_full = thinkos_sem_alloc(0); 

	/* create the producer thread */
	producer_th = thinkos_thread_create(producer_task, NULL, 
			producer_stack, sizeof(producer_stack));

	/* create the consuer thread */
	consumer_th = thinkos_thread_create(consumer_task, NULL, 
			consumer_stack, sizeof(consumer_stack));

	printf(" * Empty semaphore: %d\n", sem_empty);
	printf(" * Full semaphore: %d\n", sem_full);
	printf(" * Producer thread: %d\n", producer_th);
	printf(" * Consumer thread: %d\n", consumer_th);
	printf("\n");
	thinkos_sleep(100);

	/* number of items to be produced */
	prod_count = 100;

	/* wait for the production thread to finish */
	producer_ret = thinkos_join(producer_th);

	/* wait for the consumer thread to finish */
	consumer_ret = thinkos_join(consumer_th);

	printf(" * Production return = %d\n", producer_ret);
	printf(" * Consumer return = %d\n", consumer_ret);

	/* release the semaphores */
	thinkos_sem_free(sem_empty);
	thinkos_sem_free(sem_full);

	printf("\n");
};

void stdio_init(void);

int main(int argc, char ** argv)
{
	/* Calibrate the the uDelay loop */
	cm3_udelay_calibrate();

	/* Initialize the stdin, stdout and stderr */
	stdio_init();

	/* Print a useful information message */
	printf("\n");
	printf("---------------------------------------------------------\n");
	printf(" ThinkOS - Semaphore example\n");
	printf("---------------------------------------------------------\n");
	printf("\n");

	/* Initialize the ThinkOS kernel */
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	/* Run the test */
	semaphore_test();

	thinkos_sleep(10000);

	return 0;
}

