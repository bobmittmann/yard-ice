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

#include <thinkos.h>
#define __THINKOS_SYS__
#include <thinkos_sys.h>

#include "usb-serial.h"
#include "board.h"
#include "led.h"
#include "io.h"

struct serial_dev * serial1;
struct serial_dev * serial2;
int mode = 0;

void system_reset(void)
{
	DCC_LOG(LOG_TRACE, "...");
    CM3_SCB->aircr =  SCB_AIRCR_VECTKEY | SCB_AIRCR_SYSRESETREQ;
	for(;;);
}

void toggle_mode(void)
{
	switch (mode) {
	case 0:
		pin1_sel_gnd();
		pin2_sel_vcc();
		led2_flash(2);
		led1_flash(2);
		mode = 1;
		break;
	case 1:
		pin1_sel_input();
		pin2_sel_input();
		led2_flash(1);
		led1_flash(1);
		mode = 0;
		break;
	}
}

#define LOOP_TIME 50 
#define BUSY_TIME (5000 / LOOP_TIME)

enum {
	EXT_RST_OFF,
	EXT_RST_ON,
	EXT_RST_WAIT
};

void __attribute__((noreturn)) button_task(void)
{
	int event;
	int i;

	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());

	while (1) {
		event  = btn_event_wait();

		switch (event) {
	
		case EVENT_CLICK:
			DCC_LOG(LOG_TRACE, "EVENT_CLICK");
//			test_sched(TEST_USB);
			break;

		case EVENT_DBL_CLICK:
			DCC_LOG(LOG_TRACE, "EVENT_DBL_CLICK");
			toggle_mode();
			break;

		case EVENT_HOLD1:
			DCC_LOG(LOG_TRACE, "EVENT_HOLD1");
//			if (ext_rst_st == EXT_RST_OFF) {
				/* start external reset timer */
//				ext_rst_tmr = 500 / LOOP_TIME;
				/* start system reset timer */
//				sys_rst_tmr = 5000 / LOOP_TIME;
//				stm32f_gpio_set(EXTRST0_IO);
//				stm32f_gpio_set(EXTRST1_IO);
//				pin2_sel_gnd();
//				led_lock();
//				led2_on();
//				ext_rst_st = EXT_RST_ON;
//			}
			break;

//			DCC_LOG(LOG_TRACE, "BTN_RELEASED");
//			if (ext_rst_st == EXT_RST_WAIT) {
//				ext_rst_st = EXT_RST_OFF;
//			}
			/* reset system reset timer */
//			sys_rst_tmr = 0;

		case EVENT_CLICK_N_HOLD:
			DCC_LOG(LOG_TRACE, "EVENT_CLICK_N_HOLD");
//			stm32_gpio_clr(EXTRST0_IO);
//			stm32f_gpio_clr(EXTRST1_IO);
//			pin2_sel_vcc();
//			led2_off();
//			led_unlock();
//			ext_rst_st = EXT_RST_OFF;
			break;

		case EVENT_HOLD2:
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

//	return 0;
}


void __attribute__((noreturn)) usb_recv_task(struct vcom vcom[])
{
	struct serial_dev * serial1 = vcom[0].serial;
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
			serial_write(serial1, buf, len);
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

uint32_t __attribute__((aligned(8))) button_stack[32];
uint32_t __attribute__((aligned(8))) serial1_ctrl_stack[64];
//uint32_t __attribute__((aligned(8))) serial2_ctrl_stack[64];
uint32_t __attribute__((aligned(8))) serial1_recv_stack[RECV_STACK_SIZE / 4];
//uint32_t __attribute__((aligned(8))) serial2_recv_stack[RECV_STACK_SIZE / 4];
//uint32_t __attribute__((aligned(8))) usb_recv_stack[RECV_STACK_SIZE / 4];

int main(int argc, char ** argv)
{
	uint64_t esn;
	struct vcom vcom[2];
	unsigned int i;

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

	serial1 = serial2_open();
	serial2 = serial3_open();

	vcom[0].serial = serial1;
	vcom[1].serial = serial2;

	esn = *((uint64_t *)STM32F_UID);
	DCC_LOG2(LOG_TRACE, "ESN=0x%08x%08x", esn >> 32, esn);

	ui_init();

	led_flash_all(3);

	vcom[0].cdc = usb_cdc_init(&stm32f_usb_fs_dev, esn);
	vcom[1].cdc = vcom[0].cdc;


	thinkos_thread_create((void *)button_task, (void *)NULL,
						  button_stack, sizeof(button_stack),
						  THINKOS_OPT_PRIORITY(8) | THINKOS_OPT_ID(5));

#if 0
	thinkos_thread_create((void *)usb_recv_task, (void *)vcom,
						  usb_recv_stack, sizeof(usb_recv_stack),
						  THINKOS_OPT_PRIORITY(1) | THINKOS_OPT_ID(0));

#endif


	thinkos_thread_create((void *)serial_recv_task, (void *)&vcom[0],
						  serial1_recv_stack, sizeof(serial1_recv_stack),
						  THINKOS_OPT_PRIORITY(1) | THINKOS_OPT_ID(1));

	thinkos_thread_create((void *)serial_ctrl_task, (void *)&vcom[0],
						  serial1_ctrl_stack, sizeof(serial1_ctrl_stack),
						  THINKOS_OPT_PRIORITY(4) | THINKOS_OPT_ID(3));

#if 0
	thinkos_thread_create((void *)serial_recv_task, (void *)&vcom[1],
						  serial2_recv_stack, sizeof(serial2_recv_stack),
						  THINKOS_OPT_PRIORITY(1) | THINKOS_OPT_ID(2));

	thinkos_thread_create((void *)serial_ctrl_task, (void *)&vcom[1],
						  serial2_ctrl_stack, sizeof(serial2_ctrl_stack),
						  THINKOS_OPT_PRIORITY(4) | THINKOS_OPT_ID(4));
#endif

	test_mod_init(&vcom[0]);

	usb_vbus(true);

	pin1_sel_vcc();
	pin2_sel_input();
	mode = 0;

//	pin1_sel_gnd();
//	pin2_sel_vcc();
//	usb_console(vcom[0].cdc);

	usb_recv_task(vcom);

	for (i = 0; ; ++i) {
		thinkos_sleep(5000);
		DCC_LOG1(LOG_TRACE, "tick %d.", i);
	}

	return 0;
}

