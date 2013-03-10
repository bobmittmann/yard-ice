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

void debug_init(void)
{
	int i;

	stm32f_gpio_clock_en(STM32F_GPIOB);
	stm32f_gpio_mode(STM32F_GPIOB, 9, OUTPUT, SPEED_MED);
	stm32f_gpio_clr(STM32F_GPIOB, 9);

	stm32f_gpio_set(STM32F_GPIOB, 9);
	for (i = 0; i < 400000; i++)
		__NOP();
	stm32f_gpio_clr(STM32F_GPIOB, 9);
	for (i = 0; i < 400000; i++)
		__NOP();
	stm32f_gpio_set(STM32F_GPIOB, 9);
	for (i = 0; i < 400000; i++)
		__NOP();
	stm32f_gpio_clr(STM32F_GPIOB, 9);
	for (i = 0; i < 400000; i++)
		__NOP();
	stm32f_gpio_set(STM32F_GPIOB, 9);
	for (i = 0; i < 400000; i++)
		__NOP();
	stm32f_gpio_clr(STM32F_GPIOB, 9);
	for (i = 0; i < 400000; i++)
		__NOP();

}

/* Hardware initialization */

#ifdef STM32F10X

const uint32_t stm32f_ahb_hz = HCLK_HZ;
const uint32_t stm32f_apb1_hz = HCLK_HZ / 2;
const uint32_t stm32f_apb2_hz = HCLK_HZ / 2;

void _init(void)
{
	struct stm32f_rcc * rcc = STM32F_RCC;
	struct stm32f_flash * flash = STM32F_FLASH;
	uint32_t sr;
	uint32_t cr;
	uint32_t cfg;
	int again;
#ifdef CM3_RAM_VECTORS
	struct stm32f_syscfg * syscfg = STM32F_SYSCFG;

	/* remap the SRAM to 0x00000000  */
	syscfg->memrmp = SYSCFG_MEM_MODE_SRAM;
#endif

	/* Make sure we are using the internal oscillator */
	rcc->cfgr = RCC_PPRE2_1 | RCC_PPRE1_1 | RCC_HPRE_1 | RCC_SW_HSI;

	debug_init();

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

	/* F_HSE = 24 MHz
	   PLLCLK = 72 MHz
	   SYSCLK = 72 MHz
	   USBCLK = 48 MHz */
	cfg = RCC_USBPRE | RCC_PLLMUL(6) | RCC_PLLSRC_HSE | RCC_PPRE2_2 | \
		  RCC_PPRE1_2 | RCC_HPRE_1 | RCC_SW_HSE;

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

	/* adjust flash wait states and enable prefetch buffer */
	flash->acr = FLASH_PRFTBE | FLASH_LATENCY(1);

	if (flash->cr & FLASH_LOCK) {
		/* unlock flash write */
		flash->keyr = FLASH_KEY1;
		flash->keyr = FLASH_KEY2;
	}

	/* switch to pll oscillator */
	rcc->cfgr = (cfg & ~RCC_SW) | RCC_SW_PLL;
}

#endif



#ifdef STM32F2X

const uint32_t stm32f_ahb_hz = HCLK_HZ;
const uint32_t stm32f_apb1_hz = HCLK_HZ / 4;
const uint32_t stm32f_apb2_hz = HCLK_HZ / 2;

void _init(void)
{
	struct stm32f_rcc * rcc = STM32F_RCC;
	struct stm32f_flash * flash = STM32F_FLASH;
	uint32_t sr;
	uint32_t cr;
	uint32_t pll;
	uint32_t cfg;
	int again;
#ifdef CM3_RAM_VECTORS
	struct stm32f_syscfg * syscfg = STM32F_SYSCFG;

	/* remap the SRAM to 0x00000000  */
	syscfg->memrmp = SYSCFG_MEM_MODE_SRAM;
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
	/* F_HSE = 24 MHz
	   F_VCO = 480 MHz
	   F_MAIN = 120 MHz
	   F_USB = 48 MHz */
	pll = RCC_PLLQ(10) | 
		RCC_PLLSRC_HSE | 
		RCC_PLLP(4) | 
		RCC_PLLN(240) | RCC_PLLM(12);
	rcc->pllcfgr = pll;

	/* switch to external clock */
	cfg = RCC_MCO2_SYSCLK | RCC_MCO2PRE_2 /* Clock output 2 */
		| RCC_PPRE2_2 /* APB high speed prescaler : 60MHz */
		| RCC_PPRE1_4 /* APB low speed prescaler : 30MHz */
		| RCC_HPRE_1 /* AHB prescaler : 120MHz */ 
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

	/* adjust flash wait states and enable caches */
	flash->acr = FLASH_DCEN | FLASH_ICEN | FLASH_PRFTEN | FLASH_LATENCY(3);

	if (flash->cr & FLASH_LOCK) {
		/* unlock flash write */
		flash->keyr = FLASH_KEY1;
		flash->keyr = FLASH_KEY2;
	}

	/* switch to pll oscillator */
	rcc->cfgr = (cfg & ~RCC_SW) |  RCC_SW_PLL;
}

#endif

