/* 
 * File:	 usb-cdc.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2011 Bob Mittmann. All Rights Reserved.
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

#include <sys/stm32f.h>
#include <arch/cortex-m3.h>
#include <sys/delay.h>
#include <sys/serial.h>
#include <sys/param.h>
#include <sys/file.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <sys/usb-dev.h>

#include <sys/dcclog.h>

#include "cdc-acm.h"

#include <thinkos.h>
#define __THINKOS_IRQ__
#include <thinkos_irq.h>

struct usb_cdc_acm {
	/* modem bits */
	volatile uint8_t status; /* modem status lines */
	volatile uint8_t control; /* modem control lines */

	uint8_t lsst; /* local (set) serial state */
	uint8_t rsst; /* remote (acked) serail state */

	struct cdc_line_coding lc;
};

#define CDC_CTR_BUF_LEN 16

struct usb_cdc_class {
	/* underling USB device */
	struct usb_dev * usb;

	/* class specific block */
	struct usb_cdc_acm acm;


	int8_t rx_ev; /* RX event */
	int8_t tx_ev; /* TX event */

	int8_t tx_lock; /* TX lock */
	int8_t tx_lock_ev; /* TX lock/unlock event */

	int8_t ctrl_ev; /* Control event */
	uint8_t ctrl_rcv; /* control message received count */
	uint8_t ctrl_ack; /* control message acknowledge count */

	uint32_t ctr_buf[CDC_CTR_BUF_LEN / 4];
};

int usb_cdc_on_rcv(usb_class_t * cl, unsigned int ep_id)
{
	struct usb_cdc_class * cdc = (struct usb_cdc_class *) cl;
	uint8_t buf[128];


	usb_dev_ep_pkt_recv(cdc->usb, ep_id, buf, 128);

	DCC_LOG2(LOG_TRACE, "ep_id=%d 0x%02x", ep_id, buf[0]);

	return 0;

}

int usb_cdc_on_eot(usb_class_t * cl, unsigned int ep_id)
{
	DCC_LOG1(LOG_TRACE, "ep_id=%d", ep_id);
	return 0;

}

int usb_cdc_on_eot_int(usb_class_t * cl, unsigned int ep_id)
{
	DCC_LOG1(LOG_TRACE, "ep_id=%d", ep_id);
	return 0;
}

const usb_dev_ep_info_t usb_cdc_in_info = {
	.addr = USB_ENDPOINT_IN + EP_IN_ADDR,
	.attr = ENDPOINT_TYPE_BULK,
	.mxpktsz = EP0_MAX_PKT_SIZE,
	.on_in = usb_cdc_on_eot
};

const usb_dev_ep_info_t usb_cdc_out_info = {
	.addr = USB_ENDPOINT_OUT + EP_OUT_ADDR,
	.attr = ENDPOINT_TYPE_BULK,
	.mxpktsz = EP0_MAX_PKT_SIZE,
	.on_out = usb_cdc_on_rcv
};

const usb_dev_ep_info_t usb_cdc_int_info = {
	.addr = USB_ENDPOINT_IN + EP_INT_ADDR,
	.attr = ENDPOINT_TYPE_INTERRUPT,
	.mxpktsz = EP0_MAX_PKT_SIZE,
	.on_in = usb_cdc_on_eot_int
};

