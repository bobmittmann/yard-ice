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
 * @file stm32f-usb_fs_dev.c
 * @brief STM32F USB Full Speed Device Driver
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

#ifdef STM32F103

/* Endpoint state */
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

/* Endpoint control structure */
struct stm32f_usb_ep {
	uint16_t mxpktsz; /* Maximum packet size for this EP */
	ep_state_t state; /* Current EP state */

	uint16_t xfr_rem; /* Bytes pendig in the transfer buffer */
	uint16_t xfr_buf_len; /* Length of the transfer buffer */
	uint8_t * xfr_buf; /* Transfer buffer pointer */
	uint8_t * xfr_ptr; /* Pointer to the next transfer */

	/* Endpoint callback */
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
	usb_class_t * cl;
	const struct usb_class_events * ev;
	struct usb_request req;
};

/* -------------------------------------------------------------------------
 * End point packet buffer helpers
 * ------------------------------------------------------------------------- */

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

/* -------------------------------------------------------------------------
 * End point low level operations
 * ------------------------------------------------------------------------- */

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
	//	uint32_t epr;
	int len;

	len = MIN(ep->xfr_rem, ep->mxpktsz);

	DCC_LOG2(LOG_INFO, "ep_id=%d, len=%d", ep_id, len);

	//	epr = usb->epr[ep_id];
	//	if (epr & USB_EP_DBL_BUF) {
	//		DCC_LOG(LOG_TRACE, "double");
	//	} else {
	//		DCC_LOG(LOG_TRACE, "single");
	//	}

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

/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */

