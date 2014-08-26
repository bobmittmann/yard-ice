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

struct serdrv {
#ifndef SERDRV_RX_FLAG 
	int8_t rx_flag;
#endif
#ifndef SERDRV_TX_FLAG 
	int8_t tx_flag;
#endif
	uint8_t rx_pos;
	uint8_t rx_sel;
	uint8_t rx_dma;
	volatile uint8_t rx_cnt[2];
	uint8_t rx_buf[2][SERIAL_RX_BUF_LEN];

	uint8_t tx_sel;
	uint8_t tx_dma;
	volatile uint8_t tx_cnt[2];
	uint8_t tx_buf[2][SERIAL_TX_BUF_LEN];
};

/* static serial driver object */
static struct serdrv serial2_dev;


/*********************************************
 * TX DMA ISR
 *********************************************/
void stm32_dma1_channel7_isr(void)
{
	struct stm32f_dma * dma = STM32_DMA1;
	unsigned int sel;
	unsigned int cnt;

	if (dma->isr & DMA_TCIF7) {
		/* clear the DMA transfer complete flag */
		dma->ifcr = DMA_CTCIF7;
		/* stop DMA */
		dma->ch[UART2_TX_DMA_CHAN].ccr = 0;
		DCC_LOG(LOG_TRACE, "DMA TX complete...");

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
			DCC_LOG(LOG_TRACE, "DMA TX restart...");
		} else {

#ifdef SERDRV_TX_FLAG 
			__thinkos_flag_signal(SERDRV_TX_FLAG);
#else
			__thinkos_flag_signal(serial2_dev.tx_flag);
#endif

		}
	}

	if (dma->isr & DMA_TEIF7) {
		/* clear the DMA error flag */
		dma->ifcr = DMA_TEIF7;
		DCC_LOG(LOG_WARNING, "DMA Error");
	}
}

static inline void rx_dma_xfer(struct stm32f_dma * dma, unsigned int cnt)
{
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
		DCC_LOG(LOG_TRACE, "DMA RX restart...");
	}

#ifdef SERDRV_RX_FLAG 
	__thinkos_flag_signal(SERDRV_RX_FLAG);
#else
	__thinkos_flag_signal(serial2_dev.rx_flag);
#endif

#if 0
		head = serial2_dev.rx_head;
		cnt = serial2_dev.rx_size - dma->ch[UART2_RX_DMA_CHAN].cndtr;
		head += cnt;
		head &= (SERIAL_RX_BUF_LEN - 1);
		serial2_dev.rx_head = head;
		cnt = SERIAL_RX_BUF_LEN - head;
		if (cnt > (SERIAL_RX_BUF_LEN / 2))
			cnt = SERIAL_RX_BUF_LEN / 2;
// cnt = (cnt + 1) / 2;
		serial2_dev.rx_size = cnt;

		/* Configure and enable DMA */
		dma->ch[UART2_RX_DMA_CHAN].cmar = (void *)&serial2_dev.rx_buf[head];
		dma->ch[UART2_RX_DMA_CHAN].cndtr = cnt;
		dma->ch[UART2_RX_DMA_CHAN].ccr = DMA_MSIZE_8 | DMA_PSIZE_8 | DMA_MINC |
			DMA_DIR_PTM | DMA_TCIE | DMA_TEIE | DMA_EN;
