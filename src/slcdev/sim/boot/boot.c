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
#include <thinkos.h>
#include <sys/dcclog.h>

#include "board.h"
#include "simlnk.h"

void cm3_debug_mon_isr(void)
{
	DCC_LOG(LOG_TRACE, "board_app_exec().");
	board_app_exec(THINKOS_APP_ADDR);
}

void app_try_exec()
{
	struct cm3_dcb * dcb = CM3_DCB;

	DCC_LOG(LOG_TRACE, "debug/monitor");

	/* enable monitor and send the reset event */
	dcb->demcr |= DCB_DEMCR_MON_EN | DCB_DEMCR_MON_PEND;
	asm volatile ("isb\n" :  :  : );
}

int __attribute__((noreturn)) main(int argc, char ** argv)
{
	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	DCC_LOG(LOG_TRACE, "1. board_init().");
	board_init();

	DCC_LOG(LOG_TRACE, "2. thinkos_init().");
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	DCC_LOG(LOG_TRACE, "3. simlnk_init().");
	simlnk_init(NULL, "SIM", 0, NULL);

	DCC_LOG(LOG_TRACE, "4. app_try_exec().");
	app_try_exec();
	
	DCC_LOG(LOG_TRACE, "5. board_test().");
	board_test();

//	return 0;
}

