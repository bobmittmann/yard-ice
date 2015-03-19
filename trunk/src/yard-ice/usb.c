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
 * @file yard-ice.c
 * @brief YARD-ICE application main
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 


#include <sys/stm32f.h>
#include <sys/param.h>
#include <sys/file.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <sys/usb-cdc.h>
#include <sys/tty.h>
#include <sys/shell.h>
#include <sys/os.h>
#include <trace.h>

#include <sys/dcclog.h>

#include "command.h"
#include "version.h"

#define LANG_STR_SZ              4
static const uint8_t lang_str[LANG_STR_SZ] = {
	/* LangID = 0x0409: U.S. English */
	LANG_STR_SZ, USB_DESCRIPTOR_STRING, 0x09, 0x04
};

#define VENDOR_STR_SZ            26
static const uint8_t vendor_str[VENDOR_STR_SZ] = {
	VENDOR_STR_SZ, USB_DESCRIPTOR_STRING,
	/* Manufacturer: "Bob Mittmann" */
	'B', 0, 'o', 0, 'b', 0, ' ', 0, 'M', 0, 'i', 0, 
	't', 0, 't', 0, 'm', 0, 'a', 0, 'n', 0, 'n', 0
};


#define PRODUCT_STR_SZ           28
static const uint8_t product_str[PRODUCT_STR_SZ] = {
	PRODUCT_STR_SZ, USB_DESCRIPTOR_STRING,
	/* Product name: "YARD-ICE x.x" */
	'Y', 0, 'A', 0, 'R', 0, 'D', 0, '-', 0, 'I', 0, 'C', 0, 'E', 0, 
	' ', 0, '0' + VERSION_MAJOR , 0, '.', 0, 
	'0' + (VERSION_MINOR / 10), 0, 
	'0' + (VERSION_MINOR % 10), 0, 
};


#define SERIAL_STR_SZ            26
static const uint8_t serial_str[SERIAL_STR_SZ] = {
	SERIAL_STR_SZ, USB_DESCRIPTOR_STRING,
	/* Serial number: "594152444943" */
	'5', 0, '9', 0, '4', 0, '1', 0, '5', 0, '2', 0, 
	'4', 0, '4', 0, '4', 0, '9', 0, '4', 0, '3', 0
};


#define INTERFACE_STR_SZ         16
static const uint8_t interface_str[INTERFACE_STR_SZ] = {
	INTERFACE_STR_SZ, USB_DESCRIPTOR_STRING,
	/* Interface 0: "ST VCOM" */
	'S', 0, 'T', 0, ' ', 0, 'V', 0, 'C', 0, 'O', 0, 'M', 0
};

const uint8_t * const cdc_acm_str[] = {
	lang_str,
	vendor_str,
	product_str,
	serial_str,
	interface_str
};

const uint8_t cdc_acm_strcnt = sizeof(cdc_acm_str) / sizeof(uint8_t *);

int usb_task(void * arg)
{
	uint64_t  esn = *((uint64_t *)STM32F_UID);
	usb_cdc_class_t * cdc;
	struct tty_dev * tty;
	FILE * f_tty;
	FILE * f_raw;

	DCC_LOG(LOG_TRACE, "usb_cdc_init()");
	usb_cdc_sn_set(esn);
	cdc = usb_cdc_init(&stm32f_otg_fs_dev, cdc_acm_str, 
					   cdc_acm_strcnt);

	f_raw = usb_cdc_fopen(cdc);

	tty = tty_attach(f_raw);
	f_tty = tty_fopen(tty);

	for (;;) {
		shell(f_tty, yard_ice_get_prompt, yard_ice_greeting, yard_ice_cmd_tab);
	}
}

uint32_t usb_shell_stack[1360] __attribute__((section (".sram1")));

const struct thinkos_thread_inf usb_shell_inf = {
	.stack_ptr = usb_shell_stack, 
	.stack_size = sizeof(usb_shell_stack),
	.priority = 0,
	.thread_id = 16,
	.paused = false,
	.tag = "USB_SH"
};

int usb_shell(void)
{
	int th;

	th = thinkos_thread_create_inf((void *)usb_task, NULL, 
								   &usb_shell_inf);

	tracef("USB CDC-ACM shell thread=%d", th);

	return th;
}

