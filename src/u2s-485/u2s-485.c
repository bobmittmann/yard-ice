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
#include <sys/usb-cdc.h>

#include <sys/dcclog.h>

#include "sdu.h"

#ifndef SDU_TRACE
#ifdef DEBUG
#define SDU_TRACE 0
#else
#define SDU_TRACE 0
#endif
#endif

#ifndef RAW_TRACE
#define RAW_TRACE 0
#endif

#define FW_VERSION_MAJOR 1
#define FW_VERSION_MINOR 1

void serial_rx_disable(struct serial_dev * dev);
void serial_rx_enable(struct serial_dev * dev);

uint8_t fw_version[2] = { FW_VERSION_MAJOR, FW_VERSION_MINOR };

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
	/* Product name: "Mircom U2S485 Adapter" */
	'M', 0, 'i', 0, 'r', 0, 'c', 0, 'o', 0, 'm', 0, ' ', 0, 'U', 0, 
	'2', 0, 'S', 0, '4', 0, '8', 0, '5', 0, ' ', 0, 'A', 0, 'd', 0, 
	'a', 0, 'p', 0, 't', 0, 'e', 0, 'r', 0, ' ', 0, 
	'0' + FW_VERSION_MAJOR, 0, '.', 0, '0' + FW_VERSION_MINOR, 0, 
};


