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
#include <errno.h>

void stm32f_serial_dma_isr(struct stm32f_serial_dma_drv * drv)
{
	struct stm32_usart * us;
	uint32_t sr;
	
	us = drv->uart;
	sr = us->sr & us->cr1;

	if (sr & USART_ORE) {
		DCC_LOG2(LOG_WARNING, "%6d: OVR!, ndtr=%d", 
				 thinkos_clock_i(), drv->rx.strm->ndtr);
	}

	if (sr & USART_IDLE) {
		int c = us->rdr;
		(void)c;
		DCC_LOG(LOG_TRACE, "IDLE");
		thinkos_flag_give_i(drv->rx_idle);
	}

	if (sr & USART_TC) {
		DCC_LOG(LOG_TRACE, "TC");
		/* TC interrupt is cleared by writing 0 back to the SR register */
		us->sr = sr & ~USART_TC;
		/* diasble the transfer complete interrupt */
		drv->uart->cr1 &= ~USART_TCIE;
		thinkos_flag_give_i(drv->tx_done);
	}
}

void stm32f_serial_dma_rx_isr(struct stm32f_serial_dma_drv * drv)
{
	struct stm32_usart * us;
	uint32_t sr;
	
	us = drv->uart;
	sr = us->sr;

	if (sr & USART_ORE) {
		DCC_LOG2(LOG_WARNING, "%6d: OVR!, ndtr=%d", 
				 thinkos_clock_i(), drv->rx.strm->ndtr);
	}

	if (sr & USART_IDLE) {
		int c = us->rdr;
		(void)c;
		DCC_LOG(LOG_TRACE, "IDLE");
		thinkos_flag_give_i(drv->rx_idle);
	}

	DCC_LOG(LOG_TRACE, ".");

#if 0
	if (sr & USART_TC) {
		DCC_LOG(LOG_TRACE, "TC");
		/* TC interrupt is cleared by writing 0 back to the SR register */
		us->sr = sr & ~USART_TC;
	}
#endif
}

int stm32f_serial_dma_prepare(struct stm32f_serial_dma_drv * drv, 
							  void * buf, unsigned int len)
{
	if (drv->rx.strm->cr & DMA_EN) {
		DCC_LOG(LOG_ERROR, "DMA enabled");
		return -1;
	}
	DCC_LOG1(LOG_TRACE, "len=%d", len);
	drv->rx.buf_ptr = buf;
	drv->rx.buf_len = len;
	/* clear transfer complete interrupt flags */
	drv->rx.ifcr[TCIF_BIT] = 1; 
	/* Memory address */
	drv->rx.strm->m0ar = buf;
	/* Number of data items to transfer */
	drv->rx.strm->ndtr = len;
	/* enable DMA */
	drv->rx.strm->cr |= DMA_EN;

	return 0;
}

