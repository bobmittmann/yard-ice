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

#include <stdlib.h>
#include <thinkos.h>
#include <sys/console.h>
#include <sys/stm32f.h>
#include <sys/delay.h>

#include "board.h"

extern void usleep(unsigned int usec);

void io_init(void)
{
	asm volatile ("mov r1, #1\r\n" : : : "r1");
	asm volatile ("mov r1, #2\r\n" : : : "r1");
	asm volatile ("mov r1, #3\r\n" : : : "r1");
	asm volatile ("mov r1, #4\r\n" : : : "r1");
	asm volatile ("mov r1, #5\r\n" : : : "r1");

	thinkos_udelay_factor(&udelay_factor);

	asm volatile ("mov r0, #5\r\n" : : : "r0");
	asm volatile ("mov r0, #6\r\n" : : : "r0");
	asm volatile ("mov r0, #7\r\n" : : : "r0");
	asm volatile ("mov r0, #8\r\n" : : : "r0");
	asm volatile ("mov r0, #9\r\n" : : : "r0");

	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOG);
}

#if 0
/* -------------------------------------------------------------------------
 * Step demo
 * ------------------------------------------------------------------------- */

int step_task(void * arg)
{
	for (;;) {
		thinkos_sleep(50);
		asm volatile ("mov r0, #0\r\n" : : : "r0");
		asm volatile ("mov r0, #1\r\n" : : : "r0");
		asm volatile ("mov r0, #2\r\n" : : : "r0");
		asm volatile ("mov r0, #3\r\n" : : : "r0");
		asm volatile ("mov r0, #4\r\n" : : : "r0");
		asm volatile ("mov r0, #5\r\n" : : : "r0");
		asm volatile ("mov r0, #6\r\n" : : : "r0");
		asm volatile ("mov r0, #7\r\n" : : : "r0");
		asm volatile ("mov r0, #8\r\n" : : : "r0");
		asm volatile ("mov r0, #9\r\n" : : : "r0");
	}
	return 0;
}


void init_step_task(void)
{
	static uint32_t step_stack[128];

	static const struct thinkos_thread_inf step_inf = {
		.stack_ptr = step_stack, 
		.stack_size = sizeof(step_stack), 
		.priority = 8,
		.thread_id = 31, 
		.paused = 0,
		.tag = "STEP"
	};

	thinkos_thread_create_inf(step_task, NULL, &step_inf);
}

/* -------------------------------------------------------------------------
 * Fifo demo
 * ------------------------------------------------------------------------- */

#define FIFO_LEN 32

struct fifo {
	int sem_free; /* semaphore to signal an empty buffer */
	int sem_avail; /* semaphore to signal a full buffer */
	int put_mutex;
	int get_mutex;
	uint32_t head;
	uint32_t tail;
	uint64_t buf[FIFO_LEN];
};

void fifo_put(struct fifo * q, uint64_t val)
{
	/* waiting for room to insert a new item */
	thinkos_sem_wait(q->sem_free);
	thinkos_mutex_lock(q->put_mutex);
	q->buf[q->head++ % FIFO_LEN] = val;
	thinkos_mutex_unlock(q->put_mutex);
	/* signal data availability */
	thinkos_sem_post(q->sem_avail);
}

uint64_t fifo_get(struct fifo * q)
{
	uint64_t val;
	/* waiting for data availability */
	thinkos_sem_wait(q->sem_avail);
	thinkos_mutex_lock(q->get_mutex);
	val = q->buf[q->tail++ % FIFO_LEN];
	thinkos_mutex_unlock(q->get_mutex);
	/* signal free space */
	thinkos_sem_post(q->sem_free);

	return val;
}

void fifo_init(struct fifo * q)
{
	/* waiting for data availability */
	q->sem_avail = thinkos_sem_alloc(0);
	q->sem_free = thinkos_sem_alloc(FIFO_LEN);
	q->get_mutex = thinkos_mutex_alloc();
	q->put_mutex = thinkos_mutex_alloc();
	q->tail = 0;
	q->head = 0;
}

int producer_task(struct fifo * q)
{
	uint64_t y = 0;
	uint64_t x0 = 0;
	uint64_t x1 = 1;

	for (;;) {
		/* let's spend some time thinking */
		thinkos_sleep(2);

		/* insert the produced item in the queue */
		fifo_put(q, y);

		y = x1 + x0;
		x0 = x1;
		x1 = y;
	}

	return 0;
}

int consumer_task(struct fifo * q)
{
	uint64_t y = 0;

	for (;;) {
		/* insert the produced item in the queue */
		y = fifo_get(q);
		(void)y;

		/* unload the buffer */
//		printf(" %016llx %llu\n", y, y);
	}
	
	return 0;
};

uint32_t producer_stack[128];
uint32_t consumer_stack[128];
struct fifo queue;

static const struct thinkos_thread_inf consumer_inf = {
	.stack_ptr = consumer_stack, 
	.stack_size = sizeof(consumer_stack), 
	.priority = 8,
	.thread_id = 20, 
	.paused = 0,
	.tag = "CONSUME"
};

static const struct thinkos_thread_inf producer_inf = {
	.stack_ptr = producer_stack, 
	.stack_size = sizeof(producer_stack), 
	.priority = 8,
	.thread_id = 21, 
	.paused = 0,
	.tag = "PRODUCE"
};

void init_queue_test(void)
{
	fifo_init(&queue);
	thinkos_thread_create_inf((void *)consumer_task, &queue, &consumer_inf);
	thinkos_thread_create_inf((void *)producer_task, &queue, &producer_inf);
};
#endif

volatile unsigned int cnt1 = 0;
volatile unsigned int cnt2 = 0;

int main(int argc, char ** argv)
{
	io_init();

//	init_step_task();

//	init_queue_test();

	asm volatile (
			"1:\r\n"
			"mov r0, #0\r\n"
			"mov r0, #1\r\n"
			"mov r0, #2\r\n"
			"mov r0, #2\r\n"
			"mov r0, #3\r\n"
			"mov r0, #4\r\n"
			"mov r0, #5\r\n"
			"mov r0, #6\r\n"
			"mov r0, #7\r\n"
			"mov r0, #8\r\n"
			"mov r0, #9\r\n"
//			"b 1b\r\n"
			: : : "r0");

	for (;;) {
		usleep(4000000);
//		thinkos_sleep(2000);
		cnt1++;
		thinkos_sleep(2000);
		cnt2++;
	}
	return 0;
}


