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

#define VERSION_MAJOR 1
#define VERSION_MINOR 0

struct serial_dev * serial2_open(void);

struct vcom {
	struct serial_dev * serial;
	usb_cdc_class_t * cdc;
	struct serial_status ser_stat;
};

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

void __attribute__((noreturn)) usb_recv_task(struct vcom vcom[])
{
	struct serial_dev * serial = vcom[0].serial;
	usb_cdc_class_t * cdc = vcom[0].cdc;
	uint8_t buf[VCOM_BUF_SIZE];
	int len;

	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());

	for (;;) {
		len = usb_cdc_read(cdc, buf, VCOM_BUF_SIZE, 5000);
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
			led_flash(LED_AMBER, 50);
			usb_cdc_write(cdc, buf, len);
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

		if ((state.cfg.baudrate != prev_state.cfg.baudrate) ||
			(state.cfg.databits != prev_state.cfg.databits) ||
			(state.cfg.parity != prev_state.cfg.parity) ||
			(state.cfg.stopbits != prev_state.cfg.stopbits)) {
			serial_config_set(serial, &state.cfg);
			prev_state.cfg = state.cfg;
		}

		if (state.ctrl.dtr != prev_state.ctrl.dtr) {
			vcom->ser_stat.dsr = state.ctrl.dtr;
			usb_cdc_status_set(cdc, &vcom->ser_stat);
			prev_state.ctrl = state.ctrl;
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
	uint64_t esn;
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

//	esn = *((uint64_t *)STM32F_UID);
	esn = 0x5532733438350000 + (VERSION_MAJOR << 8) + VERSION_MINOR;
	DCC_LOG2(LOG_TRACE, "ESN=0x%08x%08x", esn >> 32, esn);
	cdc = usb_cdc_init(&stm32f_usb_fs_dev, esn);

	serial = serial2_open();

	vcom.serial = serial;
	vcom.cdc = cdc;

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

