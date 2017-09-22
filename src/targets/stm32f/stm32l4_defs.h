/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the YARD-ICE.
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
 * @file stm32l4_defs.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#if defined(STM32L4XX)

#define STM32F_BASE_DBGMCU  0xe0042000

#define STM32F_BASE_RNG     0x50060800
#define STM32F_BASE_AES     0x50060000
#define STM32F_BASE_ADC     0x50040000

#define STM32F_BASE_TSC     0x40024000
#define STM32F_BASE_CRC     0x40023000
#define STM32F_BASE_FLASH   0x40022000
#define STM32F_BASE_RCC     0x40021000
#define STM32F_BASE_DMA2    0x40020400
#define STM32F_BASE_DMA1    0x40020000

#define STM32F_BASE_GPIOH   0x40001c00
#define STM32F_BASE_GPIOE   0x40001000
#define STM32F_BASE_GPIOD   0x40000c00
#define STM32F_BASE_GPIOC   0x40000800
#define STM32F_BASE_GPIOB   0x40000400
#define STM32F_BASE_GPIOA   0x40000000


#define STM32F_BASE_USART1  0x40003800
#define STM32F_BASE_SPI1    0x40013000
#define STM32F_BASE_SDIO    0x40012c00
#define STM32F_BASE_TIM11   0x40011000
#define STM32F_BASE_TIM10   0x40010c00
#define STM32F_BASE_TIM9    0x40010800
#define STM32F_BASE_EXTI    0x40010400
#define STM32F_BASE_SYSCFG  0x40010000
#define STM32F_BASE_OPAMP   0x40007c5c
#define STM32F_BASE_RI      0x40007c04
#define STM32F_BASE_COMP    0x40007c00
#define STM32F_BASE_DAC     0x40007400
#define STM32F_BASE_PWR     0x40007000
#define STM32F_BASE_USB_FS  0x40005c00
#define STM32F_BASE_I2C2    0x40005800
#define STM32F_BASE_I2C1    0x40005400
#define STM32F_BASE_UART5   0x40005000
#define STM32F_BASE_UART4   0x40004c00
#define STM32F_BASE_USART3  0x40004800
#define STM32F_BASE_USART2  0x40004400
#define STM32F_BASE_SPI3    0x40003c00
#define STM32F_BASE_SPI2    0x40003800
#define STM32F_BASE_IWDG    0x40003000
#define STM32F_BASE_WWDG    0x40002c00
#define STM32F_BASE_RTC     0x40002800
#define STM32F_BASE_LCD     0x40002400
#define STM32F_BASE_TIM7    0x40001400
#define STM32F_BASE_TIM6    0x40001000
#define STM32F_BASE_TIM5    0x40000c00
#define STM32F_BASE_TIM4    0x40000800
#define STM32F_BASE_TIM3    0x40000400
#define STM32F_BASE_TIM2    0x40000000

#else 
#error "STM32L family undefined or invalid!"
#endif 

/*-------------------------------------------------------------------------
  RCC
  -------------------------------------------------------------------------*/

/* ------------------------------------------------------------------------- */
/* RCC clock control register */ 
#define RCC_CR 0x00

#define PLLSAI1RDY (1 << 27)
/* PLLSAI1 clock ready flag
   Set by hardware to indicate that the PLLSAI1 is locked.
0: PLLSAI1 unlocked
1: PLLSAI1 locked*/

#define PLLSAI1ON (1 << 26)
/* PLLSAI1 enable
   Set and cleared by software to enable PLLSAI1.
   Cleared by hardware when entering Stop or Standby mode.
0: PLLSAI1 OFF
1: PLLSAI1 ON*/

#define PLLRDY (1 << 25)
/* Main PLL (PLL) clock ready flag
   Set by hardware to indicate that PLL is locked.
0: PLL unlocked
1: PLL locked*/

#define PLLON (1 << 24)
/* Main PLL (PLL) enable
   Set and cleared by software to enable PLL.
   Cleared by hardware when entering Stop or Standby mode. This bit cannot be reset if PLL
   clock is used as the system clock.
0: PLL OFF
1: PLL ON
 */

#define CSSON (1 << 19)
/* Clock security system enable
   Set and cleared by software to enable the clock security system. When CSSON is set, the
   clock detector is enabled by hardware when the HSE oscillator is ready, and disabled by
   hardware if an oscillator failure is detected.
0: Clock security system OFF (Clock detector OFF)
1: Clock security system ON (Clock detector ON if HSE oscillator is stable, OFF if not)*/

#define HSEBYP (1 << 18)
/* HSE clock bypass
   Set and cleared by software to bypass the oscillator with an external clock. The external
   clock must be enabled with the HSEON bit, to be used by the device.
   The HSEBYP bit can be written only if the HSE oscillator is disabled.
0: HSE oscillator not bypassed
1: HSE oscillator bypassed with an external clock*/

#define HSERDY (1 << 17)
/* HSE clock ready flag
   Set by hardware to indicate that the HSE oscillator is stable. After the HSION bit is cleared,
   HSERDY goes low after 6 HSE oscillator clock cycles.
0: HSE oscillator not ready
1: HSE oscillator ready*/

