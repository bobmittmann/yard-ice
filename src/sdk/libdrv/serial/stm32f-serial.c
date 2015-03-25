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
 * @file stm32f-serial.c
 * @brief STM32F serial driver
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __STM32_SERIAL_I__
#include "stm32-serial-i.h"
#include <sys/param.h>

#if !SERIAL_ENABLE_DMA

void stm32f_serial_isr(struct stm32f_serial_drv * drv)
{
	struct stm32_usart * us;
	uint32_t sr;
	int c;
	
	us = drv->uart;
	sr = us->sr;

#if SERIAL_ENABLE_DMA
	if (sr & USART_RXNE) {
		DCC_LOG(LOG_TRACE, "RXNE");
		c = us->dr;
		(void)c;
	}	

	if (sr & USART_IDLE) {
		DCC_LOG(LOG_TRACE, "IDLE");
		/* Stop DMA transfer */
		drv->rx.s->cr &= ~DMA_EN;
		c = us->dr;
		(void)c;
	}

	if (sr & USART_TXE) {
		DCC_LOG(LOG_MSG, "TXE");
	}
#else

	if (sr & USART_ORE) {
		DCC_LOG(LOG_WARNING, "OVR!");
	}

	sr &= us->cr1;

	if (sr & USART_RXNE) {
		unsigned int head;
		int free;

		c = us->dr;

		head = drv->rx_fifo.head;
		free = SERIAL_RX_FIFO_LEN - (uint8_t)(head - drv->rx_fifo.tail);
		if (free > 0) { 
			drv->rx_fifo.buf[head & (SERIAL_RX_FIFO_LEN - 1)] = c;
			drv->rx_fifo.head = head + 1;
		} else {
			DCC_LOG(LOG_WARNING, "RX fifo full!");
		}
		if (free < (SERIAL_RX_FIFO_LEN / 2)) /* fifo is more than half full */
			thinkos_flag_give_i(drv->rx_flag);
	}	

	if (sr & USART_IDLE) {
		DCC_LOG(LOG_INFO, "IDLE!");
		c = us->dr;
		(void)c;
		thinkos_flag_give_i(drv->rx_flag);
	}

	if (sr & USART_TXE) {
		unsigned int tail = drv->tx_fifo.tail;
		if (tail == drv->tx_fifo.head) {
			/* FIFO empty, disable TXE interrupts */
			*drv->txie = 0; 
			thinkos_flag_set_i(drv->tx_flag);
		} else {
			us->dr = drv->tx_fifo.buf[tail & (SERIAL_TX_FIFO_LEN - 1)];
			drv->tx_fifo.tail = tail + 1;
		}
	}
#endif
}

int stm32f_serial_init(struct stm32f_serial_drv * drv, 
					   unsigned int baudrate, unsigned int flags)
{
	struct stm32_usart * uart = drv->uart;

	DCC_LOG1(LOG_TRACE, "UART=0x%08x", uart);
	DCC_LOG1(LOG_TRACE, "SERIAL_RX_FIFO_LEN=%d", SERIAL_RX_FIFO_LEN);
	DCC_LOG1(LOG_TRACE, "SERIAL_TX_FIFO_LEN=%d", SERIAL_TX_FIFO_LEN);
	DCC_LOG1(LOG_TRACE, "SERIAL_ENABLE_TX_MUTEX=%d", SERIAL_ENABLE_TX_MUTEX);

	drv->rx_flag = thinkos_flag_alloc(); 
	drv->tx_flag = thinkos_flag_alloc(); 
#if SERIAL_ENABLE_TX_MUTEX
	drv->tx_mutex = thinkos_mutex_alloc(); 
	DCC_LOG1(LOG_TRACE, "tx_mutex=%d", drv->tx_mutex);
#endif

	drv->tx_fifo.head = drv->tx_fifo.tail = 0;
	drv->rx_fifo.head = drv->rx_fifo.tail = 0;
	drv->txie = CM3_BITBAND_DEV(&uart->cr1, 7);
	thinkos_flag_give(drv->tx_flag);

	stm32_usart_init(uart);
	stm32_usart_baudrate_set(uart, baudrate);
	stm32_usart_mode_set(uart, SERIAL_8N1);

	/* enable RX interrupt */
	uart->cr1 |= USART_RXNEIE | USART_IDLEIE;

	/* enable UART */
	stm32_usart_enable(uart);

	return 0;
}

