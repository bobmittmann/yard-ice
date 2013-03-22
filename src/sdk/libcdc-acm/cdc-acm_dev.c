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

struct usb_cdc {
	/* modem bits */
	volatile uint8_t status; /* modem status lines */
	volatile uint8_t control; /* modem control lines */

	uint8_t lsst; /* local (set) serial state */
	uint8_t rsst; /* remote (acked) serail state */

	struct cdc_line_coding lc;
};

struct usb_cdc_class {
	/* underling USB device */
	struct usb_dev * usb;

	/* class specific block */
	struct usb_cdc cdc;

	volatile uint8_t state;

	int8_t rx_ev; /* RX event */
	int8_t tx_ev; /* TX event */

	int8_t tx_lock; /* TX lock */
	int8_t tx_lock_ev; /* TX lock/unlock event */

	int8_t ctrl_ev; /* Control event */
	uint8_t ctrl_rcv; /* control message received count */
	uint8_t ctrl_ack; /* control message acknowledge count */

	uint32_t setup_buf[2];
	uint32_t pkt_buf[4];
};

const char * ep_type_nm[] = {
	"CONTROL",
	"ISOCHRONOUS",
	"BULK",
	"INTERRUPT"
};

struct usb_desc_set {
	struct usb_descriptor_device device;
	struct usb_descriptor_configuration conf;
};

struct usb_descriptor {
	uint8_t length;              
	uint8_t type;      
} __attribute__((__packed__));

static struct usb_descriptor * 
	usb_desc_next(struct usb_descriptor * dsc) 
{
	return (struct usb_descriptor *)(((uint8_t *)dsc) + dsc->length);  
}


int usb_desc_device_parse(const struct usb_descriptor_device * dv)
{
	/* make sur the descriptor is a device descriptor */
	if (dv->type != USB_DESCRIPTOR_DEVICE)
		return -1;
	if (dv->length != sizeof(struct usb_descriptor_device))
		return -1;
	dv = (struct usb_descriptor_device *)dv;

	/* Control endpoint 0 max. packet size */
	DCC_LOG1(LOG_INFO, "EP0 IN/OUT CONTROL maxpacketsize=%d...", 
			 dv->max_pkt_sz0);

	return dv->max_pkt_sz0;
};

int usb_desc_config_parse(const struct usb_descriptor_configuration * cfg)
{
	struct usb_descriptor * dsc;
	struct usb_descriptor * end;

	if (cfg->type != USB_DESCRIPTOR_CONFIGURATION)
		return -1;
	if (cfg->length != sizeof(struct usb_descriptor_configuration))
		return -1;

	/* skip the configuration descriptor */
	dsc = usb_desc_next((struct usb_descriptor *) cfg);
	/* get a pointer to the end of the configuration */
	end = (struct usb_descriptor *)(((uint8_t *)cfg) + cfg->total_length);

	/* search for endpoints */
	while (dsc < end) {
		if (dsc->type == USB_DESCRIPTOR_ENDPOINT) {
			struct usb_descriptor_endpoint * e;
			e = (struct usb_descriptor_endpoint *)dsc;

			if (e->length != sizeof(struct usb_descriptor_endpoint))
				return -1;

			DCC_LOG4(LOG_TRACE, "EP%d %s %s maxpacketsize=%d...", 
					e->endpointaddress & 0x7f, 
					(e->endpointaddress & USB_ENDPOINT_IN) ? "IN" : "OUT", 
					ep_type_nm[e->attributes], 
					e->maxpacketsize);
		}
		dsc = usb_desc_next(dsc);
	}

	return 0;
};

int usb_cdc_on_reset(usb_class_t * cl)
{
	struct usb_cdc_class * cdc = (struct usb_cdc_class *)cl;
	int mxpktsz;

	mxpktsz = usb_desc_device_parse(&cdc_acm_desc.device);

	DCC_LOG1(LOG_TRACE, "mxpktsz=%d", mxpktsz);
		
	/* initializes EP0 */
	usb_dev_ep0_init(cdc->usb, mxpktsz);

	/* setup the buffer for receiving control data */
	usb_dev_ep_rx_setup(cdc->usb, 0, cdc->setup_buf, 8);

	return 0;
}

