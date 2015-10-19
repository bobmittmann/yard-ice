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
 * @file usb-serial.c
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include "board.h"

#include <string.h>
#include <stdint.h>
#include <sys/serial.h>
#include <sys/param.h>
#include <sys/usb-cdc.h>

#include <sys/dcclog.h>

struct usb_cdc_class * usb_cdc;

int usb_puts(const char * s)
{
	return  usb_cdc_write(usb_cdc, s, strlen(s));
}


#define FW_VERSION_MAJOR 0
#define FW_VERSION_MINOR 1

/* -------------------------------------------------------------------------
   USB-CDC ACM
   ------------------------------------------------------------------------- */

#define LANG_STR_SZ              4
static const uint8_t lang_str[LANG_STR_SZ] = {
	/* LangID = 0x0409: U.S. English */
	LANG_STR_SZ, USB_DESCRIPTOR_STRING, 0x09, 0x04
};

#define VENDOR_STR_SZ            26
static const uint8_t vendor_str[VENDOR_STR_SZ] = {
	VENDOR_STR_SZ, USB_DESCRIPTOR_STRING,
	/* Manufacturer: "Mircom Group" */
	'M', 0, 'i', 0, 'r', 0, 'c', 0, 'o', 0, 'm', 0, 
	' ', 0, 'G', 0, 'r', 0, 'o', 0, 'u', 0, 'p', 0
};


#define PRODUCT_STR_SZ           52
static const uint8_t product_str[PRODUCT_STR_SZ] = {
	PRODUCT_STR_SZ, USB_DESCRIPTOR_STRING,
	/* Product name: "Mircom ClassD Sniffer" */
	'M', 0, 'i', 0, 'r', 0, 'c', 0, 'o', 0, 'm', 0, ' ', 0, 'C', 0, 
	'l', 0, 'a', 0, 's', 0, 's', 0, 'D', 0, ' ', 0, 'S', 0, 'n', 0, 
	'i', 0, 'f', 0, 'f', 0, 'e', 0, 'r', 0, ' ', 0, 
	'0' + FW_VERSION_MAJOR, 0, '.', 0, 
	'0' + FW_VERSION_MINOR, 0, 
};


#define SERIAL_STR_SZ            26
static const uint8_t serial_str[SERIAL_STR_SZ] = {
	SERIAL_STR_SZ, USB_DESCRIPTOR_STRING,
	/* Serial number: "553273343836" */
	'5', 0, '5', 0, '3', 0, '2', 0, '7', 0, '3', 0, 
	'3', 0, '4', 0, '3', 0, '8', 0, '3', 0, '6', 0
};


#define INTERFACE_STR_SZ         16
static const uint8_t interface_str[INTERFACE_STR_SZ] = {
	INTERFACE_STR_SZ, USB_DESCRIPTOR_STRING,
	/* Interface 0: "ST SINF" */
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

/* -------------------------------------------------------------------------
   Firmware update 
   ------------------------------------------------------------------------- */

extern const uint8_t usb_xflash_pic[];
extern const unsigned int sizeof_usb_xflash_pic;
extern uint32_t __data_start[]; 

struct magic {
	struct {
		uint16_t pos;
		uint16_t cnt;
	} hdr;
	struct {
	    uint32_t mask;
		uint32_t comp;
	} rec[];
};

const struct magic firmware_magic = {
	.hdr = {
		.pos = 0,
		.cnt = 10
	},
	.rec = {
		{  0xffffffff, 0x20002800 },
		{  0xffff0000, 0x08000000 },
		{  0xffff0000, 0x08000000 },
		{  0xffff0000, 0x08000000 },

		{  0xffff0000, 0x08000000 },
		{  0xffff0000, 0x08000000 },
		{  0xffff0000, 0x08000000 },
		{  0xffff0000, 0x08000000 },

		{  0xffffffff, 0x00000000 },
		{  0xffff0000, 0x08000000 }
	}
};

extern int xflash(uint32_t blk_offs, unsigned int blk_size, 
				  const struct magic * magic);

void show_menu(void)
{
	usb_puts("\r\n - Options:\r\n");
	usb_puts("    [F] firmware update\r\n");
	usb_puts("[XFLASH]: ");
};

void isr(void) 
{
	for(;;);
}

void __attribute__((noreturn)) hard_fault_isr(void)
{
	for(;;);
}

__attribute__((aligned(128))) void * const cortex_m_vectors[] = {
	0,
	0,
	/* cm3_nmi_isr */
	isr,
	/* cm3_hard_fault_isr */
	hard_fault_isr,
	/* cm3_mem_manage_isr */
	isr,
	/* cm3_bus_fault_isr */
	isr,
	/* cm3_usage_fault_isr */
	isr,
	/* cm3_except7_isr */
	isr,
	/* ice_comm_blk */
	isr,
	/* cm3_except9_isr */
	isr,
	/* cm3_except10_isr */
	isr,
	/* cm3_svc_isr */
	isr,
	/* cm3_debug_mon_isr */
	isr,
	/* cm3_except13_isr */
	isr,
	/* cm3_pendsv_isr */
	isr,
	/* cm3_systick_isr */
	isr
};



int __attribute__((noreturn)) main(int argc, char ** argv)
{
	struct usb_cdc_class * cdc;
	uint8_t c;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	/* calibrate usecond delay loop */
	DCC_LOG(LOG_TRACE, "cm3_udelay_calibrate()");
	cm3_udelay_calibrate();

	DCC_LOG(LOG_TRACE, "io_init()");
	io_init();

	DCC_LOG(LOG_TRACE, "thinkos_init()");
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	cdc = usb_cdc_init(&stm32f_usb_fs_dev, cdc_acm_str, cdc_acm_strcnt);
	usb_cdc = cdc;

	usb_vbus(true);

	for (;;) {
		if (usb_cdc_read(cdc, &c, 1, 1000) != 1) {
			thinkos_sleep(100);
			continue;
		}

		switch (c) {

		case 'F':
			cm3_cpsid_i();
			/* Remap the VECTOR table to SRAM 0x20000000  */
			CM3_SCB->vtor = (uint32_t)cortex_m_vectors; /* Vector Table Offset */
			xflash(0, 32 * 1024, &firmware_magic);
			break;

		default:
			show_menu();
		}
	}
}

