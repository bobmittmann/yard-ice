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

#include <thinkos.h>
#define __THINKOS_IRQ__
#include <thinkos_irq.h>

struct usb_cdc_acm {
	/* modem bits */
	volatile uint8_t status; /* modem status lines */
	volatile uint8_t control; /* modem control lines */
	volatile uint8_t opt;
	struct cdc_line_coding lc;
};

#define ACM_LC_SET (1 << 0)

#define USB_CDC_IRQ_PRIORITY IRQ_PRIORITY_REGULAR
#define CDC_CTR_BUF_LEN 16

struct usb_cdc_acm_dev {
	/* underling USB device */
	struct usb_dev * usb;

	/* class specific block */
	struct usb_cdc_acm acm;

#ifndef CDC_RX_SEM_NO
	uint8_t rx_sem; /* RX semaphore */
#endif
#ifndef CDC_TX_FLAG_NO
	uint8_t tx_flag; /* TX event flag */
#endif
#ifndef CDC_CTL_FLAG_NO
	uint8_t ctl_flag; /* Control event flag */
#endif

	uint8_t rx_cnt; 
	uint8_t rx_pos; 
	uint8_t rx_buf[CDC_EP_IN_MAX_PKT_SIZE];


	uint32_t ctr_buf[CDC_CTR_BUF_LEN / 4];

	uint8_t ctl_ep;
	uint8_t in_ep;
	uint8_t out_ep;
	uint8_t int_ep;
};

#ifdef CDC_CTL_FLAG_NO
#define CTL_FLAG (THINKOS_FLAG_BASE + CDC_CTL_FLAG_NO)
#else
#define CTL_FLAG dev->ctl_flag
#endif

#ifdef CDC_TX_FLAG_NO
#define TX_FLAG (THINKOS_FLAG_BASE + CDC_TX_FLAG_NO)
#else
#define TX_FLAG dev->tx_flag
#endif

#ifdef CDC_RX_SEM_NO
#define RX_SEM (THINKOS_SEM_BASE + CDC_RX_SEM_NO)
#else
#define RX_SEM dev->rx_sem
#endif

int usb_cdc_on_rcv(usb_class_t * cl, unsigned int ep_id, unsigned int len)
{
	struct usb_cdc_acm_dev * dev = (struct usb_cdc_acm_dev *) cl;
	usb_dev_ep_nak(dev->usb, dev->out_ep, true);
	DCC_LOG2(LOG_INFO, "ep_id=%d len=%d", ep_id, len);
	thinkos_sem_post_i(RX_SEM);
	return 0;
}

int usb_cdc_on_eot(usb_class_t * cl, unsigned int ep_id)
{
	struct usb_cdc_acm_dev * dev = (struct usb_cdc_acm_dev *) cl;
	(void)dev;
	DCC_LOG2(LOG_INFO, "ep_id=%d flag=%d", ep_id, TX_FLAG);
	thinkos_flag_give_i(TX_FLAG);
	return 0;

}

int usb_cdc_on_eot_int(usb_class_t * cl, unsigned int ep_id)
{
	struct usb_cdc_acm_dev * dev = (struct usb_cdc_acm_dev *) cl;
	(void)dev;
	DCC_LOG1(LOG_TRACE, "ep_id=%d", ep_id);
	thinkos_flag_set_i(CTL_FLAG);
	return 0;
}

const usb_dev_ep_info_t usb_cdc_in_info = {
	.addr = USB_ENDPOINT_IN + EP_IN_ADDR,
	.attr = ENDPOINT_TYPE_BULK,
	.mxpktsz = CDC_EP_IN_MAX_PKT_SIZE,
	.on_in = usb_cdc_on_eot
};

const usb_dev_ep_info_t usb_cdc_out_info = {
	.addr = USB_ENDPOINT_OUT + EP_OUT_ADDR,
	.attr = ENDPOINT_TYPE_BULK,
	.mxpktsz = CDC_EP_OUT_MAX_PKT_SIZE,
	.on_out = usb_cdc_on_rcv
};

const usb_dev_ep_info_t usb_cdc_int_info = {
	.addr = USB_ENDPOINT_IN + EP_INT_ADDR,
	.attr = ENDPOINT_TYPE_INTERRUPT,
	.mxpktsz = CDC_EP_INT_MAX_PKT_SIZE,
	.on_in = usb_cdc_on_eot_int
};

