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
 * @file stm32f-otg_hs_dev.c
 * @brief STM32F USB Full Speed Device Driver
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifdef CONFIG_H
#include "config.h"
#endif

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

#ifndef STM32_ENABLE_OTG_HS 
#define STM32_ENABLE_OTG_HS 0
#endif 

#ifndef STM32_OTG_HS_EP_MAX 
#define STM32_OTG_HS_EP_MAX 4
#endif

#ifndef STM32_OTG_HS_ENABLE_VBUS
#define STM32_OTG_HS_ENABLE_VBUS 1
#endif

#define EP_MAX      STM32_OTG_HS_EP_MAX 
#define ENABLE_VBUS STM32_OTG_HS_ENABLE_VBUS
#define RX_FIFO_SIZE 512

#ifdef STM32F_OTG_HS

#if STM32_ENABLE_OTG_HS

/* Endpoint state */
enum ep_state {
	EP_UNCONFIGURED = 0,
	EP_IDLE,
	EP_STALLED,
	EP_SETUP,
	EP_IN_DATA,
	EP_IN_DATA_LAST,
	EP_WAIT_STATUS_IN,
	EP_WAIT_STATUS_OUT,
	EP_OUT_DATA,
	EP_OUT_DATA_LAST,
};

/* Endpoint control */
struct stm32f_otg_ep {
	uint8_t state;
	uint16_t xfr_max;
	uint16_t xfr_rem;
	uint16_t xfr_buf_len;
	uint8_t * xfr_buf;
	uint8_t * xfr_ptr;
	/* reload value for the DOEPTSIZ register */
	uint32_t doeptsiz;
	/* endpoint callback */
	union {
		usb_class_on_ep_ev_t on_ev;
		usb_class_on_ep_in_t on_in;
		usb_class_on_ep_out_t on_out;
		usb_class_on_ep_setup_t on_setup;
	};
};

/* USB Device runtime driver data */
struct stm32f_otg_drv {
	struct stm32f_otg_hs * otg_hs;
	struct stm32f_otg_ep ep[EP_MAX];
	usb_class_t * cl;
	const struct usb_class_events * ev;
	struct usb_request req;
	uint16_t fifo_addr;
};

/* EP TX fifo memory allocation */
static void __ep_pktbuf_alloc(struct stm32f_otg_drv * drv, 
							  int ep_id, int siz)
{
	struct stm32f_otg_hs * otg_hs = drv->otg_hs;

	switch (ep_id) {
	case 0:
		otg_hs->dieptxf0 = OTG_HS_TX0FD_SET(siz / 4) | 
			OTG_HS_TX0FSA_SET(drv->fifo_addr / 4);
		break;
	case 1:
		otg_hs->dieptxf1 = OTG_HS_INEPTXFD_SET(siz / 4) | 
			OTG_HS_INEPTXSA_SET(drv->fifo_addr / 4);
		break;
	case 2:
		otg_hs->dieptxf2 = OTG_HS_INEPTXFD_SET(siz / 4) | 
			OTG_HS_INEPTXSA_SET(drv->fifo_addr / 4);
		break;
#if (EP_MAX > 3)
	case 3:
		otg_hs->dieptxf3 = OTG_HS_INEPTXFD_SET(siz / 4) | 
			OTG_HS_INEPTXSA_SET(drv->fifo_addr / 4);
		break;
#endif
	}

	DCC_LOG2(LOG_INFO, "addr=%d siz=%d", drv->fifo_addr, siz);

	drv->fifo_addr += siz;
}

static void __copy_from_pktbuf(void * ptr,
							volatile uint32_t * pop,
							unsigned int cnt)
{
	uint8_t * dst = (uint8_t *)ptr;
	uint32_t data;
	int i;

	/* pop data from the fifo and copy to destination buffer */
	for (i = 0; i < (cnt + 3) / 4; i++) {
		data = *pop;
		*dst++ = data;
		*dst++ = data >> 8;
		*dst++ = data >> 16;
		*dst++ = data >> 24;
	}
}

static bool __ep_tx_push(struct stm32f_otg_drv * drv, int ep_id)
{
	struct stm32f_otg_ep * ep = &drv->ep[ep_id];
	struct stm32f_otg_hs * otg_hs = drv->otg_hs;
	int cnt;

	/* push data into transmit fifo */
	cnt = stm32f_otg_hs_txf_push(otg_hs, ep_id, ep->xfr_ptr);

	if (cnt < 0) {
		DCC_LOG(LOG_WARNING, "stm32f_otg_hs_txf_push() failed!");
		otg_hs->diepempmsk &= ~(1 << ep_id);
		return false;
	}

	DCC_LOG2(LOG_INFO, "cnt=%d ptr=%p", cnt, ep->xfr_ptr);

	ep->xfr_ptr += cnt;
	ep->xfr_rem -= cnt;

	return true;
}

static void __ep_rx_pop(struct stm32f_otg_drv * drv, int ep_id, int len)
{
	struct stm32f_otg_hs * otg_hs = drv->otg_hs;
	volatile uint32_t * pop = &otg_hs->dfifo[ep_id].pop;
	struct stm32f_otg_ep * ep = &drv->ep[ep_id];
	uint8_t * dst;
	uint32_t data;
	int wcnt;
	int i;

	/* Number of words in the receive fifo */
	wcnt = (len + 3) / 4;
	DCC_LOG1(LOG_INFO, "poping %d words from FIFO.", wcnt);

	if (ep->xfr_rem >= len) {
		/* If we have enough room in the destination buffer
		 * pop data from the fifo and copy to destination buffer */
		dst = ep->xfr_ptr;
		for (i = 0; i < wcnt; ++i) {
			data = *pop;
			*dst++ = data;
			*dst++ = data >> 8;
			*dst++ = data >> 16;
			*dst++ = data >> 24;
		}
		ep->xfr_ptr += len;
		ep->xfr_rem -= len;
	} else {
		DCC_LOG(LOG_WARNING, "not room to copy the whole packet, discarding!");
		for (i = 0; i < wcnt; ++i) {
			data = *pop;
			(void)data;
		}
	}
}

static void __ep_zlp_send(struct stm32f_otg_hs * otg_hs, int epnum)
{
	DCC_LOG(LOG_INFO, "Send: ZLP");

	otg_hs->inep[epnum].dieptsiz = OTG_HS_PKTCNT_SET(1) | OTG_HS_XFRSIZ_SET(0);
	otg_hs->inep[epnum].diepctl |= OTG_HS_EPENA | OTG_HS_CNAK;
}

