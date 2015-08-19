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

#ifdef STM32_UART7

struct stm32f_serial_dma_drv uart7_serial_dma_drv = {
	.uart = STM32_UART7
};

void stm32f_uart7_dma_isr(void)
{
	stm32f_serial_dma_isr(&uart7_serial_dma_drv);
}

void stm32f_dma1_stream1_isr(void)
{
	stm32f_serial_dma_rx_isr(&uart7_serial_dma_drv);
}

const struct serial_dev uart7_serial_dma_dev = {
	.drv = &uart7_serial_dma_drv,
	.op = &stm32f_uart_serial_dma_op
};

struct serial_dev * stm32f_uart7_serial_dma_init(unsigned int baudrate, 
												 unsigned int flags)
{
	struct stm32f_serial_dma_drv * drv = &uart7_serial_dma_drv;

	DCC_LOG(LOG_TRACE, "...");

	stm32_dmactl_init(&drv->rx.dmactl, STM32F_DMA1, 1); 
	stm32_dmactl_init(&drv->tx.dmactl, STM32F_DMA1, 3); 
	stm32f_serial_dma_init(drv, baudrate, flags, 5);

	/* configure and Enable interrupts */
#ifdef THINKAPP
	thinkos_irq_register(STM32_IRQ_UART7, SERIAL_IRQ_PRIORITY, 
						 stm32f_uart7_dma_isr);

	thinkos_irq_register(STM32F_IRQ_DMA1_STREAM1, SERIAL_IRQ_PRIORITY, 
						 stm32f_dma1_stream1_isr);
#else
	cm3_irq_pri_set(STM32_IRQ_UART7, SERIAL_IRQ_PRIORITY);
	cm3_irq_enable(STM32_IRQ_UART7);

	cm3_irq_pri_set(STM32F_IRQ_DMA1_STREAM1, SERIAL_IRQ_PRIORITY);
	cm3_irq_enable(STM32F_IRQ_DMA1_STREAM1);
#endif

	DCC_LOG(LOG_TRACE, "done!");

	return (struct serial_dev *)&uart7_serial_dma_dev;
}

#endif

