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
 * @brief YARD-ICE UART console
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifdef CONFIG_H
#include "config.h"
#endif

#include <sys/stm32f.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/serial.h>
#include <sys/param.h>

#include <thinkos.h>
#define __THINKOS_IRQ__
#include <thinkos_irq.h>

#include <sys/dcclog.h>

#if (THINKOS_FLAG_MAX > 0) && (THINKOS_ENABLE_FLAG_ALLOC)

#ifndef UART_CONSOLE_TX_MUTEX
#define UART_CONSOLE_TX_MUTEX     1
#endif

#ifndef UART_CONSOLE_TX_FIFO_LEN
#define UART_CONSOLE_TX_FIFO_LEN  512
#endif

#ifndef UART_CONSOLE_RX_FIFO_LEN
#define UART_CONSOLE_RX_FIFO_LEN  16
#endif

#ifndef UART_CONSOLE_DEV         
#define UART_CONSOLE_DEV          STM32_UART5
#endif

#ifndef UART_CONSOLE_ISR
#define UART_CONSOLE_ISR          stm32f_uart5_isr
#endif

#ifndef UART_CONSOLE_IRQ_NUM
#define UART_CONSOLE_IRQ_NUM      STM32_IRQ_UART5
#endif

#ifndef UART_CONSOLE_IRQ_PRIORITY 
#define UART_CONSOLE_IRQ_PRIORITY IRQ_PRIORITY_REGULAR
#endif

struct uart_console_dev {
	uint8_t tx_flag;
	uint8_t rx_flag;
#if UART_CONSOLE_TX_MUTEX
	uint8_t tx_mutex;
#endif
	uint32_t * txie;
	struct stm32_usart * uart;
	struct {
		volatile uint32_t head;
		volatile uint32_t tail;
		uint8_t buf[UART_CONSOLE_TX_FIFO_LEN];
	} tx_fifo;	
	struct {
		volatile uint32_t head;
		volatile uint32_t tail;
		uint8_t buf[UART_CONSOLE_RX_FIFO_LEN];
	} rx_fifo;	
};

static int uart_console_read(struct uart_console_dev * dev, char * buf, 
				 unsigned int len, unsigned int tmo)
{
	uint8_t * cp = (uint8_t *)buf;
	unsigned int tail;
	int ret;
	int cnt;
	int n;
	int i;

	DCC_LOG2(LOG_INFO, "1. len=%d tmo=%d", len, tmo);

again:
	if ((ret = thinkos_flag_timedtake(dev->rx_flag, tmo)) < 0) {
		DCC_LOG1(LOG_TRACE, "cnt=%d, timeout!", 
				 (int32_t)(dev->rx_fifo.head - dev->rx_fifo.tail));
		return ret;
	}

	tail = dev->rx_fifo.tail;
	cnt = (int32_t)(dev->rx_fifo.head - tail);
	if (cnt == 0) {
		DCC_LOG(LOG_WARNING, "RX FIFO empty!");
		goto again;
	}
	n = MIN(len, cnt);

	for (i = 0; i < n; ++i)
		cp[i] = dev->rx_fifo.buf[tail++ & (UART_CONSOLE_RX_FIFO_LEN - 1)];

	dev->rx_fifo.tail = tail;

	if (cnt > n) {
		DCC_LOG3(LOG_TRACE, "len=%d cnt=%d n=%d", len, cnt, n);
		thinkos_flag_give(dev->rx_flag);
	} else {
		DCC_LOG1(LOG_INFO, "2. n=%d", n);
	}

	return n;
}

static int uart_console_write(struct uart_console_dev * dev, const void * buf, 
					   unsigned int len)
{
	uint8_t * cp = (uint8_t *)buf;
	int rem = len;

	DCC_LOG1(LOG_INFO, "len=%d", len);

	while (rem) {
		unsigned int head;
		int free;
		int n;
		int i;

		thinkos_flag_take(dev->tx_flag);

		head = dev->tx_fifo.head;
		free = UART_CONSOLE_TX_FIFO_LEN - (int32_t)(head - dev->tx_fifo.tail);
		DCC_LOG3(LOG_MSG, "head=%d tail=%d n=%d", head, dev->tx_fifo.tail, n);
		n = MIN(rem, free);
		for (i = 0; i < n; ++i) 
			dev->tx_fifo.buf[head++ & (UART_CONSOLE_TX_FIFO_LEN - 1)] = *cp++;
		dev->tx_fifo.head = head;
		*dev->txie = 1; 

		if (free > n)
			thinkos_flag_give(dev->tx_flag);

		rem -= n;
		DCC_LOG1(LOG_INFO, "rem=%d", rem);
	}


	return len;
}