#define HSEON (1 << 16)
/* HSE clock enable
   Set and cleared by software.
   Cleared by hardware to stop the HSE oscillator when entering Stop or Standby mode. This
   bit cannot be reset if the HSE oscillator is used directly or indirectly as the system clock.
0: HSE oscillator OFF
1: HSE oscillator ON*/



#define HSIASFS (1 << 11)
/*
HSIASFS: HSI16 automatic start from Stop
Set and cleared by software. When the system wakeup clock is MSI, this bit is used to
wakeup the HSI16 is parallel of the system wakeup.
0: HSI16 oscillator is not enabled by hardware when exiting Stop mode with MSI as wakeup
clock.
1: HSI16 oscillator is enabled by hardware when exiting Stop mode with MSI as wakeup
clock.
*/

#define HSIRDY (1 << 10)
/* Internal high-speed clock ready flag
   Set by hardware to indicate that the HSI oscillator is stable. After the HSION bit is cleared,
   HSIRDY goes low after 6 HSI clock cycles.
0: HSI oscillator not ready
1: HSI oscillator ready*/

#define HSIKERON (1 << 9)
/*Bit 10 HSIRDY: HSI16 clock ready flag
Set by hardware to indicate that HSI16 oscillator is stable. This bit is set only when HSI16 is
enabled by software by setting HSION.
0: HSI16 oscillator not ready
1: HSI16 oscillator ready
Note: Once the HSION bit is cleared, HSIRDY goes low after 6 HSI16 clock cycles.
Bit 9 HSIKERON: HSI16 always enable for peripheral kernels.
Set and cleared by software to force HSI16 ON even in Stop modes. The HSI16 can only
feed USARTs and I2Cs peripherals configured with HSI16 as kernel clock. Keeping the
HSI16 ON in Stop mode allows to avoid slowing down the communication speed because of
the HSI16 startup time. This bit has no effect on HSION value.
0: No effect on HSI16 oscillator.
1: HSI16 oscillator is forced ON even in Stop mode.
Bit 8 HSION: HSI16 clock enable
Set and cleared by software.
Cleared by hardware to stop the HSI16 oscillator when entering Stop, Standby or Shutdown
mode.
Set by hardware to force the HSI16 oscillator ON when STOPWUCK=1 or HSIASFS = 1
when leaving Stop modes, or in case of failure of the HSE crystal oscillator.
This bit is set by hardware if the HSI16 is used directly or indirectly as system clock.
0: HSI16 oscillator OFF
1: HSI16 oscillator ON
*/

#define HSION (1 << 8)
/* Internal high-speed clock enable
   Set and cleared by software.
   Set by hardware to force the HSI oscillator ON when leaving the Stop or Standby mode or in
   case of a failure of the HSE oscillator used directly or indirectly as the system clock. This bit
   cannot be cleared if the HSI is used directly or indirectly as the system clock.
0: HSI oscillator OFF
1: HSI oscillator ON
 */

/*
MSIRANGE[3:0]: MSI clock ranges
These bits are configured by software to choose the frequency range of MSI when
MSIRGSEL is set.12 frequency ranges are available:
0000: range 0 around 100 kHz
0001: range 1 around 200 kHz
0010: range 2 around 400 kHz
0011: range 3 around 800 kHz
0100: range 4 around 1M Hz
0101: range 5 around 2 MHz
0110: range 6 around 4 MHz (reset value)
0111: range 7 around 8 MHz
1000: range 8 around 16 MHz
1001: range 9 around 24 MHz
1010: range 10 around 32 MHz
1011: range 11 around 48 MHz
others: not allowed (hardware write protection)
Note: Warning: MSIRANGE can be modified when MSI is OFF (MSION=0) or when MSI is
ready (MSIRDY=1). MSIRANGE must NOT be modified when MSI is ON and NOT
ready (MSION=1 and MSIRDY=0)
	Bit 3 MSIRGSEL: MSI clock range selection
	Set by software to select the MSI clock range with MSIRANGE[3:0]. Write 0 has no effect.
	After a standby or a reset MSIRGSEL is at 0 and the MSI range value is provided by
	MSISRANGE in CSR register.
	0: MSI Range is provided by MSISRANGE[3:0] in RCC_CSR register
	1: MSI Range is provided by MSIRANGE[3:0] in the RCC_CR register
	Bit 2 MSIPLLEN: MSI clock PLL enable
	Set and cleared by software to enable/ disable the PLL part of the MSI clock source.
	MSIPLLEN must be enabled after LSE is enabled (LSEON enabled) and ready (LSERDY set
																			 by hardware).There is a hardware protection to avoid enabling MSIPLLEN if LSE is not
	ready.
	This bit is cleared by hardware when LSE is disabled (LSEON = 0) or when the Clock
	Security System on LSE detects a LSE failure (refer to RCC_CSR register).
	0: MSI PLL OFF
	1: MSI PLL ON
	Bit 1 MSIRDY: MSI clock ready flag
	This bit is set by hardware to indicate that the MSI oscillator is stable.
	0: MSI oscillator not ready
	1: MSI oscillator ready
	Note: Once the MSION bit is cleared, MSIRDY goes low after 6 MSI clock cycles
	*/

#define MSION (1 << 0)
/* MSI clock enable
   This bit is set and cleared by software.
   Set by hardware to force the MSI oscillator ON when exiting from Stop 
   or Standby mode, or in case of a failure of the HSE oscillator used 
   directly or indirectly as system clock. This bit cannot be cleared if the 
   MSI is used as system clock.
 */



