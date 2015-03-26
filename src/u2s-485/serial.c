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

#include "board.h"

#include <stdio.h>
#include <string.h>
#include <sys/serial.h>
#include <sys/param.h>
#include <sys/delay.h>

#define __THINKOS_IRQ__
#include <thinkos_irq.h>

#include <sys/dcclog.h>

#define UART_TX_FIFO_BUF_LEN 64
#define UART_RX_FIFO_BUF_LEN 64
#define UART_RX_FIFO_WATER_MARK 32

struct stm32_serial_drv {
#ifndef SERDRV_TX_FLAG_NO
	int32_t tx_flag;
#endif
#ifndef SERDRV_RX_FLAG_NO
	int32_t rx_flag;
#endif	
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

#ifdef SERDRV_RX_FLAG_NO
#define RX_FLAG (THINKOS_FLAG_BASE + SERDRV_RX_FLAG_NO)
#else
#define RX_FLAG drv->rx_flag
#endif

#ifdef SERDRV_TX_FLAG_NO
#define TX_FLAG (THINKOS_FLAG_BASE + SERDRV_TX_FLAG_NO)
#else
#define TX_FLAG drv->tx_flag
#endif

int __serial_read(struct stm32_serial_drv * drv, void * buf, 
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
//	DCC_LOG(LOG_TRACE, "1.");
	if ((ret = thinkos_flag_timedtake(RX_FLAG, tmo)) < 0) {
		DCC_LOG2(LOG_INFO, "cnt=%d, timeout (%d ms)!", 
				 (int32_t)(drv->rx_fifo.head - drv->rx_fifo.tail), tmo);
		return ret;
	}

	tail = drv->rx_fifo.tail;
	cnt = (int32_t)(drv->rx_fifo.head - tail);
	if (cnt == 0) {
		DCC_LOG(LOG_INFO, "RX FIFO empty!");
		goto again;
	}
	n = MIN(len, cnt);

	for (i = 0; i < n; ++i)
		cp[i] = drv->rx_fifo.buf[tail++ & (UART_RX_FIFO_BUF_LEN - 1)];

	drv->rx_fifo.tail = tail;

	if (cnt > n) {
		DCC_LOG3(LOG_INFO, "len=%d cnt=%d n=%d", len, cnt, n);
		thinkos_flag_give(RX_FLAG);
	} else {
		DCC_LOG1(LOG_INFO, "2. n=%d", n);
	}

	return n;
}

int __serial_write(struct stm32_serial_drv * drv, const void * buf, 
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

		thinkos_flag_take(TX_FLAG);

		head = drv->tx_fifo.head;
		free = UART_TX_FIFO_BUF_LEN - (int32_t)(head - drv->tx_fifo.tail);
		DCC_LOG3(LOG_MSG, "head=%d tail=%d n=%d", head, drv->tx_fifo.tail, n);
		n = MIN(rem, free);
		for (i = 0; i < n; ++i) 
			drv->tx_fifo.buf[head++ & (UART_TX_FIFO_BUF_LEN - 1)] = *cp++;
		drv->tx_fifo.head = head;

		*drv->txie = 1; 

		if (free > n)
			thinkos_flag_give(TX_FLAG);

		rem -= n;
		DCC_LOG1(LOG_INFO, "rem=%d", rem);
	}

	return len;
}

int __serial_drain(struct stm32_serial_drv * drv)
{
	do {
		thinkos_flag_take(TX_FLAG);
	} while (((int32_t)drv->tx_fifo.head - drv->tx_fifo.tail) > 0);

	stm32_usart_flush(drv->uart);

	thinkos_flag_give(TX_FLAG);

	return 0;
}


int __serial_conf_get(struct stm32_serial_drv * drv, struct serial_config * cfg)
{
//	struct stm32_usart * uart = drv->uart;

	return 0;
}

int __serial_conf_set(struct stm32_serial_drv * drv, 
					  const struct serial_config * cfg)
{
	struct stm32_usart * uart = drv->uart;
	uint32_t flags;

	DCC_LOG(LOG_INFO, "...");

	stm32_usart_baudrate_set(uart, cfg->baudrate);

	flags = CFG_TO_FLAGS(cfg);

	stm32_usart_mode_set(uart, flags);

	return 0;
}

int __serial_ioctl(struct stm32_serial_drv * drv, int opt, unsigned int arg)
{
	struct stm32_usart * us = drv->uart;
	unsigned int msk = 0;

	DCC_LOG(LOG_TRACE, ".");

	switch (opt) {
	case SERIAL_IOCTL_ENABLE:
		msk |= (arg & SERIAL_RX_EN) ? USART_RE : 0;
		msk |= (arg & SERIAL_TX_EN) ? USART_TE : 0;
		us->cr1 |= msk;
		break;
	case SERIAL_IOCTL_DISABLE:
		msk |= (arg & SERIAL_RX_EN) ? USART_RE : 0;
		msk |= (arg & SERIAL_TX_EN) ? USART_TE : 0;
		us->cr1 &= ~msk;
		break;
	case SERIAL_IOCTL_DRAIN:
		break;
	}
	

	return 0;
}