int usb_cdc_on_setup(usb_class_t * cl, struct usb_request * req, void ** ptr) {
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
			*ptr = (void *)cdc_acm_str[n];
			len = cdc_acm_str[n][0];
			DCC_LOG1(LOG_INFO, "GetDesc: String[%d]", n);
			break;
		}

		len = -1;
		DCC_LOG1(LOG_INFO, "GetDesc: %d ?", desc);
		break;

	case STD_SET_ADDRESS:
		DCC_LOG1(LOG_INFO, "SetAddr: %d -------- [ADDRESS]", value);
		/* signal any pending threads */
//		__thinkos_ev_raise(dev->rx_ev);
		break;

	case STD_SET_CONFIGURATION: {
		DCC_LOG1(LOG_INFO, "SetCfg: %d", value);

		if (value) {
			dev->in_ep = usb_dev_ep_init(dev->usb, &usb_cdc_in_info, NULL, 0);
			dev->out_ep = usb_dev_ep_init(dev->usb, &usb_cdc_out_info, NULL, 0);
			usb_dev_ep_nak(dev->usb, dev->out_ep, true);
			dev->int_ep = usb_dev_ep_init(dev->usb, &usb_cdc_int_info, NULL, 0);
		} else {
			usb_dev_ep_disable(dev->usb, dev->in_ep);
			usb_dev_ep_disable(dev->usb, dev->out_ep);
			usb_dev_ep_disable(dev->usb, dev->int_ep);
		}

		dev->acm.opt = 0;
		/* signal any pending threads */
		thinkos_flag_set_i(CTL_FLAG);
		DCC_LOG(LOG_TRACE, "[CONFIGURED]");
		break;
	}

	case STD_GET_CONFIGURATION:
		DCC_LOG(LOG_INFO, "GetCfg");
		//              data = (udp->glb_stat & UDP_CONFG) ? 1 : 0;
		//                      usb_ep0_send_word(dev, 0);
		break;

	case STD_GET_STATUS_INTERFACE:
		DCC_LOG(LOG_INFO, "GetStIf");
		//                      usb_ep0_send_word(dev, 0);
		break;

	case STD_GET_STATUS_ZERO:
		DCC_LOG(LOG_INFO, "GetStZr");
		//                      usb_ep0_send_word(dev, 0);
		break;

	case STD_GET_STATUS_ENDPOINT:
		index &= 0x0f;
		DCC_LOG1(LOG_INFO, "GetStEpt:%d", index);
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

		if ((dev->acm.opt & ACM_LC_SET) == 0) {
			dev->acm.opt |= ACM_LC_SET;
			thinkos_flag_give_i(TX_FLAG);
		}
		thinkos_flag_set_i(CTL_FLAG);

		DCC_LOG3(LOG_INFO, "CDC SetLn: idx=%d val=%d len=%d",
				 index, value, len);

        memcpy(&dev->acm.lc, dev->ctr_buf, sizeof(struct cdc_line_coding));
        
        DCC_LOG1(LOG_INFO, "dsDTERate=%d", dev->acm.lc.dwDTERate);
        DCC_LOG1(LOG_INFO, "bCharFormat=%d", dev->acm.lc.bCharFormat);
        DCC_LOG1(LOG_INFO, "bParityType=%d", dev->acm.lc.bParityType);
        DCC_LOG1(LOG_INFO, "bDataBits=%d", dev->acm.lc.bDataBits);
		break;

	case GET_LINE_CODING:
		DCC_LOG(LOG_INFO, "CDC GetLn");
		/* Return Line Coding */
		*ptr = (void *)&dev->acm.lc;
		len = sizeof(struct cdc_line_coding);
		break;

	case SET_CONTROL_LINE_STATE:
		dev->acm.control = value;
		/* there might have threads waiting for
		   modem control line changes (DTR, RTS)
		   wake them up */
		thinkos_flag_set_i(CTL_FLAG);

		DCC_LOG3(LOG_INFO, "CDC SetCtrl: idx=%d val=%d len=%d",
				 index, value, len);

		DCC_LOG2(LOG_INFO, "CDC_DTE_PRESENT=%d ACTIVATE_CARRIER=%d",
				(value & CDC_DTE_PRESENT) ? 1 : 0,
				(value & CDC_ACTIVATE_CARRIER) ? 1 : 0);
		break;

	default:
		DCC_LOG5(LOG_INFO, "CDC t=%x r=%x v=%x i=%d l=%d",
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
	struct usb_cdc_acm_dev * dev = (struct usb_cdc_acm_dev *)cl;

	DCC_LOG(LOG_INFO, "...");

	/* initializes EP0 */
	dev->ctl_ep = usb_dev_ep_init(dev->usb, &usb_cdc_ep0_info, 
								  dev->ctr_buf, CDC_CTR_BUF_LEN);

	return 0;
}

