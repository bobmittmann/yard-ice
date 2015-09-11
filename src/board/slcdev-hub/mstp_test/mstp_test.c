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
#include <trace.h>

#include "board.h"

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
 * System Supervision
 * ------------------------------------------------------------------------- */

const char * const trace_lvl_tab[] = {
		"   NONE",
		"  ERROR",
		"WARNING",
		"   INFO",
		"  DEBUG"
};

void __attribute__((noreturn)) supervisor_task(void)
{
	struct trace_entry trace;
	uint32_t clk;

	INF("<%d> started...", thinkos_thread_self());

	trace_tail(&trace);

	clk = thinkos_clock();
	for (;;) {
		struct timeval tv;
		char s[80];

		/* 8Hz periodic task */
		clk += 125;
		thinkos_alarm(clk);

		while (trace_getnext(&trace, s, sizeof(s)) >= 0) {
			trace_ts2timeval(&tv, trace.dt);
			printf("%s %2d.%06d: %s\n", trace_lvl_tab[trace.ref->lvl],
					(int)tv.tv_sec, (int)tv.tv_usec, s);
		}

		trace_flush(&trace);
	}
}


uint32_t supervisor_stack[128];

const struct thinkos_thread_inf supervisor_inf = {
	.stack_ptr = supervisor_stack,
	.stack_size = sizeof(supervisor_stack),
	.priority = 32,
	.thread_id = 31,
	.paused = false,
	.tag = "SUPV"
};

void supervisor_init(void)
{
	trace_init();

	thinkos_thread_create_inf((void *)supervisor_task, (void *)NULL,
							  &supervisor_inf);
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

struct serial_dev * rs485_init(void)
{
	struct serial_dev * ser;

    /* IO init */
    stm32_gpio_mode(IO_RS485_RX, ALT_FUNC, PULL_UP);
    stm32_gpio_af(IO_RS485_RX, RS485_USART_AF);

    stm32_gpio_mode(IO_RS485_TX, ALT_FUNC, PUSH_PULL | SPEED_MED);
    stm32_gpio_af(IO_RS485_TX, RS485_USART_AF);

    stm32_gpio_mode(IO_RS485_MODE, OUTPUT, PUSH_PULL | SPEED_LOW);
    stm32_gpio_set(IO_RS485_MODE);

//	ser = stm32f_uart1_serial_init(500000, SERIAL_8N1);
	ser = stm32f_uart1_serial_dma_init(500000, SERIAL_8N1);
//	ser = stm32f_uart7_serial_init(500000, SERIAL_8N1);

	return ser;
}

uint32_t mstp_stack[512];

const struct thinkos_thread_inf mstp_inf = {
	.stack_ptr = mstp_stack,
	.stack_size = sizeof(mstp_stack),
	.priority = 8,
	.thread_id = 1,
	.paused = 0,
	.tag = "MS/TP"
};

struct mstp_lnk * mstp_start(int addr)
{
	struct serial_dev * ser;
	struct mstp_lnk * mstp;

	printf("1. rs485_init() ...\n");
	if ((ser = rs485_init()) == NULL) {
		return NULL;
	}

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
	thinkos_udelay_factor(&udelay_factor);

	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOA);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOB);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOC);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOD);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOE);

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

int recv_task(void * arg)
{
	struct mstp_lnk * mstp = (struct mstp_lnk *)arg;
	struct mstp_frame_inf inf;
	uint8_t pdu[502];
	int len;
	unsigned int octet_cnt = 0;
	unsigned int bcast_cnt = 0;
	unsigned int unicast_cnt = 0;
	unsigned int seconds = 10;
	uint32_t clk;

	printf("receive task started...\n");

	clk = thinkos_clock() + seconds * 1000;
	for (;;) {
		len = mstp_lnk_recv(mstp, pdu, sizeof(pdu), &inf);
		octet_cnt += len;
		if (inf.daddr == MSTP_ADDR_BCAST)
			bcast_cnt++;
		else
			unicast_cnt++;

//		printf("RCV: %d->%d (%d) \"%s\"\n", inf.saddr, inf.daddr,
//				len, (char*)pdu);
//		DBG("RCV: %d->%d (%d)", inf.saddr, inf.daddr, len);
		/* Log summary each 5 seconds */
		if ((int32_t)(clk - thinkos_clock()) <= 0) {
			unsigned int rate;
			rate = octet_cnt / seconds;

			DBG("Receive: %d bcast, %d unicast, %d octets, %d.%03d KBps.",
					bcast_cnt, unicast_cnt, octet_cnt,
					rate / 1000, rate % 1000);

			bcast_cnt = 0;
			unicast_cnt = 0;
			octet_cnt = 0;
			seconds = 10;
			clk += seconds * 1000;
		}
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

struct board_cfg {
    uint32_t magic;
    uint32_t mstp_addr;
    uint32_t ip_addr;
};

#define CFG_MAGIC 0x01020304
#define CFG_ADDR  0x0800ff00


int main(int argc, char ** argv)
{
	struct board_cfg * cfg = (struct board_cfg *)(CFG_ADDR);
	struct mstp_lnk * mstp;
	struct mstp_frame_inf inf;
	uint8_t mstp_addr = 1;
	uint8_t pdu[502];
	uint32_t clk;
	uint32_t tmo;
	unsigned int octet_cnt = 0;
	unsigned int bcast_cnt = 0;
	unsigned int itval = 5;
	int len;
	int i;

	if (cfg->magic == CFG_MAGIC)
		mstp_addr = cfg->mstp_addr;
	else
		mstp_addr = *((uint32_t *)STM32F_UID) & 0x1f;

	io_init();

	stdio_init();

	supervisor_init();

	INF("Starting MS/TP test");

	if ((mstp = mstp_start(mstp_addr)) == NULL) {
		thinkos_sleep(1000);
		return 1;
	}

	thinkos_thread_create_inf(recv_task, mstp, &recv_inf);

	clk = thinkos_clock();
	tmo = clk + itval * 1000;

	for (i = 0;; ++i) {
		inf.daddr = MSTP_ADDR_BCAST;
		inf.type = FRM_DATA_NO_REPLY;
		len = snprintf((char *)pdu, 501, "%6d"
					  " The quick brown fox jumps over the lazy dog."
					  " The quick brown fox jumps over the lazy dog."
					  " The quick brown fox jumps over the lazy dog."
				  	  " The quick brown fox jumps over the lazy dog."
				  	  " The quick brown fox jumps over the lazy dog."
				      " The quick brown fox jumps over the lazy dog."
					  " The quick brown fox jumps over the lazy dog."
					  " The quick brown fox jumps over the lazy dog."
					  " The quick brown fox jumps over the lazy dog."
				      " The quick brown fox jumps over the lazy dog."
					  " The quick brown fox jumps over the lazy dog.",
					  i);
		if (mstp_lnk_send(mstp, pdu, len, &inf) < 0) {
			ERR("mstp_lnk_send() failed!");
		}
		octet_cnt += len;
		bcast_cnt++;

		/* Log summary each 10 seconds */
		if ((int32_t)(tmo - clk) <= 0) {
			unsigned int rate;

			rate = octet_cnt / itval;

			DBG("Send: %d bcasts, %d octets, %d.%03d KBps.", bcast_cnt, octet_cnt,
					rate / 1000, rate % 1000);

			octet_cnt = 0;
			bcast_cnt = 0;
			itval = 10;
			tmo += itval * 1000;
		}
		clk = thinkos_clock();
	}

	return 0;
}