int stm32f_serial_dma_recv(struct stm32f_serial_dma_drv * drv, 
						   void * buf, unsigned int len, unsigned int tmo)
{
	unsigned int ndtr;
	unsigned int cnt;
	int ret;

	DCC_LOG3(LOG_TRACE, "%6d: len=%d tmo=%d", thinkos_clock(), len, tmo);

	/* if the sored buffer pointer is NULL it means that
	   we did not yet prepare ther DMA for a transfer,
	   in this case prepare to transfer using the provided 
	   buffer. */
	if (drv->rx.buf_ptr == NULL) {
		DCC_LOG(LOG_INFO, "DMA not prepared...");
		if (drv->rx.strm->cr & DMA_EN) {
			DCC_LOG(LOG_ERROR, "DMA enabled");
			return -11;
		}
		/* clear transfer complete interrupt flags */
		drv->rx.ifcr[TCIF_BIT] = 1; 
		/* Set memory address */
		drv->rx.strm->m0ar = buf;
		/* Number of data items to transfer */
		drv->rx.strm->ndtr = len;
		/* enable DMA */
		drv->rx.strm->cr |= DMA_EN;
		ndtr = len;
	} else {
		DCC_LOG(LOG_INFO, "DMA prepared...");
		ndtr = drv->rx.buf_len;
	}

	/* wait for the end of transfer */
//	DCC_LOG1(LOG_TRACE, "thinkos_flag_timedtake(%d)...", tmo);
	if ((ret = thinkos_flag_timedtake(drv->rx_idle, tmo)) < 0) {
		/* if the initially provided 'ndtr' differs from the
		   dma stream then the transfer already started,
		   in this case we wait until it finishes. */
		if ((cnt = ndtr - drv->rx.strm->ndtr) < 2) {
			if (drv->rx.buf_ptr == NULL) {
				/* Disable DMA stream */
				drv->rx.strm->cr &= ~DMA_EN;
			}
			DCC_LOG(LOG_INFO, "timeout...");
			return ret;
		} else {
			DCC_LOG1(LOG_WARNING, "thinkos_flag_take() cnt=%d  ...", cnt);
			thinkos_flag_take(drv->rx_idle);
		}
	}

	/* Disable DMA stream */
	drv->rx.strm->cr &= ~DMA_EN;
	/* wait for completion of DMA transfer */
	while (drv->rx.strm->cr & DMA_EN); /* Wait for the channel to be ready .. */

	/* Number of data items transfered... */
	cnt = ndtr - drv->rx.strm->ndtr;
	if (cnt == 0) {
		DCC_LOG(LOG_WARNING, "DMA XFR cnt == 0!!!");
		abort();
	}

	if (drv->rx.buf_ptr != NULL) {
		/* prepare next transfer */
		drv->rx.buf_ptr = buf;
		drv->rx.buf_len = len;
		if (buf != NULL) {
			/* prepare next transfer */
			/* clear transfer complete interrupt flags */
			drv->rx.ifcr[TCIF_BIT] = 1; 
			/* Memory address */
			drv->rx.strm->m0ar = (void *)buf;
			/* Number of data items to transfer */
			drv->rx.strm->ndtr = len;
			/* enable DMA */
			drv->rx.strm->cr |= DMA_EN;
		}
	}

	DCC_LOG2(LOG_INFO, "%6d: cnt=%d", thinkos_clock_i(), cnt);
	return cnt;
}

int stm32f_serial_dma_send(struct stm32f_serial_dma_drv * drv, 
						   const void * buf, unsigned int len)
{
	unsigned int cnt;
	DCC_LOG2(LOG_TRACE, "%6d: len=%d", thinkos_clock_i(), len);

	if (drv->tx.strm->cr & DMA_EN) {
		DCC_LOG(LOG_ERROR, "DMA enabled");
		abort();
	}

	/* Memory address */
	drv->tx.strm->m0ar = (void *)buf;
	/* Number of data items to transfer */
	drv->tx.strm->ndtr = len;
	/* clear the the DMA stream transfer complete flag */
	drv->tx.ifcr[TCIF_BIT] = 1;
	/* clear the UART transfer complete bit */
	drv->uart->sr &= ~USART_TC;
	/* enable DMA */
	drv->tx.strm->cr |= DMA_EN;
	/* enable the transfer complete interrupt */
	drv->uart->cr1 |= USART_TCIE;
	/* wait for the transfer to complete */
	thinkos_flag_take(drv->tx_done);

	/* Number of data items transfered... */
	if ((cnt = drv->tx.strm->ndtr) != 0) {
		DCC_LOG(LOG_ERROR, "DMA error!");
//		abort();
	}

	return len - cnt;
}

