/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the libstm32f.
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
 * @file stm32l-rcc.h
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __STM32L_RCC_H__
#define __STM32L_RCC_H__

/*-------------------------------------------------------------------------
  Reset and clock control (RCC)
  ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
/* RCC clock control register */ 
#define STM32_RCC_CR 0x00

#define RCC_RTC_PRE (0x3 << 29)
#define RCC_RTC_PRE_HSE_2 (0x0 << 29)
#define RCC_RTC_PRE_HSE_4 (0x1 << 29)
#define RCC_RTC_PRE_HSE_8 (0x2 << 29)
#define RCC_RTC_PRE_HSE_16 (0x3 << 29)
/* RTC/LCD prescaler
   These bits are set and reset by software to obtain a 1 MHz clock from HSE. 
   This prescaler cannot be modified if HSE is enabled (HSEON = 1).
   These bits are reset by power on reset, they keep their value after system 
   reset.
   00: HSE is divided by 2 for RTC/LCD clock
   01: HSE is divided by 4 for RTC/LCD clock
   10: HSE is divided by 8 for RTC/LCD clock
   11: HSE is divided by 16 for RTC/LCD clock */

#define RCC_PLLI2SRDY (1 << 27)
/* PLLI2S clock ready flag
   Set by hardware to indicate that the PLLI2S is locked.
0: PLLI2S unlocked
1: PLLI2S locked */

#define RCC_PLLI2SON (1 << 26)
/* PLLI2S enable
   Set and cleared by software to enable PLLI2S.
   Cleared by hardware when entering Stop or Standby mode.
0: PLLI2S OFF
1: PLLI2S ON */

#define RCC_PLLRDY (1 << 25)
/* Main PLL (PLL) clock ready flag
   Set by hardware to indicate that PLL is locked.
0: PLL unlocked
1: PLL locked */

#define RCC_PLLON (1 << 24)
/* Main PLL (PLL) enable
   Set and cleared by software to enable PLL.
   Cleared by hardware when entering Stop or Standby mode. This bit 
   cannot be reset if PLL clock is used as the system clock.
0: PLL OFF
1: PLL ON */

#define RCC_CSSON (1 << 19)
/* Clock security system enable
   Set and cleared by software to enable the clock security system. 
   When CSSON is set, the clock detector is enabled by hardware when 
   the HSE oscillator is ready, and disabled by
   hardware if an oscillator failure is detected.
0: Clock security system OFF (Clock detector OFF)
1: Clock security system ON (Clock detector ON if HSE oscillator 
is stable, OFF if not) */

#define RCC_HSEBYP (1 << 18)
/* HSE clock bypass
   Set and cleared by software to bypass the oscillator with an 
   external clock. The external clock must be enabled with the HSEON bit, 
   to be used by the device.
   The HSEBYP bit can be written only if the HSE oscillator is disabled.
0: HSE oscillator not bypassed
1: HSE oscillator bypassed with an external clock*/

#define RCC_HSERDY (1 << 17)
/* HSE clock ready flag
   Set by hardware to indicate that the HSE oscillator is stable. After 
   the HSION bit is cleared, HSERDY goes low after 6 HSE oscillator 
   clock cycles.
0: HSE oscillator not ready
1: HSE oscillator ready*/

#define RCC_HSEON (1 << 16)
/* HSE clock enable
   Set and cleared by software.
   Cleared by hardware to stop the HSE oscillator when entering Stop or 
   Standby mode. This bit cannot be reset if the HSE oscillator is used 
   directly or indirectly as the system clock.
0: HSE oscillator OFF
1: HSE oscillator ON*/

#define RCC_MSIRDY (1 << 9)
/* MSI clock ready flag
   This bit is set by hardware to indicate that the MSI oscillator is stable.
0: MSI oscillator not ready
1: MSI oscillator ready  */

#define RCC_MSION (1 << 8)
/* MSI clock enable
   This bit is set and cleared by software.
   Set by hardware to force the MSI oscillator ON when exiting from Stop 
   or Standby mode, or in case of a failure of the HSE oscillator used 
   directly or indirectly as system clock. This bit cannot be cleared if the 
   MSI is used as system clock. */

#define RCC_HSICAL (1 << 8)
/* Internal high-speed clock calibration
   These bits are initialized automatically at startup.*/

