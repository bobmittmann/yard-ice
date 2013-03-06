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
 * @file sys/stm32f.h
 * @brief YARD-ICE libstm32f
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 


#ifndef __SYS_USB_CDC_H__
#define __SYS_USB_CDC_H__

#include <stdint.h>
#include <sys/cdc.h>
#include <sys/serial.h>

#define USB_CDC_EINVAL 1
#define USB_CDC_EINTR 2
#define USB_CDC_ETIMEDOUT 3

struct usb_cdc_state {
	uint32_t rx_chars;
	uint32_t tx_chars;
	struct serial_config cfg;
    struct serial_control ctrl;
    struct serial_status stat;
    struct serial_error err;
};

struct usb_cdc_dev;

#ifdef __cplusplus
extern "C" {
#endif


void usb_connect(struct usb_cdc_dev * dev);

void usb_disconnect(struct usb_cdc_dev * dev);

void usb_enumaration_wait(struct usb_cdc_dev * dev);

void usb_reset_wait(struct usb_cdc_dev * dev);

void usb_ctrl_event_wait(struct usb_cdc_dev * dev);

void usb_device_init(struct usb_cdc_dev * dev);

struct usb_cdc_dev * usb_cdc_init(void);

int usb_cdc_write(struct usb_cdc_dev * dev, 
				  const void * buf, unsigned int len);

int usb_cdc_read(struct usb_cdc_dev * dev, void * buf, 
				 unsigned int len, unsigned int msec);

int usb_cdc_flush(struct usb_cdc_dev * dev, 
				  const void * buf, unsigned int len);

struct file * usb_cdc_fopen(void);

int usb_cdc_state_get(struct usb_cdc_dev * dev, struct usb_cdc_state * state);

int usb_cdc_ctrl_event_wait(struct usb_cdc_dev * dev, unsigned int msec);

#ifdef __cplusplus
}
#endif

#endif /* __SYS_USB_CDC_H__ */

