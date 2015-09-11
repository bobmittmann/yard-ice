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
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOA);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOB);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOC);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOD);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOE);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOF);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOG);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOI);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOJ);

	stm32_gpio_mode(IO_LED_TRBL, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(IO_LED_ALRM, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(IO_LED_AC_PWR, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(IO_LED_SPR, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(IO_LED_SLNC, OUTPUT, PUSH_PULL | SPEED_LOW);

	stm32_gpio_set(IO_LED_TRBL);
	stm32_gpio_set(IO_LED_ALRM);
	stm32_gpio_set(IO_LED_AC_PWR);
	stm32_gpio_set(IO_LED_SPR);
	stm32_gpio_set(IO_LED_SLNC);
}

int main(int argc, char ** argv)
{
	io_init();

	for (;;) {
		__led_on(IO_LED_SLNC);
		thinkos_sleep(2000);
		__led_on(IO_LED_SPR);
		thinkos_sleep(15000);
		__led_off(IO_LED_SLNC);
		__led_on(IO_LED_AC_PWR);
		thinkos_sleep(1000);
		__led_off(IO_LED_SPR);
		__led_on(IO_LED_TRBL);
		thinkos_sleep(1000);
		__led_off(IO_LED_AC_PWR);
		__led_on(IO_LED_ALRM);
		thinkos_sleep(1000);
		__led_off(IO_LED_TRBL);
		thinkos_sleep(1000);
		__led_off(IO_LED_ALRM);
		thinkos_sleep(1000);
	}

	return 0;
}

