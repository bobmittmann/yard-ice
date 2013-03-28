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

#include <sys/dcclog.h>

#ifdef STM32F_OTG_FS

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
struct stm32f_otg_ep {

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

#define OTG_FS_DRIVER_EP_MAX 4



/* USB Device runtime driver data */
struct stm32f_otg_drv {
	struct stm32f_otg_ep ep[OTG_FS_DRIVER_EP_MAX];
	usb_class_t * cl;
	const struct usb_class_events * ev;
	struct usb_request req;
	int state;
};


#define DFIFIO_ADDR (STM32F_BASE_OTG_FS +  0x20000)
#define DFIFIO (uint32_t *)DFIFIO_ADDR

struct ep_tx_ctrl {
	uint8_t ep;
	uint8_t pkts;
	uint16_t max;
	volatile uint32_t len;
	uint8_t * ptr;
};

struct ep_rx_ctrl {
	uint8_t ep;
	uint8_t rem;
	volatile uint32_t len;
	uint32_t data;
	uint32_t tmr;
};

static void __copy_from_pktbuf(void * ptr,
							uint32_t * pop,
							unsigned int cnt)
{
	uint8_t * dst = (uint8_t *)ptr;
	uint32_t data;
	int i;

	/* pop data from the fifo and copy to destination buffer */
	for (i = 0; i < (cnt + 3) / 2; i++) {
		data = *pop;
		*dst++ = data;
		*dst++ = data >> 8;
		*dst++ = data >> 16;
		*dst++ = data >> 24;
	}
}


#if 0
static void ep_tx_ctrl_init(struct ep_tx_ctrl * ctrl,
							unsigned int ep, unsigned int max)
{
	ctrl->ep = ep;
	ctrl->max = max;
	ctrl->len = 0;
	ctrl->ptr = NULL;
}

static void ep_rx_ctrl_init(struct ep_rx_ctrl * ctrl, unsigned int ep)
{
	ctrl->ep = ep;
	ctrl->rem = 0;
	ctrl->len = 0;
	ctrl->data = 0;
}


#define STM32F_USB_PKTBUF ((struct stm32f_otg_pktbuf *)STM32F_USB_PKTBUF_ADDR)

static void __copy_from_pktbuf(void * ptr,
							struct stm32f_otg_rx_pktbuf * rx,
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

static void __copy_to_pktbuf(struct stm32f_otg_tx_pktbuf * tx,
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

static void __ep_stall(struct stm32f_otg_drv * drv, int ep_id)
{
	struct stm32f_usb * usb = STM32F_USB;
	struct stm32f_otg_ep * ep = &drv->ep[ep_id];

	set_ep_txstat(usb, ep_id, USB_TX_STALL);
	set_ep_rxstat(usb, ep_id, USB_RX_STALL);

	DCC_LOG1(LOG_TRACE, "ep_id=%d [STALLED]", ep_id);
	ep->state = EP_STALLED;
}


static int __ep_pkt_send(struct stm32f_otg_drv * drv, int ep_id)
{
	struct stm32f_otg_pktbuf * pktbuf = STM32F_USB_PKTBUF;
	struct stm32f_usb * usb = STM32F_USB;
	struct stm32f_otg_ep * ep = &drv->ep[ep_id];
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

static void __ep_zlp_send(struct stm32f_otg_drv * drv, int ep_id)
{
	struct stm32f_otg_pktbuf * pktbuf = STM32F_USB_PKTBUF;
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
#endif

	/* Configure EP0 to receive Setup packets */
static void __ep0_out_start(struct stm32f_otg_fs * otg_fs)
{
	/* Prepare to receive */
	otg_fs->outep[0].doeptsiz = OTG_FS_STUPCNT_SET(3) |
		OTG_FS_PKTCNT_SET(1) | OTG_FS_XFRSIZ_SET(8 * 3);
	/* EP enable */
	otg_fs->outep[0].doepctl |= OTG_FS_EPENA | OTG_FS_CNAK;
}

static void __ep0_zlp_send(struct stm32f_otg_fs * otg_fs)
{
	otg_fs->inep[0].dieptsiz = OTG_FS_PKTCNT_SET(1) | OTG_FS_XFRSIZ_SET(0);
	otg_fs->inep[0].diepctl |= OTG_FS_EPENA | OTG_FS_CNAK;
	__ep0_out_start(otg_fs);
}

void stm32f_otg_dev_reset(struct stm32f_otg_drv * drv)
{
	struct stm32f_otg_fs * otg_fs = STM32F_OTG_FS;
	int i;

	DCC_LOG(LOG_TRACE, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");

	/* Clear the Remote Wake-up Signaling */
	otg_fs->dctl &= ~OTG_FS_RWUSIG;

	/* Flush the Tx FIFO */
	stm32f_otg_fs_txfifo_flush(otg_fs, 0x10);
	/* Flush the Rx FIFO */
	stm32f_otg_fs_rxfifo_flush(otg_fs);

	/* Clear all pending interrupts */
	otg_fs->daint = 0xffffffff;
	for (i = 0; i < 4; i++) {
		otg_fs->inep[i].diepint = 0xff;
		otg_fs->outep[i].doepint = 0xff;
	}

	/* Reset Device Address */
	otg_fs->dcfg &= ~OTG_FS_DAD_MSK;

	/* Endpoint initialization on USB reset */

	/* 1. Set the NAK bit for all OUT endpoints
	   – SNAK = 1 in OTG_FS_DOEPCTLx (for all OUT endpoints) */
	for (i = 0; i < 4; i++)
		otg_fs->outep[i].doepctl = OTG_FS_SNAK;

	/* 2. Unmask the following interrupt bits
	   – INEP0 = 1 in OTG_FS_DAINTMSK (control 0 IN endpoint)
	   – OUTEP0 = 1 in OTG_FS_DAINTMSK (control 0 OUT endpoint)
	   – STUP = 1 in DOEPMSK
	   – XFRC = 1 in DOEPMSK
	   – XFRC = 1 in DIEPMSK
	   – TOC = 1 in DIEPMSK */
	otg_fs->daintmsk = OTG_FS_IEPM0 | OTG_FS_OEPM0;
	otg_fs->doepmsk = OTG_FS_STUPM | OTG_FS_XFRCM | OTG_FS_EPDM;
//	otg_fs->diepmsk = OTG_FS_XFRCM | OTG_FS_TOM | OTG_FS_EPDM;
//	otg_fs->diepmsk = OTG_FS_INEPNEM | OTG_FS_INEPNMM | OTG_FS_ITTXFEMSK |
	otg_fs->diepmsk = OTG_FS_TOM | OTG_FS_EPDM | OTG_FS_XFRCM;

	/* 3. Set up the Data FIFO RAM for each of the FIFOs
	   – Program the OTG_FS_GRXFSIZ register, to be able to receive
	   control OUT data and setup data. If thresholding is not enabled,
	   at a minimum, this must be equal to 1 max packet size of
	   control endpoint 0 + 2 Words (for the status of the control OUT
	   data packet) + 10 Words (for setup packets).
	   - Program the OTG_FS_TX0FSIZ register (depending on the FIFO number
	   chosen) to be able to transmit control IN data. At a minimum, this
	   must be equal to 1 max packet size of control endpoint 0. */

	drv->ev->on_reset(drv->cl);


	otg_fs->gintmsk |=  OTG_FS_IEPINTM | OTG_FS_OEPINTM |
		OTG_FS_IISOIXFRM | OTG_FS_IISOOXFRM | OTG_FS_RXFLVLM;

#if 0
	struct stm32f_usb * usb = STM32F_USB;

	DCC_LOG(LOG_TRACE, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");

	/* set the btable address */
	usb->btable = 0x000;

	drv->ev->on_reset(drv->cl);

	/* Enable the device and set the address to 0 */
	usb->daddr = USB_EF + 0;

	/* Enable Correct transfer interrupts */
	usb->cntr |= USB_CTRM;
#endif
}


/* prepares to receive */
int stm32f_otg_ep_rx_setup(struct stm32f_otg_drv * drv, int ep_id,
		void * buf, unsigned int len)
{
#if 0
	struct stm32f_otg_ep * ep;

	DCC_LOG1(LOG_TRACE, "====!!!!!!============= ep_id=%d", ep_id);

	ep = &drv->ep[ep_id];
	ep->xfr_buf = buf;
#endif
	return 0;
}

int stm32f_otg_dev_ep_stall(struct stm32f_otg_drv * drv, int ep_id)
{
	DCC_LOG1(LOG_TRACE, "ep_id=%d", ep_id);
#if 0
	__ep_stall(drv, ep_id);
#endif
	return 0;
}

/* start sending */
int stm32f_otg_dev_ep_tx_start(struct stm32f_otg_drv * drv, int ep_id,
		void * buf, unsigned int len)
{
#if 0
	struct stm32f_otg_ep * ep;

	DCC_LOG1(LOG_TRACE, "ep_id=%d", ep_id);

	ep = &drv->ep[ep_id];
	ep->xfr_ptr = buf;
	ep->xfr_rem = len;

	return __ep_pkt_send(drv, ep_id);
#endif
	return 0;
}


int stm32f_otg_dev_ep_pkt_recv(struct stm32f_otg_drv * drv, int ep_id,
		void * buf, int len)
{
#if 0
	struct stm32f_usb * usb = STM32F_USB;
	struct stm32f_otg_pktbuf * pktbuf = STM32F_USB_PKTBUF;
	struct stm32f_otg_rx_pktbuf * pkt;
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
#endif
	return 0;
}

int stm32f_otg_dev_ep_zlp_send(struct stm32f_otg_drv * drv, int ep_id)
{
	DCC_LOG1(LOG_TRACE, "ep_id=%d", ep_id);
#if 0
	__ep_zlp_send(drv, ep_id);
#endif

	return 0;
}

void stm32f_otg_dev_ep_out(struct stm32f_otg_drv * drv, int ep_id)
{
#if 0
	struct stm32f_usb * usb = STM32F_USB;
	struct stm32f_otg_ep * ep = &drv->ep[ep_id];
	uint32_t epr;


	epr = usb->epr[ep_id];
	if (epr & USB_EP_DBL_BUF) {
		/* prepare to receive on other buffer */
		set_ep_rxstat(usb, ep_id, USB_RX_VALID);
	}

	/* call class endpoint callback */
	ep->on_out(drv->cl, ep_id);
#endif
}

void stm32f_otg_dev_ep_in(struct stm32f_otg_drv * drv, int ep_id)
{
#if 0
	struct stm32f_usb * usb = STM32F_USB;
	struct stm32f_otg_ep * ep = &drv->ep[ep_id];

	DCC_LOG1(LOG_TRACE, "================= ep_id=%d", ep_id);

	if (ep->state != EP_IN_DATA_LAST) {
		__ep_pkt_send(drv, ep_id);
	} else {
		set_ep_txstat(usb, ep_id, USB_TX_NAK);
		/* call class endpoint callback */
		ep->on_in(drv->cl, ep_id);
	}
#endif
}

int fifo_addr = 0;

int stm32f_otg_dev_ep_init(struct stm32f_otg_drv * drv, int ep_id,
		const usb_dev_ep_info_t * info)
{
#if 0
	struct stm32f_otg_pktbuf * pktbuf = STM32F_USB_PKTBUF;
	unsigned int sz;
	struct stm32f_usb * usb = STM32F_USB;
	struct stm32f_otg_ep * ep;
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
#endif
	return 0;
}

int stm32f_otg_dev_ep0_init(struct stm32f_otg_drv * drv,
		const usb_dev_ep_info_t * info,
		void * ctr_buf, int buf_len)
{
	struct stm32f_otg_fs * otg_fs = STM32F_OTG_FS;
	uint32_t siz;

	/* reset packet buffer address pointer */
	fifo_addr = 0;
	siz = 256;
	otg_fs->grxfsiz = siz / 4;

	/* EP0 TX fifo memory allocation */
	fifo_addr += siz;
	siz = info->mxpktsz;
	otg_fs->dieptxf0 = OTG_FS_TX0FD_SET(siz / 4) |
		OTG_FS_TX0FSA_SET(fifo_addr / 4);

	/*  4. Program the following fields in the endpoint-specific registers
	   for control OUT endpoint 0 to receive a SETUP packet
	   – STUPCNT = 3 in OTG_FS_DOEPTSIZ0 (to receive up to 3 back-to-back
	   SETUP packets)
	   At this point, all initialization required to receive SETUP packets
	   is done. */
	otg_fs->outep[0].doeptsiz = OTG_FS_STUPCNT_SET(3) |
		OTG_FS_PKTCNT_SET(1) | OTG_FS_XFRSIZ_SET(3 * (8));

	/* Initialize EP0 */
	otg_fs->inep[0].diepctl = OTG_FS_TXFNUM_SET(0);

	DCC_LOG2(LOG_TRACE, "   RX: addr=%04x size=%d",
			 0, otg_fs->grxfsiz * 4);

	DCC_LOG2(LOG_TRACE, "TX[0]: addr=%04x size=%d",
			 OTG_FS_TX0FSA_GET(otg_fs->dieptxf0) * 4,
			 OTG_FS_TX0FD_GET(otg_fs->dieptxf0) * 4);

#if 0
	/* packet buffer  */
	struct stm32f_otg_pktbuf * pktbuf = STM32F_USB_PKTBUF;
	struct stm32f_usb * usb = STM32F_USB;
	struct stm32f_otg_ep * ep;
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
#endif
	return 0;
}

void stm32f_otg_dev_ep0_out(struct stm32f_otg_drv * drv, int rxlen)
{
	struct stm32f_otg_fs * otg_fs = STM32F_OTG_FS;
	struct stm32f_otg_ep * ep = &drv->ep[0];
	int cnt;
	int n;

#if 0
	struct stm32f_usb * usb = STM32F_USB;
	struct stm32f_otg_ep * ep = &drv->ep[0];
	struct stm32f_otg_pktbuf * pktbuf = STM32F_USB_PKTBUF;
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
#endif

	cnt = rxlen;
	n = MIN(cnt, ep->xfr_rem);

	__copy_from_pktbuf(ep->xfr_ptr, &otg_fs->dfifo[0].pop, n);

	ep->xfr_ptr += n;
	ep->xfr_rem -= n;

#if 0
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
#endif
}

void stm32f_otg_dev_ep0_in(struct stm32f_otg_drv * drv)
{
	struct stm32f_otg_fs * otg_fs = STM32F_OTG_FS;
	struct stm32f_otg_ep * ep = &drv->ep[0];
	void * dummy = NULL;

	if (ep->state == EP_WAIT_STATUS_IN) {
		struct usb_request * req = &drv->req;

		if (((req->request << 8) | req->type) == STD_SET_ADDRESS) {
			DCC_LOG(LOG_TRACE, "address set!");
			stm32f_otg_fs_addr_set(otg_fs, req->value);
		}
		ep->on_setup(drv->cl, req, dummy);
		ep->state = EP_IDLE;
		DCC_LOG(LOG_TRACE, ">>>>>>>> SETUP END");
		return;
	}


#if 0
	struct stm32f_usb * usb = STM32F_USB;
	struct stm32f_otg_ep * ep = &drv->ep[0];
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
#endif

}

void stm32f_otg_dev_ep0_setup(struct stm32f_otg_drv * drv)
{
	struct stm32f_otg_fs * otg_fs = STM32F_OTG_FS;
	struct usb_request * req = &drv->req;
	struct stm32f_otg_ep * ep = &drv->ep[0];

	DCC_LOG(LOG_TRACE, "SETUP START <<<<<<<<<<<<<<<");

	DCC_LOG2(LOG_TRACE, "req len=%d type=%d", req->length, req->type);


	/* No-Data control SETUP transaction */
	if (req->length == 0) {
		struct usb_request * req = &drv->req;
		void * dummy = NULL;

		if (((req->request << 8) | req->type) == STD_SET_ADDRESS) {
			DCC_LOG(LOG_TRACE, "address set!");
			stm32f_otg_fs_addr_set(otg_fs, req->value);
		}
		ep->on_setup(drv->cl, req, dummy);
		ep->state = EP_IDLE;
		__ep0_zlp_send(otg_fs);

		DCC_LOG(LOG_TRACE, ">>>>>>>> SETUP END");
		return;
	}

#if 0
	struct stm32f_otg_pktbuf * pktbuf = STM32F_USB_PKTBUF;
	struct usb_request * req = &drv->req;
	struct stm32f_otg_ep * ep = &drv->ep[0];
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
#endif
}

void otg_fs_on_enum_done(struct stm32f_otg_fs * otg_fs)
{
	uint32_t diepctl;
	uint32_t dsts;

	/* Endpoint initialization on enumeration completion
	   1. On the Enumeration Done interrupt (ENUMDNE in OTG_FS_GINTSTS),
	   read the OTG_FS_DSTS register to determine the enumeration speed. */
	dsts = otg_fs->dsts;

	/* 2. Program the MPSIZ field in OTG_FS_DIEPCTL0 to set the maximum
	   packet size. This step configures control endpoint 0. The maximum
	   packet size for a control endpoint depends on the enumeration speed. */

	diepctl = otg_fs->inep[0].diepctl;
	diepctl &= ~OTG_FS_MPSIZ_MSK;
	if ((dsts & OTG_FS_ENUMSPD_MSK) == OTG_FS_ENUMSPD_FULL) {
		diepctl |= OTG_FS_MPSIZ_64;
		DCC_LOG(LOG_INFO, "USB full speed");
	} else {
		diepctl |= OTG_FS_MPSIZ_8;
		DCC_LOG(LOG_INFO, "USB low speed");
	}
	otg_fs->inep[0].diepctl = diepctl;

	/* At this point, the device is ready to receive SOF packets and is
	   configured to perform control transfers on control endpoint 0. */

	/*  Clear global IN NAK */
	otg_fs->dctl |= OTG_FS_CGINAK;

}

/* Private USB device driver data */
struct stm32f_otg_drv stm32f_otg_fs_drv0;

#if 0

/* USB high priority ISR */
void stm32f_can1_tx_usb_hp_isr(void)
{
	struct stm32f_otg_drv * drv = &stm32f_otg_drv0;
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
			stm32f_otg_dev_ep_out(drv, ep_id);
		}

		if (epr & USB_CTR_TX) {
			/* IN */
			clr_ep_flag(usb, ep_id, USB_CTR_TX);
			stm32f_otg_dev_ep_in(drv, ep_id);
		}
	}

}

/* USB low priority ISR */
void stm32f_can1_rx0_usb_lp_isr(void)
{
	struct stm32f_otg_drv * drv = &stm32f_otg_drv0;
	struct stm32f_usb * usb = STM32F_USB;
	uint32_t sr = usb->istr;

	if (sr & USB_SOF) {
		usb->istr = sr & ~USB_SOF;
		DCC_LOG(LOG_INFO, "SOF");
	}

	if (sr & USB_RESET) {
		usb->istr = sr & ~USB_RESET;
		DCC_LOG(LOG_INFO, "RESET");
		stm32f_otg_dev_reset(drv);
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
				stm32f_otg_dev_ep0_in(drv);
			} else {
				/* DIR = 1 -> OUT/SETUP */
				clr_ep_flag(usb, 0, USB_CTR_RX);
				if (epr & USB_SETUP) {
					/* SETUP */
					stm32f_otg_dev_ep0_setup(drv);
				} else {
					/* OUT */
					stm32f_otg_dev_ep0_out(drv);
				}
			}
			return;
		}

		if (epr & USB_CTR_RX) {
			/* OUT */
			clr_ep_flag(usb, ep_id, USB_CTR_RX);
			stm32f_otg_dev_ep_out(drv, ep_id);
		}

		if (epr & USB_CTR_TX) {
			/* IN */
			clr_ep_flag(usb, ep_id, USB_CTR_TX);
			stm32f_otg_dev_ep_in(drv, ep_id);
		}

	}
}
#endif

#define OTG_FS_DP STM32F_GPIOA, 12
#define OTG_FS_DM STM32F_GPIOA, 11
#define OTG_FS_VBUS STM32F_GPIOA, 9
#define OTG_FS_ID STM32F_GPIOA, 10

static void otg_fs_io_init(void)
{
	DCC_LOG(LOG_MSG, "Enabling GPIO clock...");
	stm32f_gpio_clock_en(STM32F_GPIOA);

	DCC_LOG(LOG_MSG, "Configuring GPIO pins...");

	stm32f_gpio_af(OTG_FS_DP, GPIO_AF10);
	stm32f_gpio_af(OTG_FS_DM, GPIO_AF10);
	stm32f_gpio_af(OTG_FS_ID, GPIO_AF10);

	stm32f_gpio_mode(OTG_FS_DP, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32f_gpio_mode(OTG_FS_DM, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32f_gpio_mode(OTG_FS_ID, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
}

void otg_fs_vbus_connect(bool connect)
{
	if (connect)
		stm32f_gpio_mode(OTG_FS_VBUS, ALT_FUNC, SPEED_LOW);
	else
		stm32f_gpio_mode(OTG_FS_VBUS, INPUT, 0);
}

void otg_fs_power_on(struct stm32f_otg_fs * otg_fs)
{
	struct stm32f_rcc * rcc = STM32F_RCC;

	otg_fs_vbus_connect(true);

	DCC_LOG(LOG_MSG, "Enabling USB FS clock...");
	rcc->ahb2enr |= RCC_OTGFSEN;

	/* Initialize as a device */
	stm32f_otg_fs_device_init(otg_fs);

	/* Enable Cortex interrupt */
	cm3_irq_enable(STM32F_IRQ_OTG_FS);
}

void otg_fs_power_off(struct stm32f_otg_fs * otg_fs)
{
	struct stm32f_rcc * rcc = STM32F_RCC;

//	usb->cntr = USB_FRES;
	/* Removing any spurious pending interrupts */
//	usb->istr = 0;

	otg_fs_vbus_connect(false);

//	usb->cntr = USB_FRES | USB_PDWN;

	DCC_LOG(LOG_TRACE, "Disabling USB device clock...");
	rcc->ahb2enr &= ~RCC_OTGFSEN;

	/* disabling IO pins */
	stm32f_gpio_mode(OTG_FS_DP, INPUT, 0);
	stm32f_gpio_mode(OTG_FS_DM, INPUT, 0);

}

int stm32f_otg_fs_dev_init(struct stm32f_otg_drv * drv, usb_class_t * cl,
		const usb_class_events_t * ev)
{
	struct stm32f_otg_fs * otg_fs = STM32F_OTG_FS;

	drv->cl = cl;
	drv->ev = ev;

	DCC_LOG1(LOG_TRACE, "ev=0x%08x", drv->ev);

	otg_fs_power_off(otg_fs);

	udelay(1000);

	/* Initialize IO pins */
	otg_fs_io_init();

	otg_fs_power_on(otg_fs);

	DCC_LOG(LOG_TRACE, "[ATTACHED]");
	return 0;
}

void stm32f_otg_fs_isr(void)
{
	struct stm32f_otg_drv * drv = &stm32f_otg_fs_drv0;
	struct stm32f_otg_fs * otg_fs = STM32F_OTG_FS;
	uint32_t gintsts;
	uint32_t ep_intr;

	gintsts = otg_fs->gintsts & otg_fs->gintmsk;

	DCC_LOG1(LOG_MSG, "GINTSTS=0x%08x", gintsts);

	if (gintsts & OTG_FS_SRQINT) {
		/* Session request/new session detected interrupt */
		DCC_LOG(LOG_INFO, "<SRQINT>  [POWERED]");
		otg_fs->gintmsk |= OTG_FS_WUIM | OTG_FS_USBRSTM | OTG_FS_ENUMDNEM |
			OTG_FS_ESUSPM | OTG_FS_USBSUSPM;
	}

	if (gintsts & OTG_FS_PTXFE) {
		DCC_LOG(LOG_TRACE, "<PTXFE>");
	}

	if (gintsts & OTG_FS_OTGINT) {
		uint32_t gotgint = otg_fs->gotgint;
		DCC_LOG(LOG_INFO, "<OTGINT>");
		if (gotgint & OTG_FS_OTGINT) {
			DCC_LOG(LOG_INFO, "<SEDET>  [ATTACHED]");
			otg_fs->gintmsk = OTG_FS_SRQIM | OTG_FS_OTGINT;
		}
		otg_fs->gotgint = gotgint;
	}

	if (gintsts & OTG_FS_GONAKEFF) {
		DCC_LOG(LOG_TRACE, "<GONAKEFF>");
		otg_fs->gintmsk &= ~OTG_FS_GONAKEFFM;
	}

	/* RxFIFO non-empty */
	if (gintsts & OTG_FS_RXFLVL) {
		uint32_t grxsts;
		int ep_id;
		int len;
		int stat;

		/* 1. On catching an RXFLVL interrupt (OTG_FS_GINTSTS register),
		   the application must read the Receive status pop
		   register (OTG_FS_GRXSTSP). */
		/* pop the rx fifo status */
		grxsts = otg_fs->grxstsp;

		ep_id = OTG_FS_EPNUM_GET(grxsts);
		len = OTG_FS_BCNT_GET(grxsts);
		(void)len;
		stat = OTG_FS_PKTSTS_GET(grxsts);
		(void)stat;

		DCC_LOG3(LOG_TRACE, "[%d] <RXFLVL> len=%d status=%d", ep_id, len, stat);

		if (ep_id == 0) {
			/* 3. If the received packet’s byte count is not 0, the byte count
			   amount of data is popped from the receive Data FIFO and stored in
			   memory. If the received packet byte count is 0, no data is popped
			   from the receive data FIFO. */

			switch (grxsts & OTG_FS_PKTSTS_MSK) {
			case OTG_FS_PKTSTS_GOUT_NACK:
				/* Global OUT NAK (triggers an interrupt) */
				DCC_LOG1(LOG_TRACE, "[%d] <RXFLVL> <GOUT_NACK>", ep_id);
				break;
			case OTG_FS_PKTSTS_OUT_DATA_UPDT: {
				/* OUT data packet received */
				DCC_LOG1(LOG_TRACE, "[%d] <RXFLVL> <OUT_DATA_UPDT>", ep_id);
				stm32f_otg_dev_ep0_out(drv, len);
				break;
			}
			case OTG_FS_PKTSTS_OUT_XFER_COMP:
				DCC_LOG1(LOG_TRACE, "[%d] <RXFLVL> <OUT_XFER_COMP>", ep_id);
				break;
			case OTG_FS_PKTSTS_SETUP_COMP:
				/* SETUP transaction completed (triggers an interrupt) */
				DCC_LOG1(LOG_TRACE, "[%d] <RXFLVL> <SETUP_COMP>", ep_id);
				stm32f_otg_dev_ep0_setup(drv);

				break;
			case OTG_FS_PKTSTS_SETUP_UPDT: {
				uint32_t * buf = (uint32_t *)&drv->req;
				/* SETUP data packet received */
				DCC_LOG1(LOG_TRACE, "[%d] <RXFLVL> <SETUP_UPDT>", ep_id);

				if (len != 8) {
					DCC_LOG(LOG_ERROR, "setup data len != 8!");
				}

				/* Copy the received setup packet into the setup buffer */
				buf[0] = otg_fs->dfifo[0].pop;
				buf[1] = otg_fs->dfifo[0].pop;

				DCC_LOG2(LOG_TRACE, "SETUP: 0x%08x 0x%08x", buf[0], buf[1]);
				break;
			}
			}
		} else {
			switch (grxsts & OTG_FS_PKTSTS_MSK) {
			case OTG_FS_PKTSTS_OUT_DATA_UPDT:
				/* OUT data packet received */
				DCC_LOG1(LOG_TRACE, "[%d] <RXFLVL> <OUT_DATA_UPDT>", ep_id);
				/* 2. The application can mask the RXFLVL interrupt (in
				   OTG_FS_GINTSTS) by writing to RXFLVL = 0 (in
				   OTG_FS_GINTMSK), until it has read the packet from
				   the receive FIFO. */
				otg_fs->gintmsk &= ~OTG_FS_RXFLVLM;
				/* Enable SOF interrupts */
				otg_fs->gintmsk |=  OTG_FS_SOFM;
				DCC_LOG(LOG_INFO, "__thinkos_ev_raise(RX)");
				stm32f_otg_dev_ep_out(drv, ep_id);
				break;
			case OTG_FS_PKTSTS_OUT_XFER_COMP:
				DCC_LOG1(LOG_TRACE, "[%d] <RXFLVL> <OUT_XFER_COMP>", ep_id);
				/* FIXME: generic code */
#if 0
				/* Prepare to receive more */
				otg_fs->outep[ep_id].doeptsiz =
					OTG_FS_PKTCNT_SET(EP_OUT_FIFO_SIZE / EP_OUT_MAX_PKT_SIZE) |
					OTG_FS_XFRSIZ_SET(EP_OUT_FIFO_SIZE);
				/* EP enable */
				otg_fs->outep[ep_id].doepctl |= OTG_FS_EPENA | OTG_FS_CNAK;
#endif
				/* Disable SOF interrupts */
				otg_fs->gintmsk &= ~OTG_FS_SOFM;

				break;
			}
		}

		/* 5. After the data payload is popped from the receive FIFO, the
		   RXFLVL interrupt (OTG_FS_GINTSTS) must be unmasked. */
		//	otg_fs->gintmsk |= OTG_FS_RXFLVLM;
	}

	if (gintsts & OTG_FS_SOF) {
		DCC_LOG(LOG_MSG, "<SOF>");
	}

	if (gintsts & OTG_FS_WKUPINT) {
		DCC_LOG(LOG_TRACE, "<WKUPINT>");
		/* disable resume/wakeup interrupts */
	}

	if (gintsts & OTG_FS_USBRST ) {
		/* end of bus reset */
		//		DCC_LOG(LOG_TRACE, "<USBRST>");
		/* initialize tx buffer */
		stm32f_otg_dev_reset(drv);
		DCC_LOG(LOG_TRACE, "<USBRST> --------------- [DEFAULT]");
	}

	if (gintsts & OTG_FS_ENUMDNE) {
		DCC_LOG(LOG_INFO, "<ENUMDNE>");
		otg_fs_on_enum_done(otg_fs);
	}

	if (gintsts & OTG_FS_ESUSP) {
		DCC_LOG(LOG_INFO, "<ESUSP>");
	}

	if (gintsts & OTG_FS_USBSUSP) {
		DCC_LOG(LOG_INFO, "<USBSUSP>");
	}

	if (gintsts & OTG_FS_IEPINT) {
		uint32_t diepmsk;
		uint32_t diepint;
		uint32_t diepempmsk;
		uint32_t msk;

		ep_intr = (otg_fs->daint & otg_fs->daintmsk);
		diepmsk = otg_fs->diepmsk;
		diepempmsk = otg_fs->diepempmsk;

		if (ep_intr & OTG_FS_IEPINT0) {
			/* add the Transmit FIFO empty bit to the mask */
			msk = diepmsk | ((diepempmsk >> 0) & 0x1) << 7;
			diepint = otg_fs->inep[0].diepint & msk;
			if (diepint & OTG_FS_XFRC) {
				DCC_LOG(LOG_TRACE, "[0] <IEPINT> <XFRC>");
			}
			if (diepint & OTG_FS_TXFE) {
				DCC_LOG(LOG_TRACE, "[0] <IEPINT> <TXFE>");
				/* mask FIFO empty interrupt */
				otg_fs->diepempmsk &= ~(1 << 1);
			}
			/* clear interrupts */
			otg_fs->inep[0].diepint = diepint;
		}

		if (ep_intr & OTG_FS_IEPINT1) {
			/* add the Transmit FIFO empty bit to the mask */
			msk = diepmsk | ((diepempmsk >> 1) & 0x1) << 7;
			diepint = otg_fs->inep[1].diepint & msk;
			if (diepint & OTG_FS_XFRC) {
				DCC_LOG(LOG_TRACE, "[1] <IEPINT> <XFRC>");
			}
			if (diepint & OTG_FS_TXFE) {
				DCC_LOG(LOG_TRACE, "[1] <IEPINT> <TXFE>");
				/* mask FIFO empty interrupt */
				otg_fs->diepempmsk &= ~(1 << 1);
			}
			/* clear interrupts */
			otg_fs->inep[1].diepint = diepint;
		}

		if (ep_intr & OTG_FS_IEPINT2) {
			/* add the Transmit FIFO empty bit to the mask */
			msk = diepmsk | ((diepempmsk >> 2) & 0x1) << 7;
			diepint = otg_fs->inep[2].diepint & msk;
			if (diepint & OTG_FS_XFRC) {
				DCC_LOG(LOG_TRACE, "[2] <IEPINT> <XFRC>");
			}
			if (diepint & OTG_FS_TXFE) {
				DCC_LOG(LOG_TRACE, "[2] <IEPINT> <TXFE>");
				/* mask FIFO empty interrupt */
				otg_fs->diepempmsk &= ~(1 << 2);
			}
			otg_fs->inep[2].diepint = diepint;
		}

		if (ep_intr & OTG_FS_IEPINT3) {
			/* add the Transmit FIFO empty bit to the mask */
			msk = diepmsk | ((diepempmsk >> 3) & 0x1) << 7;
			diepint = otg_fs->inep[3].diepint & msk;
			if (diepint & OTG_FS_XFRC) {
				DCC_LOG(LOG_TRACE, "[3] <IEPINT> <XFRC>");
			}
			if (diepint & OTG_FS_TXFE) {
				DCC_LOG(LOG_TRACE, "[3] <IEPINT> <TXFE>");
				/* mask FIFO empty interrupt */
				otg_fs->diepempmsk &= ~(1 << 2);
			}
			otg_fs->inep[3].diepint = diepint;
		}
	}

	if (gintsts & OTG_FS_OEPINT) {

		//		DCC_LOG(LOG_TRACE, "<OEPINT>");

		ep_intr = (otg_fs->daint & otg_fs->daintmsk);

		if (ep_intr & OTG_FS_OEPINT0) {
			DCC_LOG(LOG_INFO, "[0] <OEPINT>");
			stm32f_otg_dev_ep0_in(drv);
		}

		if (ep_intr & OTG_FS_OEPINT1) {
			uint32_t doepint;

			doepint = otg_fs->outep[1].doepint & otg_fs->doepmsk;

			if (doepint & OTG_FS_XFRC) {
				DCC_LOG(LOG_TRACE, "[1] <OEPINT> <OUT XFRC>");
				stm32f_otg_dev_ep_in(drv, 1);
			}
			if (doepint & OTG_FS_EPDISD) {
				DCC_LOG(LOG_TRACE, "[1] <OEPINT> <EPDISD>");
			}
			if (doepint & OTG_FS_STUP) {
				DCC_LOG(LOG_TRACE, "[1] <OEPINT> <STUP>");
			}
			/* clear interrupts */
			otg_fs->outep[1].doepint = doepint;
		}

		if (ep_intr & OTG_FS_OEPINT2) {
			uint32_t doepint;

			doepint = otg_fs->outep[2].doepint & otg_fs->doepmsk;

			if (doepint & OTG_FS_XFRC) {
				DCC_LOG(LOG_TRACE, "[2] <OEPINT> <OUT XFRC>");
				stm32f_otg_dev_ep_in(drv, 2);
			}
			if (doepint & OTG_FS_EPDISD) {
				DCC_LOG(LOG_TRACE, "[2] <OEPINT> <EPDISD>");
			}
			if (doepint & OTG_FS_STUP) {
				DCC_LOG(LOG_TRACE, "[2] <OEPINT> <STUP>");
			}
			/* clear interrupts */
			otg_fs->outep[1].doepint = doepint;
		}

		if (ep_intr & OTG_FS_OEPINT3) {
			uint32_t doepint;

			doepint = otg_fs->outep[3].doepint & otg_fs->doepmsk;

			if (doepint & OTG_FS_XFRC) {
				DCC_LOG(LOG_TRACE, "[3] <OEPINT> <OUT XFRC>");
				stm32f_otg_dev_ep_in(drv, 3);
			}
			if (doepint & OTG_FS_EPDISD) {
				DCC_LOG(LOG_TRACE, "[3] <OEPINT> <EPDISD>");
			}
			if (doepint & OTG_FS_STUP) {
				DCC_LOG(LOG_TRACE, "[3] <OEPINT> <STUP>");
			}

			/* clear interrupts */
			otg_fs->outep[1].doepint = doepint;
		}
	}

	if (gintsts & OTG_FS_IISOIXFR) {
		DCC_LOG(LOG_TRACE, "<IISOIXFR>");
	}

	if (gintsts & OTG_FS_INCOMPISOOUT) {
		DCC_LOG(LOG_TRACE, "<INCOMPISOOUT>");
	}

	if (gintsts & OTG_FS_MMIS) {
		DCC_LOG(LOG_TRACE, "<MMIS>");
	}

	/* clear pending interrupts */
	otg_fs->gintsts = gintsts;
}


/* USB device operations */
const struct usb_dev_ops stm32f_otg_fs_ops = {
	.dev_init = (usb_dev_init_t)stm32f_otg_fs_dev_init,
	.ep_tx_start= (usb_dev_ep_tx_start_t)stm32f_otg_dev_ep_tx_start,
	.ep0_init = (usb_dev_ep0_init_t)stm32f_otg_dev_ep0_init,
	.ep_init = (usb_dev_ep_init_t)stm32f_otg_dev_ep_init,
	.ep_stall = (usb_dev_ep_stall_t)stm32f_otg_dev_ep_stall,
	.ep_zlp_send = (usb_dev_ep_zlp_send_t)stm32f_otg_dev_ep_zlp_send,
	.ep_pkt_recv = (usb_dev_ep_pkt_recv_t)stm32f_otg_dev_ep_pkt_recv
};

/* USB device driver */
const struct usb_dev stm32f_otg_fs_dev = {
	.priv = (void *)&stm32f_otg_fs_drv0,
	.op = &stm32f_otg_fs_ops
};

#endif /* STM32F2X */
