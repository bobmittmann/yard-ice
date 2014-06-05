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
#include <stdbool.h>
#include <arch/cortex-m3.h>
#include <sys/serial.h>
#include <sys/delay.h>

#include <thinkos.h>
#define __THINKOS_IRQ__
#include <thinkos_irq.h>

#include <sys/dcclog.h>

#if (THINKOS_FLAG_MAX > 0) && (THINKOS_ENABLE_FLAG_ALLOC)

#ifndef ENABLE_ONLCR
#define ENABLE_ONLCR 0
#endif

#ifndef ENABLE_ICRNL
#define ENABLE_ICRNL 0
#endif

#ifndef ENABLE_INLCR
#define ENABLE_INLCR 0
#endif

#ifndef ENABLE_UART_TX_BLOCK
#define ENABLE_UART_TX_BLOCK 1
#endif

#ifndef ENABLE_UART_TX_MUTEX
#define ENABLE_UART_TX_MUTEX 1
#endif

#define UART_TX_FIFO_BUF_LEN 512
#define UART_RX_FIFO_BUF_LEN 8

#define UART STM32F_UART5
#define UART_TX STM32_GPIOC, 12
#define UART_RX STM32_GPIOD, 2
#define UART_ISR stm32f_uart5_isr
#define UART_IRQ_NUM STM32F_IRQ_UART5
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
	int ret;

	DCC_LOG(LOG_MSG, "read");

	__thinkos_flag_clr(dev->rx_flag);
	while (uart_fifo_is_empty(&dev->rx_fifo)) {
		DCC_LOG(LOG_MSG, "wait...");
		if ((ret = thinkos_flag_timedwait(dev->rx_flag, msec)) < 0) {
			DCC_LOG(LOG_MSG, "timeout.");
			return ret;
		}	
		__thinkos_flag_clr(dev->rx_flag);
		DCC_LOG(LOG_MSG, "wakeup.");
	}

	do {
		if (n == len) {
			break;
		}
		c = uart_fifo_get(&dev->rx_fifo);
#if ENABLE_ICRNL
		if (c == '\r') 
			c = '\n';
#endif
#if ENABLE_INLCR
		if (c == '\n') 
			c = '\r';
#endif
		cp[n++] = c;
	} while (!uart_fifo_is_empty(&dev->rx_fifo));


	DCC_LOG2(LOG_MSG, "[%d] n=%d", thinkos_thread_self(), n);

	return n;
}

static void uart_putc(struct uart_console_dev * dev, int c)
{
#if ENABLE_UART_TX_BLOCK
	__thinkos_flag_clr(dev->tx_flag);
	while (uart_fifo_is_full(&dev->tx_fifo)) {
		/* enable TX interrupt */
		DCC_LOG(LOG_MSG, "wait...");
		thinkos_flag_wait(dev->tx_flag);
		__thinkos_flag_clr(dev->tx_flag);
		DCC_LOG(LOG_MSG, "wakeup");
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

	DCC_LOG1(LOG_MSG, "len=%d", len);

#if ENABLE_UART_TX_MUTEX
	 thinkos_mutex_lock(dev->tx_mutex); 
#endif

	for (n = 0; n < len; n++) {
		c = cp[n];

#if ENABLE_ONLCR
		if (c == '\n') {
			DCC_LOG(LOG_MSG, "CR");
			uart_putc(dev, '\r');
		}
#endif
		uart_putc(dev, c);
	}

#if ENABLE_UART_TX_MUTEX
	thinkos_mutex_unlock(dev->tx_mutex); 
#endif

	DCC_LOG1(LOG_MSG, "cnt=%d", n);

	return n;
}

static int uart_console_flush(struct uart_console_dev * dev)
{
	return 0;
}

static int uart_console_close(struct uart_console_dev * dev)
{
	return 0;
}

struct uart_console_dev uart_console_dev;

void UART_ISR(void)
{
	struct uart_console_dev * dev = &uart_console_dev;
	struct stm32f_usart * uart = dev->uart;
	uint32_t sr;
	int c;
	
	sr = uart->sr & uart->cr1;

	if (sr & USART_RXNE) {
		DCC_LOG(LOG_MSG, "RXNE");
		c = uart->dr;
		if (!uart_fifo_is_full(&dev->rx_fifo)) { 
			uart_fifo_put(&dev->rx_fifo, c);
		} else {
			DCC_LOG(LOG_WARNING, "RX fifo full!");
		}
		
		if (uart_fifo_is_half_full(&dev->rx_fifo))
			__thinkos_flag_signal(dev->rx_flag);
	}	

	if (sr & USART_IDLE) {
		DCC_LOG(LOG_MSG, "IDLE");
		c = uart->dr;
		(void)c;
		__thinkos_flag_signal(dev->rx_flag);
	}

	if (sr & USART_TXE) {
		DCC_LOG(LOG_MSG, "TXE");
		if (uart_fifo_is_empty(&dev->tx_fifo)) {
			/* disable TXE interrupts */
			*dev->txie = 0; 
#if ENABLE_UART_TX_BLOCK
			__thinkos_flag_signal(dev->tx_flag);
#endif
		} else {
			uart->dr = uart_fifo_get(&dev->tx_fifo);
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
	struct stm32f_usart * uart = UART;
#if defined(STM32F1X)
	struct stm32f_afio * afio = STM32F_AFIO;
#endif

	stm32_gpio_clock_en(STM32_GPIOC);
	stm32_gpio_clock_en(STM32_GPIOD);

	/* USART1_TX */
	stm32_gpio_mode(UART_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);

	DCC_LOG1(LOG_TRACE, "UART=0x%08x", uart);

	dev->uart = uart;
#if defined(STM32F1X)
	/* USART1_RX */
	stm32_gpio_mode(UART_RX, INPUT, PULL_UP);
	/* Use alternate pins for USART1 */
	afio->mapr |= AFIO_USART1_REMAP;
#elif defined(STM32F4X)
	stm32_gpio_mode(UART_RX, ALT_FUNC, PULL_UP);
	stm32_gpio_af(UART_RX, GPIO_AF7);
	stm32_gpio_af(UART_TX, GPIO_AF7);
#elif defined(STM32F2X)
	stm32_gpio_mode(UART_RX, ALT_FUNC, PULL_UP);
	stm32_gpio_af(UART_RX, GPIO_AF8);
	stm32_gpio_af(UART_TX, GPIO_AF8);
#endif

	DCC_LOG(LOG_MSG, "...");
	dev->rx_flag = thinkos_flag_alloc(); 
#if ENABLE_UART_TX_BLOCK
	dev->tx_flag = thinkos_flag_alloc(); 
#endif
#if ENABLE_UART_TX_MUTEX
	dev->tx_mutex = thinkos_mutex_alloc(); 
#endif
	uart_fifo_init(&dev->tx_fifo, UART_TX_FIFO_BUF_LEN);
	uart_fifo_init(&dev->rx_fifo, UART_RX_FIFO_BUF_LEN);

	dev->txie = CM3_BITBAND_DEV(&uart->cr1, 7);
	dev->uart = uart;

	stm32f_usart_init(uart);
	stm32f_usart_baudrate_set(uart, 115200);
	stm32f_usart_mode_set(uart, SERIAL_8N1);
	stm32f_usart_enable(uart);

	/* enable RX interrupt */
	uart->cr1 |= USART_RXNEIE | USART_IDLEIE;

	cm3_irq_pri_set(UART_IRQ_NUM, UART_IRQ_PRIORITY);
	cm3_irq_enable(UART_IRQ_NUM);

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

