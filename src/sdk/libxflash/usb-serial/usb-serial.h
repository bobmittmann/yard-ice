/* 
 * Copyright(C) 2013 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the usb-serial converter.
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
 * @file usb-serial.h
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __USB_SERIAL_H__
#define __USB_SERIAL_H__

#include <sys/serial.h>
#include <sys/dcclog.h>
#include <sys/usb-cdc.h>

struct vcom {
	struct serial_dev * serial;
	usb_cdc_class_t * cdc;
	struct serial_status ser_stat;
};

enum {
	TEST_NONE = 0,
	TEST_USB,
	TEST_XFLASH,
};

extern struct serial_dev * serial1;
extern struct serial_dev * serial2;

#define VCOM_BUF_SIZE 128
#ifdef __cplusplus
extern "C" {
#endif

struct serial_dev * serial2_open(void);
struct serial_dev * serial3_open(void);

void test_main(struct vcom * vcom);
void test_sched(int test);

void usb_cdc_shell(struct usb_cdc_class * dev);

#ifdef __cplusplus
}
#endif

#endif /* __USB_SERIAL_H__ */

