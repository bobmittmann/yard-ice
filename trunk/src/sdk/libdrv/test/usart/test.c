/* 
 * File:	 usb-test.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2011 Bob Mittmann. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <sys/stm32f.h>
#include <arch/cortex-m3.h>
#include <sys/delay.h>
#include <sys/serial.h>
#include <sys/param.h>
#include <sys/file.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <thinkos.h>

#include <sys/dcclog.h>
#include <sys/usb-cdc.h>

struct vcom {
	struct serial_dev * serial;
	struct usb_cdc_dev * usb;
};

#define VCOM_BUF_SIZE 128

int usb_recv_task(struct vcom * vcom)
{
	struct serial_dev * serial = vcom->serial;
	struct usb_cdc_dev * usb = vcom->usb;
	char buf[VCOM_BUF_SIZE];
	int len;

	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());

	for (;;) {
		len = usb_cdc_read(usb, buf, VCOM_BUF_SIZE, 100);
		if (len > 0)
			serial_write(serial, buf, len);
	}

	return 0;
}

int usb_ctrl_task(struct vcom * vcom)
{
	struct serial_dev * serial = vcom->serial;
	struct usb_cdc_dev * usb = vcom->usb;
	struct usb_cdc_state prev_state;
	struct usb_cdc_state state;

	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());

	usb_cdc_state_get(usb, &prev_state);

	while (1) {
		usb_cdc_ctrl_event_wait(usb, 0);
		usb_cdc_state_get(usb, &state);
		if (memcmp(&state.cfg, &prev_state.cfg, sizeof(struct serial_config)) != 0) {
			DCC_LOG1(LOG_TRACE, "[%d] config changed.", thinkos_thread_self());
			prev_state.cfg = state.cfg;
		}
		if (memcmp(&state.ctrl, &prev_state.ctrl, sizeof(struct serial_control)) != 0) {
			DCC_LOG1(LOG_TRACE, "[%d] control changed.", thinkos_thread_self());
			prev_state.ctrl = state.ctrl;
		}
	}
	return 0;
}


int serial_recv_task(struct vcom * vcom)
{
	struct serial_dev * serial = vcom->serial;
	struct usb_cdc_dev * usb = vcom->usb;
	char buf[VCOM_BUF_SIZE];
	int len;

	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());

	for (;;) {
		len = serial_read(serial, buf, VCOM_BUF_SIZE, 100);
		if (len > 0)
			usb_cdc_write(usb, buf, len);
	}

	return 0;
}

int serial_ctrl_task(struct vcom * vcom)
{
	struct serial_dev * serial = vcom->serial;
	struct usb_cdc_dev * usb = vcom->usb;
	struct usb_cdc_state prev_state;
	struct usb_cdc_state state;

	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());

	usb_cdc_state_get(usb, &prev_state);

	while (1) {
		usb_cdc_ctrl_event_wait(usb, 0);
		usb_cdc_state_get(usb, &state);
#if 0
		if (state.cfg != prev_state.cfg) {
			DCC_LOG1(LOG_TRACE, "[%d] config changed.", thinkos_thread_self());
			prev_state.cfg = state.cfg;
		}
		if (state.ctrl != prev_state.ctrl) {
			DCC_LOG1(LOG_TRACE, "[%d] control changed.", thinkos_thread_self());
			prev_state.ctrl = state.ctrl;
		}
#endif
	}
	return 0;
}

#define STACK_SIZE 512
uint32_t usb_recv_stack[STACK_SIZE / 4];
uint32_t usb_ctrl_stack[STACK_SIZE / 4];
uint32_t serial_recv_stack[STACK_SIZE / 4];
uint32_t serial_ctrl_stack[STACK_SIZE / 4];

int main(int argc, char ** argv)
{
	struct vcom vcom;
	int i = 0;

	DCC_LOG_CONNECT();
	DCC_LOG_INIT();

	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	vcom.usb = usb_cdc_init();
	vcom.serial = serial_init(stm32f_uart5);

	thinkos_thread_create((void *)usb_recv_task, (void *)&vcom,
						  usb_recv_stack, STACK_SIZE, 0);

	thinkos_thread_create((void *)usb_ctrl_task, (void *)&vcom,
						  usb_ctrl_stack, STACK_SIZE, 0);

	thinkos_thread_create((void *)serial_recv_task, (void *)&vcom,
						  serial_recv_stack, STACK_SIZE, 0);

	thinkos_thread_create((void *)serial_ctrl_task, (void *)&vcom,
						  serial_ctrl_stack, STACK_SIZE, 0);

	for (i = 0; ;i++) {
		thinkos_sleep(10000);
	}

	return 0;
}