int usb_cdc_on_setup(usb_class_t * cl, struct usb_request * req)
{
	struct usb_cdc_class * cdc = (struct usb_cdc_class *)cl;
	int value = req->value;
	int	index = req->index;
	int len = req->length;
	int desc;

	if (req->type & 0x80) {
		DCC_LOG1(LOG_TRACE, "[0] <OEPINT> <STUP> bmRequestType=%02x "
				 "Dev->Host", req->type);
	} else {
		DCC_LOG1(LOG_TRACE, "[0] <OEPINT> <STUP> bmRequestType=%02x "
				 "Host->Dev", req->type);
		//		otg_fs_ep0_out_start(otg_fs);
	}

	DCC_LOG1(LOG_TRACE, "len = %d", len);

	/* Handle supported standard device request Cf
	   Table 9-3 in USB specification Rev 1.1 */

	switch ((req->request << 8) | req->type) {
	case STD_GET_DESCRIPTOR:
		desc = value >> 8;

		if (desc == USB_DESCRIPTOR_DEVICE) {
			/* Return Device Descriptor */
			len = MIN(sizeof(struct usb_descriptor_device), len);
			DCC_LOG1(LOG_TRACE, "GetDesc: Device: len=%d", len);
			usb_dev_ep_tx_start(cdc->usb, 0, (void *)&cdc_acm_desc, len);
			break;
		}

		if (desc == USB_DESCRIPTOR_CONFIGURATION)	{
			/* Return Configuration Descriptor */
			len = MIN(sizeof(struct usb_descriptor_set_cdc), len);
			DCC_LOG1(LOG_TRACE, "GetDesc: Config: len=%d", len);
			usb_dev_ep_tx_start(cdc->usb, 0, (void *)&cdc_acm_desc.conf, len);
			break;
		}

		DCC_LOG1(LOG_TRACE, "GetDesc: %d ?", desc);
		usb_dev_ep_stall(cdc->usb, 0);
		break;

	case STD_SET_ADDRESS:
		DCC_LOG1(LOG_TRACE, "SetAddr: %d -------- [ADDRESS]", value);
		usb_dev_addr_set(cdc->usb, value);
		/* signal any pending threads */
//		__thinkos_ev_raise(cdc->rx_ev);
		usb_dev_ep_zlp_send(cdc->usb, 0);
		break;

	case STD_SET_CONFIGURATION: {

		DCC_LOG1(LOG_TRACE, "SetCfg: %d", value);

		if (value) {
			usb_dev_ep_enable(cdc->usb, 1);
			usb_dev_ep_enable(cdc->usb, 2);
			usb_dev_ep_enable(cdc->usb, 3);
		} else {
			usb_dev_ep_disable(cdc->usb, 1);
			usb_dev_ep_disable(cdc->usb, 2);
			usb_dev_ep_disable(cdc->usb, 3);
		}

		DCC_LOG(LOG_TRACE, "[CONFIGURED]");
		/* signal any pending threads */
		__thinkos_ev_raise(cdc->rx_ev);
		usb_dev_ep_zlp_send(cdc->usb, 0);
		break;
	}

	case STD_GET_CONFIGURATION:
		DCC_LOG(LOG_TRACE, "GetCfg");
		//		data = (udp->glb_stat & UDP_CONFG) ? 1 : 0;
		//			usb_ep0_send_word(dev, 0);
		break;

	case STD_GET_STATUS_INTERFACE:
		DCC_LOG(LOG_TRACE, "GetStIf");
		//			usb_ep0_send_word(dev, 0);
		break;

	case STD_GET_STATUS_ZERO:
		DCC_LOG(LOG_TRACE, "GetStZr");
		//			usb_ep0_send_word(dev, 0);
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
		usb_dev_ep_stall(cdc->usb, 0);
		break;

	case STD_SET_FEATURE_INTERFACE:
		DCC_LOG(LOG_TRACE, "SetIf");
		usb_dev_ep_zlp_send(cdc->usb, 0);
		break;

	case STD_SET_FEATURE_ENDPOINT:
		DCC_LOG(LOG_TRACE, "SetEpt");
		index &= 0x0f;
#if 0
		if ((value == 0) && index && (index <= 3)) {
			udp->csr[index] = 0;
			goto zlp_send;
		}
#endif
		usb_dev_ep_stall(cdc->usb, 0);
		break;

	case STD_CLEAR_FEATURE_ZERO:
		DCC_LOG(LOG_TRACE, "ClrZr");
		usb_dev_ep_stall(cdc->usb, 0);
		break;

	case STD_CLEAR_FEATURE_INTERFACE:
		DCC_LOG(LOG_TRACE, "ClrIf");
		usb_dev_ep_zlp_send(cdc->usb, 0);
		break;

	case STD_CLEAR_FEATURE_ENDPOINT:
		index &= 0x0f;
		DCC_LOG1(LOG_TRACE, "ClrEpt: %d", index);
#if 0
		if ((value == 0) && index && (index <= 3)) {
			if (index == EP_OUT)
				udp->csr[EP_OUT] =
					(UDP_EPEDS | UDP_EPTYPE_BULK_OUT);

			else if (index == EP_IN)
				udp->csr[EP_IN] =
					(UDP_EPEDS | UDP_EPTYPE_BULK_IN);

#if USB_CDC_ENABLE_STATE
			else if (index == EP_INT)
				udp->csr[EP_INT] =
					(UDP_EPEDS | UDP_EPTYPE_INT_IN);
#endif

			otg_fs_ep0_zlp_send(otg_fs);
			goto zlp_send;
		}
#endif
		usb_dev_ep_stall(cdc->usb, 0);
		break;

	case SET_LINE_CODING:
		/* this will be handled after the data phase */
		DCC_LOG3(LOG_TRACE, "CDC SetLn: idx=%d val=%d len=%d",
				 index, value, len);
		break;

	case GET_LINE_CODING:
		DCC_LOG(LOG_TRACE, "CDC GetLn");
		len = MIN(sizeof(struct cdc_line_coding), len);
		usb_dev_ep_tx_start(cdc->usb, 0, (void *)&cdc->cdc.lc, len);
		break;

	case SET_CONTROL_LINE_STATE:
		DCC_LOG3(LOG_TRACE, "CDC SetCtrl: idx=%d val=%d len=%d",
				 index, value, len);
		cdc->cdc.control = value;

		DCC_LOG1(LOG_TRACE, "DTR=%d", (value & CDC_DTE_PRESENT));

#if USB_CDC_ENABLE_STATE
		/* update the local serial state */
		cdc.cdc->lsst = (value & CDC_DTE_PRESENT) ? 
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

		usb_dev_ep_zlp_send(cdc->usb, 0);
		break;

	default:
		DCC_LOG5(LOG_TRACE, "CDC t=%x r=%x v=%x i=%d l=%d",
				 req->type, req->request, value, index, len);
		usb_dev_ep_stall(cdc->usb, 0);
		break;
	}
	return 0;

}

