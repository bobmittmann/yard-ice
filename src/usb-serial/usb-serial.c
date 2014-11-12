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
#include <sys/delay.h>
#include <sys/serial.h>
#include <sys/param.h>
#include <sys/file.h>
#include <sys/null.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/tty.h>
#include <sys/shell.h>

#include <thinkos.h>
#define __THINKOS_SYS__
#include <thinkos_sys.h>

#include "usb-serial.h"
#include "board.h"
#include "led.h"
#include "io.h"

#define VERSION_NUM "0.3"
#define VERSION_DATE "Jun, 2014"

extern const struct shell_cmd shell_cmd_tab[];

const char * shell_greeting(void)
{
	return "\nUSB-Serial Converter " VERSION_NUM " - " VERSION_DATE "\n"
	"(c) Copyright 2014 - Bob Mittmann (bobmittmann@gmail.com)\n\n";
}

const char * shell_prompt(void)
{
	return "[USB-SERIAL]$ ";
}

int8_t usb_recv_th = -1;
int8_t serial_recv_th = -1;
int8_t serial_ctrl_th = -1;

struct usb_serial_ctrl {
	uint8_t io_mode;
	uint8_t mode;
	int8_t flag;
	struct {
		struct serial_dev * dev;
		struct serial_status stat;
	} serial[2];
	struct {
		struct usb_cdc_class * dev;
		FILE * f;
	} usb;
} usb_serial;

void system_reset(void)
{
	DCC_LOG(LOG_TRACE, "...");
    CM3_SCB->aircr =  SCB_AIRCR_VECTKEY | SCB_AIRCR_SYSRESETREQ;
	for(;;);
}

void io_mode_set(int mode)
{
	switch (mode) {
	case 0:
		pin1_sel_output();
		pin2_sel_vcc();
		break;
	case 1:
		pin1_sel_input();
		pin2_sel_input();
		break;
	case 2:
		pin1_sel_gnd();
		pin2_sel_vcc();
	}
	usb_serial.io_mode = mode;
}

void io_mode_toggle(void)
{
	switch (usb_serial.io_mode) {
	case 0:
		io_mode_set(1);
		led2_flash(2);
		led1_flash(2);
		break;
	case 1:
		io_mode_set(0);
		led2_flash(1);
		led1_flash(1);
		break;
	}
}

void __attribute__((noreturn)) event_task(void)
{
	int event;
	int i;

	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());

	while (1) {
		event  = event_wait();

		switch (event) {
	
		case EVENT_BTN_CLICK:
			DCC_LOG(LOG_TRACE, "EVENT_CLICK");
			break;

		case EVENT_BTN_DBL_CLICK:
			DCC_LOG(LOG_TRACE, "EVENT_DBL_CLICK");
			io_mode_toggle();
			break;

		case EVENT_BTN_HOLD1:
			DCC_LOG(LOG_TRACE, "EVENT_HOLD1");
			break;

		case EVENT_BTN_CLICK_N_HOLD:
			DCC_LOG(LOG_TRACE, "EVENT_CLICK_N_HOLD");
			thinkos_flag_set(usb_serial.flag);
			break;

		case EVENT_BTN_HOLD2:
			DCC_LOG(LOG_TRACE, "EVENT_HOLD2");
			led_lock();

			for (i = 0; i < 10; ++i) {
				led2_on();
				thinkos_sleep(100);
				led2_off();
				thinkos_sleep(200);
			}

			system_reset();
			break;
		}
	}
}


