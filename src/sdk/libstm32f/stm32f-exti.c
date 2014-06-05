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
 * @file stm32f-exti.c
 * @brief STM32F
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <sys/stm32f.h>

#include <sys/dcclog.h>

#if defined(STM32F2X)

void stm32f_exti_init(struct stm32_gpio * gpio, unsigned int pin, 
					  unsigned int opt)
{
	struct stm32f_syscfg * syscfg = STM32F_SYSCFG;
	struct stm32f_exti * exti = STM32F_EXTI;
	struct stm32_rcc * rcc = STM32_RCC;
	int port;

	/* IRQ PIN */
	stm32_gpio_clock_en(gpio);
	stm32_gpio_mode(gpio, pin, INPUT, PUSH_PULL | SPEED_HIGH);

	/* System configuration controller clock enable */
	rcc->apb2enr |= RCC_SYSCFGEN;

	port = stm32_gpio_id(gpio);

	/* Select PD6 for EXTI6 */
	syscfg->exticr[pin >> 2] = port << ((pin & 0x3) * 4);

	/* Unmask interrupt */
	exti->imr |= (1 << pin);

	if (opt & EXTI_EDGE_RISING)
		/* Select rising edge trigger */
		exti->rtsr |= (1 << pin);
	else
		/* Select falling edge trigger */
		exti->ftsr |= (1 << pin);

	/* Clear pending flag */
	exti->pr = (1 << pin);
}


#endif

