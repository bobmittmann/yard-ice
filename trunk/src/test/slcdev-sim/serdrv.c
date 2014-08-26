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

/* 
   DMA Mapping 
   USART2_RX: Channel 6 
   USART2_TX: Channel 7 
*/

#define DMA_CHAN6 5
#define DMA_CHAN7 6

#define UART2_TX_DMA_CHAN DMA_CHAN7
#define UART2_RX_DMA_CHAN DMA_CHAN6

/* serial driver structure definition */
#define SERIAL_RX_BUF_LEN 16
#define SERIAL_TX_BUF_LEN 16

#ifndef SERDRV_DOUBLE_BUFFER
#define SERDRV_DOUBLE_BUFFER 0
#endif

struct serdrv {
#ifndef SERDRV_RX_FLAG 
	int8_t rx_flag;
#endif
#ifndef SERDRV_TX_FLAG 
	int8_t tx_flag;
#endif

	uint8_t rx_pos;
#if SERDRV_DOUBLE_BUFFER
	uint8_t rx_sel;
	uint8_t rx_dma;
	volatile uint8_t rx_cnt[2];
	uint8_t rx_buf[2][SERIAL_RX_BUF_LEN];
#else
	uint8_t rx_cnt;
	uint8_t rx_buf[1][SERIAL_RX_BUF_LEN];
#endif

#if SERDRV_DOUBLE_BUFFER
	uint8_t tx_sel;
	uint8_t tx_dma;
	volatile uint8_t tx_cnt[2];
	uint8_t tx_buf[2][SERIAL_TX_BUF_LEN];
#else
	uint8_t tx_buf[1][SERIAL_TX_BUF_LEN];
#endif
};

/* static serial driver object */
static struct serdrv serial2_dev;

#ifdef SERDRV_TX_FLAG 
#define TX_FLAG SERDRV_TX_FLAG
#else
#define TX_FLAG serial2_dev.tx_flag
#endif

#ifdef SERDRV_RX_FLAG 
#define RX_FLAG SERDRV_RX_FLAG
#else
#define RX_FLAG serial2_dev.rx_flag
#endif

/*********************************************
 * TX DMA ISR
 *********************************************/
void stm32_dma1_channel7_isr(void)
{
	struct stm32f_dma * dma = STM32_DMA1;
#if SERDRV_DOUBLE_BUFFER
	unsigned int sel;
	unsigned int cnt;
#endif

	if (dma->isr & DMA_TCIF7) {
		/* clear the DMA transfer complete flag */
		dma->ifcr = DMA_CTCIF7;
		/* stop DMA */
		dma->ch[UART2_TX_DMA_CHAN].ccr = 0;
		DCC_LOG(LOG_MSG, "DMA TX complete...");

#if SERDRV_DOUBLE_BUFFER
		sel = serial2_dev.tx_dma;
		serial2_dev.tx_cnt[sel] = 0;
		sel = 1 - sel;
		serial2_dev.tx_dma = sel;
		cnt = serial2_dev.tx_cnt[sel];
		dma->ch[UART2_TX_DMA_CHAN].cmar = (void *)&serial2_dev.tx_buf[sel];
		if (cnt > 0) {
			/* Configure and enable DMA */
			dma->ch[UART2_TX_DMA_CHAN].cndtr = cnt;
			dma->ch[UART2_TX_DMA_CHAN].ccr = DMA_MSIZE_8 | DMA_PSIZE_8 | 
				DMA_MINC | DMA_DIR_MTP | DMA_TCIE | DMA_TEIE | DMA_EN;
			DCC_LOG(LOG_INFO, "DMA TX restart...");
		} else 
#endif /* SERDRV_DOUBLE_BUFFER */
			__thinkos_flag_signal(TX_FLAG);
	}

	if (dma->isr & DMA_TEIF7) {
		/* clear the DMA error flag */
		dma->ifcr = DMA_TEIF7;
		DCC_LOG(LOG_WARNING, "DMA Error");
	}
}

