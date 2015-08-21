/* 
 * File:	 sem_test.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
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


#include "config.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <sys/delay.h>
#include <sys/stm32f.h>

#define __THINKOS_DMON__
#include <thinkos_dmon.h>
#include <thinkos.h>

#include <sys/console.h>
#include <sys/dcclog.h>


int writer_task(void * arg)
{
	int id = thinkos_thread_self();
	uint8_t buf[1024];
	int i;

	DCC_LOG1(LOG_TRACE, "thread=%d", id);

	thinkos_sleep(100);

	for (i = 0; i < 1024; ++i)
		buf[i] = '0' + id;


	for (i = 0; i < 1000000; ++i) {
		thinkos_sleep(1);
		console_write(NULL, buf, 1024);
	}

	return i;
}

uint32_t writer1_stack[512];
uint32_t writer2_stack[512];

void console_test(void)
{
	/* create the writer thread */
	thinkos_thread_create(writer_task, NULL, 
			writer1_stack, sizeof(writer1_stack));

	/* create the writer thread */
	thinkos_thread_create(writer_task, NULL, 
			writer2_stack, sizeof(writer2_stack));


	for (;;) {
		thinkos_sleep(10000);
	}
};

int main(int argc, char ** argv)
{
	struct dmon_comm * comm;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	/* Calibrate the the uDelay loop */
	cm3_udelay_calibrate();

	/* Initialize the ThinkOS kernel */
	DCC_LOG(LOG_TRACE, "1. thinkos_init()");
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	DCC_LOG(LOG_TRACE, "2. usb_comm_init()");
#if STM32_ENABLE_OTG_FS
	comm = usb_comm_init(&stm32f_otg_fs_dev);
#elif STM32_ENABLE_OTG_HS
	comm = usb_comm_init(&stm32f_otg_hs_dev);
#elif STM32_ENABLE_USB_DEV
	comm = usb_comm_init(&stm32f_usb_fs_dev);
#else
#error "Undefined debug monitor comm port!"
#endif

	DCC_LOG(LOG_TRACE, "3. thinkos_console_init()");
	thinkos_console_init();

	DCC_LOG(LOG_TRACE, "4. thinkos_dmon_init()");
	thinkos_dmon_init(comm, console_task);

	/* Run the test */
	console_test();

	thinkos_sleep(10000);

	return 0;
}

