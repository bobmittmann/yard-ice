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
 * @file stm32f-io.c
 * @brief 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 


#include <stdint.h>
#include <sys/stm32f.h>

#define RELAY_GPIO STM32_GPIOB, 9
#define PWR_EN_GPIO STM32_GPIOD, 12 
#define PWR_MON_GPIO STM32_GPIOD, 11

void bsp_io_ini(void)
{
	stm32_gpio_clock_en(STM32_GPIOB);
	stm32_gpio_clock_en(STM32_GPIOD);

	stm32_gpio_mode(PWR_EN_GPIO, OUTPUT, SPEED_LOW);
	stm32_gpio_mode(RELAY_GPIO, OUTPUT, SPEED_LOW);
	stm32_gpio_mode(PWR_MON_GPIO, INPUT, SPEED_LOW | PULL_UP);

	stm32_gpio_clr(RELAY_GPIO);
	stm32_gpio_clr(PWR_EN_GPIO);
}

void relay_on(void)
{
	stm32_gpio_set(RELAY_GPIO);
}

void relay_off(void)
{
	stm32_gpio_clr(RELAY_GPIO);
}

int relay_stat(void)
{
	return stm32_gpio_stat(RELAY_GPIO);
}

void ext_pwr_on(void)
{
	stm32_gpio_set(PWR_EN_GPIO);
}

void ext_pwr_off(void)
{
	stm32_gpio_clr(PWR_EN_GPIO);
}

int ext_pwr_stat(void)
{
	return stm32_gpio_stat(PWR_EN_GPIO);
}

int ext_pwr_mon(void)
{
	return stm32_gpio_stat(PWR_MON_GPIO) ? 0 : 1;
}