static int uart_console_flush(struct uart_console_dev * dev)
{
	do {
		thinkos_flag_take(dev->tx_flag);
	} while (((int32_t)dev->tx_fifo.head - dev->tx_fifo.tail) > 0);

	stm32_usart_flush(dev->uart);

	thinkos_flag_give(dev->tx_flag);

	return 0;
}

static int uart_console_close(struct uart_console_dev * dev)
{
	return 0;
}

struct uart_console_dev uart_console_dev;

void UART_CONSOLE_ISR(void)
{
	struct uart_console_dev * dev = &uart_console_dev;
	struct stm32_usart * us = dev->uart;
	uint32_t sr;
	int c;
	
	sr = us->sr;

	if (sr & USART_ORE) {
		DCC_LOG(LOG_WARNING, "OVR!");
	}

	sr &= us->cr1;

	if (sr & USART_RXNE) {
		unsigned int head;
		int free;

		c = us->dr;

		head = dev->rx_fifo.head;
		free = UART_CONSOLE_RX_FIFO_LEN - (uint8_t)(head - dev->rx_fifo.tail);
		if (free > 0) { 
			dev->rx_fifo.buf[head & (UART_CONSOLE_RX_FIFO_LEN - 1)] = c;
			dev->rx_fifo.head = head + 1;
		} else {
			DCC_LOG(LOG_WARNING, "RX fifo full!");
		}
		if (free < (UART_CONSOLE_RX_FIFO_LEN / 2)) /* fifo is more than half full */
			__thinkos_flag_give(dev->rx_flag);
	}	

	if (sr & USART_IDLE) {
		DCC_LOG(LOG_INFO, "IDLE!");
		c = us->dr;
		(void)c;
		__thinkos_flag_give(dev->rx_flag);
	}

	if (sr & USART_TXE) {
		unsigned int tail = dev->tx_fifo.tail;
		if (tail == dev->tx_fifo.head) {
			/* FIFO empty, disable TXE interrupts */
			*dev->txie = 0; 
			__thinkos_flag_set(dev->tx_flag);
		} else {
			us->dr = dev->tx_fifo.buf[tail & (UART_CONSOLE_TX_FIFO_LEN - 1)];
			dev->tx_fifo.tail = tail + 1;
		}
	}
}


/* ----------------------------------------------------------------------
 * Console device 
 * ----------------------------------------------------------------------
 */

struct uart_console_dev * uart_console_init(unsigned int baudrate, 
											unsigned int flags)
{
	struct uart_console_dev * dev = &uart_console_dev;
	struct stm32_usart * uart = UART_CONSOLE_DEV;

	DCC_LOG1(LOG_TRACE, "UART=0x%08x", uart);

	dev->uart = uart;
	dev->rx_flag = thinkos_flag_alloc(); 
	dev->tx_flag = thinkos_flag_alloc(); 
#if UART_CONSOLE_TX_MUTEX
	dev->tx_mutex = thinkos_mutex_alloc(); 
	DCC_LOG1(LOG_TRACE, "tx_mutex=%d", dev->tx_mutex);
#endif

	dev->tx_fifo.head = dev->tx_fifo.tail = 0;
	dev->rx_fifo.head = dev->rx_fifo.tail = 0;
	dev->txie = CM3_BITBAND_DEV(&uart->cr1, 7);
	thinkos_flag_give(dev->tx_flag);

	stm32_usart_init(uart);
	stm32_usart_baudrate_set(uart, baudrate);
	stm32_usart_mode_set(uart, SERIAL_8N1);

	/* enable RX interrupt */
	uart->cr1 |= USART_RXNEIE | USART_IDLEIE;

	/* enable UART */
	stm32_usart_enable(uart);

	/* configure interrupts */
	cm3_irq_pri_set(UART_CONSOLE_IRQ_NUM, UART_CONSOLE_IRQ_PRIORITY);
	/* enable interrupts */
	cm3_irq_enable(UART_CONSOLE_IRQ_NUM);

	return dev;
}

/* ----------------------------------------------------------------------
 * Console file operations 
 * ----------------------------------------------------------------------
 */

const struct fileop uart_console_ops = {
	.write = (void *)uart_console_write,
	.read = (void *)uart_console_read,
	.flush = (void *)uart_console_flush,
	.close = (void *)uart_console_close
};

struct file * uart_console_fopen(struct uart_console_dev * dev)
{
	return file_alloc(dev, &uart_console_ops);
}

#endif /* (THINKOS_FLAG_MAX > 0) && (THINKOS_ENABLE_FLAG_ALLOC) */

