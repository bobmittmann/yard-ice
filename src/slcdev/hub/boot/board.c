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

#include <stdlib.h>
#include <stdbool.h>

#include <sys/stm32f.h>
#include <sys/dcclog.h>
#include <thinkos.h>

#include "board.h"

void board_init(void)
{
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOA);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOB);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOC);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOD);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOE);

	/* - LEDs ---------------------------------------------------------*/
	stm32_gpio_mode(IO_LED1, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_clr(IO_LED1);

	stm32_gpio_mode(IO_LED2, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_clr(IO_LED2);

	/* - Address switch -------------------------------------------------*/
	stm32_gpio_mode(IO_ADDR0, INPUT, PULL_UP);
	stm32_gpio_mode(IO_ADDR1, INPUT, PULL_UP);
	stm32_gpio_mode(IO_ADDR2, INPUT, PULL_UP);
	stm32_gpio_mode(IO_ADDR3, INPUT, PULL_UP);

	/* - Mode switch ---------------------------------------------------*/
	stm32_gpio_mode(IO_MODE0, INPUT, PULL_UP);
	stm32_gpio_mode(IO_MODE1, INPUT, PULL_UP);
	stm32_gpio_mode(IO_MODE2, INPUT, PULL_UP);
	stm32_gpio_mode(IO_MODE3, INPUT, PULL_UP);

	/* - RS485 ---------------------------------------------------------*/
	stm32_gpio_mode(IO_RS485_RX, ALT_FUNC, PULL_UP);
	stm32_gpio_af(IO_RS485_RX, RS485_USART_AF);

	stm32_gpio_mode(IO_RS485_TX, ALT_FUNC, PUSH_PULL | SPEED_MED);
	stm32_gpio_af(IO_RS485_TX, RS485_USART_AF);

	stm32_gpio_mode(IO_RS485_MODE, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_set(IO_RS485_MODE);

	stm32_gpio_mode(IO_RS485_DBG, INPUT, PULL_UP);

	/* - UART 2 ---------------------------------------------------------*/
	stm32_gpio_mode(IO_UART2_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_af(IO_UART2_TX, UART2_AF);
	stm32_gpio_mode(IO_UART2_RX, ALT_FUNC, 0);
	stm32_gpio_af(IO_UART2_RX, UART2_AF);

	/* - UART 3 ---------------------------------------------------------*/
	stm32_gpio_mode(IO_UART3_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_af(IO_UART3_TX, UART3_AF);
	stm32_gpio_mode(IO_UART3_RX, ALT_FUNC, 0);
	stm32_gpio_af(IO_UART3_RX, UART3_AF);

	/* - UART 4 ---------------------------------------------------------*/
	stm32_gpio_mode(IO_UART4_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_af(IO_UART4_TX, UART4_AF);
	stm32_gpio_mode(IO_UART4_RX, ALT_FUNC, 0);
	stm32_gpio_af(IO_UART4_RX, UART4_AF);

	/* - UART 5 ---------------------------------------------------------*/
	stm32_gpio_mode(IO_UART5_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_af(IO_UART5_TX, UART5_AF);
	stm32_gpio_mode(IO_UART5_RX, ALT_FUNC, 0);
	stm32_gpio_af(IO_UART5_RX, UART5_AF);

	/* - UART 6 ---------------------------------------------------------*/
	stm32_gpio_mode(IO_UART6_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_af(IO_UART6_TX, UART6_AF);
	stm32_gpio_mode(IO_UART6_RX, ALT_FUNC, 0);
	stm32_gpio_af(IO_UART6_RX, UART6_AF);

	/* - Serial Flash ----------------------------------------------------*/
	stm32_gpio_mode(IO_SFLASH_CS, OUTPUT, SPEED_HIGH);
	stm32_gpio_set(IO_SFLASH_CS);

	stm32_gpio_mode(IO_SFLASH_SCK, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_af(IO_SFLASH_SCK, SFLASH_SPI_AF);

	stm32_gpio_mode(IO_SFLASH_MISO, ALT_FUNC, SPEED_HIGH);
	stm32_gpio_af(IO_SFLASH_MISO, SFLASH_SPI_AF);

	stm32_gpio_mode(IO_SFLASH_MOSI, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_af(IO_SFLASH_MOSI, SFLASH_SPI_AF);
}

