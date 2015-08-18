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
#include <sys/dcclog.h>

#include <thinkos.h>

#include "board.h"

#define VERSION_NUM "0.1"
#define VERSION_DATE "Mar, 2015"

const char * const version_str = "ThinkOS Boot Loader " \
							VERSION_NUM " - " VERSION_DATE;
const char * const copyright_str = "(c) Copyright 2015 - Bob Mittmann";

int main(int argc, char ** argv)
{
	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	DCC_LOG(LOG_TRACE, "1. io_init().");

	board_init();

	DCC_LOG(LOG_TRACE, "2. thinkos_init().");
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	board_test();

	__thinkos_thread_abort(0);

	return 0;
}

