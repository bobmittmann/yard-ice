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

#include <sys/dcclog.h>

#include <thinkos.h>
#define __THINKOS_IRQ__
#include <thinkos_irq.h>

struct file * uart_console_open(unsigned int baudrate, unsigned int flags);
struct file * usb_cdc_open(void);

int main(int argc, char ** argv)
{
	char buf[516];
	int i = 0;
	FILE * f;

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

	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

//	stdout = usb_cdc_open();
//	stdin = stdout;

	f = uart_console_open(115200, SERIAL_8N1);

	fprintf(f, "Hello world!\n");

	for (i = 0; ;i++) {
//		fgets(buf, 512, stdin);
		/* echo back */
//		printf(buf);
//		buf[0] = '\0';
//		fgets(buf, 512, f);
//		fprintf(f, buf);
//		thinkos_sleep(1000);

		thinkos_sleep(1000);
		fprintf(f, "%d ", i);
	}

	return 0;
}

