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

#define UART_FIFO_BUF_LEN 64

#define USART5_TXIE ((uint32_t *) CM3_PERIPHERAL_BITBAND_ADDR( \
						STM32F_BASE_UART5 + STM32F_USART_CR1, 7))

void stm32f_uart5_isr(void)
{
	struct stm32f_usart * us = STM32F_UART5;
	uint32_t sr;
	int c;
	
	sr = us->sr;

	if (sr & USART_RXNE) {
		DCC_LOG(LOG_INFO, "RXNE");
		c = us->dr;
		(void)c;
		__thinkos_ev_raise(dev->rx_ev);
	}	

	if (sr & USART_IDLE) {
		DCC_LOG(LOG_INFO, "IDLE");
		c = us->dr;
		(void)c;
		__thinkos_ev_raise(dev->rx_ev);
	}

	if (sr & USART_TXE) {
		DCC_LOG(LOG_MSG, "TXE");
		/* disable TXE interrupts */
		*USART5_TXIE = 0;
		__thinkos_ev_raise(dev->tx_ev);
	}
}

struct stm32f_usart * us = STM32F_UART5;
