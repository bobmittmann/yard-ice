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
 * @file stm32f-rcc.h
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __STM32F_RCC_H__
#define __STM32F_RCC_H__

/*-------------------------------------------------------------------------
  Reset and clock control (RCC)
  ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
/* RCC clock control register */ 
#define STM32F_RCC_CR 0x00

/* PLLI2S clock ready flag */
#define RCC_PLLI2SRDY (1 << 27)
/* Set by hardware to indicate that the PLLI2S is locked.
   0: PLLI2S unlocked
   1: PLLI2S locked */

/* PLLI2S enable */
#define RCC_PLLI2SON (1 << 26)
/* Set and cleared by software to enable PLLI2S.
   Cleared by hardware when entering Stop or Standby mode.
   0: PLLI2S OFF
   1: PLLI2S ON */

/* Main PLL (PLL) clock ready flag */
#define RCC_PLLRDY (1 << 25)
/* Set by hardware to indicate that PLL is locked.
   0: PLL unlocked
   1: PLL locked */

/* Main PLL (PLL) enable */
#define RCC_PLLON (1 << 24)
/* Set and cleared by software to enable PLL.
   Cleared by hardware when entering Stop or Standby mode. This bit 
   cannot be reset if PLL clock is used as the system clock.
   0: PLL OFF
   1: PLL ON */

/* Clock security system enable */
#define RCC_CSSON (1 << 19)
/* Set and cleared by software to enable the clock security system. When 
   CSSON is set, the clock detector is enabled by hardware when the HSE 
   oscillator is ready, and disabled by hardware if an oscillator failure 
   is detected.
   0: Clock security system OFF (Clock detector OFF)
   1: Clock security system ON (Clock detector ON if HSE oscillator is 
   stable, OFF if not) */

/* HSE clock bypass */
#define RCC_HSEBYP (1 << 18)
/* Set and cleared by software to bypass the oscillator with an external 
   clock. The external clock must be enabled with the HSEON bit, to 
   be used by the device.
   The HSEBYP bit can be written only if the HSE oscillator is disabled.
   0: HSE oscillator not bypassed
   1: HSE oscillator bypassed with an external clock */

/* HSE clock ready flag */
#define RCC_HSERDY (1 << 17)
/* Set by hardware to indicate that the HSE oscillator is stable. After 
   the HSION bit is cleared, HSERDY goes low after 6 HSE oscillator 
   clock cycles.
   0: HSE oscillator not ready
   1: HSE oscillator ready */

/* HSE clock enable */
#define RCC_HSEON (1 << 16)
/* Set and cleared by software.
   Cleared by hardware to stop the HSE oscillator when entering Stop or 
   Standby mode. This bit cannot be reset if the HSE oscillator is used 
   directly or indirectly as the system clock.
   0: HSE oscillator OFF
   1: HSE oscillator ON */

/* Internal high-speed clock calibration */
#define RCC_HSICAL (0xff << 8)
/* These bits are initialized automatically at startup. */

/* Internal high-speed clock trimming */
#define RCC_HSITRIM (0x1f << 3)
/* These bits provide an additional user-programmable trimming value that 
   is added to the HSICAL[7:0] bits. It can be programmed to adjust to 
   variations in voltage and temperature that influence the frequency of 
   the internal HSI RC. */

/* Bit 2 Reserved, always read as 0. */

/* Internal high-speed clock ready flag */
#define RCC_HSIRDY (1 << 1)
/* Set by hardware to indicate that the HSI oscillator is stable. After the 
   HSION bit is cleared, HSIRDY goes low after 6 HSI clock cycles.
   0: HSI oscillator not ready
   1: HSI oscillator ready */

/* Internal high-speed clock enable */
#define RCC_HSION (1 << 0)
/* Set and cleared by software.
   Set by hardware to force the HSI oscillator ON when leaving the Stop 
   or Standby mode or in case of a failure of the HSE oscillator used 
   directly or indirectly as the system clock. This bit cannot be cleared 
   if the HSI is used directly or indirectly as the system clock.
   0: HSI oscillator OFF
   1: HSI oscillator ON */

#if defined(STM32F2X) || defined(STM32F4X)

/* ------------------------------------------------------------------------- */
/* RCC PLL configuration register */
#define STM32F_RCC_PLLCFGR 0x04

#define RCC_PLLQ_MASK (0xf << 24)
#define RCC_PLLQ(Q) (((Q) & 0xf) << 24)
/* Main PLL (PLL) division factor for USB OTG FS, SDIO and random number 
   generator clocks Set and cleared by software to control the frequency 
   of USB OTG FS clock, the random number generator clock and the SDIO 
   clock. These bits should be written only if PLL is disabled.
   Caution: The USB OTG FS requires a 48 MHz clock to work correctly. The 
   SDIO and the random number generator need a frequency lower than or 
   equal to 48 MHz to work correctly.
   USB OTG FS clock frequency = VCO frequency / PLLQ with 4 <= PLLQ <= 15
   0000: PLLQ = 0, wrong configuration
   ...
   0011: PLLQ = 3, wrong configuration
   0100: PLLQ = 4
   0101: PLLQ = 5
   ...
   1111: PLLQ = 15 */

/* Bit 23 Reserved, always read as 0. */

#define RCC_PLLSRC (1 << 22)
#define RCC_PLLSRC_HSI (0 << 22)
#define RCC_PLLSRC_HSE (1 << 22)
/* Main PLL(PLL) and audio PLL (PLLI2S) entry clock source Set and 
   cleared by software to select PLL and PLLI2S clock source. This bit 
   can be written only when PLL and PLLI2S are disabled.
   0: HSI clock selected as PLL and PLLI2S clock entry
   1: HSE oscillator clock selected as PLL and PLLI2S clock entr */

#define RCC_PLLP_MASK (0x3 << 16)
#define RCC_PLLP(P) (((((P) >> 1) - 1)  & 0x3) << 16)
#define RCC_PLLP_2 (0x0 << 16)
#define RCC_PLLP_4 (0x1 << 16)
#define RCC_PLLP_6 (0x2 << 16)
#define RCC_PLLP_8 (0x3 << 16)
/* Main PLL (PLL) division factor for main system clock Set and cleared by 
   software to control the frequency of the general PLL output clock. These 
   bits can be written only if PLL is disabled.
   Caution: The software has to set these bits correctly not to exceed 
   120 MHz on this domain.
   PLL output clock frequency = VCO frequency / PLLP with PLLP = 2, 4, 6, or 8
   00: PLLP = 2
   01: PLLP = 4
   10: PLLP = 6
   11: PLLP = 8 */

#define RCC_PLLN_MASK (0xff << 6)
#define RCC_PLLN(N) (((N) & 0xff) << 6)
/* Main PLL (PLL) multiplication factor for VCO Set and cleared by software 
   to control the multiplication factor of the VCO. These bits can be 
   written only when PLL is disabled. Only half-word and word accesses are 
   allowed to write these bits.
   Caution: The software has to set these bits correctly to ensure that the 
   VCO output frequency is between 192 and 432 MHz.
   VCO output frequency = VCO input frequency × PLLN with 192 ≤ PLLN ≤ 432
   000000000: PLLN = 0, wrong configuration
   000000001: PLLN = 1, wrong configuration
   ...
   011000000: PLLN = 192
   011000001: PLLN = 193
   011000010: PLLN = 194
   ...
   110110000: PLLN = 432
   110110000: PLLN = 433, wrong configuration
   ...
   111111111: PLLN = 511, wrong configuration */

#define RCC_PLLM_MASK (0x1f << 0)
#define RCC_PLLM(M) (((M) & 0x1f) << 0)
/* Division factor for the main PLL (PLL) and audio PLL (PLLI2S) input clock
   Set and cleared by software to divide the PLL and PLLI2S input clock 
   before the VCO. These bits can be written only when the PLL and PLLI2S 
   are disabled. Caution: The software has to set these bits correctly to 
   ensure that the VCO input frequency ranges from 1 to 2 MHz. It is 
   recommended to select a frequency of 2 MHz to limit PLL jitter.
   VCO input frequency = PLL input clock frequency / PLLM with 2 ≤ PLLM ≤ 63
   000000: PLLM = 0, wrong configuration
   000001: PLLM = 1, wrong configuration
   000010: PLLM = 2
   000011: PLLM = 3
   000100: PLLM = 4
   ...
   111110: PLLM = 62
   111111: PLLM = 63 */

#endif /* STM32F2X */


/* ------------------------------------------------------------------------- */
/* RCC clock configuration register */

#if defined(STM32F1X)

#define STM32F_RCC_CFGR 0x04

/* STM32F10x RCC clock configuration register */

#define RCC_MCO_SYSCLK (0x4 << 24)
#define RCC_MCO_HSI    (0x5 << 24)
#define RCC_MCO_HSE    (0x6 << 24)
#define RCC_MCO_PLL    (0x7 << 24)

#define RCC_USBPRE       (1 << 22)
#define RCC_USBPRE_1     (1 << 22)
#define RCC_USBPRE_1DOT5 (0 << 22)

