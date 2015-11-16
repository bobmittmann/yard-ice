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
 * @file stm32f-io.c
 * @brief 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 


#include <stdint.h>
#include <sys/stm32f.h>
#include "board.h"

#define FSMC_D0	   STM32_GPIOD, 14
#define FSMC_D1    STM32_GPIOD, 15
#define FSMC_D2    STM32_GPIOD, 0
#define FSMC_D3    STM32_GPIOD, 1
#define FSMC_D4    STM32_GPIOE, 7
#define FSMC_D5    STM32_GPIOE, 8
#define FSMC_D6    STM32_GPIOE, 9
#define FSMC_D7    STM32_GPIOE, 10
#define FSMC_D8    STM32_GPIOE, 11
#define FSMC_D9    STM32_GPIOE, 12
#define FSMC_D10   STM32_GPIOE, 13
#define FSMC_D11   STM32_GPIOE, 14
#define FSMC_D12   STM32_GPIOE, 15
#define FSMC_D13   STM32_GPIOD, 8
#define FSMC_D14   STM32_GPIOD, 9
#define FSMC_D15   STM32_GPIOD, 10
#define FSMC_CLK   STM32_GPIOD, 3
#define FSMC_NOE   STM32_GPIOD, 4
#define FSMC_NWE   STM32_GPIOD, 5
#define FSMC_NE1   STM32_GPIOD, 7
#define FSMC_NWAIT STM32_GPIOD, 6
#define FSMC_NL    STM32_GPIOB, 7

#define FSMC_IRQ   STM32_GPIOD, 6

void io_init(void)
{
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOA);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOB);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOC);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOD);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOE);

	/* - Relay ------------------------------------------------------------*/
	stm32_gpio_mode(IO_RELAY, OUTPUT, SPEED_LOW);
	stm32_gpio_clr(IO_RELAY);

	/* - External Power ---------------------------------------------------*/
	stm32_gpio_mode(IO_PWR_EN, OUTPUT, SPEED_LOW);
	stm32_gpio_clr(IO_PWR_EN);
	stm32_gpio_mode(IO_PWR_MON, INPUT, SPEED_LOW | PULL_UP);

	/* - Debug UART -------------------------------------------------------*/
	stm32_gpio_mode(IO_UART5_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(IO_UART5_RX, ALT_FUNC, PULL_UP);

	stm32_gpio_af(IO_UART5_RX, GPIO_AF8);
	stm32_gpio_af(IO_UART5_TX, GPIO_AF8);

	/* - FPGA PRogramming ------------------------------------------------*/
	stm32_gpio_mode(IO_N_CONFIG, OUTPUT, SPEED_MED);
	stm32_gpio_set(IO_N_CONFIG);

	stm32_gpio_mode(IO_CONF_DONE, INPUT, SPEED_MED | PULL_UP);
#if 0
	stm32_gpio_mode(IO_N_STATUS, INPUT, SPEED_MED | PULL_UP);
#endif
	stm32_gpio_mode(IO_N_STATUS, ALT_FUNC, PULL_UP | SPEED_MED);
	stm32_gpio_af(IO_N_STATUS, GPIO_AF6);

	stm32_gpio_mode(IO_DCLK, ALT_FUNC, PUSH_PULL | SPEED_MED);
	stm32_gpio_af(IO_DCLK, GPIO_AF6);

	stm32_gpio_mode(IO_DATA0, ALT_FUNC, PUSH_PULL | SPEED_MED);
	stm32_gpio_af(IO_DATA0, GPIO_AF6);

	/* - SDMC Bus ------------------------------------------------*/

	stm32_gpio_mode(FSMC_D0, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(FSMC_D1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(FSMC_D2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(FSMC_D3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(FSMC_D4, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(FSMC_D5, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(FSMC_D6, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(FSMC_D7, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(FSMC_D8, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(FSMC_D9, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(FSMC_D10, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(FSMC_D11, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(FSMC_D12, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(FSMC_D13, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(FSMC_D14, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(FSMC_D15, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(FSMC_CLK, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(FSMC_NOE, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(FSMC_NWE, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(FSMC_NE1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(FSMC_NL, ALT_FUNC, PUSH_PULL | SPEED_HIGH);

	stm32_gpio_af(FSMC_D0, GPIO_AF12);
	stm32_gpio_af(FSMC_D1, GPIO_AF12);
	stm32_gpio_af(FSMC_D2, GPIO_AF12);
	stm32_gpio_af(FSMC_D3, GPIO_AF12);
	stm32_gpio_af(FSMC_D4, GPIO_AF12);
	stm32_gpio_af(FSMC_D5, GPIO_AF12);
	stm32_gpio_af(FSMC_D6, GPIO_AF12);
	stm32_gpio_af(FSMC_D7, GPIO_AF12);
	stm32_gpio_af(FSMC_D8, GPIO_AF12);
	stm32_gpio_af(FSMC_D9, GPIO_AF12);
	stm32_gpio_af(FSMC_D10, GPIO_AF12);
	stm32_gpio_af(FSMC_D11, GPIO_AF12);
	stm32_gpio_af(FSMC_D12, GPIO_AF12);
	stm32_gpio_af(FSMC_D13, GPIO_AF12);
	stm32_gpio_af(FSMC_D14, GPIO_AF12);
	stm32_gpio_af(FSMC_D15, GPIO_AF12);
	stm32_gpio_af(FSMC_CLK, GPIO_AF12);
	stm32_gpio_af(FSMC_NOE, GPIO_AF12);
	stm32_gpio_af(FSMC_NWE, GPIO_AF12);
	stm32_gpio_af(FSMC_NE1, GPIO_AF12);
	stm32_gpio_af(FSMC_NL, GPIO_AF12);

	stm32_gpio_mode(FSMC_IRQ, INPUT, SPEED_HIGH);
}

void relay_on(void)
{
	stm32_gpio_set(IO_RELAY);
}

void relay_off(void)
{
	stm32_gpio_clr(IO_RELAY);
}

int relay_stat(void)
{
	return stm32_gpio_stat(IO_RELAY);
}

void ext_pwr_on(void)
{
	stm32_gpio_set(IO_PWR_EN);
}

void ext_pwr_off(void)
{
	stm32_gpio_clr(IO_PWR_EN);
}

int ext_pwr_stat(void)
{
	return stm32_gpio_stat(IO_PWR_EN);
}

int ext_pwr_mon(void)
{
	return stm32_gpio_stat(IO_PWR_MON) ? 0 : 1;
}

