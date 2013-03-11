/* 
 * Copyright(C) 2013 Robinson Mittmann. All Rights Reserved.
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
#include <sys/delay.h>
#include <sys/usb-dev.h>


#include <thinkos.h>
#define __THINKOS_IRQ__
#include <thinkos_irq.h>

#include <sys/dcclog.h>

struct stm32f_usb_ep {
	uint16_t * data;
	uint16_t * status;
};

struct stm32f_usb_tx_buf_desc {
	uint16_t addr;
	uint16_t count;
};

struct stm32f_usb_rx_buf_desc {
	uint16_t addr;
	uint16_t count: 9;
	uint16_t num_block: 5;
	uint16_t blsize: 1;
};

/* Buffer descriptor */
struct stm32f_usb_buf_desc {
	union {
		struct stm32f_usb_tx_buf_desc tx;
		struct stm32f_usb_rx_buf_desc rx;
	};
};

/* Endpoint control */
struct stm32f_usb_ep {
	struct stm32f_usb_buf_desc * desc;
	uint16_t * status;
};

/* USB Driver */
struct stm32f_usb_drv {
	struct stm32f_usb_ep ep[8];
	uint8_t ep_cnt;
};

/*
 Upon system and power-on reset, the first operation the application software should perform
 is to provide all required clock signals to the USB peripheral and subsequently de-assert its
 reset signal so to be able to access its registers. The whole initialization sequence is
 hereafter described.
 As a first step application software needs to activate register macrocell clock and de-assert
 macrocell specific reset signal using related control bits provided by device clock
 management logic.
 After that, the analog part of the device related to the USB transceiver must be switched on
 using the PDWN bit in CNTR register, which requires a special handling. This bit is intended
 to switch on the internal voltage references that supply the port transceiver. This circuit has
 a defined startup time (tSTARTUP specified in the datasheet) during which the behavior of the
 USB transceiver is not defined. It is thus necessary to wait this time, after setting the PDWN
 bit in the CNTR register, before removing the reset condition on the USB part (by clearing
 the FRES bit in the CNTR register). Clearing the ISTR register then removes any spurious
 pending interrupt before any other macrocell operation is enabled.
 At system reset, the microcontroller must initialize all required registers and the packet
 buffer description table, to make the USB peripheral able to properly generate interrupts and
 data transfers. All registers not specific to any endpoint must be initialized according to the
 needs of application software (choice of enabled interrupts, chosen address of packet
 buffers, etc.). Then the process continues as for the USB reset case (see further
 paragraph). */


int stm32f_usb_dev_init(struct stm32f_usb_drv * drv, struct usb_ep_info * ep, unsigned int cnt)
{
	struct stm32f_usb * usb = STM32F_USB;
	struct stm32f_rcc * rcc = STM32F_RCC;
	struct stm32f_usb_buf_desc * desc = (struct stm32f_usb_buf_desc *)STM32F_USB_PKTBUF;
	uint32_t ep_addr = STM32F_USB_PKTBUF_SIZE;
	uint32_t epr;

	int sz;
	int i;
	
	DCC_LOG(LOG_TRACE, "Enabling USB device clock...");
	rcc->apb1enr |= RCC_USBEN;

	/* USB power ON */
	usb->cntr &= ~USB_PDWN;
	/* Wait tSTARTUP time ... */
	udelay(2);
	/* Removing the reset condition */
	usb->cntr &= ~USB_FRES;

	/* Removing any spurious pending interrupts */
	usb->istr = 0;

	/* Configure the endpoints and allocate packet buffers */
	usb->btable = STM32F_USB_PKTBUF;
	for (i = 0; i < cnt; i++) {
		int blk;
		int blsize;
		sz = ep[i].mxpktsz;
		if (sz > 62) {
			blk = sz >> 4;
			blsize = 1;
		} else {
			blk = sz;
			blsize = 0;
		}

		/* round up to a multiple of 2 */
		sz = (sz + 1) & ~1;
		/* set the descriptor pointer */
		drv->ep[i].desc = desc;
		/* Set EP address */
		epr = USB_EA_SET(ep[i].addr);
		switch (ep[i].type)
		{
		case ENDPOINT_TYPE_CONTROL:
			epr |= USB_EP_CONTROL;
			/* allocate single buffers for TX and RX */
			DCC_LOG2(LOG_TRACE, "EP[%d]: CONTROL -> 0x%08x", i, desc);

			ep_addr -= sz;
			desc->tx.addr = ep_addr;
			desc->tx.count = 0;
			desc++;
			ep_addr -= sz;
			desc->rx.addr = ep_addr;
			desc->rx.count = 0;
			desc->rx.num_block = blk;
			desc->rx.blsize = blsize;
			desc++;
			break;

		case ENDPOINT_TYPE_ISOCHRONOUS:
			epr |= USB_EP_ISO | USB_EP_DBL_BUF;
			/* allocate double buffers for TX or RX */
			DCC_LOG2(LOG_TRACE, "EP[%d]: CONTROL -> 0x%08x", i, desc);
			desc += 2;
			/* FIXME: implement this */
			break;

		case ENDPOINT_TYPE_BULK:
			epr |= USB_EP_BULK | USB_EP_DBL_BUF;
			DCC_LOG2(LOG_TRACE, "EP[%d]: BULK -> 0x%08x", i, desc);
			/* allocate double buffers for TX or RX */
			ep_addr -= 2 * sz;
			desc[0].tx.addr = ep_addr;
			desc[0].tx.count = 0;
			desc[1].tx.addr = ep_addr + sz;
			desc[1].tx.count = 0;
			if ((ep[i].addr & USB_ENDPOINT_IN) == 0) {
				desc[0].rx.num_block = blk;
				desc[0].rx.blsize = blsize;
				desc[1].rx.num_block = blk;
				desc[1].rx.blsize = blsize;
			}
			desc += 2;
			break;

		case ENDPOINT_TYPE_INTERRUPT:
			epr |= USB_EP_INTERRUPT;
			/* allocate single buffers for TX or RX */
			DCC_LOG2(LOG_TRACE, "EP[%d]: CONTROL -> 0x%08x", i, desc);
			drv->ep[i].desc = desc;
			ep_addr -= sz;
			desc->tx.addr = ep_addr;
			desc->tx.count = 0;
			if ((ep[i].addr & USB_ENDPOINT_IN) == 0) {
				desc->rx.addr = ep_addr;
				desc->rx.count = 0;
				desc->rx.num_block = blk;
				desc->rx.blsize = blsize;
			}
			desc++;

			break;
		}

		usb->epr[i] = epr;
	}

	drv->ep_cnt = cnt;

	return 0;
}

/* Private USB device driver data */
struct stm32f_usb_drv stm32f_usb_drv0;

/* USB device operations */
const struct usb_dev_ops stm32f_usb_ops = {
	.usb_dev_init = (usb_dev_init_t)stm32f_usb_dev_init
};

/* USB device driver */
const struct usb_dev stm32f_usb_dev = {
	.priv = (void *)&stm32f_usb_drv0,
	.op = &stm32f_usb_ops
};


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