#define RCC_PLLMUL_MASK   (0x0f << 18)
#define RCC_PLLMUL(M) ((((M) - 2) & 0x0f) << 18)

#define RCC_PLLXTPRE      (1 << 17)

#define RCC_PLLSRC     (1 << 16)
#define RCC_PLLSRC_HSI (0 << 16)
#define RCC_PLLSRC_HSE (1 << 16)

#define RCC_ADCPRE   (0x3 << 14)
#define RCC_ADCPRE_2 (0x0 << 14)
#define RCC_ADCPRE_4 (0x1 << 14)
#define RCC_ADCPRE_6 (0x2 << 14)
#define RCC_ADCPRE_8 (0x3 << 14)

#define RCC_PPRE2    (0x7 << 11)
#define RCC_PPRE2_1  (0x0 << 11)
#define RCC_PPRE2_2  (0x4 << 11)
#define RCC_PPRE2_4  (0x5 << 11)
#define RCC_PPRE2_8  (0x6 << 11)
#define RCC_PPRE2_16 (0x7 << 11)
#define RCC_PPRE2_GET(CFGR) (((CFGR) >> 11) 0x7)

#define RCC_PPRE1    (0x7 << 8)
#define RCC_PPRE1_1  (0x0 << 8)
#define RCC_PPRE1_2  (0x4 << 8)
#define RCC_PPRE1_4  (0x5 << 8)
#define RCC_PPRE1_8  (0x6 << 8)
#define RCC_PPRE1_16 (0x7 << 8)
#define RCC_PPRE1_GET(CFGR) (((CFGR) >> 8) 0x7)

#elif defined(STM32F2X) || defined(STM32F4X)

#define STM32F_RCC_CFGR 0x08

/* Microcontroller clock output 2 */
#define RCC_MCO2 (0x3 << 30)
#define RCC_MCO2_SYSCLK (0x0 << 30)
#define RCC_MCO2_PLLI2S (0x1 << 30)
#define RCC_MCO2_HSE    (0x2 << 30)
#define RCC_MCO2_PLL    (0x3 << 30)
/* Set and cleared by software. Clock source selection may generate glitches 
   on MCO2. It is highly recommended to configure these bits only after reset 
   before enabling the external oscillators and the PLLs.
   00: System clock (SYSCLK) selected
   01: PLLI2S clock selected
   10: HSE oscillator clock selected
   11: PLL clock selected */

/* MCO2 prescaler */
#define RCC_MCO2PRE (0x7 << 27)
#define RCC_MCO2PRE_1 (0x0 << 27)
#define RCC_MCO2PRE_2 (0x4 << 27)
#define RCC_MCO2PRE_3 (0x5 << 27)
#define RCC_MCO2PRE_4 (0x6 << 27)
#define RCC_MCO2PRE_5 (0x7 << 27)
/* Set and cleared by software to configure the prescaler of the MCO2. 
   Modification of this prescaler may generate glitches on MCO2. It is 
   highly recommended to change this prescaler only after reset before 
   enabling the external oscillators and the PLLs.
   0xx: no division
   100: division by 2
   101: division by 3
   110: division by 4
   111: division by 5 */

/* MCO1 prescaler */
#define RCC_MCO1PRE (0x7 << 24)
#define RCC_MCO1PRE_1 (0x0 << 24)
#define RCC_MCO1PRE_2 (0x4 << 24)
#define RCC_MCO1PRE_3 (0x5 << 24)
#define RCC_MCO1PRE_4 (0x6 << 24)
#define RCC_MCO1PRE_5 (0x7 << 24)
/* Set and cleared by software to configure the prescaler of the MCO1. 
   Modification of this prescaler may generate glitches on MCO1. It is 
   highly recommended to change this prescaler only after reset before 
   enabling the external oscillators and the PLL.
   0xx: no division
   100: division by 2
   101: division by 3
   110: division by 4
   111: division by 5 */

/* I2S clock selection */
#define RCC_I2SSRC (1 << 23)
/* Set and cleared by software. This bit allows to select the I2S clock 
   source between the PLLI2S clock and the external clock. It is highly 
   recommended to change this bit only after reset and before enabling 
   the I2S module.
   0: PLLI2S clock used as I2S clock source
   1: External clock mapped on the I2S_CKIN pin used as I2S clock source */

/* Microcontroller clock output 1 */
#define RCC_MCO1 (0x3 << 21)
#define RCC_MCO1_HSI (0x0 << 21)
#define RCC_MCO1_LSE (0x1 << 21)
#define RCC_MCO1_HSE (0x2 << 21)
#define RCC_MCO1_PLL (0x3 << 21)
/* Set and cleared by software. Clock source selection may generate glitches 
   on MCO1. It is highly recommended to configure these bits only after reset 
   before enabling the external oscillators and PLL.
   00: HSI clock selected
   01: LSE oscillator selected
   10: HSE oscillator clock selected
   11: PLL clock selected */

/* HSE division factor for RTC clock */
#define RCC_RTCPRE (0x1f << 16)
/* Set and cleared by software to divide the HSE clock input clock to 
   generate a 1 MHz clock for RTC.
   Caution: The software has to set these bits correctly to ensure that the 
   clock supplied to the RTC is 1 MHz. These bits must be configured if 
   needed before selecting the RTC clock source.
   00000: no clock
   00001: no clock
   00010: HSE/2
   00011: HSE/3
   00100: HSE/4
   ...
   11110: HSE/30
   11111: HSE/31 */

/* APB high-speed prescaler (APB2) */
#define RCC_PPRE2 (0x7 << 13)
#define RCC_PPRE2_1 (0x0 << 13)
#define RCC_PPRE2_2 (0x4 << 13)
#define RCC_PPRE2_4 (0x5 << 13)
#define RCC_PPRE2_8 (0x6 << 13)
#define RCC_PPRE2_16 (0x7 << 13)
#define RCC_PPRE2_GET(CFGR) (((CFGR) >> 13) 0x7)
/* Set and cleared by software to control APB high-speed clock division factor.
   Caution: The software has to set these bits correctly not to exceed 
   60 MHz on this domain.
   The clocks are divided with the new prescaler factor from 1 to 16 AHB 
   cycles after PPRE2 write.
   0xx: AHB clock not divided
   100: AHB clock divided by 2
   101: AHB clock divided by 4
   110: AHB clock divided by 8
   111: AHB clock divided by 16 */

/* APB Low speed prescaler (APB1) */
#define RCC_PPRE1 (0x7 << 10)
#define RCC_PPRE1_1 (0x0 << 10)
#define RCC_PPRE1_2 (0x4 << 10)
#define RCC_PPRE1_4 (0x5 << 10)
#define RCC_PPRE1_8 (0x6 << 10)
#define RCC_PPRE1_16 (0x7 << 10)
#define RCC_PPRE1_GET(CFGR) (((CFGR) >> 10) 0x7)
/* Set and cleared by software to control APB low-speed clock division factor.
   Caution: The software has to set these bits correctly not to exceed 
   30 MHz on this domain.
   The clocks are divided with the new prescaler factor from 1 to 16 
   AHB cycles after PPRE1 write.
   0xx: AHB clock not divided
   100: AHB clock divided by 2
   101: AHB clock divided by 4
   110: AHB clock divided by 8
   111: AHB clock divided by 16 */

#endif /* STM32F1X, STM32F2X */

/* AHB prescaler */
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
#define RCC_HPRE_GET(CFGR) (((CFGR) >> 4) 0xf)

/* Set and cleared by software to control AHB clock division factor.
   Caution: The clocks are divided with the new prescaler factor from 1 
   to 16 AHB cycles after HPRE write.
   Caution: The AHB clock frequency must be at least 25 MHz when the 
   Ethernet is used.
   0xxx: system clock not divided
   1000: system clock divided by 2
   1001: system clock divided by 4
   1010: system clock divided by 8
   1011: system clock divided by 16
   1100: system clock divided by 64
   1101: system clock divided by 128
   1110: system clock divided by 256
   1111: system clock divided by 512 */

/* System clock switch status. */
#define RCC_SWS (0x3 << 2)
#define RCC_SWS_HSI (0x0 << 2)
#define RCC_SWS_HSE (0x1 << 2)
#define RCC_SWS_PLL (0x2 << 2)
/* Set and cleared by hardware to indicate which 
   clock source is used as the system clock.
   00: HSI oscillator used as the system clock
   01: HSE oscillator used as the system clock
   10: PLL used as the system clock
   11: not applicable */

/* System clock switch */
#define RCC_SW (0x3 << 0)
#define RCC_SW_HSI (0x0 << 0)
#define RCC_SW_HSE (0x1 << 0)
#define RCC_SW_PLL (0x2 << 0)
/* Set and cleared by software to select the system clock source.
   Set by hardware to force the HSI selection when leaving the Stop or 
   Standby mode or in case of failure of the HSE oscillator used directly or 
   indirectly as the system clock.
   00: HSI oscillator selected as system clock
   01: HSE oscillator selected as system clock
   10: PLL selected as system clock
   11: not allowed */


