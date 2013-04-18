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

#include <arch/cortex-m3.h>
#include <sys/delay.h>
#include <sys/serial.h>
#include <sys/param.h>
#include <stdlib.h>
#include <stdio.h>

#define __THINKOS_IRQ__
#include <thinkos_irq.h>
#include <thinkos.h>

#include "rs485lnk.h"
#include "trace.h"

void rs485_init(struct rs485_link * lnk, 
				struct stm32f_usart * uart,
				unsigned int speed,
				struct stm32f_dma * dma,
				int rx_dma_strm_id, int rx_dma_chan_id, 
				int tx_dma_strm_id, int tx_dma_chan_id)
{
	struct stm32f_dma_stream * tx_dma_strm;
	struct stm32f_dma_stream * rx_dma_strm;
	struct stm32f_rcc * rcc = STM32F_RCC;
	uint32_t us;
	int c;

	tracef("%s():...", __func__);

	lnk->uart = uart;
	lnk->dma = dma;
	lnk->tx.pend_pkt = NULL;

	lnk->uart_irq = stm32f_usart_irq_lookup(uart);

	/* DMA clock enable */
	if (dma == STM32F_DMA1) {
		rcc->ahb1enr |= RCC_DMA1EN;
		lnk->tx.dma_irq = stm32f_dma1_irqnum_lut[tx_dma_strm_id];
		lnk->rx.dma_irq = stm32f_dma1_irqnum_lut[rx_dma_strm_id];
	} else {
		rcc->ahb1enr |= RCC_DMA2EN;
		lnk->tx.dma_irq = stm32f_dma2_irqnum_lut[tx_dma_strm_id];
		lnk->rx.dma_irq = stm32f_dma2_irqnum_lut[rx_dma_strm_id];
	}

	/* TX DMA */
	lnk->tx.dma_id = tx_dma_strm_id;

	lnk->tx.isr = dma_isr_bitband(dma, tx_dma_strm_id);
	lnk->tx.ifcr = dma_ifcr_bitband(dma, tx_dma_strm_id);

	DCC_LOG2(LOG_TRACE, "0x%p 0x%p", lnk->tx.isr, lnk->tx.ifcr);

	tx_dma_strm = &dma->s[tx_dma_strm_id];
	lnk->tx.dma_strm = tx_dma_strm;
	DCC_LOG2(LOG_TRACE, "TX DMA stream[%d]=0x%p", lnk->tx.dma_id, tx_dma_strm);

	/* Disable DMA stream */
	tx_dma_strm->cr = 0;
	while (tx_dma_strm->cr & DMA_EN); /* Wait for the channel to be ready .. */

	/* clear all interrupt flags */
	lnk->tx.ifcr[FEIF_BIT] = 1;
	lnk->tx.ifcr[DMEIF_BIT] = 1;
	lnk->tx.ifcr[TEIF_BIT] = 1;
	lnk->tx.ifcr[HTIF_BIT] = 1;
	lnk->tx.ifcr[TCIF_BIT] = 1; 

	/* configure TX DMA stream */
	tx_dma_strm->cr = DMA_CHSEL_SET(tx_dma_chan_id) | 
		DMA_MBURST_1 | DMA_PBURST_1 | 
		DMA_MSIZE_8 | DMA_PSIZE_8 | DMA_MINC | DMA_DIR_MTP | 
		DMA_TCIE | DMA_TEIE;

	tx_dma_strm->par = &uart->dr;
	tx_dma_strm->fcr = DMA_FEIE | DMA_DMDIS | DMA_FTH_FULL;


	/* RX DMA */
	lnk->rx.dma_id = rx_dma_strm_id;

	lnk->rx.isr = dma_isr_bitband(dma, rx_dma_strm_id);
	lnk->rx.ifcr = dma_ifcr_bitband(dma, rx_dma_strm_id);

	rx_dma_strm = &dma->s[rx_dma_strm_id];
	lnk->rx.dma_strm = rx_dma_strm;
	DCC_LOG2(LOG_TRACE, "RX DMA stream[%d]=0x%p", lnk->rx.dma_id, rx_dma_strm);

	/* Disable DMA stream */
	rx_dma_strm->cr = 0;
	while (rx_dma_strm->cr & DMA_EN); /* Wait for the channel to be ready .. */

	/* clear all interrupt flags */
	lnk->rx.ifcr[FEIF_BIT] = 1;
	lnk->rx.ifcr[DMEIF_BIT] = 1;
	lnk->rx.ifcr[TEIF_BIT] = 1;
	lnk->rx.ifcr[HTIF_BIT] = 1;
	lnk->rx.ifcr[TCIF_BIT] = 1; 

	/* configure RX DMA stream */
	rx_dma_strm->cr = DMA_CHSEL_SET(rx_dma_chan_id) | 
		DMA_MBURST_1 | DMA_PBURST_1 | 
		DMA_MSIZE_8 | DMA_PSIZE_8 | DMA_MINC | DMA_DIR_PTM |
		DMA_TCIE | DMA_TEIE;
	rx_dma_strm->par = &uart->dr;
	rx_dma_strm->fcr = DMA_FEIE | DMA_DMDIS | DMA_FTH_FULL;

	stm32f_usart_init(uart);
	stm32f_usart_baudrate_set(uart, speed);
	stm32f_usart_mode_set(uart, SERIAL_8N1);

	/* 3 characters IDLE time:
	   - 1 char in the TX holding buffer
	   - 1 char in the TX shift register
	   - 1 idle char */
	us = ((30 * 1000 * 1000) / speed) + 1;
	lnk->idle_tm = (us / 1000) + 1;

	tracef("idle_tm = %d.%03d", lnk->idle_tm, us % 1000);
	tracef("%s() lnk->uart_irq=%d", __func__, lnk->uart_irq);

	/* Enable DMA for transmission and reception */
	uart->cr3 |= USART_DMAT | USART_DMAR;

	/* enable idle line interrupt */
	uart->cr1 |= USART_IDLEIE;

	/* clear pending data */
	c = uart->dr;
	(void)c;
	c = uart->dr;
	(void)c;

	stm32f_usart_enable(uart);
}

