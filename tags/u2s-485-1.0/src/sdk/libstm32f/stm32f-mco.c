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
 * @file stm32f-mco.c
 * @brief STM32F
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 


#include <sys/stm32f.h>

#define MCO2_GPIO STM32_GPIOC
#define MCO2_PIN 9

void stm32f_mco2_init(void)
{
	struct stm32_gpio * gpio = MCO2_GPIO;
	int pin = MCO2_PIN;

#ifdef STM32F_SYSCFG
	struct stm32f_syscfg * syscfg = STM32F_SYSCFG;
    /* enable I/O compensation cell */
	syscfg->cmpcr |= SYSCFG_CMP_EN;
#endif

	/* initial state is disabled */
	stm32_gpio_mode(gpio, pin, OUTPUT, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_clock_en(gpio);
	stm32_gpio_af(gpio, pin, GPIO_AF0);
}

void stm32f_mco2_disable(void)
{
	struct stm32_gpio * gpio = MCO2_GPIO;
	int pin = MCO2_PIN;

	stm32_gpio_mode(gpio, pin, OUTPUT, PUSH_PULL | SPEED_HIGH);
}

void stm32f_mco2_enable(void)
{
	struct stm32_gpio * gpio = MCO2_GPIO;
	int pin = MCO2_PIN;

	stm32_gpio_mode(gpio, pin, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
}