void __attribute__((noreturn)) usb_recv_task(struct vcom vcom[])
{
	struct serial_dev * serial = vcom[0].serial;
//	struct serial_dev * serial2 = vcom[1].serial;
	usb_cdc_class_t * cdc = vcom[0].cdc;
	char buf[VCOM_BUF_SIZE];
	int len;

	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());

	for (;;) {
		len = usb_cdc_read(cdc, buf, VCOM_BUF_SIZE, 5000);
		if (len > 0) {
			led1_flash(1);
			DCC_LOG1(LOG_TRACE, "USB RX: %d bytes.", len);
			serial_write(serial, buf, len);
//			serial_write(serial2, buf, len);
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
		len = serial_read(serial, buf, VCOM_BUF_SIZE, 100);
		if (len > 0) {
			DCC_LOG5(LOG_INFO, "len=%d [%c%c%c%c]", 
					 len, buf[0], buf[1], buf[2], buf[3]);
			DCC_LOG1(LOG_TRACE, "%d bytes received!", len);
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

	while (1) {
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

		DCC_LOG1(LOG_TRACE, "[%d] sleep!", thinkos_thread_self());
		usb_cdc_ctl_wait(cdc, 0);
		DCC_LOG1(LOG_TRACE, "[%d] wakeup!", thinkos_thread_self());
	}
}

#define RECV_STACK_SIZE (VCOM_BUF_SIZE + 256)

uint32_t __attribute__((aligned(8))) event_stack[32];
uint32_t __attribute__((aligned(8))) serial1_ctrl_stack[64];
//uint32_t __attribute__((aligned(8))) serial2_ctrl_stack[64];
uint32_t __attribute__((aligned(8))) serial1_recv_stack[RECV_STACK_SIZE / 4];
//uint32_t __attribute__((aligned(8))) serial2_recv_stack[RECV_STACK_SIZE / 4];
uint32_t __attribute__((aligned(8))) usb_recv_stack[RECV_STACK_SIZE / 4];

int main(int argc, char ** argv)
{
	struct usb_cdc_class * cdc;
	struct serial_dev * serial1;
	struct serial_dev * serial2;
	uint64_t esn;
	struct vcom vcom[2];
	struct tty_dev * tty;
	FILE * f_tty;
	FILE * f_raw;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	stdout = null_fopen("");
	stderr = stdout;
	stdin = stdout;

	/* calibrate usecond delay loop */
	cm3_udelay_calibrate();

	DCC_LOG(LOG_TRACE, "1. io_init()");
	io_init();

	DCC_LOG(LOG_TRACE, "2. thinkos_init()");
	thinkos_init(THINKOS_OPT_PRIORITY(8) | THINKOS_OPT_ID(7));

	leds_init();

	ui_init();
	io_mode_set(0);

	led_flash_all(3);


	serial1 = serial2_open();
	serial2 = serial3_open();

	esn = *((uint64_t *)STM32F_UID);
	DCC_LOG2(LOG_TRACE, "ESN=0x%08x%08x", esn >> 32, esn);

	cdc = usb_cdc_init(&stm32f_usb_fs_dev, esn);
	f_raw = usb_cdc_fopen(cdc);
	tty = tty_attach(f_raw);
	f_tty = tty_fopen(tty);

	vcom[0].serial = serial1;
	vcom[1].serial = serial2;

	vcom[0].cdc = cdc;
	vcom[1].cdc = vcom[0].cdc;


	thinkos_thread_create((void *)event_task, (void *)NULL,
						  event_stack, sizeof(event_stack) |
						  THINKOS_OPT_PRIORITY(8) | THINKOS_OPT_ID(5));

	serial_recv_th = thinkos_thread_create((void *)serial_recv_task, 
										  (void *)&vcom[0],
						  serial1_recv_stack, sizeof(serial1_recv_stack) |
						  THINKOS_OPT_PRIORITY(1) | THINKOS_OPT_ID(1));

	serial_ctrl_th = thinkos_thread_create((void *)serial_ctrl_task, 
										  (void *)&vcom[0],
						  serial1_ctrl_stack, sizeof(serial1_ctrl_stack) |
						  THINKOS_OPT_PRIORITY(4) | THINKOS_OPT_ID(3));

#if 0
	thinkos_thread_create((void *)serial_recv_task, (void *)&vcom[1],
						  serial2_recv_stack, sizeof(serial2_recv_stack),
						  THINKOS_OPT_PRIORITY(1) | THINKOS_OPT_ID(2));

	thinkos_thread_create((void *)serial_ctrl_task, (void *)&vcom[1],
						  serial2_ctrl_stack, sizeof(serial2_ctrl_stack),
						  THINKOS_OPT_PRIORITY(4) | THINKOS_OPT_ID(4));
#endif


	usb_vbus(true);

	usb_recv_th = thinkos_thread_create((void *)usb_recv_task, (void *)vcom,
						  usb_recv_stack, sizeof(usb_recv_stack) |
						  THINKOS_OPT_PRIORITY(1) | THINKOS_OPT_ID(0));

	usb_serial.usb.dev = cdc;
	usb_serial.usb.f = f_tty;
	usb_serial.serial[0].dev = serial1;
	usb_serial.serial[1].dev = serial2;
	usb_serial.flag = thinkos_flag_alloc();

	while (1) {
		thinkos_flag_wait(usb_serial.flag);

		led1_flash(4);
		thinkos_pause(serial_recv_th);
		thinkos_pause(usb_recv_th);
		shell(f_tty, shell_prompt, shell_greeting, shell_cmd_tab);

		led2_flash(4);
		thinkos_flag_clr(usb_serial.flag);
		thinkos_resume(usb_recv_th);
		thinkos_resume(serial_recv_th);
	}
}

