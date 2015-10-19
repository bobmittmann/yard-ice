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

#if defined(STM32F100)

  #ifndef HSE_HZ
    #define HSE_HZ 12000000
  #endif

  #ifndef HCLK_HZ
    #define HCLK_HZ 24000000
  #endif

#elif defined(STM32F10X)

  #ifndef HSE_HZ
    #define HSE_HZ 12000000
  #endif

  #ifndef HCLK_HZ
    #define HCLK_HZ 72000000
  #endif

#elif defined(STM32F30X)

  #ifndef HSE_HZ
    #define HSE_HZ 8000000
  #endif

  #ifndef HCLK_HZ
    #define HCLK_HZ 72000000
  #endif

#endif

/* Hardware initialization */
#if defined(STM32F1X) || defined(STM32F3X)

/* This constant is used to calibrate the systick timer */
const uint32_t cm3_systick_load_1ms = ((HCLK_HZ / 8) / 1000) - 1;

const uint32_t stm32f_ahb_hz = HCLK_HZ;

#if defined(STM32F10X) || defined(STM32F30X)

const uint32_t stm32f_apb1_hz = HCLK_HZ / 2;
const uint32_t stm32f_apb2_hz = HCLK_HZ;
const uint32_t stm32f_tim1_hz = HCLK_HZ;
const uint32_t stm32f_tim2_hz = HCLK_HZ;
const uint32_t stm32f_hsi_hz = 8000000;

#elif defined(STM32F100)

const uint32_t stm32f_apb1_hz = HCLK_HZ;
const uint32_t stm32f_apb2_hz = HCLK_HZ;
const uint32_t stm32f_tim1_hz = HCLK_HZ;
const uint32_t stm32f_tim2_hz = HCLK_HZ;

#endif

#ifndef THINKAPP

#if defined(STM32F10X) || defined(STM32F30X)

const uint32_t sysclk_hz[] = {
	[SYSCLK_STM32_AHB] = HCLK_HZ,
	[SYSCLK_STM32_APB1] = HCLK_HZ / 2,
	[SYSCLK_STM32_APB2] = HCLK_HZ,
	[SYSCLK_STM32_TIM1] = HCLK_HZ,
	[SYSCLK_STM32_TIM2] = HCLK_HZ,
	[SYSCLK_STM32_HSI] = 8000000,
	[SYSCLK_STM32_HSE] = HSE_HZ,
	[SYSCLK_STM32_LSI] = 40000,
	[SYSCLK_STM32_LSE] = 32768
};

#elif defined(STM32F100)

const uint32_t sysclk_hz[] = {
	[SYSCLK_STM32_AHB] = HCLK_HZ,
	[SYSCLK_STM32_APB1] = HCLK_HZ,
	[SYSCLK_STM32_APB2] = HCLK_HZ,
	[SYSCLK_STM32_TIM1] = HCLK_HZ,
	[SYSCLK_STM32_TIM2] = HCLK_HZ,
};

#endif

void __attribute__((section(".init"))) _init(void)
{
	struct stm32_rcc * rcc = STM32_RCC;
	struct stm32_flash * flash = STM32_FLASH;
	uint32_t sr;
	uint32_t cr;
	uint32_t cfg;
	uint32_t ws;
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

	/* disable PLL */
	cr &= ~RCC_PLLON;
	rcc->cr = cr;

#if defined(STM32F10X) || defined(STM32F30X)
#if (HSE_HZ == 12000000)
	/* F_HSE = 12 MHz
	   PLLCLK = 72 MHz
	   SYSCLK = 72 MHz
	   PCLK1 = 36 MHz
	   PCLK2 = 72 MHz
	   USBCLK = 48 MHz */
	cfg = RCC_USBPRE_1DOT5 | RCC_PLLMUL(6) | RCC_PLLSRC_HSE | 
		RCC_PPRE2_1 | RCC_PPRE1_2 | RCC_HPRE_1 | RCC_SW_HSE;
#elif (HSE_HZ == 8000000)
	/* F_HSE = 8 MHz
	   PLLCLK = 72 MHz
	   SYSCLK = 72 MHz
	   PCLK1 = 36 MHz
	   PCLK2 = 72 MHz
	   USBCLK = 48 MHz */
	cfg = RCC_USBPRE_1DOT5 | RCC_PLLMUL(9) | RCC_PLLSRC_HSE | 
		RCC_PPRE2_1 | RCC_PPRE1_2 | RCC_HPRE_1 | RCC_SW_HSE;
#else
#error "HSE_HZ invalid!"
#endif

#endif

#ifdef STM32F100
	/* F_HSE = 12 MHz
	   PLLCLK = 24 MHz
	   PCLK1 = 24 MHz
	   PCLK2 = 24 MHz
	   SYSCLK = 24 MHz */
	cfg = RCC_PLLMUL(2) | RCC_PLLSRC_HSE | RCC_ADCPRE_2 | 
		RCC_PPRE2_1 | RCC_PPRE1_1 | RCC_HPRE_1 | RCC_SW_HSE;
#endif

	rcc->cfgr = cfg;

	/* enable PLL */
	cr |= RCC_PLLON;
	rcc->cr = cr;

	for (again = 8192; ; again--) {
		cr = rcc->cr;
		if (cr & RCC_PLLRDY)
			break;
		if (again == 0) {
			/* PLL lock fail */
			return;
		}
	}

	for (again = 4096; ; again--) {
		sr = flash->sr;
		if ((sr & FLASH_BSY) == 0)
			break;
		if (again == 0) {
			/* flash not ready! */
			return;
		}
	}

	ws = HCLK_HZ / 30000000;

	/* adjust flash wait states and enable prefetch buffer */
	flash->acr = FLASH_PRFTBE | FLASH_LATENCY(ws);

	if (flash->cr & FLASH_LOCK) {
		/* unlock flash write */
		flash->keyr = FLASH_KEY1;
		flash->keyr = FLASH_KEY2;
	}

	/* switch to pll oscillator */
	/* select PLL as MCO output */
	rcc->cfgr = RCC_MCO_PLL | (cfg & ~RCC_SW) | RCC_SW_PLL;

#ifdef CM3_RAM_VECTORS
	/* Remap the VECTOR table to SRAM 0x20000000  */
	CM3_SCB->vtor = 0x20000000; /* Vector Table Offset */
#endif

}

#endif /* THINKAPP */

#endif /* defined(STM32F1X) || defined(STM32F3X) */