/* ------------------------------------------------------------------------- */
/* RCC clock configuration register */

/* STM32F10x RCC clock configuration register */
#define RCC_CFGR 0x08

#define MCO_SYSCLK (0x1 << 24)
#define MCO_MSI    (0x2 << 24)
#define MCO_HSI    (0x3 << 24)
#define MCO_HSE    (0x4 << 24)
#define MCO_PLL    (0x5 << 24)
#define MCO_LSI    (0x6 << 24)
#define MCO_LSE    (0x7 << 24)
#define MCO_HSI48  (0x8 << 24)

#define STOPWUCK (1 << 15)

/*
	Bit 15 STOPWUCK: Wakeup from Stop and CSS backup clock selection
	Set and cleared by software to select the system clock used when exiting Stop mode.
	The selected clock is also used as emergency clock for the Clock Security System on HSE.
	Warning: STOPWUCK must not be modified when the Clock Security System is enabled by
	HSECSSON in RCC_CR register and the system clock is HSE (SWS=”10”) or a switch on
	HSE is requested (SW=”10”).
	0: MSI oscillator selected as wakeup from stop clock and CSS backup clock.
	1: HSI16 oscillator selected as wakeup from stop clock and CSS backup clock
*/

#define PPRE2    (0x7 << 11)
#define PPRE2_1  (0x0 << 11)
#define PPRE2_2  (0x4 << 11)
#define PPRE2_4  (0x5 << 11)
#define PPRE2_8  (0x6 << 11)
#define PPRE2_16 (0x7 << 11)
/* APB high-speed prescaler (APB2) 
   These bits are set and cleared by software to control the division 
   factor of the APB high- speed clock (PCLK2).
0xx: HCLK not divided
100: HCLK divided by 2
101: HCLK divided by 4
110: HCLK divided by 8
111: HCLK divided by 16 */

#define PPRE1    (0x7 << 8)
#define PPRE1_1  (0x0 << 8)
#define PPRE1_2  (0x4 << 8)
#define PPRE1_4  (0x5 << 8)
#define PPRE1_8  (0x6 << 8)
#define PPRE1_16 (0x7 << 8)
/* APB low-speed prescaler (APB1) 
 */

#define HPRE (0xf << 4)
#define HPRE_1 (0x0 << 4)
#define HPRE_2 (0x8 << 4)
#define HPRE_4 (0x9 << 4)
#define HPRE_8 (0xa << 4)
#define HPRE_16 (0xb << 4)
#define HPRE_64 (0xc << 4)
#define HPRE_128 (0xd << 4)
#define HPRE_256 (0xe << 4)
#define HPRE_512 (0xf << 4)
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

#define SWS (0x3 << 2)
#define SWS_MSI (0x0 << 2)
#define SWS_HSI (0x1 << 2)
#define SWS_HSE (0x2 << 2)
#define SWS_PLL (0x3 << 2)
/* System clock switch status. Set and cleared by hardware to indicate which 
   clock source is used as the system clock.
00: HSI oscillator used as the system clock
01: HSE oscillator used as the system clock
10: PLL used as the system clock
11: not applicable */

#define SW (0x3 << 0)
#define SW_MSI (0x0 << 0)
#define SW_HSI (0x1 << 0)
#define SW_HSE (0x2 << 0)
#define SW_PLL (0x3 << 0)

/* System clock switch
   Set and cleared by software to select the system clock source.
   Set by hardware to force the HSI selection when leaving the Stop or Standby mode or in case of failure of the HSE oscillator used directly or indirectly as the system clock.
00: HSI oscillator selected as system clock
01: HSE oscillator selected as system clock
10: PLL selected as system clock
11: not allowed
 */

/* ------------------------------------------------------------------------- */
/* RCC PLL configuration register */
#define RCC_PLLCFGR 0x0C

#define PLLQ_MASK (0xf << 24)
#define PLLQ(Q) (((Q) & 0xf) << 24)
/* Main PLL (PLL) division factor for USB OTG FS, SDIO and random number generator clocks Set and cleared by software to control the frequency of USB OTG FS clock, the random number generator clock and the SDIO clock. These bits should be written only if PLL is disabled.
Caution: The USB OTG FS requires a 48 MHz clock to work correctly. The SDIO and the random number generator need a frequency lower than or equal to 48 MHz to work correctly.
USB OTG FS clock frequency = VCO frequency / PLLQ with 4 <= PLLQ <= 15
0000: PLLQ = 0, wrong configuration
...
0011: PLLQ = 3, wrong configuration
0100: PLLQ = 4
0101: PLLQ = 5
...
1111: PLLQ = 15
Bit 23 Reserved, always read as 0.*/

#define PLLSRC (1 << 22)
#define PLLSRC_HSI (0 << 22)
#define PLLSRC_HSE (1 << 22)
/* Main PLL(PLL) and audio PLL (PLLI2S) entry clock source Set and cleared by software to select PLL and PLLI2S clock source. This bit can be written only when PLL and PLLI2S are disabled.
0: HSI clock selected as PLL and PLLI2S clock entry
1: HSE oscillator clock selected as PLL and PLLI2S clock entr*/

