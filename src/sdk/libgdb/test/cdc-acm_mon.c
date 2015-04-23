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

#include "cdc-acm.h"

#include <sys/stm32f.h>
#include <arch/cortex-m3.h>
#include <sys/param.h>
#include <sys/serial.h>
#include <string.h>
#include <stdbool.h>

#include <sys/usb-dev.h>
#include <sys/usb-cdc.h>

#include <sys/dcclog.h>

#define __THINKOS_DMON__
#include <thinkos_dmon.h>

#ifndef STM32_ENABLE_USB_DEVICE 
#error "cdc-acm_dev.c depends on STM32_ENABLE_USB_DEVICE"
#endif 

struct usb_cdc_acm {
	/* modem bits */
	volatile uint8_t status; /* modem status lines */
	volatile uint8_t control; /* modem control lines */
	volatile uint8_t flags;
	struct cdc_line_coding lc;
};

#define ACM_LC_SET        (1 << 0)
#define ACM_USB_SUSPENDED (1 << 1)

#define USB_CDC_IRQ_PRIORITY IRQ_PRIORITY_REGULAR
#define CDC_CTR_BUF_LEN 16

struct usb_cdc_acm_dev {
	/* underling USB device */
	struct usb_dev * usb;

	/* class specific block */
	struct usb_cdc_acm acm;

	/* string table */
	const uint8_t * const * str;
	/* number of strings */
	uint8_t strcnt;
	uint8_t ctl_ep;
	uint8_t in_ep;
	uint8_t out_ep;
	uint8_t int_ep;

	volatile uint8_t rx_cnt; 
	volatile uint8_t rx_pos; 

	uint8_t rx_buf[CDC_EP_IN_MAX_PKT_SIZE + 1];

	uint32_t ctr_buf[CDC_CTR_BUF_LEN / 4];
};

static inline void __memcpy(void * __dst, void * __src,  unsigned int __len)
{
	uint8_t * dst = (uint8_t *)__dst;
	uint8_t * src = (uint8_t *)__src;
	int i;

	for (i = 0; i < __len; ++i)
		dst[i] = src[i];
}

void usb_mon_on_rcv(usb_class_t * cl, unsigned int ep_id, unsigned int len)
{
	struct usb_cdc_acm_dev * dev = (struct usb_cdc_acm_dev *)cl;
	int n;

	if (dev->rx_cnt != dev->rx_pos)
		DCC_LOG(LOG_WARNING, "overflow!");
	
	n = usb_dev_ep_pkt_recv(dev->usb, dev->out_ep, 
							dev->rx_buf, CDC_EP_IN_MAX_PKT_SIZE);

	dev->rx_pos = 0;
	dev->rx_cnt = n;

#if 0
	dev->rx_buf[n] = '\0';
	DCC_LOGSTR(LOG_TRACE, "'%s'", dev->rx_buf);
#endif

	usb_dev_ep_ctl(dev->usb, dev->out_ep, USB_EP_RECV_OK);
	DCC_LOG(LOG_INFO, "COMM_RCV!");
	dmon_signal(DMON_COMM_RCV);
}

void usb_mon_on_eot(usb_class_t * cl, unsigned int ep_id)
{
	DCC_LOG(LOG_INFO, "COMM_EOT");
	dmon_signal(DMON_COMM_EOT);
}

void usb_mon_on_eot_int(usb_class_t * cl, unsigned int ep_id)
{
	DCC_LOG1(LOG_TRACE, "ep_id=%d", ep_id);
}

const usb_dev_ep_info_t usb_mon_in_info = {
	.addr = USB_ENDPOINT_IN + EP_IN_ADDR,
	.attr = ENDPOINT_TYPE_BULK,
	.mxpktsz = CDC_EP_IN_MAX_PKT_SIZE,
	.on_in = usb_mon_on_eot
};

const usb_dev_ep_info_t usb_mon_out_info = {
	.addr = USB_ENDPOINT_OUT + EP_OUT_ADDR,
	.attr = ENDPOINT_TYPE_BULK,
	.mxpktsz = CDC_EP_OUT_MAX_PKT_SIZE,
	.on_out = usb_mon_on_rcv
};

const usb_dev_ep_info_t usb_mon_int_info = {
	.addr = USB_ENDPOINT_IN + EP_INT_ADDR,
	.attr = ENDPOINT_TYPE_INTERRUPT,
	.mxpktsz = CDC_EP_INT_MAX_PKT_SIZE,
	.on_in = usb_mon_on_eot_int
};

