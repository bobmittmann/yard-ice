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
#include <sys/dcclog.h>
#include <thinkos.h>
#include <tcpip/net.h>
#include <tcpip/dhcp.h>
#include <tcpip/in.h>

#include "board.h"
#include "monitor.h"

#define VERSION_NUM "0.3"
#define VERSION_DATE "Mar, 2015"

const char * version_str = "HTTP Server Demo " \
							VERSION_NUM " - " VERSION_DATE;
const char * copyright_str = "(c) Copyright 2015 - Bob Mittmann";

void stdio_init(void);
int stdio_shell(void);

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

		DCC_LOG(LOG_TRACE, "1. thinkos_sem_wait().");
		/* waiting for room to insert a new item */
		thinkos_sem_wait(sem_empty);

		/* insert the produced item in the buffer */
		buffer = y;

		DCC_LOG(LOG_TRACE, "3. thinkos_sem_post().");
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
uint32_t producer_stack[128];

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

void busy_test(void)
{
	int busy_th;

	/* create the busy thread */
	busy_th = thinkos_thread_create(busy_task, NULL, 
			busy_stack, sizeof(busy_stack));

	printf(" * Busy thread: %d\n", busy_th);
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

	/* Run the test */
	semaphore_test();

	busy_test();

	DCC_LOG(LOG_TRACE, "9. starting console shell...");
	for (;;) {
		stdio_shell();
//		gdb_rspd_start(stdout);
	}

	return 0;
}


