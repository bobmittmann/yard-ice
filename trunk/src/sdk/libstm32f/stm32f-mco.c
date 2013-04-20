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

void stm32f_mco2_init(void)
{
	struct stm32f_gpio * gpio = STM32F_GPIOC;
	int pin = 9;

#ifdef STM32F_SYSCFG
	struct stm32f_syscfg * syscfg = STM32F_SYSCFG;
    /* enable I/O compensation cell */
	syscfg->cmpcr |= SYSCFG_CMP_EN;
#endif

	stm32f_gpio_clock_en(gpio);
	stm32f_gpio_mode(gpio, pin, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32f_gpio_af(gpio, pin, GPIO_AF0);
}