int usb_mon_on_setup(usb_class_t * cl, struct usb_request * req, void ** ptr) 
{
	struct usb_cdc_acm_dev * dev = (struct usb_cdc_acm_dev *) cl;
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
			DCC_LOG1(LOG_TRACE, "GetDesc: String[%d]", n);
			if (n < dev->strcnt) {
				*ptr = (void *)dev->str[n];
				len = dev->str[n][0];
			}
			break;
		}

		len = -1;
		DCC_LOG1(LOG_TRACE, "GetDesc: %d ?", desc);
		break;

	case STD_SET_ADDRESS:
		DCC_LOG1(LOG_TRACE, "SetAddr: %d [ADDRESS]", value);
		break;

	case STD_SET_CONFIGURATION: {
		DCC_LOG1(LOG_TRACE, "SetCfg: %d", value);
		if (value) {
			dev->in_ep = usb_dev_ep_init(dev->usb, &usb_mon_in_info, NULL, 0);
			dev->out_ep = usb_dev_ep_init(dev->usb, &usb_mon_out_info, NULL, 0);
			dev->int_ep = usb_dev_ep_init(dev->usb, &usb_mon_int_info, NULL, 0);
			usb_dev_ep_ctl(dev->usb, dev->out_ep, USB_EP_RECV_OK);
		} else {
			usb_dev_ep_ctl(dev->usb, dev->in_ep, USB_EP_DISABLE);
			usb_dev_ep_ctl(dev->usb, dev->out_ep, USB_EP_DISABLE);
			usb_dev_ep_ctl(dev->usb, dev->int_ep, USB_EP_DISABLE);
		}
		DCC_LOG(LOG_TRACE, "[CONFIGURED]");
		break;
	}

	case STD_GET_CONFIGURATION:
		DCC_LOG(LOG_TRACE, "GetCfg");
		break;

	case STD_GET_STATUS_INTERFACE:
		DCC_LOG(LOG_TRACE, "GetStIf");
		break;

	case STD_GET_STATUS_ZERO:
		DCC_LOG(LOG_TRACE, "GetStZr");
		break;

	case STD_GET_STATUS_ENDPOINT:
		index &= 0x0f;
		DCC_LOG1(LOG_TRACE, "GetStEpt:%d", index);
		break;

	case SET_LINE_CODING:
		__memcpy(&dev->acm.lc, dev->ctr_buf, sizeof(struct cdc_line_coding));
        DCC_LOG1(LOG_TRACE, "dsDTERate=%d", dev->acm.lc.dwDTERate);
        DCC_LOG1(LOG_TRACE, "bCharFormat=%d", dev->acm.lc.bCharFormat);
        DCC_LOG1(LOG_TRACE, "bParityType=%d", dev->acm.lc.bParityType);
        DCC_LOG1(LOG_TRACE, "bDataBits=%d", dev->acm.lc.bDataBits);
		if ((dev->acm.flags & ACM_LC_SET) == 0) {
			dev->acm.flags |= ACM_LC_SET;
			DCC_LOG1(LOG_TRACE, "Signal:%d", DMON_COMM_CTL);
			DCC_LOG1(LOG_TRACE, "dev->acm.flags<-%0p", &dev->acm.flags);
			/* signal monitor */
			dmon_signal(DMON_COMM_CTL);
		}
		break;

	case GET_LINE_CODING:
		DCC_LOG(LOG_TRACE, "CDC GetLn");
		/* Return Line Coding */
		*ptr = (void *)&dev->acm.lc;
		len = sizeof(struct cdc_line_coding);
		break;

	case SET_CONTROL_LINE_STATE:
		dev->acm.control = value;
		DCC_LOG2(LOG_TRACE, "CDC_DTE_PRESENT=%d ACTIVATE_CARRIER=%d",
				(value & CDC_DTE_PRESENT) ? 1 : 0,
				(value & CDC_ACTIVATE_CARRIER) ? 1 : 0);

		/* signal monitor */
		dmon_signal(DMON_COMM_CTL);
		break;

	default:
		DCC_LOG5(LOG_TRACE, "CDC t=%x r=%x v=%x i=%d l=%d",
				req->type, req->request, value, index, len);
		break;
	}

	return len;
}

const usb_dev_ep_info_t usb_mon_ep0_info = {
	.addr = 0,
	.attr = ENDPOINT_TYPE_CONTROL,
	.mxpktsz = EP0_MAX_PKT_SIZE,
	.on_setup = usb_mon_on_setup
};

void usb_mon_on_reset(usb_class_t * cl)
{
	struct usb_cdc_acm_dev * dev = (struct usb_cdc_acm_dev *)cl;
	DCC_LOG(LOG_TRACE, "...");
	/* clear input buffer */
	dev->rx_cnt = 0;
	dev->rx_pos = 0;
	/* invalidate th line coding structure */
    dev->acm.lc.dwDTERate = 38400;
    dev->acm.lc.bCharFormat = 0;
    dev->acm.lc.bParityType = 0;
    dev->acm.lc.bDataBits = 8;
	/* reset control lines */
	dev->acm.control = 0;
	/* clear all flags */
	dev->acm.flags = 0;
	/* initializes EP0 */
	dev->ctl_ep = usb_dev_ep_init(dev->usb, &usb_mon_ep0_info, 
								  dev->ctr_buf, CDC_CTR_BUF_LEN);
}

