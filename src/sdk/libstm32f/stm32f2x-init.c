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

#if defined(STM32F429)

  #ifndef HSE_HZ
    #define HSE_HZ 25000000
  #endif

  #ifndef HCLK_HZ
    #define HCLK_HZ 168000000
  #endif

#elif defined(STM32F4X)

  #ifndef HSE_HZ
    #define HSE_HZ 12000000
  #endif

  #ifndef HCLK_HZ
    #define HCLK_HZ 168000000
  #endif

#elif defined(STM32F2X)

  #ifndef HSE_HZ
    #define HSE_HZ 24000000
  #endif

  #ifndef HCLK_HZ
    #define HCLK_HZ 120000000
  #endif

#else

#endif

#if defined(STM32F2X) || defined(STM32F4X)

#if (HCLK_HZ == 168000000)
#if (HSE_HZ == 25000000)
	/* F_HSE = 25 MHz
	   F_VCO = 335.9375 MHz (F_HSE * 215 / 16)
	   F_MAIN = 167.96875 MHz (F_VCO / 2)
	   F_USB = 47.991 MHz (F_VCO / 7) */
  #define PLLQ 7
  #define PLLP 2
  #define PLLN 215
  #define PLLM 16
#elif (HSE_HZ == 16000000)
	/* F_HSE = 16 MHz
	   F_VCO = 336 MHz (F_HSE * 42)
	   F_MAIN = 168 MHz (F_VCO / 2)
	   F_USB = 48 MHz (F_VCO / 7)*/
  #define PLLQ 7
  #define PLLP 2
  #define PLLN 168
  #define PLLM 8
#elif (HSE_HZ == 12000000)
	/* F_HSE = 12 MHz
	   F_VCO = 336 MHz (F_HSE * 28)
	   F_MAIN = 168 MHz (F_VCO / 2)
	   F_USB = 48 MHz (F_VCO / 7)*/
  #define PLLQ 7
  #define PLLP 2
  #define PLLN 112
  #define PLLM 4
#elif (HSE_HZ == 8000000)
	/* F_HSE = 8 MHz
	   F_VCO = 336 MHz (F_HSE * 42)
	   F_MAIN = 168 MHz (F_VCO / 2)
	   F_USB = 48 MHz (F_VCO / 7)*/
  #define PLLQ 7
  #define PLLP 2
  #define PLLN 168
  #define PLLM 4
#else
#error "HSE_HZ invalid!"
#endif

#elif (HCLK_HZ == 120000000)
#if (HSE_HZ == 24000000)
	/* F_HSE = 24 MHz
	   F_VCO = 240 MHz
	   F_MAIN = 120 MHz
	   F_USB = 48 MHz */
  #define PLLQ 5
  #define PLLP 2
  #define PLLN 120
  #define PLLM 12
#elif (HSE_HZ == 12000000)
	/* F_HSE = 24 MHz
	   F_VCO = 240 MHz
	   F_MAIN = 120 MHz
	   F_USB = 48 MHz */
  #define PLLQ 5
  #define PLLP 2
  #define PLLN 240
  #define PLLM 12
#else
#error "HSE_HZ invalid!"
#endif

#elif (HCLK_HZ == 192000000)
#if (HSE_HZ == 25000000)
	/* F_HSE = 25 MHz
	   F_VCO = 383.9286 MHz (F_HSE * 215 / 14)
	   F_MAIN = 191.964 MHz (F_VCO / 2)
	   F_USB = 47.991 MHz (F_VCO / 7) */
  #define PLLQ 8
  #define PLLP 2
  #define PLLN 215
  #define PLLM 14
#else
#error "HSE_HZ invalid!"
#endif

#elif (HCLK_HZ == 180000000)
#if (HSE_HZ == 25000000)
	/* F_HSE = 25 MHz
	   F_VCO = 360 MHz (F_HSE * 216 / 15)
	   F_MAIN = 180 MHz (F_VCO / 2)
	   F_USB = 45 MHz (F_VCO / 8) */
  #define PLLQ 8
  #define PLLP 2
  #define PLLN 216
  #define PLLM 15
#else
#error "HSE_HZ invalid!"
#endif

#endif

#define VCO_HZ ((HCLK_HZ) * (PLLP))
#define USB_HZ 48000000

