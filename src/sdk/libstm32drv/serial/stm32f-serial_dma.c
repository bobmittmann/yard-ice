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
	struct stm32_usart * uart = drv->uart;
	uint32_t cr;
	uint32_t sr;
	int c;

	cr = uart->cr1;
	sr = uart->sr & (cr | USART_ORE | USART_LBD);

	if (sr & USART_TC) {
		DCC_LOG1(LOG_MSG, "UART%d TC.", stm32_usart_lookup(uart) + 1);
		/* TC interrupt is cleared by writing 0 back to the SR register */
		uart->sr = sr & ~USART_TC;
		/* disable the transfer complete interrupt */
		cr &= ~USART_TCIE;
		if (cr & USART_IDLEIE) {
			/* Pulse TE to generate an IDLE Frame */
			uart->cr1 = cr & ~USART_TE;
			uart->cr1 = cr | USART_TE;
		} else {
			/* Generate a brake condition */
			uart->cr1 = cr | USART_SBK;
			thinkos_flag_give_i(drv->tx_done);
		}
	}

	/* break detection */
	if (sr & USART_LBD) {
		/* clear the break detection interrupt flag */
		uart->sr = sr & ~(USART_ORE | USART_LBD);
		/* disable DMA stream */
		drv->rx.dmactl.strm->cr &= ~(DMA_TCIE | DMA_EN);
		/* skip the break char */
		drv->rx.dmactl.strm->ndtr++;
		DCC_LOG(LOG_INFO, "BRK");
#if SERIAL_ENABLE_STATS
		drv->stats.rx_brk++;
#endif
		/* signal the waiting thread */
		thinkos_flag_give_i(drv->rx_idle);
	}

	/* idle detection */
	if (sr & USART_IDLE) {
		c = uart->rdr;
		(void)c;
		/* Disable DMA stream */
		drv->rx.dmactl.strm->cr &= ~(DMA_TCIE | DMA_EN);
//		while (drv->rx.dmactl.strm->cr & DMA_EN);
		DCC_LOG(LOG_TRACE, "IDLE");
#if SERIAL_ENABLE_STATS
		drv->stats.rx_idle++;
#endif
		thinkos_flag_give_i(drv->rx_idle);
	}

	if (sr & USART_ORE) {
		c = uart->rdr;
		(void)c;
		DCC_LOG(LOG_WARNING, "OVR!");
#if SERIAL_ENABLE_STATS
		drv->stats.err_ovr++;
#endif

	}
}

void stm32f_serial_dma_rx_isr(struct stm32f_serial_dma_drv * drv)
{
	if (drv->rx.dmactl.isr[TCIF_BIT]) {
#if SERIAL_ENABLE_STATS
		drv->stats.rx_dmatc++;
#endif
		DCC_LOG(LOG_TRACE, "TCIF");
		/* clear the RX DMA transfer complete flag */
		drv->rx.dmactl.ifcr[TCIF_BIT] = 1;
/*	XXX: slcdev debug.
 * thinkos_flag_give_i(drv->rx_idle); */
	}
	if (drv->rx.dmactl.isr[TEIF_BIT]) {
#if SERIAL_ENABLE_STATS
		drv->stats.rx_dmate++;
#endif
		DCC_LOG(LOG_TRACE, "TEIF");
		/* FIXME: DMA transfer error handling... */
		drv->rx.dmactl.ifcr[TEIF_BIT] = 1;
//		thinkos_flag_give_i(drv->rx_idle);
		/* Disable DMA stream */
		drv->rx.dmactl.strm->cr &= ~DMA_EN;
	}
}