void stm32f_usb_dev_reset(struct stm32f_usb_drv * drv)
{
	struct stm32f_usb * usb = STM32F_USB;

	DCC_LOG(LOG_TRACE, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");

	/* set the btable address */
	usb->btable = 0x000;

	drv->ev->on_reset(drv->cl);

	/* Enable the device and set the address to 0 */
	usb->daddr = USB_EF + 0;

	/* Enable Correct transfer interrupts */
	usb->cntr |= USB_CTRM;
}

/* ------------------------------------------------------------------------- */

int stm32f_usb_dev_ep_stall(struct stm32f_usb_drv * drv, int ep_id)
{
	DCC_LOG1(LOG_TRACE, "ep_id=%d", ep_id);

	__ep_stall(drv, ep_id);

	return 0;
}

void stm32f_usb_dev_ep_in(struct stm32f_usb_drv * drv, int ep_id)
{
	struct stm32f_usb * usb = STM32F_USB;
	struct stm32f_usb_ep * ep = &drv->ep[ep_id];

	DCC_LOG1(LOG_INFO, "================= ep_id=%d", ep_id);

	if (ep->state != EP_IN_DATA_LAST) {
		__ep_pkt_send(drv, ep_id);
	} else {
		set_ep_txstat(usb, ep_id, USB_TX_NAK);
		/* call class endpoint callback */
		ep->on_in(drv->cl, ep_id);
	}
}

/* start sending */
int stm32f_usb_ep_tx_start(struct stm32f_usb_drv * drv, int ep_id,
						   void * buf, unsigned int len)
{
	struct stm32f_usb_ep * ep = &drv->ep[ep_id];

	DCC_LOG2(LOG_INFO, "ep_id=%d len=%d", ep_id, len);

	ep = &drv->ep[ep_id];
	ep->xfr_ptr = buf;
	ep->xfr_rem = len;

	__ep_pkt_send(drv, ep_id);

	return len;
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


	if (epr & USB_EP_DBL_BUF) {
		/* double buffer */
		/* select the descriptor according to the data toggle bit */
		pkt = &pktbuf[ep_id].dbrx[(epr & USB_DTOG_RX) ? 0: 1];
		DCC_LOG1(LOG_INFO, "===== ep_id=%d: double buffer", ep_id);
	} else {
		/* single buffer */
		pkt = &pktbuf[ep_id].rx;
		DCC_LOG1(LOG_INFO, "===== ep_id=%d: single buffer", ep_id);
	}

	DCC_LOG1(LOG_INFO, "cnt=%d", pkt->count);

	cnt = MIN(pkt->count, len);

	/* Data received */
	__copy_from_pktbuf(buf, pkt, cnt);

	if ((epr & USB_EP_DBL_BUF) == 0) {
		/* free the out(rx) packet buffer */
		set_ep_rxstat(usb, ep_id, USB_RX_VALID);
	}

	DCC_LOG2(LOG_INFO, "OUT EP%d, cnt=%d", ea, cnt);

	return cnt;
}

void stm32f_usb_dev_ep_out(struct stm32f_usb_drv * drv, int ep_id)
{
	struct stm32f_usb * usb = STM32F_USB;
	struct stm32f_usb_pktbuf * pktbuf = STM32F_USB_PKTBUF;
	struct stm32f_usb_ep * ep = &drv->ep[ep_id];
	struct stm32f_usb_rx_pktbuf * pkt;
	uint32_t epr;

	epr = usb->epr[ep_id];
	if (epr & USB_EP_DBL_BUF) {
		/* double buffer */
		/* prepare to receive on other buffer */
		set_ep_rxstat(usb, ep_id, USB_RX_VALID);
		/* select the descriptor according to the data toggle bit */
		pkt = &pktbuf[ep_id].dbrx[(epr & USB_DTOG_RX) ? 0: 1];
	} else {
		/* single buffer */
		pkt = &pktbuf[ep_id].rx;
	}

	DCC_LOG1(LOG_INFO, "cnt=%d", pkt->count);

	/* call class endpoint callback */
	ep->on_out(drv->cl, ep_id, pkt->count);
}

#if 0

void stm32f_usb_dev_ep_out(struct stm32f_usb_drv * drv, int ep_id)
{
	struct stm32f_usb * usb = STM32F_USB;
	struct stm32f_usb_ep * ep = &drv->ep[ep_id];
	struct stm32f_usb_pktbuf * pktbuf = STM32F_USB_PKTBUF;
	struct stm32f_usb_rx_pktbuf * pkt;
	uint32_t epr;


	epr = usb->epr[ep_id];
	if (epr & USB_EP_DBL_BUF) {
		/* prepare to receive on other buffer */
		set_ep_rxstat(usb, ep_id, USB_RX_VALID);
		/* select the descriptor according to the data toggle bit */
		pkt = &pktbuf[ep_id].dbrx[(epr & USB_DTOG_RX) ? 0: 1];
	} else {
		/* single buffer */
		pkt = &pktbuf[ep_id].rx;
	}

	DCC_LOG1(LOG_TRACE, "================= ep_id=%d", ep_id);

	cnt = MIN(pkt->count, len);
	n = MIN(pkt->count, ep->xfr_rem);

	__copy_from_pktbuf(ep->xfr_ptr, &pktbuf[0].rx, n);

	ep->xfr_ptr += n;
	ep->xfr_rem -= n;

	if (!(epr & USB_EP_DBL_BUF)) {
		/* free the out(rx) packet buffer */
		set_ep_rxstat(usb, ep_id, USB_RX_VALID);
	}

	DCC_LOG2(LOG_TRACE, "OUT EP%d, cnt=%d", ep_id, cnt);

	return cnt;

}

/* prepares to receive */
int stm32f_usb_ep_rx_prep(struct stm32f_usb_drv * drv, int ep_id,
						  void * buf, unsigned int len)
{
	struct stm32f_usb_ep * ep;

	DCC_LOG1(LOG_TRACE, "====!!!!!!============= ep_id=%d", ep_id);

	ep = &drv->ep[ep_id];
	ep->xfr_buf = buf;
	ep->xfr_buf_len = buf_len;

	ep->xfr_ptr = buf;
	ep->xfr_rem = buf_len;
	ep->xfr_cnt = 0;
	ep->state = EP_IDLE;

	return 0;
}

#endif

int stm32f_usb_dev_ep_zlp_send(struct stm32f_usb_drv * drv, int ep_id)
{
	DCC_LOG1(LOG_TRACE, "ep_id=%d", ep_id);

	__ep_zlp_send(drv, ep_id);

	return 0;
}

/* FIXME: find another way of initializing the packet buffer addresses */
#define PKTBUF_BUF_BASE (8 * 8)
static unsigned int addr = PKTBUF_BUF_BASE;

int stm32f_usb_dev_ep_init(struct stm32f_usb_drv * drv, 
						   const usb_dev_ep_info_t * info,
						   void * xfr_buf, int buf_len)
{
	struct stm32f_usb_pktbuf * pktbuf = STM32F_USB_PKTBUF;
	unsigned int sz;
	struct stm32f_usb * usb = STM32F_USB;
	struct stm32f_usb_ep * ep;
	int mxpktsz = info->mxpktsz;
	int ep_id;

	if ((ep_id = info->addr & 0x7f) > 7) {
		DCC_LOG1(LOG_WARNING, "addr(%d) > 7", ep_id);
		return -1;
	}

	DCC_LOG2(LOG_TRACE, "sp_id=%d mxpktsz=%d", ep_id, mxpktsz);

	ep = &drv->ep[ep_id];
	ep->mxpktsz = mxpktsz;
	ep->state = EP_IDLE;
	ep->on_ev = info->on_ev;

	ep->xfr_buf = (uint8_t *)xfr_buf;
	ep->xfr_buf_len = buf_len;

	DCC_LOG3(LOG_TRACE, "ep_id=%d addr=%d mxpktsz=%d",
			 ep_id, info->addr & 0x7f, mxpktsz);

	if (ep_id == 0) {

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

	return ep_id;
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

		DCC_LOG(LOG_TRACE, "EP0 [SETUP] IN Dev->Host");
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
		DCC_LOG1(LOG_TRACE, "EP0 data lenght = %d", ep->xfr_rem);
		__ep_pkt_send(drv, 0);
		/* While enabling the last data stage, the opposite direction should
		   be set to NAK, so that, if the host reverses the transfer direction
		   (to perform the status stage) immediately, it is kept waiting for
		   the completion of the control operation. If the control operation
		   completes successfully, the software will change NAK to VALID,
		   otherwise to STALL.
		 */
		if (ep->state == EP_IN_DATA) {
			/* A USB device can determine the number and direction of 
			   data stages by interpreting the data transferred in the 
			   SETUP stage, and is required to STALL the transaction in 
			   the case of errors. To do so, at all data stages before 
			   the last, the unused direction should be set to STALL, so 
			   that, if the host reverses the transfer direction too soon, 
			   it gets a STALL as a status stage.
			 */
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
						const struct usb_class_events * ev)
{
	struct stm32f_usb * usb = STM32F_USB;

	drv->cl = cl;
	drv->ev = ev;

	DCC_LOG1(LOG_TRACE, "ev=0x%08x", drv->ev);

	stm32f_usb_power_off(usb);

	udelay(1000);

	/* Initialize IO pins */
	stm32f_usb_io_init();

	stm32f_usb_power_on(usb);

	/* Enable Reset, SOF  and Wakeup interrupts */
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

		/*
		 * The CTR_RX event is serviced by first
		 determining the transaction type (SETUP bit in the USB_EPnR 
		 register); the application software must clear the interrupt 
		 flag bit and get the number of received bytes reading the 
		 COUNTn_RX location inside the buffer description table entry 
		 related to the endpoint being processed. After the received 
		 data is processed, the application software should set the 
		 STAT_RX bits to ‘11 (Valid) in the USB_EPnR, enabling further 
		 transactions. While the STAT_RX bits are equal to ‘10 (NAK), 
		 any OUT request addressed to that endpoint is NAKed, indicating 
		 a flow control condition: the USB host will retry the 
		 transaction until it succeeds. It is mandatory to execute the 
		 sequence of operations in the above mentioned order to avoid 
		 losing the notification of a second OUT transaction addressed 
		 to the same endpoint following immediately the one which 
		 triggered the CTR interrupt.
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
	.ep_tx_start = (usb_dev_ep_tx_start_t)stm32f_usb_ep_tx_start,
	.ep_init = (usb_dev_ep_init_t)stm32f_usb_dev_ep_init,
	.ep_stall = (usb_dev_ep_stall_t)stm32f_usb_dev_ep_stall,
	.ep_zlp_send = (usb_dev_ep_zlp_send_t)stm32f_usb_dev_ep_zlp_send,
	.ep_pkt_recv = (usb_dev_ep_pkt_recv_t)stm32f_usb_dev_ep_pkt_recv
};

/* USB device driver */
const struct usb_dev stm32f_usb_fs_dev = {
	.priv = (void *)&stm32f_usb_drv0,
	.op = &stm32f_usb_ops
};

#endif /* STM32F103 */