int stm32f_serial_dma_init(struct stm32f_serial_dma_drv * drv, 
					   unsigned int baudrate, unsigned int flags,
					   struct stm32f_dma * dma, int dma_chan_id, 
					   int rx_dma_strm_id, int tx_dma_strm_id)
{
	drv->rx_idle = thinkos_flag_alloc();
	drv->tx_done = thinkos_flag_alloc();
	DCC_LOG2(LOG_TRACE, "rx_idle=%d tx_done=%d", drv->rx_idle, drv->tx_done);
	DCC_LOG3(LOG_TRACE, "chan=%d rx_strm=%d tx_strm=%d", 
			 dma_chan_id, rx_dma_strm_id, tx_dma_strm_id);

#if SERIAL_TX_MUTEX
	drv->tx_mutex = thinkos_mutex_alloc(); 
	DCC_LOG1(LOG_TRACE, "tx_mutex=%d", drv->tx_mutex);
#endif

	/* Enable DMA for transmission and reception */
	stm32_usart_init(drv->uart);
	stm32_usart_baudrate_set(drv->uart, baudrate);
	stm32_usart_mode_set(drv->uart, SERIAL_8N1);

	drv->dma = dma;
	drv->rx.buf_ptr = NULL;
	drv->rx.buf_len = 0;

	/* -------------------------------------------------------
	   Setup driver pointers according to the DMA stream 
	 */
	if (dma == STM32F_DMA1) {
		/* DMA clock enable */
		stm32_clk_enable(STM32_RCC, STM32_CLK_DMA1);
		DCC_LOG(LOG_TRACE, "DMA1");
	} else {
		/* DMA clock enable */
		stm32_clk_enable(STM32_RCC, STM32_CLK_DMA2);
		DCC_LOG(LOG_TRACE, "DMA2");
	}

	/* Adjust TX pointers */
	drv->tx.strm = &dma->s[tx_dma_strm_id];
	drv->tx.ifcr = dma_ifcr_bitband(dma, tx_dma_strm_id);
	/* Adjust RX pointers */
	drv->rx.strm = &dma->s[rx_dma_strm_id];
	drv->rx.ifcr = dma_ifcr_bitband(dma, rx_dma_strm_id);
	/* ------------------------------------------------------- 
	 */

	DCC_LOG3(LOG_TRACE, "dma=%p rx.strm=%p tx.strm=%p",
			 drv->dma, drv->rx.strm, drv->tx.strm);

	/* -------------------------------------------------------
	   Configure TX DMA stream
	 */
	/* Disable DMA stream */
	drv->tx.strm->cr = 0;
	while (drv->tx.strm->cr & DMA_EN); /* Wait for the channel to be ready .. */
	/* clear all interrupt flags */
	drv->tx.ifcr[TEIF_BIT] = 1;
	drv->tx.ifcr[HTIF_BIT] = 1;
	drv->tx.ifcr[TCIF_BIT] = 1; 
	drv->tx.strm->par = &drv->uart->tdr;
//	drv->tx.strm->fcr = DMA_FEIE | DMA_DMDIS | DMA_FTH_FULL;
	/* configure TX DMA stream */
	drv->tx.strm->cr = DMA_CHSEL_SET(dma_chan_id) | 
		DMA_MSIZE_8 | DMA_PSIZE_8 | DMA_MINC | 
		DMA_DIR_MTP | DMA_TCIE | DMA_TEIE;
	/* ------------------------------------------------------- 
	 */

	/* -------------------------------------------------------
	   Configure RX DMA stream
	 */
	/* Disable DMA stream */
	drv->rx.strm->cr = 0;
	while (drv->rx.strm->cr & DMA_EN); /* Wait for the channel to be ready .. */
	/* clear all interrupt flags */
	drv->rx.ifcr[TEIF_BIT] = 1;
	drv->rx.ifcr[HTIF_BIT] = 1;
	drv->rx.ifcr[TCIF_BIT] = 1; 
	/* peripheral address */
	drv->rx.strm->par = &drv->uart->rdr;
	/* cofigure FIFO */
//	drv->rx.strm->fcr = DMA_FEIE | DMA_DMDIS | DMA_FTH_FULL;
	/* configure DMA */
	drv->rx.strm->cr = DMA_CHSEL_SET(dma_chan_id) | 
		DMA_MSIZE_8 | DMA_PSIZE_8 | DMA_MINC |
		DMA_DIR_PTM | DMA_TCIE | DMA_TEIE;
	/* ------------------------------------------------------- 
	 */

	/* configure the UART for DMA transfer */
	drv->uart->cr3 |= USART_DMAT | USART_DMAR;
	/* enable RX IDLE and errors interrupt */
	drv->uart->cr1 |= USART_IDLEIE | USART_EIE;
	/* enable UART */
	drv->uart->cr1 |= USART_UE | USART_TE | USART_RE;

	return 0;
}