#define PLLP_MASK (0x3 << 16)
#define PLLP(P) (((((P) >> 1) - 1)  & 0x3) << 16)
#define PLLP_2 (0x0 << 16)
#define PLLP_4 (0x1 << 16)
#define PLLP_6 (0x2 << 16)
#define PLLP_8 (0x3 << 16)
/* Main PLL (PLL) division factor for main system clock Set and cleared by software to control the frequency of the general PLL output clock. These bits can be written only if PLL is disabled.
Caution: The software has to set these bits correctly not to exceed 120 MHz on this domain.
PLL output clock frequency = VCO frequency / PLLP with PLLP = 2, 4, 6, or 8
00: PLLP = 2
01: PLLP = 4
10: PLLP = 6
11: PLLP = 8 */

#define PLLN_MASK (0xff << 6)
#define PLLN(N) (((N) & 0xff) << 6)
/* Main PLL (PLL) multiplication factor for VCO Set and cleared by software to control the multiplication factor of the VCO. These bits can be written only when PLL is disabled. Only half-word and word accesses are allowed to write these bits.
Caution: The software has to set these bits correctly to ensure that the VCO output
frequency is between 192 and 432 MHz.
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
111111111: PLLN = 511, wrong configuration*/

#define PLLM_MASK (0x1f << 0)
#define PLLM(M) (((M) & 0x1f) << 0)
/* Division factor for the main PLL (PLL) and audio PLL (PLLI2S) input clock
Set and cleared by software to divide the PLL and PLLI2S input clock before the VCO. These bits can be written only when the PLL and PLLI2S are disabled. Caution: The software has to set these bits correctly to ensure that the VCO input frequency ranges from 1 to 2 MHz. It is recommended to select a frequency of 2 MHz to limit PLL jitter.
VCO input frequency = PLL input clock frequency / PLLM with 2 ≤ PLLM ≤ 63
000000: PLLM = 0, wrong configuration
000001: PLLM = 1, wrong configuration
000010: PLLM = 2
000011: PLLM = 3
000100: PLLM = 4
...
111110: PLLM = 62
111111: PLLM = 63 */

/* RCC AHB1 peripheral clock register */
#define RCC_AHB1ENR 0x48

#define FLASHEN (1 << 8)

#define DMA2EN (1 << 1)
/* DMA2 clock enable
   Set and cleared by software.
0: DMA2 clock disabled
1: DMA2 clock enabled*/
#define DMA1EN (1 << 0)
/* DMA1 clock enable
   Set and cleared by software.
0: DMA1 clock disabled
1: DMA1 clock enabled




Bits 20:19 Reserved, always read as 0.*/
#define BKPSRAMEN (1 << 18)
/* Backup SRAM interface clock enable
   Set and cleared by software.
0: Backup SRAM interface clock disabled
1: Backup SRAM interface clock enabled
Bits 17:13 Reserved, always read as 0.*/
#define CRCEN (1 << 12)
/* CRC clock enable
   Set and cleared by software.
0: CRC clock disabled
1: CRC clock enabled
Bits 11:9 Reserved, always read as 0.*/
#define GPIOIEN (1 << 8)
/* IO port I clock enable
   Set and cleared by software.
0: IO port I clock disabled
1: IO port I clock enabled*/
#define GPIOHEN (1 << 7)
/* IO port H clock enable
   Set and cleared by software.
0: IO port H clock disabled
1: IO port H clock enabled*/
#define GPIOGEN (1 << 6)
/* IO port G clock enable
   Set and cleared by software.
0: IO port G clock disabled
1: IO port G clock enabled*/
#define GPIOFEN (1 << 5)
/* IO port F clock enable
   Set and cleared by software.
0: IO port F clock disabled
1: IO port F clock enabled*/
#define GPIOEEN (1 << 4)
/* IO port E clock enable
   Set and cleared by software.
0: IO port E clock disabled
1: IO port E clock enabled*/
#define GPIODEN (1 << 3)
/* IO port D clock enable
   Set and cleared by software.
0: IO port D clock disabled
1: IO port D clock enabled*/
#define GPIOCEN (1 << 2)
/* IO port C clock enable
   Set and cleared by software.
0: IO port C clock disabled
1: IO port C clock enabled*/
#define GPIOBEN (1 << 1)
/* IO port B clock enable
   Set and cleared by software.
0: IO port B clock disabled
1: IO port B clock enabled*/
#define GPIOAEN (1 << 0)
/* IO port A clock enable
   Set and cleared by software.
0: IO port A clock disabled
1: IO port A clock enabled */

/* RCC Backup domain control register */
#define RCC_BDCR 0x70

#define BDRST (1 << 16)
/* Backup domain software reset
   Set and cleared by software.
0: Reset not activated
1: Resets the entire Backup domain
Note: The BKPSRAM is not affected by this reset, the only way of resetting the BKPSRAM is
through the Flash interface when a protection level change from level 1 to level 0 is
requested. */

#define RTCEN (1 << 15)
/* RTC clock enable
   Set and cleared by software.
0: RTC clock disabled
1: RTC clock enabled
Bits 14:10
Reserved, always read as 0. */

