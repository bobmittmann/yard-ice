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


int stm32f_serial_init(struct stm32f_serial_dev * dev, 
					   unsigned int baudrate, unsigned int flags)
{
	dev->rx.flag = thinkos_flag_alloc();
	dev->tx.flag = thinkos_flag_alloc();

#if SERIAL_TX_MUTEX
	dev->tx_mutex = thinkos_mutex_alloc(); 
	DCC_LOG1(LOG_TRACE, "tx_mutex=%d", dev->tx_mutex);
#endif

	DCC_LOG(LOG_TRACE, "...");

	/* Enable DMA for transmission and reception */
	stm32_usart_init(dev->uart);
	stm32_usart_baudrate_set(dev->uart, baudrate);
	stm32_usart_mode_set(dev->uart, SERIAL_8N1);

	dev->uart->cr3 |= USART_DMAT | USART_DMAR;

	/* DMA clock enable */
	if (dev->dma == STM32F_DMA1)
		stm32_clk_enable(STM32_RCC, STM32_CLK_DMA1);
	else
		stm32_clk_enable(STM32_RCC, STM32_CLK_DMA2);

	/* Disable DMA stream */
	dev->tx.s->cr = 0;
	while (dev->tx.s->cr & DMA_EN); /* Wait for the channel to be ready .. */
	dev->tx.s->par = &dev->uart->dr;
	dev->tx.s->fcr = DMA_DMDIS | DMA_FTH_FULL;

	/* Disable DMA stream */
	dev->rx.s->cr = 0;
	while (dev->rx.s->cr & DMA_EN); /* Wait for the channel to be ready .. */
	/* peripheral address */
	dev->rx.s->par = &dev->uart->dr;
	/* Memory address */
	dev->rx.s->m0ar = (void *)dev->rx.buf[0];
	dev->rx.s->m1ar = (void *)dev->rx.buf[1];
	/* Number of data items to transfer */
	dev->rx.s->ndtr = SERIAL_RX_BUF_LEN;
	dev->rx.s->cr = DMA_CHSEL_SET(4) | DMA_MBURST_1 | DMA_PBURST_1 |
		DMA_CT_M0AR | DMA_DBM |  DMA_MSIZE_8 | DMA_PSIZE_8 | DMA_MINC |
		DMA_CIRC | DMA_DIR_PTM | DMA_TCIE;

	/* receive buffer index */
	dev->rx.idx = -1;

	/* Enable DMA */
	dev->rx.s->cr |= DMA_EN;

	return 0;
}

int stm32f_serial_read(struct stm32f_serial_dev * dev, char * buf, 
					   unsigned int len)
{
	uint32_t st;
	unsigned int cnt;

	DCC_LOG(LOG_INFO, "read");

	if (dev->rx.s->cr & DMA_EN) {
		DCC_LOG(LOG_TRACE, "DMA enabled");
	}

	/* Memory address */
	dev->rx.s->m0ar = (void *)buf;
	/* Number of data items to transfer */
	dev->rx.s->ndtr = len;

	/* enable DMA */
	dev->rx.s->cr = DMA_CHSEL_SET(4) | DMA_MBURST_1 | DMA_PBURST_1 |
		DMA_MSIZE_8 | DMA_PSIZE_8 | DMA_MINC |
		DMA_DIR_PTM | DMA_TCIE  | DMA_EN;

	/* wait for the DMA transfer to complete */
	while (((st = dev->dma->lisr) & DMA_TCIF0) == 0) {
		DCC_LOG(LOG_INFO, "wait...");
		__thinkos_irq_wait(dev->rx.irq);
		DCC_LOG(LOG_INFO, "wakeup.");
	}

	/* clear the the DMA stream trasfer complete flag */
	dev->dma->lifcr = DMA_CTCIF0;

	/* Number of data items transfered... */
	cnt = len - dev->rx.s->ndtr;

	DCC_LOG1(LOG_TRACE, "cnt=%d", cnt);
	return cnt;
}

int stm32f_serial_write(struct stm32f_serial_dev * dev, const void * buf,
						unsigned int len)
{
	unsigned int cnt;
	uint32_t st;
	uint32_t sr;

	DCC_LOG1(LOG_TRACE, "len=%d", len);

	if (dev->tx.s->cr & DMA_EN) {
		DCC_LOG(LOG_TRACE, "DMA enabled");
	}

	/* Memory address */
	dev->tx.s->m0ar = (void *)buf;
	/* Number of data items to transfer */
	dev->tx.s->ndtr = len;

	/* clear the TC bit */
	if ((sr = dev->uart->sr) & USART_TC) {
		DCC_LOG(LOG_INFO, "TC=1");
	}

	/* enable DMA */
	dev->tx.s->cr = DMA_CHSEL_SET(4) | DMA_MBURST_1 | DMA_PBURST_1 |
		DMA_MSIZE_8 | DMA_PSIZE_8 | DMA_MINC |
		DMA_DIR_MTP | DMA_TCIE  | DMA_EN;

	/* wait for the DMA transfer to complete */
	while (((st = dev->dma->hisr) & DMA_TCIF7) == 0) {
		DCC_LOG(LOG_INFO, "wait...");
		__thinkos_irq_wait(dev->tx.irq);
		DCC_LOG(LOG_INFO, "wakeup");
	}

	/* clear the the DMA stream transfer complete flag */
	dev->dma->hifcr = DMA_CTCIF7;

	/* Number of data items transfered... */
	cnt = len - dev->tx.s->ndtr;

	DCC_LOG1(LOG_INFO, "cnt=%d", cnt);
	return cnt;
}


int serial_write(struct serial_dev * dev, const void * buf, 
				 unsigned int len) 
__attribute__ ((weak, alias ("stm32f_serial_write")));

int serial_read(struct serial_dev * dev, void * buf, 
				unsigned int len, unsigned int msec)
__attribute__ ((weak, alias ("stm32f_serial_read")));