static inline void rx_dma_xfer(struct stm32f_dma * dma, unsigned int cnt)
{
#if SERDRV_DOUBLE_BUFFER
	unsigned int sel;
#endif

	/* stop DMA */
	dma->ch[UART2_RX_DMA_CHAN].ccr = 0;

#if SERDRV_DOUBLE_BUFFER
	unsigned int sel;

	sel = serial2_dev.rx_dma;
	serial2_dev.rx_cnt[sel] = cnt; 
	sel = 1 - sel;
	/* Configure and enable DMA */
	dma->ch[UART2_RX_DMA_CHAN].cmar = (void *)&serial2_dev.rx_buf[sel];
	dma->ch[UART2_RX_DMA_CHAN].cndtr = SERIAL_RX_BUF_LEN;
	serial2_dev.rx_dma = sel;
	if (serial2_dev.rx_cnt[sel] == 0) {
		dma->ch[UART2_RX_DMA_CHAN].ccr = DMA_MSIZE_8 | DMA_PSIZE_8 | DMA_MINC |
			DMA_DIR_PTM | DMA_TCIE | DMA_TEIE | DMA_EN;
		DCC_LOG(LOG_INFO, "DMA RX restart...");
	}
#else  /* SERDRV_DOUBLE_BUFFER */
	serial2_dev.rx_cnt = cnt;
#endif /* SERDRV_DOUBLE_BUFFER */
	__thinkos_flag_signal(RX_FLAG);
}

/*********************************************
 * RX DMA ISR
 *********************************************/
void stm32_dma1_channel6_isr(void)
{
	struct stm32f_dma * dma = STM32_DMA1;

	if (dma->isr & DMA_TCIF6) {
		/* clear the DMA transfer complete flag */
		dma->ifcr = DMA_CTCIF6;
		DCC_LOG(LOG_INFO, "DMA RX complete...");
		rx_dma_xfer(dma, SERIAL_RX_BUF_LEN);
	}
	if (dma->isr & DMA_TEIF6) {
		/* clear the DMA error flag */
		dma->ifcr = DMA_TEIF6;
		DCC_LOG(LOG_WARNING, "DMA Error");
	}
}

/*********************************************
 * USART ISR
 *********************************************/
void stm32_usart2_isr(void)
{
	struct stm32_usart * uart = STM32_USART2;
	struct stm32f_dma * dma = STM32_DMA1;
	uint32_t sr;
	
	sr = uart->sr;
	DCC_LOG1(LOG_MSG, "sr=%04x", sr);

	if (sr & USART_IDLE) {
		unsigned int cnt;
		int c = uart->dr; /* clear IDLE flag */
		(void)c;

		DCC_LOG(LOG_INFO, "IDLE");

		cnt = SERIAL_RX_BUF_LEN - dma->ch[UART2_RX_DMA_CHAN].cndtr;
		if (cnt != 0)
			rx_dma_xfer(dma, cnt);

	} else {
		DCC_LOG1(LOG_MSG, "sr=%04x", sr);
	}
}

struct serdrv * serdrv_init(unsigned int speed)
{
	struct serdrv * drv  = &serial2_dev;
	struct stm32_usart * uart = STM32_USART2;
	struct stm32f_dma * dma = STM32_DMA1;

	drv->rx_pos = 0;
#if SERDRV_DOUBLE_BUFFER
	drv->rx_sel = 0;
	drv->rx_dma = 0;
	drv->rx_cnt[0] = 0;
	drv->rx_cnt[1] = 0;
#else
	drv->rx_cnt = 0;
#endif

#if SERDRV_DOUBLE_BUFFER
	drv->tx_sel = 0;
	drv->tx_dma = 0;
	drv->tx_cnt[0] = 0;
	drv->tx_cnt[1] = 0;
#endif

	/* alloc kernel objects */
#ifndef SERDRV_RX_FLAG 
	drv->rx_flag = thinkos_flag_alloc();
#endif
#ifndef SERDRV_TX_FLAG 
	drv->tx_flag = thinkos_flag_alloc();
#endif

#if !SERDRV_DOUBLE_BUFFER
	/* signal the TX flag to indicate the 
	   transmitter is ready to start transmitting */
	thinkos_flag_set(TX_FLAG);
#endif /* SERDRV_DOUBLE_BUFFER */

	DCC_LOG1(LOG_MSG, "speed=%d", speed);

	/* clock enable */
	stm32_clk_enable(STM32_RCC, STM32_CLK_DMA1);
	stm32_clk_enable(STM32_RCC, STM32_CLK_USART2);

	/*********************************************
	 * TX DMA configuration
	 *********************************************/

	/* DMA Disable */
	dma->ch[UART2_TX_DMA_CHAN].ccr = 0;
	/* Wait for the channel to be ready .. */
	while (dma->ch[UART2_TX_DMA_CHAN].ccr & DMA_EN);

