/* 
 * File:	stm32f-init.c
 * Author:  Robinson Mittmann (bobmittmann@gmail.com)
 * Target:  jtagtool3
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

/* Set default values for system clocks */

#if defined(STM32L1X)

  #ifndef HSE_HZ
    #define HSE_HZ 8000000
  #endif

  #ifndef HCLK_HZ
    #define HCLK_HZ 8000000
  #endif

/* This constant is used to calibrate the systick timer */
const uint32_t cm3_systick_load_1ms = ((HCLK_HZ / 8) / 1000) - 1;

/* Hardware initialization */
const uint32_t stm32f_apb1_hz = HCLK_HZ;
const uint32_t stm32f_apb2_hz = HCLK_HZ;
const uint32_t stm32f_tim1_hz = HCLK_HZ;
const uint32_t stm32f_tim2_hz = HCLK_HZ;

#endif

void _init(void)
{
	struct stm32_rcc * rcc = STM32_RCC;
//	struct stm32f_flash * flash = STM32F_FLASH;
	uint32_t cr;
	int again;

	/* Make sure we are using the internal oscillator */
	rcc->cfgr = RCC_PPRE2_1 | RCC_PPRE1_1 | RCC_HPRE_1 | RCC_SW_HSI;

	/* Enable external oscillator */
	cr = rcc->cr;
	cr |= RCC_HSEON;
	rcc->cr = cr;;

	for (again = 8192; ; again--) {
		cr = rcc->cr;
		if (cr & RCC_HSERDY)
			break;
		if (again == 0) {
			/* external clock startup fail! */
			return;
		}
	}

#if 0
	for (again = 4096; ; again--) {
		sr = flash->sr;
		if ((sr & FLASH_BSY) == 0)
			break;
		if (again == 0) {
			/* flash not ready! */
			return;
		}
	}
#endif

	/* select HSE as system clock */
	rcc->cfgr = RCC_PPRE2_1 | RCC_PPRE1_1 | RCC_HPRE_1 | RCC_SW_HSE;
}

