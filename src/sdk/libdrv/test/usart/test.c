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

#define __THINKOS_IRQ__
#include <thinkos_irq.h>
#include <thinkos.h>

#include <sys/dcclog.h>
#include <sys/usb-cdc.h>

struct serial_dev * serial;

int printer_task(FILE * f)
{

	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());
	thinkos_sleep(100);

	return 0;
}

#define BUF_SIZE 256

int usb_rcv_task(struct usb_dev * usb)
{
	char buf[BUF_SIZE];
	int ret;
	struct serial_cfg cfg;

	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());
	thinkos_sleep(100);

	while (1) {
		ret = usb_cdc_read(usb, buf, BUF_SIZE, 100);
		if (ret < 0) {
			if (ret == -USB_CDC_EINTR) {
				usb_cdc_serial_cfg_get(usb, &cfg);
				serial_cfg_set(serial, &cfg);
			}

		}
	}

	return 0;
}

#define STACK_SIZE 512
uint32_t usb_rcv_stack[STACK_SIZE / 4];
uint32_t printer_stack2[STACK_SIZE / 4];

uint32_t echo_stack1[STACK_SIZE / 4];
uint32_t echo_stack2[STACK_SIZE / 4];

int main(int argc, char ** argv)
{
	struct usb_dev * usb;
	int i = 0;

//	DCC_LOG(LOG_TRACE, "cm3_udelay_calibrate()");
	DCC_LOG_CONNECT();
	DCC_LOG_INIT();

	cm3_udelay_calibrate();
	DCC_LOG(LOG_TRACE, "stm32f_usart_open().");
	stderr = stm32f_usart_open(STM32F_UART5, 115200, SERIAL_8N1);

	fprintf(stderr, "\n");
	fprintf(stderr, "------------------------------------------------------\n");
	fprintf(stderr, "- USB test\n");
	fprintf(stderr, "------------------------------------------------------\n");
	fprintf(stderr, "\n");
	udelay(1000);

	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	usb = usb_cdc_init();


	/* create some printer threads */
	thinkos_thread_create((void *)usb_rcv_task, (void *)usb, 
						  usb_rcv_stack, STACK_SIZE, 0);


	for (i = 0; ;i++) {
		thinkos_sleep(10000);
	}

	return 0;
}

