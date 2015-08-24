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

#include <thinkos.h>
#include <sys/serial.h>
#include <crc.h>

#include "board.h"

void board_init(void)
{
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOA);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOB);

	/* LEDs */
	stm32_gpio_mode(IO_LED1, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(IO_LED2, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(IO_LED3, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(IO_LED4, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_clr(IO_LED1);
	stm32_gpio_clr(IO_LED2);
	stm32_gpio_clr(IO_LED3);
	stm32_gpio_clr(IO_LED4);

	/* Switches */
	stm32_gpio_mode(IO_SW1_UP,  INPUT, PULL_UP | SPEED_LOW);
	stm32_gpio_mode(IO_SW1_DWN, INPUT, PULL_UP | SPEED_LOW);
	stm32_gpio_mode(IO_SW2_UP,  INPUT, PULL_UP | SPEED_LOW);
	stm32_gpio_mode(IO_SW2_DWN, INPUT, PULL_UP | SPEED_LOW);

	/* USART2_TX */
	stm32_gpio_mode(IO_UART_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_af(IO_UART_TX, GPIO_AF7);
	/* USART2_RX */
	stm32_gpio_mode(IO_UART_RX, ALT_FUNC, 0);
	stm32_gpio_af(IO_UART_RX, GPIO_AF7);

	/* Comparator */ 
	stm32_gpio_mode(IO_COMP, INPUT, SPEED_MED);

	/* Negative volatge supply */
	stm32_gpio_mode(IO_VNEG_SW, ALT_FUNC, PUSH_PULL | SPEED_MED);
	stm32_gpio_af(IO_VNEG_SW, GPIO_AF1);

	/* Current rate control */
	stm32_gpio_mode(IO_IRATE, ANALOG, 0);

	/* Current sink */
	stm32_gpio_mode(IO_SINK1, OUTPUT, PUSH_PULL | SPEED_MED);
	stm32_gpio_mode(IO_SINK2, OUTPUT, PUSH_PULL | SPEED_MED);
	stm32_gpio_mode(IO_SINK3, OUTPUT, PUSH_PULL | SPEED_MED);
	stm32_gpio_mode(IO_SINK4, OUTPUT, PUSH_PULL | SPEED_MED);
	stm32_gpio_clr(IO_SINK1);
	stm32_gpio_clr(IO_SINK2);
	stm32_gpio_clr(IO_SINK3);
	stm32_gpio_clr(IO_SINK4);
}