int stm32f_serial_dma_recv(struct stm32f_serial_dma_drv * drv, 
						   void * buf, unsigned int len, unsigned int tmo)
{
	unsigned int ndtr;
	unsigned int cnt;
	uint32_t cr;
	int ret;

	DCC_LOG3(LOG_INFO, "%6d: len=%d tmo=%d", thinkos_clock(), len, tmo);

	/* if the stored buffer pointer is NULL it means that
	   we did not yet prepare other DMA for a transfer,
	   in this case prepare to transfer using the provided 
	   buffer. */
	if (drv->rx.buf_ptr == NULL) {
		DCC_LOG(LOG_MSG, "DMA not prepared...");
		if ((cr = drv->rx.dmactl.strm->cr) & DMA_EN) {
			DCC_LOG(LOG_ERROR, "DMA enabled");
#if 0
			return -11;
#endif
			/* Disable DMA stream */
			drv->rx.dmactl.strm->cr = 0;
			/* Wait for the channel to be ready .. */
			while (drv->rx.dmactl.strm->cr & DMA_EN);
		}

		/* clear transfer complete interrupt flags */
		drv->rx.dmactl.ifcr[TCIF_BIT] = 1; 
		/* Set memory address */
		drv->rx.dmactl.strm->m0ar = buf;
		/* Number of data items to transfer */
		drv->rx.dmactl.strm->ndtr = len;
		/* enable DMA */
		drv->rx.dmactl.strm->cr = cr | DMA_TCIE | DMA_EN;
		ndtr = len;
	} else {
		DCC_LOG(LOG_MSG, "DMA prepared...");
		ndtr = drv->rx.buf_len;
	}

	/* wait for the end of transfer */
//	DCC_LOG1(LOG_TRACE, "thinkos_flag_timedtake(%d)...", tmo);
	if ((ret = thinkos_flag_timedtake(drv->rx_idle, tmo)) < 0) {
		/* if the initially provided 'ndtr' differs from the
		   DMA stream then the transfer already started,
		   in this case we wait until it finishes. */
		if ((cnt = ndtr - drv->rx.dmactl.strm->ndtr) < 2) {
			if (drv->rx.buf_ptr == NULL) {
				/* Disable DMA stream */
				drv->rx.dmactl.strm->cr &= ~(DMA_TCIE | DMA_EN);
			}
			DCC_LOG(LOG_INFO, "timeout...");
			return ret;
		} else {
			DCC_LOG1(LOG_WARNING, "thinkos_flag_take() cnt=%d  ...", cnt);
			thinkos_flag_take(drv->rx_idle);
		}
	}

	/* Number of data items transfered... */
	cnt = ndtr - drv->rx.dmactl.strm->ndtr;

	if (drv->rx.buf_ptr != NULL) {
		/* prepare next transfer */
		drv->rx.buf_ptr = buf;
		drv->rx.buf_len = len;
		if (buf != NULL) {
			/* prepare next transfer */
			/* clear transfer complete interrupt flags */
			drv->rx.dmactl.ifcr[TCIF_BIT] = 1; 
			/* Memory address */
			drv->rx.dmactl.strm->m0ar = (void *)buf;
			/* Number of data items to transfer */
			drv->rx.dmactl.strm->ndtr = len;
			/* enable DMA */
			drv->rx.dmactl.strm->cr |= DMA_TCIE | DMA_EN;
		}
	}

	if (cnt == 0)
		DCC_LOG1(LOG_WARNING, "%6d: DMA XFR cnt == 0!!!", thinkos_clock_i());
	else
		DCC_LOG2(LOG_INFO, "%6d: cnt=%d", thinkos_clock_i(), cnt);

	return cnt;
}

void stm32f_serial_dma_tx_isr(struct stm32f_serial_dma_drv * drv)
{
	if (drv->tx.dmactl.isr[TCIF_BIT]) {
		DCC_LOG(LOG_MSG, "TCIF");
		/* clear the TX DMA transfer complete flag */
		drv->tx.dmactl.ifcr[TCIF_BIT] = 1;
	}
	if (drv->tx.dmactl.isr[TEIF_BIT]) {
		DCC_LOG(LOG_MSG, "TEIF");
		/* FIXME: DMA transfer error handling... */
		drv->tx.dmactl.ifcr[TEIF_BIT] = 1;
		/* Disable DMA stream */
		drv->tx.dmactl.strm->cr &= ~DMA_EN;
	}
}

int stm32f_serial_dma_prepare(struct stm32f_serial_dma_drv * drv, 
							  void * buf, unsigned int len)
{
	if (drv->rx.dmactl.strm->cr & DMA_EN) {
		DCC_LOG(LOG_ERROR, "DMA enabled");
		return -1;
	}
	DCC_LOG1(LOG_TRACE, "len=%d", len);
	drv->rx.buf_ptr = buf;
	drv->rx.buf_len = len;
	/* clear transfer complete interrupt flags */
	drv->rx.dmactl.ifcr[TCIF_BIT] = 1; 
	/* Memory address */
	drv->rx.dmactl.strm->m0ar = buf;
	/* Number of data items to transfer */
	drv->rx.dmactl.strm->ndtr = len;
	/* enable DMA */
	drv->rx.dmactl.strm->cr |= DMA_EN;

	return 0;
}

