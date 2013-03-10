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
 * @brief YARD-ICE UART console
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

#if (THINKOS_EVENT_MAX > 0) && (THINKOS_ENABLE_EVENT_ALLOC)

#define UART_FIFO_BUF_LEN 64

#define USART5_TXIE ((uint32_t *) CM3_PERIPHERAL_BITBAND_ADDR( \
						STM32F_BASE_UART5 + STM32F_USART_CR1, 7))


struct uart_fifo {
	volatile uint32_t head;
	volatile uint32_t tail;
	uint8_t buf[UART_FIFO_BUF_LEN];
};

static inline void uart_fifo_init(struct uart_fifo * fifo)
{
	fifo->head = 0;
	fifo->tail = 0;
}

static inline uint8_t uart_fifo_get(struct uart_fifo * fifo)
{
	return fifo->buf[fifo->tail++ & (UART_FIFO_BUF_LEN - 1)];
}

static inline void uart_fifo_put(struct uart_fifo * fifo, uint8_t c)
{
	fifo->buf[fifo->head++ & (UART_FIFO_BUF_LEN - 1)] = c;
}

static inline bool uart_fifo_is_empty(struct uart_fifo * fifo)
{
	return (fifo->tail == fifo->head) ? true : false;
}

static inline bool uart_fifo_is_full(struct uart_fifo * fifo)
{
	return ((fifo->head - fifo->tail) == UART_FIFO_BUF_LEN) ? true : false;
}

static inline bool uart_fifo_is_half_full(struct uart_fifo * fifo)
{
	return ((fifo->head - fifo->tail) > (UART_FIFO_BUF_LEN / 2)) ? true : false;
}

struct uart_console_dev {
	int8_t rx_ev;
	int8_t tx_ev;
	struct uart_fifo tx_fifo;
	struct uart_fifo rx_fifo;
};

int uart_console_read(struct uart_console_dev * dev, char * buf, 
				 unsigned int len, unsigned int msec)
{

	char * cp = (char *)buf;
	int c;
	int n = 0;
	int th;

	DCC_LOG(LOG_INFO, "read");
	th = thinkos_thread_self();

	__thinkos_critical_enter();
	while (uart_fifo_is_empty(&dev->rx_fifo)) {
		DCC_LOG(LOG_INFO, "wait...");
		if ( th == 4) {
			DCC_LOG1(LOG_TRACE, "[%d] wait ...", th);
		}
		__thinkos_ev_wait(dev->rx_ev);
		if ( th == 4) {
			DCC_LOG1(LOG_TRACE, "[%d] wakeup.", th);
		}
		DCC_LOG(LOG_INFO, "wakeup.");
	}
	__thinkos_critical_exit();

	do {
		if (n == len) {
			if (!uart_fifo_is_empty(&dev->rx_fifo)) { 
				__thinkos_ev_raise(dev->rx_ev);
			}
			break;
		}
		c = uart_fifo_get(&dev->rx_fifo);
		if (c == '\r') 
			c = '\n';
		cp[n++] = c;
	} while (!uart_fifo_is_empty(&dev->rx_fifo));



	DCC_LOG2(LOG_TRACE, "[%d] n=%d", thinkos_thread_self(), n);

	return n;
}

static void uart_putc(struct uart_console_dev * dev, int c)
{
	__thinkos_critical_enter();
	while (uart_fifo_is_full(&dev->tx_fifo)) {
		/* enable TX interrupt */
		DCC_LOG(LOG_INFO, "wait...");
		__thinkos_ev_wait(dev->tx_ev);
		DCC_LOG(LOG_INFO, "wakeup");
	}
	__thinkos_critical_exit();

	uart_fifo_put(&dev->tx_fifo, c);
	*USART5_TXIE = 1; 
}

int uart_console_write(struct uart_console_dev * dev, const void * buf, 
					   unsigned int len)
{
	char * cp = (char *)buf;
	int c;
	int n;

	DCC_LOG1(LOG_INFO, "len=%d", len);

	for (n = 0; n < len; n++) {
		c = cp[n];
		if (c == '\n') {
			DCC_LOG(LOG_INFO, "CR");
			uart_putc(dev, '\r');
		}
		uart_putc(dev, c);
	}

	DCC_LOG1(LOG_INFO, "cnt=%d", n);

	return n;
}

int uart_console_flush(struct uart_console_dev * ctrl)
{
	return 0;
}

const struct fileop uart_console_ops = {
	.write = (void *)uart_console_write,
	.read = (void *)uart_console_read,
	.flush = (void *)uart_console_flush,
	.close = (void *)NULL
};

struct uart_console_dev uart5_console_dev;

