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

#define RELAY_GPIO STM32F_GPIOB, 9
#define PWR_EN_GPIO STM32F_GPIOD, 12 
#define PWR_ST_GPIO STM32F_GPIOD, 11

void bsp_io_ini(void)
{
	stm32f_gpio_clock_en(STM32F_GPIOB);
	stm32f_gpio_clock_en(STM32F_GPIOD);

	stm32f_gpio_mode(PWR_EN_GPIO, OUTPUT, SPEED_MED);
	stm32f_gpio_mode(RELAY_GPIO, OUTPUT, SPEED_MED);
	stm32f_gpio_mode(PWR_ST_GPIO, INPUT, SPEED_SLOW);

	stm32f_gpio_clr(RELAY_GPIO);
	stm32f_gpio_clr(PWR_EN_GPIO);
}

void relay_on(void)
{
	stm32f_gpio_set(RELAY_GPIO);
}

void relay_off(void)
{
	stm32f_gpio_clr(RELAY_GPIO);
}

void ext_pwr_on(void)
{
	stm32f_gpio_set(PWR_EN_GPIO);
}

void ext_pwr_off(void)
{
	stm32f_gpio_clr(PWR_EN_GPIO);
}