#if 0
int stm32f_serial_dma_recv(struct stm32f_serial_dma_drv * drv, 
						   void * buf, unsigned int len, unsigned int tmo)
{
	unsigned int ndtr;
	unsigned int cnt;
	uint32_t cr;
	int ret;

	DCC_LOG3(LOG_INFO, "%6d: len=%d tmo=%d", thinkos_clock(), len, tmo);

	/* if the stored buffer pointer is NULL it means that
	   we did not yet prepare other DMA for a transfer,
	   in this case prepare to transfer using the provided 
	   buffer. */
	if (drv->rx.buf_ptr == NULL) {
		DCC_LOG(LOG_MSG, "DMA not prepared...");
		if ((cr = drv->rx.dmactl.strm->cr) & DMA_EN) {
			DCC_LOG(LOG_ERROR, "DMA enabled");
			return -11;
		}
		/* clear transfer complete interrupt flags */
		drv->rx.dmactl.ifcr[TCIF_BIT] = 1; 
		/* Set memory address */
		drv->rx.dmactl.strm->m0ar = buf;
		/* Number of data items to transfer */
		drv->rx.dmactl.strm->ndtr = len;
		/* enable DMA */
		drv->rx.dmactl.strm->cr = cr | DMA_TCIE | DMA_EN;
		ndtr = len;
	} else {
		DCC_LOG(LOG_INFO, "DMA prepared...");
		ndtr = drv->rx.buf_len;
	}

	/* wait for the end of transfer */
//	DCC_LOG1(LOG_TRACE, "thinkos_flag_timedtake(%d)...", tmo);
	if ((ret = thinkos_flag_timedtake(drv->rx_idle, tmo)) < 0) {
		/* if the initially provided 'ndtr' differs from the
		   DMA stream then the transfer already started,
		   in this case we wait until it finishes. */
		if ((cnt = ndtr - drv->rx.dmactl.strm->ndtr) < 2) {
			if (drv->rx.buf_ptr == NULL) {
				/* Disable DMA stream */
				drv->rx.dmactl.strm->cr &= ~(DMA_TCIE | DMA_EN);
			}
			DCC_LOG(LOG_INFO, "timeout...");
			return ret;
		} else {
			DCC_LOG1(LOG_WARNING, "thinkos_flag_take() cnt=%d  ...", cnt);
			thinkos_flag_take(drv->rx_idle);
		}
	}

	/* Disable DMA stream */
	drv->rx.dmactl.strm->cr &= ~(DMA_TCIE | DMA_EN);
	/* wait for completion of DMA transfer */
	while (drv->rx.dmactl.strm->cr & DMA_EN); /* Wait for the channel to be ready .. */

	/* Number of data items transfered... */
	cnt = ndtr - drv->rx.dmactl.strm->ndtr;
	if (cnt == 0) {
		DCC_LOG(LOG_WARNING, "DMA XFR cnt == 0!!!");
//		abort();
	}

	if (drv->rx.buf_ptr != NULL) {
		/* prepare next transfer */
		drv->rx.buf_ptr = buf;
		drv->rx.buf_len = len;
		if (buf != NULL) {
			/* prepare next transfer */
			/* clear transfer complete interrupt flags */
			drv->rx.dmactl.ifcr[TCIF_BIT] = 1; 
			/* Memory address */
			drv->rx.dmactl.strm->m0ar = (void *)buf;
			/* Number of data items to transfer */
			drv->rx.dmactl.strm->ndtr = len;
			/* enable DMA */
			drv->rx.dmactl.strm->cr |= DMA_TCIE | DMA_EN;
		}
	}

	DCC_LOG2(LOG_INFO, "%6d: cnt=%d", thinkos_clock_i(), cnt);
	return cnt;
}
#endif

int stm32f_serial_dma_send(struct stm32f_serial_dma_drv * drv, 
						   const void * buf, unsigned int len)
{
//	struct stm32_usart * uart = drv->uart;
	uint32_t ccr;
	unsigned int cnt;

	DCC_LOG2(LOG_MSG, "UART%d len=%d", 
			 stm32_usart_lookup(drv->uart) + 1, len);

	if ((ccr = drv->tx.dmactl.strm->cr) & DMA_EN) {
		DCC_LOG(LOG_ERROR, "DMA enabled");
		abort();
	}

	/* Disable DMA */
//	while ((ccr = drv->tx.dmactl.strm->cr) & DMA_EN)
//		drv->tx.dmactl.strm->cr = ccr & ~DMA_EN; 

	/* Memory address */
	drv->tx.dmactl.strm->m0ar = (void *)buf;
	/* Number of data items to transfer */
	drv->tx.dmactl.strm->ndtr = len;
	/* clear the UART transfer complete bit */
	drv->uart->sr &= ~USART_TC;
	/* enable DMA */
	drv->tx.dmactl.strm->cr = ccr | DMA_EN;
	/* enable the transfer complete interrupt */
	drv->uart->cr1 |= USART_TCIE;
	/* wait for the transfer to complete */
	thinkos_flag_take(drv->tx_done);

	/* Number of data items transfered... */
	if ((cnt = drv->tx.dmactl.strm->ndtr) != 0) {
		DCC_LOG(LOG_ERROR, "DMA error!");
		abort();
	}

	/* clear the the DMA stream transfer complete flag */
	drv->tx.dmactl.ifcr[TCIF_BIT] = 1;

	return len - cnt;
}