#if defined(STM32F2X) || defined(STM32F4X)

/* ------------------------------------------------------------------------- */
/* RCC AHB1 peripheral clock register */
#define STM32F_RCC_AHB1ENR 0x30

/* USB OTG HSULPI clock enable */
#define RCC_OTGHSULPIEN (1 << 30)
/* Set and cleared by software.
   0: USB OTG HS ULPI clock disabled
   1: USB OTG HS ULPI clock enabled */

/* USB OTG HS clock enable */
#define RCC_OTGHSEN (1 << 29)
/* Set and cleared by software.
   0: USB OTG HS clock disabled
   1: USB OTG HS clock enabled */

/* Ethernet PTP clock enable */
#define RCC_ETHMACPTPEN (1 << 28)
/* Set and cleared by software.
   0: Ethernet PTP clock disabled
   1: Ethernet PTP clock enabled */

/* Ethernet Reception clock enable */
#define RCC_ETHMACRXEN (1 << 27)
/* Set and cleared by software.
   0: Ethernet Reception clock disabled
   1: Ethernet Reception clock enabled */

/* Ethernet Transmission clock enable */
#define RCC_ETHMACTXEN (1 << 26)
/* Set and cleared by software.
   0: Ethernet Transmission clock disabled
   1: Ethernet Transmission clock enabled */

/* Ethernet MAC clock enable */
#define RCC_ETHMACEN (1 << 25)
/* Set and cleared by software.
   0: Ethernet MAC clock disabled
   1: Ethernet MAC clock enabled */

/* Bits 24:23 Reserved, always read as 0. */

/* DMA2 clock enable */
#define RCC_DMA2EN (1 << 22)
/* Set and cleared by software.
   0: DMA2 clock disabled
   1: DMA2 clock enabled */

/* DMA1 clock enable */
#define RCC_DMA1EN (1 << 21)
/* Set and cleared by software.
   0: DMA1 clock disabled
   1: DMA1 clock enabled */

/* Bits 20:19 Reserved, always read as 0. */

/* Backup SRAM interface clock enable */
#define RCC_BKPSRAMEN (1 << 18)
/* Set and cleared by software.
   0: Backup SRAM interface clock disabled
   1: Backup SRAM interface clock enabled */

/* Bits 17:13 Reserved, always read as 0. */

/* CRC clock enable */
#define RCC_CRCEN (1 << 12)
/* Set and cleared by software.
   0: CRC clock disabled
   1: CRC clock enabled */

/* Bits 11:9 Reserved, always read as 0. */

/* IO port I clock enable */
#define RCC_GPIOIEN (1 << 8)
/* Set and cleared by software.
   0: IO port I clock disabled
   1: IO port I clock enabled */

/* IO port H clock enable */
#define RCC_GPIOHEN (1 << 7)
/* Set and cleared by software.
   0: IO port H clock disabled
   1: IO port H clock enabled */

/* IO port G clock enable */
#define RCC_GPIOGEN (1 << 6)
/* Set and cleared by software.
   0: IO port G clock disabled
   1: IO port G clock enabled */

/* IO port F clock enable */
#define RCC_GPIOFEN (1 << 5)
/* Set and cleared by software.
   0: IO port F clock disabled
   1: IO port F clock enabled */

/* IO port E clock enable */
#define RCC_GPIOEEN (1 << 4)
/* Set and cleared by software.
   0: IO port E clock disabled
   1: IO port E clock enabled */

/* IO port D clock enable */
#define RCC_GPIODEN (1 << 3)
/* Set and cleared by software.
   0: IO port D clock disabled
   1: IO port D clock enabled */

/* IO port C clock enable */
#define RCC_GPIOCEN (1 << 2)
/* Set and cleared by software.
   0: IO port C clock disabled
   1: IO port C clock enabled */

/* IO port B clock enable */
#define RCC_GPIOBEN (1 << 1)
/* Set and cleared by software.
   0: IO port B clock disabled
   1: IO port B clock enabled */

/* IO port A clock enable */
#define RCC_GPIOAEN (1 << 0)
/* Set and cleared by software.
   0: IO port A clock disabled
   1: IO port A clock enabled */


/* ------------------------------------------------------------------------- */
/* RCC AHB2 peripheral clock enable register */
#define STM32F_RCC_AHB2ENR 0x34

/* Bit 7 - USB OTG FS clock enable */
#define RCC_OTGFSEN (1 << 7)
/* Set and cleared by software.
   0: USB OTG FS clock disabled
   1: USB OTG FS clock enabled */

/* Bit 6 - Random number generator clock enable */
#define RCC_RNGEN (1 << 6)
/* Set and cleared by software.
   0: Random number generator clock disabled
   1: Random number generator clock enabled */

/* Bit 5 - Hash modules clock enable */
#define RCC_HASHEN (1 << 5)
/* Set and cleared by software.
   0: Hash modules clock disabled
   1: Hash modules clock enabled */

/* Bit 4 - Cryptographic modules clock enable */
#define RCC_CRYPEN (1 << 4)
/* Set and cleared by software.
   0: cryptographic module clock disabled
   1: cryptographic module clock enabled */

/* Bit 0 - Camera interface enable */
#define RCC_DCMIEN (1 << 0)
/* Set and cleared by software.
   0: Camera interface clock disabled
   1: Camera interface clock enabled */


/* ------------------------------------------------------------------------- */
/* RCC AHB3 peripheral clock enable register */
#define STM32F_RCC_AHB3ENR 0x38


/* Bit 0 - Flexible static memory controller module clock enable */
#define RCC_FSMCEN (1 << 0)
/* Set and cleared by software.
   0: FSMC module clock disabled
   1: FSMC module clock enabled */


/* ------------------------------------------------------------------------- */
/* RCC APB1 peripheral clock enable register */
#define STM32F_RCC_APB1ENR 0x40

/* Bit 29 - DAC interface clock enable */
#define RCC_DACEN (1 << 29)
/* Set and cleared by software.
   0: DAC interface clock disabled
   1: DAC interface clock enable */

/* Bit 28 - Power interface clock enable */
#define RCC_PWREN (1 << 28)
/* Set and cleared by software.
   0: Power interface clock disabled
   1: Power interface clock enable */

/* Bit 26 - CAN 2 clock enable */
#define RCC_CAN2EN (1 << 26)
/* Set and cleared by software.
   0: CAN 2 clock disabled
   1: CAN 2 clock enabled */

/* Bit 25 - CAN 1 clock enable */
#define RCC_CAN1EN (1 << 25)
/* Set and cleared by software.
   0: CAN 1 clock disabled
   1: CAN 1 clock enabled */

/* Bit 23 - I2C3 clock enable */
#define RCC_I2C3EN (1 << 23)
/* Set and cleared by software.
   0: I2C3 clock disabled
   1: I2C3 clock enabled */

/* Bit 22 - I2C2 clock enable */
#define RCC_I2C2EN (1 << 22)
/* Set and cleared by software.
   0: I2C2 clock disabled
   1: I2C2 clock enabled */

/* Bit 21 - I2C1 clock enable */
#define RCC_I2C1EN (1 << 21)
/* Set and cleared by software.
   0: I2C1 clock disabled
   1: I2C1 clock enabled */

/* Bit 20 - UART5 clock enable */
#define RCC_UART5EN (1 << 20)
/* Set and cleared by software.
   0: UART5 clock disabled
   1: UART5 clock enabled */

/* Bit 19 - UART4 clock enable */
#define RCC_UART4EN (1 << 19)
/* Set and cleared by software.
   0: UART4 clock disabled
   1: UART4 clock enabled */

/* Bit 18 - USART3 clock enable */
#define RCC_USART3EN (1 << 18)
/* Set and cleared by software.
   0: USART3 clock disabled
   1: USART3 clock enabled */

/* Bit 17 - USART 2 clock enable */
#define RCC_USART2EN (1 << 17)
/* Set and cleared by software.
   0: USART2 clock disabled
   1: USART2 clock enabled */

/* Bit 15 - SPI3 clock enable */
#define RCC_SPI3EN (1 << 15)
/* Set and cleared by software.
   0: SPI3 clock disabled
   1: SPI3 clock enabled */

/* Bit 14 - SPI2 clock enable */
#define RCC_SPI2EN (1 << 14)
/* Set and cleared by software.
   0: SPI2 clock disabled
   1: SPI2 clock enabled */

/* Bit 11 - Window watchdog clock enable */
#define RCC_WWDGEN (1 << 11)
/* Set and cleared by software.
   0: Window watchdog clock disabled
   1: Window watchdog clock enabled */

/* Bit 8 - TIM14 clock enable */
#define RCC_TIM14EN (1 << 8)
/* Set and cleared by software.
   0: TIM14 clock disabled
   1: TIM14 clock enabled */

