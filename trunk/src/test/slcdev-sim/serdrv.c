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

#if 0

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
		rem -= n;
		src += n;
#else /* SERDRV_DOUBLE_BUFFER */
		/* having the thinkos_flag_wait() at the beginning of the
		   loop provides thread safeness, as only one thread will be 
		   allowed to enter the critical section. 
		   The last statement in the section is the DMA enabling. */
		thinkos_flag_wait(TX_FLAG);
		thinkos_flag_clr(TX_FLAG);

		/* Copy data to buffer */
		n = MIN(rem, SERIAL_TX_BUF_LEN);
		for (i = 0; i < n; ++i)
			drv->tx_buf[0][i] = src[i];

		rem -= n;
		src += n;

		/* Configure and enable DMA */
		dma->ch[UART2_TX_DMA_CHAN].cndtr = n;
		dma->ch[UART2_TX_DMA_CHAN].ccr = DMA_MSIZE_8 | DMA_PSIZE_8 | 
			DMA_MINC | DMA_DIR_MTP | DMA_TCIE | DMA_TEIE | DMA_EN;
#endif /* SERDRV_DOUBLE_BUFFER */
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

int serdrv_send(struct serdrv * dev, const void * buf, int len)
{
	uint8_t * cp = (uint8_t *)buf;
	int rem = len;

	DCC_LOG1(LOG_INFO, "len=%d", len);

	while (rem) {
		uint8_t * dst;
		unsigned int head;
		int free;
		int cnt;
		int pos;
		int i;

		thinkos_flag_wait(TX_FLAG);
		thinkos_flag_clr(TX_FLAG);

		head = dev->tx_fifo.head;
		free = TX_LEN - (int8_t)(head - dev->tx_fifo.tail);
		DCC_LOG3(LOG_MSG, "head=%d tail=%d n=%d", head, dev->tx_fifo.tail, n);

		cnt = MIN(rem, free);
		pos = head & (TX_LEN - 1);

		if ((pos + cnt) > TX_LEN) {
			int n;
			int m;
			
			/* get the number of chars from head pos until the end of buffer */
			n = TX_LEN - pos;
			/* the remaining empty space is at the beginning of the buffer */
			m = cnt - n;

			dst = &dev->tx_fifo.buf[pos];
			for (i = 0; i < n; ++i) 
				dst[i] = cp[i];
			cp += n;
			dst = &dev->tx_fifo.buf[0];
			for (i = 0; i < m; ++i) 
				dst[i] = cp[i];
		} else {
			dst = &dev->tx_fifo.buf[pos];
			for (i = 0; i < cnt; ++i) 
				dst[i] = cp[i];
		}

		dev->tx_fifo.head = head + cnt;
		*dev->txie = 1; 

		if (free > cnt)
			__thinkos_flag_signal(TX_FLAG);

		rem -= cnt;
		DCC_LOG1(LOG_INFO, "rem=%d", rem);
	}

	return len;
}

#else

#define UART_TX_FIFO_BUF_LEN 16
#define UART_RX_FIFO_BUF_LEN 16

#define RX_FLAG SERDRV_RX_FLAG 
#define TX_FLAG SERDRV_TX_FLAG 
#define TX_LEN UART_TX_FIFO_BUF_LEN
#define RX_LEN UART_RX_FIFO_BUF_LEN

struct serdrv {
	struct {
		volatile uint8_t head;
		volatile uint8_t tail;
		uint8_t buf[UART_TX_FIFO_BUF_LEN];
	} tx_fifo;	
	struct {
		volatile uint8_t head;
		volatile uint8_t tail;
		uint8_t buf[UART_RX_FIFO_BUF_LEN];
	} rx_fifo;	
	uint32_t * txie;
};

int serdrv_recv(struct serdrv * dev, void * buf, int len, unsigned int tmo)
{
	uint8_t * cp = (uint8_t *)buf;
	unsigned int tail;
	int ret;
	int cnt;
	int n;
	int i;

	DCC_LOG2(LOG_INFO, "1. len=%d tmo=%d", len, tmo);

again:
	if ((ret = thinkos_flag_timedwait(RX_FLAG, tmo)) < 0) {
		DCC_LOG(LOG_INFO, "Timeout!");
		return ret;
	}
	thinkos_flag_clr(RX_FLAG);


	tail = dev->rx_fifo.tail;
	cnt = (int8_t)(dev->rx_fifo.head - tail);
	if (cnt == 0) {
		DCC_LOG(LOG_WARNING, "RX FIFO empty!");
		goto again;
	}
	n = MIN(len, cnt);
	DCC_LOG2(LOG_INFO, "tail=%d cnt=%d", tail, cnt);

	for (i = 0; i < n; ++i)
		cp[i] = dev->rx_fifo.buf[tail++ & (RX_LEN - 1)];

	dev->rx_fifo.tail = tail;

	if (cnt > n)
		__thinkos_flag_signal(RX_FLAG);

	return n;
}

