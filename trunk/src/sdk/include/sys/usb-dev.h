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

struct usb_ep_info {
	/* Address of the endpoint on the USB device */
	uint8_t addr;
	/* Endpoint type */
	uint8_t type;
	/* Maximum packet size this endpoint is capable of
	   sending or receiving */
	uint16_t mxpktsz;
};

typedef int (* usb_dev_init_t)(void *, struct usb_ep_info *, unsigned int);

struct usb_dev_ops {
	/* Initialize the USB device */
	usb_dev_init_t usb_dev_init;
};

struct usb_dev {
	void * priv;
	const struct usb_dev_ops * op;
};

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __SYS_USB_DEV_H__ */

