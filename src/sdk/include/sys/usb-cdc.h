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
#include <stdio.h>
#include <sys/cdc.h>
#include <sys/usb-dev.h>
#include <sys/serial.h>
#include <sys/usb-cdc.h>

#define USB_CDC_EINVAL 1
#define USB_CDC_EINTR 2
#define USB_CDC_ETIMEDOUT 3

struct usb_cdc_state {
	union {
		struct {
			uint32_t reserved: 1;
			uint32_t suspended: 1;
		};
		uint32_t flags;
	};
	uint32_t rx_chars;
	uint32_t tx_chars;
	struct serial_config cfg;
    struct serial_control ctrl;
    struct serial_status stat;
    struct serial_error err;
};

typedef struct usb_cdc_state usb_cdc_state_t;

struct usb_cdc_class;

typedef struct usb_cdc_class usb_cdc_class_t;

extern const uint8_t * const cdc_acm_def_str[];
extern const uint8_t cdc_acm_def_strcnt;

extern const struct serial_dev cdc_acm_serial_dev;

#ifdef __cplusplus
extern "C" {
#endif

usb_cdc_class_t * usb_cdc_init(const usb_dev_t * usb, 
							   const uint8_t * const str[], 
							   unsigned int strcnt);

int usb_cdc_write(usb_cdc_class_t * cdc,
				  const void * buf, unsigned int len);

int usb_cdc_read(usb_cdc_class_t * cdc, void * buf,
				 unsigned int len, unsigned int msec);

void usb_connect(usb_cdc_class_t * cdc);

void usb_disconnect(usb_cdc_class_t * cdc);

void usb_enumaration_wait(usb_cdc_class_t * cdc);

void usb_reset_wait(usb_cdc_class_t * cdc);

void usb_ctrl_event_wait(usb_cdc_class_t * cdc);

void usb_device_init(usb_cdc_class_t * cdc);

int usb_cdc_flush(usb_cdc_class_t * cdc);

int usb_cdc_release(usb_cdc_class_t * cdc);

int usb_cdc_state_get(usb_cdc_class_t * cdc, usb_cdc_state_t * state);

int usb_cdc_status_set(usb_cdc_class_t * cl, struct serial_status * stat);

int usb_cdc_ctl_wait(usb_cdc_class_t * cdc, unsigned int msec);

void usb_cdc_sn_set(uint64_t sn);

void usb_cdc_product_set(const char * s);

struct file * usb_cdc_fopen(usb_cdc_class_t * cdc);

bool usb_cdc_is_usb_file(FILE * f);

int usb_cdc_acm_lc_wait(usb_cdc_class_t * cl);

#ifdef __cplusplus
}
#endif

#endif /* __SYS_USB_CDC_H__ */