/* Bit 7 - TIM13 clock enable */
#define RCC_TIM13EN (1 << 7)
/* Set and cleared by software.
   0: TIM13 clock disabled
   1: TIM13 clock enabled */

/* Bit 6 - TIM12 clock enable */
#define RCC_TIM12EN (1 << 6)
/* Set and cleared by software.
   0: TIM12 clock disabled
   1: TIM12 clock enabled */

/* Bit 5 - TIM7 clock enable */
#define RCC_TIM7EN (1 << 5)
/* Set and cleared by software.
   0: TIM7 clock disabled
   1: TIM7 clock enabled */

/* Bit 4 - TIM6 clock enable */
#define RCC_TIM6EN (1 << 4)
/* Set and cleared by software.
   0: TIM6 clock disabled
   1: TIM6 clock enabled */

/* Bit 3 - TIM5 clock enable */
#define RCC_TIM5EN (1 << 3)
/* Set and cleared by software.
   0: TIM5 clock disabled
   1: TIM5 clock enabled */

/* Bit 2 - TIM4 clock enable */
#define RCC_TIM4EN (1 << 2)
/* Set and cleared by software.
   0: TIM4 clock disabled
   1: TIM4 clock enabled */

/* Bit 1 - TIM3 clock enable */
#define RCC_TIM3EN (1 << 1)
/* Set and cleared by software.
   0: TIM3 clock disabled
   1: TIM3 clock enabled */

/* Bit 0 - TIM2 clock enable */
#define RCC_TIM2EN (1 << 0)
/* Set and cleared by software.
   0: TIM2 clock disabled
   1: TIM2 clock enabled */


/* ------------------------------------------------------------------------- */
/* RCC APB2 peripheral clock enable register */
#define STM32F_RCC_APB2ENR 0x44


/* Bit 18 - TIM11 clock enable */
#define RCC_TIM11EN (1 << 18)
/* Set and cleared by software.
   0: TIM11 clock disabled
   1: TIM11 clock enabled */

/* Bit 17 - TIM10 clock enable */
#define RCC_TIM10EN (1 << 17)
/* Set and cleared by software.
   0: TIM10 clock disabled
   1: TIM10 clock enabled */

/* Bit 16 - TIM9 clock enable */
#define RCC_TIM9EN (1 << 16)
/* Set and cleared by software.
   0: TIM9 clock disabled
   1: TIM9 clock enabled */

/* Bit 14 - System configuration controller clock enable */
#define RCC_SYSCFGEN (1 << 14)
/* Set and cleared by software.
   0: System configuration controller clock disabled
   1: System configuration controller clock enabled */

/* Bit 12 - SPI1 clock enable */
#define RCC_SPI1EN (1 << 12)
/* Set and cleared by software.
   0: SPI1 clock disabled
   1: SPI1 clock enabled */

/* Bit 11 - SDIO clock enable */
#define RCC_SDIOEN (1 << 11)
/* Set and cleared by software.
   0: SDIO module clock disabled
   1: SDIO module clock enabled */

/* Bit 10 - ADC3 clock enable */
#define RCC_ADC3EN (1 << 10)
/* Set and cleared by software.
   0: ADC3 clock disabled
   1: ADC3 clock disabled */

/* Bit 9 - ADC2 clock enable */
#define RCC_ADC2EN (1 << 9)
/*      Set and cleared by software.
    0: ADC2 clock disabled
   1: ADC2 clock disabled */

/* Bit 8 - ADC1 clock enable */
#define RCC_ADC1EN (1 << 8)
/* Set and cleared by software.
   0: ADC1 clock disabled
   1: ADC1 clock disabled */

/* Bit 5 - USART6 clock enable */
#define RCC_USART6EN (1 << 5)
/* Set and cleared by software.
   0: USART6 clock disabled
   1: USART6 clock enabled */

/* Bit 4 - USART1 clock enable */
#define RCC_USART1EN (1 << 4)
/* Set and cleared by software.
   0: USART1 clock disabled
   1: USART1 clock enabled */

/* Bit 1 - TIM8 clock enable */
#define RCC_TIM8EN (1 << 1)
/* Set and cleared by software.
   0: TIM8 clock disabled
   1: TIM8 clock enabled */

/* Bit 0 - TIM1 clock enable */
#define RCC_TIM1EN (1 << 0)
/* Set and cleared by software.
   0: TIM1 clock disabled
   1: TIM1 clock enabled */

/* ------------------------------------------------------------------------- */
/* RCC AHB1 peripheral clock enable in low power mode register */
#define STM32F_RCC_AHB1LPENR 0x50

/* Bit 30 - USB OTG HS ULPI clock enable during Sleep mode */
#define RCC_OTGHSULPILPEN (1 << 30)
/* Set and cleared by software.
   0: USB OTG HS ULPI clock disabled during Sleep mode
   1: USB OTG HS ULPI clock enabled during Sleep mode */

/* Bit 29 - USB OTG HS clock enable during Sleep mode */
#define RCC_OTGHSLPEN (1 << 29)
/* Set and cleared by software.
   0: USB OTG HS clock disabled during Sleep mode
   1: USB OTG HS clock enabled during Sleep mode */

/* Bit 28 - Ethernet PTP clock enable during Sleep mode */
#define RCC_ETHMACPTPLPEN (1 << 28)
/* Set and cleared by software.
   0: Ethernet PTP clock disabled during Sleep mode
   1: Ethernet PTP clock enabled during Sleep mode */

/* Bit 27 - Ethernet reception clock enable during Sleep mode */
#define RCC_ETHMACRXLPEN (1 << 27)
/* Set and cleared by software.
   0: Ethernet reception clock disabled during Sleep mode
   1: Ethernet reception clock enabled during Sleep mode */

/* Bit 26 - Ethernet transmission clock enable during Sleep mode */
#define RCC_ETHMACTXLPEN (1 << 26)
/* Set and cleared by software.
   0: Ethernet transmission clock disabled during sleep mode
   1: Ethernet transmission clock enabled during sleep mode */

/* Bit 25 - Ethernet MAC clock enable during Sleep mode */
#define RCC_ETHMACLPEN (1 << 25)
/* Set and cleared by software.
   0: Ethernet MAC clock disabled during Sleep mode
   1: Ethernet MAC clock enabled during Sleep mode */

/* Bit 22 - DMA2 clock enable during Sleep mode */
#define RCC_DMA2LPEN (1 << 22)
/* Set and cleared by software.
   0: DMA2 clock disabled during Sleep mode
   1: DMA2 clock enabled during Sleep mode */

/* Bit 21 - DMA1 clock enable during Sleep mode */
#define RCC_DMA1LPEN (1 << 21)
/* Set and cleared by software.
   0: DMA1 clock disabled during Sleep mode
   1: DMA1 clock enabled during Sleep mode */

/* Bit 18 - Backup SRAM interface clock enable during Sleep mode */
#define RCC_BKPSRAMLPEN (1 << 18)
/* Set and cleared by software.
   0: Backup SRAM interface clock disabled during Sleep mode
   1: Backup SRAM interface clock enabled during Sleep mode */

/* Bit 17 - SRAM 2 interface clock enable during Sleep mode */
#define RCC_SRAM2LPEN (1 << 17)
/* Set and cleared by software.
   0: SRAM 2 interface clock disabled during Sleep mode
   1: SRAM 2 interface clock enabled during Sleep mode */

/* Bit 16 - SRAM 1interface clock enable during Sleep mode */
#define RCC_SRAM1LPEN (1 << 16)
/* Set and cleared by software.
   0: SRAM 1 interface clock disabled during Sleep mode
   1: SRAM 1 interface clock enabled during Sleep mode */

/* Bit 15 - Flash interface clock enable during Sleep mode */
#define RCC_FLITFLPEN (1 << 15)
/* Set and cleared by software.
   0: Flash interface clock disabled during Sleep mode
   1: Flash interface clock enabled during Sleep mode */

/* Bit 12 - CRC clock enable during Sleep mode */
#define RCC_CRCLPEN (1 << 12)
/* Set and cleared by software.
   0: CRC clock disabled during Sleep mode
   1: CRC clock enabled during Sleep mode */

/* Bit 8 - IO port I clock enable during Sleep mode */
#define RCC_GPIOILPEN (1 << 8)
/* Set and cleared by software.
   0: IO port I clock disabled during Sleep mode
   1: IO port I clock enabled during Sleep mode */ 

/* Bit 7 - IO port H clock enable during Sleep mode */
#define RCC_GPIOHLPEN (1 << 7)
/* Set and cleared by software.
   0: IO port H clock disabled during Sleep mode
   1: IO port H clock enabled during Sleep mode */

/* Bit 6 - IO port G clock enable during Sleep mode */
#define RCC_GPIOGLPEN (1 << 6)
/* Set and cleared by software.
   0: IO port G clock disabled during Sleep mode
   1: IO port G clock enabled during Sleep mode */

/* Bit 5 - IO port F clock enable during Sleep mode */
#define RCC_GPIOFLPEN (1 << 5)
/* Set and cleared by software.
   0: IO port F clock disabled during Sleep mode
   1: IO port F clock enabled during Sleep mode */