int rs485_pkt_receive(struct rs485_link * lnk, void ** ppkt, int max_len)
{
	struct stm32f_dma_stream * dma_strm = lnk->rx.dma_strm;
	struct stm32f_usart * uart = lnk->uart;
	void * rcvd_pkt;
	uint32_t sr;
	int len;
	int c;

	DCC_LOG2(LOG_INFO, "DMA stream[%d]=0x%p", lnk->rx.dma_id, dma_strm);

	if (lnk->rx.pend_pkt == NULL) {
		trace("no queued DMA buffer");
		rcvd_pkt = NULL;
		len = 0;
	} else {
		DCC_LOG(LOG_TRACE, "pending packet...");

//		trace("pending packet...");

		while (((sr = uart->sr) & USART_IDLE) == 0) {
//			trace("wait UART IDLE ...");
			thinkos_irq_wait(lnk->uart_irq);
		};

		/* clear interrupt flag */
		c = uart->dr;
		(void)c;

		if (dma_strm->cr & DMA_EN) {
//			trace("DMA enabled, disabling!");
			lnk->rx.dma_strm->cr &= ~DMA_EN;
		}

		/* wait for completion of DMA transfer */
		while (!lnk->rx.isr[TCIF_BIT]) {

			if (lnk->rx.isr[TEIF_BIT]) {
				DCC_LOG(LOG_ERROR, "DMA transfer error!");
				trace("DMA transfer error...");
				lnk->rx.ifcr[TEIF_BIT] = 1;
			}

			if (lnk->rx.isr[FEIF_BIT]) {
				trace("DMA fifo error...");
				lnk->rx.ifcr[FEIF_BIT] = 1;
			}

			if (lnk->rx.isr[HTIF_BIT]) {
//				trace("DMA half transfer...");
				lnk->rx.ifcr[HTIF_BIT] = 1;
			}

//			trace("wait DMA transfer ...");
			thinkos_irq_wait(lnk->rx.dma_irq);
		} 

		/* return a reference to the packet just received */
		rcvd_pkt = lnk->rx.pend_pkt;

		/* Number of data items transfered... */
		len = lnk->rx.max_len - dma_strm->ndtr;

		/* clear the the DMA trasfer complete flag */
		lnk->rx.ifcr[TCIF_BIT] = 1;
//		tracef("DMA rcvd len=%d max_len=%d.", len, max_len);
	}

	if (dma_strm->cr & DMA_EN) {
		DCC_LOG(LOG_ERROR, "DMA enabled");
		trace("ERROR: DMA enabled!");
	}

	/* set DMA memory address */
	dma_strm->m0ar = (void *)lnk->rx.pend_pkt;
	/* set DMA number of data items to transfer */
	dma_strm->ndtr = max_len;
	/* enable DMA */
	dma_strm->cr |= DMA_EN;

	lnk->rx.pend_pkt = *ppkt;
	lnk->rx.max_len = max_len;

	/* return previous pending packet */
	*ppkt = rcvd_pkt;

	return len;
}