#define RTCSEL (1 << 8)
#define RTCSEL_NONE (0 << 8)
#define RTCSEL_LSE (1 << 8)
#define RTCSEL_LSI (2 << 8)
#define RTCSEL_HSE (3 << 8)
/* RTC clock source selection
   Set by software to select the clock source for the RTC. Once the RTC clock source has been
   selected, it cannot be changed anymore unless the Backup domain is reset. The BDRST bit
   can be used to reset them.
00: No clock
01: LSE oscillator clock used as the RTC clock
10: LSI oscillator clock used as the RTC clock
11: HSE oscillator clock divided by a programmable prescaler (selection through the RTCPRE[4:0] bits in the RCC clock configuration register (RCC_CFGR)) used as the RTC clock */

#define LSEBYP (1 << 2)
/* External low-speed oscillator bypass
   Set and cleared by software to bypass oscillator in debug mode. This bit can be written only
   when the LSE clock is disabled.
0: LSE oscillator not bypassed
1: LSE oscillator bypassed */

#define LSERDY (1 << 1)
/* External low-speed oscillator ready. Set and cleared by hardware to 
   indicate when the external 32 kHz oscillator is stable. 
   After the LSEON bit is cleared, LSERDY goes low after 6 external 
   low-speed oscillator clock cycles.
0: LSE clock not ready
1: LSE clock ready */

#define LSEON (1 << 0)
/* External low-speed oscillator enable
   Set and cleared by software.
0: LSE clock OFF
1: LSE clock ON */

/* RCC clock control & status register */
#define RCC_CSR 0x74

#define LPWRRSTF (1 << 31)
/* Low-power reset flag
   Set by hardware when a Low-power management reset occurs.
   Cleared by writing to the RMVF bit.
0: No Low-power management reset occurred
1: Low-power management reset occurred
For further information on Low-power management reset, 
refer to Low-power management reset. */

#define WWDGRSTF (1 << 30)
/* Window watchdog reset flag
   Set by hardware when a window watchdog reset occurs.
   Cleared by writing to the RMVF bit.
0: No window watchdog reset occurred
1: Window watchdog reset occurred */

#define IWDGRSTF (1 << 29)
/* Independent watchdog reset flag
   Set by hardware when an independent watchdog reset from VDD domain occurs.
   Cleared by writing to the RMVF bit.
0: No watchdog reset occurred
1: Watchdog reset occurred */

#define SFTRSTF (1 << 28)
/* Software reset flag
   Set by hardware when a software reset occurs.
   Cleared by writing to the RMVF bit.
0: No software reset occurred
1: Software reset occurred */

#define PORRSTF (1 << 27)
/* POR/PDR reset flag
   Set by hardware when a POR/PDR reset occurs.
   Cleared by writing to the RMVF bit.
0: No POR/PDR reset occurred
1: POR/PDR reset occurred */

#define PINRSTF (1 << 26)
/* PIN reset flag
   Set by hardware when a reset from the NRST pin occurs.
   Cleared by writing to the RMVF bit.
0: No reset from NRST pin occurred
1: Reset from NRST pin occurred */

#define BORRSTF (1 << 25)
/* BOR reset flag
   Cleared by software by writing the RMVF bit.
   Set by hardware when a POR/PDR or BOR reset occurs.
0: No POR/PDR or BOR reset occurred
1: POR/PDR or BOR reset occurred */

#define RMVF (1 << 24)
/* Remove reset flag
   Set by software to clear the reset flags.
0: No effect
1: Clear the reset flags */

#define LSIRDY (1 << 1)
/* Internal low-speed oscillator ready
   Set and cleared by hardware to indicate when the internal RC 
   40 kHz oscillator is stable.
   After the LSION bit is cleared, LSIRDY goes low after 3 LSI clock cycles.
0: LSI RC oscillator not ready
1: LSI RC oscillator ready */

#define LSION (1 << 0)
/* Internal low-speed oscillator enable
   Set and cleared by software.
0: LSI RC oscillator OFF
1: LSI RC oscillator ON */

/* RCC APB1 peripheral clock enable register */
#define RCC_APB1ENR 0x40

#define DACEN (1 << 29)
/* DAC interface clock enable
   Set and cleared by software.
0: DAC interface clock disabled
1: DAC interface clock enable */

#define PWREN (1 << 28)
/* Power interface clock enable
   Set and cleared by software.
0: Power interface clock disabled
1: Power interface clock enable */

#define CAN2EN (1 << 26)
/* CAN 2 clock enable
   Set and cleared by software.
0: CAN 2 clock disabled
1: CAN 2 clock enabled */

#define CAN1EN (1 << 25)
/* CAN 1 clock enable
   Set and cleared by software.
0: CAN 1 clock disabled
1: CAN 1 clock enabled */

#define I2C3EN (1 << 23)
/* I2C3 clock enable
   Set and cleared by software.
0: I2C3 clock disabled
1: I2C3 clock enabled */

#define I2C2EN (1 << 22)
/* I2C2 clock enable
   Set and cleared by software.
0: I2C2 clock disabled
1: I2C2 clock enabled */

#define I2C1EN (1 << 21)
/* I2C1 clock enable
   Set and cleared by software.
0: I2C1 clock disabled
1: I2C1 clock enabled */

#define UART5EN (1 << 20)
/* UART5 clock enable
   Set and cleared by software.
0: UART5 clock disabled
1: UART5 clock enabled */

#define UART4EN (1 << 19)
/* UART4 clock enable
   Set and cleared by software.
0: UART4 clock disabled
1: UART4 clock enabled */