/* Bit 4 - IO port E clock enable during Sleep mode */
#define RCC_GPIOELPEN (1 << 4)
/* Set and cleared by software.
   0: IO port E clock disabled during Sleep mode
   1: IO port E clock enabled during Sleep mode */

/* Bit 3 - IO port D clock enable during Sleep mode */
#define RCC_GPIODLPEN (1 << 3)
/* Set and cleared by software.
   0: IO port D clock disabled during Sleep mode
   1: IO port D clock enabled during Sleep mode */

/* Bit 2 - IO port C clock enable during Sleep mode */
#define RCC_GPIOCLPEN (1 << 2)
/* Set and cleared by software.
   0: IO port C clock disabled during Sleep mode
   1: IO port C clock enabled during Sleep mode */

/* Bit 1 - IO port B clock enable during Sleep mode */
#define RCC_GPIOBLPEN (1 << 1)
/* Set and cleared by software.
   0: IO port B clock disabled during Sleep mode
   1: IO port B clock enabled during Sleep mode */

/* Bit 0 - IO port A clock enable during sleep mode */
#define RCC_GPIOALPEN (1 << 0)
/* Set and cleared by software.
   0: IO port A clock disabled during Sleep mode
   1: IO port A clock enabled during Sleep mode */

/* ------------------------------------------------------------------------- */
/* RCC AHB2 peripheral clock enable in low power mode register */
#define STM32F_RCC_AHB2LPENR 0x54

/* Bit 7 - USB OTG FS clock enable during Sleep mode */
#define RCC_OTGFSLPEN (1 << 7)
/* Set and cleared by software.
   0: USB OTG FS clock disabled during Sleep mode
   1: USB OTG FS clock enabled during Sleep mode */

/* Bit 6 - Random number generator clock enable during Sleep mode */
#define RCC_RNGLPEN (1 << 6)
/* Set and cleared by software.
   0: Random number generator clock disabled during Sleep mode
   1: Random number generator clock enabled during Sleep mode */

/* Bit 5 - Hash modules clock enable during Sleep mode */
#define RCC_HASHLPEN (1 << 5)
/* Set and cleared by software.
   0: Hash modules clock disabled during Sleep mode
   1: Hash modules clock enabled during Sleep mode */

/* Bit 4 - Cryptography modules clock enable during Sleep mode */
#define RCC_CRYPLPEN (1 << 4)
/* Set and cleared by software.
   0: cryptography modules clock disabled during Sleep mode
   1: cryptography modules clock enabled during Sleep mode */

/* Bit 0 - Camera interface enable during Sleep mode */
#define RCC_DCMILPEN (1 << 0)
/* Set and cleared by software.
   0: Camera interface clock disabled during Sleep mode
   1: Camera interface clock enabled during Sleep mode */


/* ------------------------------------------------------------------------- */
/* RCC AHB3 peripheral clock enable in low power mode register */
#define STM32F_RCC_AHB3LPENR 0x58

/* [31..1] Reserved, always read as 0. */

/* Flexible static memory controller module clock enable during Sleep mode */
#define RCC_FSMCLPEN (1 << 0)
/* Set and cleared by software.
   0: FSMC module clock disabled during Sleep mode
   1: FSMC module clock enabled during Sleep mode */

/* ------------------------------------------------------------------------- */
/* RCC APB1 peripheral clock enable in low power mode register */
#define STM32F_RCC_APB1LPENR 0x60

/* Bit 29 - DAC interface clock enable during Sleep mode */
#define RCC_DACLPEN (1 << 29)
/* Set and cleared by software.
   0: DAC interface clock disabled during Sleep mode
   1: DAC interface clock enabled during Sleep mode */

/* Bit 28 - Power interface clock enable during Sleep mode */
#define RCC_PWRLPEN (1 << 28)
/* Set and cleared by software.
   0: Power interface clock disabled during Sleep mode 
   1: Power interface clock enabled during Sleep mode */

/* Bit 26 - CAN 2 clock enable during Sleep mode */
#define RCC_CAN2LPEN (1 << 26)
/* Set and cleared by software.
   0: CAN 2 clock disabled during sleep mode
   1: CAN 2 clock enabled during sleep mode */

/* Bit 25 - CAN 1 clock enable during Sleep mode */
#define RCC_CAN1LPEN (1 << 25)
/* Set and cleared by software.
   0: CAN 1 clock disabled during Sleep mode
   1: CAN 1 clock enabled during Sleep mode */

/* Bit 23 - I2C3 clock enable during Sleep mode */
#define RCC_I2C3LPEN (1 << 23)
/* Set and cleared by software.
   0: I2C3 clock disabled during Sleep mode
   1: I2C3 clock enabled during Sleep mode */

/* Bit 22 - I2C2 clock enable during Sleep mode */
#define RCC_I2C2LPEN (1 << 22)
/* Set and cleared by software.
   0: I2C2 clock disabled during Sleep mode
   1: I2C2 clock enabled during Sleep mode */

/* Bit 21 - I2C1 clock enable during Sleep mode */
#define RCC_I2C1LPEN (1 << 21)
/* Set and cleared by software.
   0: I2C1 clock disabled during Sleep mode
   1: I2C1 clock enabled during Sleep mode */

/* Bit 20 - UART5 clock enable during Sleep mode */
#define RCC_UART5LPEN (1 << 20)
/* Set and cleared by software.
   0: UART5 clock disabled during Sleep mode
   1: UART5 clock enabled during Sleep mode */

/* Bit 19 - UART4 clock enable during Sleep mode */
#define RCC_UART4LPEN (1 << 19)
/* Set and cleared by software.
   0: UART4 clock disabled during Sleep mode
   1: UART4 clock enabled during Sleep mode */

/* Bit 18 - USART3 clock enable during Sleep mode */
#define RCC_USART3LPEN (1 << 18)
/* Set and cleared by software.
   0: USART3 clock disabled during Sleep mode
   1: USART3 clock enabled during Sleep mode */

/* Bit 17 - USART2 clock enable during Sleep mode */
#define RCC_USART2LPEN (1 << 17)
/* Set and cleared by software.
   0: USART2 clock disabled during Sleep mode
   1: USART2 clock enabled during Sleep mode */

/* Bit 15 - SPI3 clock enable during Sleep mode */
#define RCC_SPI3LPEN (1 << 15)
/* Set and cleared by software.
   0: SPI3 clock disabled during Sleep mode
   1: SPI3 clock enabled during Sleep mode */

/* Bit 14 - SPI2 clock enable during Sleep mode */
#define RCC_SPI2LPEN (1 << 14)
/* Set and cleared by software.
   0: SPI2 clock disabled during Sleep mode
   1: SPI2 clock enabled during Sleep mode */

/* Bit 11 - Window watchdog clock enable during Sleep mode */
#define RCC_WWDGLPEN (1 << 11)
/* Set and cleared by software.
   0: Window watchdog clock disabled during sleep mode
   1: Window watchdog clock enabled during sleep mode */

/* Bit 8 - TIM14 clock enable during Sleep mode */
#define RCC_TIM14LPEN (1 << 8)
/* Set and cleared by software.
   0: TIM14 clock disabled during Sleep mode
   1: TIM14 clock enabled during Sleep mode */

/* Bit 7 - TIM13 clock enable during Sleep mode */
#define RCC_TIM13LPEN (1 << 7)
/* Set and cleared by software.
   0: TIM13 clock disabled during Sleep mode
   1: TIM13 clock enabled during Sleep mode */

/* Bit 6 - TIM12 clock enable during Sleep mode */
#define RCC_TIM12LPEN (1 << 6)
/* Set and cleared by software.
   0: TIM12 clock disabled during Sleep mode
   1: TIM12 clock enabled during Sleep mode */

/* Bit 5 - TIM7 clock enable during Sleep mode */
#define RCC_TIM7LPEN (1 << 5)
/* Set and cleared by software.
   0: TIM7 clock disabled during Sleep mode
   1: TIM7 clock enabled during Sleep mode */

/* Bit 4 - TIM6 clock enable during Sleep mode */
#define RCC_TIM6LPEN (1 << 4)
/* Set and cleared by software.
   0: TIM6 clock disabled during Sleep mode
   1: TIM6 clock enabled during Sleep mode */

/* Bit 3 - TIM5 clock enable during Sleep mode */
#define RCC_TIM5LPEN (1 << 3)
/* Set and cleared by software.
   0: TIM5 clock disabled during Sleep mode
   1: TIM5 clock enabled during Sleep mode */

/* Bit 2 - TIM4 clock enable during Sleep mode */
#define RCC_TIM4LPEN (1 << 2)
/* Set and cleared by software.
   0: TIM4 clock disabled during Sleep mode
   1: TIM4 clock enabled during Sleep mode */

/* Bit 1 - TIM3 clock enable during Sleep mode */
#define RCC_TIM3LPEN (1 << 1)
/* Set and cleared by software.
   0: TIM3 clock disabled during Sleep mode
   1: TIM3 clock enabled during Sleep mode */

