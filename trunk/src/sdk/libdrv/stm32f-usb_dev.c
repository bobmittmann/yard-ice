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
#include <sys/param.h>


#include <thinkos.h>
#define __THINKOS_IRQ__
#include <thinkos_irq.h>

#include <sys/dcclog.h>

typedef enum {
	EP_IDLE,
	EP_STALLED,
	EP_SETUP,
	EP_IN_DATA,
	EP_IN_DATA_LAST,
	EP_WAIT_STATUS_IN,
	EP_WAIT_STATUS_OUT,
	EP_OUT_DATA,
	EP_OUT_DATA_LAST,
} ep_state_t;

/* Endpoint control */
struct stm32f_usb_ep {

	uint16_t mxpktsz;
	ep_state_t state;

	uint16_t xfr_rem;
	uint16_t xfr_buf_len;

	uint8_t * xfr_buf;
	uint8_t * xfr_ptr;

	union {
		usb_class_on_ep_ev_t on_ev;
		usb_class_on_ep_in_t on_in;
		usb_class_on_ep_out_t on_out;
		usb_class_on_ep_setup_t on_setup;
	};
};

#define USB_DRIVER_EP_MAX 8

/* USB Driver */
struct stm32f_usb_drv {
	struct stm32f_usb_ep ep[USB_DRIVER_EP_MAX];
	uint8_t ep_cnt;
	usb_class_t * cl;
	const struct usb_class_events * ev;
	const struct usb_descriptor_device * dsc_dev;
	uint8_t ctrl_st;
	struct usb_request req;
};

enum {
	CTRL_IDLE,
	CTRL_ERROR,
	CTRL_PAUSE,
	CTRL_SETUP,
	CTRL_SETUP_IN,
	CTRL_SETUP_OUT,
	CTRL_STALLED,
	CTRL_IN_DATA,
	CTRL_IN_DATA_LAST,
	CTRL_IN_DATA_ZLP,
	CTRL_WAIT_STATUS_IN,
	CTRL_ADDR_SET
};

#define STM32F_USB_PKTBUF ((struct stm32f_usb_pktbuf *)STM32F_USB_PKTBUF_ADDR)

static void __copy_from_pktbuf(void * ptr,
							struct stm32f_usb_rx_pktbuf * rx,
							unsigned int cnt)
{
	uint32_t * src;
	uint8_t * dst = (uint8_t *)ptr;
	uint32_t data;
	int i;

	/* copy data to destination buffer */
	src = (uint32_t *)STM32F_USB_PKTBUF_ADDR + (rx->addr / 2);
	for (i = 0; i < (cnt + 1) / 2; i++) {
		data = *src++;
		*dst++ = data;
		*dst++ = data >> 8;
	}
}

static void __copy_to_pktbuf(struct stm32f_usb_tx_pktbuf * tx,
						   uint8_t * src, int len)
{
	uint32_t * dst;
	int i;

	/* copy data to destination buffer */
	dst = (uint32_t *)STM32F_USB_PKTBUF_ADDR + (tx->addr / 2);
	for (i = 0; i < ((len + 1) / 2); i++) {
		*dst++ = src[0] | (src[1] << 8);
		src += 2;
	}

	tx->count = len;
}

static void __ep_stall(struct stm32f_usb_drv * drv, int ep_id)
{
	struct stm32f_usb * usb = STM32F_USB;
	struct stm32f_usb_ep * ep = &drv->ep[ep_id];

	set_ep_txstat(usb, ep_id, USB_TX_STALL);
	set_ep_rxstat(usb, ep_id, USB_RX_STALL);

	DCC_LOG1(LOG_TRACE, "ep_id=%d [STALLED]", ep_id);
	ep->state = EP_STALLED;
}


static int __ep_pkt_send(struct stm32f_usb_drv * drv, int ep_id)
{
	struct stm32f_usb_pktbuf * pktbuf = STM32F_USB_PKTBUF;
	struct stm32f_usb * usb = STM32F_USB;
	struct stm32f_usb_ep * ep = &drv->ep[ep_id];
	int len;

	len = MIN(ep->xfr_rem, ep->mxpktsz);

	DCC_LOG2(LOG_INFO, "ep_id=%d, len=%d", ep_id, len);

	if (len > 0) {
		__copy_to_pktbuf(&pktbuf[ep_id].tx, ep->xfr_ptr, len);
		pktbuf[ep_id].tx.count = len;
		ep->xfr_rem -= len;
		ep->xfr_ptr += len;
	}

	if ((ep->xfr_rem == 0) && (len != ep->mxpktsz)) {
		/* if we put all data into the TX packet buffer but the data
		 * didn't filled the whole packet, this is the last packet,
		 * otherwise we need to send a ZLP to finish the transaction */
		DCC_LOG1(LOG_INFO, "ep_id=%d [EP_IN_DATA_LAST]", ep_id);
		ep->state = EP_IN_DATA_LAST;
	} else {
		DCC_LOG1(LOG_INFO, "ep_id=%d [EP_IN_DATA]", ep_id);
		ep->state = EP_IN_DATA;
	}

	pktbuf[ep_id].tx.count = len;
	set_ep_txstat(usb, ep_id, USB_TX_VALID);

	return len;
}