int usb_cdc_on_setup_in(usb_class_t * cl, struct usb_request * req, void ** ptr) {
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
			*ptr = (void *)&cdc_acm_desc;
			len = sizeof(struct usb_descriptor_device);
			DCC_LOG1(LOG_TRACE, "GetDesc: Device: len=%d", len);
			break;
		}

		if (desc == USB_DESCRIPTOR_CONFIGURATION) {
			/* Return Configuration Descriptor */
			*ptr = (void *)&cdc_acm_desc.conf;
			len = sizeof(struct usb_descriptor_set_cdc);
			DCC_LOG1(LOG_TRACE, "GetDesc: Config: len=%d", len);
			break;
		}

		DCC_LOG1(LOG_TRACE, "GetDesc: %d ?", desc);
		break;

	case STD_SET_ADDRESS:
		DCC_LOG1(LOG_TRACE, "SetAddr: %d -------- [ADDRESS]", value);
		/* signal any pending threads */
//		__thinkos_ev_raise(cdc->rx_ev);
		break;

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
		usb_dev_ep_stall(cdc->usb, 0);
		break;

	case GET_LINE_CODING:
		DCC_LOG(LOG_TRACE, "CDC GetLn");
		len = MIN(sizeof(struct cdc_line_coding), len);
		usb_dev_ep_tx_start(cdc->usb, 0, (void *) &cdc->cdc.lc, len);
		break;

	default:
		DCC_LOG5(LOG_TRACE, "CDC t=%x r=%x v=%x i=%d l=%d",
				req->type, req->request, value, index, len);
		usb_dev_ep_stall(cdc->usb, 0);
		break;
	}

	return len;
}




int usb_cdc_on_ep0_rx(usb_class_t * cl, uint32_t * buf, unsigned int len)
{
	DCC_LOG1(LOG_TRACE, "len=%d", len);
	return 0;
}

int usb_cdc_on_data_rx(usb_class_t * cl, uint32_t * buf, unsigned int len)
{
	DCC_LOG1(LOG_TRACE, "len=%d", len);
	return 0;
}

struct usb_cdc_class usb_cdc_rt;

const struct usb_class_events usb_cdc_ev = {
	.on_reset = usb_cdc_on_reset,
	.on_setup = usb_cdc_on_setup,
	.on_setup_in = usb_cdc_on_setup_in
};

struct usb_cdc_class * usb_cdc_init(const usb_dev_t * usb)
{
	struct usb_cdc_class * cdc = &usb_cdc_rt;
	usb_class_t * cl =  (usb_class_t *)cdc;

	DCC_LOG(LOG_TRACE, "...");

//	usb_desc_parse(&cdc_acm_desc.device, &cdc_acm_desc.conf.cfg);

	/* initialize USB device */
	cdc->usb = (usb_dev_t *)usb;
	usb_dev_init(usb, cl, &cdc_acm_desc.device, &usb_cdc_ev);

	return cdc;
}


