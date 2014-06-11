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
#include <sys/halt.h>

#ifndef STM32_HSI_AS_HCLK
#define STM32_HSI_AS_HCLK 1
#endif

/* Set default values for system clocks */

#if defined(STM32L1X)

#ifndef HSE_HZ
  #define HSE_HZ 8000000
#endif

#ifndef HSI_HZ
  #define HSI_HZ 16000000
#endif

#ifndef HCLK_HZ
  #if STM32_HSI_AS_HCLK
    #define HCLK_HZ HSI_HZ
  #else
    #define HCLK_HZ HSE_HZ
  #endif
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
	struct stm32_flash * flash = STM32_FLASH;
	uint32_t cr;
	uint32_t acr;
	int again;

	/* Enable external oscillator */
	cr = rcc->cr;

#if STM32_HSI_AS_HCLK
	cr |= RCC_HSION;
	rcc->cr = cr;

	for (again = 8192; ; again--) {
		cr = rcc->cr;
		if (cr & RCC_HSIRDY)
			break;
		if (again == 0) {
			/* internal clock startup fail! */
			halt();
		}
	}

	/* configure flash access and wait states */
	flash->acr = FLASH_ACC64;
	acr = flash->acr;
	
	if ((acr & FLASH_ACC64) == 0) 
		halt();
	
	flash->acr = FLASH_ACC64 | FLASH_PRFTEN | FLASH_LATENCY;


	/* select HSI as system clock */
	rcc->cfgr = RCC_PPRE2_1 | RCC_PPRE1_1 | RCC_HPRE_1 | RCC_SW_HSI;
#else
	cr |= RCC_HSEON;
	rcc->cr = cr;

	for (again = 8192; ; again--) {
		cr = rcc->cr;
		if (cr & RCC_HSERDY)
			break;
		if (again == 0) {
			/* external clock startup fail! */
			return;
		}
	}

	/* select HSE as system clock */
	rcc->cfgr = RCC_PPRE2_1 | RCC_PPRE1_1 | RCC_HPRE_1 | RCC_SW_HSE;
#endif

}