int stm32f_serial_read(struct stm32f_serial_drv * drv, void * buf, 
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
	if ((ret = thinkos_flag_timedtake(drv->rx_flag, tmo)) < 0) {
		DCC_LOG1(LOG_INFO, "cnt=%d, timeout!", 
				 (int32_t)(drv->rx_fifo.head - drv->rx_fifo.tail));
		return ret;
	}

	tail = drv->rx_fifo.tail;
	cnt = (int32_t)(drv->rx_fifo.head - tail);
	if (cnt == 0) {
		DCC_LOG(LOG_WARNING, "RX FIFO empty!");
		goto again;
	}
	n = MIN(len, cnt);

	for (i = 0; i < n; ++i)
		cp[i] = drv->rx_fifo.buf[tail++ & (SERIAL_RX_FIFO_LEN - 1)];

	drv->rx_fifo.tail = tail;

	if (cnt > n) {
		DCC_LOG3(LOG_INFO, "len=%d cnt=%d n=%d", len, cnt, n);
		thinkos_flag_give(drv->rx_flag);
	} else {
		DCC_LOG1(LOG_INFO, "2. n=%d", n);
	}

//	DCC_LOG1(LOG_TRACE, "len=%d", n);
	DCC_LOG2(LOG_INFO, "len=%d '%c'...", n, cp[0]);

	return n;
}

int stm32f_serial_write(struct stm32f_serial_drv * drv, const void * buf,
						unsigned int len)
{
	uint8_t * cp = (uint8_t *)buf;
	int rem = len;

	DCC_LOG1(LOG_INFO, "len=%d", len);

#if SERIAL_ENABLE_TX_MUTEX
	 thinkos_mutex_lock(drv->tx_mutex); 
#endif

	while (rem) {
		unsigned int head;
		int free;
		int n;
		int i;

		thinkos_flag_take(drv->tx_flag);

		head = drv->tx_fifo.head;
		free = SERIAL_TX_FIFO_LEN - (int32_t)(head - drv->tx_fifo.tail);
		DCC_LOG3(LOG_MSG, "head=%d tail=%d n=%d", head, drv->tx_fifo.tail, n);
		n = MIN(rem, free);
		for (i = 0; i < n; ++i) 
			drv->tx_fifo.buf[head++ & (SERIAL_TX_FIFO_LEN - 1)] = *cp++;
		drv->tx_fifo.head = head;
		*drv->txie = 1; 

		if (free > n)
			thinkos_flag_give(drv->tx_flag);

		rem -= n;
		DCC_LOG1(LOG_INFO, "rem=%d", rem);
	}

#if SERIAL_ENABLE_TX_MUTEX
	thinkos_mutex_unlock(drv->tx_mutex); 
#endif

	return len;
}

int stm32f_serial_flush(struct stm32f_serial_drv * drv)
{
	do {
		thinkos_flag_take(drv->tx_flag);
	} while (((int32_t)drv->tx_fifo.head - drv->tx_fifo.tail) > 0);

	stm32_usart_flush(drv->uart);

	thinkos_flag_give(drv->tx_flag);

	return 0;
}

int stm32f_serial_close(struct stm32f_serial_drv * drv)
{
	return 0;
}

int stm32f_serial_conf_get(struct stm32f_serial_drv * drv, 
							struct serial_config * cfg)
{
//	struct stm32f_usart * uart = dev->uart;

	return 0;
}

int stm32f_serial_conf_set(struct stm32f_serial_drv * drv, 
						   const struct serial_config * cfg)
{
	struct stm32_usart * uart = drv->uart;
	uint32_t flags;

	DCC_LOG(LOG_TRACE, "...");

	stm32_usart_baudrate_set(uart, cfg->baudrate);

	flags = CFG_TO_FLAGS(cfg);

	stm32_usart_mode_set(uart, flags);

	return 0;
}


const struct serial_op stm32f_uart_serial_op = {
	.send = (void *)stm32f_serial_write,
	.recv = (void *)stm32f_serial_read,
	.drain = (void *)stm32f_serial_flush,
	.close = (void *)stm32f_serial_close,
	.conf_set = (void *)stm32f_serial_conf_set,
	.conf_get = (void *)stm32f_serial_conf_get
};

#endif /* !SERIAL_ENABLE_DMA */