const struct file uart5_console_file = {
	.data = (void *)&uart5_console_dev, 
	.op = &uart_console_ops
};

void stm32f_uart5_isr(void)
{
	struct uart_console_dev * dev = &uart5_console_dev;
	struct stm32f_usart * us = STM32F_UART5;
	uint32_t sr;
	int c;
	
	sr = us->sr;

	if (sr & USART_RXNE) {
		DCC_LOG(LOG_INFO, "RXNE");
		c = us->dr;
		if (!uart_fifo_is_full(&dev->rx_fifo)) { 
			uart_fifo_put(&dev->rx_fifo, c);
		} else {
			DCC_LOG(LOG_WARNING, "RX fifo full!");
		}
		
		if (uart_fifo_is_half_full(&dev->rx_fifo)) { 
			__thinkos_ev_raise(dev->rx_ev);
		}
	}	

	if (sr & USART_IDLE) {
		DCC_LOG(LOG_INFO, "IDLE");
		c = us->dr;
		(void)c;
		__thinkos_ev_raise(dev->rx_ev);
	}

	if (sr & USART_TXE) {
		DCC_LOG(LOG_MSG, "TXE");
		if (uart_fifo_is_empty(&dev->tx_fifo)) {
			/* disable TXE interrupts */
			*USART5_TXIE = 0; 
			__thinkos_ev_raise(dev->tx_ev);
		} else {
			us->dr = uart_fifo_get(&dev->tx_fifo);
		}
	}
}

struct file * uart_console_open(unsigned int baudrate, unsigned int mode)
{
	struct stm32f_usart * us = STM32F_UART5;
	struct uart_console_dev * dev = &uart5_console_dev;

	DCC_LOG(LOG_TRACE, "...");
	stm32f_usart_init(us);

	dev->rx_ev = thinkos_ev_alloc(); 
	dev->tx_ev = thinkos_ev_alloc(); 
	uart_fifo_init(&dev->tx_fifo);
	uart_fifo_init(&dev->rx_fifo);

	stm32f_usart_baudrate_set(us, baudrate);

	stm32f_usart_mode_set(us, mode);

	cm3_irq_enable(STM32F_IRQ_UART5);
	/* enable RX interrupt */
	us->cr1 |= USART_RXNEIE | USART_IDLEIE;

	return (struct file *)&uart5_console_file;
}


#if 0

#define CONSOLE_RX_BUF_LEN 16

struct uart_console_ctrl {
	struct stm32f_usart * uart;
	struct stm32f_dma * dma;
	struct {
		int irq;
		struct stm32f_dma_stream * s;
	} tx;
	struct {
		int irq;
		struct stm32f_dma_stream * s;
		uint8_t buf[CONSOLE_RX_BUF_LEN];
	} rx;
};

int uart_console_read(struct uart_console_ctrl * ctrl, char * buf, 
				 unsigned int len, unsigned int msec)
{
	uint32_t st;
	unsigned int cnt;

	DCC_LOG(LOG_INFO, "read");

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
	while (((st = ctrl->dma->lisr) & DMA_TCIF0) == 0) {
		DCC_LOG(LOG_INFO, "wait...");
		__thinkos_irq_wait(ctrl->rx.irq);
		DCC_LOG(LOG_INFO, "wakeup.");
	} 

	/* clear the the DMA stream trasfer complete flag */
	ctrl->dma->lifcr = DMA_CTCIF0;

	/* Number of data items transfered... */
	cnt = len - ctrl->rx.s->ndtr;
	
	DCC_LOG1(LOG_TRACE, "cnt=%d", cnt);
	return cnt;
}

int uart_console_write(struct uart_console_ctrl * ctrl, const void * buf, 
					   unsigned int len)
{
	unsigned int cnt;
	uint32_t st;
	uint32_t sr;

	DCC_LOG1(LOG_TRACE, "len=%d", len);

	if (ctrl->tx.s->cr & DMA_EN) {
		DCC_LOG(LOG_TRACE, "DMA enabled");
	}

	/* Memory address */
	ctrl->tx.s->m0ar = (void *)buf;
	/* Number of data items to transfer */
	ctrl->tx.s->ndtr = len;

	/* clear the TC bit */
	if ((sr = ctrl->uart->sr) & USART_TC) {
		DCC_LOG(LOG_INFO, "TC=1");
	}

	/* enable DMA */
	ctrl->tx.s->cr = DMA_CHSEL_SET(4) | DMA_MBURST_1 | DMA_PBURST_1 | 
		DMA_MSIZE_8 | DMA_PSIZE_8 | DMA_MINC | 
		DMA_DIR_MTP | DMA_TCIE  | DMA_EN;

	/* wait for the DMA transfer to complete */
	while (((st = ctrl->dma->hisr) & DMA_TCIF7) == 0) {
		DCC_LOG(LOG_INFO, "wait...");
		__thinkos_irq_wait(ctrl->tx.irq);
		DCC_LOG(LOG_INFO, "wakeup");
	} 

	/* clear the the DMA stream trasfer complete flag */
	ctrl->dma->hifcr = DMA_CTCIF7;

	/* Number of data items transfered... */
	cnt = len - ctrl->tx.s->ndtr;
	
	DCC_LOG1(LOG_INFO, "cnt=%d", cnt);
	return cnt;
}

