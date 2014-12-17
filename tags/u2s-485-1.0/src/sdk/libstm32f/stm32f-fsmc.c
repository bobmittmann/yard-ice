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
 * @file stm32f-fsmc.c
 * @brief STM32F
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <sys/stm32f.h>

#include <sys/dcclog.h>

#if defined(STM32F2X) || defined(STM32F4X)

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

const gpio_io_t fsmc_io[] = {
	GPIO(GPIOD, 14), /* D0 */
	GPIO(GPIOD, 15), /* D1 */
	GPIO(GPIOD, 0),  /* D2 */
	GPIO(GPIOD, 1),  /* D3 */
	GPIO(GPIOE, 7),  /* D4 */
	GPIO(GPIOE, 8),  /* D5 */
	GPIO(GPIOE, 9),  /* D6 */
	GPIO(GPIOE, 10), /* D7 */
	GPIO(GPIOE, 11), /* D8 */
	GPIO(GPIOE, 12), /* D9 */
	GPIO(GPIOE, 13), /* D10 */
	GPIO(GPIOE, 14), /* D11 */
	GPIO(GPIOE, 15), /* D12 */
	GPIO(GPIOD, 8),  /* D13 */
	GPIO(GPIOD, 9),  /* D14 */
	GPIO(GPIOD, 10), /* D15 */
	GPIO(GPIOD, 3), /* CLK */
	GPIO(GPIOD, 4), /* NOE */
	GPIO(GPIOD, 5), /* NWE */
	GPIO(GPIOD, 7), /* NE1 */
//	GPIO(GPIOD, 6), /* NWAIT */
	GPIO(GPIOB, 7), /* NL */
};

void stm32f_fsmc_speed(int div)
{
	struct stm32f_fsmc * fsmc = STM32F_FSMC;

	fsmc->btr1 = FSMC_ACCMOD_A | FSMC_DATLAT_SET(0) |
		FSMC_CLKDIV_SET(div) | FSMC_BUSTURN_SET(0) |
		FSMC_DATAST_SET(0) | FSMC_ADDHDL_SET(0) |
		FSMC_ADDSET_SET(0);
}

void stm32f_fsmc_init(void)
{
	struct stm32f_fsmc * fsmc = STM32F_FSMC;
	int i;

	DCC_LOG(LOG_TRACE, ".");

	/* Flexible static memory controller module clock enable */
	stm32_clk_enable(STM32_RCC, STM32_CLK_FSMC);

	/* Configure IO pins */
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOD);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOE);

	for (i = 0; i < sizeof(fsmc_io) / sizeof(gpio_io_t); i++) {
		gpio_io_t io = fsmc_io[i];
		stm32_gpio_mode(STM32_GPIO(io.port), io.pin, 
						 ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_af(STM32_GPIO(io.port), io.pin, GPIO_AF12);
	}

	stm32_gpio_mode(STM32_GPIO(GPIOD), 6, INPUT, PUSH_PULL | SPEED_HIGH);

	fsmc->bcr1 = FSMC_CBURSTRW |	
		FSMC_WREN | 
		FSMC_BURSTEN | 
		FSMC_MWID_16 | 
		FSMC_MTYP_PSRAM | 
		FSMC_MUXEN | /* Address/Data multiplexed */
		FSMC_MBKEN |
		FSMC_WAITEN |
		FSMC_WAITPOL;
	
	fsmc->btr1 = FSMC_ACCMOD_A | FSMC_DATLAT_SET(0) |
		FSMC_CLKDIV_SET(3) | FSMC_BUSTURN_SET(0) |
		FSMC_DATAST_SET(0) | FSMC_ADDHDL_SET(0) |
		FSMC_ADDSET_SET(0);

	fsmc->bwtr1 = FSMC_ACCMOD_A | FSMC_DATLAT_SET(0) |
		FSMC_CLKDIV_SET(1) | FSMC_BUSTURN_SET(0) |
		FSMC_DATAST_SET(0) | FSMC_ADDHDL_SET(0) |
		FSMC_ADDSET_SET(0);
}

#endif