#endif

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

		DCC_LOG(LOG_TRACE, "DMA RX complete...");

		/* stop DMA */
		dma->ch[UART2_RX_DMA_CHAN].ccr = 0;

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
	unsigned int cnt;
	uint32_t sr;
	int c;
	
	sr = uart->sr;
	DCC_LOG1(LOG_MSG, "sr=%04x", sr);

	if (sr & USART_IDLE) {
		/* clear IDLE flag */
		c = uart->dr;
		(void)c;

		DCC_LOG(LOG_TRACE, "IDLE");

		cnt = SERIAL_RX_BUF_LEN - dma->ch[UART2_RX_DMA_CHAN].cndtr;

		if (cnt != 0) {
			/* stop DMA */
			dma->ch[UART2_RX_DMA_CHAN].ccr = 0;
			rx_dma_xfer(dma, cnt);
		}

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
	drv->rx_sel = 0;
	drv->rx_dma = 0;
	drv->rx_cnt[0] = 0;
	drv->rx_cnt[1] = 0;

	drv->tx_sel = 0;
	drv->tx_dma = 0;
	drv->tx_cnt[0] = 0;
	drv->tx_cnt[1] = 0;

	/* alloc kernel objects */
#ifndef SERDRV_RX_FLAG 
	drv->rx_flag = thinkos_flag_alloc();
#endif
#ifndef SERDRV_TX_FLAG 
	drv->tx_flag = thinkos_flag_alloc();
#endif
	/* signal the TX flag, indicates the 
	   transmitter is ready to start transmitting */
#ifdef SERDRV_TX_FLAG 
//	thinkos_flag_set(SERDRV_TX_FLAG);
#else
//	thinkos_flag_set(drv->tx_flag);
#endif

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
	unsigned int sel;
	unsigned int n;
	int i;

	sel = serial2_dev.tx_sel;

	while (rem) { 
		while (serial2_dev.tx_cnt[sel] != 0) {
			DCC_LOG(LOG_MSG, "TX wait...");
#ifdef SERDRV_TX_FLAG 
			/* wait for pending DMA xfer to complete */
			thinkos_flag_wait(SERDRV_TX_FLAG);
			/* clear idle flag */
			thinkos_flag_clr(SERDRV_TX_FLAG);
#else
			thinkos_flag_wait(drv->tx_flag);
			thinkos_flag_clr(drv->tx_flag);
#endif
			DCC_LOG(LOG_TRACE, "TX...");
		}

		n = MIN(rem, SERIAL_TX_BUF_LEN);

		DCC_LOG2(LOG_TRACE, "sel=%d cnt=%d", sel, n);

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
			DCC_LOG(LOG_TRACE, "TX DMA start...");
		}

		DCC_LOG1(LOG_MSG, "n=%d", n);
		rem -= n;
		src += n;
	}

	serial2_dev.tx_sel = sel;

	return len;
}

int serdrv_recv(struct serdrv * drv, void * buf, int len, unsigned int tmo)
{
//	struct stm32f_dma * dma = STM32_DMA1;
	uint8_t * dst = (uint8_t *)buf;
	uint8_t * src;
	unsigned int sel;
	unsigned int cnt;
	unsigned int rem;
	int ret;
	int n;
	int i;


	sel = drv->rx_sel;
	cnt = drv->rx_cnt[sel];

	DCC_LOG3(LOG_TRACE, "sel=%d pos=%d cnt=%d", sel, drv->rx_pos, cnt);

	/* Number of data items transfered... */
	while ((rem = cnt - drv->rx_pos) == 0) {

		/* wait for DMA xfer to complete */
		DCC_LOG(LOG_MSG, "RX wait...");
#ifdef SERDRV_RX_FLAG 
		if ((ret = thinkos_flag_timedwait(SERDRV_RX_FLAG, tmo)) < 0) {
#else 
		if ((ret = thinkos_flag_timedwait(drv->rx_flag, tmo)) < 0) {
#endif
			DCC_LOG(LOG_MSG, "Timeout!");
			return ret;
		}
		/* clear flag */
#ifdef SERDRV_RX_FLAG 
		thinkos_flag_clr(SERDRV_RX_FLAG);
#else
		thinkos_flag_clr(drv->rx_flag);
#endif
		cnt = drv->rx_cnt[sel];
	}

//	rem = (rem + SERIAL_RX_BUF_LEN) & (SERIAL_RX_BUF_LEN - 1);
//	if (rem < 0)
//		rem += SERIAL_RX_BUF_LEN;

	DCC_LOG3(LOG_TRACE, "sel=%d pos=%d rem=%d", 
			 drv->rx_sel, drv->rx_pos, rem);

	n = MIN(rem, len);

	src = &drv->rx_buf[sel][drv->rx_pos];

	/* Copy data to buffer */
	for (i = 0; i < n; ++i)
		dst[i] = src[i];

//	rem -= n;

//	if (rem == 0) {
		/* reset buffer position index */
//		drv->rx_tail = 0;
		/* Number of data items to transfer */
//		dma->ch[UART2_RX_DMA_CHAN].cndtr = SERIAL_RX_BUF_LEN;
		/* Configure and enable DMA */
//		dma->ch[UART2_RX_DMA_CHAN].ccr = DMA_MSIZE_8 | DMA_PSIZE_8 | DMA_MINC |
//			DMA_DIR_PTM | DMA_TCIE | DMA_TEIE | DMA_EN;

//		DCC_LOG(LOG_TRACE, "DMA RX enabled.");
//	}

	drv->rx_pos += n;
//	drv->rx_tail &= (SERIAL_RX_BUF_LEN - 1);
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

	return n;
}


/* ----------------------------------------------------------------------
 * Serial driver file operations 
 * ----------------------------------------------------------------------
 */

#include <sys/tty.h>

const struct fileop serdrv_ops = {
	.write = (void *)serdrv_send,
	.read = (void *)serdrv_recv,
	.flush = (void *)NULL,
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

