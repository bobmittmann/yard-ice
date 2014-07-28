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

#define __THINKOS_IRQ__
#include <thinkos_irq.h>
#include <thinkos.h>

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
	int8_t rx_flag;
	int8_t tx_flag;
	int8_t rx_pos;
	uint8_t tx_buf[SERIAL_TX_BUF_LEN];
	uint8_t rx_buf[SERIAL_RX_BUF_LEN];
};

/* static serial driver object */
static struct serdrv serial2_dev;

/*********************************************
 * RX DMA ISR
 *********************************************/
void stm32_dma1_channel6_isr(void)
{
	struct stm32f_dma * dma = STM32_DMA1;

	if (dma->isr & DMA_TCIF6) {
		/* clear the DMA transfer complete flag */
		dma->ifcr = DMA_CTCIF6;
		/* stop DMA */
		dma->ch[UART2_RX_DMA_CHAN].ccr = 0;
		DCC_LOG(LOG_INFO, "DMA RX complete...");
		__thinkos_flag_signal(serial2_dev.rx_flag);

	}
	if (dma->isr & DMA_TEIF6) {
		/* clear the DMA error flag */
		dma->ifcr = DMA_TEIF6;
		DCC_LOG(LOG_WARNING, "DMA Error");
	}
}


/*********************************************
 * TX DMA ISR
 *********************************************/
void stm32_dma1_channel7_isr(void)
{
	struct stm32f_dma * dma = STM32_DMA1;

	if (dma->isr & DMA_TCIF7) {
		/* clear the DMA transfer complete flag */
		dma->ifcr = DMA_CTCIF7;
		/* stop DMA */
		dma->ch[UART2_TX_DMA_CHAN].ccr = 0;
		DCC_LOG(LOG_INFO, "DMA TX complete...");
		__thinkos_flag_signal(serial2_dev.tx_flag);

	}
	if (dma->isr & DMA_TEIF7) {
		/* clear the DMA error flag */
		dma->ifcr = DMA_TEIF7;
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
	int c;
	
	sr = uart->sr;
	DCC_LOG1(LOG_INFO, "sr=%04x", sr);

	if (sr & USART_IDLE) {
		/* clear IDLE flag */
		c = uart->dr;
		(void)c;
		/* stop DMA */
		dma->ch[UART2_RX_DMA_CHAN].ccr = 0;
		DCC_LOG(LOG_INFO, "IDLE");
		__thinkos_flag_signal(serial2_dev.rx_flag);
	} else {
		DCC_LOG1(LOG_INFO, "sr=%04x", sr);
	}
}

struct serdrv * serdrv_init(unsigned int speed)
{
	struct serdrv * drv  = &serial2_dev;
	struct stm32_usart * uart = STM32_USART2;
	struct stm32f_dma * dma = STM32_DMA1;

	/* alloc kernel objects */
	drv->rx_flag = thinkos_flag_alloc();
	drv->tx_flag = thinkos_flag_alloc();
	drv->rx_pos = 0;
	/* signal the TX flag, indicates the 
	   transmitter is ready to start transmitting */
	thinkos_flag_set(drv->tx_flag);

	DCC_LOG1(LOG_INFO, "speed=%d", speed);

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
	dma->ch[UART2_TX_DMA_CHAN].cmar = (void *)drv->tx_buf;

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
	dma->ch[UART2_RX_DMA_CHAN].cmar = (void *)drv->rx_buf;
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
	int rem = len;
	int n;
	int i;

	while (rem > 0) {
		DCC_LOG(LOG_INFO, "TX wait...");
		/* wait for pending DMA xfer to complete */
		thinkos_flag_wait(drv->tx_flag);

		/* clear idle flag */
		thinkos_flag_clr(drv->tx_flag);

		n = MIN(rem, SERIAL_TX_BUF_LEN);

		/* Copy data to buffer */
		for (i = 0; i < n; ++i)
			drv->tx_buf[i] = src[i];

		/* Number of data items to transfer */
		dma->ch[UART2_TX_DMA_CHAN].cndtr = n;
		/* Configure and enable DMA */
		dma->ch[UART2_TX_DMA_CHAN].ccr = DMA_MSIZE_8 | DMA_PSIZE_8 | DMA_MINC |
			DMA_DIR_MTP | DMA_TCIE | DMA_TEIE | DMA_EN;

		DCC_LOG1(LOG_INFO, "n=%d", n);
		rem -= n;
		src += n;
	}

	return len;
}

int serdrv_recv(struct serdrv * drv, void * buf, int len, unsigned int tmo)
{
	struct stm32f_dma * dma = STM32_DMA1;
	uint8_t * dst = (uint8_t *)buf;
	uint8_t * src;
	int rem;
	int cnt;
	int n;
	int i;
	int ret;

	DCC_LOG(LOG_INFO, "RX wait...");

	/* wait for DMA xfer to complete */
	if ((ret = thinkos_flag_timedwait(drv->rx_flag, tmo)) < 0) {
		DCC_LOG(LOG_INFO, "Timeout!");
		return ret;
	}

	src = &drv->rx_buf[drv->rx_pos];

	/* Number of data items transfered... */
	cnt = SERIAL_RX_BUF_LEN - dma->ch[UART2_RX_DMA_CHAN].cndtr;
	rem = cnt - drv->rx_pos;

	DCC_LOG3(LOG_INFO, "pos=%d cnt=%d rem=%d", drv->rx_pos, cnt, rem);

	n = MIN(rem, len);

	/* Copy data to buffer */
	for (i = 0; i < n; ++i)
		dst[i] = src[i];

	rem -= n;

	if (rem == 0) {
		/* reset buffer position index */
		drv->rx_pos = 0;
		/* clear flag */
		thinkos_flag_clr(drv->rx_flag);
		/* Number of data items to transfer */
		dma->ch[UART2_RX_DMA_CHAN].cndtr = SERIAL_RX_BUF_LEN;
		/* Configure and enable DMA */
		dma->ch[UART2_RX_DMA_CHAN].ccr = DMA_MSIZE_8 | DMA_PSIZE_8 | DMA_MINC |
			DMA_DIR_PTM | DMA_TCIE | DMA_TEIE | DMA_EN;

		DCC_LOG(LOG_INFO, "DMA RX enabled.");
	} else  {
		drv->rx_pos += n;
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