/* start sending */
int stm32f_otg_hs_dev_ep_pkt_xmit(struct stm32f_otg_drv * drv, int ep_id,
							   void * buf, unsigned int len)
{
	struct stm32f_otg_hs * otg_hs = drv->otg_hs;
	struct stm32f_otg_ep * ep;
	int ret;

	if ((unsigned int)ep_id >= EP_MAX) {
		DCC_LOG(LOG_WARNING, "invalid EP");
		return -1;
	}

	ep = &drv->ep[ep_id];
	ep->xfr_ptr = buf;
	ep->xfr_rem = MIN(len, ep->xfr_max);

	/* prepare fifo to transmit */
	if ((ret = stm32f_otg_hs_txf_setup(otg_hs, ep_id, ep->xfr_rem)) < 0) {
		DCC_LOG(LOG_WARNING, "stm32f_otg_hs_txf_setup() failed!");
	} else {
		/* umask FIFO empty interrupt */
		otg_hs->diepempmsk |= (1 << ep_id);
	}

	DCC_LOG4(LOG_INFO, "ep_id=%d len=%d xfr_max=%d ret=%d", 
			 ep_id, len, ep->xfr_max, ret);

	return ret;
}

int stm32f_otg_hs_dev_ep_pkt_recv(struct stm32f_otg_drv * drv, int ep_id,
							   void * buf, int len)
{
	struct stm32f_otg_hs * otg_hs = drv->otg_hs;
	uint8_t * cp = (uint8_t *)buf;
	struct stm32f_otg_ep * ep;
	uint32_t data;
	int cnt;
	int rem;

	if ((unsigned int)ep_id >= EP_MAX) {
		DCC_LOG(LOG_WARNING, "invalid EP");
		return -1;
	}

	DCC_LOG2(LOG_INFO, "ep_id=%d len=%d", ep_id, len);

	ep = &drv->ep[ep_id];
	
	/* transfer data from fifo */
	cnt = MIN(ep->xfr_rem, len);

	rem = cnt;
	while (rem >= 4) {
		/* word trasfer */
		data = otg_hs->dfifo[ep_id].pop;
		cp[0] = data;
		cp[1] = data >> 8;
		cp[2] = data >> 16;
		cp[3] = data >> 24;
		cp += 4;
		rem -= 4;
	}

	if (rem > 0) {
		/* remaining data */
		data = otg_hs->dfifo[ep_id].pop;
		cp[0] = data;
		if (rem > 1)
			cp[1] = data >> 8;
		if (rem > 2)
			cp[2] = data >> 16;
		rem = 0;
	}

	if ((rem = ep->xfr_rem - cnt) > 0) {
		DCC_LOG1(LOG_WARNING, "dropping %d bytes...", rem);
		/* remove remaining data from fifo */
		do {
			data = otg_hs->dfifo[ep_id].pop;
			(void)data;
			rem -= 4;
		} while (rem > 0);
	}

	/* reset transfer pointer */
	ep->xfr_rem = 0;

	/* 5. After the data payload is popped from the receive FIFO, the 
	   RXFLVL interrupt (OTG_HS_GINTSTS) must be unmasked. */
	DCC_LOG1(LOG_INFO, "cnt=%d enabling RXFLVL interrupt", cnt);

	/* Reenable RX fifo interrupts */
	otg_hs->gintmsk |= OTG_HS_RXFLVLM;

	return cnt;
}


int stm32f_otg_hs_dev_ep_ctl(struct stm32f_otg_drv * drv, 
						  int ep_id, unsigned int opt)
{
	struct stm32f_otg_hs * otg_hs = drv->otg_hs;
	struct stm32f_otg_ep * ep = &drv->ep[ep_id];

	DCC_LOG2(LOG_INFO, "ep=%d opt=%d", ep_id, opt);

	switch (opt) {
	case USB_EP_RECV_OK:
	case USB_EP_NAK_CLR:
		/* Clear NAK, prepare to receive  ... */
		otg_hs->outep[ep_id].doepctl |= OTG_HS_CNAK;
		break;

	case USB_EP_NAK_SET:
		otg_hs->outep[ep_id].doepctl |= OTG_HS_SNAK;
		break;

	case USB_EP_ZLP_SEND:
		__ep_zlp_send(otg_hs, ep_id);
		break;

	case USB_EP_STALL:
#if 0
		__ep_stall(drv, ep_id);
		ep->state = EP_STALLED;
#endif
		break;

	case USB_EP_DISABLE:
		ep->state = EP_UNCONFIGURED;
		break;
	}

	return 0;
}


