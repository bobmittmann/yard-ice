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

#ifdef CONFIG_H
#include "config.h"
#endif

#ifndef STM32_ENABLE_HSI
#define STM32_ENABLE_HSI 1
#endif

#ifndef STM32_ENABLE_HSE
#define STM32_ENABLE_HSE 0
#endif

#ifndef STM32_ENABLE_PLL
#define STM32_ENABLE_PLL 0
#endif


/* Set default values for system clocks */
#if defined(STM32L1X)

#ifndef HSE_HZ
  #define HSE_HZ 8000000
#endif

#ifndef HSI_HZ
  #define HSI_HZ 16000000
#endif

#ifndef PLL_HZ
  #define PLL_HZ 32000000
#endif

#ifndef HCLK_HZ
  #if STM32_ENABLE_PLL
    #define HCLK_HZ PLL_HZ
  #elif STM32_ENABLE_HSI
    #define HCLK_HZ HSI_HZ
  #elif STM32_ENABLE_HSE
    #define HCLK_HZ HSE_HZ
  #else
	#error "HCLK_HZ undefined!"
  #endif
#endif


#ifndef STM32_APB1_HZ
  #if STM32_ENABLE_PLL
    #define STM32_APB1_HZ (HCLK_HZ / 2)
  #else 
    #define STM32_APB1_HZ HCLK_HZ
  #endif
#endif

#ifndef STM32_APB2_HZ
  #if STM32_ENABLE_PLL
    #define STM32_APB2_HZ (HCLK_HZ / 2)
  #else 
    #define STM32_APB2_HZ HCLK_HZ
  #endif
#endif

#if STM32_APB1_HZ == HCLK_HZ
#define STM32_TIM1_HZ STM32_APB1_HZ
#else
#define STM32_TIM1_HZ (2 * STM32_APB1_HZ)
#endif

#if STM32_APB2_HZ == HCLK_HZ
#define STM32_TIM2_HZ STM32_APB2_HZ
#else
#define STM32_TIM2_HZ (2 * STM32_APB2_HZ)
#endif

/* This constant is used to calibrate the systick timer */
const uint32_t cm3_systick_load_1ms = ((HCLK_HZ / 8) / 1000) - 1;

/* Hardware initialization */
const uint32_t stm32f_apb1_hz = STM32_APB1_HZ;
const uint32_t stm32f_apb2_hz = STM32_APB2_HZ;
const uint32_t stm32f_tim1_hz = STM32_TIM1_HZ;
const uint32_t stm32f_tim2_hz = STM32_TIM2_HZ;

#endif