static void __ep_zlp_send(struct stm32f_usb_drv * drv, int ep_id)
{
	struct stm32f_usb_pktbuf * pktbuf = STM32F_USB_PKTBUF;
	struct stm32f_usb * usb = STM32F_USB;

	pktbuf[ep_id].tx.count = 0;
	set_ep_txstat(usb, ep_id, USB_TX_VALID);
}



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


#if 0
	/* Configure the endpoints and allocate packet buffers */
	for (i = 0; i < cnt; i++) {
		int mxpktsz = epi[i].mxpktsz;
		uint32_t epr;

		/* set the endpoint descriptor pointer */
		drv->ep[i].desc = desc;
		/* set the endpoint rx/tx/setup callback */
		drv->ep[i].on_rx = epi[i].on_rx;
		drv->ep[i].xfr_buf = NULL;
		drv->ep[i].rx_max = 0;
		/* Set EP address */
		epr = USB_EA_SET(epi[i].addr);
		/* Allocate packet buffers */
		switch (epi[i].type)
		{
		case ENDPOINT_TYPE_CONTROL:
			epr |= USB_EP_CONTROL;
			/* allocate single buffers for TX and RX */
			DCC_LOG2(LOG_TRACE, "EP[%d]: CONTROL -> 0x%08x", i, desc);
			stack = pktbuf_tx_cfg(desc++, stack, mxpktsz);
			stack = pktbuf_rx_cfg(desc++, stack, mxpktsz);
			break;

		case ENDPOINT_TYPE_ISOCHRONOUS:
			epr |= USB_EP_ISO | USB_EP_DBL_BUF;
			/* allocate double buffers for TX or RX */
			DCC_LOG2(LOG_TRACE, "EP[%d]: CONTROL -> 0x%08x", i, desc);
			/* allocate double buffers for TX or RX */
			if ((epi[i].addr & USB_ENDPOINT_IN) == 0) {
				stack = pktbuf_rx_cfg(desc++, stack, mxpktsz);
				stack = pktbuf_rx_cfg(desc++, stack, mxpktsz);
			} else {
				stack = pktbuf_tx_cfg(desc++, stack, mxpktsz);
				stack = pktbuf_tx_cfg(desc++, stack, mxpktsz);
			}
			break;

		case ENDPOINT_TYPE_BULK:
			epr |= USB_EP_BULK | USB_EP_DBL_BUF;
			DCC_LOG2(LOG_TRACE, "EP[%d]: BULK -> 0x%08x", i, desc);
			/* allocate double buffers for TX or RX */
			if ((epi[i].addr & USB_ENDPOINT_IN) == 0) {
				stack = pktbuf_rx_cfg(desc++, stack, mxpktsz);
				stack = pktbuf_rx_cfg(desc++, stack, mxpktsz);
			} else {
				stack = pktbuf_tx_cfg(desc++, stack, mxpktsz);
				stack = pktbuf_tx_cfg(desc++, stack, mxpktsz);
			}
			break;

		case ENDPOINT_TYPE_INTERRUPT:
			epr |= USB_EP_INTERRUPT;
			/* allocate single buffers for TX or RX */
			DCC_LOG2(LOG_TRACE, "EP[%d]: CONTROL -> 0x%08x", i, desc);
			stack = pktbuf_tx_cfg(desc++, stack, mxpktsz);
			stack = pktbuf_rx_cfg(desc++, stack, mxpktsz);
			break;
		}

		usb->epr[i] = epr;
	}

	drv->ep_cnt = cnt;


	return 0;
#endif

int stm32f_usb_dev_connect(struct stm32f_usb_drv * drv)
{
	return 0;
}

int stm32f_usb_dev_disconnect(struct stm32f_usb_drv * drv)
{
	return 0;
}

