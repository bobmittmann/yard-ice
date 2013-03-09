/* 
 * File:	 spi-test.c
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

void stm32f_mco2_cfg(void)
{
	struct stm32f_gpio * gpio = STM32F_GPIOC;
	int pin = 9;

#ifdef STM32F_SYSCFG
	struct stm32f_syscfg * syscfg = STM32F_SYSCFG;
    /* enable I/O compensation cell */
	syscfg->cmpcr |= SYSCFG_CMP_PD;
#endif

	stm32f_gpio_clock_en(gpio);
	stm32f_gpio_mode(gpio, pin, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32f_gpio_af(gpio, pin, GPIO_AF0);

}

