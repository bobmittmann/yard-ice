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
#include <sys/usb-cdc.h>
#include <sys/tty.h>
#include <xmodem.h>

#include <thinkos.h>

void __attribute__((noreturn)) watchdog_task(void)
{
	unsigned int i;

	for (i = 0; ;++i) {
		thinkos_sleep(1000);
		DCC_LOG1(LOG_TRACE, "tick %d", i);
	}
}
	
uint32_t __attribute__((aligned(8))) watchdog_stack[32];

int console_shell(void);

int main(int argc, char ** argv)
{
	usb_cdc_class_t * cdc;
	struct tty_dev * tty;
	FILE * f_tty;
	FILE * f_raw;
	int i;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	DCC_LOG(LOG_TRACE, "1. cm3_udelay_calibrate()");
	cm3_udelay_calibrate();

	DCC_LOG(LOG_TRACE, "2. thinkos_init()");
	thinkos_init(THINKOS_OPT_PRIORITY(8) | THINKOS_OPT_ID(7));

	/* create some threads */
	DCC_LOG(LOG_TRACE, "3. watchdog_task create");
	thinkos_thread_create((void *)watchdog_task, (void *)NULL, 
						  watchdog_stack, sizeof(watchdog_stack), 0);

	DCC_LOG(LOG_TRACE, "4. usb_cdc_init()");
	cdc = usb_cdc_init(&stm32f_otg_fs_dev, *((uint64_t *)STM32F_UID));

	DCC_LOG(LOG_TRACE, "usb_cdc_fopen()");
	f_raw = usb_cdc_fopen(cdc);

	tty = tty_attach(f_raw);
	f_tty = tty_fopen(tty);

	stdout = f_tty;
	stdin = stdout;

	for (i = 0; ;i++) {
		thinkos_sleep(2000);
		console_shell();

//		fprintf(f_tty, "[%d] hello world...\n", i);
	}

	return 0;
}

