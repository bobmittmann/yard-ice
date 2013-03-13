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
#include <sys/usb.h>

/* USB class callback functions */

/* opaque USB class endpoint interface */
struct usb_class_ep_if;

/* opaque USB endpoint interface */
struct usb_ep_if;

/* opaque USB class interface */
struct usb_class_if;

/* generic usb_class type */
typedef struct usb_class_if usb_class_t;
/* generic usb_endpoint type */
typedef struct usb_ep_if usb_ep_t;

/* This callback is invoked after a successful OUT transaction */
typedef int (* usb_class_on_ep_rx_t)(usb_class_t * cl,
		uint32_t * buf, unsigned int len);

typedef int (* usb_class_on_setup_t)(usb_class_t * cl,
		uint32_t * buf, unsigned int len);

typedef int (* usb_class_on_enum_t)(usb_class_t * cl, unsigned int addr);


struct usb_ep_info {
	/* Address of the endpoint on the USB device */
	uint8_t addr;
	/* Endpoint type */
	uint8_t type;
	/* Maximum packet size this endpoint is capable of
	   sending or receiving */
	uint16_t mxpktsz;
	union {
		usb_class_on_ep_rx_t on_rx;
	};
};

typedef struct usb_ep_info usb_ep_info_t;

/*
 * Receiving data
 *
 * Class should call: dev->ep_rx_setup(ep, buf);
 *
 *  Buffer must be at least MASPKTSZ len.
 *
 *  After a successful OUT transaction, the device would
 *  call: ep->on_rx(cl, buf, len) form the ISR.
 *
 */



typedef int (* usb_dev_ep_rx_setup_t)(void *, int, uint32_t *, unsigned int);

typedef int (* usb_dev_init_t)(void *, usb_class_t *,
		const usb_ep_info_t *, unsigned int);

typedef int (* usb_dev_connect_t)(void *);

typedef int (* usb_dev_disconnect_t)(void *);

typedef int (* usb_dev_ep_tx_start_t)(void *, int, const void *, int);

typedef int (* usb_dev_ep_stall_t)(void *, int);

typedef int (* usb_dev_addr_set_t)(void *, unsigned int);

typedef int (* usb_dev_ep_zlp_send_t)(void *, int);

typedef int (* usb_dev_ep_enable_t)(void *, int);

typedef int (* usb_dev_ep_disable_t)(void *, int);

struct usb_dev_ops {
	/* Initialize the USB device */
	usb_dev_init_t dev_init;
	usb_dev_connect_t connect;
	usb_dev_disconnect_t disconnect;
	/* Set the device address */
	usb_dev_addr_set_t addr_set;

	/* Prepare an endpoint to receive data */
	usb_dev_ep_rx_setup_t ep_rx_setup;
	/* Start sending data on an endpoint  */
	usb_dev_ep_tx_start_t ep_tx_start;
	/* Stall the endpoint */
	usb_dev_ep_stall_t ep_stall;
	usb_dev_ep_zlp_send_t ep_zlp_send;
	usb_dev_ep_enable_t ep_enable;
	usb_dev_ep_disable_t ep_disable;
};

struct usb_ep;

struct usb_dev {
	void * priv;
	const struct usb_dev_ops * op;
	struct usb_ep * ep;
};

typedef struct usb_dev usb_dev_t;

#ifdef __cplusplus
extern "C" {
#endif

extern inline int usb_dev_init(const usb_dev_t * dev, usb_class_t * cl,
		const usb_ep_info_t * epi, unsigned int cnt) {
	return dev->op->dev_init(dev->priv, cl, epi, cnt);
}

extern inline int usb_dev_connect(const usb_dev_t * dev) {
	return dev->op->connect(dev->priv);
}

extern inline int usb_dev_disconnect(const usb_dev_t * dev) {
	return dev->op->disconnect(dev->priv);
}

extern inline int usb_dev_ep_tx_start(const usb_dev_t * dev, int ep_id,
		const void * buf, int len) {
	return dev->op->ep_tx_start(dev->priv, ep_id, buf, len);
}

extern inline int usb_dev_ep_stall(const usb_dev_t * dev, int ep_id) {
	return dev->op->ep_stall(dev->priv, ep_id);
}

extern inline int usb_dev_addr_set(const usb_dev_t * dev, unsigned int addr) {
	return dev->op->addr_set(dev->priv, addr);
}

extern inline int usb_dev_ep_zlp_send(const usb_dev_t * dev, int ep_id) {
	return dev->op->ep_zlp_send(dev->priv, ep_id);
}

extern inline int usb_dev_ep_enable(const usb_dev_t * dev, int ep_id) {
	return dev->op->ep_enable(dev->priv, ep_id);
}

extern inline int usb_dev_ep_disable(const usb_dev_t * dev, int ep_id) {
	return dev->op->ep_disable(dev->priv, ep_id);
}

#ifdef __cplusplus
}
#endif

#endif /* __SYS_USB_DEV_H__ */