int stm32f_otg_hs_dev_ep_init(struct stm32f_otg_drv * drv, 
						   const usb_dev_ep_info_t * info, 
						   void * xfr_buf, int buf_len)
{
	struct stm32f_otg_hs * otg_hs = drv->otg_hs;
	struct stm32f_otg_ep * ep;
	int mxpktsz = info->mxpktsz;
	int ep_id;

	if ((ep_id = info->addr & 0x7f) > 3) {
		DCC_LOG1(LOG_ERROR, "invalid address addr=%d", ep_id);
		return -1;
	}

	DCC_LOG3(LOG_INFO, "ep_id=%d addr=%d mxpktsz=%d", 
			 ep_id, info->addr & 0x7f, mxpktsz);

	ep = &drv->ep[ep_id];
	ep->xfr_buf = (uint8_t *)xfr_buf;
	ep->xfr_buf_len = buf_len;
	ep->xfr_ptr = NULL;
	ep->xfr_rem = 0;
	ep->state = EP_IDLE;
	ep->on_ev = info->on_ev;

	/* mask FIFO empty interrupt */
	otg_hs->diepempmsk &= ~(1 << ep_id);

	if (ep_id == 0) {
		ep->xfr_max = mxpktsz;

		/* Initialize EP0 */
		otg_hs->inep[0].diepctl = OTG_HS_TXFNUM_SET(0) |
			OTG_HS_EP0_MPSIZ_SET(mxpktsz);

		/* 3. Set up the Data FIFO RAM for each of the FIFOs
		   - Program the OTG_HS_GRXFSIZ register, to be able to receive
		   control OUT data and setup data. If thresholding is not enabled,
		   at a minimum, this must be equal to 1 max packet size of
		   control endpoint 0 + 2 Words (for the status of the control OUT
		   data packet) + 10 Words (for setup packets).
		   - Program the OTG_HS_TX0FSIZ register (depending on the FIFO number
		   chosen) to be able to transmit control IN data. At a minimum, this
		   must be equal to 1 max packet size of control endpoint 0. */

		__ep_pktbuf_alloc(drv, 0, mxpktsz);

		/*  4. Program the following fields in the endpoint-specific registers
			for control OUT endpoint 0 to receive a SETUP packet
			- STUPCNT = 3 in OTG_HS_DOEPTSIZ0 (to receive up to 3 back-to-back
			SETUP packets)
			At this point, all initialization required to receive SETUP packets
			is done. */

		/* cache this value to be reused */
		ep->doeptsiz = OTG_HS_STUPCNT_SET(3) | 
			OTG_HS_PKTCNT_SET(1) | OTG_HS_XFRSIZ_SET(40);

		/* Prepare to receive */
		otg_hs->outep[0].doeptsiz = ep->doeptsiz;
		/* EP enable */
		otg_hs->outep[0].doepctl |= OTG_HS_EPENA | OTG_HS_CNAK;

		/* Unmask EP0 interrupts */
		otg_hs->daintmsk = OTG_HS_IEPM0 | OTG_HS_OEPM0;

		DCC_LOG2(LOG_INFO, "TX[0]: addr=%04x size=%d",
				 OTG_HS_TX0FSA_GET(otg_hs->dieptxf0) * 4,
				 OTG_HS_TX0FD_GET(otg_hs->dieptxf0) * 4);
	} else {
		uint32_t depctl;

		if (info->addr & USB_ENDPOINT_IN) {
			DCC_LOG(LOG_INFO, "IN ENDPOINT");

			if ((info->attr & 0x03) == ENDPOINT_TYPE_BULK) {
				ep->xfr_max = 6 * mxpktsz;
			} else {
				ep->xfr_max = mxpktsz;
			}

			__ep_pktbuf_alloc(drv, ep_id, ep->xfr_max);
			depctl = otg_hs->inep[ep_id].diepctl;
		} else {
			DCC_LOG(LOG_INFO, "OUT ENDPOINT");
			depctl = otg_hs->outep[ep_id].doepctl;
		}

		depctl &= ~(OTG_HS_MPSIZ_MSK | OTG_HS_EPTYP_MSK | OTG_HS_TXFNUM_MSK);

		depctl |= OTG_HS_MPSIZ_SET(mxpktsz);
		depctl |= OTG_HS_SD0PID | OTG_HS_USBAEP;

		switch (info->attr & 0x03) {
		case ENDPOINT_TYPE_CONTROL:
			DCC_LOG(LOG_INFO, "ENDPOINT_TYPE_CONTROL");
			depctl |= OTG_HS_EPTYP_SET(OTG_HS_EPTYP_CTRL);
			break;

		case ENDPOINT_TYPE_ISOCHRONOUS:
			DCC_LOG(LOG_INFO, "ENDPOINT_TYPE_ISOCHRONOUS");
			depctl |= OTG_HS_EPTYP_SET(OTG_HS_EPTYP_ISOC);
			break;

		case ENDPOINT_TYPE_BULK:
			DCC_LOG(LOG_INFO, "ENDPOINT_TYPE_BULK");
			depctl |= OTG_HS_EPTYP_SET(OTG_HS_EPTYP_BULK);
			break;

		case ENDPOINT_TYPE_INTERRUPT:
			DCC_LOG(LOG_INFO, "ENDPOINT_TYPE_INTERRUPT");
			depctl |= OTG_HS_EPTYP_SET(OTG_HS_EPTYP_INT);
			break;
		}

		if (info->addr & USB_ENDPOINT_IN) {
			/* Activate IN endpoint */
			otg_hs->inep[ep_id].diepctl = depctl | OTG_HS_TXFNUM_SET(ep_id);

			/* Enable endpoint interrupt */
			otg_hs->daintmsk |= OTG_HS_IEPM(ep_id);
		} else {
			uint32_t rxfsiz;
			uint32_t pktcnt;

			/* get the size of the RX fifio */
			rxfsiz = otg_hs->grxfsiz * 4;

			pktcnt = rxfsiz / mxpktsz;

			/* cache this value to be reused */
			ep->doeptsiz = OTG_HS_PKTCNT_SET(pktcnt) |
				OTG_HS_XFRSIZ_SET(pktcnt * mxpktsz);

			/* Prepare EP_OUT to receive */
			otg_hs->outep[ep_id].doeptsiz = ep->doeptsiz;

			/* EP enable */
			/* FIXME: the single input fifo creates a problem as 
			   packets pending on the fifo for one endpoint blocks packets for 
			   other  endpoints. */
			/* XXX: set the nak on endpoint ???? */
			otg_hs->outep[ep_id].doepctl = depctl | OTG_HS_EPENA | OTG_HS_SNAK;

			/* Enable endpoint interrupt */
			otg_hs->daintmsk |= OTG_HS_OEPM(ep_id);
		}

	}

	/* 2. Once the endpoint is activated, the core starts decoding the
	   tokens addressed to that endpoint and sends out a valid
	   handshake for each valid token received for the
	   endpoint. */

	return ep_id;
}

#define OTG_HS_DP   STM32_GPIOB, 15
#define OTG_HS_DM   STM32_GPIOB, 14
#define OTG_HS_ID   STM32_GPIOB, 12

#ifdef STM32_OTG_HS_VBUS 
#define OTG_HS_VBUS STM32_OTG_HS_VBUS
#else
#define OTG_HS_VBUS STM32_GPIOB, 13 
#endif

static inline struct stm32_gpio * vbus_gpio(struct stm32_gpio *__gpio, 
											int __pin) {
	return __gpio;
}

static void otg_io_init(void)
{
	DCC_LOG(LOG_INFO, "Configuring GPIO pins...");

	if (vbus_gpio(OTG_HS_VBUS) == STM32_GPIOA)
		stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOA);

	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOB);

	stm32_gpio_af(OTG_HS_DP, GPIO_AF12);
	stm32_gpio_af(OTG_HS_DM, GPIO_AF12);
	stm32_gpio_af(OTG_HS_VBUS, GPIO_AF12);
#if 0
	stm32_gpio_af(OTG_HS_ID, GPIO_AF12);