int usb_cdc_on_setup(usb_class_t * cl, struct usb_request * req, void ** ptr) {
	struct usb_cdc_class * cdc = (struct usb_cdc_class *) cl;
	int value = req->value;
	int index = req->index;
	int len = 0;
	int desc;

	/* Handle supported standard device request Cf
	 Table 9-3 in USB specification Rev 1.1 */

	switch ((req->request << 8) | req->type) {
	case STD_GET_DESCRIPTOR:
		desc = value >> 8;

		if (desc == USB_DESCRIPTOR_DEVICE) {
			/* Return Device Descriptor */
			*ptr = (void *)&cdc_acm_desc_dev;
			len = sizeof(struct usb_descriptor_device);
			DCC_LOG1(LOG_TRACE, "GetDesc: Device: len=%d", len);
			break;
		}

		if (desc == USB_DESCRIPTOR_CONFIGURATION) {
			/* Return Configuration Descriptor */
			*ptr = (void *)&cdc_acm_desc_cfg;
			len = sizeof(struct cdc_acm_descriptor_config);
			DCC_LOG1(LOG_TRACE, "GetDesc: Config: len=%d", len);
			break;
		}

		if (desc == USB_DESCRIPTOR_STRING) {
			int n = value & 0xff;
			*ptr = (void *)cdc_acm_str[n].str;
			len = cdc_acm_str[n].len;
			DCC_LOG1(LOG_TRACE, "GetDesc: String[%d]", n);
			break;
		}

		len = -1;
		DCC_LOG1(LOG_TRACE, "GetDesc: %d ?", desc);
		break;

	case STD_SET_ADDRESS:
		DCC_LOG1(LOG_TRACE, "SetAddr: %d -------- [ADDRESS]", value);
		/* signal any pending threads */
//		__thinkos_ev_raise(cdc->rx_ev);
		break;

	case STD_SET_CONFIGURATION: {
		DCC_LOG1(LOG_TRACE, "SetCfg: %d", value);

		if (value) {
			usb_dev_ep_init(cdc->usb, 1, &usb_cdc_in_info);
			usb_dev_ep_init(cdc->usb, 2, &usb_cdc_out_info);
			usb_dev_ep_init(cdc->usb, 3, &usb_cdc_int_info);
		} else {
			usb_dev_ep_disable(cdc->usb, 1);
			usb_dev_ep_disable(cdc->usb, 2);
			usb_dev_ep_disable(cdc->usb, 3);
		}

		DCC_LOG(LOG_TRACE, "[CONFIGURED]");
		/* signal any pending threads */
		__thinkos_ev_raise(cdc->rx_ev);
		break;
	}

	case STD_GET_CONFIGURATION:
		DCC_LOG(LOG_TRACE, "GetCfg");
		//              data = (udp->glb_stat & UDP_CONFG) ? 1 : 0;
		//                      usb_ep0_send_word(dev, 0);
		break;

	case STD_GET_STATUS_INTERFACE:
		DCC_LOG(LOG_TRACE, "GetStIf");
		//                      usb_ep0_send_word(dev, 0);
		break;

	case STD_GET_STATUS_ZERO:
		DCC_LOG(LOG_TRACE, "GetStZr");
		//                      usb_ep0_send_word(dev, 0);
		break;

	case STD_GET_STATUS_ENDPOINT:
		index &= 0x0f;
		DCC_LOG1(LOG_TRACE, "GetStEpt:%d", index);
#if 0
		if ((udp->glb_stat & UDP_CONFG) && (index <= 3)) {
			data = (udp->csr[index] & UDP_EPEDS) ? 0 : 1;
			usb_ep0_send_word(dev, data);
			break;
		}

		if ((udp->glb_stat & UDP_FADDEN) && (index == 0)) {
			data = (udp->csr[index] & UDP_EPEDS) ? 0 : 1;
			usb_ep0_send_word(dev, data);
			break;
		}
#endif
		break;

	case SET_LINE_CODING:
		DCC_LOG3(LOG_TRACE, "CDC SetLn: idx=%d val=%d len=%d",
				 index, value, len);

        memcpy(&cdc->acm.lc, cdc->ctr_buf, sizeof(struct cdc_line_coding));
        
        DCC_LOG1(LOG_TRACE, "dsDTERate=%d", cdc->acm.lc.dwDTERate);
        DCC_LOG1(LOG_TRACE, "bCharFormat=%d", cdc->acm.lc.bCharFormat);
        DCC_LOG1(LOG_TRACE, "bParityType=%d", cdc->acm.lc.bParityType);
        DCC_LOG1(LOG_TRACE, "bDataBits=%d", cdc->acm.lc.bDataBits);
//            otg_fs_ep0_zlp_send(otg_fs);
		break;

	case GET_LINE_CODING:
		DCC_LOG(LOG_TRACE, "CDC GetLn");
		len = MIN(sizeof(struct cdc_line_coding), len);
//		usb_dev_ep_tx_start(cdc->usb, 0, (void *)&cdc->cdc.lc, len);
		break;

	case SET_CONTROL_LINE_STATE:
		DCC_LOG3(LOG_TRACE, "CDC SetCtrl: idx=%d val=%d len=%d",
				 index, value, len);
		cdc->acm.control = value;

		DCC_LOG2(LOG_TRACE, "CDC_DTE_PRESENT=%d ACTIVATE_CARRIER=%d",
				(value & CDC_DTE_PRESENT) ? 1 : 0,
				(value & CDC_ACTIVATE_CARRIER) ? 1 : 0);

#if USB_CDC_ENABLE_STATE
		/* update the local serial state */
		acm.acm->lsst = (value & CDC_DTE_PRESENT) ?
			CDC_SERIAL_STATE_RX_CARRIER | CDC_SERIAL_STATE_TX_CARRIER : 0;
		/* trigger a local state notification */
		usb_cdc_state_notify(otg_fs);
#endif

		/* there might have threads waiting for
		   modem control line changes (DTR, RTS)
		   wake them up */
		__thinkos_ev_raise(cdc->rx_ev);
		__thinkos_ev_raise(cdc->tx_ev);
		__thinkos_ev_raise(cdc->ctrl_ev);
		break;

	default:
		DCC_LOG5(LOG_TRACE, "CDC t=%x r=%x v=%x i=%d l=%d",
				req->type, req->request, value, index, len);
		break;
	}

	return len;
}

const usb_dev_ep_info_t usb_cdc_ep0_info = {
	.addr = 0,
	.attr = ENDPOINT_TYPE_CONTROL,
	.mxpktsz = EP0_MAX_PKT_SIZE,
	.on_setup = usb_cdc_on_setup
};

int usb_cdc_on_reset(usb_class_t * cl)
{
	struct usb_cdc_class * cdc = (struct usb_cdc_class *)cl;

	/* initializes EP0 */
	usb_dev_ep0_init(cdc->usb, &usb_cdc_ep0_info, cdc->ctr_buf, CDC_CTR_BUF_LEN);

	return 0;
}

int usb_cdc_on_error(usb_class_t * cl, int code)
{
	return 0;
}

struct usb_cdc_class usb_cdc_rt;

const usb_class_events_t usb_cdc_ev = {
	.on_reset = usb_cdc_on_reset,
	.on_error = usb_cdc_on_error
};

struct usb_cdc_class * usb_cdc_init(const usb_dev_t * usb)
{
	struct usb_cdc_class * cdc = &usb_cdc_rt;
	usb_class_t * cl =  (usb_class_t *)cdc;

	/* initialize USB device */
	cdc->usb = (usb_dev_t *)usb;
	usb_dev_init(usb, cl, &usb_cdc_ev);

	return cdc;
}


