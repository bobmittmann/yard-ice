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
 * @file yard-ice.c
 * @brief YARD-ICE application main
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifdef CONFIG_H
#include "config.h"
#endif

#include <sys/stm32f.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/serial.h>
#include <sys/delay.h>
#include <sys/tty.h>
#include <sys/dcclog.h>
#include <thinkos.h>
#include <tcpip/net.h>
#include <tcpip/dhcp.h>
#include <tcpip/in.h>
#include <crc.h>
#include <md5.h>

#include "board.h"
#include "monitor.h"

#define VERSION_NUM "0.3"
#define VERSION_DATE "Mar, 2015"

const char * version_str = "HTTP Server Demo " \
							VERSION_NUM " - " VERSION_DATE;
const char * copyright_str = "(c) Copyright 2015 - Bob Mittmann";

void stdio_init(void);
int test_shell(FILE * f);
FILE * console_fopen(void);

volatile uint64_t buffer; /* production buffer */

int sem_empty; /* semaphore to signal an empty buffer */
int sem_full; /* semaphore to signal a full buffer */

volatile bool prod_done; /* production control flag */

int producer_task(void * arg)
{
	int prod_count;
	uint64_t y;
	unsigned int i = 0;
	uint64_t x0 = 0;
	uint64_t x1 = 0;

	prod_done = false;

	printf(" %s(): [%d] started...\n", __func__, thinkos_thread_self());
	thinkos_sleep(100);

	/* number of items to be produced */
	prod_count = 1000000000;

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

		DCC_LOG(LOG_INFO, "1. thinkos_sem_wait().");
		/* waiting for room to insert a new item */
		thinkos_sem_wait(sem_empty);

		/* insert the produced item in the buffer */
		buffer = y;

		DCC_LOG(LOG_INFO, "3. thinkos_sem_post().");
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

uint32_t consumer_stack[128];
const struct thinkos_thread_inf consumer_inf = {
	.stack_ptr = consumer_stack, 
	.stack_size = sizeof(consumer_stack), 
	.priority = 10,
	.thread_id = 8, 
	.paused = 0,
	.tag = "CONSUM"
};

uint32_t producer_stack[128];
const struct thinkos_thread_inf producer_inf = {
	.stack_ptr = producer_stack, 
	.stack_size = sizeof(producer_stack), 
	.priority = 10,
	.thread_id = 9, 
	.paused = 0,
	.tag = "PRODUC"
};

void semaphore_test(void)
{
	int producer_th;
	int consumer_th;

	/* allocate the empty signal semaphores */
	/* initialize the empty as 1 so we can insert an item immediately. */
	sem_empty = thinkos_sem_alloc(1); 

	/* allocate the full signal semaphores */
	/* initialize the full as 0 as we don't have produced anything yet. */
	sem_full = thinkos_sem_alloc(0); 

	/* create the producer thread */
	producer_th = thinkos_thread_create_inf(producer_task, NULL, 
											&producer_inf);

	/* create the consuer thread */
	consumer_th = thinkos_thread_create_inf(consumer_task, NULL, 
											&consumer_inf);

	printf(" * Empty semaphore: %d\n", sem_empty);
	printf(" * Full semaphore: %d\n", sem_full);
	printf(" * Producer thread: %d\n", producer_th);
	printf(" * Consumer thread: %d\n", consumer_th);
	printf("\n");
};



volatile uint32_t filt_y = 1;
volatile uint32_t filt_x = 1;
volatile uint32_t coef1 = 1234;
volatile uint32_t coef2 = 3455;

int busy_task(void * arg)
{
	uint32_t a0 = 123;
	uint32_t a1 = 455;
	uint32_t b0 = 111;
	uint32_t b1 = 222;
	volatile uint32_t x0 = 0;
	volatile uint32_t y0 = 0;
	volatile uint32_t x1 = 0;
	volatile uint32_t y1 = 0;
	uint32_t y;
	unsigned int i;

	printf(" %s(): [%d] started...\n", __func__, thinkos_thread_self());
	thinkos_sleep(100);

	for (i = 0; ; ++i) {
		x0 = filt_x;
		x0 += coef1;
		y =  x0 * a0;
		y += x1 * a1;
		y -= y0 * b0;
		y -= y1 * b1;
		filt_y = y + coef2;
		y0 = y;
		y1 = y0;
		x1 = x0;
	}

	return i;
}

uint32_t busy_stack[128];
const struct thinkos_thread_inf busy_inf = {
	.stack_ptr = busy_stack, 
	.stack_size = sizeof(busy_stack), 
	.priority = 32,
	.thread_id = 31, 
	.paused = 0,
	.tag = "BUSY"
};

void busy_test(void)
{
	int busy_th;

	/* create the busy thread */
	busy_th = thinkos_thread_create_inf(busy_task, NULL, &busy_inf);

	printf(" * Busy thread: %d\n", busy_th);
	printf("\n");
};

int sleep_task(void * arg)
{
	int i;
	int sum = 0;

	for (i = 0; i < 1000000000; ++i) {
		thinkos_sleep(10000);
	//	sum += rand();
	}

	return sum;
}


uint32_t sleep_stack[128];
const struct thinkos_thread_inf sleep_inf = {
	.stack_ptr = sleep_stack, 
	.stack_size = sizeof(sleep_stack), 
	.priority = 32,
	.thread_id = 31, 
	.paused = 0,
	.tag = "LAZY"
};

void sleep_test(void)
{
	int sleep_th;

	/* create the sleep thread */
	sleep_th = thinkos_thread_create_inf(sleep_task, NULL, &sleep_inf);

	printf(" * Lazy thread: %d\n", sleep_th);
	printf("\n");
};


int step_task(void * arg)
{
	asm volatile (
				  "1:\r\n"
				  "mov r0, #0\r\n" 
				  "mov r0, #1\r\n" 
				  "mov r0, #2\r\n" 
				  "mov r0, #3\r\n" 
				  "mov r0, #4\r\n" 
				  "mov r0, #5\r\n" 
				  "mov r0, #6\r\n" 
				  "mov r0, #7\r\n" 
				  "mov r0, #8\r\n" 
				  "mov r0, #9\r\n" 
				  "b 1b\r\n" 
				  : : );

	return 0;
}


uint32_t step_stack[128];
const struct thinkos_thread_inf step_inf = {
	.stack_ptr = step_stack, 
	.stack_size = sizeof(step_stack), 
	.priority = 32,
	.thread_id = 31, 
	.paused = 0,
	.tag = "STEP"
};

void step_test(void)
{
	int step_th;

	/* create the step thread */
	step_th = thinkos_thread_create_inf(step_task, NULL, &step_inf);

	printf(" * Lazy thread: %d\n", step_th);
	printf("\n");
};

int console_write(void * dev, const void * buf, unsigned int len); 
int console_read(void * dev, void * buf, unsigned int len, unsigned int msec);

#if 0
void console_file_recv(void)
{
	char buf[128];
	struct md5ctx md5;
	uint32_t r[4];
	int size = 0;
	int n;

	md5_init(&md5);

	while ((n = console_read(NULL, buf, 100, 5000)) > 0) { 
		md5_update(&md5, buf, n);
		size += n;
		thinkos_sleep(2);
	}

	md5_final((uint8_t *)r, &md5);

	r[0] = ntohl(r[0]);
	r[1] = ntohl(r[1]);
	r[2] = ntohl(r[2]);
	r[3] = ntohl(r[3]);
	DCC_LOG5(LOG_TRACE, "size=%d md5sum=%08x%08x%08x%08x.", size, 
			 r[0], r[1], r[2], r[3]);
}
#endif

int console_test_task(void * arg)
{
	struct tty_dev * tty;
	FILE * f_raw;
	FILE * f;

	DCC_LOG(LOG_TRACE, "...");

	f_raw = console_fopen();
	tty = tty_attach(f_raw);
	f = tty_fopen(tty);

#if 0
	thinkos_sleep(5000);

	DCC_LOG(LOG_TRACE, "=================================================");

	console_file_recv();

	console_write(NULL, 
				  "| 0123456789abcdefg | 0123456789ABCDEFG |" 
				  " 0123456789abcdefg | 0123456789ABCDEF |"
				  "| 0123456789abcdefg | 0123456789ABCDEFG |"
				  " 0123456789abcdefg | 0123456789ABCDEF |", 160);

	thinkos_sleep(1000);
	fprintf(f_raw, "\r\n");

	thinkos_sleep(1000);
	console_write(NULL, 
				  "| 0123456789abcdefg | 0123456789ABCDEFG |" 
				  " 0123456789abcdefg | 0123456789ABCDEF |"
				  "| 0123456789abcdefg | 0123456789ABCDEFG |"
				  " 0123456789abcdefg | 0123456789ABCDEF |", 160);
	thinkos_sleep(1000);
#endif

	for (;;) {
//		test_shell(f);
	}

	return 0;
}

uint32_t console_test_stack[512];

const struct thinkos_thread_inf console_test_inf = {
	.stack_ptr = console_test_stack, 
	.stack_size = sizeof(console_test_stack), 
	.priority = 32,
	.thread_id = 1, 
	.paused = 0,
	.tag = "SHELL"
};

void console_test(void)
{
	int thread_id;

	DCC_LOG(LOG_TRACE, "...");

	/* create the sleep thread */
	thread_id = thinkos_thread_create_inf(console_test_task, NULL, 
										  &console_test_inf);
	
	printf(" * Console test thread: %d\n", thread_id);
	printf("\n");
};

void io_init(void)
{
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOA);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOB);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOC);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOD);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOE);

