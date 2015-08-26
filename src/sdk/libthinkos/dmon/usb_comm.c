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
#include <sys/param.h>
#include <sys/serial.h>
#include <string.h>
#include <stdbool.h>

#include <sys/usb-dev.h>
#include <sys/usb-cdc.h>

#include <sys/dcclog.h>

#define __THINKOS_DMON__
#include <thinkos_dmon.h>

#if (THINKOS_ENABLE_MONITOR)

#define EP0_ADDR 0
#define EP0_MAX_PKT_SIZE 64

#define EP_OUT_ADDR 1
#define EP_IN_ADDR  2
#define EP_INT_ADDR 3

#ifndef CDC_EP_OUT_MAX_PKT_SIZE
#define CDC_EP_OUT_MAX_PKT_SIZE 64
#endif

#ifndef CDC_EP_IN_MAX_PKT_SIZE 
#define CDC_EP_IN_MAX_PKT_SIZE 64
#endif

#ifndef CDC_EP_INT_MAX_PKT_SIZE 
#define CDC_EP_INT_MAX_PKT_SIZE 64
#endif

struct cdc_acm_descriptor_config {
	struct usb_descriptor_configuration cfg;
	struct usb_descriptor_interface comm_if;
	struct cdc_header_descriptor hdr;
	struct cdc_call_management_descriptor cm;
	struct cdc_abstract_control_management_descriptor acm;
	struct cdc_union_1slave_descriptor un;
#if 1
	struct usb_descriptor_endpoint ep_int;
#endif
	struct usb_descriptor_interface if_data;
	struct usb_descriptor_endpoint ep_out;
	struct usb_descriptor_endpoint ep_in;
} __attribute__((__packed__));

#define ATMEL_VCOM_PRODUCT_ID 0x6119
#define ST_VCOM_PRODUCT_ID    0x5740

#ifndef CDC_ACM_PRODUCT_ID
#define CDC_ACM_PRODUCT_ID ST_VCOM_PRODUCT_ID 
#endif

const struct usb_descriptor_device cdc_acm_desc_dev = {
		/* Size of this descriptor in bytes */
		sizeof(struct usb_descriptor_device),
		/* DEVICE descriptor type */
		USB_DESCRIPTOR_DEVICE,
		/* USB specification release number */
		USB1_10,
		/* Class code */
		USB_CLASS_COMMUNICATION,
		/* Subclass code */
		0x00,
		/* Protocol code */
		0x00,
		/* Control endpoint 0 max. packet size */
		EP0_MAX_PKT_SIZE,
		/* Vendor ID */
		USB_VENDOR_ST,
		/* Product ID */
		CDC_ACM_PRODUCT_ID,
		/* Device release number */
		0x0002,
		/* Index of manufacturer string descriptor */
		0x01,
		/* Index of product string descriptor */
		0x02,
		/* Index of S.N.  string descriptor */
		0x03,
		/* Number of possible configurations */
		0x01
};