/* Endpoint initialization
The first step to initialize an endpoint is to write appropriate values to the
ADDRn_TX/ADDRn_RX registers so that the USB peripheral finds the data to be
transmitted already available and the data to be received can be buffered. The EP_TYPE
bits in the USB_EPnR register must be set according to the endpoint type, eventually using
the EP_KIND bit to enable any special required feature. On the transmit side, the endpoint
must be enabled using the STAT_TX bits in the USB_EPnR register and COUNTn_TX must
be initialized. For reception, STAT_RX bits must be set to enable reception and
COUNTn_RX must be written with the allocated buffer size using the BL_SIZE and
NUM_BLOCK fields. Unidirectional endpoints, except Isochronous and double-buffered bulk
endpoints, need to initialize only bits and registers related to the supported direction. Once
the transmission and/or reception are enabled, register USB_EPnR and locations
ADDRn_TX/ADDRn_RX, COUNTn_TX/COUNTn_RX (respectively), should not be modified
by the application software, as the hardware can change their value on the fly. When the
data transfer operation is completed, notified by a CTR interrupt event, they can be
accessed again to re-enable a new operation. */

/* USB reset (RESET interrupt)
When this event occurs, the USB peripheral is put in the same conditions it is left by the
system reset after the initialization described in the previous paragraph: communication is
disabled in all endpoint registers (the USB peripheral will not respond to any packet). As a
response to the USB reset event, the USB function must be enabled, having as USB
address 0, implementing only the default control endpoint (endpoint address is 0 too). This
is accomplished by setting the Enable Function (EF) bit of the USB_DADDR register and
initializing the EP0R register and its related packet buffers accordingly. During USB
enumeration process, the host assigns a unique address to this device, which must be
written in the ADD[6:0] bits of the USB_DADDR register, and configures any other
necessary endpoint.
When a RESET interrupt is received, the application software is responsible to enable again
the default endpoint of USB function 0 within 10mS from the end of reset sequence which
triggered the interrupt. */