int uart_console_flush(struct uart_console_ctrl * ctrl)
{
	return 0;
}

const struct uart_console_ctrl uart5_ctrl = {
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

const struct fileop uart_console_ops = {
	.write = (void *)uart_console_write,
	.read = (void *)uart_console_read,
	.flush = (void *)uart_console_flush,
	.close = (void *)NULL
};

const struct file uart_console_file = {
	.data = (void *)&uart5_ctrl, 
	.op = &uart_console_ops
};

void stm32f_uart5_isr(void)
{
//	struct serial_dev * dev = &dev_ttyS5;
	struct stm32f_usart * us;
	struct uart_console_ctrl * ctrl;
	uint32_t sr;
	int c;
	
	ctrl = (struct uart_console_ctrl *)uart_console_file.data;
	us = ctrl->uart;
	sr = us->sr;

	if (sr & USART_RXNE) {
		DCC_LOG(LOG_TRACE, "RXNE");
		c = us->dr;
		c = c;
/*		if (!uart_fifo_is_full(&dev->rx_fifo)) { 
			uart_fifo_put(&dev->rx_fifo, c);
		} */
	}	

	if (sr & USART_IDLE) {
		DCC_LOG(LOG_TRACE, "IDLE");
		/* disable IDLE interrupt */
//		us->cr1 &= ~USART_IDLEIE;
		ctrl->rx.s->cr &= ~DMA_EN;
		c = us->dr;
		c = c;
	}

	if (sr & USART_TXE) {
		DCC_LOG(LOG_MSG, "TXE");
/*		if (uart_fifo_is_empty(&dev->tx_fifo)) {
			*dev->txeie = 0;
		} else {
			us->dr = uart_fifo_get(&dev->tx_fifo);
		} */
	}

}


struct file * uart_console_open(unsigned int baudrate, unsigned int flags)
{
	struct stm32f_rcc * rcc = STM32F_RCC;
	struct stm32f_usart * us;
	struct uart_console_ctrl * ctrl;

	ctrl = (struct uart_console_ctrl *)uart_console_file.data;
	us = ctrl->uart;

	DCC_LOG(LOG_TRACE, "...");
	stm32f_usart_init(ctrl->uart, baudrate, flags);

	/* Enable DMA for transmission and reception */
	ctrl->uart->cr3 |= USART_DMAT | USART_DMAR;

	/* DMA clock enable */
	rcc->ahb1enr |= (ctrl->dma == STM32F_DMA1) ? RCC_DMA1EN : RCC_DMA2EN;

	/* Disable DMA stream */
	ctrl->tx.s->cr = 0;
	while (ctrl->tx.s->cr & DMA_EN); /* Wait for the channel to be ready .. */
	ctrl->tx.s->par = &ctrl->uart->dr;
	ctrl->tx.s->fcr = DMA_DMDIS | DMA_FTH_FULL;

	/* Disable DMA stream */
	ctrl->rx.s->cr = 0;
	while (ctrl->rx.s->cr & DMA_EN); /* Wait for the channel to be ready .. */
	ctrl->rx.s->par = &ctrl->uart->dr;
	/* Memory address */
	ctrl->tx.s->m0ar = (void *)ctrl->rx.buf;
	/* Number of data items to transfer */
	ctrl->tx.s->ndtr = CONSOLE_RX_BUF_LEN;
	ctrl->rx.s->cr = DMA_CHSEL_SET(4) | DMA_MBURST_1 | DMA_PBURST_1 | 
		DMA_MSIZE_8 | DMA_PSIZE_8 | DMA_MINC | 
		DMA_DIR_PTM;


	cm3_irq_enable(STM32F_IRQ_UART5);
	/* enable IDLE interrupt */
	us->cr1 |= USART_IDLEIE;

	return (struct file *)&uart_console_file;
}

#endif

#endif /* (THINKOS_EVENT_MAX > 0) && (THINKOS_ENABLE_EVENT_ALLOC) */