#define RCC_HSITRIM (1 << 3)
/* Internal high-speed clock trimming
   These bits provide an additional user-programmable trimming value that is 
   added to the HSICAL[7:0] bits. It can be programmed to adjust to 
   variations in voltage and temperature that influence the frequency of 
   the internal HSI RC. */

#define RCC_HSIRDY (1 << 1)
/* Internal high-speed clock ready flag
   Set by hardware to indicate that the HSI oscillator is stable. 
   After the HSION bit is cleared,
   HSIRDY goes low after 6 HSI clock cycles.
0: HSI oscillator not ready
1: HSI oscillator ready*/

#define RCC_HSION (1 << 0)
/* Internal high-speed clock enable
   Set and cleared by software.
   Set by hardware to force the HSI oscillator ON when leaving the 
   Stop or Standby mode or in case of a failure of the HSE oscillator 
   used directly or indirectly as the system clock. This bit cannot be 
   cleared if the HSI is used directly or indirectly as the system clock.
0: HSI oscillator OFF
1: HSI oscillator ON */



/* ------------------------------------------------------------------------- */
/* RCC clock configuration register */

/* RCC clock configuration register */
#define RCC_CFGR 0x08

#define RCC_MCO_SYSCLK (0x1 << 24)
#define RCC_MCO_HSI    (0x2 << 24)
#define RCC_MCO_MSI    (0x3 << 24)
#define RCC_MCO_HSE    (0x4 << 24)
#define RCC_MCO_PLL    (0x5 << 24)
#define RCC_MCO_LSI    (0x6 << 24)
#define RCC_MCO_LSE    (0x7 << 24)

#define RCC_PLLDIV     (0x3 << 22)
#define RCC_PLLDIV_2   (0x1 << 22)
#define RCC_PLLDIV_3   (0x2 << 22)
#define RCC_PLLDIV_4   (0x3 << 22)

#define RCC_PLLMUL     (0xf << 18)
#define RCC_PLLMUL_3   (0x0 << 18)
#define RCC_PLLMUL_4   (0x1 << 18)
#define RCC_PLLMUL_6   (0x2 << 18)
#define RCC_PLLMUL_8   (0x3 << 18)
#define RCC_PLLMUL_12  (0x4 << 18)
#define RCC_PLLMUL_16  (0x5 << 18)
#define RCC_PLLMUL_24  (0x6 << 18)
#define RCC_PLLMUL_32  (0x7 << 18)
#define RCC_PLLMUL_48  (0x8 << 18)

#define RCC_PLLXTPRE   (1 << 17)

#define RCC_PLLSRC     (1 << 16)
#define RCC_PLLSRC_HSI (0 << 16)
#define RCC_PLLSRC_HSE (1 << 16)

#define RCC_ADCPRE     (0x3 << 14)
#define RCC_ADCPRE_2   (0x0 << 14)
#define RCC_ADCPRE_4   (0x1 << 14)
#define RCC_ADCPRE_6   (0x2 << 14)
#define RCC_ADCPRE_8   (0x3 << 14)

#define RCC_PPRE2      (0x7 << 11)
#define RCC_PPRE2_1    (0x0 << 11)
#define RCC_PPRE2_2    (0x4 << 11)
#define RCC_PPRE2_4    (0x5 << 11)
#define RCC_PPRE2_8    (0x6 << 11)
#define RCC_PPRE2_16   (0x7 << 11)
/* APB high-speed prescaler (APB2) 
   These bits are set and cleared by software to control the division 
   factor of the APB high- speed clock (PCLK2).
0xx: HCLK not divided
100: HCLK divided by 2
101: HCLK divided by 4
110: HCLK divided by 8
111: HCLK divided by 16 */

#define RCC_PPRE1    (0x7 << 8)
#define RCC_PPRE1_1  (0x0 << 8)
#define RCC_PPRE1_2  (0x4 << 8)
#define RCC_PPRE1_4  (0x5 << 8)
#define RCC_PPRE1_8  (0x6 << 8)
#define RCC_PPRE1_16 (0x7 << 8)
/* APB low-speed prescaler (APB1) */

