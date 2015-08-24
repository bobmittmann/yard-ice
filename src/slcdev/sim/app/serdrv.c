/* 
 * File:	 rs485_lnk.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2013 Bob Mittmann. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <sys/serial.h>
#include <sys/param.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/dcclog.h>

#include "board.h"


#define UART_TX_FIFO_BUF_LEN 128
#define UART_RX_FIFO_BUF_LEN 128

struct serdrv {
	struct {
		volatile uint8_t head;
		volatile uint8_t tail;
		uint8_t buf[UART_TX_FIFO_BUF_LEN];
	} tx_fifo;	
	struct {
		volatile uint8_t head;
		volatile uint8_t tail;
		uint8_t buf[UART_RX_FIFO_BUF_LEN];
	} rx_fifo;	
	uint32_t * txie;
};

int serdrv_recv(struct serdrv * dev, void * buf, int len, unsigned int tmo)
{
	uint8_t * cp = (uint8_t *)buf;
	unsigned int tail;
	int ret;
	int cnt;
	int n;
	int i;

	DCC_LOG2(LOG_INFO, "1. len=%d tmo=%d", len, tmo);

again:
	if ((ret = thinkos_gate_timedwait(SERDRV_RX_GATE, tmo)) < 0) {
		DCC_LOG1(LOG_INFO, "cnt=%d, timeout!", 
				 (int8_t)(dev->rx_fifo.head - dev->rx_fifo.tail));
		return ret;
	}

	tail = dev->rx_fifo.tail;
	cnt = (int8_t)(dev->rx_fifo.head - tail);
	if (cnt == 0) {
		DCC_LOG(LOG_WARNING, "RX FIFO empty!");
		goto again;
	}
	n = MIN(len, cnt);

	for (i = 0; i < n; ++i)
		cp[i] = dev->rx_fifo.buf[tail++ & (UART_RX_FIFO_BUF_LEN - 1)];

	dev->rx_fifo.tail = tail;

	thinkos_gate_exit(SERDRV_RX_GATE, cnt > n);

	return n;
}

int serdrv_send(struct serdrv * dev, const void * buf, int len)
{
	uint8_t * cp = (uint8_t *)buf;
	int rem = len;

	DCC_LOG1(LOG_INFO, "len=%d", len);

	while (rem) {
		unsigned int head;
		int free;
		int n;
		int i;

		thinkos_gate_wait(SERDRV_TX_GATE);

		head = dev->tx_fifo.head;
		free = UART_TX_FIFO_BUF_LEN - (int8_t)(head - dev->tx_fifo.tail);
		DCC_LOG3(LOG_MSG, "head=%d tail=%d n=%d", head, dev->tx_fifo.tail, n);
		n = MIN(rem, free);
		for (i = 0; i < n; ++i) 
			dev->tx_fifo.buf[head++ & (UART_TX_FIFO_BUF_LEN - 1)] = *cp++;
		dev->tx_fifo.head = head;
		*dev->txie = 1; 

		rem -= n;
		free -= n;

		thinkos_gate_exit(SERDRV_TX_GATE, free);

		DCC_LOG1(LOG_INFO, "rem=%d", rem);
	}


	return len;
}

void serdrv_flush(struct serdrv * dev)
{
	thinkos_gate_wait(SERDRV_TX_GATE);

	stm32_usart_flush(STM32_USART2);

	thinkos_gate_exit(SERDRV_TX_GATE, 0);
}


/* static serial driver object */
static struct serdrv serial2_dev;

void stm32_usart2_isr(void)
{
	struct serdrv * dev = &serial2_dev;
	struct stm32_usart * us = STM32_USART2;
	uint32_t sr;
	int c;
	
	sr = us->sr & us->cr1;

	if (sr & USART_RXNE) {
		unsigned int head;
		int free;

		DCC_LOG(LOG_INFO, "RXNE");

		c = us->dr;
		head = dev->rx_fifo.head;
		free = UART_RX_FIFO_BUF_LEN - (uint8_t)(head - dev->rx_fifo.tail);
		if (free > 0) { 
			dev->rx_fifo.buf[head & (UART_RX_FIFO_BUF_LEN - 1)] = c;
			dev->rx_fifo.head = head + 1;
		} else {
			DCC_LOG(LOG_WARNING, "RX fifo full!");
		}
		if (free == (UART_RX_FIFO_BUF_LEN - 8)) /* fifo is partially full */
			thinkos_gate_open_i(SERDRV_RX_GATE);
	}	

	if (sr & USART_IDLE) {
		DCC_LOG(LOG_INFO, "IDLE!");
		c = us->dr;
		(void)c;
		thinkos_gate_open_i(SERDRV_RX_GATE);
	}

	if (sr & USART_TXE) {
		unsigned int tail = dev->tx_fifo.tail;
		if (tail == dev->tx_fifo.head) {
			/* FIFO empty, disable TXE interrupts */
			*dev->txie = 0; 
			/* Signal the Tx thread */
			thinkos_gate_open_i(SERDRV_TX_GATE);
		} else {
			us->dr = dev->tx_fifo.buf[tail & (UART_TX_FIFO_BUF_LEN - 1)];
			dev->tx_fifo.tail = tail + 1;
		}
	}
}


struct serdrv * serdrv_init(unsigned int speed)
{
	struct serdrv * drv  = &serial2_dev;
	struct stm32_usart * uart = STM32_USART2;

	DCC_LOG1(LOG_MSG, "speed=%d", speed);

	drv->tx_fifo.head = drv->tx_fifo.tail = 0;
	drv->rx_fifo.head = drv->rx_fifo.tail = 0;
	drv->txie = CM3_BITBAND_DEV(&uart->cr1, 7);
	thinkos_gate_open(SERDRV_TX_GATE);

	/* clock enable */
	stm32_clk_enable(STM32_RCC, STM32_CLK_USART2);

	/*********************************************
	 * USART 
	 *********************************************/
	stm32_usart_init(uart);
	stm32_usart_baudrate_set(uart, speed);
	stm32_usart_mode_set(uart, SERIAL_8N1);

	/* Enable DMA for transmission and reception */
//	uart->cr3 |= USART_DMAT | USART_DMAR;
	/* enable idle line interrupt */
	/* enable RX interrupt */
	uart->cr1 |= USART_RXNEIE | USART_IDLEIE;

	/* enable UART */
	stm32_usart_enable(uart);

	/* configure interrupts */
	cm3_irq_pri_set(STM32_IRQ_USART2, IRQ_PRIORITY_LOW);
	/* enable interrupts */
	cm3_irq_enable(STM32_IRQ_USART2);

	return drv;
}


/* ----------------------------------------------------------------------
 * Serial driver file operations 
 * ----------------------------------------------------------------------
 */

#include <sys/tty.h>

const struct fileop serdrv_ops = {
	.write = (void *)serdrv_send,
	.read = (void *)serdrv_recv,
	.flush = (void *)serdrv_flush,
	.close = (void *)NULL
};

FILE * serdrv_tty_fopen(struct serdrv * drv)
{
	struct tty_dev * tty;
	FILE * f_raw;
	FILE * f_tty;

	f_raw = file_alloc(drv, &serdrv_ops);
	tty = tty_attach(f_raw);
	f_tty = tty_fopen(tty);

	return f_tty;
}