int stm32f_serial_dma_drain(struct stm32f_serial_dma_drv * drv)
{
	DCC_LOG(LOG_TRACE, "...");
#if 0
	do {
		thinkos_flag_take(drv->tx_flag);
	} while ((drv->tx_fifo.head - drv->tx_fifo.tail) > 0);
	stm32_usart_flush(drv->uart);
#endif
	return 0;
}

int stm32f_serial_dma_close(struct stm32f_serial_dma_drv * drv)
{
	DCC_LOG(LOG_TRACE, "...");
	return 0;
}

int stm32f_serial_dma_ioctl(struct stm32f_serial_dma_drv * drv, int opt, 
							uintptr_t arg1, uintptr_t arg2)
{
	struct stm32_usart * us = drv->uart;
	unsigned int msk = 0;

	DCC_LOG(LOG_TRACE, "...");

	switch (opt) {
	case SERIAL_IOCTL_ENABLE:
		DCC_LOG(LOG_TRACE, "SERIAL_IOCTL_ENABLE");
		msk |= (arg1 & SERIAL_RX_EN) ? USART_RE : 0;
		msk |= (arg1 & SERIAL_TX_EN) ? USART_TE : 0;
		us->cr1 |= msk;
		break;

	case SERIAL_IOCTL_DISABLE:
		DCC_LOG(LOG_TRACE, "SERIAL_IOCTL_DISABLE");
		msk |= (arg1 & SERIAL_RX_EN) ? USART_RE : 0;
		msk |= (arg1 & SERIAL_TX_EN) ? USART_TE : 0;
		us->cr1 &= ~msk;
		break;

	case SERIAL_IOCTL_DRAIN:
		DCC_LOG(LOG_TRACE, "SERIAL_IOCTL_DRAIN");
		return stm32f_serial_dma_drain(drv);

	case SERIAL_IOCTL_FLOWCTRL_SET: 
		switch (arg1) { 
		case SERIAL_FLOWCTRL_NONE:
		case SERIAL_FLOWCTRL_RTSCTS:
			break;
		case SERIAL_FLOWCTRL_XONXOFF:
			break;
		}
		break;

	case SERIAL_IOCTL_CONF_SET: 
		{
			struct serial_config * cfg = (struct serial_config *)arg1;
			uint32_t flags;
			DCC_LOG(LOG_TRACE, "SERIAL_IOCTL_CONF_SET");

			stm32_usart_baudrate_set(us, cfg->baudrate);
			flags = CFG_TO_FLAGS(cfg);
			stm32_usart_mode_set(us, flags);
		}
		break;

	case SERIAL_IOCTL_DMA_PREPARE: 
		return stm32f_serial_dma_prepare(drv, (void *)arg1, arg2);

	default:
		return -EINVAL;
	}

	return 0;
}

const struct serial_op stm32f_uart_serial_dma_op = {
	.send = (void *)stm32f_serial_dma_send,
	.recv = (void *)stm32f_serial_dma_recv,
	.drain = (void *)stm32f_serial_dma_drain,
	.close = (void *)stm32f_serial_dma_close,
	.ioctl = (void *)stm32f_serial_dma_ioctl
};