/* Validate the PLL configuration */
#if ((HSE_HZ * PLLN) / PLLM) > ((VCO_HZ) + (VCO_HZ) / 1000)
#error "invalid PLL configuration (err > 0.1 %)!"
#endif

#if ((HSE_HZ * PLLN) / PLLM) < ((VCO_HZ) - (VCO_HZ) / 1000)
#error "invalid PLL configuration (err < -0.1 %)!"
#endif

#if (VCO_HZ / PLLQ) > ((USB_HZ) + (USB_HZ) / 1000) 
#error "invalid PLL configuration (err > 0.1 %)!"
#endif

#if (VCO_HZ / PLLQ) < ((USB_HZ) - (USB_HZ) / 1000) 
#error "invalid PLL configuration (err < -0.1 %)!"
#endif

#if (PLLN > 432)
#error "invalid PLLN!"
#endif

#if (PLLM > 63)
#error "invalid PLLM!"
#endif

#define __VCO_HZ (((uint64_t)HSE_HZ * PLLN) / PLLM)
#define __HCLK_HZ (__VCO_HZ / PLLP)

const uint32_t stm32f_ahb_hz  = __HCLK_HZ;
const uint32_t stm32f_apb1_hz = __HCLK_HZ / 4;
const uint32_t stm32f_tim1_hz = __HCLK_HZ / 2;
const uint32_t stm32f_apb2_hz = __HCLK_HZ / 2;
const uint32_t stm32f_tim2_hz = __HCLK_HZ;

#ifndef THINKAPP

/* This constant is used to calibrate the systick timer */
const uint32_t cm3_systick_load_1ms = ((__HCLK_HZ / 8) / 1000) - 1;

const uint32_t sysclk_hz[] = {
	[SYSCLK_STM32_AHB] = __HCLK_HZ,
	[SYSCLK_STM32_APB1] = __HCLK_HZ / 4,
	[SYSCLK_STM32_APB2] = __HCLK_HZ / 2,
	[SYSCLK_STM32_TIM1] = __HCLK_HZ / 2,
	[SYSCLK_STM32_TIM2] = __HCLK_HZ,
};

void _init(void)
{
	struct stm32_rcc * rcc = STM32_RCC;
	struct stm32_flash * flash = STM32_FLASH;
	uint32_t sr;
	uint32_t cr;
	uint32_t pll;
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

	pll = RCC_PLLQ(PLLQ) | RCC_PLLSRC_HSE | RCC_PLLP(PLLP) | 
		RCC_PLLN(PLLN) | RCC_PLLM(PLLM);

	rcc->pllcfgr = pll;

	/* enable PLL */
	cr |= RCC_PLLON;
	rcc->cr = cr;;

	for (again = 8192; ; again--) {
		cr = rcc->cr;
		if (cr & RCC_PLLRDY)
			break;
		if (again == 0) {
			/* PLL lock fail */
			return;
		}
	}

	/* switch to external clock */
	cfg = RCC_MCO2_SYSCLK | RCC_MCO2PRE_2 /* Clock output 2 */
		| RCC_PPRE2_2 /* APB high speed prescaler : 60|84MHz */
		| RCC_PPRE1_4 /* APB low speed prescaler : 30|42MHz */
		| RCC_HPRE_1 /* AHB prescaler : 120|168MHz */ 
		| RCC_SW_HSE;
	
	rcc->cfgr = cfg;

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

	/* adjust flash wait states and enable caches */
	flash->acr = FLASH_DCEN | FLASH_ICEN | FLASH_PRFTEN | FLASH_LATENCY(ws);

	if (flash->cr & FLASH_LOCK) {
		/* unlock flash write */
		flash->keyr = FLASH_KEY1;
		flash->keyr = FLASH_KEY2;
	}

	/* switch to pll oscillator */
	rcc->cfgr = (cfg & ~RCC_SW) | RCC_SW_PLL;

#ifdef CM3_RAM_VECTORS
	/* Remap the VECTOR table to SRAM 0x20000000  */
	CM3_SCB->vtor = 0x20000000; /* Vector Table Offset */
#endif
}

#endif /* THINKAPP */

#endif /* defined(STM32F2X) || defined(STM32F4X) */


