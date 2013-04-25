/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the YARD-ICE.
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
 * @file yard-ice.c
 * @brief YARD-ICE application main
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 


#include <sys/stm32f.h>
#include <sys/param.h>
#include <sys/file.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <sys/usb-cdc.h>
#include <sys/tty.h>

#include <sys/os.h>

#include <sys/dcclog.h>
#include <sys/shell.h>

int usb_task(void * arg)
{
	uint64_t  esn = *((uint64_t *)STM32F_UID);
	usb_cdc_class_t * cdc;
	struct tty_dev * tty;
	FILE * f_tty;
	FILE * f_raw;

	DCC_LOG(LOG_TRACE, "usb_cdc_init()");
	cdc = usb_cdc_init(&stm32f_otg_fs_dev, esn);
	f_raw = usb_cdc_fopen(cdc);

	tty = tty_attach(f_raw);
	f_tty = tty_fopen(tty);

	for (;;) {
		shell(f_tty, yard_ice_get_prompt, yard_ice_greeting);
	}
}

uint32_t usb_stack[1024 + 512];

int usb_shell(void)
{
	int th;

	th = __os_thread_create((void *)usb_task, NULL, 
						   usb_stack, sizeof(usb_stack), 0); 
	return th;
}

