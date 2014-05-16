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

#define UART_TX_FIFO_BUF_LEN 64
#define UART_RX_FIFO_BUF_LEN 64

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

struct serial_dev {
	int32_t tx_flag;
	int32_t rx_flag;
	struct uart_fifo tx_fifo;
	uint8_t tx_buf[UART_TX_FIFO_BUF_LEN];
	struct uart_fifo rx_fifo;
	uint8_t rx_buf[UART_RX_FIFO_BUF_LEN];
	uint32_t * txie;
	struct stm32f_usart * uart;
};

int serial_read(struct serial_dev * dev, char * buf, 
				unsigned int len, unsigned int msec)
{
	char * cp = (char *)buf;
	int n = 0;
	int c;
	int ret;

	DCC_LOG(LOG_MSG, "read");

	__thinkos_flag_clr(dev->rx_flag);
	while (uart_fifo_is_empty(&dev->rx_fifo)) {
		DCC_LOG(LOG_INFO, "wait...");
		ret = thinkos_flag_timedwait(dev->rx_flag, msec);
		if (ret < 0)
			return ret;
		__thinkos_flag_clr(dev->rx_flag);
		DCC_LOG(LOG_INFO, "wakeup.");
	}

	do {
		if (n == len) {
			break;
		}
		c = uart_fifo_get(&dev->rx_fifo);
		cp[n++] = c;
	} while (!uart_fifo_is_empty(&dev->rx_fifo));

	DCC_LOG2(LOG_INFO, "[%d] n=%d", thinkos_thread_self(), n);

	return n;
}

static void uart_putc(struct serial_dev * dev, int c)
{
	DCC_LOG3(LOG_MSG, "fifo: len=%d head=%d tail=%d", 
			 dev->tx_fifo.len, dev->tx_fifo.head, dev->tx_fifo.tail);

	while (uart_fifo_is_full(&dev->tx_fifo)) {
		/* enable TX interrupt */
		DCC_LOG(LOG_INFO, "wait...");
		thinkos_flag_wait(dev->tx_flag);
		__thinkos_flag_clr(dev->tx_flag);
		DCC_LOG(LOG_INFO, "wakeup");
	}

	DCC_LOG1(LOG_INFO, "%02x", c);

	uart_fifo_put(&dev->tx_fifo, c);
	*dev->txie = 1; 
}

int serial_write(struct serial_dev * dev, const void * buf, 
				 unsigned int len)
{
	char * cp = (char *)buf;
	int c;
	int n;

	DCC_LOG1(LOG_INFO, "len=%d", len);

	for (n = 0; n < len; ++n) {
		c = cp[n];
		uart_putc(dev, c);
	}

	DCC_LOG1(LOG_INFO, "cnt=%d", n);

	return n;
}


int serial_config_get(struct serial_dev * dev, struct serial_config * cfg)
{
//	struct stm32f_usart * uart = dev->uart;

	return 0;
}

int serial_config_set(struct serial_dev * dev, 
					  const struct serial_config * cfg)
{
	struct stm32f_usart * uart = dev->uart;
	uint32_t flags;

	DCC_LOG(LOG_INFO, "...");

	stm32f_usart_baudrate_set(uart, cfg->baudrate);

	flags = CFG_TO_FLAGS(cfg);

	stm32f_usart_mode_set(uart, flags);

	return 0;
}