#define USART3EN (1 << 18)
/* USART3 clock enable
   Set and cleared by software.
0: USART3 clock disabled
1: USART3 clock enabled */

#define USART2EN (1 << 17)
/* USART 2 clock enable
   Set and cleared by software.
0: USART2 clock disabled
1: USART2 clock enabled */

#define SPI3EN (1 << 15)
/* SPI3 clock enable
   Set and cleared by software.
0: SPI3 clock disabled
1: SPI3 clock enabled */

#define SPI2EN (1 << 14)
/* SPI2 clock enable
   Set and cleared by software.
0: SPI2 clock disabled
1: SPI2 clock enabled */

#define WWDGEN (1 << 11)
/* Window watchdog clock enable
   Set and cleared by software.
0: Window watchdog clock disabled
1: Window watchdog clock enabled */

#define TIM14EN (1 << 8)
/* TIM14 clock enable
   Set and cleared by software.
0: TIM14 clock disabled
1: TIM14 clock enabled */

#define TIM13EN (1 << 7)
/* TIM13 clock enable
   Set and cleared by software.
0: TIM13 clock disabled
1: TIM13 clock enabled */

#define TIM12EN (1 << 6)
/* TIM12 clock enable
   Set and cleared by software.
0: TIM12 clock disabled
1: TIM12 clock enabled */

#define TIM7EN (1 << 5)
/* TIM7 clock enable
   Set and cleared by software.
0: TIM7 clock disabled
1: TIM7 clock enabled */

#define TIM6EN (1 << 4)
/* TIM6 clock enable
   Set and cleared by software.
0: TIM6 clock disabled
1: TIM6 clock enabled */

#define TIM5EN (1 << 3)
/* TIM5 clock enable
   Set and cleared by software.
0: TIM5 clock disabled
1: TIM5 clock enabled */

#define TIM4EN (1 << 2)
/* TIM4 clock enable
   Set and cleared by software.
0: TIM4 clock disabled
1: TIM4 clock enabled */

#define TIM3EN (1 << 1)
/* TIM3 clock enable
   Set and cleared by software.
0: TIM3 clock disabled
1: TIM3 clock enabled */

#define TIM2EN (1 << 0)
/* TIM2 clock enable
   Set and cleared by software.
0: TIM2 clock disabled
1: TIM2 clock enabled */

/* RCC APB2 peripheral clock enable register */
#define RCC_APB2ENR 0x44

#define TIM11EN (1 << 18)
/* TIM11 clock enable
   Set and cleared by software.
0: TIM11 clock disabled
1: TIM11 clock enabled */

#define TIM10EN (1 << 17)
/* TIM10 clock enable
   Set and cleared by software.
0: TIM10 clock disabled
1: TIM10 clock enabled */

#define TIM9EN (1 << 16)
/* TIM9 clock enable
   Set and cleared by software.
0: TIM9 clock disabled
1: TIM9 clock enabled */

#define SYSCFGEN (1 << 14)
/* System configuration controller clock enable
   Set and cleared by software.
0: System configuration controller clock disabled
1: System configuration controller clock enabled */

#define SPI1EN (1 << 12)
/* SPI1 clock enable
   Set and cleared by software.
0: SPI1 clock disabled
1: SPI1 clock enabled */

#define SDIOEN (1 << 11)
/* SDIO clock enable
   Set and cleared by software.
0: SDIO module clock disabled
1: SDIO module clock enabled */

#define ADC3EN (1 << 10)
/* ADC3 clock enable
   Set and cleared by software.
0: ADC3 clock disabled
1: ADC3 clock disabled
 */

#define ADC2EN (1 << 9)
/* ADC2 clock enable
   Set and cleared by software.
0: ADC2 clock disabled
1: ADC2 clock disabled */

#define ADC1EN (1 << 8)
/* ADC1 clock enable
   Set and cleared by software.
0: ADC1 clock disabled
1: ADC1 clock disabled
Bits 7:6
Reserved, always read as 0. */

#define USART6EN (1 << 5)
/* USART6 clock enable
   Set and cleared by software.
0: USART6 clock disabled
1: USART6 clock enabled */

#define USART1EN (1 << 4)
/* USART1 clock enable
   Set and cleared by software.
0: USART1 clock disabled
1: USART1 clock enabled
Bits 3:2
Reserved, always read as 0. */

#define TIM8EN (1 << 1)
/* TIM8 clock enable
   Set and cleared by software.
0: TIM8 clock disabled
1: TIM8 clock enabled */

#define TIM1EN (1 << 0)
/* TIM1 clock enable
   Set and cleared by software.
0: TIM1 clock disabled
1: TIM1 clock enabled */


/*-------------------------------------------------------------------------
  GPIO
  -------------------------------------------------------------------------*/

/* GPIO port mode register */
#define GPIO_MODER 0x00
#define MODE_INPUT(P) (0 << (2 * (P)))
#define MODE_OUTPUT(P) (1 << (2 * (P)))
#define MODE_ALT_FUNC(P) (2 << (2 * (P)))
#define MODE_ANALOG(P) (3 << (2 * (P)))

/* GPIO port output type register */
#define GPIO_OTYPER 0x04
#define PUSH_PULL(P) (0 << (P))
#define OPEN_DRAIN(P) (1 << (P))

