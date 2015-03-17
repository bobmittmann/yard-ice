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
#include <sys/tty.h>

#include <sys/dcclog.h>

#include <thinkos.h>
#define __THINKOS_SYS__
#include <thinkos_sys.h>

#include "board.h"

#include <bacnet/bacnet-ptp.h>

#define VERSION_NUM "0.1"
#define VERSION_DATE "Mar, 2015"

const char * version_str = "BACnet Demo " \
							VERSION_NUM " - " VERSION_DATE;
const char * copyright_str = "(c) Copyright 2015 - Bob Mittmann";

int stdio_shell(void);

void io_init(void)
{
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

	/* USART6_TX */
	stm32_gpio_mode(UART6_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_af(UART6_TX, GPIO_AF7);
	/* USART6_RX */
	stm32_gpio_mode(UART6_RX, ALT_FUNC, PULL_UP);
	stm32_gpio_af(UART6_RX, GPIO_AF7);

}

const struct file stm32_uart5_file = {
	.data = STM32_UART5, 
	.op = &stm32_usart_fops 
};

void stdio_init(struct serial_dev * serdev)
{
	struct tty_dev * tty;
	FILE * f_tty;
	FILE * f_raw;

	f_raw = serial_fopen(serdev);
	tty = tty_attach(f_raw);
	f_tty = tty_fopen(tty);

	stderr = (struct file *)&stm32_uart5_file;
	stdout = f_tty;
	stdin = f_tty;
}

int main(int argc, char ** argv)
{
	struct serial_dev * console;
	struct bacnet_ptp_lnk * ptp_lnk;
	uint8_t buf[512];
	int len;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	cm3_udelay_calibrate();

	DCC_LOG(LOG_TRACE, "1. stm32f_nvram_env_init().");
	stm32f_nvram_env_init();

	DCC_LOG(LOG_TRACE, "2. thinkos_init().");
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	DCC_LOG(LOG_TRACE, "3. io_init()");
	io_init();

	DCC_LOG(LOG_TRACE, "4. console serial init");
	console = stm32f_uart5_serial_init(57600, SERIAL_8N1);

	DCC_LOG(LOG_TRACE, "4. stdio_init().");
	stdio_init(console);

	fprintf(stderr, "\r\n!\r\n");

	printf("\n");
	printf("---------------------------------------------------------\n");
	printf(" BACnet Test\n");
	printf("---------------------------------------------------------\n");
	printf("\n");


	DCC_LOG(LOG_TRACE, "5. starting console shell...");

	for (;;) {
		DCC_LOG(LOG_WARNING, "Console shell!");
		stdio_shell();

		/* BACnet protocol... */
		DCC_LOG(LOG_WARNING, "BACnet Data Link Connection!");
		ptp_lnk = bacnet_ptp_inbound(console);

		while ((len = bacnet_ptp_recv(ptp_lnk, buf)) >= 0) {

			DCC_LOG(LOG_TRACE, "BACnet PDU received...");
		}
	}

	return 0;
}