void serial_isr(struct serial_dev * dev)
{
	struct stm32f_usart * uart = dev->uart;
	uint32_t sr;
	int c;
	
	DCC_LOG2(LOG_INFO, "dev=%p uart=%p...", dev, uart);

	sr = uart->sr;

#if 1	
	if (sr & USART_LBD) {
		/* Line break detection */
		DCC_LOG(LOG_TRACE, "LBD");
	}

	if (sr & USART_ORE) {
		/* Overrun error */
		DCC_LOG(LOG_TRACE, "ORE");
	}

	if (sr & USART_NE) {
		/* Noise error */
		DCC_LOG(LOG_TRACE, "NE");
	}

	if (sr & USART_FE) {
		/* Framming error */
		DCC_LOG(LOG_TRACE, "FE");
	}

	if (sr & USART_PE) {
		/* Parity error */
		DCC_LOG(LOG_TRACE, "PE");
	}
#endif

	sr &= uart->cr1;

	if (sr & USART_RXNE) {
		DCC_LOG(LOG_INFO, "RXNE");
		c = uart->dr;
		if (!uart_fifo_is_full(&dev->rx_fifo)) { 
			uart_fifo_put(&dev->rx_fifo, c);
		} else {
			DCC_LOG(LOG_INFO, "RX fifo full!");
		}
		
		if (uart_fifo_is_half_full(&dev->rx_fifo)) 
			__thinkos_flag_signal(dev->rx_flag);
	}	

	if (sr & USART_IDLE) {
		DCC_LOG(LOG_INFO, "IDLE");
		c = uart->dr;
		(void)c;
		__thinkos_flag_signal(dev->rx_flag);
	}

	if (sr & USART_TXE) {
		DCC_LOG(LOG_INFO, "TXE");
		if (uart_fifo_is_empty(&dev->tx_fifo)) {
			/* disable TXE interrupts */
			*dev->txie = 0; 
			__thinkos_flag_signal(dev->tx_flag);
		} else {
			uart->dr = uart_fifo_get(&dev->tx_fifo);
		}
	}
}

struct serial_dev serial2_dev;

void stm32f_usart2_isr(void)
{
	serial_isr(&serial2_dev);
}

struct serial_dev * serial2_open(void)
{
	struct serial_dev * dev = &serial2_dev;
	struct stm32f_usart * uart = STM32F_USART2;

	DCC_LOG2(LOG_INFO, "dev=%p uart=%p...", dev, uart);
	dev->rx_flag = thinkos_flag_alloc(); 
	dev->tx_flag = thinkos_flag_alloc(); 
	uart_fifo_init(&dev->tx_fifo, UART_TX_FIFO_BUF_LEN);
	uart_fifo_init(&dev->rx_fifo, UART_RX_FIFO_BUF_LEN);

	dev->txie = CM3_BITBAND_DEV(&uart->cr1, 7);
	dev->uart = uart;

	cm3_irq_pri_set(STM32F_IRQ_USART2, UART_IRQ_PRIORITY);
	cm3_irq_enable(STM32F_IRQ_USART2);

	stm32f_usart_init(uart);
	stm32f_usart_baudrate_set(uart, 9600);
	stm32f_usart_mode_set(uart, SERIAL_8N1);
	stm32f_usart_enable(uart);

	__thinkos_flag_clr(dev->tx_flag);

	/* enable RX and IDLE interrupts */
	uart->cr1 |= USART_RXNEIE | USART_IDLEIE;
	/* Errors interrupt */
	uart->cr3 |= USART_EIE;

	return dev;
}

struct serial_dev serial3_dev;

void stm32f_usart3_isr(void)
{
	serial_isr(&serial3_dev);
}

struct serial_dev * serial3_open(void)
{
	struct serial_dev * dev = &serial3_dev;
	struct stm32f_usart * uart = STM32F_USART3;

	DCC_LOG2(LOG_INFO, "dev=%p uart=%p...", dev, uart);
	dev->rx_flag = thinkos_flag_alloc(); 
	dev->tx_flag = thinkos_flag_alloc(); 
	uart_fifo_init(&dev->tx_fifo, UART_TX_FIFO_BUF_LEN);
	uart_fifo_init(&dev->rx_fifo, UART_RX_FIFO_BUF_LEN);

	dev->txie = CM3_BITBAND_DEV(&uart->cr1, 7);
	dev->uart = uart;

	cm3_irq_pri_set(STM32F_IRQ_USART3, UART_IRQ_PRIORITY);
	cm3_irq_enable(STM32F_IRQ_USART3);

	stm32f_usart_init(uart);
	stm32f_usart_baudrate_set(uart, 9600);
	stm32f_usart_mode_set(uart, SERIAL_8N1);
	stm32f_usart_enable(uart);

	__thinkos_flag_clr(dev->tx_flag);

	/* enable RX and IDLE interrupts */
	uart->cr1 |= USART_RXNEIE | USART_IDLEIE;
	/* Errors interrupt */
	uart->cr3 |= USART_EIE;

	return dev;
}