/* Configuration 1 descriptor */
const struct cdc_acm_descriptor_config cdc_acm_desc_cfg = {
		.cfg = {
			/* Size of this descriptor in bytes */
			sizeof(struct usb_descriptor_configuration),
			/* CONFIGURATION descriptor type */
			USB_DESCRIPTOR_CONFIGURATION,
			/* Total length of data returned for this configuration */
			sizeof(struct cdc_acm_descriptor_config),
			/* Number of interfaces for this configuration */
			2,
			/* Value to use as an argument for the
			   Set Configuration request to select this configuration */
			1,
			/* Index of string descriptor describing this configuration */
			0,
			/* Configuration characteristics - attributes */
			USB_CONFIG_SELF_NOWAKEUP,
			/* Maximum power consumption of the device */
			USB_POWER_MA(250)
		},
		/* Communication Class Interface Descriptor Requirement */
		.comm_if = {
			/* Size of this descriptor in bytes */
			sizeof(struct usb_descriptor_interface),
			/* INTERFACE descriptor type */
			USB_DESCRIPTOR_INTERFACE,
			/* Number of this interface */
			0,
			/* Value used to select this alternate setting */
			0,
			/* Number of endpoints used by this interface
			   (excluding endpoint zero) */
			1,
			/* Class code */
			CDC_INTERFACE_COMMUNICATION,
			/* Sub-class */
			CDC_ABSTRACT_CONTROL_MODEL,
			/* Protocol code: (V.25ter, Common AT commands)*/
			CDC_PROTOCOL_COMMON_AT_COMMANDS,
			/* Index of string descriptor describing this interface */
			0x04
		},
		/* Header Functional Descriptor */
		.hdr = {
			/* Size of this descriptor in bytes */
			sizeof(struct cdc_header_descriptor),
			/* CS_INTERFACE descriptor type */
			CDC_CS_INTERFACE,
			/* Header functional descriptor subtype */
			CDC_HEADER,
			/* USB CDC specification release version */
			CDC1_10
		},


		/* Call Management Functional Descriptor */
		.cm = {
			/* Size of this descriptor in bytes */
			sizeof(struct cdc_call_management_descriptor),
			/* CS_INTERFACE descriptor type */
			CDC_CS_INTERFACE,
			/* Call management functional descriptor subtype */
			CDC_CALL_MANAGEMENT,
			/* The capabilities that this configuration supports */
			1, /* D1 + D0 */
			/* Interface number of the data class
			   interface used for call management */
			1
		},
		/* Abstract Control Management Functional Descriptor */
		.acm = {
			/* Size of this descriptor in bytes */
			sizeof(struct cdc_abstract_control_management_descriptor),
			/* CS_INTERFACE descriptor type */
			CDC_CS_INTERFACE,
			/* Abstract control management functional descriptor subtype */
			CDC_ABSTRACT_CONTROL_MANAGEMENT,
			/* Capabilities supported by this configuration */
			0x06
		},
		/* Union Functional Descriptor */
		.un = {
			{
				/* Size of this descriptor in bytes */
				sizeof(struct cdc_union_1slave_descriptor),
				/* CS_INTERFACE descriptor type */
				CDC_CS_INTERFACE,
				/* Union functional descriptor subtype */
				CDC_UNION,
				/* The interface number designated as master */
				0
			},
			/* The interface number designated as first slave */
			{ 1 }
		},
#if 1
		/* Endpoint 3 descriptor */
		.ep_int = {
			/* Size of this descriptor in bytes */
			sizeof(struct usb_descriptor_endpoint),
			/* ENDPOINT descriptor type */
			USB_DESCRIPTOR_ENDPOINT,
			/* Address of the endpoint on the USB device */
			USB_ENDPOINT_IN + EP_INT_ADDR,
			/* Endpoint attributes when configured */
			ENDPOINT_TYPE_INTERRUPT,
			/* Maximum packet size this endpoint is capable
			   of sending or receiving */
			CDC_EP_INT_MAX_PKT_SIZE,
			/* Interval for polling endpoint (ms) */
			100
		},
#endif
		/* Data Class Interface Descriptor Requirement */
		.if_data = {
			/* Size of this descriptor in bytes */
			sizeof(struct usb_descriptor_interface),
			/* INTERFACE descriptor type */
			USB_DESCRIPTOR_INTERFACE,
			/* Number of this interface */
			1,
			/* Value used to select this alternate setting */
			0,
			/* Number of endpoints used by this interface
			   (excluding endpoint zero) */
			2,
			/* Class code */
			CDC_INTERFACE_DATA,
			/* Sub-class */
			0,
			/* Protocol code */
			0,
			/* Index of string descriptor describing this interface */
			0
		},
		/* First alternate setting */
		/* Endpoint 1 descriptor */
		.ep_out = {
			/* Size of this descriptor in bytes */
			sizeof(struct usb_descriptor_endpoint),
			/* ENDPOINT descriptor type */
			USB_DESCRIPTOR_ENDPOINT,
			/* Address of the endpoint on the USB device */
			USB_ENDPOINT_OUT + EP_OUT_ADDR,
			/* Endpoint attributes when configured */
			ENDPOINT_TYPE_BULK,
			/* Maximum packet size this endpoint is capable of
			   sending or receiving */
			CDC_EP_OUT_MAX_PKT_SIZE,
			/* Interval for polling endpoint for data transfers */
			0x0
		},
		/* Endpoint 2 descriptor */
		.ep_in = {
			/* Size of this descriptor in bytes */
			sizeof(struct usb_descriptor_endpoint),
			/* ENDPOINT descriptor type */
			USB_DESCRIPTOR_ENDPOINT,
			/* Address of the endpoint on the USB device */
			USB_ENDPOINT_IN + EP_IN_ADDR,
			/* Endpoint attributes when configured */
			ENDPOINT_TYPE_BULK,
			/* Maximum packet size this endpoint is capable of
			   sending or receiving */
			CDC_EP_IN_MAX_PKT_SIZE,
			/* Interval for polling endpoint for data transfers */
			0x0
		}
};