int __serial_close(struct stm32_serial_drv * drv)
{
	struct stm32_usart * us = drv->uart;
	us->cr1 = 0;

	return 0;
}

struct stm32_serial_drv uart2_serial_drv;

void stm32f_usart2_isr(void)
{
	struct stm32_serial_drv * drv = &uart2_serial_drv;
	struct stm32_usart * us = drv->uart;
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
		head = drv->rx_fifo.head;
		free = UART_RX_FIFO_BUF_LEN - (uint8_t)(head - drv->rx_fifo.tail);
		if (free > 0) { 
			drv->rx_fifo.buf[head & (UART_RX_FIFO_BUF_LEN - 1)] = c;
			drv->rx_fifo.head = head + 1;
			drv->rx_full = 0;
		} else {
			if ((drv->rx_full & 0x3f) == 0)
				DCC_LOG(LOG_WARNING, "RX fifo full!");
			drv->rx_full++;
		}
		if (free < (UART_RX_FIFO_BUF_LEN  - UART_RX_FIFO_WATER_MARK)) 
			thinkos_flag_give_i(RX_FLAG);
	}	

	if (sr & USART_IDLE) {
		DCC_LOG(LOG_INFO, "IDLE!");
		c = us->dr;
		(void)c;
		thinkos_flag_give_i(RX_FLAG);
	}

	if (sr & USART_TXE) {
		unsigned int tail = drv->tx_fifo.tail;
		if (tail == drv->tx_fifo.head) {
			/* FIFO empty, disable TXE interrupts */
			*drv->txie = 0; 
			/* enable TC interrupts */
			*drv->tcie = 1;
			thinkos_flag_set_i(TX_FLAG);
		} else {
			/* disable UART RX */
			us->cr1 &= ~USART_RE;
			/* RS485 disable receiver */ 
//			rs485_rxdis();
			/* RS485 enable transmitter */ 
			rs485_txen();
			us->dr = drv->tx_fifo.buf[tail & (UART_TX_FIFO_BUF_LEN - 1)];
			drv->tx_fifo.tail = tail + 1;
		}
	}

	if (sr & USART_TC) {
		DCC_LOG(LOG_INFO, "TC");
		/* RS485 disable ransmitter */ 
		rs485_txdis();
		/* RS485 enable receiver */ 
//		rs485_rxen();
		/* enable UART RX */
		us->cr1 |= USART_RE;
		/* disable TC interrupts */
		*drv->tcie = 0;
	}
}

const struct serial_op stm32f_uart_serial_op = {
	.send = (void *)__serial_write,
	.recv = (void *)__serial_read,
	.drain = (void *)__serial_drain,
	.close = (void *)__serial_close,
	.conf_set = (void *)__serial_conf_set,
	.conf_get = (void *)__serial_conf_get,
	.ioctl = (void *)__serial_ioctl
};

const struct serial_dev uart2_serial_dev = {
	.drv = &uart2_serial_drv,
	.op = &stm32f_uart_serial_op
};

struct serial_dev * serial2_open(void)
{
	struct stm32_serial_drv * drv = &uart2_serial_drv;
	struct stm32_usart * uart = STM32_USART2;

	DCC_LOG2(LOG_TRACE, "drv=%p uart=%p...", drv, uart);
#ifndef SERDRV_RX_FLAG_NO
	drv->rx_flag = thinkos_flag_alloc(); 
#endif
#ifndef SERDRV_TX_FLAG_NO
	drv->tx_flag = thinkos_flag_alloc(); 
#endif
	drv->tx_fifo.head = drv->tx_fifo.tail = 0;
	drv->rx_fifo.head = drv->rx_fifo.tail = 0;

	drv->txie = CM3_BITBAND_DEV(&uart->cr1, 7);
	drv->tcie = CM3_BITBAND_DEV(&uart->cr1, 6);
	drv->uart = uart;

	thinkos_flag_give(TX_FLAG);

	/* clock enable */
	stm32_clk_enable(STM32_RCC, STM32_CLK_USART2);

	stm32_usart_init(uart);
	stm32_usart_baudrate_set(uart, 19200);
	stm32_usart_mode_set(uart, SERIAL_8N1);

	/* enable UART, RX and IDLE interrupts */
	uart->cr1 |= USART_UE | USART_RXNEIE | USART_IDLEIE;
	/* Errors interrupt */
	uart->cr3 |= USART_EIE;

	/* configure interrupts */
	cm3_irq_pri_set(STM32_IRQ_USART2, IRQ_PRIORITY_REGULAR);
	/* enable interrupts */
	cm3_irq_enable(STM32_IRQ_USART2);

	return (struct serial_dev *)&uart2_serial_dev;
}

