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
#include <sys/param.h>
#include <sys/delay.h>

#include "board.h"

#include <thinkos.h>
#define __THINKOS_IRQ__
#include <thinkos_irq.h>

#include <sys/dcclog.h>

#define UART_TX_FIFO_BUF_LEN 128
#define UART_RX_FIFO_BUF_LEN 64
#define UART_RX_FIFO_WATER_MARK 32

struct serial_dev {
	int32_t tx_flag;
	int32_t rx_flag;
	struct {
		volatile uint32_t head;
		volatile uint32_t tail;
		uint8_t buf[UART_TX_FIFO_BUF_LEN];
	} tx_fifo;	
	struct {
		volatile uint32_t head;
		volatile uint32_t tail;
		uint8_t buf[UART_RX_FIFO_BUF_LEN];
	} rx_fifo;	
	uint32_t * txie;
	uint32_t * tcie;
	struct stm32_usart * uart;
	int rx_full;
};

int serial_read(struct serial_dev * dev, void * buf, 
				unsigned int len, unsigned int tmo)
{
	uint8_t * cp = (uint8_t *)buf;
	unsigned int tail;
	int ret;
	int cnt;
	int n;
	int i;

//	DCC_LOG1(LOG_TRACE, "cnt=%d", (int32_t)(dev->rx_fifo.head - dev->rx_fifo.tail));
again:
//	DCC_LOG(LOG_TRACE, "1.");
	if ((ret = thinkos_flag_timedtake(dev->rx_flag, tmo)) < 0) {
		DCC_LOG2(LOG_INFO, "cnt=%d, timeout (%d ms)!", 
				 (int32_t)(dev->rx_fifo.head - dev->rx_fifo.tail), tmo);
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
		cp[i] = dev->rx_fifo.buf[tail++ & (UART_RX_FIFO_BUF_LEN - 1)];

	dev->rx_fifo.tail = tail;

	if (cnt > n) {
		DCC_LOG3(LOG_INFO, "len=%d cnt=%d n=%d", len, cnt, n);
		thinkos_flag_give(dev->rx_flag);
	} else {
		DCC_LOG1(LOG_INFO, "2. n=%d", n);
	}

	return n;
}

int serial_write(struct serial_dev * dev, const void * buf, 
				 unsigned int len)
{
	uint8_t * cp = (uint8_t *)buf;
	int rem = len;

	DCC_LOG1(LOG_TRACE, "len=%d", len);

	while (rem) {
		unsigned int head;
		int free;
		int n;
		int i;

		thinkos_flag_take(dev->tx_flag);

		head = dev->tx_fifo.head;
		free = UART_TX_FIFO_BUF_LEN - (int32_t)(head - dev->tx_fifo.tail);
		DCC_LOG3(LOG_MSG, "head=%d tail=%d n=%d", head, dev->tx_fifo.tail, n);
		n = MIN(rem, free);
		for (i = 0; i < n; ++i) 
			dev->tx_fifo.buf[head++ & (UART_TX_FIFO_BUF_LEN - 1)] = *cp++;
		dev->tx_fifo.head = head;

		if (!rs485_is_txen()) {
//			rs485_rxdis();
//			rs485_txen();
//			thinkos_sleep(12);
		}
		*dev->txie = 1; 

		if (free > n)
			thinkos_flag_give(dev->tx_flag);

		rem -= n;
		DCC_LOG1(LOG_INFO, "rem=%d", rem);
	}

	return len;
}

int serial_drain(struct serial_dev * dev)
{
	do {
		thinkos_flag_take(dev->tx_flag);
	} while (((int32_t)dev->tx_fifo.head - dev->tx_fifo.tail) > 0);

	stm32_usart_flush(dev->uart);

	thinkos_flag_give(dev->tx_flag);

	return 0;
}


int serial_config_get(struct serial_dev * dev, struct serial_config * cfg)
{
//	struct stm32_usart * uart = dev->uart;

	return 0;
}

int serial_enable(struct serial_dev * dev)
{
	struct stm32_usart * uart = dev->uart;

	DCC_LOG(LOG_TRACE, ".");

	stm32_usart_enable(uart);

	return 0;
}

void serial_rx_enable(struct serial_dev * dev)
{
	struct stm32_usart * us = dev->uart;
	/* enable RX */
	us->cr1 |= USART_RE;
}

void serial_rx_disable(struct serial_dev * dev)
{
	struct stm32_usart * us = dev->uart;
	/* enable RX */
	us->cr1 &= ~USART_RE;
}

int serial_config_set(struct serial_dev * dev, 
					  const struct serial_config * cfg)
{
	struct stm32_usart * uart = dev->uart;
	uint32_t flags;

	DCC_LOG(LOG_INFO, "...");

	stm32_usart_baudrate_set(uart, cfg->baudrate);

	flags = CFG_TO_FLAGS(cfg);

	stm32_usart_mode_set(uart, flags);

	return 0;
}

struct serial_dev serial2_dev;

void stm32f_usart2_isr(void)
{
	struct serial_dev * dev = &serial2_dev;
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
		free = UART_RX_FIFO_BUF_LEN - (uint8_t)(head - dev->rx_fifo.tail);
		if (free > 0) { 
			dev->rx_fifo.buf[head & (UART_RX_FIFO_BUF_LEN - 1)] = c;
			dev->rx_fifo.head = head + 1;
			dev->rx_full = 0;
		} else {
			if ((dev->rx_full & 0xff) == 0)
				DCC_LOG(LOG_TRACE, "RX fifo full!");
			dev->rx_full++;
		}
		if (free < (UART_RX_FIFO_BUF_LEN  - UART_RX_FIFO_WATER_MARK)) 
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
			/* enable TC interrupts */
			*dev->tcie = 1;
			__thinkos_flag_set(dev->tx_flag);
		} else {
			/* RS485 enable transmitter */ 
//			if (!rs485_is_txen()) {
//				rs485_rxdis();
				rs485_txen();
//				udelay(100);
//			}
			us->dr = dev->tx_fifo.buf[tail & (UART_TX_FIFO_BUF_LEN - 1)];
			dev->tx_fifo.tail = tail + 1;
		}
	}

	if (sr & USART_TC) {
		DCC_LOG(LOG_INFO, "TC");
//		udelay(53);
		/* RS485 disable ransmitter */ 
		rs485_txdis();
		/* RS485 enable receiver */ 
		rs485_rxen();
		/* disable TC interrupts */
		*dev->tcie = 0;
	}
}