int usb_cdc_on_suspend(usb_class_t * cl)
{
	struct usb_cdc_acm_dev * dev = (struct usb_cdc_acm_dev *)cl;

	DCC_LOG(LOG_INFO, "...");
	dev->acm.control = 0;
	thinkos_flag_set_i(CTL_FLAG);
	thinkos_flag_clr_i(TX_FLAG);

	return 0;
}

int usb_cdc_on_resume(usb_class_t * cl)
{
	struct usb_cdc_acm_dev * dev = (struct usb_cdc_acm_dev *)cl;

	DCC_LOG(LOG_INFO, "...");

	if (dev->acm.opt & ACM_LC_SET)
		thinkos_flag_give_i(TX_FLAG);

	return 0;
}

int usb_cdc_on_error(usb_class_t * cl, int code)
{
	DCC_LOG(LOG_INFO, "...");


	return 0;
}

volatile int tx_blk = 0;
volatile int tx_wr = 0;
volatile int tx_ep = 0;

int usb_cdc_write(usb_cdc_class_t * cl,
				  const void * buf, unsigned int len)
{
	struct usb_cdc_acm_dev * dev = (struct usb_cdc_acm_dev *)cl;
	uint8_t * ptr = (uint8_t *)buf;
	unsigned int rem = len;
	int n;

	tx_wr = 1;
	while (rem) {
#if 0
FIXME: Flexnet pannel do not set DTE_PRESENT nor ACTIVATE_CARRIER ....
//		while ((dev->acm.control & CDC_DTE_PRESENT) == 0) {
		while ((dev->acm.control & CDC_ACTIVATE_CARRIER) == 0) {
			DCC_LOG(LOG_INFO, "CTL wait...");
			thinkos_flag_wait(CTL_FLAG);
			DCC_LOG(LOG_INFO, "CTL wakeup...");
			thinkos_flag_clr_i(CTL_FLAG);
		}
#endif
		DCC_LOG2(LOG_INFO, "len=%d rem=%d", len, rem);
		tx_blk = 1;
		thinkos_flag_take(TX_FLAG);
		tx_blk = 0;
		DCC_LOG(LOG_INFO, "wakeup");

		tx_ep = 1;
		if ((n = usb_dev_ep_tx_start(dev->usb, dev->in_ep, ptr, rem)) < 0) {
			DCC_LOG(LOG_WARNING, "usb_dev_ep_tx_start() failed!!");
			return n;
		}
		tx_ep = 0;

		if (n == 0) {
			DCC_LOG(LOG_WARNING, "n == 0!!");
		}

		rem -= n;
		ptr += n;

	}
	tx_wr = 0;

	return len;
}

#if 0
#define __THINKOS_SYS__
#include <thinkos_sys.h>

void usb_cdc_dump_stat(usb_cdc_class_t * cl)
{
	struct usb_cdc_acm_dev * dev = (struct usb_cdc_acm_dev *)cl;
	struct thinkos_rt rt;

	(void)dev;

	thinkos_rt_snapshot(&rt);

	DCC_LOG5(LOG_INFO, "ep=%d wr=%d blk=%d TX_FLAG(%d)=%d", 
			 tx_ep, tx_wr, tx_blk, TX_FLAG, 
			 thinkos_flag_val(TX_FLAG));

	thinkos_trace_rt(&rt);
}
#endif

