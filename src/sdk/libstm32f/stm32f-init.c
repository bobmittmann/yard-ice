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

/* This constant is used to calibrate the systick timer */
const uint32_t cm3_systick_load_1ms = ((HCLK_HZ / 8) / 1000) - 1;

#define ENABLE_DEBUG_INIT 0

#if ENABLE_DEBUG_INIT
void debug_init(void)
{
	int i;

	stm32_gpio_clock_en(STM32F_GPIOB);
	stm32_gpio_mode(STM32F_GPIOB, 9, OUTPUT, SPEED_MED);
	stm32_gpio_clr(STM32F_GPIOB, 9);

	stm32_gpio_set(STM32F_GPIOB, 9);
	for (i = 0; i < 400000; i++)
		__NOP();
	stm32_gpio_clr(STM32F_GPIOB, 9);
	for (i = 0; i < 400000; i++)
		__NOP();
	stm32_gpio_set(STM32F_GPIOB, 9);
	for (i = 0; i < 400000; i++)
		__NOP();
	stm32_gpio_clr(STM32F_GPIOB, 9);
	for (i = 0; i < 400000; i++)
		__NOP();
	stm32_gpio_set(STM32F_GPIOB, 9);
	for (i = 0; i < 400000; i++)
		__NOP();
	stm32_gpio_clr(STM32F_GPIOB, 9);
	for (i = 0; i < 400000; i++)
		__NOP();

}
#endif


/* Hardware initialization */

#if defined(STM32F1X) || defined(STM32F3X)

const uint32_t stm32f_ahb_hz = HCLK_HZ;

#if defined(STM32F10X) || defined(STM32F30X)

const uint32_t stm32f_apb1_hz = HCLK_HZ / 2;
const uint32_t stm32f_apb2_hz = HCLK_HZ;
const uint32_t stm32f_tim1_hz = HCLK_HZ;
const uint32_t stm32f_tim2_hz = HCLK_HZ;

#elif defined(STM32F100)

const uint32_t stm32f_apb1_hz = HCLK_HZ;
const uint32_t stm32f_apb2_hz = HCLK_HZ;
const uint32_t stm32f_tim1_hz = HCLK_HZ;
const uint32_t stm32f_tim2_hz = HCLK_HZ;

#endif

void _init(void)
{
	struct stm32f_rcc * rcc = STM32F_RCC;
	struct stm32f_flash * flash = STM32F_FLASH;
	uint32_t sr;
	uint32_t cr;
	uint32_t cfg;
	uint32_t ws;
	int again;
#ifdef CM3_RAM_VECTORS
	struct stm32f_syscfg * syscfg = STM32F_SYSCFG;

	/* remap the SRAM to 0x00000000  */
	syscfg->memrmp = SYSCFG_MEM_MODE_SRAM;
#endif

	/* Make sure we are using the internal oscillator */
	rcc->cfgr = RCC_PPRE2_1 | RCC_PPRE1_1 | RCC_HPRE_1 | RCC_SW_HSI;

#if ENABLE_DEBUG_INIT
	debug_init();
#endif

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
}

#endif


#if defined(STM32F2X) || defined(STM32F4X)

const uint32_t stm32f_ahb_hz = HCLK_HZ;
const uint32_t stm32f_apb1_hz = HCLK_HZ / 4;
const uint32_t stm32f_tim1_hz = HCLK_HZ / 2;
const uint32_t stm32f_apb2_hz = HCLK_HZ / 2;
const uint32_t stm32f_tim2_hz = HCLK_HZ;

void _init(void)
{
	struct stm32_rcc * rcc = STM32_RCC;
	struct stm32f_flash * flash = STM32F_FLASH;
	uint32_t sr;
	uint32_t cr;
	uint32_t pll;
	uint32_t cfg;
	uint32_t ws;
	int again;
#ifdef CM3_RAM_VECTORS
	struct stm32f_syscfg * syscfg = STM32F_SYSCFG;

	/* remap the SRAM to 0x00000000  */
	syscfg->memrmp = SYSCFG_MEM_MODE_SRAM;
#endif

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

#if (HCLK_HZ == 168000000)

#if (HSE_HZ == 12000000)
	/* F_HSE = 12 MHz
	   F_VCO = 336 MHz (F_HSE * 28)
	   F_MAIN = 168 MHz (F_VCO / 2)
	   F_USB = 48 MHz (F_VCO / 7)*/
	pll = RCC_PLLQ(7) | 
		RCC_PLLSRC_HSE | 
		RCC_PLLP(2) | 
		RCC_PLLN(112) | RCC_PLLM(4);
#elif (HSE_HZ == 8000000)
	/* F_HSE = 8 MHz
	   F_VCO = 336 MHz (F_HSE * 42)
	   F_MAIN = 168 MHz (F_VCO / 2)
	   F_USB = 48 MHz (F_VCO / 7)*/
	pll = RCC_PLLQ(7) | 
		RCC_PLLSRC_HSE | 
		RCC_PLLP(2) | 
		RCC_PLLN(168) | RCC_PLLM(4);
#else
#error "HSE_HZ invalid!"
#endif

#elif (HCLK_HZ == 120000000)

#if (HSE_HZ == 24000000)
	/* F_HSE = 24 MHz
	   F_VCO = 240 MHz
	   F_MAIN = 120 MHz
	   F_USB = 48 MHz */
	pll = RCC_PLLQ(5) | 
		RCC_PLLSRC_HSE | 
		RCC_PLLP(2) | 
		RCC_PLLN(120) | RCC_PLLM(12);
#elif (HSE_HZ == 12000000)
	/* F_HSE = 24 MHz
	   F_VCO = 240 MHz
	   F_MAIN = 120 MHz
	   F_USB = 48 MHz */
	pll = RCC_PLLQ(5) | 
		RCC_PLLSRC_HSE | 
		RCC_PLLP(2) | 
		RCC_PLLN(240) | RCC_PLLM(12);
#else
//#error "HSE_HZ invalid!"
#endif

#endif

	rcc->pllcfgr = pll;

	/* switch to external clock */
	cfg = RCC_MCO2_SYSCLK | RCC_MCO2PRE_2 /* Clock output 2 */
		| RCC_PPRE2_2 /* APB high speed prescaler : 60|84MHz */
		| RCC_PPRE1_4 /* APB low speed prescaler : 30|42MHz */
		| RCC_HPRE_1 /* AHB prescaler : 120|168MHz */ 
		| RCC_SW_HSE;
	
	rcc->cfgr = cfg;

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
}

#endif