#define CDC_STOP_BITS_1   (0 << 0)
#define CDC_STOP_BITS_1_5 (1 << 0)
#define CDC_STOP_BITS_2   (2 << 0)
#define CDC_STOP_BITS_ERR (3 << 0)
#define CDC_CHAR_FORMAT_SET(VAL)  (((VAL) & 0x3) << 0)
#define CDC_CHAR_FORMAT_GET(VAL)  (((VAL) >> 0) & 0x3)
#define CDC_STOP_BITS_MAX 2

#define CDC_PARITY_NONE   (0 << 2)
#define CDC_PARITY_ODD    (1 << 2)
#define CDC_PARITY_EVEN   (2 << 2)
#define CDC_PARITY_MARK   (3 << 2)
#define CDC_PARITY_SPACE  (4 << 2)
#define CDC_PARITY_ERR    (7 << 2)
#define CDC_PARITY_SET(VAL)  (((VAL) & 0x7) << 2)
#define CDC_PARITY_GET(VAL)  (((VAL) >> 2) & 0x7)
#define CDC_PARITY_MAX    4

#define CDC_DATA_BITS_5   (0 << 5)
#define CDC_DATA_BITS_6   (1 << 5)
#define CDC_DATA_BITS_7   (2 << 5)
#define CDC_DATA_BITS_8   (3 << 5)
#define CDC_DATA_BITS_16  (4 << 5)
#define CDC_DATA_BITS_ERR (7 << 5)
#define CDC_DATA_BITS_SET(VAL)  data_bit_lut[(VAL) & 0x1f]
#define CDC_DATA_BITS_GET(VAL)  data_bit_rev_lut[((VAL) >> 5) & 0x7]
#define CDC_DATA_BITS_MAX 16


#define LANG_STR_SZ              4
/* LangID = 0x0409: U.S. English */
const uint8_t cdc_acm_lang_str[LANG_STR_SZ] = {
	LANG_STR_SZ, USB_DESCRIPTOR_STRING,
	0x09, 0x04
};

#define VENDOR_STR_SZ            38
const uint8_t cdc_acm_vendor_str[VENDOR_STR_SZ] = {
	VENDOR_STR_SZ, USB_DESCRIPTOR_STRING,
	/* Manufacturer: "STMicroelectronics" */
	'S', 0, 'T', 0, 'M', 0, 'i', 0, 'c', 0, 'r', 0, 'o', 0, 'e', 0, 'l', 0, 
	'e', 0, 'c', 0, 't', 0, 'r', 0, 'o', 0, 'n', 0, 'i', 0, 'c', 0, 's', 0
};