struct serial_dev * serial2_open(void)
{
	struct serial_dev * dev = &serial2_dev;
	struct stm32_usart * uart = STM32_USART2;

	DCC_LOG2(LOG_TRACE, "dev=%p uart=%p...", dev, uart);
	dev->rx_flag = thinkos_flag_alloc(); 
	dev->tx_flag = thinkos_flag_alloc(); 
	dev->tx_fifo.head = dev->tx_fifo.tail = 0;
	dev->rx_fifo.head = dev->rx_fifo.tail = 0;

	dev->txie = CM3_BITBAND_DEV(&uart->cr1, 7);
	dev->tcie = CM3_BITBAND_DEV(&uart->cr1, 6);
	dev->uart = uart;

	thinkos_flag_give(dev->tx_flag);

	/* clock enable */
	stm32_clk_enable(STM32_RCC, STM32_CLK_USART2);

	stm32_usart_init(uart);
	stm32_usart_baudrate_set(uart, 19200);
	stm32_usart_mode_set(uart, SERIAL_8N1);

	/* enable RX and IDLE interrupts */
	uart->cr1 |= USART_RXNEIE | USART_IDLEIE;
	/* Errors interrupt */
	uart->cr3 |= USART_EIE;

	/* configure interrupts */
	cm3_irq_pri_set(STM32_IRQ_USART2, IRQ_PRIORITY_REGULAR);
	/* enable interrupts */
	cm3_irq_enable(STM32_IRQ_USART2);

	return dev;
}