#if 0
	/* JTAG TRST Pin */
	stm32_gpio_mode(MODSW, INPUT, SPEED_LOW);
#endif

	/* USART5 TX */
	stm32_gpio_mode(UART5_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_af(UART5_TX, GPIO_AF8);
	/* USART5 RX */
	stm32_gpio_mode(UART5_RX, ALT_FUNC, PULL_UP);
	stm32_gpio_af(UART5_RX, GPIO_AF8);

	/* USART6_TX */
	stm32_gpio_mode(UART6_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_af(UART6_TX, GPIO_AF7);
	/* USART6_RX */
	stm32_gpio_mode(UART6_RX, ALT_FUNC, PULL_UP);
	stm32_gpio_af(UART6_RX, GPIO_AF7);

}

int main(int argc, char ** argv)
{
//	struct cm3_dcb * dcb = CM3_DCB;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	cm3_udelay_calibrate();

	DCC_LOG(LOG_TRACE, "3. io_init()");
	io_init();

	DCC_LOG(LOG_TRACE, "2. thinkos_init().");
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	monitor_init();

	DCC_LOG(LOG_TRACE, "4. stdio_init().");
	stdio_init();

	printf("\n");
	printf("---------------------------------------------------------\n");
	printf(" ThinkOS (GDB Test)\n");
	printf("---------------------------------------------------------\n");
	printf("\n");

	DCC_LOG(LOG_TRACE, "5. starting tests...");

	/* Run the test */
//	semaphore_test();

//	sleep_test();

//	console_test();

//	busy_test();

//	step_test();

//	step_task(NULL);

	for (;;) {
		thinkos_sleep(2000);
//	thinkos_debug_step_i(0);
//		DCC_LOG(LOG_TRACE, "6. DCB_DEMCR_MON_REQ...");
//		dcb->demcr |= DCB_DEMCR_MON_REQ | DCB_DEMCR_MON_PEND;
//		dcb->demcr = demcr & ~DCB_DEMCR_MON_REQ;
//		thinkos_suspend_all();
//		test_shell(stdout);
	}

	return 0;
}