int usb_cdc_read(usb_cdc_class_t * cl, void * buf,
				 unsigned int len, unsigned int msec)
{
	struct usb_cdc_acm_dev * dev = (struct usb_cdc_acm_dev *)cl;
	int ret;
	int n;

	DCC_LOG2(LOG_INFO, "len=%d msec=%d", len, msec);
	
	if ((n = dev->rx_cnt - dev->rx_pos) > 0) {
		DCC_LOG(LOG_INFO, "read from intern buffer");
		goto read_from_buffer;
	};

	usb_dev_ep_nak(dev->usb, dev->out_ep, false);

	if ((ret = thinkos_sem_timedwait(RX_SEM, msec)) < 0) {
		if (ret == THINKOS_ETIMEDOUT) {
			DCC_LOG(LOG_INFO, "timeout!!");
		}
		return ret;
	}

	if (len >= CDC_EP_IN_MAX_PKT_SIZE) {
		n = usb_dev_ep_pkt_recv(dev->usb, dev->out_ep, buf, len);
		DCC_LOG1(LOG_INFO, "wakeup, pkt rcv extern buffer: %d bytes", n);
		return n;
	} 
	
	n = usb_dev_ep_pkt_recv(dev->usb, dev->out_ep, 
								dev->rx_buf, CDC_EP_IN_MAX_PKT_SIZE);
	DCC_LOG1(LOG_INFO, "wakeup, pkt rcv intern buffer: %d bytes", n);
		
	dev->rx_pos = 0;
	dev->rx_cnt = n;

read_from_buffer:
	DCC_LOG(LOG_INFO, "reading from buffer");
	/* get data from the rx buffer if not empty */
	n = MIN(n, len);
	memcpy(buf, &dev->rx_buf[dev->rx_pos], n); 

	dev->rx_pos += n;
	return n;

#if 0
	{
		int rem;
		uint8_t * cp = (uint8_t *)buf;

		rem = n;

		while (rem > 4) {
			DCC_LOG4(LOG_INFO, "%02x %02x %02x %02x", 
					 cp[0], cp[1], cp[2], cp[3]);
			rem -= 4;
			cp += 4;
		}

		switch (rem) {
		case 3:
			DCC_LOG3(LOG_INFO, "%02x %02x %02x", cp[0], cp[1], cp[2]);
			break;
		case 2:
			DCC_LOG2(LOG_INFO, "%02x %02x", cp[0], cp[1]);
			break;
		case 1:
			if ((*cp) >= ' ') { 
				DCC_LOG1(LOG_INFO, "'%c'", cp[0]);
			} else {
				DCC_LOG1(LOG_INFO, "%02x", cp[0]);
			}
			break;
		}
	}
#endif

}

int usb_cdc_flush(usb_cdc_class_t * cl)
{
	struct usb_cdc_acm_dev * dev = (struct usb_cdc_acm_dev *)cl;
	(void)dev;

	DCC_LOG(LOG_ERROR, "not implemented!");

	return 0;
}

int usb_cdc_release(usb_cdc_class_t * cl)
{
	struct usb_cdc_acm_dev * dev = (struct usb_cdc_acm_dev *)cl;
	(void)dev;

	DCC_LOG(LOG_ERROR, "not implemented!");

	return 0;
}

#define ACM_PARITY_NONE 0
#define ACM_PARITY_ODD 1
#define ACM_PARITY_EVEN 2

int usb_cdc_state_get(usb_cdc_class_t * cl, usb_cdc_state_t * state)
{
	struct usb_cdc_acm_dev * dev = (struct usb_cdc_acm_dev *)cl;

	state->cfg.baudrate = dev->acm.lc.dwDTERate;
	state->cfg.databits = dev->acm.lc.bDataBits;
	switch (dev->acm.lc.bParityType) {
	case ACM_PARITY_ODD:
		state->cfg.parity = SERIAL_PARITY_ODD;
		break;
	case ACM_PARITY_EVEN:
		state->cfg.parity = SERIAL_PARITY_EVEN;
		break;
	case ACM_PARITY_NONE:
	default:
		state->cfg.parity = SERIAL_PARITY_NONE;
		break;
	}

	state->cfg.stopbits = dev->acm.lc.bCharFormat;

	state->ctrl.dtr = (dev->acm.control & CDC_DTE_PRESENT);
	state->ctrl.rts = (dev->acm.control & CDC_ACTIVATE_CARRIER);

	state->stat.dsr = (dev->acm.status & CDC_SERIAL_STATE_TX_CARRIER);
	state->stat.ri = (dev->acm.status & CDC_SERIAL_STATE_RING);
	state->stat.dcd = (dev->acm.status & CDC_SERIAL_STATE_RX_CARRIER);
	state->stat.cts = 0;
	state->stat.brk = (dev->acm.status & CDC_SERIAL_STATE_BREAK);

	state->err.ovr = (dev->acm.status & CDC_SERIAL_STATE_OVERRUN);
	state->err.par = (dev->acm.status & CDC_SERIAL_STATE_PARITY);
	state->err.frm = (dev->acm.status & CDC_SERIAL_STATE_FRAMING);

	return 0;
}

int usb_cdc_ctl_wait(usb_cdc_class_t * cl, unsigned int msec)
{
	struct usb_cdc_acm_dev * dev = (struct usb_cdc_acm_dev *)cl;
	(void)dev;

	thinkos_flag_wait(CTL_FLAG);
	thinkos_flag_clr(CTL_FLAG);
	DCC_LOG(LOG_INFO, "CTL wakeup...");

	return 0;
}

