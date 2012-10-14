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
 * @brief YARD-ICE application main
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

#include <sys/dcclog.h>

#define CONSOLE_RX_BUF_LEN 16

struct uart_console_ctrl {
	struct stm32f_usart * uart;
	struct stm32f_dma * dma;
	struct {
		struct stm32f_dma_stream * s;
	} tx;
	struct {
		struct stm32f_dma_stream * s;
		uint8_t buf[CONSOLE_RX_BUF_LEN];
	} rx;
};

int uart_console_read(struct uart_console_ctrl * ctrl, char * buf, 
				 unsigned int len, unsigned int msec)
{
	uint32_t st;

	if (ctrl->rx.s->cr & DMA_EN) {
		DCC_LOG(LOG_TRACE, "DMA enabled");
	}

	/* Memory address */
	ctrl->rx.s->m0ar = (void *)buf;
	/* Number of data items to transfer */
	ctrl->rx.s->ndtr = len;

	/* enable DMA */
	ctrl->rx.s->cr = DMA_CHSEL_SET(4) | DMA_MBURST_1 | DMA_PBURST_1 | 
		DMA_MSIZE_8 | DMA_PSIZE_8 | DMA_MINC | 
		DMA_DIR_PTM | DMA_TCIE  | DMA_EN;

	/* wait for the DMA transfer to complete */
	while (((st = ctrl->dma->hisr) & DMA_TCIF7) == 0) {
		thinkos_irq_wait(STM32F_IRQ_DMA1_STREAM7);
	} 

	/* clear the the DMA stream trasfer complete flag */
	ctrl->dma->hifcr = DMA_CTCIF7;

	return len;
}

int uart_console_write(struct uart_console_ctrl * ctrl, const void * buf, 
					   unsigned int len)
{
	uint32_t st;
	uint32_t sr;

	if (ctrl->tx.s->cr & DMA_EN) {
		DCC_LOG(LOG_TRACE, "DMA enabled");
	}

	/* Memory address */
	ctrl->tx.s->m0ar = (void *)buf;
	/* Number of data items to transfer */
	ctrl->tx.s->ndtr = len;

	/* clear the TC bit */
	if ((sr = ctrl->uart->sr) & USART_TC) {
		DCC_LOG(LOG_TRACE, "TC=1");
	}

	/* enable DMA */
	ctrl->tx.s->cr = DMA_CHSEL_SET(4) | DMA_MBURST_1 | DMA_PBURST_1 | 
		DMA_MSIZE_8 | DMA_PSIZE_8 | DMA_MINC | 
		DMA_DIR_MTP | DMA_TCIE  | DMA_EN;

	/* wait for the DMA transfer to complete */
	while (((st = ctrl->dma->lisr) & DMA_TCIF0) == 0) {
		thinkos_irq_wait(STM32F_IRQ_DMA1_STREAM0);
	} 

	/* clear the the DMA stream trasfer complete flag */
	ctrl->dma->lifcr = DMA_CTCIF0;

	return len;
}

int uart_console_flush(struct uart_console_ctrl * ctrl)
{
	return 0;
}

const struct fileop uart_console_ops = {
	.write = (void *)uart_console_write,
	.read = (void *)uart_console_read,
	.flush = (void *)uart_console_flush,
	.close = (void *)NULL
};

const struct uart_console_ctrl uart5_ctrl = {
	.uart = STM32F_UART5, 
	.dma = STM32F_DMA1,
	.rx.s = &STM32F_DMA1->s[0],
	.tx.s = &STM32F_DMA1->s[7]
};

const struct file uart_console_file = {
	.data = (void *)&uart5_ctrl, 
	.op = &uart_console_ops
};

struct file * uart_console_open(unsigned int baudrate, unsigned int flags)
{
	struct stm32f_rcc * rcc = STM32F_RCC;
	struct uart_console_ctrl * ctrl = (struct uart_console_ctrl *)uart_console_file.data;

	stm32f_usart_init(ctrl->uart, baudrate, flags);

	/* Enable DMA for transmission and reception */
	ctrl->uart->cr3 |= USART_DMAT | USART_DMAR;

	/* DMA clock enable */
	rcc->ahb1enr |= (ctrl->dma == STM32F_DMA1) ? RCC_DMA1EN : RCC_DMA2EN;

	/* Disable DMA stream */
	ctrl->tx.s->cr = 0;
	while (ctrl->tx.s->cr & DMA_EN); /* Wait for the channel to be ready .. */
	ctrl->tx.s->par = &ctrl->uart->dr;
//	ctrl->tx.s->fcr = 0;
	ctrl->tx.s->fcr = DMA_DMDIS | DMA_FTH_FULL;

	/* Disable DMA stream */
	ctrl->rx.s->cr = 0;
	while (ctrl->rx.s->cr & DMA_EN); /* Wait for the channel to be ready .. */
	ctrl->rx.s->par = &ctrl->uart->dr;
	/* Memory address */
	ctrl->tx.s->m0ar = (void *)ctrl->rx.buf;
//	ctrl->tx.s->m0ar = (void *)ctrl->rx.buf[0];
//	ctrl->tx.s->m1ar = (void *)ctrl->rx.buf[1];
	/* Number of data items to transfer */
	ctrl->tx.s->ndtr = CONSOLE_RX_BUF_LEN;
	ctrl->rx.s->cr = DMA_CHSEL_SET(4) | DMA_MBURST_1 | DMA_PBURST_1 | 
		DMA_MSIZE_8 | DMA_PSIZE_8 | DMA_MINC | 
		DMA_DIR_PTM;

	return (struct file *)&uart_console_file;
}