/* Bit 0 - TIM2 clock enable during Sleep mode */
#define RCC_TIM2LPEN (1 << 0)
/* Set and cleared by software.
   0: TIM2 clock disabled during Sleep mode
   1: TIM2 clock enabled during Sleep mode */

/* ------------------------------------------------------------------------- */
/* RCC APB2 peripheral clock enabled in low power mode register */
#define STM32F_RCC_APB2LPENR 0x64

/* Bit 18 - TIM11 clock enable during Sleep mode */
#define RCC_TIM11LPEN (1 << 18)
/* Set and cleared by software.
   0: TIM11 clock disabled during Sleep mode
   1: TIM11 clock enabled during Sleep mode */

/* Bit 17 - TIM10 clock enable during Sleep mode */
#define RCC_TIM10LPEN (1 << 17)
/* Set and cleared by software.
   0: TIM10 clock disabled during Sleep mode
   1: TIM10 clock enabled during Sleep mode */

/* Bit 16 - TIM9 clock enable during sleep mode */
#define RCC_TIM9LPEN (1 << 16)
/* Set and cleared by software.
   0: TIM9 clock disabled during Sleep mode
   1: TIM9 clock enabled during Sleep mode */

/* Bit 14 - System configuration controller clock enable during Sleep mode */
#define RCC_SYSCFGLPEN (1 << 14)
/* Set and cleared by software.
   0: System configuration controller clock disabled during Sleep mode
   1: System configuration controller clock enabled during Sleep mode */

/* Bit 12 - SPI 1 clock enable during Sleep mode */
#define RCC_SPI1LPEN (1 << 12)
/* Set and cleared by software.
   0: SPI 1 clock disabled during Sleep mode
   1: SPI 1 clock enabled during Sleep mode */

/* Bit 11 - SDIO clock enable during Sleep mode */
#define RCC_SDIOLPEN (1 << 11)
/* Set and cleared by software.
   0: SDIO module clock disabled during Sleep mode
   1: SDIO module clock enabled during Sleep mode */

/* Bit 10 - ADC 3 clock enable during Sleep mode */
#define RCC_ADC3LPEN (1 << 10)
/* Set and cleared by software.
   0: ADC 3 clock disabled during Sleep mode
   1: ADC 3 clock disabled during Sleep mode */

/* Bit 9 - ADC2 clock enable during Sleep mode */
#define RCC_ADC2LPEN (1 << 9)
/* Set and cleared by software.
   0: ADC2 clock disabled during Sleep mode
   1: ADC2 clock disabled during Sleep mode */

/* Bit 8 - ADC1 clock enable during Sleep mode */
#define RCC_ADC1LPEN (1 << 8)
/* Set and cleared by software.
   0: ADC1 clock disabled during Sleep mode
   1: ADC1 clock disabled during Sleep mode */

/* Bit 5 - USART6 clock enable during Sleep mode */
#define RCC_USART6LPEN (1 << 5)
/* Set and cleared by software.
   0: USART6 clock disabled during Sleep mode
   1: USART6 clock enabled during Sleep mode */

/* Bit 4 - USART1 clock enable during Sleep mode */
#define RCC_USART1LPEN (1 << 4)
/* Set and cleared by software.
   0: USART1 clock disabled during Sleep mode
   1: USART1 clock enabled during Sleep mode */

/* Bit 1 - TIM8 clock enable during Sleep mode */
#define RCC_TIM8LPEN (1 << 1)
/* Set and cleared by software.
   0: TIM8 clock disabled during Sleep mode
   1: TIM8 clock enabled during Sleep mode */

/* Bit 0 - TIM1 clock enable during Sleep mode */
#define RCC_TIM1LPEN (1 << 0)
/* Set and cleared by software.
   0: TIM1 clock disabled during Sleep mode
   1: TIM1 clock enabled during Sleep mode */


/* ------------------------------------------------------------------------- */
/* RCC Backup domain control register */
#define STM32F_RCC_BDCR 0x70


/* Bit 16 - Backup domain software reset */
#define RCC_BDRST (1 << 16)
/* Set and cleared by software.
   0: Reset not activated
   1: Resets the entire Backup domain
   Note: The BKPSRAM is not affected by this reset, the only way of 
   resetting the BKPSRAM is through the Flash interface when a protection 
   level change from level 1 to level 0 is requested. */

/* Bit 15 - RTC clock enable */
#define RCC_RTCEN (1 << 15)
/* Set and cleared by software.
   0: RTC clock disabled
   1: RTC clock enabled */

/* Bits [9..8] - RTC clock source selection */
#define RCC_RTCSEL_MSK (((1 << (1 + 1)) - 1) << 8)
#define RCC_RTCSEL_SET(VAL) (((VAL) << 8) & RTCSEL_MSK)
#define RCC_RTCSEL_GET(REG) (((REG) & RTCSEL_MSK) >> 8)
/* Set by software to select the clock source for the RTC. Once the RTC 
   clock source has been selected, it cannot be changed anymore unless the 
   Backup domain is reset. The BDRST bit can be used to reset them.
   00: No clock
   01: LSE oscillator clock used as the RTC clock
   10: LSI oscillator clock used as the RTC clock
   11: HSE oscillator clock divided by a programmable prescaler (selection 
   through the RTCPRE[4:0] bits in the RCC clock configuration register 
   (RCC_CFGR)) used as the RTC clock */

#define RCC_RTCSEL_MSK (((1 << (1 + 1)) - 1) << 8)
#define RCC_RTCSEL_SET(VAL) (((VAL) << 8) & RTCSEL_MSK)
#define RCC_RTCSEL_GET(REG) (((REG) & RTCSEL_MSK) >> 8)

/* Bit 2 - External low-speed oscillator bypass */
#define RCC_LSEBYP (1 << 2)
/* Set and cleared by software to bypass oscillator in debug mode. This 
   bit can be written only when the LSE clock is disabled.
   0: LSE oscillator not bypassed
   1: LSE oscillator bypassed */

/* Bit 1 - External low-speed oscillator ready */
#define RCC_LSERDY (1 << 1)
/* Set and cleared by hardware to indicate when the external 32 kHz 
   oscillator is stable. After the LSEON bit is cleared, LSERDY goes low 
   after 6 external low-speed oscillator clock cycles.
   0: LSE clock not ready
   1: LSE clock ready */

/* Bit 0 - External low-speed oscillator enable */
#define RCC_LSEON (1 << 0)
/* Set and cleared by software.
   0: LSE clock OFF
   1: LSE clock ON */


/* ------------------------------------------------------------------------- */
/* RCC clock control & status register */
#define STM32F_RCC_CSR 0x74


/* Bit 31 - Low-power reset flag */
#define RCC_LPWRRSTF (1 << 31)
/* Set by hardware when a Low-power management reset occurs.
   Cleared by writing to the RMVF bit.
   0: No Low-power management reset occurred
   1: Low-power management reset occurred
   For further information on Low-power management reset, refer to 
   Low-power management reset. */

/* Bit 30 - Window watchdog reset flag */
#define RCC_WWDGRSTF (1 << 30)
/* Set by hardware when a window watchdog reset occurs.
   Cleared by writing to the RMVF bit.
   0: No window watchdog reset occurred
   1: Window watchdog reset occurred */

/* Bit 29 - Independent watchdog reset flag */
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

/* Bit 27 - POR/PDR reset flag */
#define RCC_PORRSTF (1 << 27)
/* Set by hardware when a POR/PDR reset occurs.
   Cleared by writing to the RMVF bit.
   0: No POR/PDR reset occurred
   1: POR/PDR reset occurred */

/* Bit 26 - PIN reset flag */
#define RCC_PINRSTF (1 << 26)
/* Set by hardware when a reset from the NRST pin occurs.
   Cleared by writing to the RMVF bit.
   0: No reset from NRST pin occurred
   1: Reset from NRST pin occurred */

/* Bit 25 - BOR reset flag */
#define RCC_BORRSTF (1 << 25)
/* Cleared by software by writing the RMVF bit.
   Set by hardware when a POR/PDR or BOR reset occurs.
   0: No POR/PDR or BOR reset occurred
   1: POR/PDR or BOR reset occurred */

/* Bit 24 - Remove reset flag */
#define RCC_RMVF (1 << 24)
/* Set by software to clear the reset flags.
   0: No effect
   1: Clear the reset flags */

/* Bit 1 - Internal low-speed oscillator ready */
#define RCC_LSIRDY (1 << 1)
/* Set and cleared by hardware to indicate when the internal RC 40 kHz 
   oscillator is stable.
   After the LSION bit is cleared, LSIRDY goes low after 3 LSI clock cycles.
   0: LSI RC oscillator not ready
   1: LSI RC oscillator ready */

/* Bit 0 - Internal low-speed oscillator enable */
#define RCC_LSION (1 << 0)
/* Set and cleared by software.
   0: LSI RC oscillator OFF
   1: LSI RC oscillator ON */


