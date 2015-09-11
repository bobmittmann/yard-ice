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

#include <sys/stm32f.h>
#include <sys/console.h>
#include <stdio.h>
#include <thinkos.h>

#include "board.h"

void io_init(void)
{
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOG);
	stm32_gpio_mode(LED1, OUTPUT, OPEN_DRAIN | SPEED_MED);
	stm32_gpio_mode(LED2, OUTPUT, OPEN_DRAIN | SPEED_MED);
	stm32_gpio_mode(LED3, OUTPUT, OPEN_DRAIN | SPEED_MED);
	stm32_gpio_mode(LED4, OUTPUT, OPEN_DRAIN | SPEED_MED);

	stm32_gpio_set(LED1);
	stm32_gpio_set(LED2);
	stm32_gpio_set(LED3);
	stm32_gpio_set(LED4);
}

int main(int argc, char ** argv)
{
	io_init();

	for (;;) {
		__led_on(LED1);
		thinkos_sleep(100);
		__led_on(LED2);
		thinkos_sleep(100);
		__led_off(LED1);
		__led_on(LED3);
		thinkos_sleep(100);
		__led_off(LED2);
		__led_on(LED4);
		thinkos_sleep(100);
		__led_off(LED3);
		thinkos_sleep(100);
		__led_off(LED4);
		thinkos_sleep(100);


		__led_on(LED4);
		thinkos_sleep(100);
		__led_on(LED3);
		thinkos_sleep(100);
		__led_off(LED4);
		__led_on(LED2);
		thinkos_sleep(100);
		__led_off(LED3);
		__led_on(LED1);
		thinkos_sleep(100);
		__led_off(LED2);
		thinkos_sleep(100);
		__led_off(LED1);
		thinkos_sleep(100);
	}

	return 0;
}

