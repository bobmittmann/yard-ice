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

#define __STM32_SERIAL_I__
#include "stm32-serial-i.h"

struct stm32f_serial_dev uart5_serial_dev = {
	.uart = STM32_UART5,
#if SERIAL_ENABLE_DMA
	.dma = STM32F_DMA1,
	.rx = {
		.irq = STM32F_IRQ_DMA1_STREAM0,
		.s = &STM32F_DMA1->s[0]
	},
	.tx = {
		.irq = STM32F_IRQ_DMA1_STREAM7,
		.s = &STM32F_DMA1->s[7]
	}
#endif
};

void stm32f_uart5_isr(void)
{
	struct stm32f_serial_dev * dev = &uart5_serial_dev;
	struct stm32_usart * us;
	uint32_t sr;
	int c;
	
	us = dev->uart;
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
		dev->rx.s->cr &= ~DMA_EN;
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

		head = dev->rx_fifo.head;
		free = SERIAL_RX_FIFO_LEN - (uint8_t)(head - dev->rx_fifo.tail);
		if (free > 0) { 
			dev->rx_fifo.buf[head & (SERIAL_RX_FIFO_LEN - 1)] = c;
			dev->rx_fifo.head = head + 1;
		} else {
			DCC_LOG(LOG_WARNING, "RX fifo full!");
		}
		if (free < (SERIAL_RX_FIFO_LEN / 2)) /* fifo is more than half full */
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
			us->dr = dev->tx_fifo.buf[tail & (SERIAL_TX_FIFO_LEN - 1)];
			dev->tx_fifo.tail = tail + 1;
		}
	}
#endif
}

struct serial_dev * stm32f_uart5_serial_init(unsigned int baudrate, 
											 unsigned int flags)
{
	struct stm32f_serial_dev * dev = &uart5_serial_dev;

	DCC_LOG(LOG_TRACE, "IDLE!");

	stm32f_serial_init(dev, baudrate, flags);

	/* configure interrupts */
	cm3_irq_pri_set(STM32_IRQ_UART5, SERIAL_IRQ_PRIORITY);
	/* enable interrupts */
	cm3_irq_enable(STM32_IRQ_UART5);

	return (struct serial_dev *)dev;
}

