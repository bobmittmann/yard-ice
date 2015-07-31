/* 
 * File:	 usb-test.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2011 Bob Mittmann. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <sys/stm32f.h>
#include <sys/delay.h>
#include <thinkos.h>

#define LCD_TMR_FREQ 1000000

void __attribute__((constructor)) usleep_timer_init(void)
{
	struct stm32f_tim * tim = STM32F_TIM10;
	unsigned int div;

	/* Timer clock enable */
	stm32_clk_enable(STM32_RCC, STM32_CLK_TIM10);
	/* get the total divisior */
	div = (stm32f_tim2_hz + (LCD_TMR_FREQ / 2)) / LCD_TMR_FREQ;
	/* Timer configuration */
	tim->psc = div - 1;
	tim->arr = 0;
	tim->cnt = 0;
	tim->dier = TIM_UIE; /* Update interrupt enable */
	tim->cr1 = TIM_CMS_EDGE | TIM_OPM | TIM_URS; 
	cm3_irq_pri_set(STM32F_IRQ_TIM10, IRQ_PRIORITY_HIGH);
}

void usleep(unsigned int usec)
{
	struct stm32f_tim * tim = STM32F_TIM10;

	tim->arr = usec - 1; 
	tim->cr1 = TIM_CMS_EDGE | TIM_OPM | TIM_URS | TIM_CEN; 
	thinkos_irq_wait(STM32F_IRQ_TIM10);
	tim->sr = 0;
}

