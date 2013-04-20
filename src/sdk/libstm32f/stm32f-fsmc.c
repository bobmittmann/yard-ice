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

#ifdef STM32F2X

gpio_io_t fsmc_io[] = {
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
	struct stm32f_rcc * rcc = STM32F_RCC;
	gpio_io_t io;
	int i;

	DCC_LOG(LOG_TRACE, ".");

	/* Flexible static memory controller module clock enable */
	rcc->ahb3enr |= RCC_FSMCEN;

	/* Configur IO pins */
	stm32f_gpio_clock_en(STM32F_GPIO(GPIOD));
	stm32f_gpio_clock_en(STM32F_GPIO(GPIOE));

	for (i = 0; i < sizeof(fsmc_io) / sizeof(gpio_io_t); i++) {
		io = fsmc_io[i];
		stm32f_gpio_mode(STM32F_GPIO(io.port), io.pin, 
						 ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32f_gpio_af(STM32F_GPIO(io.port), io.pin, GPIO_AF12);
	}

	stm32f_gpio_mode(STM32F_GPIO(GPIOD), 6, INPUT, PUSH_PULL | SPEED_HIGH);

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

