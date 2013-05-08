/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
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
 * @file sys/usb_dev.h
 * @brief USB device
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 


#ifndef __SYS_USB_DEV_H__
#define __SYS_USB_DEV_H__

#include <stdint.h>
#include <stdbool.h>
#include <sys/usb.h>

/* USB class callback functions */

/* opaque USB endpoint interface */
struct usb_ep_if;

/* opaque USB class interface */
struct usb_class_if;

/* generic usb_class type */
typedef struct usb_class_if usb_class_t;

/* generic usb_endpoint type */
typedef struct usb_ep_if usb_ep_t;

typedef int (* usb_class_on_ep_setup_t)(usb_class_t * cl,
		struct usb_request * req, void ** tx_ptr);

typedef int (* usb_class_on_ep_in_t)(usb_class_t * cl,
		unsigned int ep_id);

/* This callback is invoked after a successful OUT transaction */
typedef int (* usb_class_on_ep_out_t)(usb_class_t * cl,
		unsigned int ep_id, unsigned int len);

typedef int (* usb_class_on_ep_ev_t)(usb_class_t * cl,
		unsigned int ep_id);

typedef int (* usb_class_on_reset_t)(usb_class_t * cl);

typedef int (* usb_class_on_suspend_t)(usb_class_t * cl);

typedef int (* usb_class_on_error_t)(usb_class_t * cl, int code);


struct usb_class_events {
	usb_class_on_reset_t on_reset;
	usb_class_on_suspend_t on_suspend;
	usb_class_on_error_t on_error;
};

typedef struct usb_class_events usb_class_events_t;

struct usb_dev_ep_info {
	/* Address of the endpoint on the USB device */
	uint8_t addr;
	/* Endpoint attributes */
	uint8_t attr;
	/* Maximum packet size this endpoint is capable of
	   sending or receiving */
	uint16_t mxpktsz;
	union {
		usb_class_on_ep_ev_t on_ev;
		usb_class_on_ep_in_t on_in;
		usb_class_on_ep_out_t on_out;
		usb_class_on_ep_setup_t on_setup;
	};
};

typedef struct usb_dev_ep_info usb_dev_ep_info_t;

/*
 * Receiving data
 *
 */

typedef int (* usb_dev_init_t)(void *, usb_class_t *,
		const struct usb_class_events * ev);

typedef int (* usb_dev_ep_tx_start_t)(void *, int, const void *, int);

typedef int (* usb_dev_ep_stall_t)(void *, int);

typedef int (* usb_dev_ep_zlp_send_t)(void *, int);

typedef int (* usb_dev_ep_disable_t)(void *, int);

typedef void (* usb_dev_ep_nak_t)(void *, int, bool);

typedef int (* usb_dev_ep_init_t)(void *, const usb_dev_ep_info_t *,
								  void *, int);

typedef int (* usb_dev_ep_pkt_recv_t)(void *, int, const void *, int);

struct usb_dev_ops {
	/* Initialize the USB device */
	usb_dev_init_t dev_init;
	/* Start sending data on an endpoint  */
	usb_dev_ep_tx_start_t ep_tx_start;
	/* Stall the endpoint */
	usb_dev_ep_stall_t ep_stall;
	/* Send a zero-length package */
	usb_dev_ep_zlp_send_t ep_zlp_send;
	/* EP set/clear nak */
	usb_dev_ep_nak_t ep_nak;
	/* EP Disable */
	usb_dev_ep_disable_t ep_disable;
	/* EP Initialize */
	usb_dev_ep_init_t ep_init;
	/* EP packet receive - get data from end-point receiving FIFO */
	usb_dev_ep_pkt_recv_t ep_pkt_recv;
};

/* USB device endpoint object */
struct usb_dev_ep {
	void * priv;
	const struct usb_dev_ep_ops * op;
};

typedef struct usb_dev_ep usb_dev_ep_t;

/* USB device object */
struct usb_dev {
	void * priv;
	const struct usb_dev_ops * op;
};

typedef struct usb_dev usb_dev_t;

#ifdef __cplusplus
extern "C" {
#endif

extern inline int usb_dev_init(const usb_dev_t * dev, usb_class_t * cl,
		const usb_class_events_t * ev) {
	return dev->op->dev_init(dev->priv, cl, ev);
}

extern inline int usb_dev_ep_tx_start(const usb_dev_t * dev, int ep_id,
		const void * buf, int len) {
	return dev->op->ep_tx_start(dev->priv, ep_id, buf, len);
}

extern inline int usb_dev_ep_stall(const usb_dev_t * dev, int ep_id) {
	return dev->op->ep_stall(dev->priv, ep_id);
}

extern inline void usb_dev_ep_nak(const usb_dev_t * dev, int ep_id, bool flag) {
	return dev->op->ep_nak(dev->priv, ep_id, flag);
}

extern inline int usb_dev_ep_zlp_send(const usb_dev_t * dev, int ep_id) {
	return dev->op->ep_zlp_send(dev->priv, ep_id);
}

extern inline int usb_dev_ep_disable(const usb_dev_t * dev, int ep_id) {
	return dev->op->ep_disable(dev->priv, ep_id);
}

extern inline int usb_dev_ep_init(const usb_dev_t * dev, 
								  const usb_dev_ep_info_t * info,
								  void * xfr_buf, unsigned int buf_len) {
	return dev->op->ep_init(dev->priv, info, xfr_buf, buf_len);
}

extern inline int usb_dev_ep_pkt_recv(const usb_dev_t * dev, int ep_id,
		void * buf, int len) {
	return dev->op->ep_pkt_recv(dev->priv, ep_id, buf, len);
}

#ifdef __cplusplus
}
#endif

#endif /* __SYS_USB_DEV_H__ */

