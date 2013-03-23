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
 * @file stm32f-usb_fs.c
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
#include <sys/dcclog.h>

#define USB_FS_DP STM32F_GPIOA, 12
#define USB_FS_DM STM32F_GPIOA, 11
#define USB_FS_VBUS STM32F_GPIOB, 12

void stm32f_usb_io_init(void)
{
	stm32f_gpio_clock_en(STM32F_GPIOA);
	stm32f_gpio_clock_en(STM32F_GPIOB);

	stm32f_gpio_mode(USB_FS_DP, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32f_gpio_mode(USB_FS_DM, ALT_FUNC, PUSH_PULL | SPEED_HIGH);

	/* PB12: External Pull-up */
	stm32f_gpio_mode(USB_FS_VBUS, INPUT, 0);
	stm32f_gpio_set(USB_FS_VBUS);
}

void stm32f_usb_vbus_connect(bool connect)
{
	if (connect)
		stm32f_gpio_mode(USB_FS_VBUS, OUTPUT, PUSH_PULL | SPEED_LOW);
	else
		stm32f_gpio_mode(USB_FS_VBUS, INPUT, SPEED_LOW);
#if 0
	if (connect)
		stm32f_gpio_mode(USB_FS_VBUS, ALT_FUNC, SPEED_LOW);
	else
		stm32f_gpio_mode(USB_FS_VBUS, INPUT, 0);
#endif
}

void stm32f_usb_power_on(struct stm32f_usb * usb)
{
	struct stm32f_rcc * rcc = STM32F_RCC;

	DCC_LOG(LOG_TRACE, "Enabling USB device clock...");

	stm32f_usb_vbus_connect(true);

	rcc->apb1enr |= RCC_USBEN;

	/* USB power ON */
	usb->cntr = USB_FRES;
	/* Wait tSTARTUP time ... */
	udelay(2);
	/* Removing the reset condition */
	usb->cntr = 0;

	/* Removing any spurious pending interrupts */
	usb->istr = 0;

	/* enable Cortex interrupts */
	cm3_irq_enable(STM32F_IRQ_USB_LP);

	cm3_irq_enable(STM32F_IRQ_USB_HP);

}

void stm32f_usb_power_off(struct stm32f_usb * usb)
{
	struct stm32f_rcc * rcc = STM32F_RCC;

	usb->cntr = USB_FRES;
	/* Removing any spurious pending interrupts */
	usb->istr = 0;

	stm32f_usb_vbus_connect(false);

	usb->cntr = USB_FRES | USB_PDWN;

	DCC_LOG(LOG_TRACE, "Disabling USB device clock...");
	rcc->apb1enr &= ~RCC_USBEN;

	/* disabling IO pins */
	stm32f_gpio_mode(USB_FS_DP, INPUT, 0);
	stm32f_gpio_mode(USB_FS_DM, INPUT, 0);
}

/* configure a RX descriptor */
int pktbuf_rx_cfg(struct stm32f_usb_rx_pktbuf * rx,
						 unsigned int addr, unsigned int mxpktsz)
{
//	int sz = mxpktsz + 2; /* alloc 2 extra bytes for CRC */
	int sz = mxpktsz;

	if (sz < 63) {
		sz = (sz + 1) & ~0x01;
		rx->num_block = sz >> 1;
		rx->blsize = 0;
	} else {
		/* round up to a multiple of 32 */
		sz = (sz + 0x1f) & ~0x1f;
		rx->num_block = (sz >> 5) - 1;
		rx->blsize = 1;
	}

	rx->addr = addr;
	rx->count = 0;

	return sz;
}

/* configure a TX descriptor */
int pktbuf_tx_cfg(struct stm32f_usb_tx_pktbuf * tx,
						 unsigned int addr, unsigned int mxpktsz)
{
	tx->addr = addr;
	tx->count = 0;

	return mxpktsz;
}

int pktbuf_rx_mxpktsz(struct stm32f_usb_rx_pktbuf * rx)
{
	int sz;

	if (rx->blsize)
		sz = (rx->num_block + 1) * 32;
	else
		sz = rx->num_block * 2;

	return sz;
}


void clr_ep_kind(struct stm32f_usb * usb, int ep_id)
{
	clr_ep_flag(usb, ep_id, USB_EP_KIND);
}

void set_ep_kind(struct stm32f_usb * usb, int ep_id)
{
	set_ep_flag(usb, ep_id, USB_EP_KIND);
}

void clr_status_out(struct stm32f_usb * usb, int ep_id)
{
	clr_ep_flag(usb, ep_id, USB_EP_KIND);
}

void set_ep_type(struct stm32f_usb * usb, int ep_id, int type)
{
	uint32_t epr;
	epr = usb->epr[ep_id] & USB_EP_TYPE_MSK;
	usb->epr[ep_id] = epr | type;
}

void set_ep_addr(struct stm32f_usb * usb, int ep_id, int addr)
{
	uint32_t epr;
	epr = usb->epr[ep_id] & USB_EPREG_MASK & ~USB_EA_MSK;
	usb->epr[ep_id] = epr | addr;
}

/* Set the base of the packet buffers just above the buffer descriptors:
   We have up to 8 descriptors of 8 bytes each. */
#define PKTBUF_BUF_BASE (8 * 8)
#define STM32F_USB_PKTBUF ((struct stm32f_usb_pktbuf *)STM32F_USB_PKTBUF_ADDR)

static unsigned int addr = PKTBUF_BUF_BASE;

void stm32f_usb_ep0_init(struct stm32f_usb * usb, int mxpktsz)
{
	/* packet buffer  */
	struct stm32f_usb_pktbuf * pktbuf = STM32F_USB_PKTBUF;
	unsigned int sz;

	/* clear the correct transfer bits */
	usb->epr[0] &= ~(USB_CTR_RX | USB_CTR_TX);

	set_ep_type(usb, 0, USB_EP_CONTROL);
	set_ep_txstat(usb, 0, USB_TX_NAK);

	/* allocate single buffers for TX and RX */
	sz = pktbuf_tx_cfg(&pktbuf[0].tx, addr, mxpktsz);
	addr += sz;
	sz = pktbuf_rx_cfg(&pktbuf[0].rx, addr, mxpktsz);
	addr += sz;

	clr_status_out(usb, 0);
	set_ep_rxstat(usb, 0, USB_RX_VALID);

	DCC_LOG1(LOG_TRACE, "epr=0x%04x...", usb->epr[0]);
}

/* This is a bitmask that when applied to the EPR register
 * will NOT change its value except possibly for the address
 */
#define EPR_INVARIANT (USB_CTR_RX | USB_CTR_TX)

void stm32f_usb_ep_init(struct stm32f_usb * usb, int ep_id,
		struct usb_descriptor_endpoint * desc)
{
	struct stm32f_usb_pktbuf * pktbuf = STM32F_USB_PKTBUF;
	unsigned int sz;
	unsigned int mxpktsz;

	mxpktsz = desc->maxpacketsize;

	DCC_LOG2(LOG_TRACE, "ep_id=%d mxpktsz=%d", ep_id, mxpktsz);

	/* clear the correct transfer bits */
	usb->epr[ep_id] &= ~(USB_CTR_RX | USB_CTR_TX);
	set_ep_addr(usb, ep_id, desc->endpointaddress & 0x7f);
	set_ep_rxstat(usb, ep_id, USB_RX_NAK);
	set_ep_rxstat(usb, ep_id, USB_TX_NAK);

	switch (desc->attributes & 0x03) {
	case ENDPOINT_TYPE_CONTROL:
		set_ep_type(usb, ep_id, USB_EP_CONTROL);
		/* allocate single buffers for TX and RX */
		sz = pktbuf_tx_cfg(&pktbuf[ep_id].tx, addr, mxpktsz);
		addr += sz;
		sz = pktbuf_rx_cfg(&pktbuf[ep_id].rx, addr, mxpktsz);
		addr += sz;
		break;

	case ENDPOINT_TYPE_ISOCHRONOUS:
		break;

	case ENDPOINT_TYPE_BULK:
		set_ep_type(usb, ep_id, USB_EP_BULK);
		if (desc->endpointaddress & USB_ENDPOINT_IN) {
			sz = pktbuf_tx_cfg(&pktbuf[ep_id].dbtx[0], addr, mxpktsz);
			addr += sz;
			sz = pktbuf_tx_cfg(&pktbuf[ep_id].dbtx[1], addr, mxpktsz);
			addr += sz;
			set_ep_rxstat(usb, ep_id, USB_TX_VALID);
		} else {
			sz = pktbuf_rx_cfg(&pktbuf[ep_id].dbrx[0], addr, mxpktsz);
			addr += sz;
			sz = pktbuf_rx_cfg(&pktbuf[ep_id].dbrx[1], addr, mxpktsz);
			addr += sz;
			set_ep_rxstat(usb, ep_id, USB_RX_VALID);
		}
		break;

	case ENDPOINT_TYPE_INTERRUPT:
		set_ep_type(usb, ep_id, USB_EP_INTERRUPT);
		if (desc->endpointaddress & USB_ENDPOINT_IN) {
			sz = pktbuf_tx_cfg(&pktbuf[ep_id].tx, addr, mxpktsz);
			addr += sz;
		} else {
			sz = pktbuf_rx_cfg(&pktbuf[ep_id].rx, addr, mxpktsz);
			addr += sz;
		}
		break;
	}



//	clr_status_out(usb, ep_id);

}