void _init(void)
{
	struct stm32_rcc * rcc = STM32_RCC;
	struct stm32_pwr * pwr = STM32_PWR;
	struct stm32_flash * flash = STM32_FLASH;
	uint32_t rcc_cr;
	uint32_t csr;
	uint32_t acr;
	int again;
#if STM32_ENABLE_PLL
	uint32_t cfg;
#endif

	rcc->cr = rcc_cr = RCC_MSION;
	rcc->cfgr = RCC_PPRE2_1 | RCC_PPRE1_1 | RCC_HPRE_1 | RCC_SW_MSI;

	/*******************************************************************
	 * Wait for voltage rises to 3.1 volts enabling full power
	 * operation.
	 *******************************************************************/
	pwr->cr = PWR_VOS_1_5V | PWR_PLS_3_1V | PWR_PVDE;
	for (again = 8192; ; again--) {
		if (((csr = pwr->csr) & PWR_PVDO) == 0)
			break;
		if (again == 0)
			halt(); /* voltage regulator failed! */
	}

	/*******************************************************************
	 * Adjust core voltage regulator
	 *******************************************************************/
	pwr->cr = PWR_VOS_1_8V;
	/* wait for voltage to stabilize */
	for (again = 8192; ; again--) {
		if (((csr = pwr->csr) & PWR_VOSF) == 0)
			break;
		if (again == 0)
			halt(); /* voltage regulator failed! */
	}

#if STM32_ENABLE_HSI
	/*******************************************************************
	 * Enable internal oscillator 
	 *******************************************************************/
	rcc_cr |= RCC_HSION;
	rcc->cr = rcc_cr;

	for (again = 8192; ; again--) {
		if ((rcc_cr = rcc->cr) & RCC_HSIRDY)
			break;
		if (again == 0)
			halt(); /* internal clock startup failed! */
	}
#endif

#if STM32_ENABLE_HSE
	/*******************************************************************
	 * Enable external oscillator 
	 *******************************************************************/
	rcc_cr |= RCC_HSEON;
	rcc->cr = rcc_cr;

	for (again = 8192; ; again--) {
		if ((rcc_cr = rcc->cr) & RCC_HSERDY)
			break;
		if (again == 0)
			halt(); /* external clock startup failed! */
	}
#endif


#if STM32_ENABLE_PLL
	/*******************************************************************
	 * Configure PLL
	 *******************************************************************/
#if STM32_ENABLE_HSI
	/* F_HSI = 16 MHz
	   VCOCLK = 64 MHz
	   PLLCLK = 32 MHz
	   SYSCLK = 32 MHz
	   PCLK1 = 8 MHz
	   PCLK2 = 8 MHz */
	cfg = RCC_PLLDIV_2 | RCC_PLLMUL_4 | RCC_PLLSRC_HSI | 
		RCC_PPRE2_1 | RCC_PPRE1_1 | RCC_HPRE_2 | RCC_SW_MSI;
#else

	/* F_HSE = 8 MHz
	   VCOCLK = 32 MHz
	   PLLCLK = 64 MHz
	   SYSCLK = 32 MHz
	   PCLK1 = 8 MHz
	   PCLK2 = 8 MHz */
	cfg = RCC_PLLDIV_2 | RCC_PLLMUL_8 | RCC_PLLSRC_HSE | 
		RCC_PPRE2_2 | RCC_PPRE1_2 | RCC_HPRE_2 | RCC_SW_MSI;
#endif
	rcc->cfgr = cfg;

	/* enable PLL */
	rcc_cr |= RCC_PLLON;
	rcc->cr = rcc_cr;

	for (again = 8192; ; again--) {
		if ((rcc_cr = rcc->cr) & RCC_PLLRDY)
			break;
		if (again == 0)
			halt(); /* PLL lock failed! */
	}
#endif /* STM32_ENABLE_PLL */

	/*******************************************************************
	 * Configure flash access and wait states 
	 *******************************************************************/
	flash->acr = FLASH_ACC64;
	
	for (again = 8192; ; again--) {
		acr = flash->acr;
		if (acr & FLASH_ACC64)
			break;
		if (again == 0) {
			/* PLL lock fail */
			halt();
		}
	}
	
#if STM32_ENABLE_PLL
	flash->acr = FLASH_ACC64 | FLASH_PRFTEN | FLASH_LATENCY;
#else
//	flash->acr = FLASH_ACC64 | FLASH_PRFTEN;
	flash->acr = FLASH_ACC64 | FLASH_PRFTEN | FLASH_LATENCY;
#endif

	for (again = 8192; ; again--) {
		acr = flash->acr;
		if (acr & FLASH_LATENCY)
			break;
		if (again == 0) {
			/* FLASH WS configuration failed */
			halt();
		}
	}

#if STM32_ENABLE_PLL
	/* switch to PLL oscillator */
	rcc->cfgr = (cfg & ~RCC_SW) | RCC_SW_PLL;
	rcc->cr = rcc_cr & ~RCC_MSION;
#elif STM32_ENABLE_HSI
	/* select HSI as system clock */
	rcc->cfgr = RCC_PPRE2_1 | RCC_PPRE1_1 | RCC_HPRE_1 | RCC_SW_HSI;
	rcc->cr = rcc_cr & ~RCC_MSION;
#elif STM32_ENABLE_HSE
	/* select HSE as system clock */
	rcc->cfgr = RCC_PPRE2_1 | RCC_PPRE1_1 | RCC_HPRE_1 | RCC_SW_HSE;
	rcc->cr = rcc_cr & ~RCC_MSION;
#endif

#ifdef CM3_RAM_VECTORS
	/* Remap the VECTOR table to SRAM 0x20000000  */
	CM3_SCB->vtor = 0x20000000; /* Vector Table Offset */
#endif
}

