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
 * @file boot.c
 * @brief YARD-ICE bootloader main
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifdef CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdbool.h>

#include <sys/stm32f.h>
#include <sys/delay.h>

#define __THINKOS_DBGMON__
#include <thinkos/dbgmon.h>
#include <thinkos.h>

#include <sys/dcclog.h>

#include "board.h"

void monitor_task(struct dmon_comm * comm);
void board_init(void);

int main(int argc, char ** argv)
{
	struct dmon_comm * comm;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	{
		int i;

		for(i = 0; i < 16; ++i)
			DCC_LOG1(LOG_TRACE, "%d.", i);
	}

#ifndef UDELAY_FACTOR 
	DCC_LOG(LOG_TRACE, "1. cm3_udelay_calibrate().");
	cm3_udelay_calibrate();
#endif

	DCC_LOG(LOG_TRACE, "2. thinkos_init().");
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

#if THINKOS_ENABLE_MPU
	DCC_LOG(LOG_TRACE, "3. thinkos_mpu_init()");
	thinkos_mpu_init(0x0c00);
#endif

	DCC_LOG(LOG_TRACE, "4. board_init().");
	board_init();

	/* Wait for the other power supply and subsystems to stabilize */
	DCC_LOG(LOG_TRACE, "5. thinkos_sleep().");
	thinkos_sleep(64);

	DCC_LOG(LOG_TRACE, "6. thinkos_console_init()");
	thinkos_console_init();

	DCC_LOG(LOG_TRACE, "7. usb_comm_init()");
	comm = usb_comm_init(&stm32f_otg_fs_dev);

	DCC_LOG(LOG_TRACE, "8. thinkos_dbgmon_init()");
	thinkos_dbgmon_init(comm, monitor_task);

#if THINKOS_ENABLE_MPU
	DCC_LOG(LOG_TRACE, "9. thinkos_userland()");
	thinkos_userland();
#endif

	DCC_LOG(LOG_TRACE, "10. thinkos_thread_abort()");
	thinkos_thread_abort(0);

	DCC_LOG(LOG_ERROR, "11. unreachable code reched!!!");

	return 0;
}