int __rs485_pkt_receive(struct rs485_link * lnk, void ** ppkt, int max_len)
{
	struct stm32f_dma_stream * dma_strm = lnk->rx.dma_strm;
	struct stm32f_usart * uart = lnk->uart;
	void * rcvd_pkt;
	uint32_t sr;
	int len;

	DCC_LOG2(LOG_INFO, "DMA stream[%d]=0x%p", lnk->rx.dma_id, dma_strm);

//	tracef(".");
	if (lnk->rx.pend_pkt == NULL) {
		rcvd_pkt = NULL;
		len = 0;
	} else {
		DCC_LOG(LOG_TRACE, "pending packet...");

		trace("pending packet...");

		/* wait for completion of DMA transfer */
		while (!lnk->rx.isr[TCIF_BIT]) {

			if (lnk->rx.isr[TEIF_BIT]) {
				DCC_LOG(LOG_ERROR, "DMA transfer error!");
				trace("DMA transfer error...");
				lnk->rx.ifcr[TEIF_BIT] = 1;
			}

			if (lnk->rx.isr[FEIF_BIT]) {
				trace("DMA fifo error...");
				lnk->rx.ifcr[FEIF_BIT] = 1;
			}

			if (lnk->rx.isr[HTIF_BIT]) {
//				trace("DMA half transfer...");
				lnk->rx.ifcr[HTIF_BIT] = 1;
			}

			trace("wait...");
			thinkos_irq_wait(lnk->rx.dma_irq);
		} 

		/* return a reference to the packet just received */
		rcvd_pkt = lnk->rx.pend_pkt;

		/* Number of data items transfered... */
		len = lnk->rx.max_len - dma_strm->ndtr;

		/* clear the the DMA trasfer complete flag */
		lnk->rx.ifcr[TCIF_BIT] = 1;

		tracef("DMA received len=%d max_len=%d.", len, max_len);
	}

	if (dma_strm->cr & DMA_EN) {
		DCC_LOG(LOG_ERROR, "DMA enabled");
		trace("ERROR: DMA enabled!");
	}

	lnk->rx.pend_pkt = *ppkt;
	lnk->rx.max_len = max_len;

	/* set DMA memory address */
	dma_strm->m0ar = (void *)lnk->rx.pend_pkt;
	/* set DMA number of data items to transfer */
	dma_strm->ndtr = max_len;

	/* clear the TC bit to start transfer */
	if ((sr = uart->sr) & USART_TC) {
		DCC_LOG(LOG_INFO, "TC=1");
		trace("TC=1");
		uart->sr = 0;
	}

	/* enable DMA */
	dma_strm->cr |= DMA_EN;

	/* return previous pending packet */
	*ppkt = rcvd_pkt;

	return len;
}


void * rs485_pkt_enqueue(struct rs485_link * lnk, void * pkt, int len)
{
	struct stm32f_usart * uart = lnk->uart;
	struct stm32f_dma_stream * dma_strm = lnk->tx.dma_strm;
	uint32_t sr;
	uint32_t cr;
	void * pend_pkt = NULL;

	DCC_LOG2(LOG_INFO, "DMA stream[%d]=0x%p", lnk->tx.dma_id, dma_strm);

	if (lnk->tx.pend_pkt) {
		DCC_LOG(LOG_TRACE, "pending packet...");

		/* wait for the DMA transfer to complete */
		while (!lnk->tx.isr[TCIF_BIT]) {
			if (lnk->tx.isr[TEIF_BIT]) {
				DCC_LOG(LOG_ERROR, "DMA transfer error!");
				lnk->tx.ifcr[TEIF_BIT] = 1;
			}
			thinkos_irq_wait(lnk->tx.dma_irq);
		} 
		/* clear the the DMA trasfer complete flag */
		lnk->tx.ifcr[TCIF_BIT] = 1;

		/* return a reference to the packet just transmitted */
		pend_pkt = lnk->tx.pend_pkt;

		if ((sr = uart->sr) & USART_TC) {
//			trace("USART TC=1, generating an idle frame");
			/* pulse the TE bit to generate an idle frame */
			cr = uart->cr1;
			uart->cr1 = cr & ~USART_TE;
			uart->cr1 = cr | USART_TE;
		} else {
			thinkos_sleep(lnk->idle_tm);
		}
	}
	
	if (dma_strm->cr & DMA_EN) {
		DCC_LOG(LOG_ERROR, "DMA enabled");
		trace("ERROR: DMA enabled!");
	}

	/* set this packet as pending */
	lnk->tx.pend_pkt = pkt;
	/* set DMA memory address */
	dma_strm->m0ar = (void *)pkt;
	/* set DMA number of data items to transfer */
	dma_strm->ndtr = len;

	/* clear the TC bit */
	if ((sr = uart->sr) & USART_TC) {
		DCC_LOG(LOG_INFO, "TC=1");
		uart->sr = 0;
	}

	/* enable DMA */
	dma_strm->cr |= DMA_EN;

	/* return previous pending packet */
	return pend_pkt;
}

