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

int stm32f_serial_init(struct stm32f_serial_dev * dev, 
					   unsigned int baudrate, unsigned int flags)
{
	struct stm32_usart * uart = dev->uart;

	DCC_LOG1(LOG_TRACE, "UART=0x%08x", uart);
	DCC_LOG1(LOG_TRACE, "SERIAL_RX_FIFO_LEN=%d", SERIAL_RX_FIFO_LEN);
	DCC_LOG1(LOG_TRACE, "SERIAL_TX_FIFO_LEN=%d", SERIAL_TX_FIFO_LEN);
	DCC_LOG1(LOG_TRACE, "SERIAL_ENABLE_TX_MUTEX=%d", SERIAL_ENABLE_TX_MUTEX);

	dev->rx_flag = thinkos_flag_alloc(); 
	dev->tx_flag = thinkos_flag_alloc(); 
#if SERIAL_ENABLE_TX_MUTEX
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

	return 0;
}

int stm32f_serial_read(struct stm32f_serial_dev * dev, void * buf, 
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
		DCC_LOG1(LOG_INFO, "cnt=%d, timeout!", 
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
		cp[i] = dev->rx_fifo.buf[tail++ & (SERIAL_RX_FIFO_LEN - 1)];

	dev->rx_fifo.tail = tail;

	if (cnt > n) {
		DCC_LOG3(LOG_INFO, "len=%d cnt=%d n=%d", len, cnt, n);
		thinkos_flag_give(dev->rx_flag);
	} else {
		DCC_LOG1(LOG_INFO, "2. n=%d", n);
	}

//	DCC_LOG1(LOG_TRACE, "len=%d", n);
	DCC_LOG2(LOG_INFO, "len=%d '%c'...", n, cp[0]);

	return n;
}

int stm32f_serial_write(struct stm32f_serial_dev * dev, const void * buf,
						unsigned int len)
{
	uint8_t * cp = (uint8_t *)buf;
	int rem = len;

	DCC_LOG1(LOG_INFO, "len=%d", len);

#if SERIAL_ENABLE_TX_MUTEX
	 thinkos_mutex_lock(dev->tx_mutex); 
#endif

	while (rem) {
		unsigned int head;
		int free;
		int n;
		int i;

		thinkos_flag_take(dev->tx_flag);

		head = dev->tx_fifo.head;
		free = SERIAL_TX_FIFO_LEN - (int32_t)(head - dev->tx_fifo.tail);
		DCC_LOG3(LOG_MSG, "head=%d tail=%d n=%d", head, dev->tx_fifo.tail, n);
		n = MIN(rem, free);
		for (i = 0; i < n; ++i) 
			dev->tx_fifo.buf[head++ & (SERIAL_TX_FIFO_LEN - 1)] = *cp++;
		dev->tx_fifo.head = head;
		*dev->txie = 1; 

		if (free > n)
			thinkos_flag_give(dev->tx_flag);

		rem -= n;
		DCC_LOG1(LOG_INFO, "rem=%d", rem);
	}

#if SERIAL_ENABLE_TX_MUTEX
	thinkos_mutex_unlock(dev->tx_mutex); 
#endif

	return len;
}

int stm32f_serial_flush(struct stm32f_serial_dev * dev)
{
	do {
		thinkos_flag_take(dev->tx_flag);
	} while (((int32_t)dev->tx_fifo.head - dev->tx_fifo.tail) > 0);

	stm32_usart_flush(dev->uart);

	thinkos_flag_give(dev->tx_flag);

	return 0;
}

int stm32f_serial_close(struct stm32f_serial_dev * dev)
{
	return 0;
}

int serial_send(struct serial_dev * dev, const void * buf, 
				 unsigned int len) 
__attribute__ ((weak, alias ("stm32f_serial_write")));

int serial_recv(struct serial_dev * dev, void * buf, 
				unsigned int len, unsigned int msec)
__attribute__ ((weak, alias ("stm32f_serial_read")));

int serial_drain(struct serial_dev * dev)
__attribute__ ((weak, alias ("stm32f_serial_flush")));

#endif /* !SERIAL_ENABLE_DMA */