#define SERIAL_STR_SZ            26
static const uint8_t serial_str[SERIAL_STR_SZ] = {
	SERIAL_STR_SZ, USB_DESCRIPTOR_STRING,
	/* Serial number: "553273343835" */
	'5', 0, '5', 0, '3', 0, '2', 0, '7', 0, '3', 0, 
	'3', 0, '4', 0, '3', 0, '8', 0, '3', 0, '5', 0
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


struct serial_dev * serial2_open(void);

enum {
	VCOM_MODE_CONVERTER = 0,
	VCOM_MODE_SERVICE = 1,
	VCOM_MODE_INTERACTIVE = 2
};

struct vcom {
	volatile int mode;
	struct serial_dev * serial;
	usb_cdc_class_t * cdc;
	struct serial_status ser_stat;
};

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

int usb_printf(usb_cdc_class_t * cdc, const char *fmt, ... )
{
	char s[64];
	char * cp = s;
	va_list ap;

	va_start(ap, fmt);
	cp += vsnprintf(cp, 64 - 1, fmt, ap);
	va_end(ap);

	return usb_cdc_write(cdc, s, cp - s);
}

void show_menu(usb_cdc_class_t * cdc)
{
	usb_printf(cdc, "--- Option:\r\n");
	usb_printf(cdc, " [f] firmware update\r\n");
	usb_printf(cdc, " [q] quit\r\n");
};

void vcom_service_input(struct vcom * vcom, uint8_t buf[], int len)
{
	usb_cdc_class_t * cdc = vcom->cdc;
	int i;
	int c;

	if (vcom->mode == VCOM_MODE_SERVICE) {
		usb_printf(cdc, "\r\n\r\n");
		usb_printf(cdc, "--- U2S-485 Service mode -------------");
		usb_printf(cdc, "\r\n\r\n");
		vcom->mode = VCOM_MODE_INTERACTIVE;
	}

	for (i = 0; i < len; ++i) {
		c = buf[i];
		(void)c;

		switch (c) {
		case 'q':
			usb_printf(cdc, "-- Converter mode...\r\n");
			vcom->mode = VCOM_MODE_CONVERTER;
			break;
		case 'f':
			usb_printf(cdc, "-- Firmware update...\r\n");
			usb_xflash(0, 32 * 1024);
			break;
		default:
			show_menu(cdc);
		}
	}
}

#if 0

#define DBG_BUF_LEN 4096
uint8_t dbg_buf[DBG_BUF_LEN];
uint32_t dbg_cnt = 0;
uint32_t dbg_head = 0;

void dbg_buf_dump(void)
{
	DCC_LOG1(LOG_TRACE, "dbg_cnt=%d", dbg_cnt);

	if (dbg_cnt < DBG_BUF_LEN)
		RX(dbg_buf, dbg_cnt);
	else {
		int n;
		int tail = (dbg_head + 1) & (DBG_BUF_LEN - 1);
		n = DBG_BUF_LEN - tail;
		RX(&dbg_buf[tail], n);
		if ((n = tail) > 0) 
			RX(&dbg_buf[0], n);
	}
	dbg_cnt = 0;
	dbg_head = 0;
}

void dbg_write(uint8_t * buf, int len)
{
	int i;

	for (i = 0; i < len; ++i) {
		dbg_buf[dbg_head++ & (DBG_BUF_LEN - 1)] = buf[i];
		if (dbg_cnt < DBG_BUF_LEN)
			dbg_cnt++;
	}
}
#endif

#define VCOM_BUF_SIZE 64

void __attribute__((noreturn)) usb_recv_task(struct vcom * vcom)
{
	struct serial_dev * serial = vcom->serial;
	usb_cdc_class_t * cdc = vcom->cdc;
	uint8_t buf[VCOM_BUF_SIZE];
	int len;

	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());

	for (;;) {
		len = usb_cdc_read(cdc, buf, VCOM_BUF_SIZE, 1000);
		if (vcom->mode == VCOM_MODE_CONVERTER) {
			if (len > 0) {
				led_flash(LED_RED, 50);
				serial_write(serial, buf, len);
#if RAW_TRACE
				if (len == 1)
					DCC_LOG1(LOG_TRACE, "TX: %02x", buf[0]);
				else if (len == 2)
					DCC_LOG2(LOG_TRACE, "TX: %02x %02x", 
							 buf[0], buf[1]);
				else if (len == 3)
					DCC_LOG3(LOG_TRACE, "TX: %02x %02x %02x", 
							 buf[0], buf[1], buf[2]);
				else if (len == 4)
					DCC_LOG4(LOG_TRACE, "TX: %02x %02x %02x %02x", 
							 buf[0], buf[1], buf[2], buf[3]);
				else if (len == 5)
					DCC_LOG5(LOG_TRACE, "TX: %02x %02x %02x %02x %02x", 
							 buf[0], buf[1], buf[2], buf[3], buf[4]);
				else if (len == 6)
					DCC_LOG6(LOG_TRACE, "TX: %02x %02x %02x %02x %02x %02x", 
							 buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
				else if (len == 7)
					DCC_LOG7(LOG_TRACE, "TX: %02x %02x %02x %02x %02x %02x %02x ",
							 buf[0], buf[1], buf[2], buf[3], 
							 buf[4], buf[5], buf[6]);
				else
					DCC_LOG8(LOG_TRACE, "TX: %02x %02x %02x %02x %02x %02x "
							 "%02x %02x ...", buf[0], buf[1], buf[2], buf[3], 
							 buf[4], buf[5], buf[6], buf[7]);
#endif
#if SDU_TRACE
				TX(buf, len);
#endif
				//			dbg_write(buf, len);
			}
		} else {
			// forward to service input
			vcom_service_input(vcom, buf, len);
		}
	}
}

void __attribute__((noreturn)) serial_recv_task(struct vcom * vcom)
{
	struct serial_dev * serial = vcom->serial;
	struct usb_cdc_class * cdc = vcom->cdc;
	uint8_t buf[VCOM_BUF_SIZE];
	int len;

	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());

	/* wait for line configuration */
	usb_cdc_acm_lc_wait(cdc);

	/* enable serial */
	serial_enable(serial);

	for (;;) {
		len = serial_read(serial, buf, VCOM_BUF_SIZE, 1000);
		if (len > 0) {
//			dbg_write(buf, len);
			if (vcom->mode == VCOM_MODE_CONVERTER) {
				led_flash(LED_AMBER, 50);
				usb_cdc_write(cdc, buf, len);
			}
#if RAW_TRACE
			if (len == 1)
				DCC_LOG1(LOG_TRACE, "RX: %02x", buf[0]);
			else if (len == 2)
				DCC_LOG2(LOG_TRACE, "RX: %02x %02x", 
						 buf[0], buf[1]);
			else if (len == 3)
				DCC_LOG3(LOG_TRACE, "RX: %02x %02x %02x", 
						 buf[0], buf[1], buf[2]);
			else if (len == 4)
				DCC_LOG4(LOG_TRACE, "RX: %02x %02x %02x %02x", 
						 buf[0], buf[1], buf[2], buf[3]);
			else if (len == 5)
				DCC_LOG5(LOG_TRACE, "RX: %02x %02x %02x %02x %02x", 
						 buf[0], buf[1], buf[2], buf[3], buf[4]);
			else if (len == 6)
				DCC_LOG6(LOG_TRACE, "RX: %02x %02x %02x %02x %02x %02x", 
						 buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
			else if (len == 7)
				DCC_LOG7(LOG_TRACE, "RX: %02x %02x %02x %02x %02x %02x %02x ",
						 buf[0], buf[1], buf[2], buf[3], 
						 buf[4], buf[5], buf[6]);
			else
				DCC_LOG8(LOG_TRACE, "RX: %02x %02x %02x %02x %02x %02x "
						 "%02x %02x ...", buf[0], buf[1], buf[2], buf[3], 
						 buf[4], buf[5], buf[6], buf[7]);
#endif
#if SDU_TRACE
			RX(buf, len);
#endif
		}
	}
}

void __attribute__((noreturn)) serial_ctrl_task(struct vcom * vcom)
{
	struct serial_dev * serial = vcom->serial;
	struct usb_cdc_class * cdc = vcom->cdc;
	struct usb_cdc_state prev_state;
	struct usb_cdc_state state;

	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());

	memset(&prev_state, 0, sizeof(struct usb_cdc_state));
	usb_cdc_state_get(cdc, &state);

	while (1) {
		DCC_LOG1(LOG_INFO, "[%d] usb_cdc_ctl_wait() sleep!", 
				 thinkos_thread_self());
		usb_cdc_ctl_wait(cdc, 0);
		DCC_LOG1(LOG_INFO, "[%d] wakeup!", thinkos_thread_self());

		usb_cdc_state_get(cdc, &state);

		if (state.flags != prev_state.flags) {
			if (state.suspended) {
				DCC_LOG1(LOG_TRACE, "[%d] suspending...", 
						 thinkos_thread_self());
			} else {
				if (prev_state.suspended) {
					DCC_LOG1(LOG_TRACE, "[%d] wakeup from suspended!", 
							 thinkos_thread_self());
				} 
			}
			prev_state.flags = state.flags;
		}

		if (state.ctrl.dtr != prev_state.ctrl.dtr) {
			vcom->ser_stat.dsr = state.ctrl.dtr;
			usb_cdc_status_set(cdc, &vcom->ser_stat);
			prev_state.ctrl = state.ctrl;
		}

		if ((state.cfg.baudrate != prev_state.cfg.baudrate) ||
			(state.cfg.databits != prev_state.cfg.databits) ||
			(state.cfg.parity != prev_state.cfg.parity) ||
			(state.cfg.stopbits != prev_state.cfg.stopbits)) {

			serial_rx_disable(serial);
			DCC_LOG1(LOG_TRACE, "baudrate=%d", state.cfg.baudrate);
			DCC_LOG1(LOG_TRACE, "databits=%d", state.cfg.databits);
			DCC_LOG1(LOG_TRACE, "parity=%d", state.cfg.parity);
			DCC_LOG1(LOG_TRACE, "stopbits=%d", state.cfg.stopbits);
			if ((state.cfg.baudrate == 921600) && 
				(state.cfg.databits == 8) &&
				(state.cfg.parity == SERIAL_PARITY_NONE)) {
				DCC_LOG(LOG_TRACE, "magic config!");
				vcom->mode = VCOM_MODE_SERVICE; 
			} else {
				serial_config_set(serial, &state.cfg);
				prev_state.cfg = state.cfg;
//				serial_enable(serial);
				serial_rx_enable(serial);
			}
		}
	}
}

