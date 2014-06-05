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
//	struct stm32_afio * afio = STM32_AFIO;

	stm32_gpio_clock_en(STM32_GPIOA);
	stm32_gpio_clock_en(STM32_GPIOB);
	stm32_gpio_clock_en(STM32_GPIOC);

	stm32_gpio_mode(LED1, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(LED2, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(LED3, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(LED4, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(LED5, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(LED6, OUTPUT, PUSH_PULL | SPEED_LOW);

	stm32_gpio_mode(TRIG, OUTPUT, PUSH_PULL | SPEED_LOW);

	stm32_gpio_mode(ADDR0, INPUT, PULL_UP);
	stm32_gpio_mode(ADDR1, INPUT, PULL_UP);
	stm32_gpio_mode(ADDR2, INPUT, PULL_UP);
	stm32_gpio_mode(ADDR3, INPUT, PULL_UP);

	stm32_gpio_mode(ADDR4, INPUT, PULL_UP);
	stm32_gpio_mode(ADDR5, INPUT, PULL_UP);
	stm32_gpio_mode(ADDR6, INPUT, PULL_UP);
	stm32_gpio_mode(ADDR7, INPUT, PULL_UP);


	/* USART2_TX */
	stm32f_gpio_mode(USART2_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	/* USART1_RX */
	stm32f_gpio_mode(USART2_RX, INPUT, 0);
	/* Use alternate pins for USART2 */
//	afio->mapr |= AFIO_USART2_REMAP;


	stm32_gpio_clr(LED1);
	stm32_gpio_clr(LED2);
	stm32_gpio_clr(LED3);
	stm32_gpio_clr(LED4);
	stm32_gpio_clr(LED5);
	stm32_gpio_clr(LED6);
}