#define RCC_HPRE (0xf << 4)
#define RCC_HPRE_1 (0x0 << 4)
#define RCC_HPRE_2 (0x8 << 4)
#define RCC_HPRE_4 (0x9 << 4)
#define RCC_HPRE_8 (0xa << 4)
#define RCC_HPRE_16 (0xb << 4)
#define RCC_HPRE_64 (0xc << 4)
#define RCC_HPRE_128 (0xd << 4)
#define RCC_HPRE_256 (0xe << 4)
#define RCC_HPRE_512 (0xf << 4)
/* AHB prescaler
   Set and cleared by software to control AHB clock division factor.
Caution: The clocks are divided with the new prescaler factor from 
1 to 16 AHB cycles after HPRE write.
Caution: The AHB clock frequency must be at least 25 MHz when the Ethernet 
is used.
0xxx: system clock not divided
1000: system clock divided by 2
1001: system clock divided by 4
1010: system clock divided by 8
1011: system clock divided by 16
1100: system clock divided by 64
1101: system clock divided by 128
1110: system clock divided by 256
1111: system clock divided by 512 */

#define RCC_SWS (0x3 << 2)
#define RCC_SWS_MSI (0x0 << 2)
#define RCC_SWS_HSI (0x1 << 2)
#define RCC_SWS_HSE (0x2 << 2)
#define RCC_SWS_PLL (0x3 << 2)
/* System clock switch status. Set and cleared by hardware to indicate which 
   clock source is used as the system clock.
00: MSI oscillator used as system clock
01: HSI oscillator used as system clock
10: HSE oscillator used as system clock
11: PLL used as system clock */

#define RCC_SW (0x3 << 0)
#define RCC_SW_MSI (0x0 << 0)
#define RCC_SW_HSI (0x1 << 0)
#define RCC_SW_HSE (0x2 << 0)
#define RCC_SW_PLL (0x3 << 0)

/* System clock switch

These bits are set and cleared by software to select SYSCLK source.
Set by hardware to force MSI selection when leaving Stop and Standby mode or in case of failure of the HSE oscillator used directly or indirectly as system clock (if the Clock Security
System is enabled).
00: MSI oscillator used as system clock
01: HSI oscillator used as system clock
10: HSE oscillator used as system clock
11: PLL used as system clock
 */

/* ------------------------------------------------------------------------- */
/* Clock interrupt register */
#define STM32_RCC_CIR 0x0c

/* ------------------------------------------------------------------------- */
/* AHB peripheral reset register  */
#define STMF32_RCC_AHBRSTR 0x10
/* AHB peripheral clock enable register */
#define STM32_RCC_AHBENR 0x1c
/* AHB peripheral clock enable in low power mode register */
#define STM32_RCC_AHBLPENR 0x28
/* AHB preipheral bit mapping */
#define RCC_FSMC   30
#define RCC_AES    27
#define RCC_DMA2   25
#define RCC_DMA1   24
#define RCC_FLITD  15
#define RCC_CRC    12
#define RCC_GPIOG  7
#define RCC_GPIOF  6
#define RCC_GPIOH  5
#define RCC_GPIOE  4
#define RCC_GPIOD  3
#define RCC_GPIOC  2
#define RCC_GPIOB  1
#define RCC_GPIOA  0

/* ------------------------------------------------------------------------- */
/* APB2 peripheral reset register */
#define STM32_RCC_APB2RSTR 0x14
/* APB2 peripheral clock enable register */
#define STM32_RCC_APB2ENR 0x20
/* APB2 peripheral clock enable in low power mode register */
#define STM32_RCC_APB2LPENR 0x2c
/* APB2 preipheral bit mapping */
#define RCC_USART1 14
#define RCC_SPI1   12
#define RCC_SDIO   11
#define RCC_ADC1   9
#define RCC_TIM11  4
#define RCC_TIM10  3
#define RCC_TIM9   2
#define RCC_SYSCFG 0


/* ------------------------------------------------------------------------- */
/* APB1 peripheral reset register */
#define STM32_RCC_APB1RSTR 0x18
/* APB1 peripheral clock enable register */
#define STM32_RCC_APB1ENR 0x24
/* APB1 peripheral clock enable in low power mode register */
#define STM32_RCC_APB1LPENR 0x30
/* APB1 preipheral bit mapping */
#define RCC_COMP   31
#define RCC_DAC    29 
#define RCC_PWR    28 
#define RCC_USB    23
#define RCC_I2C2   22 
#define RCC_I2C1   21 
#define RCC_UART5  20 
#define RCC_UART4  19 
#define RCC_USART3 18  
#define RCC_USART2 17   
#define RCC_SPI3   15
#define RCC_SPI2   14
#define RCC_WWDG   11 
#define RCC_LCD    9
#define RCC_TIM7   6
#define RCC_TIM6   4
#define RCC_TIM5   3
#define RCC_TIM4   2
#define RCC_TIM3   1
#define RCC_TIM2   0


