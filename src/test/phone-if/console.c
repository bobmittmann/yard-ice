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

#ifndef ENABLE_UART_TX_MUTEX
#define ENABLE_UART_TX_MUTEX 0
#endif

#define UART_TX_FIFO_BUF_LEN 512
#define UART_RX_FIFO_BUF_LEN 8

#define UART_IRQ_PRIORITY IRQ_PRIORITY_REGULAR

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
#if ENABLE_UART_TX_BLOCK
	int32_t tx_flag;
#endif
	int32_t rx_flag;
#if ENABLE_UART_TX_MUTEX
	int32_t tx_mutex;
#endif
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
	int n = 0;
	int c;

	DCC_LOG(LOG_INFO, "read");

	__thinkos_flag_clr(dev->rx_flag);
	while (uart_fifo_is_empty(&dev->rx_fifo)) {
		DCC_LOG(LOG_INFO, "wait...");
		thinkos_flag_wait(dev->rx_flag);
		__thinkos_flag_clr(dev->rx_flag);
		DCC_LOG(LOG_INFO, "wakeup.");
	}

	do {
		if (n == len) {
			break;
		}
		c = uart_fifo_get(&dev->rx_fifo);
		if (c == '\r') 
			c = '\n';
		cp[n++] = c;
	} while (!uart_fifo_is_empty(&dev->rx_fifo));


	DCC_LOG2(LOG_INFO, "[%d] n=%d", thinkos_thread_self(), n);

	return n;
}

static void uart_putc(struct uart_console_dev * dev, int c)
{
#if ENABLE_UART_TX_BLOCK
	__thinkos_flag_clr(dev->tx_flag);
	while (uart_fifo_is_full(&dev->tx_fifo)) {
		/* enable TX interrupt */
		DCC_LOG(LOG_INFO, "wait...");
		thinkos_flag_wait(dev->tx_flag);
		__thinkos_flag_clr(dev->tx_flag);
		DCC_LOG(LOG_INFO, "wakeup");
	}
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

#if ENABLE_UART_TX_MUTEX
	 thinkos_mutex_lock(dev->tx_mutex); 
#endif

	for (n = 0; n < len; n++) {
		c = cp[n];
		if (c == '\n') {
			DCC_LOG(LOG_INFO, "CR");
			uart_putc(dev, '\r');
		}
		uart_putc(dev, c);
	}

#if ENABLE_UART_TX_MUTEX
	thinkos_mutex_unlock(dev->tx_mutex); 
#endif

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

void stm32f_usart1_isr(void)
{
	struct uart_console_dev * dev = &uart_console_dev;
	struct stm32f_usart * us = dev->uart;
	uint32_t sr;
	int c;
	
	sr = us->sr & us->cr1;

	if (sr & USART_RXNE) {
		DCC_LOG(LOG_INFO, "RXNE");
		c = us->dr;
		if (!uart_fifo_is_full(&dev->rx_fifo)) { 
			uart_fifo_put(&dev->rx_fifo, c);
		} else {
			DCC_LOG(LOG_WARNING, "RX fifo full!");
		}
		
		if (uart_fifo_is_half_full(&dev->rx_fifo))
			__thinkos_flag_signal(dev->rx_flag);
	}	

	if (sr & USART_IDLE) {
		DCC_LOG(LOG_INFO, "IDLE");
		c = us->dr;
		(void)c;
		__thinkos_flag_signal(dev->rx_flag);
	}

	if (sr & USART_TXE) {
		DCC_LOG(LOG_INFO, "TXE");
		if (uart_fifo_is_empty(&dev->tx_fifo)) {
			/* disable TXE interrupts */
			*dev->txie = 0; 
#if ENABLE_UART_TX_BLOCK
			__thinkos_flag_signal(dev->tx_flag);
#endif
		} else {
			us->dr = uart_fifo_get(&dev->tx_fifo);
		}
	}
}

struct file * uart_console_open(struct stm32f_usart * us)
{
	struct uart_console_dev * dev = &uart_console_dev;

	DCC_LOG(LOG_INFO, "...");
	dev->rx_flag = thinkos_flag_alloc(); 
#if ENABLE_UART_TX_BLOCK
	dev->tx_flag = thinkos_flag_alloc(); 
#endif
#if ENABLE_UART_TX_MUTEX
	dev->tx_mutex = thinkos_mutex_alloc(); 
#endif
	uart_fifo_init(&dev->tx_fifo, UART_TX_FIFO_BUF_LEN);
	uart_fifo_init(&dev->rx_fifo, UART_RX_FIFO_BUF_LEN);

	dev->txie = CM3_BITBAND_DEV(&us->cr1, 7);
	dev->uart = us;

	cm3_irq_pri_set(STM32F_IRQ_USART1, UART_IRQ_PRIORITY);
	cm3_irq_enable(STM32F_IRQ_USART1);

	/* enable RX interrupt */
	us->cr1 |= USART_RXNEIE | USART_IDLEIE;

	return (struct file *)&uart_console_file;
}


/* ----------------------------------------------------------------------
 * Console 
 * ----------------------------------------------------------------------
 */

#define USART1_TX STM32F_GPIOB, 6
#define USART1_RX STM32F_GPIOB, 7

struct file stm32f_uart1_file = {
	.data = STM32F_USART1, 
	.op = &stm32f_usart_fops 
};

void stdio_init(void)
{
	struct stm32f_usart * uart = STM32F_USART1;
#if defined(STM32F1X)
	struct stm32f_afio * afio = STM32F_AFIO;
#endif

	DCC_LOG(LOG_TRACE, "...");

	/* USART1_TX */
	stm32f_gpio_mode(USART1_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);

#if defined(STM32F1X)
	/* USART1_RX */
	stm32f_gpio_mode(USART1_RX, INPUT, PULL_UP);
	/* Use alternate pins for USART1 */
	afio->mapr |= AFIO_USART1_REMAP;
#elif defined(STM32F4X)
	stm32f_gpio_mode(USART1_RX, ALT_FUNC, PULL_UP);
	stm32f_gpio_af(USART1_RX, GPIO_AF7);
	stm32f_gpio_af(USART1_TX, GPIO_AF7);
#endif

	stm32f_usart_init(uart);
	stm32f_usart_baudrate_set(uart, 115200);
	stm32f_usart_mode_set(uart, SERIAL_8N1);
	stm32f_usart_enable(uart);

	stderr = &stm32f_uart1_file;
	stdin = uart_console_open(uart);
	stdout = stdin;
}

