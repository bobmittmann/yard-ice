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

#include <sys/stm32f.h>
#include <arch/cortex-m3.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/serial.h>
#include <sys/usb-cdc.h>

#include <thinkos.h>

#include <sys/dcclog.h>

#include "board.h"
#include "led.h"

struct serial_dev * serial2_open(void);

struct vcom {
	struct serial_dev * serial;
	usb_cdc_class_t * cdc;
	struct serial_status ser_stat;
};

#define VCOM_BUF_SIZE 256

void __attribute__((noreturn)) usb_recv_task(struct vcom vcom[])
{
	struct serial_dev * serial = vcom[0].serial;
	usb_cdc_class_t * cdc = vcom[0].cdc;
	char buf[VCOM_BUF_SIZE];
	int len;

	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());

	for (;;) {
		len = usb_cdc_read(cdc, buf, VCOM_BUF_SIZE, 5000);
		if (len > 0) {
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

			led1_flash(1);
			serial_write(serial, buf, len);
		}

	}
}

void __attribute__((noreturn)) serial_recv_task(struct vcom * vcom)
{
	struct serial_dev * serial = vcom->serial;
	struct usb_cdc_class * cdc = vcom->cdc;
	char buf[VCOM_BUF_SIZE];
	int len;

	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());

	for (;;) {
		len = serial_read(serial, buf, VCOM_BUF_SIZE, 1000);
		if (len > 0) {
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

			led2_flash(1);
			usb_cdc_write(cdc, buf, len);
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
		DCC_LOG1(LOG_TRACE, "[%d] usb_cdc_ctl_wait() sleep!", 
				 thinkos_thread_self());
		usb_cdc_ctl_wait(cdc, 0);

		DCC_LOG1(LOG_TRACE, "[%d] wakeup!", thinkos_thread_self());
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

uint32_t __attribute__((aligned(8))) serial_ctrl_stack[64];
uint32_t __attribute__((aligned(8))) serial_recv_stack[RECV_STACK_SIZE / 4];

int main(int argc, char ** argv)
{
	struct usb_cdc_class * cdc;
	struct serial_dev * serial;
	struct vcom vcom;
	uint64_t esn;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	/* calibrate usecond delay loop */
	cm3_udelay_calibrate();

	DCC_LOG(LOG_TRACE, "1. io_init()");
	io_init();

	DCC_LOG(LOG_TRACE, "2. thinkos_init()");
	thinkos_init(THINKOS_OPT_PRIORITY(1) | THINKOS_OPT_ID(1));

	leds_init();

	led_flash_all(3);

	serial = serial2_open();

	esn = *((uint64_t *)STM32F_UID);
	DCC_LOG2(LOG_TRACE, "ESN=0x%08x%08x", esn >> 32, esn);
	cdc = usb_cdc_init(&stm32f_usb_fs_dev, esn);

	vcom.serial = serial;
	vcom.cdc = cdc;

	thinkos_thread_create((void *)serial_recv_task, 
						  (void *)&vcom,
						  serial_recv_stack, sizeof(serial_recv_stack) |
						  THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	thinkos_thread_create((void *)serial_ctrl_task, 
						  (void *)&vcom,
						  serial_ctrl_stack, sizeof(serial_ctrl_stack) |
						  THINKOS_OPT_PRIORITY(2) | THINKOS_OPT_ID(2));

	usb_vbus(true);

	for (;;) {
		usb_recv_task(&vcom);
	}
}