/* ------------------------------------------------------------------------- */
/* RCC spread spectrum clock generation register */
#define STM32F_RCC_SSCGR 0x80


/* Bit 31 - Spread spectrum modulation enable */
#define RCC_SSCGEN (1 << 31)
/* Set and cleared by software.
   0: Spread spectrum modulation DISABLE. (To write after 
   clearing CR[24]=PLLON bit)
   1: Spread spectrum modulation ENABLE. (To write before 
   setting CR[24]=PLLON bit) */

/* Bit 30 - Spread Select */
#define RCC_SPREADSEL (1 << 30)
/* Set and cleared by software.
   To write before to set CR[24]=PLLON bit.
   0: Center spread
   1: Down spread */

/* Bits [27..13] - Incrementation step */
#define RCC_INCSTEP ((27 - 13) << 13)
/* Set and cleared by software. To write before setting CR[24]=PLLON bit.
   Configuration input for modulation profile amplitude. */

/* Bits [12..0] - Modulation period */
#define RCC_MODPER ((12 - 0) << 0)
/* Set and cleared by software. To write before setting CR[24]=PLLON bit.
   Configuration input for modulation profile period. */


/* ------------------------------------------------------------------------- */
/* RCC PLLI2S configuration register */
#define STM32F_RCC_PLLI2SCFGR 0x84


/* Bits [30..28] - PLLI2S division factor for I2S clocks */
#define RCC_PLLI2SR ((30 - 28) << 28)
/* Set and cleared by software to control the I2S clock frequency. These bits 
   should be written only if the PLLI2S is disabled. The factor must be 
   chosen in accordance with the prescaler values inside the I2S peripherals, 
   to reach 0.3% error when using standard crystals and 0% error with audio 
   crystals. For more information about I2S clock frequency and precision, 
   refer to Section 25.4.3: Clock generator in the I2S chapter.
   Caution: The I2Ss requires a frequency lower than or equal to 192 MHz 
   to work correctly.
   I2S clock frequency = VCO frequency / PLLR with 2 ≤ PLLR ≤ 7
   000: PLLR = 0, wrong configuration
   001: PLLR = 1, wrong configuration
   010: PLLR = 2
   ...
   111: PLLR = 7 */

/* [27..15] Reserved, always read as 0. */

/* Bits [14..6] - PLLI2S multiplication factor for VCO */
#define RCC_PLLI2SN ((14 - 6) << 6)
/* Set and cleared by software to control the multiplication factor 
   of the VCO. These bits can be written only when the PLLI2S is 
   disabled. Only half-word and word accesses are allowed to write 
   these bits.
   Caution: The software has to set these bits correctly to ensure that 
   the VCO output frequency is between 192 and 432 MHz.
   VCO output frequency = VCO input frequency × PLLI2SN with 
   192 ≤ PLLI2SN ≤ 432
   000000000: PLLI2SN = 0, wrong configuration
   000000001: PLLI2SN = 1, wrong configuration
   ...
   011000000: PLLI2SN = 192
   011000001: PLLI2SN = 193
   011000010: PLLI2SN = 194
   ...
   110110000: PLLI2SN = 432
   110110000: PLLI2SN = 433, wrong configuration
   ...
   111111111: PLLI2SN = 511, wrong configuration */

/* [5..0] Reserved, always read as 0. */


#endif /* STM32F2X */
/* ------------------------------------------------------------------------- */


/* ------------------------------------------------------------------------- */
#if defined(STM32F1X)

/* ------------------------------------------------------------------------- */
/* AHB peripheral clock enable register */
#define STM32F_RCC_AHBENR 0x14

/* Bit 10 - SDIO clock enable */
#define RCC_SDIOEN (1 << 10)
/* 
Set and cleared by software.
0: SDIO clock disabled
1: SDIO clock enabled
Bits 9
Reserved, always read as 0. */

/* Bit 8 - FSMC clock enable */
#define RCC_FSMCEN (1 << 8)
/* 
Set and cleared by software.
0: FSMC clock disabled
1: FSMC clock enabled
Bit 7
Reserved, always read as 0. */

/* Bit 6 - CRC clock enable */
#define RCC_CRCEN (1 << 6)
/* 
Set and cleared by software.
0: CRC clock disabled
1: CRC clock enabled */

/* Bit 5 Reserved, must be kept at reset value. */


/* Bit 4 - FLITF clock enable */
#define RCC_FLITFEN (1 << 4)
/* 
Set and cleared by software to disable/enable FLITF clock during Sleep mode.
0: FLITF clock disabled during Sleep mode
1: FLITF clock enabled during Sleep mode */

/* Bit 3 Reserved, must be kept at reset value. */


/* Bit 2 - SRAM interface clock enable */
#define RCC_SRAMEN (1 << 2)
/* 
Set and cleared by software to disable/enable SRAM interface clock during Sleep mode.
0: SRAM interface clock disabled during Sleep mode.
1: SRAM interface clock enabled during Sleep mode */

/* Bit 1 - DMA2 clock enable */
#define RCC_DMA2EN (1 << 1)
/* 
Set and cleared by software.
0: DMA2 clock disabled
1: DMA2 clock enabled */

/* Bit 0 - DMA1 clock enable */
#define RCC_DMA1EN (1 << 0)
/* 
Set and cleared by software.
0: DMA1 clock disabled
1: DMA1 clock enabled */

/* APB2 peripheral clock enable register */
#define STM32F_RCC_APB2ENR 0x18

/* [31..22] Reserved, must be kept at reset value. */


/* Bit 21 - TIM11 timer clock enable */
#define RCC_TIM11EN (1 << 21)
/* 
Set and cleared by software.
0: TIM11 timer clock disabled
1: TIM11 timer clock enabled */

/* Bit 20 - TIM10 timer clock enable */
#define RCC_TIM10EN (1 << 20)
/* Set and cleared by software.
0: TIM10 timer clock disabled
1: TIM10 timer clock enabled */

/* Bit 19 - TIM9 timer clock enable */
#define RCC_TIM9EN (1 << 19)
/*  Set and cleared by software.
0: TIM9 timer clock disabled
1: TIM9 timer clock enabled */


/* Bit 18 - TIM17 timer clock enable */
#define RCC_TIM17EN (1 << 18)
/* Set and cleared by software.
0: TIM17 timer clock disabled
1: TIM17 timer clock enabled */

/* Bit 17 - TIM16 timer clock enable */
#define RCC_TIM16EN (1 << 17)
/* Set and cleared by software.
0: TIM16 timer clock disabled
1: TIM16 timer clock enabled */

/* Bit 16 - TIM15 timer clock enable */
#define RCC_TIM15EN (1 << 16)
/* Set and cleared by software.
0: TIM15 timer clock disabled
1: TIM15 timer clock enabled */


/* Bit 15 - ADC3 interface clock enable */
#define RCC_ADC3EN (1 << 15)
/* 
Set and cleared by software.
0: ADC3 interface clock disabled
1: ADC3 interface clock enabled */ 

/* Bit 14 - USART1 clock enable */
#define RCC_USART1EN (1 << 14)
/* 
Set and cleared by software.
0: USART1 clock disabled
1: USART1 clock enabled */

/* Bit 13 - TIM8 Timer clock enable */
#define RCC_TIM8EN (1 << 13)
/* 
Set and cleared by software.
0: TIM8 timer clock disabled
1: TIM8 timer clock enabled */

/* Bit 12 - SPI1 clock enable */
#define RCC_SPI1EN (1 << 12)
/* 
Set and cleared by software.
0: SPI1 clock disabled
1: SPI1 clock enabled */

/* Bit 11 - TIM1 timer clock enable */
#define RCC_TIM1EN (1 << 11)
/* 
Set and cleared by software.
0: TIM1 timer clock disabled
1: TIM1 timer clock enabled */

/* Bit 10 - ADC 2 interface clock enable */
#define RCC_ADC2EN (1 << 10)
/* 
Set and cleared by software.
0: ADC 2 interface clock disabled
1: ADC 2 interface clock enabled */

/* Bit 9 - ADC 1 interface clock enable */
#define RCC_ADC1EN (1 << 9)
/* 
Set and cleared by software.
0: ADC 1 interface disabled
1: ADC 1 interface clock enabled */

/* Bit 8 - IO port G clock enable */
#define RCC_IOPGEN (1 << 8)
/* 
Set and cleared by software.
0: IO port G clock disabled
1: IO port G clock enabled */

/* Bit 7 - IO port F clock enable */
#define RCC_IOPFEN (1 << 7)
/* 
Set and cleared by software.
0: IO port F clock disabled
1: IO port F clock enabled */

/* Bit 6 - IO port E clock enable */
#define RCC_IOPEEN (1 << 6)
/* 
Set and cleared by software.
0: IO port E clock disabled
1: IO port E clock enabled */

/* Bit 5 - IO port D clock enable */
#define RCC_IOPDEN (1 << 5)
/* 
Set and cleared by software.
0: IO port D clock disabled
1: IO port D clock enabled */