int stm32f_serial_dma_init(struct stm32f_serial_dma_drv * drv, 
						   unsigned int baudrate, unsigned int flags,
						   int dma_chan_id)
{
	drv->rx_idle = thinkos_flag_alloc();
	drv->tx_done = thinkos_flag_alloc();
	DCC_LOG2(LOG_TRACE, "rx_idle=%d tx_done=%d", drv->rx_idle, 
			 drv->tx_done);

#if SERIAL_TX_MUTEX
	drv->tx_mutex = thinkos_mutex_alloc(); 
	DCC_LOG1(LOG_TRACE, "tx_mutex=%d", drv->tx_mutex);
#endif

	/* Enable DMA for transmission and reception */
	stm32_usart_init(drv->uart);
	stm32_usart_baudrate_set(drv->uart, baudrate);
	stm32_usart_mode_set(drv->uart, flags);

	drv->rx.buf_ptr = NULL;
	drv->rx.buf_len = 0;

	/* -------------------------------------------------------
	   Configure TX DMA stream
	 */
	/* Disable DMA stream */
	drv->tx.dmactl.strm->cr = 0;
	/* Wait for the channel to be ready .. */
	while (drv->tx.dmactl.strm->cr & DMA_EN); 
	drv->tx.dmactl.strm->par = &drv->uart->tdr;
//	drv->tx.dmactl.strm->fcr = DMA_FEIE | DMA_DMDIS | DMA_FTH_FULL;
	/* configure TX DMA stream */
	drv->tx.dmactl.strm->cr = DMA_CHSEL_SET(dma_chan_id) | 
		DMA_MSIZE_8 | DMA_PSIZE_8 | DMA_MINC | 
		DMA_DIR_MTP | DMA_TCIE | DMA_TEIE;
	/* ------------------------------------------------------- 
	 */

	/* -------------------------------------------------------
	   Configure RX DMA stream
	 */
	/* Disable DMA stream */
	drv->rx.dmactl.strm->cr = 0;
	/* Wait for the channel to be ready .. */
	while (drv->rx.dmactl.strm->cr & DMA_EN); 
	/* peripheral address */
	drv->rx.dmactl.strm->par = &drv->uart->rdr;
	/* cofigure FIFO */
//	drv->rx.dmactl.strm->fcr = DMA_FEIE | DMA_DMDIS | DMA_FTH_FULL;
	/* configure DMA */
	drv->rx.dmactl.strm->cr = DMA_CHSEL_SET(dma_chan_id) | 
		DMA_MSIZE_8 | DMA_PSIZE_8 | DMA_MINC |
		DMA_DIR_PTM | DMA_TCIE | DMA_TEIE;
	/* ------------------------------------------------------- 
	 */

	/* Clear pending interrupts */
	drv->uart->sr = 0;

	/* configure the UART for DMA transfer */
	drv->uart->cr3 |= USART_DMAT | USART_DMAR;

	if (flags & SERIAL_EOT_BREAK) {
		/* line break detection */
		drv->uart->cr2 |= USART_LBDIE | USART_LINEN;
	} else {
		/* enable RX IDLE interrupt */
		drv->uart->cr1 |= USART_IDLEIE;
	}	

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
	struct stm32_usart * uart = drv->uart;
	unsigned int msk = 0;

	DCC_LOG(LOG_TRACE, "...");

	switch (opt) {
	case SERIAL_IOCTL_ENABLE:
		DCC_LOG(LOG_TRACE, "SERIAL_IOCTL_ENABLE");
		msk |= (arg1 & SERIAL_RX_EN) ? USART_RE : 0;
		msk |= (arg1 & SERIAL_TX_EN) ? USART_TE : 0;
		uart->cr1 |= msk;
		break;

	case SERIAL_IOCTL_DISABLE:
		DCC_LOG(LOG_TRACE, "SERIAL_IOCTL_DISABLE");
		msk |= (arg1 & SERIAL_RX_EN) ? USART_RE : 0;
		msk |= (arg1 & SERIAL_TX_EN) ? USART_TE : 0;
		uart->cr1 &= ~msk;
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

			stm32_usart_baudrate_set(uart, cfg->baudrate);
			flags = CFG_TO_FLAGS(cfg);
			stm32_usart_mode_set(uart, flags);
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

