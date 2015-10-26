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
 * @file stm32f-otg_fs_dev.c
 * @brief YARD-ICE UART console
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

#include <thinkos.h>

#include <sys/dcclog.h>

#ifndef STM32_ENABLE_OTG_FS 
#define STM32_ENABLE_OTG_FS 0
#endif 

#ifndef STM32_OTG_FS_EP_MAX 
#define STM32_OTG_FS_EP_MAX 4
#endif

#ifndef STM32_OTG_FS_VBUS_ENABLE
#define STM32_OTG_FS_VBUS_ENABLE 1
#endif

#if defined(STM32F_OTG_FS) &&  (STM32_ENABLE_OTG_FS)

#define OTG_EP_MAX          STM32_OTG_FS_EP_MAX 
#define OTG_VBUS_ENABLE     STM32_OTG_FS_VBUS_ENABLE
#define OTG_RX_FIFO_SIZE    512

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
	EP_IN_DATA_ZLP,
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
	struct stm32f_otg_fs * otg_fs;
	struct stm32f_otg_ep ep[OTG_EP_MAX];
	usb_class_t * cl;
	const struct usb_class_events * ev;
	struct usb_request req;
	uint16_t fifo_addr;
};

/* EP TX fifo memory allocation */
static void __ep_pktbuf_alloc(struct stm32f_otg_drv * drv, 
							  int ep_id, int siz)
{
	struct stm32f_otg_fs * otg_fs = drv->otg_fs;

	switch (ep_id) {
	case 0:
		otg_fs->dieptxf0 = OTG_FS_TX0FD_SET(siz / 4) | 
			OTG_FS_TX0FSA_SET(drv->fifo_addr / 4);
		break;
	case 1:
		otg_fs->dieptxf1 = OTG_FS_INEPTXFD_SET(siz / 4) | 
			OTG_FS_INEPTXSA_SET(drv->fifo_addr / 4);
		break;
	case 2:
		otg_fs->dieptxf2 = OTG_FS_INEPTXFD_SET(siz / 4) | 
			OTG_FS_INEPTXSA_SET(drv->fifo_addr / 4);
		break;
#if (OTG_EP_MAX > 3)
	case 3:
		otg_fs->dieptxf3 = OTG_FS_INEPTXFD_SET(siz / 4) | 
			OTG_FS_INEPTXSA_SET(drv->fifo_addr / 4);
		break;
#endif
	}

	DCC_LOG2(LOG_MSG, "addr=%d siz=%d", drv->fifo_addr, siz);

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

static void __ep_rx_pop(struct stm32f_otg_drv * drv, int ep_id, int len)
{
	struct stm32f_otg_fs * otg_fs = drv->otg_fs;
	volatile uint32_t * pop = &otg_fs->dfifo[ep_id].pop;
	struct stm32f_otg_ep * ep = &drv->ep[ep_id];
	uint8_t * dst;
	uint32_t data;
	int wcnt;
	int i;

	/* Number of words in the receive fifo */
	wcnt = (len + 3) / 4;
	DCC_LOG1(LOG_MSG, "poping %d words from FIFO.", wcnt);

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

static void __ep_zlp_send(struct stm32f_otg_fs * otg_fs, int epnum)
{
	DCC_LOG(LOG_MSG, "Send: ZLP");

	otg_fs->inep[epnum].dieptsiz = OTG_FS_PKTCNT_SET(1) | OTG_FS_XFRSIZ_SET(0);
	otg_fs->inep[epnum].diepctl |= OTG_FS_EPENA | OTG_FS_CNAK;
}

int stm32f_otg_dev_ep_pkt_xmit(struct stm32f_otg_drv * drv, int ep_id,
							   void * buf, unsigned int len)
{
	struct stm32f_otg_fs * otg_fs = drv->otg_fs;
	struct stm32f_otg_ep * ep;
	uint32_t deptsiz;
	uint32_t depctl;
	uint32_t mpsiz;
	uint32_t xfrsiz;
	uint32_t pktcnt;

	if ((unsigned int)ep_id >= OTG_EP_MAX) {
		DCC_LOG(LOG_WARNING, "invalid EP");
		return -1;
	}

	deptsiz = otg_fs->inep[ep_id].dieptsiz;
	if ((!OTG_FS_XFRSIZ_GET(deptsiz)) && (OTG_FS_PKTCNT_GET(deptsiz))) {
		DCC_LOG3(LOG_WARNING, "ep_id=%d len=%d %d outstanding packets in FIFO", 
				 ep_id, len, OTG_FS_PKTCNT_GET(deptsiz));
		return -1;
	}

	depctl = otg_fs->inep[ep_id].diepctl;
	if (ep_id == 0)
		mpsiz = OTGFS_EP0_MPSIZ_GET(depctl);
	else
		mpsiz = OTG_FS_MPSIZ_GET(depctl);

	if (len > 0) {
		/* XXX: check whether to get rid of this division or not,
		 if the CM3 div is used it is not necessary.... */
		pktcnt = (len + (mpsiz - 1)) / mpsiz;
		if (pktcnt > 6) {
			pktcnt = 6;
			xfrsiz = 6 * mpsiz;
		} else {
			xfrsiz = len;
		}
	} else {
		/* zero lenght packet */
		pktcnt = 1;
		xfrsiz = 0;
	}

	ep = &drv->ep[ep_id];
	ep->xfr_ptr = buf;
	ep->xfr_rem = xfrsiz;
	if ((xfrsiz % mpsiz) == 0) 
		ep->state = EP_IN_DATA_ZLP;
	else
		ep->state = EP_IN_DATA;

	DCC_LOG4(LOG_INFO, "ep_id=%d pktcnt=%d xfrsiz=%d xfr_max=%d", 
			 ep_id, pktcnt, xfrsiz, ep->xfr_max);

	otg_fs->inep[ep_id].dieptsiz = OTG_FS_PKTCNT_SET(pktcnt) | 
		OTG_FS_XFRSIZ_SET(xfrsiz); 
	/* enable end point, clear NACK */
	otg_fs->inep[ep_id].diepctl = depctl | OTG_FS_EPENA | OTG_FS_CNAK; 
	/* umask FIFO empty interrupt */
	otg_fs->diepempmsk |= (1 << ep_id);

	return xfrsiz;
}

static bool __ep_tx_push(struct stm32f_otg_drv * drv, int ep_id)
{
	struct stm32f_otg_ep * ep = &drv->ep[ep_id];
	struct stm32f_otg_fs * otg_fs = drv->otg_fs;
	uint32_t depctl;
	uint32_t deptsiz;
	uint32_t mpsiz;
	uint32_t xfrsiz;
	uint32_t pktcnt;
	uint32_t free;
	uint32_t data;
	void * buf = ep->xfr_ptr;
	uint8_t * cp;
	int cnt;
	int i;

	free = otg_fs->inep[ep_id].dtxfsts * 4;
	depctl = otg_fs->inep[ep_id].diepctl;
	deptsiz = otg_fs->inep[ep_id].dieptsiz;
	if (ep_id == 0)
		mpsiz = OTGFS_EP0_MPSIZ_GET(depctl);
	else
		mpsiz = OTG_FS_MPSIZ_GET(depctl);
	xfrsiz = OTG_FS_XFRSIZ_GET(deptsiz);
	pktcnt = OTG_FS_PKTCNT_GET(deptsiz);
	(void)pktcnt;

	DCC_LOG5(LOG_INFO, "ep_id=%d mpsiz=%d pktcnt=%d xfrsiz=%d free=%d", 
			 ep_id, mpsiz, pktcnt, xfrsiz, free);

	if (xfrsiz < mpsiz) {
		if (free < xfrsiz) {
			DCC_LOG(LOG_PANIC, "free < xfrsiz !!!");
			otg_fs->diepempmsk &= ~(1 << ep_id);
			return false;
		}
		/* Transfer the last partial packet */
		cnt = xfrsiz;
	} else {
		if (free < mpsiz) {
			DCC_LOG(LOG_PANIC, "free < mpsiz !!!");
			otg_fs->diepempmsk &= ~(1 << ep_id);
			return false;
		}
		if (free < xfrsiz) {
			/* Transfer only full packets */
			cnt = (free / mpsiz) * mpsiz;
		} else {
			/* Transfer all */
			cnt = xfrsiz;
		}
	}

	/* push into fifo */
	cp = (uint8_t *)buf;
	for (i = 0; i < cnt; i += 4) {
		data = cp[0] + (cp[1] << 8) + (cp[2] << 16) + (cp[3] << 24);
		otg_fs->dfifo[ep_id].push = data;
		cp += 4;
	}	

	DCC_LOG2(LOG_MSG, "Tx: (%d) free=%d", cnt, 
			 otg_fs->inep[ep_id].dtxfsts * 4 );

	ep->xfr_ptr += cnt;
	ep->xfr_rem -= cnt;

	return true;
}

int stm32f_otg_dev_ep_pkt_recv(struct stm32f_otg_drv * drv, int ep_id,
							   void * buf, int len)
{
	struct stm32f_otg_fs * otg_fs = drv->otg_fs;
	uint8_t * cp = (uint8_t *)buf;
	struct stm32f_otg_ep * ep;
	uint32_t data;
	int cnt;
	int rem;

	if ((unsigned int)ep_id >= OTG_EP_MAX) {
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
		data = otg_fs->dfifo[ep_id].pop;
		cp[0] = data;
		cp[1] = data >> 8;
		cp[2] = data >> 16;
		cp[3] = data >> 24;
		cp += 4;
		rem -= 4;
	}

	if (rem > 0) {
		/* remaining data */
		data = otg_fs->dfifo[ep_id].pop;
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
			data = otg_fs->dfifo[ep_id].pop;
			(void)data;
			rem -= 4;
		} while (rem > 0);
	}

	/* reset transfer pointer */
	ep->xfr_rem = 0;

	/* 5. After the data payload is popped from the receive FIFO, the 
	   RXFLVL interrupt (OTG_FS_GINTSTS) must be unmasked. */
	DCC_LOG1(LOG_INFO, "cnt=%d enabling RXFLVL interrupt", cnt);

	/* Reenable RX fifo interrupts */
	otg_fs->gintmsk |= OTG_FS_RXFLVLM;

	return cnt;
}


int stm32f_otg_dev_ep_ctl(struct stm32f_otg_drv * drv, 
						  int ep_id, unsigned int opt)
{
	struct stm32f_otg_fs * otg_fs = drv->otg_fs;
	struct stm32f_otg_ep * ep = &drv->ep[ep_id];

	DCC_LOG2(LOG_INFO, "ep=%d opt=%d", ep_id, opt);

	switch (opt) {
	case USB_EP_RECV_OK:
	case USB_EP_NAK_CLR:
		/* Clear NAK, prepare to receive  ... */
		otg_fs->outep[ep_id].doepctl |= OTG_FS_CNAK;
		break;

	case USB_EP_NAK_SET:
		otg_fs->outep[ep_id].doepctl |= OTG_FS_SNAK;
		break;

	case USB_EP_ZLP_SEND:
		__ep_zlp_send(otg_fs, ep_id);
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


int stm32f_otg_dev_ep_init(struct stm32f_otg_drv * drv, 
						   const usb_dev_ep_info_t * info, 
						   void * xfr_buf, int buf_len)
{
	struct stm32f_otg_fs * otg_fs = drv->otg_fs;
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
	otg_fs->diepempmsk &= ~(1 << ep_id);

	if (ep_id == 0) {
		ep->xfr_max = mxpktsz;

		/* Initialize EP0 */
		otg_fs->inep[0].diepctl = OTG_FS_TXFNUM_SET(0) |
			OTG_FS_EP0_MPSIZ_SET(mxpktsz);

		/* 3. Set up the Data FIFO RAM for each of the FIFOs
		   - Program the OTG_FS_GRXFSIZ register, to be able to receive
		   control OUT data and setup data. If thresholding is not enabled,
		   at a minimum, this must be equal to 1 max packet size of
		   control endpoint 0 + 2 Words (for the status of the control OUT
		   data packet) + 10 Words (for setup packets).
		   - Program the OTG_FS_TX0FSIZ register (depending on the FIFO number
		   chosen) to be able to transmit control IN data. At a minimum, this
		   must be equal to 1 max packet size of control endpoint 0. */

		__ep_pktbuf_alloc(drv, 0, mxpktsz);

		/*  4. Program the following fields in the endpoint-specific registers
			for control OUT endpoint 0 to receive a SETUP packet
			- STUPCNT = 3 in OTG_FS_DOEPTSIZ0 (to receive up to 3 back-to-back
			SETUP packets)
			At this point, all initialization required to receive SETUP packets
			is done. */

		/* cache this value to be reused */
		ep->doeptsiz = OTG_FS_STUPCNT_SET(3) | 
			OTG_FS_PKTCNT_SET(1) | OTG_FS_XFRSIZ_SET(40);

		/* Prepare to receive */
		otg_fs->outep[0].doeptsiz = ep->doeptsiz;
		/* EP enable */
		otg_fs->outep[0].doepctl |= OTG_FS_EPENA | OTG_FS_CNAK;

		/* Unmask EP0 interrupts */
		otg_fs->daintmsk = OTG_FS_IEPM0 | OTG_FS_OEPM0;

		DCC_LOG2(LOG_MSG, "TX[0]: addr=%04x size=%d",
				 OTG_FS_TX0FSA_GET(otg_fs->dieptxf0) * 4,
				 OTG_FS_TX0FD_GET(otg_fs->dieptxf0) * 4);
	} else {
		uint32_t depctl;

		if (info->addr & USB_ENDPOINT_IN) {
			DCC_LOG(LOG_MSG, "IN ENDPOINT");

			if ((info->attr & 0x03) == ENDPOINT_TYPE_BULK) {
				ep->xfr_max = 6 * mxpktsz;
			} else {
				ep->xfr_max = mxpktsz;
			}

			__ep_pktbuf_alloc(drv, ep_id, ep->xfr_max);
			depctl = otg_fs->inep[ep_id].diepctl;
		} else {
			DCC_LOG(LOG_MSG, "OUT ENDPOINT");
			depctl = otg_fs->outep[ep_id].doepctl;
		}

		depctl &= ~(OTG_FS_MPSIZ_MSK | OTG_FS_EPTYP_MSK | OTG_FS_TXFNUM_MSK);

		depctl |= OTG_FS_MPSIZ_SET(mxpktsz);
		depctl |= OTG_FS_SD0PID | OTG_FS_USBAEP;

		switch (info->attr & 0x03) {
		case ENDPOINT_TYPE_CONTROL:
			DCC_LOG(LOG_MSG, "ENDPOINT_TYPE_CONTROL");
			depctl |= OTG_FS_EPTYP_SET(OTG_FS_EPTYP_CTRL);
			break;

		case ENDPOINT_TYPE_ISOCHRONOUS:
			DCC_LOG(LOG_MSG, "ENDPOINT_TYPE_ISOCHRONOUS");
			depctl |= OTG_FS_EPTYP_SET(OTG_FS_EPTYP_ISOC);
			break;

		case ENDPOINT_TYPE_BULK:
			DCC_LOG(LOG_MSG, "ENDPOINT_TYPE_BULK");
			depctl |= OTG_FS_EPTYP_SET(OTG_FS_EPTYP_BULK);
			break;

		case ENDPOINT_TYPE_INTERRUPT:
			DCC_LOG(LOG_MSG, "ENDPOINT_TYPE_INTERRUPT");
			depctl |= OTG_FS_EPTYP_SET(OTG_FS_EPTYP_INT);
			break;
		}

		if (info->addr & USB_ENDPOINT_IN) {
			/* Activate IN endpoint */
			otg_fs->inep[ep_id].diepctl = depctl | OTG_FS_TXFNUM_SET(ep_id);

			/* Enable endpoint interrupt */
			otg_fs->daintmsk |= OTG_FS_IEPM(ep_id);
		} else {
			uint32_t rxfsiz;
			uint32_t pktcnt;

			/* get the size of the RX fifio */
			rxfsiz = otg_fs->grxfsiz * 4;

			pktcnt = rxfsiz / mxpktsz;

			/* cache this value to be reused */
			ep->doeptsiz = OTG_FS_PKTCNT_SET(pktcnt) |
				OTG_FS_XFRSIZ_SET(pktcnt * mxpktsz);

			/* Prepare EP_OUT to receive */
			otg_fs->outep[ep_id].doeptsiz = ep->doeptsiz;

			/* EP enable */
			/* FIXME: the single input fifo creates a problem as 
			   packets pending on the fifo for one endpoint blocks packets for 
			   other  endpoints. */
			/* XXX: set the nak on endpoint ???? */
			otg_fs->outep[ep_id].doepctl = depctl | OTG_FS_EPENA | OTG_FS_SNAK;

			/* Enable endpoint interrupt */
			otg_fs->daintmsk |= OTG_FS_OEPM(ep_id);
		}

	}

	/* 2. Once the endpoint is activated, the core starts decoding the
	   tokens addressed to that endpoint and sends out a valid
	   handshake for each valid token received for the
	   endpoint. */

	return ep_id;
}

#define OTG_FS_DP   STM32_GPIOA, 12
#define OTG_FS_DM   STM32_GPIOA, 11
#define OTG_FS_ID   STM32_GPIOA, 10

#ifdef STM32_OTG_FS_VBUS 
#define OTG_FS_VBUS STM32_OTG_FS_VBUS
#else
#define OTG_FS_VBUS STM32_GPIOA, 9
#endif

static inline struct stm32_gpio * vbus_gpio(struct stm32_gpio *__gpio, 
											int __pin) {
	return __gpio;
}

static void otg_io_init(void)
{
	DCC_LOG(LOG_MSG, "Configuring GPIO pins...");

	if (vbus_gpio(OTG_FS_VBUS) == STM32_GPIOB)
		stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOB);

	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOA);

	stm32_gpio_af(OTG_FS_DP, GPIO_AF10);
	stm32_gpio_af(OTG_FS_DM, GPIO_AF10);
	stm32_gpio_af(OTG_FS_VBUS, GPIO_AF10);
#if 0
	stm32_gpio_af(OTG_FS_ID, GPIO_AF10);
#endif
	stm32_gpio_mode(OTG_FS_DP, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(OTG_FS_DM, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
#ifdef OTG_VBUS_ENABLE
	stm32_gpio_mode(OTG_FS_VBUS, ALT_FUNC, SPEED_LOW);
#else
	stm32_gpio_mode(OTG_FS_VBUS, INPUT, 0);
#endif
#if 0
	stm32_gpio_mode(OTG_FS_ID, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
#endif
}

static void otg_vbus_connect(bool connect)
{
#ifdef OTG_VBUS_ENABLE
	if (connect)
		stm32_gpio_mode(OTG_FS_VBUS, ALT_FUNC, SPEED_LOW);
	else
		stm32_gpio_mode(OTG_FS_VBUS, INPUT, 0);
#endif
}

static void otg_connect(struct stm32f_otg_fs * otg_fs)
{
	/* Connect device */
	otg_fs->dctl &= ~OTG_FS_SDIS;
	udelay(3000);

}

static void otg_disconnect(struct stm32f_otg_fs * otg_fs)
{
	otg_fs->dctl |= OTG_FS_SDIS;
	udelay(3000);
}

void otg_power_on(struct stm32f_otg_fs * otg_fs)
{
	DCC_LOG(LOG_MSG, "Enabling USB FS clock...");
	stm32_clk_enable(STM32_RCC, STM32_CLK_OTGFS);

	otg_connect(otg_fs);
	DCC_LOG(LOG_INFO, "[ATTACHED]");
 
	otg_vbus_connect(true);

	/* Enable Cortex interrupt */
#if 0
	cm3_irq_enable(STM32F_IRQ_OTG_HS);
#else
	thinkos_irq_enable(STM32F_IRQ_OTG_HS);
#endif
}

void otg_power_off(struct stm32f_otg_fs * otg_fs)
{
	otg_disconnect(otg_fs);

	otg_vbus_connect(false);

	DCC_LOG(LOG_INFO, "Disabling USB device clock...");
	stm32_clk_disable(STM32_RCC, STM32_CLK_OTGHS);
}

int stm32f_otg_fs_dev_init(struct stm32f_otg_drv * drv, usb_class_t * cl,
		const usb_class_events_t * ev)
{
	struct stm32f_otg_fs * otg_fs = drv->otg_fs;

	drv->cl = cl;
	drv->ev = ev;

	/* Initialize IO pins */
	otg_io_init();

	DCC_LOG(LOG_INFO, "Enabling USB FS clock...");
	stm32_clk_enable(STM32_RCC, STM32_CLK_OTGFS);
	
	/* Initialize as a device */
	stm32f_otg_fs_device_init(otg_fs);

	/* Reset global interrupts mask */
	otg_fs->gintmsk = OTG_FS_WUIM |
		OTG_FS_SRQIM |
		OTG_FS_DISCINT |
		OTG_FS_CIDSCHGM |
		OTG_FS_IISOOXFRM |
		OTG_FS_IISOIXFRM |
		OTG_FS_OEPINTM |
		OTG_FS_IEPINTM |
		OTG_FS_EPMISM |
		OTG_FS_EOPFM |
		OTG_FS_ISOODRPM |
		OTG_FS_ENUMDNEM |
		OTG_FS_USBRSTM |
		OTG_FS_USBSUSPM |
		OTG_FS_ESUSPM |
		OTG_FS_GONAKEFFM |
		OTG_FS_GINAKEFFM |
//		OTG_HS_NPTXFEM |
		OTG_FS_RXFLVLM |
		OTG_FS_SOFM |
		OTG_FS_MMISM;

	/* Enable Cortex interrupt */
#if 0
	cm3_irq_enable(STM32F_IRQ_OTG_FS);
#else
	thinkos_irq_enable(STM32F_IRQ_OTG_FS);
#endif

	DCC_LOG(LOG_TRACE, "done ----------------------------------------");

	return 0;
}

/* 
 *  ---------------------------------------------------------------------------
 * Interrupt handling
 * ---------------------------------------------------------------------------
 */

static void stm32f_otg_dev_ep_out(struct stm32f_otg_drv * drv, 
								  int ep_id, int len)
{
	struct stm32f_otg_ep * ep = &drv->ep[ep_id];
	struct stm32f_otg_fs * otg_fs = drv->otg_fs;

	DCC_LOG1(LOG_MSG, "ep_id=%d", ep_id);

	ep->xfr_rem = len;

	/* FIXME: the single input fifo creates a problem as 
	   packets pending on the fifo for one endpoint blocks packets for 
	   other  endpoints. Ex: an outstanding OUT packet may block a control
	   packet. Either the upper layer garantees the removal or
	   buffering at this layer should be implemented.
	 */
	/* XXX: set the nak on endpoint ???? */
	otg_fs->outep[ep_id].doepctl |= OTG_FS_SNAK;

	/* call class endpoint callback */
	ep->on_out(drv->cl, ep_id, len);
}


static void stm32f_otg_dev_ep_in(struct stm32f_otg_drv * drv, int ep_id)
{
	struct stm32f_otg_ep * ep = &drv->ep[ep_id];
	struct stm32f_otg_fs * otg_fs = drv->otg_fs;

	if (ep->state == EP_IN_DATA_ZLP) {
		DCC_LOG1(LOG_INFO, "ep_id=%d ZLP!", ep_id);
		otg_fs->inep[ep_id].dieptsiz = OTG_FS_PKTCNT_SET(1) | 
			OTG_FS_XFRSIZ_SET(0);
		otg_fs->inep[ep_id].diepctl |= OTG_FS_EPENA | OTG_FS_CNAK;
		/* umask FIFO empty interrupt */
		otg_fs->diepempmsk |= (1 << ep_id);
		ep->state = EP_IDLE;
		return;
	}

	DCC_LOG1(LOG_INFO, "ep_id=%d EOT", ep_id);
	/* call class endpoint callback */
	ep->on_in(drv->cl, ep_id);
}

static void stm32f_otg_ep0_xfer_setup(struct stm32f_otg_fs * otg_fs, int len)
{
	uint32_t depctl;
	uint32_t mpsiz;
	uint32_t xfrsiz;
	uint32_t pktcnt;

	depctl = otg_fs->inep[0].diepctl;
	mpsiz = OTGFS_EP0_MPSIZ_GET(depctl);

	if (len > 0) {
		/* XXX: check whether to get rid of this division or not,
		 if the CM3 div is used it is not necessary.... */
		pktcnt = (len + (mpsiz - 1)) / mpsiz;
		if (pktcnt > 6) {
			pktcnt = 6;
			xfrsiz = 6 * mpsiz;
		} else {
			xfrsiz = len;
		}
	} else {
		/* zero lenght packet */
		pktcnt = 1;
		xfrsiz = 0;
	}

	DCC_LOG2(LOG_INFO, "ep_id=0 pktcnt=%d xfrsiz=%d", pktcnt, xfrsiz);

	otg_fs->inep[0].dieptsiz = OTG_FS_PKTCNT_SET(pktcnt) | 
		OTG_FS_XFRSIZ_SET(xfrsiz); 
	/* enable end point, clear NACK */
	otg_fs->inep[0].diepctl = depctl | OTG_FS_EPENA | OTG_FS_CNAK; 
	/* umask FIFO empty interrupt */
	otg_fs->diepempmsk |= (1 << 0);
}

static void stm32f_otg_dev_ep0_in(struct stm32f_otg_drv * drv)
{
	struct stm32f_otg_fs * otg_fs = drv->otg_fs;
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
		otg_fs->diepempmsk &= ~(1 << 0);
		/* Prepare to receive */
		otg_fs->outep[0].doeptsiz = ep->doeptsiz;
		/* EP enable */
		otg_fs->outep[0].doepctl |= OTG_FS_EPENA | OTG_FS_CNAK;
		ep->state = EP_IN_DATA;
		DCC_LOG(LOG_INFO, "EP0 [IN_DATA]");
		return;
	} 
	
	stm32f_otg_ep0_xfer_setup(otg_fs, ep->xfr_rem);
}

static void stm32f_otg_dev_ep0_out(struct stm32f_otg_drv * drv)
{
	struct stm32f_otg_fs * otg_fs = drv->otg_fs;
	struct stm32f_otg_ep * ep = &drv->ep[0];

	/* last and only transfer */
	if (ep->state == EP_OUT_DATA_LAST) {
		ep->state = EP_WAIT_STATUS_IN;
		DCC_LOG(LOG_MSG, "EP0 [WAIT_STATUS_IN]");
		__ep_zlp_send(otg_fs, 0);
		return;
	}
}

static void stm32f_otg_dev_ep0_setup(struct stm32f_otg_drv * drv)
{
	struct stm32f_otg_fs * otg_fs = drv->otg_fs;
	struct usb_request * req = &drv->req;
	struct stm32f_otg_ep * ep = &drv->ep[0];
	int len;

	DCC_LOG3(LOG_MSG, "type=0x%02x request=0x%02x len=%d ",
			req->type, req->request, req->length);

	/* No-Data control SETUP transaction */
	if (req->length == 0) {
		void * dummy = NULL;

		if (((req->request << 8) | req->type) == STD_SET_ADDRESS) {
			DCC_LOG1(LOG_MSG, "address=%d",  req->value);
			stm32f_otg_fs_addr_set(otg_fs, req->value);
		}
		ep->on_setup(drv->cl, req, dummy);
		ep->state = EP_IDLE;
		__ep_zlp_send(otg_fs, 0);
		return;
	}

	if (req->type & 0x80) {
		/* Control Read SETUP transaction (IN Data Phase) */
		DCC_LOG(LOG_MSG, "EP0 [SETUP] IN Dev->Host");
		ep->xfr_ptr = NULL;
		len = ep->on_setup(drv->cl, req, (void *)&ep->xfr_ptr);
		ep->xfr_rem = MIN(req->length, len);
		DCC_LOG1(LOG_MSG, "EP0 data lenght = %d", ep->xfr_rem);
		/* prepare fifo to transmit */
		stm32f_otg_ep0_xfer_setup(otg_fs, ep->xfr_rem);
	} else {
		/* Control Write SETUP transaction (OUT Data Phase) */
		ep->xfr_ptr = ep->xfr_buf;
		ep->xfr_rem = req->length;

		DCC_LOG1(LOG_MSG, "xfr_ptr=0x%08x", ep->xfr_ptr);

		if (ep->xfr_rem > ep->xfr_buf_len) {
			ep->xfr_rem = ep->xfr_buf_len;
			DCC_LOG(LOG_ERROR, "transfer to large to fit in the buffer!");
		}

		if (ep->xfr_rem < ep->xfr_max) {
			/* last and only transfer */
			ep->state = EP_OUT_DATA_LAST;
			DCC_LOG(LOG_MSG, "EP0 [OUT_DATA_LAST] OUT Host->Dev!!!!");
		} else {
			ep->state = EP_OUT_DATA;
			DCC_LOG(LOG_MSG, "EP0 [OUT_DATA] OUT Host->Dev!!!!");
		}

		DCC_LOG(LOG_MSG, "Prepare to receive");

		/* Prepare to receive */
		otg_fs->outep[0].doeptsiz = OTG_FS_STUPCNT_SET(0) |
			OTG_FS_PKTCNT_SET(1) | OTG_FS_XFRSIZ_SET(ep->xfr_max);

		/* EP enable */
		otg_fs->outep[0].doepctl |= OTG_FS_EPENA | OTG_FS_CNAK;
	}

}

static void stm32f_otg_dev_reset(struct stm32f_otg_drv * drv)
{
	struct stm32f_otg_fs * otg_fs = drv->otg_fs;
	uint32_t siz;
	int i;

	DCC_LOG(LOG_INFO, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");

	/* Clear the Remote Wake-up Signaling */
	otg_fs->dctl &= ~OTG_FS_RWUSIG;

	/* Clear all pending interrupts */
	otg_fs->diepmsk = 0;
	otg_fs->doepmsk = 0;
	otg_fs->daint = 0xffffffff;
	otg_fs->daintmsk = 0;
	otg_fs->diepempmsk = 0;
	for (i = 0; i < OTG_EP_MAX; i++) {
		otg_fs->inep[i].diepint = 0xff;
		otg_fs->outep[i].doepint = 0xff;
		otg_fs->inep[i].dieptsiz = 0;

		drv->ep[i].xfr_rem = 0;
	}

	/* Flush the Tx FIFO */
	stm32f_otg_fs_txfifo_flush(otg_fs, TXFIFO_ALL);
	/* Flush the Rx FIFO */
	stm32f_otg_fs_rxfifo_flush(otg_fs);

	/* Reset global interrupts mask */
	otg_fs->gintmsk =  OTG_FS_SRQIM | OTG_FS_OTGINT | OTG_FS_WUIM |
			OTG_FS_USBRSTM | OTG_FS_ENUMDNEM |
			OTG_FS_ESUSPM | OTG_FS_USBSUSPM;

	/* Reset Device Address */
	otg_fs->dcfg &= ~OTG_FS_DAD_MSK;

	/*  Set global IN NAK */
	otg_fs->dctl |= OTG_FS_SGINAK;

	/* Endpoint initialization on USB reset */

	/* 1. Set the NAK bit for all OUT endpoints
	   – SNAK = 1 in OTG_FS_DOEPCTLx (for all OUT endpoints) */
	for (i = 0; i < OTG_EP_MAX; i++) {
		otg_fs->outep[i].doepctl = OTG_FS_SNAK;
	}

	/* 2. Unmask the following interrupt bits
	   - INEP0 = 1 in OTG_FS_DAINTMSK (control 0 IN endpoint)
	   - OUTEP0 = 1 in OTG_FS_DAINTMSK (control 0 OUT endpoint)
	   - STUP = 1 in DOEPMSK
	   - XFRC = 1 in DOEPMSK
	   - XFRC = 1 in DIEPMSK
	   - TOC = 1 in DIEPMSK */
	otg_fs->doepmsk = OTG_FS_STUPM | OTG_FS_XFRCM | OTG_FS_EPDM;
	otg_fs->diepmsk = OTG_FS_TOM | OTG_FS_EPDM | OTG_FS_XFRCM;

	/* 3. Set up the Data FIFO RAM for each of the FIFOs
	   - Program the OTG_FS_GRXFSIZ register, to be able to receive
	   control OUT data and setup data. If thresholding is not enabled,
	   at a minimum, this must be equal to 1 max packet size of
	   control endpoint 0 + 2 Words (for the status of the control OUT
	   data packet) + 10 Words (for setup packets).
	   - Program the OTG_FS_TX0FSIZ register (depending on the FIFO number
	   chosen) to be able to transmit control IN data. At a minimum, this
	   must be equal to 1 max packet size of control endpoint 0. */

	/* reset fifo memory (packet buffer) allocation pointer */
	drv->fifo_addr = 0;
	/* initialize RX fifo size */
	siz = OTG_RX_FIFO_SIZE;
	otg_fs->grxfsiz = siz / 4;
	/* update fifo memory allocation pointer */
	drv->fifo_addr += siz;

	DCC_LOG2(LOG_MSG, "   RX: addr=%04x size=%d",
			 0, otg_fs->grxfsiz * 4);

	drv->ev->on_reset(drv->cl);
}

/* Private USB device driver data */
struct stm32f_otg_drv stm32f_otg_fs_drv0 = {
	.otg_fs = STM32F_OTG_FS
};

void stm32f_otg_fs_isr(void)
{
	struct stm32f_otg_drv * drv = &stm32f_otg_fs_drv0;
	struct stm32f_otg_fs * otg_fs = drv->otg_fs;
	uint32_t gintsts;
	uint32_t ep_intr;

	gintsts = otg_fs->gintsts & otg_fs->gintmsk;

	DCC_LOG1(LOG_JABBER, "GINTSTS=0x%08x", gintsts);

	if (gintsts & OTG_FS_IEPINT) {
		uint32_t diepmsk;
		uint32_t diepint;
		uint32_t diepempmsk;
		uint32_t msk;

		DCC_LOG(LOG_JABBER, "<IEPINT>");

		ep_intr = (otg_fs->daint & otg_fs->daintmsk);
		diepmsk = otg_fs->diepmsk;
		diepempmsk = otg_fs->diepempmsk;

		if (ep_intr & OTG_FS_IEPINT0) {
			/* add the Transmit FIFO empty bit to the mask */
			msk = diepmsk | ((diepempmsk >> 0) & 0x1) << 7;
			diepint = otg_fs->inep[0].diepint & msk;
			/* clear interrupts */
			otg_fs->inep[0].diepint = diepint;
			if (diepint & OTG_FS_XFRC) {
				DCC_LOG(LOG_MSG, "[0] <IEPINT> <XFRC>");
				stm32f_otg_dev_ep0_in(drv);
			} else if (diepint & OTG_FS_TXFE) {
				DCC_LOG(LOG_MSG, "[0] <IEPINT> <TXFE>");
				__ep_tx_push(drv, 0);
			}
		}

		if (ep_intr & OTG_FS_IEPINT1) {
			/* add the Transmit FIFO empty bit to the mask */
			msk = diepmsk | ((diepempmsk >> 1) & 0x1) << 7;
			diepint = otg_fs->inep[1].diepint & msk;
			/* clear interrupts */
			otg_fs->inep[1].diepint = diepint;
			if (diepint & OTG_FS_TXFE) {
				DCC_LOG(LOG_MSG, "[1] <IEPINT> <TXFE>");
				__ep_tx_push(drv, 1);
			}
			if (diepint & OTG_FS_XFRC) {
				DCC_LOG(LOG_MSG, "[1] <IEPINT> <XFRC>");
				stm32f_otg_dev_ep_in(drv, 1);
			}
		}

		if (ep_intr & OTG_FS_IEPINT2) {
			/* add the Transmit FIFO empty bit to the mask */
			msk = diepmsk | ((diepempmsk >> 2) & 0x1) << 7;
			diepint = otg_fs->inep[2].diepint & msk;
			otg_fs->inep[2].diepint = diepint;
			if (diepint & OTG_FS_TXFE) {
				DCC_LOG(LOG_MSG, "[2] <IEPINT> <TXFE>");
				__ep_tx_push(drv, 2);
			}
			if (diepint & OTG_FS_XFRC) {
				DCC_LOG(LOG_MSG, "[2] <IEPINT> <XFRC>");
				stm32f_otg_dev_ep_in(drv, 2);
			}
		}

#if (OTG_EP_MAX > 3)
		if (ep_intr & OTG_FS_IEPINT3) {
			/* add the Transmit FIFO empty bit to the mask */
			msk = diepmsk | ((diepempmsk >> 3) & 0x1) << 7;
			diepint = otg_fs->inep[3].diepint & msk;
			otg_fs->inep[3].diepint = diepint;
			if (diepint & OTG_FS_TXFE) {
				DCC_LOG(LOG_MSG, "[3] <IEPINT> <TXFE>");
				__ep_tx_push(drv, 3);
			}
			if (diepint & OTG_FS_XFRC) {
				DCC_LOG(LOG_MSG, "[3] <IEPINT> <XFRC>");
				stm32f_otg_dev_ep_in(drv, 3);
			}
		}
#endif
	}

	if (gintsts & OTG_FS_OEPINT) {
		ep_intr = (otg_fs->daint & otg_fs->daintmsk);

		DCC_LOG(LOG_JABBER, "<OEPINT>");

		if (ep_intr & OTG_FS_OEPINT0) {
			uint32_t doepint;

			doepint = otg_fs->outep[0].doepint & otg_fs->doepmsk;

			if (doepint & OTG_FS_XFRC) {
				DCC_LOG(LOG_MSG, "[0] <OEPINT> <OUT XFRC>");
				stm32f_otg_dev_ep0_out(drv);
			}
			if (doepint & OTG_FS_EPDISD) {
				DCC_LOG(LOG_MSG, "[0] <OEPINT> <EPDISD>");
			}
			if (doepint & OTG_FS_STUP) {

				DCC_LOG(LOG_MSG, "[0] <OEPINT> <STUP>");
#if 0
				uint32_t doeptsiz;
				int pktcnt;
				int stupcnt;
				int xfrsiz;


				doeptsiz = otg_fs->outep[0].doeptsiz;
				stupcnt = OTG_FS_STUPCNT_GET(doeptsiz);
				(void)stupcnt;
				pktcnt = OTG_FS_PKTCNT_GET(doeptsiz);
				(void)pktcnt;
				xfrsiz = OTG_FS_XFRSIZ_GET(doeptsiz);
				(void)xfrsiz;

				DCC_LOG3(LOG_MSG, "stupcnt=%d pktcnt=%d xfrsiz=%d",
						stupcnt, pktcnt, xfrsiz);
#endif
				stm32f_otg_dev_ep0_setup(drv);
			}
			/* clear interrupts */
			otg_fs->outep[0].doepint = doepint;
		}

		if (ep_intr & OTG_FS_OEPINT1) {
			uint32_t doepint;

			doepint = otg_fs->outep[1].doepint & otg_fs->doepmsk;

			if (doepint & OTG_FS_XFRC) {
				DCC_LOG(LOG_MSG, "[1] <OEPINT> <OUT XFRC>");
			}
			if (doepint & OTG_FS_EPDISD) {
				DCC_LOG(LOG_MSG, "[1] <OEPINT> <EPDISD>");
			}
			if (doepint & OTG_FS_STUP) {
				DCC_LOG(LOG_MSG, "[1] <OEPINT> <STUP>");
			}
			/* clear interrupts */
			otg_fs->outep[1].doepint = doepint;
		}

		if (ep_intr & OTG_FS_OEPINT2) {
			uint32_t doepint;

			doepint = otg_fs->outep[2].doepint & otg_fs->doepmsk;

			if (doepint & OTG_FS_XFRC) {
				DCC_LOG(LOG_MSG, "[2] <OEPINT> <OUT XFRC>");
			}
			if (doepint & OTG_FS_EPDISD) {
				DCC_LOG(LOG_MSG, "[2] <OEPINT> <EPDISD>");
			}
			if (doepint & OTG_FS_STUP) {
				DCC_LOG(LOG_MSG, "[2] <OEPINT> <STUP>");
			}
			/* clear interrupts */
			otg_fs->outep[2].doepint = doepint;
		}

#if (OTG_EP_MAX > 3)
		if (ep_intr & OTG_FS_OEPINT3) {
			uint32_t doepint;

			doepint = otg_fs->outep[3].doepint & otg_fs->doepmsk;

			if (doepint & OTG_FS_XFRC) {
				DCC_LOG(LOG_MSG, "[3] <OEPINT> <OUT XFRC>");
			}
			if (doepint & OTG_FS_EPDISD) {
				DCC_LOG(LOG_MSG, "[3] <OEPINT> <EPDISD>");
			}
			if (doepint & OTG_FS_STUP) {
				DCC_LOG(LOG_MSG, "[3] <OEPINT> <STUP>");
			}

			/* clear interrupts */
			otg_fs->outep[3].doepint = doepint;
		}
#endif
	}

	/* RxFIFO non-empty */
	if (gintsts & OTG_FS_RXFLVL) {
		uint32_t grxsts;
		int epnum;
		int len;
		int stat;

		/* 1. On catching an RXFLVL interrupt (OTG_FS_GINTSTS register),
		   the application must read the Receive status pop
		   register (OTG_FS_GRXSTSP). */
		grxsts = otg_fs->grxstsp;

		epnum = OTG_FS_EPNUM_GET(grxsts);
		len = OTG_FS_BCNT_GET(grxsts);
		(void)len;
		stat = OTG_FS_PKTSTS_GET(grxsts);
		(void)stat;

		DCC_LOG3(LOG_MSG, "[%d] <RXFLVL> len=%d status=%d", 
				 epnum, len, stat);

		if (epnum == 0) {

			/* 3. If the received packet’s byte count is not 0, the byte count
			   amount of data is popped from the receive Data FIFO and stored in
			   memory. If the received packet byte count is 0, no data is popped
			   from the receive data FIFO. */
			switch (grxsts & OTG_FS_PKTSTS_MSK) {
			case OTG_FS_PKTSTS_GOUT_NACK:
				/* Global OUT NAK (triggers an interrupt) */
				DCC_LOG1(LOG_MSG, "[%d] <RXFLVL> <GOUT_NACK>", epnum);
				break;
			case OTG_FS_PKTSTS_OUT_DATA_UPDT: {
				/* OUT data packet received */
				DCC_LOG1(LOG_MSG, "[%d] <RXFLVL> <OUT_DATA_UPDT>", epnum);
				__ep_rx_pop(drv, 0, len);
				break;
			}
			case OTG_FS_PKTSTS_OUT_XFER_COMP:
				DCC_LOG1(LOG_MSG, "[%d] <RXFLVL> <OUT_XFER_COMP>", epnum);
				break;
			case OTG_FS_PKTSTS_SETUP_COMP:
				/* SETUP transaction completed (triggers an interrupt) */
				DCC_LOG1(LOG_MSG, "[%d] <RXFLVL> <SETUP_COMP>", epnum);
				break;
			case OTG_FS_PKTSTS_SETUP_UPDT:
				/* SETUP data packet received */
				DCC_LOG1(LOG_MSG, "[%d] <RXFLVL> <SETUP_UPDT>", epnum);

				if (len != 8) {
					DCC_LOG(LOG_ERROR, "setup data len != 8!");
				}

				/* Copy the received setup packet into the setup buffer */
				__copy_from_pktbuf(&drv->req, &otg_fs->dfifo[0].pop, len);

				DCC_LOG3(LOG_MSG, "type=0x%02x request=0x%02x len=%d ",
						drv->req.type, drv->req.request, drv->req.length);

				break;
			}
		} else {
			switch (grxsts & OTG_FS_PKTSTS_MSK) {
			case OTG_FS_PKTSTS_OUT_DATA_UPDT:
				/* OUT data packet received */
				DCC_LOG1(LOG_MSG, "[%d] <RXFLVL> <OUT_DATA_UPDT>", epnum);
				/* 2. The application can mask the RXFLVL interrupt (in
				   OTG_FS_GINTSTS) by writing to RXFLVL = 0 (in
				   OTG_FS_GINTMSK), until it has read the packet from
				   the receive FIFO. */
				otg_fs->gintmsk &= ~OTG_FS_RXFLVLM;
				/* Enable SOF interrupts */
//				otg_fs->gintmsk |=  OTG_FS_SOFM;
//				__ep_rx_pop(drv, 0, len);
				stm32f_otg_dev_ep_out(drv, epnum, len);
				break;
			case OTG_FS_PKTSTS_OUT_XFER_COMP:
				DCC_LOG1(LOG_MSG, "[%d] <RXFLVL> <OUT_XFER_COMP>", epnum);
				DCC_LOG2(LOG_MSG, "[%d] doeptsiz=%08x", epnum, 
						 otg_fs->outep[epnum].doeptsiz);	 
				/* Prepare to receive more */
				otg_fs->outep[epnum].doeptsiz = drv->ep[epnum].doeptsiz;
				/* EP enable */
				/* FIXME: not clearing the NAK here reduces the performance,
				   but we have to garantee tha the class driver
				   periodically remove the packets from 
				   the fifo. Otherwise the EP0 will not receive its
				   packets and we end up with a deadlock situation */
//				otg_fs->outep[epnum].doepctl |= OTG_FS_EPENA | OTG_FS_CNAK;
				/* Disable SOF interrupts */
//				otg_fs->gintmsk &= ~OTG_FS_SOFM;
				break;
			}
		}

		/* 5. After the data payload is popped from the receive FIFO, the
		   RXFLVL interrupt (OTG_FS_GINTSTS) must be unmasked. */
		//	otg_fs->gintmsk |= OTG_FS_RXFLVLM;
	}

	if (gintsts & OTG_FS_SRQINT) {
		/* Session request/new session detected interrupt */
		DCC_LOG(LOG_MSG, "<SRQINT>  [POWERED]");
		otg_fs->gintmsk |= OTG_FS_WUIM | OTG_FS_USBRSTM | OTG_FS_ENUMDNEM |
			OTG_FS_ESUSPM | OTG_FS_USBSUSPM;
	}

	if (gintsts & OTG_FS_OTGINT) {
		uint32_t gotgint = otg_fs->gotgint;
		DCC_LOG(LOG_MSG, "<OTGINT>");
		if (gotgint & OTG_FS_OTGINT) {
			DCC_LOG(LOG_MSG, "<SEDET>  [ATTACHED]");
			otg_fs->gintmsk = OTG_FS_SRQIM | OTG_FS_OTGINT;
		}
		otg_fs->gotgint = gotgint;
	}

	if (gintsts & OTG_FS_ENUMDNE) {
		/* Unmask global interrupts */
		otg_fs->gintmsk |=  OTG_FS_IEPINTM | OTG_FS_OEPINTM |
			OTG_FS_IISOIXFRM | OTG_FS_IISOOXFRM | OTG_FS_RXFLVLM;
		/*  Clear global IN NAK */
		otg_fs->dctl |= OTG_FS_CGINAK;

	   /* Wait for the ENUMDNE interrupt in OTG_HS_GINTSTS.
	   This interrupt indicates the end of reset on the USB. On receiving this 
	   interrupt, the application must read the OTG_HS_DSTS register to 
	   determine the enumeration speed and perform the steps listed in 
	   Endpoint initialization on enumeration completion on page 1035.
	   At this point, the device is ready to accept SOF packets and perform 
	   control transfers on control endpoint 0. */
	}


	if (gintsts & OTG_FS_USBRST ) {
		/* end of bus reset */
		DCC_LOG(LOG_MSG, "<USBRST> --------------- [DEFAULT]");
		stm32f_otg_dev_reset(drv);
	}

	if (gintsts & OTG_FS_ESUSP) {
		uint32_t dsts = otg_fs->dsts;
		(void)dsts;

		DCC_LOG4(LOG_INFO, "<ESUSP> %s%s ENUMSPD=%d %s", 
				 (dsts & OTG_FS_EERR) ? " EERR" : "",
				 (dsts & OTG_FS_SUSPSTS) ? " SUSPSTS" : "",
				 OTG_FS_ENUMSPD_GET(dsts),
				 otg_fs->pcgcctl & OTG_FS_PHYSUSP ? "PHYSUSP" : "");
	}

#if DEBUG
	if (gintsts & OTG_FS_GONAKEFF) {
		DCC_LOG(LOG_MSG, "<GONAKEFF>");
		otg_fs->gintmsk &= ~OTG_FS_GONAKEFFM;
	}

	if (gintsts & OTG_FS_SOF) {
		DCC_LOG(LOG_MSG, "<SOF>");
	}

	if (gintsts & OTG_FS_PTXFE) {
		DCC_LOG(LOG_MSG, "<PTXFE>");
	}

	if (gintsts & OTG_FS_WKUPINT) {
		DCC_LOG(LOG_MSG, "<WKUPINT>");
		/* disable resume/wakeup interrupts */
	}

	if (gintsts & OTG_FS_USBSUSP) {
		DCC_LOG(LOG_MSG, "<USBSUSP>");
	}

	if (gintsts & OTG_FS_IISOIXFR) {
		DCC_LOG(LOG_MSG, "<IISOIXFR>");
	}

	if (gintsts & OTG_FS_INCOMPISOOUT) {
		DCC_LOG(LOG_MSG, "<INCOMPISOOUT>");
	}

	if (gintsts & OTG_FS_MMIS) {
		DCC_LOG(LOG_MSG, "<MMIS>");
	}
#endif
	/* clear pending interrupts */
	otg_fs->gintsts = gintsts;
}

/* USB device operations */
const struct usb_dev_ops stm32f_otg_fs_ops = {
	.dev_init = (usb_dev_init_t)stm32f_otg_fs_dev_init,
	.ep_init = (usb_dev_ep_init_t)stm32f_otg_dev_ep_init,
	.ep_ctl = (usb_dev_ep_ctl_t)stm32f_otg_dev_ep_ctl,
	.ep_pkt_xmit = (usb_dev_ep_pkt_xmit_t)stm32f_otg_dev_ep_pkt_xmit,
	.ep_pkt_recv = (usb_dev_ep_pkt_recv_t)stm32f_otg_dev_ep_pkt_recv
};

/* USB device driver */
const struct usb_dev stm32f_otg_fs_dev = {
	.priv = (void *)&stm32f_otg_fs_drv0,
	.op = &stm32f_otg_fs_ops
};

#endif /* STM32_ENABLE_OTG_FS && STM32F_OTG_FS */


