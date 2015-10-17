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

#include "sdu.h"
#include "damp.h"
#include "trace.h"
#include "profclk.h"
#include "capture.h"

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

/* -------------------------------------------------------------------------
   Virtual COM 
   ------------------------------------------------------------------------- */

enum {
	VCOM_MODE_STANDBY = 0,
	VCOM_MODE_RAW_TRACE = 1,
	VCOM_MODE_DAMP_TRACE = 2,
	VCOM_MODE_SDU_TRACE = 3
};

struct vcom {
	volatile int mode;
	usb_cdc_class_t * cdc;
};

/* -------------------------------------------------------------------------
   Firmware update 
   ------------------------------------------------------------------------- */

extern const uint8_t usb_xflash_pic[];
extern const unsigned int sizeof_usb_xflash_pic;
extern uint32_t __data_start[]; 

void __attribute__((noreturn)) usb_xflash(uint32_t offs, uint32_t len)
{
	uint32_t * xflash_code = __data_start;
	int (* xflash_ram)(uint32_t, uint32_t) = ((void *)xflash_code) + 1;

	DCC_LOG3(LOG_TRACE, "sp=%08x offs=%08x len=%d", cm3_sp_get(), offs, len);

	cm3_cpsid_i();

	memcpy(xflash_code, usb_xflash_pic, sizeof_usb_xflash_pic);

	xflash_ram(offs, len);

	cm3_sysrst();
}

/* -------------------------------------------------------------------------
   Debug and trace 
   ------------------------------------------------------------------------- */

void show_menu(void)
{
	usb_printf("\r\n - Service options:\r\n");
	usb_printf(" [0..9] Idle time set (bits)\r\n");
	usb_printf("    [-]  9600 bps\r\n");
	usb_printf("    [=] 19200 bps\r\n");
	usb_printf("    [+] 38400 bps\r\n");
	usb_printf("    [F] firmware update\r\n");
	usb_printf("  [A/a] absolute/relative time\r\n");
	usb_printf("  [D/d] enable/disable DAMP trace\r\n");
	usb_printf("  [P/p] enable/disable packets\r\n");
	usb_printf("  [R/r] enable/disable raw data trace\r\n");
	usb_printf("  [S/s] enable/disable SDU trace\r\n");
	usb_printf("  [U/u] enable/disable SDU supervisory\r\n");
	usb_printf("[DAMP-SNIFFER %d.%d]: ", FW_VERSION_MAJOR, FW_VERSION_MINOR);
};

#define VCOM_BUF_SIZE 32

uint32_t proto_buf[128];