#endif
	stm32_gpio_mode(OTG_HS_DP, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(OTG_HS_DM, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
#ifdef OTG_ENABLE_VBUS
	stm32_gpio_mode(OTG_HS_VBUS, ALT_FUNC, SPEED_LOW);
#else
	stm32_gpio_mode(OTG_HS_VBUS, INPUT, 0);
#endif
#if 0
	stm32_gpio_mode(OTG_HS_ID, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
#endif
}

static void otg_vbus_connect(bool connect)
{
#ifdef OTG_ENABLE_VBUS
	if (connect)
		stm32_gpio_mode(OTG_HS_VBUS, ALT_FUNC, SPEED_HIGH);
	else
		stm32_gpio_mode(OTG_HS_VBUS, INPUT, 0);
#endif
}

void otg_connect(struct stm32f_otg_hs * otg_hs)
{
	/* Connect device */
	otg_hs->dctl &= ~OTG_HS_SDIS;
	udelay(3000);

}

void otg_disconnect(struct stm32f_otg_hs * otg_hs)
{
	otg_hs->dctl |= OTG_HS_SDIS;
	udelay(3000);
}

static void otg_power_on(struct stm32f_otg_hs * otg_hs)
{
	DCC_LOG(LOG_INFO, "Enabling USB FS clock...");
	stm32_clk_enable(STM32_RCC, STM32_CLK_OTGHS);

	otg_connect(otg_hs);
	DCC_LOG(LOG_INFO, "[ATTACHED]");
 
	otg_vbus_connect(true);

	/* Enable Cortex interrupt */
	cm3_irq_enable(STM32F_IRQ_OTG_HS);
}

static void otg_power_off(struct stm32f_otg_hs * otg_hs)
{
	otg_disconnect(otg_hs);

	otg_hs_vbus_connect(false);

	DCC_LOG(LOG_INFO, "Disabling USB device clock...");
	stm32_clk_disable(STM32_RCC, STM32_CLK_OTGHS);
}

int stm32f_otg_hs_dev_init(struct stm32f_otg_drv * drv, usb_class_t * cl,
		const usb_class_events_t * ev)
{
	struct stm32f_otg_hs * otg_hs = drv->otg_hs;

	drv->cl = cl;
	drv->ev = ev;

	/* Initialize IO pins */
	otg_io_init();

	DCC_LOG(LOG_INFO, "Enabling USB HS clock...");
	stm32_clk_enable(STM32_RCC, STM32_CLK_OTGHS);
	
	DCC_LOG(LOG_TRACE, "Enabling USB HS clock during sleep...");
//	STM32_RCC->ahb1lpenr |= (1 << RCC_OTGHS);

	/* Initialize as a device */
	stm32f_otg_hs_device_init(otg_hs);
//	otg_hs_disconnect(otg_hs);

	/* Reset global interrupts mask */
	otg_hs->gintmsk = OTG_HS_WUIM |
		OTG_HS_SRQIM |
		OTG_HS_DISCINT |
		OTG_HS_CIDSCHGM |
		OTG_HS_FSUSPM |
		OTG_HS_IISOOXFRM |
		OTG_HS_IISOIXFRM |
		OTG_HS_OEPINTM |
		OTG_HS_IEPINTM |
		OTG_HS_EPMISM |
		OTG_HS_EOPFM |
		OTG_HS_ISOODRPM |
		OTG_HS_ENUMDNEM |
		OTG_HS_USBRSTM |
		OTG_HS_USBSUSPM |
		OTG_HS_ESUSPM |
		OTG_HS_GONAKEFFM |
		OTG_HS_GINAKEFFM |
//		OTG_HS_NPTXFEM |
		OTG_HS_RXFLVLM |
		OTG_HS_SOFM |
		OTG_HS_OTGINTM |
		OTG_HS_MMISM;


	
//	otg_hs_connect(otg_hs);
	/* Enable Cortex interrupt */
	cm3_irq_enable(STM32F_IRQ_OTG_HS);

	DCC_LOG(LOG_INFO, "----------------------------------------");

//	mdelay(500);

//	DCC_LOG(LOG_INFO, "........................................");

//	mdelay(500);

	DCC_LOG(LOG_INFO, "Done.");

	return 0;
}

/* 
 *  ---------------------------------------------------------------------------
 * Interrupt handling
 * ---------------------------------------------------------------------------
 */

static void stm32f_otg_hs_dev_ep_out(struct stm32f_otg_drv * drv, 
								  int ep_id, int len)
{
	struct stm32f_otg_ep * ep = &drv->ep[ep_id];
	struct stm32f_otg_hs * otg_hs = drv->otg_hs;

	DCC_LOG1(LOG_INFO, "ep_id=%d", ep_id);

	ep->xfr_rem = len;

	/* FIXME: the single input fifo creates a problem as 
	   packets pending on the fifo for one endpoint blocks packets for 
	   other  endpoints. Ex: an outstanding OUT packet may block a control
	   packet. Either the upper layer garantees the removal or
	   buffering at this layer should be implemented.
	 */
	/* XXX: set the nak on endpoint ???? */
	otg_hs->outep[ep_id].doepctl |= OTG_HS_SNAK;

	/* call class endpoint callback */
	ep->on_out(drv->cl, ep_id, len);
}


static void stm32f_otg_hs_dev_ep_in(struct stm32f_otg_drv * drv, int ep_id)
{
	struct stm32f_otg_ep * ep = &drv->ep[ep_id];

	DCC_LOG1(LOG_INFO, "ep_id=%d", ep_id);

	/* call class endpoint callback */
	ep->on_in(drv->cl, ep_id);
}

static void stm32f_otg_hs_dev_ep0_in(struct stm32f_otg_drv * drv)
{
	struct stm32f_otg_hs * otg_hs = drv->otg_hs;
	struct stm32f_otg_ep * ep = &drv->ep[0];

	if (ep->state == EP_WAIT_STATUS_IN) {
		struct usb_request * req = &drv->req;
		void * dummy = NULL;
		/* End of SETUP transaction (OUT Data Phase) */	
		ep->on_setup(drv->cl, req, dummy);
		ep->state = EP_IDLE;
		DCC_LOG(LOG_INFO, "EP0 [IDLE]");
		return;
	}


	if (ep->xfr_rem == 0) {
		otg_hs->diepempmsk &= ~(1 << 0);
		/* Prepare to receive */
		otg_hs->outep[0].doeptsiz = ep->doeptsiz;
		/* EP enable */
		otg_hs->outep[0].doepctl |= OTG_HS_EPENA | OTG_HS_CNAK;
		ep->state = EP_IN_DATA;
		DCC_LOG(LOG_INFO, "EP0 [IN_DATA]");
		return;
	} 
	
	if (stm32f_otg_hs_txf_setup(otg_hs, 0, ep->xfr_rem) < 0) {
		DCC_LOG(LOG_ERROR, "stm32f_otg_hs_txf_setup() failed!");
	} else {
		/* umask FIFO empty interrupt */
		otg_hs->diepempmsk |= (1 << 0);
	}
}

static void stm32f_otg_hs_dev_ep0_out(struct stm32f_otg_drv * drv)
{
	struct stm32f_otg_hs * otg_hs = drv->otg_hs;
	struct stm32f_otg_ep * ep = &drv->ep[0];

	/* last and only transfer */
	if (ep->state == EP_OUT_DATA_LAST) {
		ep->state = EP_WAIT_STATUS_IN;
		DCC_LOG(LOG_INFO, "EP0 [WAIT_STATUS_IN]");
		__ep_zlp_send(otg_hs, 0);
		return;
	}
}

static void stm32f_otg_hs_dev_ep0_setup(struct stm32f_otg_drv * drv)
{
	struct stm32f_otg_hs * otg_hs = drv->otg_hs;
	struct usb_request * req = &drv->req;
	struct stm32f_otg_ep * ep = &drv->ep[0];
	int len;

	DCC_LOG3(LOG_INFO, "type=0x%02x request=0x%02x len=%d ",
			req->type, req->request, req->length);

	/* No-Data control SETUP transaction */
	if (req->length == 0) {
		void * dummy = NULL;

		if (((req->request << 8) | req->type) == STD_SET_ADDRESS) {
			DCC_LOG1(LOG_INFO, "address=%d",  req->value);
			stm32f_otg_hs_addr_set(otg_hs, req->value);
		}
		ep->on_setup(drv->cl, req, dummy);
		ep->state = EP_IDLE;
		__ep_zlp_send(otg_hs, 0);
		return;
	}

	if (req->type & 0x80) {
		/* Control Read SETUP transaction (IN Data Phase) */
		DCC_LOG(LOG_INFO, "EP0 [SETUP] IN Dev->Host");
		ep->xfr_ptr = NULL;
		len = ep->on_setup(drv->cl, req, (void *)&ep->xfr_ptr);
		ep->xfr_rem = MIN(req->length, len);
		DCC_LOG1(LOG_INFO, "EP0 data lenght = %d", ep->xfr_rem);
		/* prepare fifo to transmit */
		if (stm32f_otg_hs_txf_setup(otg_hs, 0, ep->xfr_rem) < 0) {
			DCC_LOG(LOG_WARNING, "stm32f_otg_hs_txf_setup() failed!");
		} else {
			/* umask FIFO empty interrupt */
			otg_hs->diepempmsk |= (1 << 0);
		}
	} else {
		/* Control Write SETUP transaction (OUT Data Phase) */
		ep->xfr_ptr = ep->xfr_buf;
		ep->xfr_rem = req->length;

		DCC_LOG1(LOG_INFO, "xfr_ptr=0x%08x", ep->xfr_ptr);

		if (ep->xfr_rem > ep->xfr_buf_len) {
			ep->xfr_rem = ep->xfr_buf_len;
			DCC_LOG(LOG_ERROR, "transfer to large to fit in the buffer!");
		}

		if (ep->xfr_rem < ep->xfr_max) {
			/* last and only transfer */
			ep->state = EP_OUT_DATA_LAST;
			DCC_LOG(LOG_INFO, "EP0 [OUT_DATA_LAST] OUT Host->Dev!!!!");
		} else {
			ep->state = EP_OUT_DATA;
			DCC_LOG(LOG_INFO, "EP0 [OUT_DATA] OUT Host->Dev!!!!");
		}

		DCC_LOG(LOG_INFO, "Prepare to receive");

		/* Prepare to receive */
		otg_hs->outep[0].doeptsiz = OTG_HS_STUPCNT_SET(0) |
			OTG_HS_PKTCNT_SET(1) | OTG_HS_XFRSIZ_SET(ep->xfr_max);

		/* EP enable */
		otg_hs->outep[0].doepctl |= OTG_HS_EPENA | OTG_HS_CNAK;
	}

}

static void stm32f_otg_hs_dev_reset(struct stm32f_otg_drv * drv)
{
	struct stm32f_otg_hs * otg_hs = drv->otg_hs;
	uint32_t siz;
	int i;

	DCC_LOG(LOG_MSG, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");

	/* Clear the Remote Wake-up Signaling */
	otg_hs->dctl &= ~OTG_HS_RWUSIG;

	/* Clear all pending interrupts */
	otg_hs->diepmsk = 0;
	otg_hs->doepmsk = 0;
	otg_hs->daint = 0xffffffff;
	otg_hs->daintmsk = 0;
	otg_hs->diepempmsk = 0;
	for (i = 0; i < 4; i++) {
		otg_hs->inep[i].diepint = 0xff;
		otg_hs->outep[i].doepint = 0xff;
		otg_hs->inep[i].dieptsiz = 0;

		drv->ep[i].xfr_rem = 0;
	}

	/* Flush the Tx FIFO */
	stm32f_otg_hs_txfifo_flush(otg_hs, TXFIFO_ALL);
	/* Flush the Rx FIFO */
	stm32f_otg_hs_rxfifo_flush(otg_hs);

	/* Reset global interrupts mask */
	otg_hs->gintmsk = OTG_HS_SRQIM | OTG_HS_OTGINT | OTG_HS_WUIM |
		OTG_HS_USBRSTM | OTG_HS_ENUMDNEM |
		OTG_HS_ESUSPM | OTG_HS_USBSUSPM;

	/* Reset Device Address */
	otg_hs->dcfg &= ~OTG_HS_DAD_MSK;

	/*  Set global IN NAK */
	otg_hs->dctl |= OTG_HS_SGINAK;

	/* Endpoint initialization on USB reset */

	/* 1. Set the NAK bit for all OUT endpoints
	   - SNAK = 1 in OTG_HS_DOEPCTLx (for all OUT endpoints) */
	for (i = 0; i < 4; i++) {
		otg_hs->outep[i].doepctl = OTG_HS_SNAK;
	}

	/* 2. Unmask the following interrupt bits
	   - INEP0 = 1 in OTG_HS_DAINTMSK (control 0 IN endpoint)
	   - OUTEP0 = 1 in OTG_HS_DAINTMSK (control 0 OUT endpoint)
	   - STUP = 1 in DOEPMSK
	   - XFRC = 1 in DOEPMSK
	   - XFRC = 1 in DIEPMSK
	   - TOC = 1 in DIEPMSK */
	otg_hs->doepmsk = OTG_HS_STUPM | OTG_HS_XFRCM | OTG_HS_EPDM;
	otg_hs->diepmsk = OTG_HS_TOM | OTG_HS_EPDM | OTG_HS_XFRCM;

	/* 3. Set up the Data FIFO RAM for each of the FIFOs
	   - Program the OTG_HS_GRXFSIZ register, to be able to receive
	   control OUT data and setup data. If thresholding is not enabled,
	   at a minimum, this must be equal to 1 max packet size of
	   control endpoint 0 + 2 Words (for the status of the control OUT
	   data packet) + 10 Words (for setup packets).
	   - Program the OTG_HS_TX0FSIZ register (depending on the FIFO number
	   chosen) to be able to transmit control IN data. At a minimum, this
	   must be equal to 1 max packet size of control endpoint 0. */

	/* reset fifo memory (packet buffer) allocation pointer */
	drv->fifo_addr = 0;
	/* initialize RX fifo size */
	siz = RX_FIFO_SIZE;
	otg_hs->grxfsiz = siz / 4;
	/* update fifo memory allocation pointer */
	drv->fifo_addr += siz;

	DCC_LOG2(LOG_INFO, "   RX: addr=%04x size=%d",
			 0, otg_hs->grxfsiz * 4);

	drv->ev->on_reset(drv->cl);
}

/* Private USB device driver data */
struct stm32f_otg_drv stm32f_otg_hs_drv0 = {
	.otg_hs = STM32F_OTG_HS
};

void stm32f_otg_hs_isr(void)
{
	struct stm32f_otg_drv * drv = &stm32f_otg_hs_drv0;
	struct stm32f_otg_hs * otg_hs = drv->otg_hs;
	uint32_t gintsts;
	uint32_t ep_intr;

	gintsts = otg_hs->gintsts & otg_hs->gintmsk;

	DCC_LOG1(LOG_JABBER, "GINTSTS=0x%08x", gintsts);

	if (gintsts & OTG_HS_IEPINT) {
		uint32_t diepmsk;
		uint32_t diepint;
		uint32_t diepempmsk;
		uint32_t msk;

		DCC_LOG(LOG_JABBER, "<IEPINT>");

		ep_intr = (otg_hs->daint & otg_hs->daintmsk);
		diepmsk = otg_hs->diepmsk;
		diepempmsk = otg_hs->diepempmsk;

		if (ep_intr & OTG_HS_IEPINT0) {
			/* add the Transmit FIFO empty bit to the mask */
			msk = diepmsk | ((diepempmsk >> 0) & 0x1) << 7;
			diepint = otg_hs->inep[0].diepint & msk;
			/* clear interrupts */
			otg_hs->inep[0].diepint = diepint;
			if (diepint & OTG_HS_XFRC) {
				DCC_LOG(LOG_MSG, "[0] <IEPINT> <XFRC>");
				stm32f_otg_hs_dev_ep0_in(drv);
			} else if (diepint & OTG_HS_TXFE) {
				DCC_LOG(LOG_MSG, "[0] <IEPINT> <TXFE>");
				__ep_tx_push(drv, 0);
			}
		}

		if (ep_intr & OTG_HS_IEPINT1) {
			/* add the Transmit FIFO empty bit to the mask */
			msk = diepmsk | ((diepempmsk >> 1) & 0x1) << 7;
			diepint = otg_hs->inep[1].diepint & msk;
			/* clear interrupts */
			otg_hs->inep[1].diepint = diepint;
			if (diepint & OTG_HS_TXFE) {
				DCC_LOG(LOG_INFO, "[1] <IEPINT> <TXFE>");
				__ep_tx_push(drv, 1);
			}
			if (diepint & OTG_HS_XFRC) {
				DCC_LOG(LOG_INFO, "[1] <IEPINT> <XFRC>");
				stm32f_otg_hs_dev_ep_in(drv, 1);
			}
		}

		if (ep_intr & OTG_HS_IEPINT2) {
			/* add the Transmit FIFO empty bit to the mask */
			msk = diepmsk | ((diepempmsk >> 2) & 0x1) << 7;
			diepint = otg_hs->inep[2].diepint & msk;
			otg_hs->inep[2].diepint = diepint;
			if (diepint & OTG_HS_TXFE) {
				DCC_LOG(LOG_INFO, "[2] <IEPINT> <TXFE>");
				__ep_tx_push(drv, 2);
			}
			if (diepint & OTG_HS_XFRC) {
				DCC_LOG(LOG_INFO, "[2] <IEPINT> <XFRC>");
				stm32f_otg_hs_dev_ep_in(drv, 2);
			}
		}

#if (EP_MAX > 3)
		if (ep_intr & OTG_HS_IEPINT3) {
			/* add the Transmit FIFO empty bit to the mask */
			msk = diepmsk | ((diepempmsk >> 3) & 0x1) << 7;
			diepint = otg_hs->inep[3].diepint & msk;
			otg_hs->inep[3].diepint = diepint;
			if (diepint & OTG_HS_TXFE) {
				DCC_LOG(LOG_INFO, "[3] <IEPINT> <TXFE>");
				__ep_tx_push(drv, 3);
			}
			if (diepint & OTG_HS_XFRC) {
				DCC_LOG(LOG_INFO, "[3] <IEPINT> <XFRC>");
				stm32f_otg_hs_dev_ep_in(drv, 3);
			}
		}
#endif
	}

	if (gintsts & OTG_HS_OEPINT) {
		ep_intr = (otg_hs->daint & otg_hs->daintmsk);

		DCC_LOG(LOG_MSG, "<OEPINT>");

		if (ep_intr & OTG_HS_OEPINT0) {
			uint32_t doepint;

			doepint = otg_hs->outep[0].doepint & otg_hs->doepmsk;

			if (doepint & OTG_HS_XFRC) {
				DCC_LOG(LOG_INFO, "[0] <OEPINT> <OUT XFRC>");
				stm32f_otg_hs_dev_ep0_out(drv);
			}
			if (doepint & OTG_HS_EPDISD) {
				DCC_LOG(LOG_INFO, "[0] <OEPINT> <EPDISD>");
			}
			if (doepint & OTG_HS_STUP) {

				DCC_LOG(LOG_INFO, "[0] <OEPINT> <STUP>");
#if 0
				uint32_t doeptsiz;
				int pktcnt;
				int stupcnt;
				int xfrsiz;


				doeptsiz = otg_hs->outep[0].doeptsiz;
				stupcnt = OTG_HS_STUPCNT_GET(doeptsiz);
				(void)stupcnt;
				pktcnt = OTG_HS_PKTCNT_GET(doeptsiz);
				(void)pktcnt;
				xfrsiz = OTG_HS_XFRSIZ_GET(doeptsiz);
				(void)xfrsiz;

				DCC_LOG3(LOG_INFO, "stupcnt=%d pktcnt=%d xfrsiz=%d",
						stupcnt, pktcnt, xfrsiz);
#endif
				stm32f_otg_hs_dev_ep0_setup(drv);
			}
			/* clear interrupts */
			otg_hs->outep[0].doepint = doepint;
		}

		if (ep_intr & OTG_HS_OEPINT1) {
			uint32_t doepint;

			doepint = otg_hs->outep[1].doepint & otg_hs->doepmsk;

			if (doepint & OTG_HS_XFRC) {
				DCC_LOG(LOG_INFO, "[1] <OEPINT> <OUT XFRC>");
			}
			if (doepint & OTG_HS_EPDISD) {
				DCC_LOG(LOG_INFO, "[1] <OEPINT> <EPDISD>");
			}
			if (doepint & OTG_HS_STUP) {
				DCC_LOG(LOG_INFO, "[1] <OEPINT> <STUP>");
			}
			/* clear interrupts */
			otg_hs->outep[1].doepint = doepint;
		}

		if (ep_intr & OTG_HS_OEPINT2) {
			uint32_t doepint;

			doepint = otg_hs->outep[2].doepint & otg_hs->doepmsk;

			if (doepint & OTG_HS_XFRC) {
				DCC_LOG(LOG_INFO, "[2] <OEPINT> <OUT XFRC>");
			}
			if (doepint & OTG_HS_EPDISD) {
				DCC_LOG(LOG_INFO, "[2] <OEPINT> <EPDISD>");
			}
			if (doepint & OTG_HS_STUP) {
				DCC_LOG(LOG_INFO, "[2] <OEPINT> <STUP>");
			}
			/* clear interrupts */
			otg_hs->outep[2].doepint = doepint;
		}

#if (EP_MAX > 3)
		if (ep_intr & OTG_HS_OEPINT3) {
			uint32_t doepint;

			doepint = otg_hs->outep[3].doepint & otg_hs->doepmsk;

			if (doepint & OTG_HS_XFRC) {
				DCC_LOG(LOG_INFO, "[3] <OEPINT> <OUT XFRC>");
			}
			if (doepint & OTG_HS_EPDISD) {
				DCC_LOG(LOG_INFO, "[3] <OEPINT> <EPDISD>");
			}
			if (doepint & OTG_HS_STUP) {
				DCC_LOG(LOG_INFO, "[3] <OEPINT> <STUP>");
			}

			/* clear interrupts */
			otg_hs->outep[3].doepint = doepint;
		}
#endif
	}

	/* RxFIFO non-empty */
	if (gintsts & OTG_HS_RXFLVL) {
		uint32_t grxsts;
		int epnum;
		int len;
		int stat;

		/* 1. On catching an RXFLVL interrupt (OTG_HS_GINTSTS register),
		   the application must read the Receive status pop
		   register (OTG_HS_GRXSTSP). */
		grxsts = otg_hs->grxstsp;

		epnum = OTG_HS_EPNUM_GET(grxsts);
		len = OTG_HS_BCNT_GET(grxsts);
		(void)len;
		stat = OTG_HS_PKTSTS_GET(grxsts);
		(void)stat;

		DCC_LOG3(LOG_MSG, "[%d] <RXFLVL> len=%d status=%d", epnum, len, stat);

		if (epnum == 0) {

			/* 3. If the received packet’s byte count is not 0, the byte count
			   amount of data is popped from the receive Data FIFO and stored in
			   memory. If the received packet byte count is 0, no data is popped
			   from the receive data FIFO. */
			switch (grxsts & OTG_HS_PKTSTS_MSK) {
			case OTG_HS_PKTSTS_GOUT_NACK:
				/* Global OUT NAK (triggers an interrupt) */
				DCC_LOG1(LOG_INFO, "[%d] <RXFLVL> <GOUT_NACK>", epnum);
				break;
			case OTG_HS_PKTSTS_OUT_DATA_UPDT: {
				/* OUT data packet received */
				DCC_LOG1(LOG_INFO, "[%d] <RXFLVL> <OUT_DATA_UPDT>", epnum);
				__ep_rx_pop(drv, 0, len);
				break;
			}
			case OTG_HS_PKTSTS_OUT_XFER_COMP:
				DCC_LOG1(LOG_INFO, "[%d] <RXFLVL> <OUT_XFER_COMP>", epnum);
				break;
			case OTG_HS_PKTSTS_SETUP_COMP:
				/* SETUP transaction completed (triggers an interrupt) */
				DCC_LOG1(LOG_INFO, "[%d] <RXFLVL> <SETUP_COMP>", epnum);
				break;
			case OTG_HS_PKTSTS_SETUP_UPDT:
				/* SETUP data packet received */
				DCC_LOG1(LOG_INFO, "[%d] <RXFLVL> <SETUP_UPDT>", epnum);

				if (len != 8) {
					DCC_LOG(LOG_ERROR, "setup data len != 8!");
				}

				/* Copy the received setup packet into the setup buffer */
				__copy_from_pktbuf(&drv->req, &otg_hs->dfifo[0].pop, len);

				DCC_LOG3(LOG_MSG, "type=0x%02x request=0x%02x len=%d ",
						drv->req.type, drv->req.request, drv->req.length);

				break;
			}
		} else {
			switch (grxsts & OTG_HS_PKTSTS_MSK) {
			case OTG_HS_PKTSTS_OUT_DATA_UPDT:
				/* OUT data packet received */
				DCC_LOG1(LOG_INFO, "[%d] <RXFLVL> <OUT_DATA_UPDT>", epnum);
				/* 2. The application can mask the RXFLVL interrupt (in
				   OTG_HS_GINTSTS) by writing to RXFLVL = 0 (in
				   OTG_HS_GINTMSK), until it has read the packet from
				   the receive FIFO. */
				otg_hs->gintmsk &= ~OTG_HS_RXFLVLM;
				/* Enable SOF interrupts */
//				otg_hs->gintmsk |=  OTG_HS_SOFM;
//				__ep_rx_pop(drv, 0, len);
				stm32f_otg_hs_dev_ep_out(drv, epnum, len);
				break;
			case OTG_HS_PKTSTS_OUT_XFER_COMP:
				DCC_LOG1(LOG_INFO, "[%d] <RXFLVL> <OUT_XFER_COMP>", epnum);
				DCC_LOG2(LOG_MSG, "[%d] doeptsiz=%08x", epnum, 
						 otg_hs->outep[epnum].doeptsiz);	 
				/* Prepare to receive more */
				otg_hs->outep[epnum].doeptsiz = drv->ep[epnum].doeptsiz;
				/* EP enable */
				/* FIXME: not clearing the NAK here reduces the performance,
				   but we have to garantee tha the class driver
				   periodically remove the packets from 
				   the fifo. Otherwise the EP0 will not receive its
				   packets and we end up with a deadlock situation */
//				otg_hs->outep[epnum].doepctl |= OTG_HS_EPENA | OTG_HS_CNAK;
				/* Disable SOF interrupts */
//				otg_hs->gintmsk &= ~OTG_HS_SOFM;
				break;
			}
		}

		/* 5. After the data payload is popped from the receive FIFO, the
		   RXFLVL interrupt (OTG_HS_GINTSTS) must be unmasked. */
		//	otg_hs->gintmsk |= OTG_HS_RXFLVLM;
	}

	if (gintsts & OTG_HS_SRQINT) {
		/* Session request/new session detected interrupt */
		DCC_LOG(LOG_MSG, "<SRQINT>  [POWERED]");
		otg_hs->gintmsk |= OTG_HS_WUIM | OTG_HS_USBRSTM | OTG_HS_ENUMDNEM |
			OTG_HS_ESUSPM | OTG_HS_USBSUSPM;
	}

	if (gintsts & OTG_HS_OTGINT) {
		uint32_t gotgint = otg_hs->gotgint;
		DCC_LOG(LOG_INFO, "<OTGINT>");
		if (gotgint & OTG_HS_SEDET) {
			DCC_LOG(LOG_INFO, "<SEDET> [ATTACHED]");
			otg_hs->gintmsk = OTG_HS_SRQIM | OTG_HS_OTGINT;
		}
		otg_hs->gotgint = gotgint;
	}

	if (gintsts & OTG_HS_ENUMDNE) {
		uint32_t dsts = otg_hs->dsts;
		(void)dsts;
		
		DCC_LOG3(LOG_INFO, "<ENUMDNE> DSTS={%s%s ENUMSPD=%d }", 
				 (dsts & OTG_HS_EERR) ? " EERR" : "",
				 (dsts & OTG_HS_SUSPSTS) ? " SUSPSTS" : "",
				 OTG_HS_ENUMSPD_GET(dsts));
		/* Unmask global interrupts */
		otg_hs->gintmsk |=  OTG_HS_IEPINTM | OTG_HS_OEPINTM |
			OTG_HS_IISOIXFRM | OTG_HS_IISOOXFRM | OTG_HS_RXFLVLM;
		/*  Clear global IN NAK */
		otg_hs->dctl |= OTG_HS_CGINAK;

	   /* Wait for the ENUMDNE interrupt in OTG_HS_GINTSTS.
	   This interrupt indicates the end of reset on the USB. On receiving this 
	   interrupt, the application must read the OTG_HS_DSTS register to 
	   determine the enumeration speed and perform the steps listed in 
	   Endpoint initialization on enumeration completion on page 1035.
	   At this point, the device is ready to accept SOF packets and perform 
	   control transfers on control endpoint 0. */

	}


	if (gintsts & OTG_HS_USBRST ) {
		/* end of bus reset */
		DCC_LOG(LOG_INFO, "<USBRST> --------------- [DEFAULT]");
		stm32f_otg_hs_dev_reset(drv);
	}

	if (gintsts & OTG_HS_ESUSP) {
		uint32_t dsts = otg_hs->dsts;
		(void)dsts;

		DCC_LOG4(LOG_INFO, "<ESUSP> %s%s ENUMSPD=%d %s", 
				 (dsts & OTG_HS_EERR) ? " EERR" : "",
				 (dsts & OTG_HS_SUSPSTS) ? " SUSPSTS" : "",
				 OTG_HS_ENUMSPD_GET(dsts),
				 otg_hs->pcgcctl & OTG_HS_PHYSUSP ? "PHYSUSP" : "");
	}

#if DEBUG
	if (gintsts & OTG_HS_GONAKEFF) {
		DCC_LOG(LOG_INFO, "<GONAKEFF>");
		otg_hs->gintmsk &= ~OTG_HS_GONAKEFFM;
	}

	if (gintsts & OTG_FS_SOF) {
		DCC_LOG(LOG_MSG, "<SOF>");
	}

	if (gintsts & OTG_HS_PTXFE) {
		DCC_LOG(LOG_INFO, "<PTXFE>");
	}

	if (gintsts & OTG_HS_WKUPINT) {
		DCC_LOG(LOG_INFO, "<WKUPINT>");
		/* disable resume/wakeup interrupts */
	}

	if (gintsts & OTG_HS_USBSUSP) {
		DCC_LOG(LOG_INFO, "<USBSUSP>");
	}

	if (gintsts & OTG_HS_IISOIXFR) {
		DCC_LOG(LOG_INFO, "<IISOIXFR>");
	}

	if (gintsts & OTG_HS_INCOMPISOOUT) {
		DCC_LOG(LOG_INFO, "<INCOMPISOOUT>");
	}

	if (gintsts & OTG_HS_MMIS) {
		DCC_LOG(LOG_INFO, "<MMIS>");
	}

	if (gintsts & OTG_HS_CIDSCHG) {
		DCC_LOG(LOG_INFO, "<CIDSCHG>");
	}

	if (gintsts & OTG_HS_CMOD) {
		DCC_LOG(LOG_INFO, "<CMOD>");
	}
#endif

	/* clear pending interrupts */
	otg_hs->gintsts = gintsts;
}


void stm32f_otg_hs_ep1_out_isr(void)
{
	DCC_LOG(LOG_WARNING, "not implemented!");
}

void stm32f_otg_hs_ep1_in_isr(void)
{
	DCC_LOG(LOG_WARNING, "not implemented!");
}

/* USB device operations */
const struct usb_dev_ops stm32f_otg_hs_ops = {
	.dev_init = (usb_dev_init_t)stm32f_otg_hs_dev_init,
	.ep_init = (usb_dev_ep_init_t)stm32f_otg_hs_dev_ep_init,
	.ep_ctl = (usb_dev_ep_ctl_t)stm32f_otg_hs_dev_ep_ctl,
	.ep_pkt_xmit = (usb_dev_ep_pkt_xmit_t)stm32f_otg_hs_dev_ep_pkt_xmit,
	.ep_pkt_recv = (usb_dev_ep_pkt_recv_t)stm32f_otg_hs_dev_ep_pkt_recv
};

/* USB device driver */
const struct usb_dev stm32f_otg_hs_dev = {
	.priv = (void *)&stm32f_otg_hs_drv0,
	.irq[0] = STM32F_IRQ_OTG_HS,
	.irq_cnt = 1,
	.op = &stm32f_otg_hs_ops
};

#endif /* STM32_ENABLE_OTG_HS */

#endif /* STM32F_OTG_HS */