#define PRODUCT_STR_SZ           44
const uint8_t cdc_acm_product_str[PRODUCT_STR_SZ] = {
	PRODUCT_STR_SZ, USB_DESCRIPTOR_STRING,
	/* Product name: "ThinkOS Debug Monitor" */
	'T', 0, 'h', 0, 'i', 0, 'n', 0, 'k', 0, 'O', 0, 'S', 0, ' ', 0, 'D', 0, 
	'e', 0, 'b', 0, 'u', 0, 'g', 0, ' ', 0, 'M', 0, 'o', 0, 'n', 0, 'i', 0, 
	't', 0, 'o', 0, 'r', 0,
};

#define SERIAL_STR_SZ            30
const uint8_t cdc_acm_serial_str[SERIAL_STR_SZ] = {
	SERIAL_STR_SZ, USB_DESCRIPTOR_STRING,
	/* Serial number: "54 68 69 6e 6b 4f 53" */
	'5', 0, '4', 0, '6', 0, '8', 0, '6', 0, '9', 0, '6', 0, 'E', 0, 
	'6', 0, 'B', 0, '4', 0, 'F', 0, '5', 0, '3', 0
};

#define INTERFACE_STR_SZ         16
const uint8_t cdc_acm_interface_str[INTERFACE_STR_SZ] = {
	INTERFACE_STR_SZ, USB_DESCRIPTOR_STRING,
	/* Interface 0: "ST VCOM" */
	'S', 0, 'T', 0, ' ', 0, 'V', 0, 'C', 0, 'O', 0, 'M', 0
};

const uint8_t * const cdc_acm_str[] = {
	cdc_acm_lang_str,
	cdc_acm_vendor_str,
	cdc_acm_product_str,
	cdc_acm_serial_str,
	cdc_acm_interface_str
};

#define STRCNT() (sizeof(cdc_acm_str) / sizeof(uint8_t *))

const struct cdc_line_coding usb_cdc_lc = {
    .dwDTERate = 38400,
    .bCharFormat = 0,
    .bParityType = 0,
    .bDataBits = 8
};

#define ACM_USB_SUSPENDED (1 << 1)
#define ACM_CONNECTED     (1 << 2)

#define CDC_CTR_BUF_LEN 16

struct usb_cdc_acm_dev {
	/* underling USB device */
	struct usb_dev * usb;

//	struct cdc_line_coding acm_lc;

	volatile uint8_t acm_ctrl; /* modem control lines */

	/* number of strings */
	uint8_t ctl_ep;
	uint8_t in_ep;
	uint8_t out_ep;
#if 1
	uint8_t int_ep;
#endif
	uint8_t rx_flowctrl;
	uint8_t rx_paused;

	volatile uint8_t rx_cnt; 
	volatile uint8_t rx_pos; 

	uint8_t rx_buf[CDC_EP_IN_MAX_PKT_SIZE + 1];

	uint32_t ctr_buf[CDC_CTR_BUF_LEN / 4];
};

void usb_mon_on_rcv(usb_class_t * cl, unsigned int ep_id, unsigned int len)
{
	struct usb_cdc_acm_dev * dev = (struct usb_cdc_acm_dev *)cl;
	int pos = dev->rx_pos;
	int cnt = dev->rx_cnt;
	int free;
	int n;

	if (cnt == pos) {
		cnt = 0;
		dev->rx_pos = pos = 0;
		free = CDC_EP_IN_MAX_PKT_SIZE;
		DCC_LOG3(LOG_INFO, "1. pos=%d free=%d len=%d.......", pos, free, len);
	} else {
		free = CDC_EP_IN_MAX_PKT_SIZE - cnt;
		if (free < len) {
			if (dev->rx_flowctrl) {
				dev->rx_paused = true;
				DCC_LOG(LOG_INFO, "RX paused!");
				return;
			} else {
				DCC_LOG(LOG_WARNING, "overflow!");
				cnt = 0;
				dev->rx_pos = pos = 0;
				free = CDC_EP_IN_MAX_PKT_SIZE; 
			}
		}
		DCC_LOG2(LOG_INFO, "2. pos=%d cnt=%d.......", pos, cnt);
	}

	n = usb_dev_ep_pkt_recv(dev->usb, dev->out_ep, &dev->rx_buf[cnt], free);
	if (n <= 0) {
		DCC_LOG(LOG_WARNING, "usb_dev_ep_pkt_recv() failed!");
	}
	dev->rx_cnt = cnt + n;

#if 0
	dev->rx_buf[n] = '\0';
	DCC_LOGSTR(LOG_INFO, "'%s'", dev->rx_buf);
#endif

	usb_dev_ep_ctl(dev->usb, dev->out_ep, USB_EP_RECV_OK);
	DCC_LOG(LOG_INFO, "COMM_RCV!");
	dmon_signal(DMON_COMM_RCV);
}

