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

#ifdef STM32F2X

gpio_io_t fsmc_io[] = {
	GPIO(PD, 14), /* D0 */
	GPIO(PD, 15), /* D1 */
	GPIO(PD, 0),  /* D2 */
	GPIO(PD, 1),  /* D3 */
	GPIO(PE, 7),  /* D4 */
	GPIO(PE, 8),  /* D5 */
	GPIO(PE, 9),  /* D6 */
	GPIO(PE, 10), /* D7 */
	GPIO(PE, 11), /* D8 */
	GPIO(PE, 12), /* D9 */
	GPIO(PE, 13), /* D10 */
	GPIO(PE, 14), /* D11 */
	GPIO(PE, 15), /* D12 */
	GPIO(PD, 8),  /* D13 */
	GPIO(PD, 9),  /* D14 */
	GPIO(PD, 10), /* D15 */
	GPIO(PD, 3), /* CLK */
	GPIO(PD, 4), /* NOE */
	GPIO(PD, 5), /* NWE */
	GPIO(PD, 7), /* NE1 */
//	GPIO(PD, 6), /* NWAIT */
	GPIO(PB, 7), /* NL */
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

	/* Flexible static memory controller module clock enable */
	rcc->ahb3enr |= RCC_FSMCEN;

	/* Configur IO pins */
	stm32f_gpio_clock_en(STM32F_GPIO(PD));
	stm32f_gpio_clock_en(STM32F_GPIO(PE));

	for (i = 0; i < sizeof(fsmc_io) / sizeof(gpio_io_t); i++) {
		io = fsmc_io[i];
		stm32f_gpio_mode(STM32F_GPIO(io.port), io.pin, 
						 ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32f_gpio_af(STM32F_GPIO(io.port), io.pin, GPIO_AF12);
	}

	stm32f_gpio_mode(STM32F_GPIO(PD), 6, INPUT, PUSH_PULL | SPEED_HIGH);

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

