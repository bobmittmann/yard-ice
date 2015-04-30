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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <sys/stm32f.h>
#include <sys/delay.h>

#define __THINKOS_DMON__
#include <thinkos_dmon.h>
#include <thinkos.h>

#include <sys/dcclog.h>
#include <gdb.h>

#include "board.h"

#define VERSION_NUM "0.1"
#define VERSION_DATE "Mar, 2015"

const char * const version_str = "ThinkOS Boot Loader " \
							VERSION_NUM " - " VERSION_DATE;
const char * const copyright_str = "(c) Copyright 2015 - Bob Mittmann";

void io_init(void)
{
#if 0
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOA);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOB);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOC);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOD);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOE);

	/* USART5 TX */
	stm32_gpio_mode(UART5_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_af(UART5_TX, GPIO_AF8);
	/* USART5 RX */
	stm32_gpio_mode(UART5_RX, ALT_FUNC, PULL_UP);
	stm32_gpio_af(UART5_RX, GPIO_AF8);
#endif
}

void boot_task(struct dmon_comm * comm)
{
	__thinkos_thread_abort(0);

	DCC_LOG(LOG_TRACE, "dmon_app_exec()");
	dmon_app_exec();

	dmon_exec(monitor_task);
}


void monitor_init(void)
{
	struct dmon_comm * comm;

	comm = usb_comm_init(&stm32f_otg_fs_dev);

	thinkos_console_init();

	gdb_init(monitor_task);

	thinkos_dmon_init(comm, boot_task);
}

int main(int argc, char ** argv)
{
	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	DCC_LOG(LOG_TRACE, "1. cm3_udelay_calibrate().");
	cm3_udelay_calibrate();

	DCC_LOG(LOG_TRACE, "2. thinkos_init().");
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	DCC_LOG(LOG_TRACE, "3. io_init()");
	io_init();
		
	DCC_LOG(LOG_TRACE, "4. monitor_init()");
	monitor_init();

	__thinkos_thread_abort(0);

	return 0;
}