void usb_mon_on_eot(usb_class_t * cl, unsigned int ep_id)
{
	DCC_LOG(LOG_MSG, "COMM_EOT");
	dmon_signal(DMON_COMM_EOT);
}

void usb_mon_on_eot_int(usb_class_t * cl, unsigned int ep_id)
{
	DCC_LOG1(LOG_MSG, "ep_id=%d", ep_id);
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

#if 1
const usb_dev_ep_info_t usb_mon_int_info = {
	.addr = USB_ENDPOINT_IN + EP_INT_ADDR,
	.attr = ENDPOINT_TYPE_INTERRUPT,
	.mxpktsz = CDC_EP_INT_MAX_PKT_SIZE,
	.on_in = usb_mon_on_eot_int
};
#endif

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
			DCC_LOG1(LOG_INFO, "GetDesc: Device: len=%d", len);
			break;
		}

		if (desc == USB_DESCRIPTOR_CONFIGURATION) {
			/* Return Configuration Descriptor */
			*ptr = (void *)&cdc_acm_desc_cfg;
			len = sizeof(struct cdc_acm_descriptor_config);
			DCC_LOG1(LOG_INFO, "GetDesc: Config: len=%d", len);
			break;
		}

		if (desc == USB_DESCRIPTOR_STRING) {
			int n = value & 0xff;
			DCC_LOG1(LOG_INFO, "GetDesc: String[%d]", n);
			if (n < STRCNT()) {
				*ptr = (void *)cdc_acm_str[n];
				len = cdc_acm_str[n][0];
			}
			break;
		}

		len = -1;
		DCC_LOG1(LOG_INFO, "GetDesc: %d ?", desc);
		break;

	case STD_SET_ADDRESS:
		DCC_LOG1(LOG_INFO, "SetAddr: %d [ADDRESS]", value);
		break;

	case STD_SET_CONFIGURATION: {
		DCC_LOG1(LOG_INFO, "SetCfg: %d", value);
		if (value) {
			dev->in_ep = usb_dev_ep_init(dev->usb, &usb_mon_in_info, NULL, 0);
			dev->out_ep = usb_dev_ep_init(dev->usb, &usb_mon_out_info, NULL, 0);
#if 1
			dev->int_ep = usb_dev_ep_init(dev->usb, &usb_mon_int_info, NULL, 0);
#endif
			usb_dev_ep_ctl(dev->usb, dev->out_ep, USB_EP_RECV_OK);
		} else {
			usb_dev_ep_ctl(dev->usb, dev->in_ep, USB_EP_DISABLE);
			usb_dev_ep_ctl(dev->usb, dev->out_ep, USB_EP_DISABLE);
#if 1
			usb_dev_ep_ctl(dev->usb, dev->int_ep, USB_EP_DISABLE);
#endif
		}
		DCC_LOG(LOG_INFO, "[CONFIGURED]");
		break;
	}

	case STD_GET_CONFIGURATION:
		DCC_LOG(LOG_INFO, "GetCfg");
		break;

	case STD_GET_STATUS_INTERFACE:
		DCC_LOG(LOG_INFO, "GetStIf");
		break;

	case STD_GET_STATUS_ZERO:
		DCC_LOG(LOG_INFO, "GetStZr");
		break;

	case STD_GET_STATUS_ENDPOINT:
		index &= 0x0f;
		DCC_LOG1(LOG_INFO, "GetStEpt:%d", index);
		break;

	case SET_LINE_CODING: 
		{
			struct cdc_line_coding * lc;
			lc = (struct cdc_line_coding *)dev->ctr_buf;
			(void)lc;
			DCC_LOG3(LOG_INFO, "CDC SetLn: idx=%d val=%d len=%d",
					 index, value, len);
			DCC_LOG1(LOG_INFO, "dsDTERate=%d", lc->dwDTERate);
			DCC_LOG1(LOG_INFO, "bCharFormat=%d", lc->bCharFormat);
			DCC_LOG1(LOG_INFO, "bParityType=%d", lc->bParityType);
			DCC_LOG1(LOG_INFO, "bDataBits=%d", lc->bDataBits);
			break;
		}

	case GET_LINE_CODING:
		DCC_LOG(LOG_INFO, "CDC GetLn");
		/* Return Line Coding */
		*ptr = (void *)&usb_cdc_lc;
		len = sizeof(struct cdc_line_coding);
		break;

	case SET_CONTROL_LINE_STATE:
		dev->acm_ctrl = value;
		DCC_LOG2(LOG_INFO, "CDC_DTE_PRESENT=%d ACTIVATE_CARRIER=%d",
				(value & CDC_DTE_PRESENT) ? 1 : 0,
				(value & CDC_ACTIVATE_CARRIER) ? 1 : 0);

		/* signal monitor */
		dmon_signal(DMON_COMM_CTL);
		break;

	default:
		DCC_LOG5(LOG_INFO, "CDC t=%x r=%x v=%x i=%d l=%d",
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
	DCC_LOG(LOG_MSG, "...");
	/* clear input buffer */
	dev->rx_cnt = 0;
	dev->rx_pos = 0;
	/* reset control lines */
	dev->acm_ctrl = 0;
	/* initializes EP0 */
	dev->ctl_ep = usb_dev_ep_init(dev->usb, &usb_mon_ep0_info, 
								  dev->ctr_buf, CDC_CTR_BUF_LEN);
}