	/*  DMA Configuration */
	/* Peripheral address */
	dma->ch[UART2_TX_DMA_CHAN].cpar = &uart->dr;
	/* Memory pointer */
	dma->ch[UART2_TX_DMA_CHAN].cmar = (void *)&drv->tx_buf[0];

	/*********************************************
	 * RX DMA configuration
	 *********************************************/
	/* DMA Disable */
	dma->ch[UART2_RX_DMA_CHAN].ccr = 0;
	/* Wait for the channel to be ready .. */
	while (dma->ch[UART2_RX_DMA_CHAN].ccr & DMA_EN);

	/*  DMA Configuration */
	/* Peripheral address */
	dma->ch[UART2_RX_DMA_CHAN].cpar = &uart->dr;
	/* Memory pointer */
	dma->ch[UART2_RX_DMA_CHAN].cmar = (void *)&drv->rx_buf[0];
	/* Number of data items to transfer */
	dma->ch[UART2_RX_DMA_CHAN].cndtr = SERIAL_RX_BUF_LEN;
	/* Configure and enable DMA */
	dma->ch[UART2_RX_DMA_CHAN].ccr = DMA_MSIZE_8 | DMA_PSIZE_8 | DMA_MINC |
		DMA_DIR_PTM | DMA_TCIE | DMA_TEIE | DMA_EN;


	/*********************************************
	 * USART 
	 *********************************************/
	stm32_usart_init(uart);
	stm32_usart_baudrate_set(uart, speed);
	stm32_usart_mode_set(uart, SERIAL_8N1);

	/* Enable DMA for transmission and reception */
	uart->cr3 |= USART_DMAT | USART_DMAR;
	/* enable idle line interrupt */
	uart->cr1 |= USART_IDLEIE;

	/* enable UART */
	stm32_usart_enable(uart);

	/* configure interrupts */
	cm3_irq_pri_set(STM32_IRQ_DMA1_CHANNEL6, IRQ_PRIORITY_LOW);
	cm3_irq_pri_set(STM32_IRQ_DMA1_CHANNEL7, IRQ_PRIORITY_LOW);
	cm3_irq_pri_set(STM32_IRQ_USART2, IRQ_PRIORITY_LOW);

	/* enable interrupts */
	cm3_irq_enable(STM32_IRQ_DMA1_CHANNEL6);
	cm3_irq_enable(STM32_IRQ_DMA1_CHANNEL7);
	cm3_irq_enable(STM32_IRQ_USART2);

	return drv;
}

int serdrv_send(struct serdrv * drv, const void * buf, int len)
{
	struct stm32f_dma * dma = STM32_DMA1;
	uint8_t * src = (uint8_t *)buf;
	unsigned int rem = len;
	unsigned int n;
	int i;
#if SERDRV_DOUBLE_BUFFER
	unsigned int sel = serial2_dev.tx_sel;
#endif

	while (rem) { 
#if SERDRV_DOUBLE_BUFFER
		while (serial2_dev.tx_cnt[sel] != 0) {
			DCC_LOG(LOG_MSG, "TX wait...");
			/* wait for pending DMA xfer to complete */
			thinkos_flag_wait(TX_FLAG);
			thinkos_flag_clr(TX_FLAG); /* clear idle flag */
		}

		n = MIN(rem, SERIAL_TX_BUF_LEN);

		DCC_LOG2(LOG_INFO, "sel=%d cnt=%d", sel, n);

		/* Copy data to buffer */
		for (i = 0; i < n; ++i)
			drv->tx_buf[sel][i] = src[i];

		serial2_dev.tx_cnt[sel] = n;
		sel = 1 - sel;
		if ((dma->ch[UART2_TX_DMA_CHAN].ccr & DMA_EN) == 0) {
			/* Number of data items to transfer */
			dma->ch[UART2_TX_DMA_CHAN].cndtr = n;
			/* Configure and enable DMA */
			dma->ch[UART2_TX_DMA_CHAN].ccr = DMA_MSIZE_8 | DMA_PSIZE_8 | 
				DMA_MINC | DMA_DIR_MTP | DMA_TCIE | DMA_TEIE | DMA_EN;
			DCC_LOG(LOG_INFO, "TX DMA start...");
		}
#else /* SERDRV_DOUBLE_BUFFER */
		thinkos_flag_wait(TX_FLAG);
		thinkos_flag_clr(TX_FLAG);

		/* Copy data to buffer */
		n = MIN(rem, SERIAL_TX_BUF_LEN);
		for (i = 0; i < n; ++i)
			drv->tx_buf[0][i] = src[i];

		/* Configure and enable DMA */
		dma->ch[UART2_TX_DMA_CHAN].cndtr = n;
		dma->ch[UART2_TX_DMA_CHAN].ccr = DMA_MSIZE_8 | DMA_PSIZE_8 | 
			DMA_MINC | DMA_DIR_MTP | DMA_TCIE | DMA_TEIE | DMA_EN;
#endif /* SERDRV_DOUBLE_BUFFER */
		rem -= n;
		src += n;
	}

#if SERDRV_DOUBLE_BUFFER
	serial2_dev.tx_sel = sel;
#endif

	return len;
}