void usb_mon_on_suspend(usb_class_t * cl)
{
	struct usb_cdc_acm_dev * dev = (struct usb_cdc_acm_dev *)cl;
	DCC_LOG(LOG_TRACE, "...");
	dev->acm.control = 0;
	dev->acm.flags |= ACM_USB_SUSPENDED;
}

void usb_mon_on_wakeup(usb_class_t * cl)
{
	struct usb_cdc_acm_dev * dev = (struct usb_cdc_acm_dev *)cl;
	DCC_LOG(LOG_TRACE, "...");
	dev->acm.flags &= ~ACM_USB_SUSPENDED;
}

void usb_mon_on_error(usb_class_t * cl, int code)
{
}

int dmon_comm_send(struct dmon_comm * comm, const void * buf, unsigned int len)
{
	struct usb_cdc_acm_dev * dev = (struct usb_cdc_acm_dev *)comm;
	uint8_t * ptr = (uint8_t *)buf;
	unsigned int rem = len;
	int ret;
	int n;

	while (rem) {
		if ((n = usb_dev_ep_pkt_xmit(dev->usb, dev->in_ep, ptr, rem)) < 0) {
			DCC_LOG(LOG_WARNING, "usb_dev_ep_pkt_xmit() failed!!");
			return n;
		}

		DCC_LOG1(LOG_INFO, "n=%d!!", n);

		rem -= n;
		ptr += n;

		if ((ret = dmon_wait(DMON_COMM_EOT)) < 0) {
			DCC_LOG1(LOG_WARNING, "ret=%d!!", ret);
			return ret;
		}
	}

	return len;
}

int dmon_comm_recv(struct dmon_comm * comm, void * buf, unsigned int len)
{
	struct usb_cdc_acm_dev * dev = (struct usb_cdc_acm_dev *)comm;
	int ret;
	int n;

	do {
		if ((n = dev->rx_cnt - dev->rx_pos) > 0) {
			/* get data from the rx buffer if not empty */
			n = MIN(n, len);
			__memcpy(buf, &dev->rx_buf[dev->rx_pos], n);
			dev->rx_pos += n;
			return n;
		}
	} while ((ret = dmon_wait(DMON_COMM_RCV)) == 0);

	return ret;
}

int dmon_comm_connect(struct dmon_comm * comm)
{
	struct usb_cdc_acm_dev * dev = (struct usb_cdc_acm_dev *)comm;
	struct cdc_notification * pkt;
	uint32_t buf[4];

	int ret;

//	while ((dev->acm.flags & ACM_LC_SET) == 0) {
	while ((dev->acm.control & CDC_DTE_PRESENT) == 0) {
		if ((ret = dmon_wait(DMON_COMM_CTL)) < 0) {
			DCC_LOG1(LOG_WARNING, "ret=%d!!", ret);
			return ret;
		}
	}

	pkt = (struct cdc_notification *)buf;
	/* bmRequestType */
	pkt->bmRequestType = USB_CDC_NOTIFICATION;
	/* bNotification */
	pkt->bNotification = CDC_NOTIFICATION_SERIAL_STATE;
	/* wValue */
	pkt->wValue = 0;
	/* wIndex */
	pkt->wIndex = 1;
	/* wLength */
	pkt->wLength = 2;
	/* data */
	pkt->bData[0] = CDC_SERIAL_STATE_TX_CARRIER | CDC_SERIAL_STATE_RX_CARRIER;
	pkt->bData[1] = 0;

	ret = usb_dev_ep_pkt_xmit(dev->usb, dev->int_ep, pkt, 
							  sizeof(struct cdc_notification));
	if (ret < 0) {
		DCC_LOG(LOG_WARNING, "usb_dev_ep_tx_start() failed!");
		return ret;
	}

	return 0;
}

bool dmon_comm_isconnected(struct dmon_comm * comm)
{
	struct usb_cdc_acm_dev * dev = (struct usb_cdc_acm_dev *)comm;
	return (dev->acm.control & CDC_DTE_PRESENT) ? true : false;
}


struct usb_cdc_acm_dev usb_cdc_rt;

const usb_class_events_t usb_mon_ev = {
	.on_reset = usb_mon_on_reset,
	.on_suspend = usb_mon_on_suspend,
	.on_wakeup = usb_mon_on_wakeup,
	.on_error = usb_mon_on_error
};

void * usb_mon_init(const usb_dev_t * usb, 
					const uint8_t * const str[], 
					unsigned int strcnt)
{
	struct usb_cdc_acm_dev * dev = &usb_cdc_rt;
	usb_class_t * cl =  (usb_class_t *)dev;

	/* initialize USB device */
	dev->usb = (usb_dev_t *)usb;

	dev->rx_cnt = 0;
	dev->rx_pos = 0;
	dev->str = str;
	dev->strcnt = strcnt;
	
	usb_dev_init(dev->usb, cl, &usb_mon_ev);
	DCC_LOG1(LOG_TRACE, "dev->acm.flags<-%0p", &dev->acm.flags);

	return (void *)dev;
}

