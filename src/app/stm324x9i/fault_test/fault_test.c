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

#include "board.h"

void io_init(void)
{
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOC);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOD);
}

void read_fault(void)
{
	volatile uint32_t * ptr = (uint32_t *)(0x0);
	uint32_t x;
	int i;

	for (i = 0; i < (16 << 4); ++i) {
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

	for (i = 0; i < (16 << 4); ++i) {
		*ptr = x;
		ptr += 0x10000000 / (2 << 4);
	}
}

int main(int argc, char ** argv)
{
	int i;

	io_init();

	for (i = 5; i > 0; --i) {
		thinkos_sleep(1000);
	}

	read_fault();

	return 0;
}