/* GPIO port output speed register */
#define GPIO_OSPEEDR 0x08
#define OSPEEDR_LOW(P) (0 << (2 * (P)))
#define OSPEEDR_MED(P) (1 << (2 * (P)))
#define OSPEEDR_FAST(P) (2 << (2 * (P)))
#define OSPEEDR_HIGH(P) (3 << (2 * (P)))

/* GPIO alternate function low register */
#define GPIO_AFRL 0x20
#define AFRL(P, F) ((F) << (16 * (P)))

/* GPIO alternate function high register */
#define GPIO_AFRH 0x24
#define AFRH(P, F) ((F) << (16 * ((P) - 8)))

#define AF0 0
#define AF1 1
#define AF2 2
#define AF3 3
#define AF4 4
#define AF5 5
#define AF6 6
#define AF7 7
#define AF8 8
#define AF9 9
#define AF10 10
#define AF11 11
#define AF12 12
#define AF13 13
#define AF14 14
#define AF15 15

/*-------------------------------------------------------------------------
  Flash Interface
  -------------------------------------------------------------------------*/

/* Flash access control register */
#define FLASH_ACR 0x00
/*	The Flash access control register is used to enable/disable 
	the acceleration features and control the Flash memory access 
	time according to CPU frequency. */

#define SLEEP_PD (1 << 14)

#define RUN_PD (1 << 13)

#define DCRST (1 << 12)
/* Data cache reset
0: Data cache is not reset
1: Data cache is reset
This bit can be written only when the D cache is disabled.*/

#define ICRST (1 << 11)
/* Instruction cache reset
0: Instruction cache is not reset
1: Instruction cache is reset
This bit can be written only when the I cache is disabled.*/

#define DCEN (1 << 10)
/* Data cache enable
0: Data cache is disabled
1: Data cache is enabled*/

#define ICEN (1 << 9)
/* Instruction cache enable
0: Instruction cache is disabled
1: Instruction cache is enabled*/

#define PRFTEN (1 << 8)
/* Prefetch enable
0: Prefetch is disabled
1: Prefetch is enabled */

#define LATENCY(WS) (((WS) & 0x7) << 0)
/* Latency. These bits represent the ratio of the CPU clock period to the Flash memory access time. 
000: Zero wait state
001: One wait state
010: Two wait states
011: Three wait states
100: Four wait states */


/* ------------------------------------------------------------------------- */
/* Power down key register (FLASH_PDKEYR) */
#define FLASH_PDKEYR 0x04
/* The Power down key register is used to unlock 
   the RUN_PD bit in FLASH_ACR. */
#define PDKEY1 0x04152637
#define PDKEY2 0xfafbfcfd

/* ------------------------------------------------------------------------- */
/* Flash key register */
#define FLASH_KEYR 0x08
/* The Flash key register is used to allow access to the Flash control 
   register and so, to allow program and erase operations. */
#define FLASH_KEY1 0x45670123
#define FLASH_KEY2 0xcdef89ab

/* Flash option key register */
#define FLASH_OPTKEYR 0x0c
/* The Flash option key register is used to allow program and erase 
   operations in the user configuration sector. */
#define OPTKEY1 0x08192a3b
#define OPTKEY2 0x4c5d6e7f

/* ------------------------------------------------------------------------- */
/* Flash status register */
#define FLASH_SR 0x10
/* The Flash status register gives information on ongoing program and 
   erase operations. */

#define PEMPTY (1 << 17)
/* Set by hardware on power-on reset or after OBL_LAUNCH command execution
if the Flash is not programmed and the user intends to boot from the main Flash.
Cleared by hardware on power-on reset or after OBL_LAUNCH command
execution if the Flash is programmed and the user intends to boot from main
Flash. This bit can also be set and cleared by software.
1: The bit value is toggling
0: No effect
This bit can be set to clear the Program Empty bit if an OBL_LAUNCH is done by
software after Flash programming (boot in main flash selected). It finally forces
the boot in the main flash, without loosing the debugger connection.
*/

#define BSY (1 << 16)
/* Busy. This bit indicates that a Flash memory operation is in progress. 
   It is set at the beginning of a Flash memory operation and cleared 
   when the operation finishes or an error occurs.
	0: no Flash memory operation ongoing
	1: Flash memory operation ongoing */

#define OPTVERR (1 << 15)
/* Option validity error
   Set by hardware when the options read may not be the one configured by the
   user. If option haven’t been properly loaded, OPTVERR is set again after each
   system reset.
   Cleared by writing 1.
   */

#define RDERR (1 << 14)
/* PCROP read error
   Set by hardware when an address to be read through the D-bus belongs to a
   read protected area of the flash (PCROP protection). An interrupt is generated if
   RDERRIE is set in FLASH_CR.
   Cleared by writing 1.
   */

#define FASTERR (1 << 9)
/* Fast programming error
   Set by hardware when a fast programming sequence (activated by FSTPG) is
   interrupted due to an error (alignment, size, write protection or data miss). The
   corresponding status bit (PGAERR, SIZERR, WRPERR or MISSERR) is set at
   the same time.
   Cleared by writing 1.
   */

#define MISERR (1 << 9)
/* Fast programming data miss error
In Fast programming mode, 32 double words must be sent to flash successively,
   and the new data must be sent to the flash logic control before the current data is
   fully programmed. MISSERR is set by hardware when the new data is not
   present in time.
   Cleared by writing 1.
   */

