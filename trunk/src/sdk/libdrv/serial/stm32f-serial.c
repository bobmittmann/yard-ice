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

#define SERIAL_RX_BUF_LEN 64

struct stm32f_serial_dev {
	struct stm32f_usart * uart;
	struct stm32f_dma * dma;
	struct {
		int irq;
		struct stm32f_dma_stream * s;
	} tx;
	struct {
		int irq;
		struct stm32f_dma_stream * s;
		uint8_t buf[2][SERIAL_RX_BUF_LEN];
		uint32_t idx;
	} rx;

	int8_t rx_ev;
	int8_t tx_ev;
};

int stm32f_serial_init(struct stm32f_serial_dev * dev)
{
	struct stm32f_rcc * rcc = STM32F_RCC;

	dev->rx_ev = thinkos_ev_alloc();
	dev->tx_ev = thinkos_ev_alloc();

	DCC_LOG(LOG_TRACE, "...");
	stm32f_usart_init(dev->uart);

	/* Enable DMA for transmission and reception */
	dev->uart->cr3 |= USART_DMAT | USART_DMAR;

	/* DMA clock enable */
	rcc->ahb1enr |= (dev->dma == STM32F_DMA1) ? RCC_DMA1EN : RCC_DMA2EN;

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

	stm32f_usart_baudrate_set(dev->uart, 115200);
	stm32f_usart_mode_set(dev->uart, SERIAL_8N1);

	/* receive buffer index */
	dev->rx.idx = -1;

	/* Enable DMA */
	dev->rx.s->cr |= DMA_EN;

	return 0;
}

int stm32f_serial_dma_recv(struct stm32f_serial_dev * dev, char ** buf)
{
	buf = dev->rx.buf[dev->rx.idx++ & 1];
}

void stm32f_dma2_stream0_isr(void)
{
	struct stm32f_dma * dma = STM32F_DMA2;
	uint16_t * data;

	if ((dma->lisr & DMA_TCIF0) == 0)
		return;

	/* clear the DMA transfer complete flag */
	dma->lifcr = DMA_CTCIF0;

	/* get a pointer to the last filled DMA transfer buffer */
	data = adc_buf[adc_dma_cnt++ & 1];

	/* scale and sotore the samples */
	adc_vin = (data[0] * ADC_INPUT_6_SCALE) / 4096;
	adc_vbat = (data[1] * ADC_INPUT_6_SCALE) / 4096;
	adc_temp = (data[2] * ADC_TEMP_SENS_SCALE) / 4096;

#if (ENABLE_ADC_SYNC)
	__thinkos_ev_raise(ev_adc_dma);
#endif
}


int stm32f_serial_read(struct stm32f_serial_dev * dev, char * buf, unsigned int len)
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

int uart_serial_write(struct stm32f_serial_dev * dev, const void * buf,
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


struct stm32f_serial_dev uart5_dev = {
	.uart = STM32F_UART5,
	.dma = STM32F_DMA1,
	.rx = {
		.irq = STM32F_IRQ_DMA1_STREAM0,
		.s = &STM32F_DMA1->s[0]
	},
	.tx = {
		.irq = STM32F_IRQ_DMA1_STREAM7,
		.s = &STM32F_DMA1->s[7]
	}
};

void stm32f_uart5_isr(void)
{
	struct stm32f_serial_dev * dev= (struct stm32f_serial_dev *)&uart5_dev;
	struct stm32f_usart * us;
	uint32_t sr;
	int c;
	
	us = dev->uart;
	sr = us->sr;

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

}
