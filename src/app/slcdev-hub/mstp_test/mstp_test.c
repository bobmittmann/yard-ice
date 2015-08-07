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
 * but WITHOUT ANY WARRANTY; without even the implied warranty of * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */

/** 
 * @file mstp_test.c
 * @brief MS/TP test application
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stm32f.h>
#include <sys/delay.h>
#include <errno.h>

#include <thinkos.h>

#include <bacnet/mstp_lnk.h>
#include <sys/console.h>

#include "board.h"
#include "lattice.h"

/* -------------------------------------------------------------------------
 * stdio
 * ------------------------------------------------------------------------- */

void stdio_init(void)
{
	FILE * f;

	f = console_fopen();
	/* initialize STDIO */
	stderr = f;
	stdout = f;
	stdin = f;
}

/* -------------------------------------------------------------------------
 * MS/TP 
 * ------------------------------------------------------------------------- */

int mstp_task(void * arg)
{
	struct mstp_lnk * mstp = (struct mstp_lnk *)arg;

	printf("MS/TP task started...\n");

	mstp_lnk_loop(mstp);

	return 0;
}


extern const uint8_t ice40lp384_bin[];
extern const unsigned int sizeof_ice40lp384_bin;

struct serial_dev * rs485_init(void)
{
	struct serial_dev * ser;

    /* IO init */
    stm32_gpio_mode(IO_RS485_RX, ALT_FUNC, PULL_UP);
    stm32_gpio_af(IO_RS485_RX, GPIO_AF8);

    stm32_gpio_mode(IO_RS485_TX, ALT_FUNC, PUSH_PULL | SPEED_MED);
    stm32_gpio_af(IO_RS485_TX, GPIO_AF8);

    stm32_gpio_mode(IO_RS485_MODE, OUTPUT, PUSH_PULL | SPEED_LOW);
    stm32_gpio_set(IO_RS485_MODE);

    lattice_ice40_configure(ice40lp384_bin, sizeof_ice40lp384_bin);

	ser = stm32f_uart1_serial_init(500000, SERIAL_8N1);
//	ser = stm32f_uart1_serial_dma_init(500000, SERIAL_8N1);
//	ser = stm32f_uart7_serial_init(500000, SERIAL_8N1);

	return ser;
}

uint32_t mstp_stack[512];

struct mstp_lnk * mstp_start(int addr)
{
	const struct thinkos_thread_inf mstp_inf = {
		.stack_ptr = mstp_stack, 
		.stack_size = sizeof(mstp_stack), 
		.priority = 8,
		.thread_id = 1, 
		.paused = 0,
		.tag = "MS/TP"
	};
	struct serial_dev * ser;
	struct mstp_lnk * mstp;


	printf("1. rs485_init() ...\n");
	ser = rs485_init();

	printf("2. mstp_lnk_alloc() ...\n");
	mstp = mstp_lnk_alloc();

	printf("3. MS/TP link addr: %d ...\n", addr);
	mstp_lnk_init(mstp, "MS/TP1", addr, ser);

	printf("4. thinkos_thread_create_inf()\n");
	thinkos_thread_create_inf(mstp_task, mstp, &mstp_inf);

	thinkos_sleep(100);

	printf("5. mstp_lnk_resume()\n");
	mstp_lnk_resume(mstp);

	return mstp;
}

/* -------------------------------------------------------------------------
 * Test
 * ------------------------------------------------------------------------- */
void io_init(void)
{
	thinkos_sysinfo_udelay_factor(&udelay_factor);

	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOA);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOB);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOC);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOD);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOE);
}

int recv_task(void * arg)
{
	struct mstp_lnk * mstp = (struct mstp_lnk *)arg;
	struct mstp_frame_inf inf;
	uint8_t pdu[508];
	int len;

	printf("receive task started...\n");

	mstp_lnk_loop(mstp);
	for (;;) {
		len = mstp_lnk_recv(mstp, pdu, sizeof(pdu), &inf);
	
		printf("RCV: %d (%d)..\n", inf.saddr, len);
	}

	return 0;
}

uint32_t recv_stack[512];

const struct thinkos_thread_inf recv_inf = {
	.stack_ptr = recv_stack, 
	.stack_size = sizeof(recv_stack), 
	.priority = 32,
	.thread_id = 8, 
	.paused = 0,
	.tag = "RECV"
};

int main(int argc, char ** argv)
{
	struct mstp_lnk * mstp;
	struct mstp_frame_inf inf;
	uint8_t mstp_addr = 1;
	uint8_t pdu[508];
	int len;
	int i;

	io_init();

	stdio_init();

	mstp = mstp_start(mstp_addr);

	thinkos_thread_create_inf(recv_task, mstp, &recv_inf);

	for (i = 0;; ++i) {
		inf.daddr = MSTP_ADDR_BCAST;
		inf.type = FRM_DATA_NO_REPLY;
		len = sprintf((char *)pdu, 
					  "%4d The quick brown fox jumps over the lazy dog.",
					  i);
		len++;
		mstp_lnk_send(mstp, pdu, len, &inf);
	
		thinkos_sleep(1000);
	}

	return 0;
}


