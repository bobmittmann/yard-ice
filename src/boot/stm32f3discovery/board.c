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

#include "board.h"

void io_init(void)
{
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOA);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOE);

	stm32_gpio_mode(LED4_IO, OUTPUT, OPEN_DRAIN | SPEED_MED);
	stm32_gpio_mode(LED5_IO, OUTPUT, OPEN_DRAIN | SPEED_MED);
	stm32_gpio_mode(LED6_IO, OUTPUT, OPEN_DRAIN | SPEED_MED);
	stm32_gpio_mode(LED7_IO, OUTPUT, OPEN_DRAIN | SPEED_MED);
	stm32_gpio_mode(LED8_IO, OUTPUT, OPEN_DRAIN | SPEED_MED);
	stm32_gpio_mode(LED9_IO, OUTPUT, OPEN_DRAIN | SPEED_MED);
	stm32_gpio_mode(LED10_IO, OUTPUT, OPEN_DRAIN | SPEED_MED);
}

