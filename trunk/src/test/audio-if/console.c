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
 * @file console.c
 * @brief YARD-ICE UART console
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <sys/stm32f.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <arch/cortex-m3.h>
#include <sys/serial.h>
#include <sys/delay.h>

#include <thinkos.h>
#define __THINKOS_IRQ__
#include <thinkos_irq.h>

#include <sys/dcclog.h>

#ifndef ENABLE_UART_TX_BLOCK
#define ENABLE_UART_TX_BLOCK 0
#endif

#define UART_TX_FIFO_BUF_LEN 8192
#define UART_RX_FIFO_BUF_LEN 64

struct uart_fifo {
	volatile uint32_t head;
	volatile uint32_t tail;
	uint32_t mask;
	uint32_t len;
	uint8_t buf[];
};

static inline void uart_fifo_init(struct uart_fifo * fifo, int len)
{
	fifo->head = 0;
	fifo->tail = 0;
	fifo->len = len;
	fifo->mask = len - 1;
}

static inline uint8_t uart_fifo_get(struct uart_fifo * fifo)
{
	return fifo->buf[fifo->tail++ & fifo->mask];
}

static inline void uart_fifo_put(struct uart_fifo * fifo, int c)
{
	fifo->buf[fifo->head++ & fifo->mask] = c;
}

static inline bool uart_fifo_is_empty(struct uart_fifo * fifo)
{
	return (fifo->tail == fifo->head) ? true : false;
}

static inline bool uart_fifo_is_full(struct uart_fifo * fifo)
{
	return ((fifo->head - fifo->tail) == fifo->len) ? true : false;
}

static inline bool uart_fifo_is_half_full(struct uart_fifo * fifo)
{
	return ((fifo->head - fifo->tail) > (fifo->len / 2)) ? true : false;
}

struct uart_console_dev {
	int32_t tx_ev;
	int32_t rx_ev;
	struct uart_fifo tx_fifo;
	uint8_t tx_buf[UART_TX_FIFO_BUF_LEN];
	struct uart_fifo rx_fifo;
	uint8_t rx_buf[UART_RX_FIFO_BUF_LEN];
	uint32_t * txie;
	struct stm32f_usart * uart;
};

static int uart_console_read(struct uart_console_dev * dev, char * buf, 
				 unsigned int len, unsigned int msec)
{

	char * cp = (char *)buf;
	int c;
	int n = 0;
	int th;

	DCC_LOG(LOG_TRACE, "read");
	th = thinkos_thread_self();

	__thinkos_critical_enter();
	while (uart_fifo_is_empty(&dev->rx_fifo)) {
		DCC_LOG(LOG_TRACE, "wait...");
		if ( th == 4) {
			DCC_LOG1(LOG_TRACE, "[%d] wait ...", th);
		}
		__thinkos_ev_wait(dev->rx_ev);
		if ( th == 4) {
			DCC_LOG1(LOG_TRACE, "[%d] wakeup.", th);
		}
		DCC_LOG(LOG_TRACE, "wakeup.");
	}
	__thinkos_critical_exit();

	do {
		if (n == len) {
			if (!uart_fifo_is_empty(&dev->rx_fifo)) { 
				__thinkos_ev_raise(dev->rx_ev);
			}
			break;
		}
		c = uart_fifo_get(&dev->rx_fifo);
		if (c == '\r') 
			c = '\n';
		cp[n++] = c;
	} while (!uart_fifo_is_empty(&dev->rx_fifo));


	DCC_LOG2(LOG_TRACE, "[%d] n=%d", thinkos_thread_self(), n);

	return n;
}

static void uart_putc(struct uart_console_dev * dev, int c)
{
#if ENABLE_UART_TX_BLOCK
	__thinkos_critical_enter();
	while (uart_fifo_is_full(&dev->tx_fifo)) {
		/* enable TX interrupt */
		DCC_LOG(LOG_TRACE, "wait...");
		__thinkos_ev_wait(dev->tx_ev);
		DCC_LOG(LOG_TRACE, "wakeup");
	}
	__thinkos_critical_exit();
#else
	if (uart_fifo_is_full(&dev->tx_fifo))
		return;
#endif

	uart_fifo_put(&dev->tx_fifo, c);
	*dev->txie = 1; 
}

static int uart_console_write(struct uart_console_dev * dev, const void * buf, 
					   unsigned int len)
{
	char * cp = (char *)buf;
	int c;
	int n;

	DCC_LOG1(LOG_INFO, "len=%d", len);

	for (n = 0; n < len; n++) {
		c = cp[n];
		if (c == '\n') {
			DCC_LOG(LOG_INFO, "CR");
			uart_putc(dev, '\r');
		}
		uart_putc(dev, c);
	}

	DCC_LOG1(LOG_INFO, "cnt=%d", n);

	return n;
}

static int uart_console_flush(struct uart_console_dev * ctrl)
{
	return 0;
}

const struct fileop uart_console_ops = {
	.write = (void *)uart_console_write,
	.read = (void *)uart_console_read,
	.flush = (void *)uart_console_flush,
	.close = (void *)NULL
};

struct uart_console_dev uart_console_dev;

const struct file uart_console_file = {
	.data = (void *)&uart_console_dev, 
	.op = &uart_console_ops
};

void uart_console_isr(struct stm32f_usart * us)
{
	struct uart_console_dev * dev = &uart_console_dev;
	uint32_t sr;
	int c;
	
	sr = us->sr;

	if (sr & USART_RXNE) {
		DCC_LOG(LOG_TRACE, "RXNE");
		c = us->dr;
		if (!uart_fifo_is_full(&dev->rx_fifo)) { 
			uart_fifo_put(&dev->rx_fifo, c);
		} else {
			DCC_LOG(LOG_WARNING, "RX fifo full!");
		}
		
		if (uart_fifo_is_half_full(&dev->rx_fifo)) { 
			__thinkos_ev_raise(dev->rx_ev);
		}
	}	

	if (sr & USART_IDLE) {
		DCC_LOG(LOG_TRACE, "IDLE");
		c = us->dr;
		(void)c;
		__thinkos_ev_raise(dev->rx_ev);
	}

	if (sr & USART_TXE) {
		DCC_LOG(LOG_MSG, "TXE");
		if (uart_fifo_is_empty(&dev->tx_fifo)) {
			/* disable TXE interrupts */
			*dev->txie = 0; 
#if ENABLE_UART_TX_BLOCK
			__thinkos_ev_raise(dev->tx_ev);
#endif
		} else {
			us->dr = uart_fifo_get(&dev->tx_fifo);
		}
	}
}

struct file * uart_console_open(struct stm32f_usart * us)
{
	struct uart_console_dev * dev = &uart_console_dev;

	DCC_LOG(LOG_TRACE, "...");
	dev->rx_ev = thinkos_ev_alloc(); 
	dev->tx_ev = thinkos_ev_alloc(); 
	uart_fifo_init(&dev->tx_fifo, UART_TX_FIFO_BUF_LEN);
	uart_fifo_init(&dev->rx_fifo, UART_RX_FIFO_BUF_LEN);

	dev->txie = CM3_BITBAND_DEV(&us->cr1, 7);
	dev->uart = us;

	/* enable RX interrupt */
	us->cr1 |= USART_RXNEIE | USART_IDLEIE;

	return (struct file *)&uart_console_file;
}