void stm32f_usb_dev_reset(struct stm32f_usb_drv * drv)
{
	struct stm32f_usb * usb = STM32F_USB;

	DCC_LOG(LOG_TRACE, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");

	/* set the btable address */
	usb->btable = 0x000;

	drv->ev->on_reset(drv->cl);

	/* Enable the device and set the address to 0 */
	usb->daddr = USB_EF + 0;

	/* Enable Correct transfer and other interrupts */
//	usb->cntr |= USB_CTRM | USB_PMAOVRM | USB_ERRM | USB_SUSPM;
	usb->cntr |= USB_CTRM;

	/* set the control endpoint state */
	drv->ctrl_st = CTRL_IDLE;
}

void stm32f_usb_dev_enumerate(struct stm32f_usb_drv * drv)
{
	struct stm32f_usb * usb = STM32F_USB;

	(void)usb;
}

/*
 * The CTR_RX event is serviced by first
   determining the transaction type (SETUP bit in the USB_EPnR register); the application
   software must clear the interrupt flag bit and get the number of received bytes reading the
   COUNTn_RX location inside the buffer description table entry related to the endpoint being
 processed. After the received data is processed, the application software should set the
STAT_RX bits to ‘11 (Valid) in the USB_EPnR, enabling further transactions. While the
STAT_RX bits are equal to ‘10 (NAK), any OUT request addressed to that endpoint is
NAKed, indicating a flow control condition: the USB host will retry the transaction until it
succeeds. It is mandatory to execute the sequence of operations in the above mentioned
order to avoid losing the notification of a second OUT transaction addressed to the same
endpoint following immediately the one which triggered the CTR interrupt.
 */



/* prepares to receive */
int stm32f_usb_ep_rx_setup(struct stm32f_usb_drv * drv, int ep_id,
		void * buf, unsigned int len)
{
	struct stm32f_usb_ep * ep;

	DCC_LOG1(LOG_TRACE, "====!!!!!!============= ep_id=%d", ep_id);

	ep = &drv->ep[ep_id];
	ep->xfr_buf = buf;

	return 0;
}

int stm32f_usb_dev_ep_stall(struct stm32f_usb_drv * drv, int ep_id)
{
	DCC_LOG1(LOG_TRACE, "ep_id=%d", ep_id);

	__ep_stall(drv, ep_id);

	return 0;
}

/* start sending */
int stm32f_usb_ep_tx_start(struct stm32f_usb_drv * drv, int ep_id,
		void * buf, unsigned int len)
{
	struct stm32f_usb_ep * ep;

	DCC_LOG1(LOG_TRACE, "ep_id=%d", ep_id);

	ep = &drv->ep[ep_id];
	ep->xfr_ptr = buf;
	ep->xfr_rem = len;

	return __ep_pkt_send(drv, ep_id);
}


int stm32f_usb_dev_ep_pkt_recv(struct stm32f_usb_drv * drv, int ep_id,
		void * buf, int len)
{
	struct stm32f_usb * usb = STM32F_USB;
	struct stm32f_usb_pktbuf * pktbuf = STM32F_USB_PKTBUF;
	struct stm32f_usb_rx_pktbuf * pkt;
	uint32_t epr;
	int cnt;
	int ea;

	epr = usb->epr[ep_id];
	ea = USB_EA_GET(epr);
	(void)ea;

	DCC_LOG1(LOG_TRACE, "ep_id=%d", ep_id);

	if (epr & USB_EP_DBL_BUF) {
		/* double buffer */
		/* select the descriptor according to the data toggle bit */
		pkt = &pktbuf[ep_id].dbrx[(epr & USB_DTOG_RX) ? 0: 1];
	} else {
		/* single buffer */
		pkt = &pktbuf[ep_id].rx;
	}

	DCC_LOG1(LOG_TRACE, "================= ep_id=%d", ep_id);

	cnt = MIN(pkt->count, len);

	/* Data received */
	__copy_from_pktbuf(buf, pkt, cnt);

	if ((epr & USB_EP_DBL_BUF) == 0) {
		/* free the out(rx) packet buffer */
		set_ep_rxstat(usb, ep_id, USB_RX_VALID);
	}

	DCC_LOG2(LOG_TRACE, "OUT EP%d, cnt=%d", ea, cnt);

	return cnt;
}


int stm32f_usb_dev_addr_set(struct stm32f_usb_drv * drv, unsigned int addr)
{
//	struct stm32f_usb * usb = STM32F_USB;

	DCC_LOG1(LOG_TRACE, "addr=%d [WAIT_STATUS_IN]", addr);
//	usb->daddr = addr | USB_EF;
	drv->ctrl_st = CTRL_WAIT_STATUS_IN;
//	drv->ctrl_st = CTRL_ADDR_SET;

	return 0;
}

int stm32f_usb_dev_ep_zlp_send(struct stm32f_usb_drv * drv, int ep_id)
{
	DCC_LOG1(LOG_TRACE, "ep_id=%d", ep_id);

	__ep_zlp_send(drv, ep_id);

	return 0;
}

void stm32f_usb_dev_ep_out(struct stm32f_usb_drv * drv, int ep_id)
{
	struct stm32f_usb * usb = STM32F_USB;
	struct stm32f_usb_ep * ep = &drv->ep[ep_id];
	uint32_t epr;


	epr = usb->epr[ep_id];
	if (epr & USB_EP_DBL_BUF) {
		/* prepare to receive on other buffer */
		set_ep_rxstat(usb, ep_id, USB_RX_VALID);
	}

	/* call class endpoint callback */
	ep->on_out(drv->cl, ep_id);
}

void stm32f_usb_dev_ep_in(struct stm32f_usb_drv * drv, int ep_id)
{
	struct stm32f_usb * usb = STM32F_USB;
	struct stm32f_usb_ep * ep = &drv->ep[ep_id];

	DCC_LOG1(LOG_TRACE, "================= ep_id=%d", ep_id);

	if (ep->state != EP_IN_DATA_LAST) {
		__ep_pkt_send(drv, ep_id);
	} else {
		set_ep_txstat(usb, ep_id, USB_TX_NAK);
		/* call class endpoint callback */
		ep->on_in(drv->cl, ep_id);
	}
}

/* FIXME: find another way of initializing the packet buffer addresses */
#define PKTBUF_BUF_BASE (8 * 8)
static unsigned int addr = PKTBUF_BUF_BASE;


int stm32f_usb_dev_ep_init(struct stm32f_usb_drv * drv, int ep_id,
		const usb_dev_ep_info_t * info)
{
	struct stm32f_usb_pktbuf * pktbuf = STM32F_USB_PKTBUF;
	unsigned int sz;
	struct stm32f_usb * usb = STM32F_USB;
	struct stm32f_usb_ep * ep;
	int mxpktsz = info->mxpktsz;

	DCC_LOG2(LOG_TRACE, "sp_id=%d mxpktsz=%d", ep_id, mxpktsz);

	ep = &drv->ep[ep_id];
	ep->mxpktsz = mxpktsz;
	ep->xfr_ptr = NULL;
	ep->xfr_rem = 0;
	ep->state = EP_IDLE;
	ep->on_ev = info->on_ev;

	DCC_LOG3(LOG_TRACE, "ep_id=%d addr=%d mxpktsz=%d",
			ep_id, info->addr & 0x7f, mxpktsz);

	/* clear the correct transfer bits */
	clr_ep_flag(usb, ep_id, USB_CTR_RX | USB_CTR_TX);
	set_ep_addr(usb, ep_id, info->addr & 0x7f);

	set_ep_rxstat(usb, ep_id, USB_RX_NAK);
	set_ep_txstat(usb, ep_id, USB_TX_NAK);

	switch (info->attr & 0x03) {
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
		if (info->addr & USB_ENDPOINT_IN) {
			sz = pktbuf_tx_cfg(&pktbuf[ep_id].dbtx[0], addr, mxpktsz);
			addr += sz;
			sz = pktbuf_tx_cfg(&pktbuf[ep_id].dbtx[1], addr, mxpktsz);
			addr += sz;
//			set_ep_txstat(usb, ep_id, USB_TX_VALID);
		} else {
			sz = pktbuf_rx_cfg(&pktbuf[ep_id].dbrx[0], addr, mxpktsz);
			addr += sz;
			sz = pktbuf_rx_cfg(&pktbuf[ep_id].dbrx[1], addr, mxpktsz);
			addr += sz;
			clr_status_out(usb, ep_id);
			set_ep_rxstat(usb, ep_id, USB_RX_VALID);
		}
		break;

	case ENDPOINT_TYPE_INTERRUPT:
		set_ep_type(usb, ep_id, USB_EP_INTERRUPT);
		if (info->addr & USB_ENDPOINT_IN) {
			sz = pktbuf_tx_cfg(&pktbuf[ep_id].tx, addr, mxpktsz);
			addr += sz;
//			set_ep_txstat(usb, ep_id, USB_TX_VALID);
		} else {
			sz = pktbuf_rx_cfg(&pktbuf[ep_id].rx, addr, mxpktsz);
			addr += sz;
			clr_status_out(usb, ep_id);
			set_ep_rxstat(usb, ep_id, USB_RX_VALID);
		}
		break;
	}

	DCC_LOG1(LOG_TRACE, "epr=0x%04x...", usb->epr[ep_id]);

//	clr_status_out(usb, ep_id);

	return 0;
}

int stm32f_usb_dev_ep0_init(struct stm32f_usb_drv * drv,
		const usb_dev_ep_info_t * info,
		void * ctr_buf, int buf_len)
{
	/* packet buffer  */
	struct stm32f_usb_pktbuf * pktbuf = STM32F_USB_PKTBUF;
	struct stm32f_usb * usb = STM32F_USB;
	struct stm32f_usb_ep * ep;
	int mxpktsz = info->mxpktsz;
	unsigned int sz;

	DCC_LOG1(LOG_TRACE, "mxpktsz=%d", mxpktsz);
	DCC_LOG1(LOG_TRACE, "ctr_buf=0x%08x", ctr_buf);

	ep = &drv->ep[0];
	ep->mxpktsz = mxpktsz;
	ep->xfr_buf = (uint8_t *)ctr_buf;
	ep->xfr_buf_len = buf_len;
	ep->state = EP_IDLE;
	ep->on_setup = info->on_setup;

	/* clear the correct transfer bits */
	clr_ep_flag(usb, 0, USB_CTR_RX | USB_CTR_TX);
	set_ep_type(usb, 0, USB_EP_CONTROL);
	set_ep_txstat(usb, 0, USB_TX_NAK);

	/* reset packet buffer address pointer */
	addr = PKTBUF_BUF_BASE;

	/* allocate single buffers for TX and RX */
	sz = pktbuf_tx_cfg(&pktbuf[0].tx, addr, mxpktsz);
	addr += sz;
	sz = pktbuf_rx_cfg(&pktbuf[0].rx, addr, mxpktsz);
	addr += sz;

	clr_status_out(usb, 0);
	set_ep_rxstat(usb, 0, USB_RX_VALID);

	DCC_LOG1(LOG_TRACE, "epr=0x%04x...", usb->epr[0]);

	return 0;
}

void stm32f_usb_dev_ep0_out(struct stm32f_usb_drv * drv)
{
	struct stm32f_usb * usb = STM32F_USB;
	struct stm32f_usb_ep * ep = &drv->ep[0];
	struct stm32f_usb_pktbuf * pktbuf = STM32F_USB_PKTBUF;
	int cnt;
	int n;

	if (ep->state == EP_WAIT_STATUS_OUT) {
		ep->state = EP_IDLE;
		DCC_LOG(LOG_INFO, "EP0 OUT [IDLE] >>>>>>>> SETUP END");
		set_ep_rxstat(usb, 0, USB_RX_VALID);
		return;
	}

	if ((ep->state == EP_IN_DATA)|| (ep->state == EP_IN_DATA_LAST)) {
		ep->state = EP_STALLED;
		DCC_LOG(LOG_TRACE, "EP0 OUT [STALLED]");
		set_ep_rxstat(usb, 0, USB_RX_STALL);
		set_ep_txstat(usb, 0, USB_TX_STALL);
		return;
	}

		//	cnt = MIN(rx->count, max);

	cnt = pktbuf[0].rx.count;
	n = MIN(cnt, ep->xfr_rem);

	__copy_from_pktbuf(ep->xfr_ptr, &pktbuf[0].rx, n);

	ep->xfr_ptr += n;
	ep->xfr_rem -= n;

	if (ep->state == EP_OUT_DATA_LAST) {
		ep->state = EP_WAIT_STATUS_IN;
		DCC_LOG1(LOG_INFO, "EP0 cnt=%d [EP_WAIT_STATUS_IN]", cnt);
		__ep_zlp_send(drv, 0);
		return;
	}

	if (ep->xfr_rem < ep->mxpktsz) {
		/* last transfer */
		ep->state = EP_OUT_DATA_LAST;
		DCC_LOG1(LOG_INFO, "EP0 cnt=%d [OUT_DATA_LAST]", cnt);
	}
	set_ep_rxstat(usb, 0, USB_RX_VALID);

	DCC_LOG1(LOG_INFO, "EP0 cnt=%d", cnt);
}

void stm32f_usb_dev_ep0_in(struct stm32f_usb_drv * drv)
{
	struct stm32f_usb * usb = STM32F_USB;
	struct stm32f_usb_ep * ep = &drv->ep[0];
	void * dummy = NULL;

	DCC_LOG(LOG_MSG, "EP0 IN");

	if (ep->state == EP_WAIT_STATUS_IN) {
		struct usb_request * req = &drv->req;

		if (((req->request << 8) | req->type) == STD_SET_ADDRESS) {
			usb->daddr = req->value | USB_EF;
			DCC_LOG(LOG_INFO, "address set!");
		}
		set_ep_rxstat(usb, 0, USB_RX_STALL);
		set_ep_txstat(usb, 0, USB_TX_STALL);
		ep->on_setup(drv->cl, req, dummy);
		ep->state = EP_IDLE;
		DCC_LOG(LOG_INFO, ">>>>>>>> SETUP END");
		return;
	}

	if (ep->state == EP_IN_DATA_LAST) {
		ep->state = EP_WAIT_STATUS_OUT;
		DCC_LOG(LOG_INFO, "EP0 [WAIT_STATUS_OUT]");
		set_ep_rxstat(usb, 0, USB_RX_VALID);
		return;
	}

	__ep_pkt_send(drv, 0);

	if (ep->state == EP_IN_DATA) {
		set_ep_rxstat(usb, 0, USB_RX_STALL);
	}
}

void stm32f_usb_dev_ep0_setup(struct stm32f_usb_drv * drv) {
	struct stm32f_usb_pktbuf * pktbuf = STM32F_USB_PKTBUF;
	struct usb_request * req = &drv->req;
	struct stm32f_usb_ep * ep = &drv->ep[0];
	struct stm32f_usb * usb = STM32F_USB;
	int cnt;
	int len;

	DCC_LOG(LOG_INFO, "SETUP START <<<<<<<<<<<<<<<");

	cnt = pktbuf[0].rx.count;

#if ENABLE_PEDANTIC_CHECK
	if (cnt != 8) {
		__ep_stall(drv, 0);
		DCC_LOG1(LOG_ERROR, "cnt(%d) != 8 [ERROR]", cnt);
		return;
	}
#endif

	/* copy data from packet buffer */
	__copy_from_pktbuf(req, &pktbuf[0].rx, cnt);

	/* No-Data control SETUP transaction */
	if (req->length == 0) {
		__ep_zlp_send(drv, 0);
		DCC_LOG(LOG_INFO, "EP0 [WAIT_STATUS_IN] no data Dev->Host");
		ep->state = EP_WAIT_STATUS_IN;
		return;
	}

	if (req->type & 0x80) {
		/* Control Read SETUP transaction (IN Data Phase) */

		DCC_LOG(LOG_INFO, "EP0 [SETUP] IN Dev->Host");
		ep->xfr_ptr = NULL;
		len = ep->on_setup(drv->cl, req, (void *)&ep->xfr_ptr);
#if ENABLE_PEDANTIC_CHECK
		if (len < 0) {
			__ep_stall(drv, 0);
			DCC_LOG(LOG_TRACE, "EP0 [STALLED] len < 0");
			return;
		}
		if (ep->tx_buf == NULL) {
			__ep_stall(drv, 0);
			DCC_LOG(LOG_TRACE, "EP0 [STALLED] tx_buf == NULL");
			return;
		}
#endif
		ep->xfr_rem = MIN(req->length, len);
		DCC_LOG1(LOG_INFO, "EP0 data lenght = %d", ep->xfr_rem);
		__ep_pkt_send(drv, 0);
		/* While enabling the last data stage, the opposite direction should
			 * be set to NAK, so that, if the host reverses the transfer direction
			 * (to perform the status stage) immediately, it is kept waiting for
			 * the completion of the control operation. If the control operation
			 * completes successfully, the software will change NAK to VALID,
			 * otherwise to STALL.
			 * */
		if (ep->state == EP_IN_DATA) {
			/* A USB device can determine the number and direction of data stages
			 * by interpreting the data transferred in the SETUP stage, and is
			 * required to STALL the transaction in the case of errors. To do
			 * so, at all data stages before the last, the unused direction should
			 * be set to STALL, so that, if the host reverses the transfer
			 * direction too soon, it gets a STALL as a status stage.
			 * */
			set_ep_rxstat(usb, 0, USB_RX_STALL);
		}

	} else {
		/* Control Write SETUP transaction (OUT Data Phase) */

		ep->xfr_ptr = ep->xfr_buf;
		ep->xfr_rem = req->length;

		DCC_LOG1(LOG_INFO, "xfr_ptr=0x%08x", ep->xfr_ptr);

		if (ep->xfr_rem > ep->xfr_buf_len) {
			ep->xfr_rem = ep->xfr_buf_len;
			DCC_LOG(LOG_ERROR, "transfer to large to fit the buffer!");
		}

		if (ep->xfr_rem < ep->mxpktsz) {
			/* last and only transfer */
			ep->state = EP_OUT_DATA_LAST;
			DCC_LOG(LOG_INFO, "EP0 [OUT_DATA_LAST] OUT Host->Dev!!!!");

		} else {
			ep->state = EP_OUT_DATA;
			DCC_LOG(LOG_INFO, "EP0 [OUT_DATA] OUT Host->Dev!!!!");
		}

		set_ep_rxstat(usb, 0, USB_RX_VALID);
	}
}

/*
 * Arguments:
 *
 *   drv: pointer to the driver runtime structure.
 *
 *   ep: list of endpoint configuration information. The first item on this list
 *       must be the endpoint 0.
 *
 *  cnt: number of items in the list.
 */

int stm32f_usb_dev_init(struct stm32f_usb_drv * drv, usb_class_t * cl,
		const struct usb_descriptor_device * dsc,
		const struct usb_class_events * ev)
{
	struct stm32f_usb * usb = STM32F_USB;
//	uint32_t stack = STM32F_USB_PKTBUF_SIZE;
//	struct stm32f_usb_pktbuf * desc;
	
//	desc = (struct stm32f_usb_pktbuf *)STM32F_USB_PKTBUF;
	drv->cl = cl;
	drv->dsc_dev = dsc;
	drv->ev = ev;
	
	DCC_LOG1(LOG_TRACE, "ev=0x%08x", drv->ev);


	stm32f_usb_power_off(usb);

	udelay(1000);

	/* Initialize IO pins */
	stm32f_usb_io_init();

	stm32f_usb_power_on(usb);

	/* Enable Reset, SOF  and Wakeup interrupts */
//	usb->cntr = USB_WKUP | USB_SUSPM | USB_RESETM;
	usb->cntr = USB_WKUP | USB_RESETM;

	return 0;
}


/* Private USB device driver data */
struct stm32f_usb_drv stm32f_usb_drv0;


/* USB high priority ISR */
void stm32f_can1_tx_usb_hp_isr(void)
{
	struct stm32f_usb_drv * drv = &stm32f_usb_drv0;
	struct stm32f_usb * usb = STM32F_USB;
	uint32_t sr = usb->istr;

	DCC_LOG1(LOG_TRACE, "SR 0x%04x", sr);

	if (sr & USB_CTR) {
		uint32_t epr;
		int ep_id;

		/* clear interrupt */
		usb->istr = sr & ~USB_CTR;

		ep_id = USB_EP_ID_GET(sr);

		DCC_LOG1(LOG_TRACE, "CTR ep_id=%d", ep_id);

		epr = usb->epr[ep_id];

		if (epr & USB_CTR_RX) {
			/* OUT */
			clr_ep_flag(usb, ep_id, USB_CTR_RX);
			stm32f_usb_dev_ep_out(drv, ep_id);
		}

		if (epr & USB_CTR_TX) {
			/* IN */
			clr_ep_flag(usb, ep_id, USB_CTR_TX);
			stm32f_usb_dev_ep_in(drv, ep_id);
		}
	}

}


/* USB low priority ISR */
void stm32f_can1_rx0_usb_lp_isr(void)
{
	struct stm32f_usb_drv * drv = &stm32f_usb_drv0;
	struct stm32f_usb * usb = STM32F_USB;
	uint32_t sr = usb->istr;

	if (sr & USB_SOF) {
		usb->istr = sr & ~USB_SOF;
		DCC_LOG(LOG_INFO, "SOF");
	}

	if (sr & USB_RESET) {
		usb->istr = sr & ~USB_RESET;
		DCC_LOG(LOG_INFO, "RESET");
		stm32f_usb_dev_reset(drv);
	}

	if (sr & USB_SUSP) {
		usb->istr = sr & ~USB_SUSP;
		DCC_LOG(LOG_TRACE, "SUSP");
	}

	if (sr & USB_WKUP) {
		usb->istr = sr & ~USB_WKUP;
		DCC_LOG(LOG_TRACE, "WKUP");
	}

	if (sr & USB_ERR) {
		usb->istr = sr & ~USB_ERR;
		DCC_LOG(LOG_TRACE, "ERR");
	}

	if (sr & USB_PMAOVR) {
		usb->istr = sr & ~USB_PMAOVR;
		DCC_LOG(LOG_TRACE, "PMAOVR");
	}

	if (sr & USB_CTR) {
		uint32_t epr;
		int ep_id;

		/* clear interrupt */
		usb->istr = sr & ~USB_CTR;

		ep_id = USB_EP_ID_GET(sr);

		DCC_LOG1(LOG_INFO, "CTR ep_id=%d", ep_id);

		epr = usb->epr[ep_id];

		if (ep_id == 0) {
			/* Service control point */
			set_ep_rxstat(usb, ep_id, USB_RX_NAK);
			set_ep_txstat(usb, ep_id, USB_TX_NAK);

			if (((sr & USB_DIR) == 0) || (epr & USB_CTR_TX)) {
				/* DIR = 0 -> IN */
				clr_ep_flag(usb, 0, USB_CTR_TX);
				stm32f_usb_dev_ep0_in(drv);
			} else {
				/* DIR = 1 -> OUT/SETUP */
				clr_ep_flag(usb, 0, USB_CTR_RX);
				if (epr & USB_SETUP) {
					/* SETUP */
					stm32f_usb_dev_ep0_setup(drv);
				} else {
					/* OUT */
					stm32f_usb_dev_ep0_out(drv);
				}
			}
			return;
		}

		if (epr & USB_CTR_RX) {
			/* OUT */
			clr_ep_flag(usb, ep_id, USB_CTR_RX);
			stm32f_usb_dev_ep_out(drv, ep_id);
		}

		if (epr & USB_CTR_TX) {
			/* IN */
			clr_ep_flag(usb, ep_id, USB_CTR_TX);
			stm32f_usb_dev_ep_in(drv, ep_id);
		}

	}
}

/* USB device operations */
const struct usb_dev_ops stm32f_usb_ops = {
	.dev_init = (usb_dev_init_t)stm32f_usb_dev_init,
	.connect = (usb_dev_connect_t)stm32f_usb_dev_connect,
	.disconnect = (usb_dev_disconnect_t)stm32f_usb_dev_disconnect,
	.ep_rx_setup = (usb_dev_ep_rx_setup_t)stm32f_usb_ep_rx_setup,
	.ep_tx_start= (usb_dev_ep_tx_start_t)stm32f_usb_ep_tx_start,
	.ep0_init = (usb_dev_ep0_init_t)stm32f_usb_dev_ep0_init,
	.ep_init = (usb_dev_ep_init_t)stm32f_usb_dev_ep_init,
	.ep_stall = (usb_dev_ep_stall_t)stm32f_usb_dev_ep_stall,
	.addr_set = (usb_dev_addr_set_t)stm32f_usb_dev_addr_set,
	.ep_zlp_send = (usb_dev_ep_zlp_send_t)stm32f_usb_dev_ep_zlp_send,
	.ep_pkt_recv = (usb_dev_ep_pkt_recv_t)stm32f_usb_dev_ep_pkt_recv
};

/* USB device driver */
const struct usb_dev stm32f_usb_dev = {
	.priv = (void *)&stm32f_usb_drv0,
	.op = &stm32f_usb_ops
};