int serdrv_recv(struct serdrv * drv, void * buf, int len, unsigned int tmo)
{
//	struct stm32f_dma * dma = STM32_DMA1;
	uint8_t * dst = (uint8_t *)buf;
	uint8_t * src;
	unsigned int rem;
	int ret;
	int n;
	int i;

#if SERDRV_DOUBLE_BUFFER
	unsigned int sel;
	unsigned int cnt;

	sel = drv->rx_sel;
	cnt = drv->rx_cnt[sel];

	DCC_LOG3(LOG_INFO, "sel=%d pos=%d cnt=%d", sel, drv->rx_pos, cnt);

	while ((rem = cnt - drv->rx_pos) == 0) {
		/* wait for DMA xfer to complete */
		DCC_LOG(LOG_MSG, "RX wait...");
		if ((ret = thinkos_flag_timedwait(RX_FLAG, tmo)) < 0) {
			DCC_LOG(LOG_MSG, "Timeout!");
			return ret;
		}
		thinkos_flag_clr(RX_FLAG); /* clear flag */
		cnt = drv->rx_cnt[sel];
	}

	DCC_LOG3(LOG_INFO, "sel=%d pos=%d rem=%d", 
			 drv->rx_sel, drv->rx_pos, rem);

	n = MIN(rem, len);
	src = &drv->rx_buf[sel][drv->rx_pos];

	/* Copy data to buffer */
	for (i = 0; i < n; ++i)
		dst[i] = src[i];

	drv->rx_pos += n;
	if (drv->rx_pos == cnt) {
		struct stm32f_dma * dma = STM32_DMA1;

		drv->rx_pos = 0;
		drv->rx_cnt[sel] = 0;
		drv->rx_sel = 1 - sel;
		if ((dma->ch[UART2_RX_DMA_CHAN].ccr & DMA_EN) == 0) {
			dma->ch[UART2_RX_DMA_CHAN].ccr = DMA_MSIZE_8 | DMA_PSIZE_8 | 
				DMA_MINC | DMA_DIR_PTM | DMA_TCIE | DMA_TEIE | DMA_EN;
			DCC_LOG(LOG_MSG, "RX DMA restart...");
		}
	}

#else /* SERDRV_DOUBLE_BUFFER */

	/* wait for DMA xfer to complete */
	DCC_LOG(LOG_MSG, "RX wait...");
	if ((ret = thinkos_flag_timedwait(RX_FLAG, tmo)) < 0) {
		DCC_LOG(LOG_MSG, "Timeout!");
		return ret;
	}

	rem = drv->rx_cnt - drv->rx_pos;

	n = MIN(rem, len);
	src = &drv->rx_buf[0][drv->rx_pos];

	/* Copy data to buffer */
	for (i = 0; i < n; ++i)
		dst[i] = src[i];

	drv->rx_pos += n;

	if (drv->rx_pos == drv->rx_cnt) {
		struct stm32f_dma * dma = STM32_DMA1;
		drv->rx_pos = 0;
		drv->rx_cnt = 0;
		thinkos_flag_clr(RX_FLAG); /* clear flag */
		dma->ch[UART2_RX_DMA_CHAN].cndtr = SERIAL_RX_BUF_LEN;
		dma->ch[UART2_RX_DMA_CHAN].ccr = DMA_MSIZE_8 | DMA_PSIZE_8 | 
			DMA_MINC | DMA_DIR_PTM | DMA_TCIE | DMA_TEIE | DMA_EN;
		DCC_LOG(LOG_MSG, "RX DMA restart...");
	}


#endif /* SERDRV_DOUBLE_BUFFER */

	return n;
}

void serdrv_flush(struct serdrv * drv)
{
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