void usb_mon_on_suspend(usb_class_t * cl)
{
	struct usb_cdc_acm_dev * dev = (struct usb_cdc_acm_dev *)cl;
	DCC_LOG(LOG_INFO, "...");
	dev->acm_ctrl = 0;
}

void usb_mon_on_wakeup(usb_class_t * cl)
{
	DCC_LOG(LOG_INFO, "...");
}

void usb_mon_on_error(usb_class_t * cl, int code)
{
}

int dmon_comm_send(struct dmon_comm * comm, const void * buf, unsigned int len)
{
	struct usb_cdc_acm_dev * dev = (struct usb_cdc_acm_dev *)comm;
	uint8_t * ptr = (uint8_t *)buf;
	unsigned int rem;
	int ret;
	int n;

	rem = len;
	while (rem) {
		if ((n = usb_dev_ep_pkt_xmit(dev->usb, dev->in_ep, ptr, rem)) < 0) {
			DCC_LOG(LOG_WARNING, "usb_dev_ep_pkt_xmit() failed!!");
			return n;
		}

		DCC_LOG1(LOG_MSG, "n=%d!!", n);

		rem -= n;
		ptr += n;

		if ((ret = dmon_wait(DMON_COMM_EOT)) < 0) {
			DCC_LOG1(LOG_WARNING, "ret=%d!!", ret);
			return ret;
		}

		DCC_LOG(LOG_MSG, "EOT");
	}

	DCC_LOG1(LOG_MSG, "return=%d.", len);

	return len;
}