void __attribute__((noreturn)) usb_recv_task(struct vcom * vcom)
{
	usb_cdc_class_t * cdc = vcom->cdc;
	uint8_t buf[VCOM_BUF_SIZE];
	int len;
	int i;
	int c;


	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());
	DCC_LOG2(LOG_TRACE, "vcom->%p, cdc->%p", vcom, cdc);

	for (;;) {
		len = usb_cdc_read(cdc, buf, VCOM_BUF_SIZE, 1000);

		for (i = 0; i < len; ++i) {
			c = buf[i];
			(void)c;

			switch (c) {

			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				usb_printf(" - Idle time ser to %d bits...\r\n", c - '0');
				capture_idletime_set(c - '0');
				break;
	
			case '-':
				capture_baudrate_set(9600);
				usb_printf(" - 9600 bps ...\r\n");
				break;
				
			case '=':
				capture_baudrate_set(19200);
				usb_printf(" - 19200 bps ...\r\n");
				break;

			case '+':
				capture_baudrate_set(38400);
				usb_printf(" - 38400 bps ...\r\n");
				break;

			case 'S':
				usb_printf(" - SDU trace ...\r\n");
				capture_stop();
				vcom->mode = VCOM_MODE_SDU_TRACE;
				sdu_trace_init(proto_buf);
				capture_start();
				break;

			case 'D':
				usb_printf(" - DAMP data trace ...\r\n");
				capture_stop();
				vcom->mode = VCOM_MODE_DAMP_TRACE;
				damp_trace_init(proto_buf);
				capture_start();
				break;

			case 'R':
				usb_printf(" - Raw data trace ...\r\n");
				vcom->mode = VCOM_MODE_RAW_TRACE;
				capture_start();
				break;

			case 'r':
			case 'd':
			case 's':
				capture_stop();
				vcom->mode = VCOM_MODE_STANDBY;
				usb_printf(" - Standby ...\r\n");
				break;

			case 'F':
				if (vcom->mode == VCOM_MODE_STANDBY) {
					usb_printf(" - Firmware update...\r\n");
					usb_xflash(0, 32 * 1024);
				}
				break;

			case 'U':
				sdu_trace_show_supv(true);
				usb_printf(" - Show supervisory...\r\n");
				break;

			case 'u':
				sdu_trace_show_supv(false);
				usb_printf(" - Don't show supervisory...\r\n");
				break;

			case 'P':
				sdu_trace_show_pkt(true);
				usb_printf(" - Show packets...\r\n");
				break;

			case 'p':
				sdu_trace_show_pkt(false);
				usb_printf(" - Don't show packets...\r\n");
				break;

			case 'A':
				trace_time_abs(true);
				usb_printf(" - Absolute timestamps...\r\n");
				break;

			case 'a':
				trace_time_abs(false);
				usb_printf(" - Relative timestamps...\r\n");
				break;

			default:
				show_menu();
			}
		}
	}
}

void __attribute__((noreturn)) capture_task(struct vcom * vcom)
{
	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());

	for (;;) {
		struct packet * pkt;

		pkt = capture_pkt_recv();
		if (pkt == NULL) {
			continue;
		}

		led_flash(LED_AMBER, 50);

		if (vcom->mode == VCOM_MODE_RAW_TRACE)
			trace_raw_pkt(pkt);
		else if (vcom->mode == VCOM_MODE_DAMP_TRACE)
			trace_damp_pkt(pkt);
		else if (vcom->mode == VCOM_MODE_SDU_TRACE)
			trace_sdu_pkt(pkt);
	}
}

uint32_t __attribute__((aligned(8))) led_stack[32];
uint32_t __attribute__((aligned(8))) capture_stack[192];

extern struct usb_cdc_class * usb_cdc;

int main(int argc, char ** argv)
{
	struct usb_cdc_class * cdc;
	struct vcom vcom;
	int i;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	/* calibrate usecond delay loop */
	cm3_udelay_calibrate();

	DCC_LOG(LOG_TRACE, "1. io_init()");
	io_init();

	DCC_LOG(LOG_TRACE, "2. thinkos_init()");
	thinkos_init(THINKOS_OPT_PRIORITY(3) | THINKOS_OPT_ID(3));

	profclk_init();

	leds_init();

	capture_init();

	cdc = usb_cdc_init(&stm32f_usb_fs_dev, cdc_acm_str, 
					   cdc_acm_strcnt);
	usb_cdc = cdc;

	vcom.cdc = cdc;
	vcom.mode = VCOM_MODE_STANDBY;

	thinkos_thread_create((void *)led_task, (void *)NULL,
						  led_stack, sizeof(led_stack) |
						  THINKOS_OPT_PRIORITY(1) | THINKOS_OPT_ID(1));

	thinkos_thread_create((void *)capture_task, 
						  (void *)&vcom,
						  capture_stack, sizeof(capture_stack) |
						  THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	usb_vbus(true);

	for (i = 0; i < 5; ++i) {
		led_on(LED_AMBER);
		thinkos_sleep(100);
		led_off(LED_AMBER);
		led_on(LED_RED);
		thinkos_sleep(100);
		led_off(LED_RED);
	}

	for (;;) {
		usb_recv_task(&vcom);
	}
}