#define RECV_STACK_SIZE (VCOM_BUF_SIZE + 256)

uint32_t __attribute__((aligned(8))) led_stack[32];
uint32_t __attribute__((aligned(8))) serial_ctrl_stack[64];
uint32_t __attribute__((aligned(8))) serial_recv_stack[RECV_STACK_SIZE / 4];

int main(int argc, char ** argv)
{
	struct usb_cdc_class * cdc;
	struct serial_dev * serial;
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

	leds_init();

	cdc = usb_cdc_init(&stm32f_usb_fs_dev, cdc_acm_str, 
					   cdc_acm_strcnt);

	serial = serial2_open();

	vcom.serial = serial;
	vcom.cdc = cdc;
	vcom.mode = VCOM_MODE_CONVERTER;

	thinkos_thread_create((void *)led_task, (void *)NULL,
						  led_stack, sizeof(led_stack) |
						  THINKOS_OPT_PRIORITY(1) | THINKOS_OPT_ID(1));


	thinkos_thread_create((void *)serial_recv_task, 
						  (void *)&vcom,
						  serial_recv_stack, sizeof(serial_recv_stack) |
						  THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	thinkos_thread_create((void *)serial_ctrl_task, 
						  (void *)&vcom,
						  serial_ctrl_stack, sizeof(serial_ctrl_stack) |
						  THINKOS_OPT_PRIORITY(2) | THINKOS_OPT_ID(2));

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

