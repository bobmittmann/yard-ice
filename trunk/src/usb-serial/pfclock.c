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
 * @file pflock.c
 * @brief Real-time trace
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <sys/stm32f.h>
#include <stdlib.h>

/*---------------------------------------------------------------------------
 * Profiling clock
 *---------------------------------------------------------------------------
 */

static uint16_t clock;

#define PFCLOCK_FREQ 20000

uint32_t pfdt2us(uint32_t dt)
{
	return (1000000 / PFCLOCK_FREQ) * dt;
}

uint32_t pfdt(void)
{
	struct stm32f_tim * tim = STM32F_TIM2;
	uint16_t now;
	uint16_t dt;

	now = tim->cnt;
	dt = now - clock;
	clock = now;

	return dt;
}

uint32_t pfclock(void)
{
	struct stm32f_tim * tim = STM32F_TIM2;

	return tim->cnt;
}

void pfclock_init(void)
{
	struct stm32f_rcc * rcc = STM32F_RCC;
	struct stm32f_tim * tim = STM32F_TIM2;
	uint32_t div;

	/* get the total divisior */
	div = (stm32f_tim1_hz + (PFCLOCK_FREQ / 2)) / PFCLOCK_FREQ;

	/* Timer clock enable */
	rcc->apb1enr |= RCC_TIM2EN;
	
	/* Timer configuration */
	tim->psc = div - 1;
	tim->arr = 0xffffffff;
	tim->cnt = 0;
	tim->egr = 0;
	tim->dier = 0;
	tim->cr2 = 0;
	tim->cr1 = TIM_CEN; /* Enable counter */

	tim->egr = TIM_UG; /* Update registers */
}

