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

#ifdef CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdbool.h>

#include <sys/stm32f.h>
#include <sys/delay.h>

#define __THINKOS_DMON__
#include <thinkos_dmon.h>
#include <thinkos.h>

#include <sys/dcclog.h>

#ifndef BOOT_ENABLE_GDB
#define BOOT_ENABLE_GDB 0
#endif

#if (BOOT_ENABLE_GDB)
#include <gdb.h>
#endif

#include "board.h"

#define VERSION_NUM "0.2"
#define VERSION_DATE "Jul, 2015"

const char * const version_str = "ThinkOS Boot Loader " \
							VERSION_NUM " - " VERSION_DATE;
const char * const copyright_str = "(c) Copyright 2015 - Bob Mittmann";

void monitor_task(struct dmon_comm * comm);

void monitor_init(void)
{
	struct dmon_comm * comm;

	DCC_LOG(LOG_TRACE, "1. usb_comm_init()");
#if STM32_ENABLE_OTG_FS
	comm = usb_comm_init(&stm32f_otg_fs_dev);
#elif STM32_ENABLE_OTG_HS
	comm = usb_comm_init(&stm32f_otg_hs_dev);
#elif STM32_ENABLE_USB_DEV
	comm = usb_comm_init(&stm32f_usb_fs_dev);
#else
#error "Undefined debug monitor comm port!"
#endif

	DCC_LOG(LOG_TRACE, "2. thinkos_console_init()");
	thinkos_console_init();

#if (BOOT_ENABLE_GDB)
	DCC_LOG(LOG_TRACE, "3. gdb_init()");
	gdb_init(monitor_task);
#endif

	DCC_LOG(LOG_TRACE, "4. thinkos_dmon_init()");
	thinkos_dmon_init(comm, monitor_task);
}

int main(int argc, char ** argv)
{
	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	DCC_LOG(LOG_TRACE, "1. board_init().");
	this_board.init();

	DCC_LOG(LOG_TRACE, "2. cm3_udelay_calibrate().");
	cm3_udelay_calibrate();

	DCC_LOG(LOG_TRACE, "3. thinkos_init().");
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	DCC_LOG(LOG_TRACE, "4. monitor_init()");
	monitor_init();

	__thinkos_thread_abort(0);

	return 0;
}