#define PGSERR (1 << 7)
/* Programming sequence error. Set by hardware when a write access to 
   the Flash memory is performed by the code while the control register 
   has not been correctly configured.
   Cleared by writing 1.*/

#define PGSIZERR (1 << 6)
/* Set by hardware when the size of the access is a byte or half-word during a
   program or a fast program sequence. Only double word programming is allowed
   (consequently: word access).
   Cleared by writing 1.
   */

#define PGAERR (1 << 5)
/* Programming alignment error. Set by hardware when the data to program 
   cannot be contained in the same 128-bit Flash memory row.
   Cleared by writing 1.*/

#define WRPERR (1 << 4)
/* Write protection error. Set by hardware when an address to be 
   erased/programmed belongs to a write-protected part of the Flash memory.
   Cleared by writing 1.*/

#define PROGERR (1 << 3)
/* : Programming error
Set by hardware when a double-word address to be programmed contains a
value different from '0xFFFF FFFF' before programming, except if the data to
write is '0x0000 0000'.
Cleared by writing 1 */

#define OPERR (1 << 1)
/* Operation error. Set by hardware when a flash operation 
   (programming / erase) request is detected and can not be run because 
   of parallelism, alignment, sequence or write protection error. This bit 
   is set only if error interrupts are enabled (ERRIE = 1).*/

#define EOP (1 << 0)
/* End of operation. Set by hardware when one or more Flash memory 
   operations (program/erase) has/have completed successfully. 
   It is set only if the end of operation interrupts are 
   enabled (EOPIE = 1).
   Cleared by writing a 1. */

/* ------------------------------------------------------------------------- */
/* Flash control register */
#define FLASH_CR 0x14
/* The Flash control register is used to configure and start 
   Flash memory operations. */

#define LOCK (1 << 31)
/* Lock. Write to 1 only. When it is set, this bit indicates that the 
   FLASH_CR register is locked. It is cleared by hardware after detecting 
   the unlock sequence. In the event of an unsuccessful unlock operation, 
   this bit remains set until the next reset. */

#define OPTLOCK (1 << 30)
/* Option lock. Write to 1 only. When this bit is set, it indicates that 
   the FLASH_OPTCR register is locked. This bit is cleared by hardware 
   after detecting the unlock sequence. In the event of an unsuccessful 
   unlock operation, this bit remains set until the next reset. */

#define OBL_LAUNCH (1 << 27)

/*
OBL_LAUNCH: Force the option byte loading
When set to 1, this bit forces the option byte reloading. This bit is cleared only
when the option byte loading is complete. It cannot be written if OPTLOCK is set.
0: Option byte loading complete
1: Option byte loading requested
*/

#define RDERRIE (1 << 26)

#define ERRIE (1 << 25)
/* Error interrupt enable. This bit enables the interrupt generation when 
   the OPERR bit in the FLASH_SR register is set to 1.
0: Error interrupt generation disabled
1: Error interrupt generation enabled */

#define EOPIE (1 << 24)
/* End of operation interrupt enable. This bit enables the interrupt 
   generation when the EOP bit in the FLASH_SR register goes to 1.
0: Interrupt generation disabled
1: Interrupt generation enabled */

/* Bit 18 : Fast programming */
#define FSTPG (1 << 18)
/* 
0: Fast programming disabled
1: Fast programming enabled
*/

/*Bit 17 : Options modification start */
#define OPTSTRT (1 << 16)
/* This bit triggers an options operation when set.
This bit is set only by software, and is cleared when the BSY bit is cleared in
FLASH_SR.
*/

/*Bit 16 START: Start */
#define STRT (1 << 16)
/* This bit triggers an erase operation when set. If MER1, MER2 and PER bits are
reset and the STRT bit is set, an unpredictable behavior may occur without
generating any error flag. This condition should be forbidden.
This bit is set only by software, and is cleared when the BSY bit is cleared in
FLASH_SR.
*/

/*Bits 15:11 Reserved, must be kept at reset value.
*/

/*Bits 10:3 Page number selection */
#define PNB(N) (((N) & 0xff) << 3)
/* These bits select the page to erase:
00000000: page 0
00000001: page 1
...
11111111: page 255
Note: Bit 10 is used on STM32L45x and STM32L46x devices only. */

#define MER1 (1 << 2)
/*Bit 2 MER1: Mass erase
This bit triggers the mass erase (all user pages) when set.
*/

#define PER (1 << 1)
/* Page erase
0: page erase disabled
1: page erase enabled
*/

#define PG (1 << 0)
/* Programming
0: Flash programming disabled
1: Flash programming enabled
*/

/*-------------------------------------------------------------------------
  Debug MCU Interface
  -------------------------------------------------------------------------*/

/* Flash access control register */
#define DBGMCU_IDCODE      0x00
#define DBGMCU_CR          0x04
#define DBGMCU_APB1FZR1    0x08
#define DBGMCU_APB1FZR2    0x0c
#define DBGMCU_APB2FZR     0x10

#define DBG_SLEEP         (1 << 0)
#define DBG_STOP          (1 << 1)
#define DBG_STANDBY       (1 << 2)
#define TRACE_IOEN        (1 << 5)
#define TRACE_MODE        (3 << 6)