/* ------------------------------------------------------------------------- */
/* Control/status register (RCC_CSR) */
#define STMF32_RCC_CSR 0x34

/* Bit 31 - Low-power reset flag */
#define RCC_LPWRSTF (1 << 31)
/* Set by hardware when a Low-power management reset occurs.
Cleared by writing to the RMVF bit.
0: No Low-power management reset occurred
1: Low-power management reset occurred
For further information on Low-power management reset, refer to Low-power management
reset. */

/* Bit 30 - Window watchdog reset flag */
#define RCC_WWDGRSTF (1 << 30)
/* Set by hardware when a window watchdog reset occurs.
Cleared by writing to the RMVF bit.
0: No window watchdog reset occurred
1: Window watchdog reset occurred */

/* Bit 29 - Independent window watchdog reset flag */
#define RCC_IWDGRSTF (1 << 29)
/* Set by hardware when an independent watchdog reset from VDD domain occurs.
Cleared by writing to the RMVF bit.
0: No watchdog reset occurred
1: Watchdog reset occurred */

/* Bit 28 - Software reset flag */
#define RCC_SFTRSTF (1 << 28)
/* Set by hardware when a software reset occurs.
Cleared by writing to the RMVF bit.
0: No software reset occurred
1: Software reset occurred */

/* Bit 27 - POR/PDR flag */
#define RCC_PORRSTF (1 << 27)
/* Set by hardware when a POR/PDR occurs.
Cleared by writing to the RMVF bit.
0: No POR/PDR occurred
1: POR/PDR occurred */

/* Bit 26 - PIN reset flag */
#define RCC_PINRSTF (1 << 26)
/* Set by hardware when a reset from the NRST pin occurs.
Cleared by writing to the RMVF bit.
0: No reset from NRST pin occurred
1: Reset from NRST pin occurred */

/* Bit 25 - Option byte loader reset flag */
#define RCC_OBLRSTF (1 << 25)
/* Set by hardware when a reset from the OBL occurs.
Cleared by writing to the RMVF bit.
0: No reset from OBL occurred
1: Reset from OBL occurred */

/* Bit 24 - Remove reset flag */
#define RCC_RMVF (1 << 24)
/* Set by software to clear the reset flags.
0: No effect
1: Clear the reset flags */

/* [23..2] Reserved, must be kept at reset value. */


/* Bit 1 - LSI oscillator ready */
#define RCC_LSIRDY (1 << 1)
/* Set and cleared by hardware to indicate when the LSI oscillator is stable. After the LSION bit
is cleared, LSIRDY goes low after 3 LSI oscillator clock cycles.
0: LSI oscillator not ready
1: LSI oscillator ready */

/* Bit 0 - LSI oscillator enable */
#define RCC_LSION (1 << 0)
/* Set and cleared by software.
0: LSI oscillator OFF
1: LSI oscillator ON */


#define STM32_AHB  0
#define STM32_APB1 1
#define STM32_APB2 2

#define STM32_CLK_FSMC   STM32_AHB, RCC_FSMC
#define STM32_CLK_AES    STM32_AHB, RCC_AES
#define STM32_CLK_DMA2   STM32_AHB, RCC_DMA2
#define STM32_CLK_DMA1   STM32_AHB, RCC_DMA1
#define STM32_CLK_FLITD  STM32_AHB, RCC_FLITD
#define STM32_CLK_CRC    STM32_AHB, RCC_CRC
#define STM32_CLK_GPIOG  STM32_AHB, RCC_GPIOG
#define STM32_CLK_GPIOF  STM32_AHB, RCC_GPIOF
#define STM32_CLK_GPIOH  STM32_AHB, RCC_GPIOH
#define STM32_CLK_GPIOE  STM32_AHB, RCC_GPIOE
#define STM32_CLK_GPIOD  STM32_AHB, RCC_GPIOD
#define STM32_CLK_GPIOC  STM32_AHB, RCC_GPIOC
#define STM32_CLK_GPIOB  STM32_AHB, RCC_GPIOB
#define STM32_CLK_GPIOA  STM32_AHB, RCC_GPIOA

