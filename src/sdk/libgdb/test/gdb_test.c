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

#include <sys/stm32f.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/serial.h>
#include <sys/delay.h>
#include <sys/dcclog.h>
#include <thinkos.h>
#include <tcpip/net.h>
#include <tcpip/dhcp.h>
#include <tcpip/in.h>

#include "board.h"
#include "monitor.h"

#define VERSION_NUM "0.3"
#define VERSION_DATE "Mar, 2015"

const char * version_str = "HTTP Server Demo " \
							VERSION_NUM " - " VERSION_DATE;
const char * copyright_str = "(c) Copyright 2015 - Bob Mittmann";

void stdio_init(void);
int stdio_shell(void);
int gdb_rspd_start(FILE * f);


void io_init(void)
{
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOA);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOB);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOC);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOD);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOE);

#if 0
	/* JTAG TRST Pin */
	stm32_gpio_mode(MODSW, INPUT, SPEED_LOW);
#endif

	/* USART5 TX */
	stm32_gpio_mode(UART5_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_af(UART5_TX, GPIO_AF8);
	/* USART5 RX */
	stm32_gpio_mode(UART5_RX, ALT_FUNC, PULL_UP);
	stm32_gpio_af(UART5_RX, GPIO_AF8);

	/* USART6_TX */
	stm32_gpio_mode(UART6_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_af(UART6_TX, GPIO_AF7);
	/* USART6_RX */
	stm32_gpio_mode(UART6_RX, ALT_FUNC, PULL_UP);
	stm32_gpio_af(UART6_RX, GPIO_AF7);

}

int main(int argc, char ** argv)
{
	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	cm3_udelay_calibrate();

	DCC_LOG(LOG_TRACE, "3. io_init()");
	io_init();

	DCC_LOG(LOG_TRACE, "2. thinkos_init().");
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	monitor_init();

	DCC_LOG(LOG_TRACE, "4. stdio_init().");
	stdio_init();

	printf("\n");
	printf("---------------------------------------------------------\n");
	printf(" ThinkOS (GDB Test)\n");
	printf("---------------------------------------------------------\n");
	printf("\n");


	DCC_LOG(LOG_TRACE, "9. starting console shell...");
	for (;;) {
		stdio_shell();
//		gdb_rspd_start(stdout);
	}

	return 0;
}