/* Bit 4 - IO port C clock enable */
#define RCC_IOPCEN (1 << 4)
/* 
Set and cleared by software.
0: IO port C clock disabled
1: IO port C clock enabled */

/* Bit 3 - IO port B clock enable */
#define RCC_IOPBEN (1 << 3)
/* 
Set and cleared by software.
0: IO port B clock disabled
1: IO port B clock enabled */

/* Bit 2 - IO port A clock enable */
#define RCC_IOPAEN (1 << 2)
/* 
Set and cleared by software.
0: IO port A clock disabled
1: IO port A clock enabled */

/* Bit 1 Reserved, must be kept at reset value. */

/* Bit 0 - Alternate function IO clock enable */
#define RCC_AFIOEN (1 << 0)
/* 
Set and cleared by software.
0: Alternate Function IO clock disabled
1: Alternate Function IO clock enabled */

/* APB1 peripheral clock enable register */
#define STM32F_RCC_APB1ENR 0x1c


/* Bit 30 - CEC  clock enable */
#define RCC_CECEN (1 << 30)
/*
Set and cleared by software.
0: CEC  clock disabled
1: CEC  clock enable */

/* Bit 29 - DAC interface clock enable */
#define RCC_DACEN (1 << 29)
/* 
Set and cleared by software.
0: DAC interface clock disabled
1: DAC interface clock enable */

/* Bit 28 - Power interface clock enable */
#define RCC_PWREN (1 << 28)
/* 
Set and cleared by software.
0: Power interface clock disabled
1: Power interface clock enable */

/* Bit 27 - Backup interface clock enable */
#define RCC_BKPEN (1 << 27)
/* 
Set and cleared by software.
0: Backup interface clock disabled
1: Backup interface clock enabled */

/* Bit 26 Reserved, must be kept at reset value. */


/* Bit 25 - CAN clock enable */
#define RCC_CANEN (1 << 25)
/* 
Set and cleared by software.
0: CAN clock disabled
1: CAN clock enabled */

/* Bit 24 Reserved, always read as 0. */

/* Bit 23 - USB clock enable */
#define RCC_USBEN (1 << 23)
/* 
Set and cleared by software.
0: USB clock disabled
1: USB clock enabled */

/* Bit 22 - I2C2 clock enable */
#define RCC_I2C2EN (1 << 22)
/* 
Set and cleared by software.
0: I2C2 clock disabled
1: I2C2 clock enabled */

/* Bit 21 - I2C1 clock enable */
#define RCC_I2C1EN (1 << 21)
/* 
Set and cleared by software.
0: I2C1 clock disabled
1: I2C1 clock enabled */

/* Bit 20 - USART5 clock enable */
#define RCC_UART5EN (1 << 20)
/* 
Set and cleared by software.
0: USART5 clock disabled
1: USART5 clock enabled */

/* Bit 19 - USART4 clock enable */
#define RCC_UART4EN (1 << 19)
/* 
Set and cleared by software.
0: USART4 clock disabled
1: USART4 clock enabled */

/* Bit 18 - USART3 clock enable */
#define RCC_USART3EN (1 << 18)
/* 
Set and cleared by software.
0: USART3 clock disabled
1: USART3 clock enabled */

/* Bit 17 - USART2 clock enable */
#define RCC_USART2EN (1 << 17)
/* 
Set and cleared by software.
0: USART2 clock disabled
1: USART2 clock enabled */

/* Bit 16 Reserved, always read as 0. */

/* Bit 15 - SPI 3 clock enable */
#define RCC_SPI3EN (1 << 15)
/* 
Set and cleared by software.
0: SPI 3 clock disabled
1: SPI 3 clock enabled */

/* Bit 14 - SPI2 clock enable */
#define RCC_SPI2EN (1 << 14)
/* 
Set and cleared by software.
0: SPI2 clock disabled
1: SPI2 clock enabled */

/* [13..12] Reserved, must be kept at reset value. */

/* Bit 11 - Window watchdog clock enable */
#define RCC_WWDGEN (1 << 11)
/* 
Set and cleared by software.
0: Window watchdog clock disabled
1: Window watchdog clock enabled */

/* [10..9] Reserved, must be kept at reset value. */

/* Bit 8 - TIM14 timer clock enable */
#define RCC_TIM14EN (1 << 8)
/* 
Set and cleared by software.
0: TIM14 clock disabled
1: TIM14 clock enabled */

/* Bit 7 - TIM13 timer clock enable */
#define RCC_TIM13EN (1 << 7)
/* 
Set and cleared by software.
0: TIM13 clock disabled
1: TIM13 clock enabled */

/* Bit 6 - TIM12 timer clock enable */
#define RCC_TIM12EN (1 << 6)
/* 
Set and cleared by software.
0: TIM12 clock disabled
1: TIM12 clock enabled */

/* Bit 5 - TIM7 timer clock enable */
#define RCC_TIM7EN (1 << 5)
/* 
Set and cleared by software.
0: TIM7 clock disabled
1: TIM7 clock enabled */

/* Bit 4 - TIM6 timer clock enable */
#define RCC_TIM6EN (1 << 4)
/* 
Set and cleared by software.
0: TIM6 clock disabled
1: TIM6 clock enabled */

/* Bit 3 - TIM5 timer clock enable */
#define RCC_TIM5EN (1 << 3)
/* 
Set and cleared by software.
0: TIM5 clock disabled
1: TIM5 clock enabled */

/* Bit 2 - TIM4 timer clock enable */
#define RCC_TIM4EN (1 << 2)
/* 
Set and cleared by software.
0: TIM4 clock disabled
1: TIM4 clock enabled */

/* Bit 1 - TIM3 timer clock enable */
#define RCC_TIM3EN (1 << 1)
/* 
Set and cleared by software.
0: TIM3 clock disabled
1: TIM3 clock enabled */

/* Bit 0 - TIM2 timer clock enable */
#define RCC_TIM2EN (1 << 0)
/* 
Set and cleared by software.
0: TIM2 clock disabled
1: TIM2 clock enabled */

#endif /* STM32F1X */

#ifndef __ASSEMBLER__

#include <stdint.h>

#if defined(STM32F2X) || defined(STM32F4X)
struct stm32f_rcc {
	volatile uint32_t cr; /* Control Register */
	volatile uint32_t pllcfgr; 
	volatile uint32_t cfgr; 
	volatile uint32_t cir; 

	volatile uint32_t ahb1rstr;
	volatile uint32_t ahb2rstr;
	volatile uint32_t ahb3rstr;
	uint32_t res0; 

	volatile uint32_t apb1rstr;
	volatile uint32_t pb2rstr;
	uint32_t res1[2]; 

	volatile uint32_t ahb1enr;
	volatile uint32_t ahb2enr;
	volatile uint32_t ahb3enr;
	uint32_t res2; 

	volatile uint32_t apb1enr;
	volatile uint32_t apb2enr;
	uint32_t res3[2]; 

	volatile uint32_t ahb1lpenr;
	volatile uint32_t hb2lpenr; 
	volatile uint32_t ahb3lpenr;
	uint32_t res4; 

	volatile uint32_t apb1lpenr;
	volatile uint32_t apb2lpenr;
	uint32_t res5[2]; 

	volatile uint32_t bdcr;
	volatile uint32_t csr;
	uint32_t res6[2]; 

	volatile uint32_t sscgr;
	volatile uint32_t plli2scfgr;
};
#endif /* STM32F2X || STM32F4X  */

#if defined(STM32F1X)
struct stm32f_rcc {
	volatile uint32_t cr; /* Control Register */
	volatile uint32_t cfgr; 
	volatile uint32_t cir; 
	volatile uint32_t apb2rstr;

	volatile uint32_t apb1rstr;
	volatile uint32_t ahbenr;
	volatile uint32_t apb2enr;
	volatile uint32_t apb1enr;

	volatile uint32_t bdcr;
	volatile uint32_t csr;
	volatile uint32_t ahbstr;
	volatile uint32_t cfgr2; 
};
#endif /* STM32F1X */

#define STM32F_APB1 0
#define STM32F_APB2 1

struct stm32f_clk {
	uint8_t apb:3;
	uint8_t bit:5;
} __attribute__((packed))__;

extern const struct stm32f_usart * stm32f_usart_lut[];

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

static inline void stm32f_clk_enable(struct stm32f_rcc * rcc, 
									 struct stm32f_clk clk) {
	if (clk.apb == STM32F_APB2)
		rcc->apb2enr |= 1 << (clk.bit);
	else
		rcc->apb1enr |= 1 << (clk.bit);
}

static inline void stm32f_clk_disable(struct stm32f_rcc * rcc,
									  struct stm32f_clk clk) {
	if (clk.apb == STM32F_APB2)
		rcc->apb2enr &= ~(1 << (clk.bit));
	else
		rcc->apb1enr &= ~(1 << (clk.bit));
}

#endif /* __ASSEMBLER__ */

#endif /* __STM32F_RCC_H__ */