#define STM32_CLK_USART1 STM32_APB2, RCC_USART1
#define STM32_CLK_SPI1   STM32_APB2, RCC_SPI1
#define STM32_CLK_SDIO   STM32_APB2, RCC_SDIO
#define STM32_CLK_ADC1   STM32_APB2, RCC_ADC1
#define STM32_CLK_TIM11  STM32_APB2, RCC_TIM11
#define STM32_CLK_TIM10  STM32_APB2, RCC_TIM10
#define STM32_CLK_TIM9   STM32_APB2, RCC_TIM9
#define STM32_CLK_SYSCFG STM32_APB2, RCC_SYSCFG

#define STM32_CLK_COMP   STM32_APB1, RCC_COMP
#define STM32_CLK_DAC    STM32_APB1, RCC_DAC
#define STM32_CLK_PWR    STM32_APB1, RCC_PWR
#define STM32_CLK_USB    STM32_APB1, RCC_USB
#define STM32_CLK_I2C2   STM32_APB1, RCC_I2C2
#define STM32_CLK_I2C1   STM32_APB1, RCC_I2C1
#define STM32_CLK_UART5  STM32_APB1, RCC_UART5
#define STM32_CLK_UART4  STM32_APB1, RCC_UART4
#define STM32_CLK_USART3 STM32_APB1, RCC_USART3
#define STM32_CLK_USART2 STM32_APB1, RCC_USART2
#define STM32_CLK_SPI3   STM32_APB1, RCC_SPI3
#define STM32_CLK_SPI2   STM32_APB1, RCC_SPI2
#define STM32_CLK_WWDG   STM32_APB1, RCC_WWDG
#define STM32_CLK_LCD    STM32_APB1, RCC_LCD
#define STM32_CLK_TIM7   STM32_APB1, RCC_TIM7
#define STM32_CLK_TIM6   STM32_APB1, RCC_TIM6
#define STM32_CLK_TIM5   STM32_APB1, RCC_TIM5
#define STM32_CLK_TIM4   STM32_APB1, RCC_TIM4
#define STM32_CLK_TIM3   STM32_APB1, RCC_TIM3
#define STM32_CLK_TIM2   STM32_APB1, RCC_TIM2

#ifndef __ASSEMBLER__

#include <stdint.h>

struct stm32_rcc {
	volatile uint32_t cr;
	volatile uint32_t icscr; 
	volatile uint32_t cfgr; 
	volatile uint32_t cir; 

	volatile uint32_t ahbrstr;
	volatile uint32_t apb2rstr;
	volatile uint32_t apb1rstr;
	volatile uint32_t ahbenr;

	volatile uint32_t apb2enr;
	volatile uint32_t apb1enr;
	volatile uint32_t ahblpenr;
	volatile uint32_t apb2lpenr;

	volatile uint32_t apb1lpenr;
	volatile uint32_t csr;
};

struct stm32_clk {
	uint8_t bus:3;
	uint8_t bit:5;
} __attribute__((packed))__;

extern const uint32_t stm32f_apb1_hz;
extern const uint32_t stm32f_apb2_hz;
extern const uint32_t stm32f_ahb_hz;
extern const uint32_t stm32f_tim2_hz;
extern const uint32_t stm32f_tim1_hz;

#ifdef __cplusplus
extern "C" {
#endif

static inline void stm32_clk_enable(struct stm32_rcc * rcc, 
									int bus, int bit) {
	if (bus == STM32_APB2)
		rcc->apb2enr |= 1 << bit;
	else if (bus == STM32_APB1)
		rcc->apb1enr |= 1 << bit;
	else
		rcc->ahbenr |= 1 << bit;
}

static inline void stm32_clk_disable(struct stm32_rcc * rcc, 
									 int bus, int bit) {
	if (bus == STM32_APB2)
		rcc->apb2enr &= ~(1 << bit);
	else if (bus == STM32_APB1)
		rcc->apb1enr &= ~(1 << bit);
	else
		rcc->ahbenr &= ~(1 << bit);
}

static inline uint32_t stm32_clk_hz(int bus, int bit) {
	if (bus == STM32_APB2)
		return stm32f_apb2_hz;
	if (bus == STM32_APB1)
		return stm32f_apb2_hz;
	return stm32f_ahb_hz;
}

#ifdef __cplusplus
}
#endif


#endif /* __ASSEMBLER__ */

#endif /* __STM32L_RCC_H__ */