int usb_cdc_dte_wait(usb_cdc_class_t * cl)
{
	struct usb_cdc_acm_dev * dev = (struct usb_cdc_acm_dev *)cl;

	while ((dev->acm.control & CDC_DTE_PRESENT) == 0) {
		DCC_LOG(LOG_INFO, "wait");
		thinkos_flag_wait(CTL_FLAG);
		thinkos_flag_clr(CTL_FLAG);
		DCC_LOG(LOG_INFO, "CTL wakeup...");
	}

	return 0;
}

int usb_cdc_acm_lc_wait(usb_cdc_class_t * cl)
{
	struct usb_cdc_acm_dev * dev = (struct usb_cdc_acm_dev *)cl;

	while ((dev->acm.opt & ACM_LC_SET) == 0) {
		DCC_LOG(LOG_INFO, "CTL wait");
		thinkos_flag_wait(CTL_FLAG);
		thinkos_flag_clr(CTL_FLAG);
		DCC_LOG(LOG_INFO, "CTL wakeup...");
	}

	return 0;
}

int usb_cdc_status_set(usb_cdc_class_t * cl, struct serial_status * stat)
{
	struct usb_cdc_acm_dev * dev = (struct usb_cdc_acm_dev *)cl;
	struct cdc_notification * pkt;
	uint32_t buf[4];
	uint32_t status;

	status = stat->dsr ? CDC_SERIAL_STATE_TX_CARRIER : 0;
	status |= stat->ri ? CDC_SERIAL_STATE_RING : 0;
	status |= stat->dcd ? CDC_SERIAL_STATE_RX_CARRIER : 0;
	status |= stat->cts ? 0 : 0;
	status |= stat->brk ? CDC_SERIAL_STATE_BREAK : 0;

	if (dev->acm.status != status) {
		int ret;

		DCC_LOG(LOG_INFO, "status update");

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
		pkt->bData[0] = status;
		pkt->bData[1] = 0;

		thinkos_flag_clr(CTL_FLAG);

		ret = usb_dev_ep_tx_start(dev->usb, dev->int_ep, pkt, 
							sizeof(struct cdc_notification));
		if (ret < 0) {
			DCC_LOG(LOG_WARNING, "usb_dev_ep_tx_start() failed!");
			return ret;
		}

		DCC_LOG1(LOG_INFO, "ret=%d wait", ret);
		thinkos_flag_wait(CTL_FLAG);
		thinkos_flag_clr(CTL_FLAG);
		DCC_LOG(LOG_INFO, "CTL wakeup...");

		/* FIXME: handle failures .... */
		/* set the status */
		dev->acm.status = status;
	}

	return 0;
}


struct usb_cdc_acm_dev usb_cdc_rt;

const usb_class_events_t usb_cdc_ev = {
	.on_reset = usb_cdc_on_reset,
	.on_suspend = usb_cdc_on_suspend,
	.on_error = usb_cdc_on_error
};

usb_cdc_class_t * usb_cdc_init(const usb_dev_t * usb, uint64_t sn)
{
	struct usb_cdc_acm_dev * dev = &usb_cdc_rt;
	usb_class_t * cl =  (usb_class_t *)dev;

	/* initialize USB device */
	dev->usb = (usb_dev_t *)usb;

#ifndef CDC_RX_SEM_NO
	dev->rx_sem = thinkos_sem_alloc(0); 
#endif
#ifndef CDC_TX_FLAG_NO
	dev->tx_flag = thinkos_flag_alloc(); 
#endif
#ifndef CDC_CTL_FLAG_NO
	dev->ctl_flag = thinkos_flag_alloc(); 
#endif

	dev->rx_cnt = 0;
	dev->rx_pos = 0;

	DCC_LOG3(LOG_INFO, "tx_flag=%d rx_sem=%d ctl_flag=%d", 
			 TX_FLAG, RX_SEM, CTL_FLAG);

	thinkos_flag_clr(TX_FLAG); 
	thinkos_flag_clr(CTL_FLAG); 
	
	DCC_LOG2(LOG_INFO, "ESN: %08x %08x", (uint32_t)sn, (uint32_t)(sn >> 32LL));
	usb_cdc_sn_set(sn);

	usb_dev_init(dev->usb, cl, &usb_cdc_ev);

	return (usb_cdc_class_t *)dev;
}