int serdrv_send(struct serdrv * dev, const void * buf, int len)
{
	uint8_t * cp = (uint8_t *)buf;
	int rem = len;

	DCC_LOG1(LOG_INFO, "len=%d", len);

	while (rem) {
		unsigned int head;
		int free;
		int n;
		int i;

		thinkos_flag_wait(TX_FLAG);
		thinkos_flag_clr(TX_FLAG);

		head = dev->tx_fifo.head;
		free = TX_LEN - (int8_t)(head - dev->tx_fifo.tail);
		DCC_LOG3(LOG_MSG, "head=%d tail=%d n=%d", head, dev->tx_fifo.tail, n);
		n = MIN(rem, free);
		for (i = 0; i < n; ++i) 
			dev->tx_fifo.buf[head++ & (TX_LEN - 1)] = *cp++;
		dev->tx_fifo.head = head;
		*dev->txie = 1; 

		if (free > n)
			__thinkos_flag_signal(TX_FLAG);
		rem -= n;
		DCC_LOG1(LOG_INFO, "rem=%d", rem);
	}


	return len;
}

/* static serial driver object */
static struct serdrv serial2_dev;

void stm32_usart2_isr(void)
{
	struct serdrv * dev = &serial2_dev;
	struct stm32_usart * us = STM32_USART2;
	uint32_t sr;
	int c;
	
	sr = us->sr & us->cr1;

	if (sr & USART_RXNE) {
		unsigned int head;
		int free;

		c = us->dr;

		head = dev->rx_fifo.head;
		free = RX_LEN - (uint8_t)(head - dev->rx_fifo.tail);
		if (free > 0) { 
			dev->rx_fifo.buf[head & (RX_LEN - 1)] = c;
			dev->rx_fifo.head = head + 1;
			if (free < (RX_LEN / 2))
				__thinkos_flag_signal(RX_FLAG);
		} else {
			DCC_LOG(LOG_WARNING, "RX fifo full!");
		}
		
	}	

	if (sr & USART_IDLE) {
		DCC_LOG(LOG_MSG, "IDLE!");
		c = us->dr;
		(void)c;
		__thinkos_flag_signal(RX_FLAG);
	}

	if (sr & USART_TXE) {
		unsigned int tail = dev->tx_fifo.tail;
		if (tail == dev->tx_fifo.head) {
			/* FIFO empty, disable TXE interrupts */
			*dev->txie = 0; 
			__thinkos_flag_signal(TX_FLAG);
		} else {
			us->dr = dev->tx_fifo.buf[tail & (TX_LEN - 1)];
			dev->tx_fifo.tail = tail + 1;
		}
	}
}


struct serdrv * serdrv_init(unsigned int speed)
{
	struct serdrv * drv  = &serial2_dev;
	struct stm32_usart * uart = STM32_USART2;

	DCC_LOG1(LOG_MSG, "speed=%d", speed);

	/* clock enable */
	stm32_clk_enable(STM32_RCC, STM32_CLK_USART2);

	/*********************************************
	 * USART 
	 *********************************************/
	stm32_usart_init(uart);
	stm32_usart_baudrate_set(uart, speed);
	stm32_usart_mode_set(uart, SERIAL_8N1);

	/* Enable DMA for transmission and reception */
	uart->cr3 |= USART_DMAT | USART_DMAR;
	/* enable idle line interrupt */
	/* enable RX interrupt */
	uart->cr1 |= USART_RXNEIE | USART_IDLEIE;

	/* enable UART */
	stm32_usart_enable(uart);

	drv->tx_fifo.head = drv->tx_fifo.tail = 0;
	drv->rx_fifo.head = drv->rx_fifo.tail = 0;
	drv->txie = CM3_BITBAND_DEV(&uart->cr1, 7);
	__thinkos_flag_signal(TX_FLAG);

	/* configure interrupts */
	cm3_irq_pri_set(STM32_IRQ_USART2, IRQ_PRIORITY_LOW);

	/* enable interrupts */
	cm3_irq_enable(STM32_IRQ_USART2);

	return drv;
}

#endif

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

