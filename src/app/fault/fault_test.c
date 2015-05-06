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
 * @file hello.c
 * @brief application test
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdio.h>
#include <thinkos.h>
#include <sys/console.h>
#include <sys/stm32f.h>
#include <sys/serial.h>
#include <sys/tty.h>

#include "board.h"

void io_init(void)
{
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOC);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOD);
	/* USART5 TX */
	stm32_gpio_mode(UART5_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_af(UART5_TX, GPIO_AF8);
	/* USART5 RX */
	stm32_gpio_mode(UART5_RX, ALT_FUNC, PULL_UP);
	stm32_gpio_af(UART5_RX, GPIO_AF8);
}

void stdio_init(void)
{
	struct serial_dev * ser;
	struct tty_dev * tty;
	FILE * f_tty;
	FILE * f_raw;

	ser = stm32f_uart5_serial_init(115200, SERIAL_8N1);
	f_raw = serial_fopen(ser);
	tty = tty_attach(f_raw);
	f_tty = tty_fopen(tty);

	stderr = console_fopen();
	stdout = f_tty;
	stdin = f_tty;
}

void read_fault(void)
{
	volatile uint32_t * ptr = (uint32_t *)(0x0);
	uint32_t x;
	int i;

	printf("Invalid read access\n");

	for (i = 0; i < (16 << 4); ++i) {
		printf("0x%08x\n", (uintptr_t)ptr);
//		fflush(stdout);
		x = *ptr;
		(void)x;
		ptr += 0x10000000 / (2 << 4);
	}
}

void write_fault(void)
{
	volatile uint32_t * ptr = (uint32_t *)(0x0);
	uint32_t x = 0;
	int i;

	printf("Invalid write access\n");
	for (i = 0; i < (16 << 4); ++i) {
		printf("0x%08x\n", (uintptr_t)ptr);
		fflush(stdout);
		*ptr = x;
		ptr += 0x10000000 / (2 << 4);
	}
}

int main(int argc, char ** argv)
{
	int i;

	io_init();
	stdio_init();

	printf("\n");
	printf("--------------------------------------------\n");
	printf("- Serial Console Test\n");
	printf("--------------------------------------------\n");

	for (i = 10; i > 0; --i) {
		thinkos_sleep(1000);
		printf("- %d\n", i);
	}
	printf("- %d\n", i);

	read_fault();

	return 0;
}