int dmon_comm_recv(struct dmon_comm * comm, void * buf, unsigned int len)
{
	struct usb_cdc_acm_dev * dev = (struct usb_cdc_acm_dev *)comm;
	int pos;
	int cnt;
	int ret;
	int n;

	do {
		pos = dev->rx_pos;
		cnt = dev->rx_cnt;
		if ((n = cnt - pos) > 0) {
			/* get data from the rx buffer if not empty */
			n = MIN(n, len);
			DCC_LOG3(LOG_INFO, "1. pos=%d cnt=%d n=%d .......", pos, cnt, n);
			__thinkos_memcpy(buf, &dev->rx_buf[pos], n);
			pos += n;
			if (cnt == pos) {
				/* buffer is now empty */
				pos = 0;
				cnt = 0;
				if (dev->rx_paused) {
					dev->rx_paused = false;
					cnt = usb_dev_ep_pkt_recv(dev->usb, dev->out_ep, 
											  dev->rx_buf, 
											  CDC_EP_IN_MAX_PKT_SIZE);
					DCC_LOG2(LOG_INFO, "2. pos=%d cnt=%d unpaused!!!", 
							 pos, cnt);
					usb_dev_ep_ctl(dev->usb, dev->out_ep, USB_EP_RECV_OK);
				} 
				if (cnt == 0)
					dmon_clear(DMON_COMM_RCV); /* next call will block */
				dev->rx_cnt = cnt;
			} 
			dev->rx_pos = pos;
			return n;
		}

		DCC_LOG2(LOG_INFO, "3. pos=%d cnt=%d blocked!!!", pos, cnt);
	} while ((ret = dmon_expect(DMON_COMM_RCV)) == 0);

	return ret;
}

int dmon_comm_connect(struct dmon_comm * comm)
{
	struct usb_cdc_acm_dev * dev = (struct usb_cdc_acm_dev *)comm;
//	struct cdc_notification * pkt;
//	uint32_t buf[4];

	int ret;

	while ((dev->acm_ctrl & CDC_DTE_PRESENT) == 0) {
		DCC_LOG1(LOG_TRACE, "ctrl=%02x, waiting...", dev->acm_ctrl);
		if ((ret = dmon_wait(DMON_COMM_CTL)) < 0) {
			DCC_LOG1(LOG_WARNING, "ret=%d!!", ret);
			return ret;
		}
	}

#if 0
	if ((dev->acm.flags & ACM_CONNECTED) == 0) {
		dev->acm.flags |= ACM_CONNECTED;
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
		pkt->bData[0] = CDC_SERIAL_STATE_TX_CARRIER | 
			CDC_SERIAL_STATE_RX_CARRIER;
		pkt->bData[1] = 0;

		ret = usb_dev_ep_pkt_xmit(dev->usb, dev->int_ep, pkt, 
								  sizeof(struct cdc_notification));
		if (ret < 0) {
			DCC_LOG(LOG_WARNING, "usb_dev_ep_pkt_xmit() failed!");
			return ret;
		}
	}
#endif

	return 0;
}

bool dmon_comm_isconnected(struct dmon_comm * comm)
{
	struct usb_cdc_acm_dev * dev = (struct usb_cdc_acm_dev *)comm;
	return (dev->acm_ctrl & CDC_DTE_PRESENT) ? true : false;
}

void dmon_comm_rxflowctrl(struct dmon_comm * comm, bool en)
{
	struct usb_cdc_acm_dev * dev = (struct usb_cdc_acm_dev *)comm;
	dev->rx_flowctrl = en;
}

struct usb_cdc_acm_dev usb_cdc_rt;

const usb_class_events_t usb_mon_ev = {
	.on_reset = usb_mon_on_reset,
	.on_suspend = usb_mon_on_suspend,
	.on_wakeup = usb_mon_on_wakeup,
	.on_error = usb_mon_on_error
};

struct dmon_comm * usb_comm_init(const usb_dev_t * usb)
{
	struct usb_cdc_acm_dev * dev = &usb_cdc_rt;
	usb_class_t * cl =  (usb_class_t *)dev;

	/* initialize USB device */
	dev->usb = (usb_dev_t *)usb;

	dev->rx_cnt = 0;
	dev->rx_pos = 0;
	dev->rx_flowctrl = false;

	DCC_LOG(LOG_TRACE, "usb_dev_init()");
	usb_dev_init(dev->usb, cl, &usb_mon_ev);

	return (struct dmon_comm *)dev;
}

#endif

