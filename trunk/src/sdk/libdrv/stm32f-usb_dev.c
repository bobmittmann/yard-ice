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


/* Endpoint control */
struct stm32f_usb_ep {
	struct stm32f_usb_pktbuf * pktbuf;
	uint8_t * rx_buf;
	uint8_t * tx_buf;
	uint16_t tx_rem;
	uint16_t mxpktsz;
	union {
		usb_class_on_ep_rx_t on_rx;
		usb_class_on_setup_t on_setup;
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
};

int stm32f_copy_from_pktbuf(uint8_t * dst, 
							struct stm32f_usb_rx_pktbuf * rx)
{
	uint32_t * src;
	uint32_t data;
	int cnt;
	int i;

	/* Data received */
	cnt = rx->count;

	/* copy data to destination buffer */
	src = (uint32_t *)STM32F_USB_PKTBUF_ADDR + (rx->addr / 2);
	for (i = 0; i < (cnt + 1) / 2; i++) {
		data = *src++;
		*dst++ = data;
		*dst++ = data >> 8;
	}

	return cnt;
}

void stm32f_copy_to_pktbuf(struct stm32f_usb_tx_pktbuf * tx, 
						   uint8_t * src, int len)
{
	uint32_t * dst;
	int i;

	/* copy data to destination buffer */
	dst = (uint32_t *)STM32F_USB_PKTBUF_ADDR + (tx->addr / 2);
	for (i = 0; i < (len + 1) / 2; i++) {
		*dst = src[0] | (src[1] << 8);
		src += 2;
	}

	tx->count = len;
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
		drv->ep[i].rx_buf = NULL;
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

int stm32f_usb_dev_ep0_init(struct stm32f_usb_drv * drv, int mxpktsz)
{
	struct stm32f_usb * usb = STM32F_USB;
	struct stm32f_usb_ep * ep;

	DCC_LOG1(LOG_TRACE, "mxpktsz=%d", mxpktsz);

	ep = &drv->ep[0];
	ep->mxpktsz = mxpktsz;

	/* initializes EP0 */
	stm32f_usb_ep0_init(usb, mxpktsz);

	return 0;
}


void stm32f_usb_dev_reset(struct stm32f_usb_drv * drv)
{
	struct stm32f_usb * usb = STM32F_USB;

	DCC_LOG(LOG_TRACE, "...");

	/* set the btable address */
	usb->btable = 0x000;

	drv->ev->on_reset(drv->cl);

	/* Enable the device and set the address to 0 */
	usb->daddr = USB_EF + 0;

	/* Enable Correct transfer and other interrupts */
//	usb->cntr |= USB_CTRM | USB_PMAOVRM | USB_ERRM | USB_SUSPM;
	usb->cntr |= USB_CTRM;
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

#define STM32F_USB_PKTBUF ((struct stm32f_usb_pktbuf *)STM32F_USB_PKTBUF_ADDR)

void stm32f_usb_dev_ep_out(struct stm32f_usb_drv * drv, int ep_id)
{
	struct stm32f_usb * usb = STM32F_USB;
	struct stm32f_usb_pktbuf * pktbuf = STM32F_USB_PKTBUF;
	struct stm32f_usb_ep * ep;
	uint32_t epr;
	int cnt;
	int ea;

	epr = usb->epr[ep_id];
	ep = &drv->ep[ep_id];
	ea = USB_EA_GET(epr);
	(void)ea;

#if 0
	if (epr & USB_EP_DBL_BUF) {
		/* double buffer */
		/* select the descriptor according to the data toggle bit */
		desc = &ep->pktbuf[(epr & USB_DTOG_RX) ? 0: 1];
	} else {
		/* single buffer */
		/* select the RX descriptor */
		desc = &ep->pktbuf[1];
	}
#endif

	/* Data received */
	cnt = stm32f_copy_from_pktbuf(ep->rx_buf, &pktbuf[ep_id].rx);

	if (epr & USB_SETUP) {
		DCC_LOG2(LOG_TRACE, "SETUP EP%d, cnt=%d", ea, cnt);
		/* call class endpoint callback */
		drv->ev->on_setup(drv->cl, ep->rx_buf, cnt);
	} else {
		DCC_LOG2(LOG_TRACE, "OUT EP%d, cnt=%d", ea, cnt);
		/* call class endpoint callback */
		ep->on_rx(drv->cl, ep->rx_buf, cnt);
	}

	/* Set the endpoint as valid to continue receiving */
	epr = epr ^ USB_RX_VALID; /* adjust the bits we want to toggle */
	epr &= ~(USB_CTR_RX); 	/* clear the CTR_RX bit */
	usb->epr[ep_id] = epr;
}

/* prepares to receive */
int stm32f_usb_ep_rx_setup(struct stm32f_usb_drv * drv, int ep_id,
		void * buf, unsigned int len)
{
	struct stm32f_usb_ep * ep;

	DCC_LOG1(LOG_TRACE, "ep_id=%d", ep_id);

	ep = &drv->ep[ep_id];
	ep->rx_buf = buf;

	return 0;
}

int stm32f_usb_dev_ep_stall(struct stm32f_usb_drv * drv, int ep_id)
{
	struct stm32f_usb * usb = STM32F_USB;

	DCC_LOG1(LOG_TRACE, "ep_id=%d", ep_id);

	set_ep_txstat(usb, ep_id, USB_TX_STALL);

	return 0;
}

int stm32f_usb_dev_ep_in(struct stm32f_usb_drv * drv, int ep_id)
{
	struct stm32f_usb_pktbuf * pktbuf = STM32F_USB_PKTBUF;
	struct stm32f_usb * usb = STM32F_USB;
	struct stm32f_usb_ep * ep;
	int len;

	ep = &drv->ep[ep_id];

	if (ep->tx_rem == 0) {
		DCC_LOG1(LOG_TRACE, "fixme: ep_id=%d", ep_id);
		/* TODO: ... */
		return -1;
	}

	len = MIN(ep->tx_rem, ep->mxpktsz);

	DCC_LOG2(LOG_TRACE, "ep_id=%d len=%d", ep_id, len);
		
	stm32f_copy_to_pktbuf(&pktbuf[ep_id].tx, ep->tx_buf, len);

	ep->tx_rem -= len;
	ep->tx_buf += len;

	set_ep_txstat(usb, ep_id, USB_TX_VALID);

	return 0;
}

/* start sending */
int stm32f_usb_ep_tx_start(struct stm32f_usb_drv * drv, int ep_id,
		void * buf, unsigned int len)
{
	struct stm32f_usb_ep * ep;

	DCC_LOG1(LOG_TRACE, "ep_id=%d", ep_id);

	ep = &drv->ep[ep_id];
	ep->tx_buf = buf;
	ep->tx_rem = len;

	stm32f_usb_dev_ep_in(drv, ep_id);

	return 0;
}


void stm32f_usb_dev_ep0_out(struct stm32f_usb_drv * drv,
		struct stm32f_usb_pktbuf * desc)
{
	struct stm32f_usb * usb = STM32F_USB;

	(void)usb;

	DCC_LOG(LOG_TRACE, "OUT 0");
}

void stm32f_usb_dev_ep0_in(struct stm32f_usb_drv * drv,
		struct stm32f_usb_pktbuf * desc)
{
	struct stm32f_usb * usb = STM32F_USB;

	(void)usb;

	DCC_LOG(LOG_TRACE, "IN 0");
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
	struct stm32f_usb * usb = STM32F_USB;
	uint32_t sr;

	sr = usb->istr;
	(void)sr;
}


/* USB low priority ISR */
void stm32f_can1_rx0_usb_lp_isr(void)
{
	struct stm32f_usb_drv * drv = &stm32f_usb_drv0;
	struct stm32f_usb * usb = STM32F_USB;
	uint32_t sr;

	sr = usb->istr;
	(void)sr;

	if (sr & USB_SOF) {
		DCC_LOG(LOG_TRACE, "SOF");
	}

	if (sr & USB_RESET) {
		DCC_LOG(LOG_INFO, "RESET");
		usb->istr = ~USB_RESET;
		stm32f_usb_dev_reset(drv);
	}

	if (sr & USB_SUSP) {
		DCC_LOG(LOG_TRACE, "SUSP");
	}

	if (sr & USB_WKUP) {
		DCC_LOG(LOG_TRACE, "WKUP");
	}

	if (sr & USB_ERR) {
		DCC_LOG(LOG_TRACE, "ERR");
	}

	if (sr & USB_PMAOVR) {
		DCC_LOG(LOG_TRACE, "PMAOVR");
	}

	if (sr & USB_CTR) {
		int ep_id;

		ep_id = USB_EP_ID_GET(sr);

		DCC_LOG1(LOG_TRACE, "CTR ep_id=%d", ep_id);
		if (sr & USB_DIR)
			stm32f_usb_dev_ep_out(drv, ep_id);
		else
			stm32f_usb_dev_ep_in(drv, ep_id);
	}

	/* clear interrupts */
	usb->istr = ~sr;
}

/* USB device operations */
const struct usb_dev_ops stm32f_usb_ops = {
	.dev_init = (usb_dev_init_t)stm32f_usb_dev_init,
	.connect = (usb_dev_connect_t)stm32f_usb_dev_connect,
	.disconnect = (usb_dev_disconnect_t)stm32f_usb_dev_disconnect,
	.ep_rx_setup = (usb_dev_ep_rx_setup_t)stm32f_usb_ep_rx_setup,
	.ep_tx_start= (usb_dev_ep_tx_start_t)stm32f_usb_ep_tx_start,
	.ep0_init = (usb_dev_ep0_init_t)stm32f_usb_dev_ep0_init,
	.ep_stall = (usb_dev_ep_stall_t)stm32f_usb_dev_ep_stall
};

/* USB device driver */
const struct usb_dev stm32f_usb_dev = {
	.priv = (void *)&stm32f_usb_drv0,
	.op = &stm32f_usb_ops
};

