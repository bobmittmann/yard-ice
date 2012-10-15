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
 * @file .c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <uthreads.h>
#include <sys/clock.h>
#include <crc.h>
#include <stdlib.h>

#include "target.h"
#include "script.h"
#include "cm3ice.h"
#include "target/stm32f.h"

#include <sys/dcclog.h>

#define STM32F_BASE_FSMC    0xa0000000
#define STM32F_BASE_RNG     0x50060800
#define STM32F_BASE_HASH    0x50060400
#define STM32F_BASE_CRYP    0x50060000
#define STM32F_BASE_DCMI    0x50050000
#define STM32F_BASE_OTG_FS  0x50000000
#define STM32F_BASE_OTG_HS  0x40040000
#define STM32F_BASE_EMAC    0x40028000
#define STM32F_BASE_DMA2    0x40026400
#define STM32F_BASE_DMA1    0x40026000
#define STM32F_BASE_BKPSRAM 0x40024000
#define STM32F_BASE_FLASH   0x40023c00
#define STM32F_BASE_RCC     0x40023800
#define STM32F_BASE_CRC     0x40023000
#define STM32F_BASE_GPIOI   0x40022000
#define STM32F_BASE_GPIOH   0x40021c00
#define STM32F_BASE_GPIOG   0x40021800
#define STM32F_BASE_GPIOF   0x40021400
#define STM32F_BASE_GPIOE   0x40021000
#define STM32F_BASE_GPIOD   0x40020c00
#define STM32F_BASE_GPIOC   0x40020800
#define STM32F_BASE_GPIOB   0x40020400
#define STM32F_BASE_GPIOA   0x40020000
#define STM32F_BASE_TIM11   0x40014800
#define STM32F_BASE_TIM10   0x40014400
#define STM32F_BASE_TIM9    0x40014000
#define STM32F_BASE_EXTI    0x40013c00
#define STM32F_BASE_SYSCFG  0x40013800
#define STM32F_BASE_SPI1    0x40013000
#define STM32F_BASE_SDIO    0x40012c00
#define STM32F_BASE_USART6  0x40011400
#define STM32F_BASE_USART1  0x40011000
#define STM32F_BASE_TIM8    0x40010400
#define STM32F_BASE_TIM1    0x40010000
#define STM32F_BASE_DAC     0x40007400
#define STM32F_BASE_PWR     0x40007000
#define STM32F_BASE_CAN2    0x40006800
#define STM32F_BASE_CAN1    0x40006400
#define STM32F_BASE_I2C3    0x40005c00
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
#define STM32F_BASE_TIM14   0x40002000
#define STM32F_BASE_TIM13   0x40001c00
#define STM32F_BASE_TIM12   0x40001800
#define STM32F_BASE_TIM7    0x40001400
#define STM32F_BASE_TIM6    0x40001000
#define STM32F_BASE_TIM5    0x40000c00
#define STM32F_BASE_TIM4    0x40000800
#define STM32F_BASE_TIM3    0x40000400
#define STM32F_BASE_TIM2    0x40000000


/*-------------------------------------------------------------------------
  Power Control (PWR)
  -------------------------------------------------------------------------*/

/* PWR power control register */
#define PWR_CR 0x00 

#define FPDS (1 << 9)
/* Flash power down in Stop mode. When set, the Flash memory enters power 
   down mode when the device enters Stop mode. This allows to achieve a lower 
   consumption in stop mode but a longer restart time.
0: Flash memory not in power down when the device is in Stop mode
1: Flash memory in power down when the device is in Stop mode */

#define DBP (1 << 8)
/* Disable backup domain write protection. In reset state, the RCC_BDCR 
   register, the RTC registers (including the backup registers), and 
   the BRE bit of the PWR_CSR register, are protected against parasitic 
   write access. This bit must be set to enable write access to these registers.
0: Access to RTC and RTC Backup registers and backup SRAM disabled
1: Access to RTC and RTC Backup registers and backup SRAM enabled */

#define PLS (0x7 << 5)
#define PLS_2_0V (0x0 << 5)
#define PLS_2_1V (0x1 << 5)
#define PLS_2_3V (0x2 << 5)
#define PLS_2_5V (0x3 << 5)
#define PLS_2_6V (0x4 << 5)
#define PLS_2_7V (0x5 << 5)
#define PLS_2_8V (0x6 << 5)
#define PLS_2_9V (0x7 << 5)
/* PVD level selection. These bits are written by software to select the voltage threshold detected by the Power
Voltage Detector
000: 2.0 V
001: 2.1 V
010: 2.3 V
011: 2.5 V
100: 2.6 V
101: 2.7 V
110: 2.8 V
111: 2.9 V
Note: Refer to the electrical characteristics of the datasheet for more details. */

#define PVDE (1 << 4)
/* Power voltage detector enable
This bit is set and cleared by software.
0: PVD disabled
1: PVD enabled */

#define CSBF (1 << 3)
/* Clear standby flag
This bit is always read as 0.
0: No effect
1: Clear the SBF Standby Flag (write). */

#define CWUF (1 << 2)
/* Clear wakeup flag
This bit is always read as 0.
0: No effect
1: Clear the WUF Wakeup Flag after 2 System clock cycles */

#define PDDS (1 << 1)
/* Power down deepsleep. This bit is set and cleared by software. 
   It works together with the LPDS bit. 
0: Enter Stop mode when the CPU enters deepsleep. 
	The regulator status depends on the LPDS bit.
1: Enter Standby mode when the CPU enters deepsleep. */

#define LPDS (1 << 0)
/* Low-power deep sleep
This bit is set and cleared by software. It works together with the PDDS bit.
0: Voltage regulator on during Stop mode
1: Voltage regulator in low-power mode during Stop mode */


/* PWR power control/status register */
#define PWR_CSR 0x04

#define BRE (1 << 9)
/* Backup regulator enable
When set, the Backup regulator (used to maintain backup SRAM content in 
Standby and VBAT modes) is enabled. If BRE is reset, the backup regulator is 
switched off. The backup SRAM can still be used but its content will be 
lost in the Standby and VBAT modes. Once set, the application must wait 
that the Backup Regulator Ready flag (BRR) is set to indicate that the
data written into the RAM will be maintained in the Standby and VBAT modes.
0: Backup regulator disabled
1: Backup regulator enabled
Note: This bit is not reset when the device wakes up from Standby mode, 
by a system reset, or by a power reset. */

#define EWUP (1 << 8)
/* Enable WKUP pin
This bit is set and cleared by software.
0: WKUP pin is used for general purpose I/O. An event on the WKUP 
pin does not wakeup the device from Standby mode.
1: WKUP pin is used for wakeup from Standby mode and forced in input pull 
down configuration (rising edge on WKUP pin wakes-up the system from 
Standby mode).
Note: This bit is reset by a system reset. */

#define BRR (1 << 3)
/* Backup regulator ready
Set by hardware to indicate that the Backup Regulator is ready.
0: Backup Regulator not ready
1: Backup Regulator ready
Note: This bit is not reset when the device wakes up from Standby 
mode or by a system reset or power reset. */

#define PVDO (1 << 2)
/* PVD output
This bit is set and cleared by hardware. It is valid only if PVD is enabled by the PVDE bit.
0: VDD/VDDA is higher than the PVD threshold selected with the PLS[2:0] bits.
1: VDD/VDDA is lower than the PVD threshold selected with the PLS[2:0] bits.
Note: The PVD is stopped by Standby mode. For this reason, this bit is equal to 0 after
Standby or reset until the PVDE bit is set. */

#define SBF (1 << 1)
/* Standby flag
This bit is set by hardware and cleared only by a 
POR/PDR (power-on reset/power-down reset)
or by setting the CSBF bit in the PWR power control register (PWR_CR)
0: Device has not been in Standby mode
1: Device has been in Standby mode */

#define WUF (1 << 0)
/* Wakeup flag
This bit is set by hardware and cleared only by a POR/PDR (power-on 
reset/power-down reset) or by setting the CWUF bit in the PWR power 
control register (PWR_CR)
0: No wakeup event occurred
1: A wakeup event was received from the WKUP pin or from the 
RTC alarm (Alarm A or Alarm B), RTC Tamper event, RTC TimeStamp event or 
RTC Wakeup).
Note: An additional wakeup event is detected if the WKUP pin is enabled 
(by setting the EWUP bit) when the WKUP pin level is already high. */

/*-------------------------------------------------------------------------
  System configuration controller
  -------------------------------------------------------------------------*/

/* Compensation cell control register */
#define SYSCFG_CMPCR 0x20

#define READY (1 << 8)
/* Compensation cell ready flag
0: I/O compensation cell not ready
1: O compensation cell ready*/

#define CMP_PD (1 << 0)
/* Compensation cell power-down
0: I/O compensation cell power-down mode
1: I/O compensation cell enabled */


/*-------------------------------------------------------------------------
  RCC
  -------------------------------------------------------------------------*/

/* RCC clock control register */ 
#define RCC_CR 0x00

/**/
#define PLLI2SRDY (1 << 27)
/* PLLI2S clock ready flag
Set by hardware to indicate that the PLLI2S is locked.
0: PLLI2S unlocked
1: PLLI2S locked*/
#define PLLI2SON (1 << 26)
/* PLLI2S enable
Set and cleared by software to enable PLLI2S.
Cleared by hardware when entering Stop or Standby mode.
0: PLLI2S OFF
1: PLLI2S ON*/
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
#define HSICAL (1 << 8)
/* Internal high-speed clock calibration
These bits are initialized automatically at startup.*/
#define HSITRIM (1 << 3)
/* Internal high-speed clock trimming
These bits provide an additional user-programmable trimming value that is added to the
HSICAL[7:0] bits. It can be programmed to adjust to variations in voltage and temperature
that influence the frequency of the internal HSI RC.
Bit 2
Reserved, always read as 0.*/
#define HSIRDY (1 << 1)
/* Internal high-speed clock ready flag
Set by hardware to indicate that the HSI oscillator is stable. After the HSION bit is cleared,
HSIRDY goes low after 6 HSI clock cycles.
0: HSI oscillator not ready
1: HSI oscillator ready*/
#define HSION (1 << 0)
/* Internal high-speed clock enable
Set and cleared by software.
Set by hardware to force the HSI oscillator ON when leaving the Stop or Standby mode or in
case of a failure of the HSE oscillator used directly or indirectly as the system clock. This bit
cannot be cleared if the HSI is used directly or indirectly as the system clock.
0: HSI oscillator OFF
1: HSI oscillator ON
*/

/* RCC PLL configuration register */
#define RCC_PLLCFGR 0x04

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

/* RCC clock configuration register */
#define RCC_CFGR 0x08

/**/
#define MCO2 (0x3 << 30)
#define MCO2_SYSCLK (0x0 << 30)
#define MCO2_PLLI2S (0x1 << 30)
#define MCO2_HSE    (0x2 << 30)
#define MCO2_PLL    (0x3 << 30)
/* Microcontroller clock output 2
   Set and cleared by software. Clock source selection may generate glitches on MCO2. It is highly recommended to configure these bits only after reset before enabling the external oscillators and the PLLs.
   00: System clock (SYSCLK) selected
   01: PLLI2S clock selected
   10: HSE oscillator clock selected
   11: PLL clock selected*/

#define MCO2PRE (0x7 << 27)
#define MCO2PRE_1 (0x0 << 27)
#define MCO2PRE_2 (0x4 << 27)
#define MCO2PRE_3 (0x5 << 27)
#define MCO2PRE_4 (0x6 << 27)
#define MCO2PRE_5 (0x7 << 27)
/* MCO2 prescaler
   Set and cleared by software to configure the prescaler of the MCO2. Modification of this prescaler may generate glitches on MCO2. It is highly recommended to change this prescaler only after reset before enabling the external oscillators and the PLLs.
   0xx: no division
   100: division by 2
   101: division by 3
   110: division by 4
   111: division by 5 */

#define MCO1PRE (0x7 << 24)
#define MCO1PRE_1 (0x0 << 24)
#define MCO1PRE_2 (0x4 << 24)
#define MCO1PRE_3 (0x5 << 24)
#define MCO1PRE_4 (0x6 << 24)
#define MCO1PRE_5 (0x7 << 24)
/* MCO1 prescaler
   Set and cleared by software to configure the prescaler of the MCO1. Modification of this prescaler may generate glitches on MCO1. It is highly recommended to change this prescaler only after reset before enabling the external oscillators and the PLL.
   0xx: no division
   100: division by 2
   101: division by 3
   110: division by 4
   111: division by 5*/

#define I2SSRC (1 << 23)
/* I2S clock selection
   Set and cleared by software. This bit allows to select the I2S clock source between the PLLI2S clock and the external clock. It is highly recommended to change this bit only after reset and before enabling the I2S module.
   0: PLLI2S clock used as I2S clock source
   1: External clock mapped on the I2S_CKIN pin used as I2S clock source */

#define MCO1 (0x3 << 21)
#define MCO1_HSI (0x0 << 21)
#define MCO1_LSE (0x1 << 21)
#define MCO1_HSE (0x2 << 21)
#define MCO1_PLL (0x3 << 21)
/* Microcontroller clock output 1
Set and cleared by software. Clock source selection may generate glitches on MCO1. It is highly recommended to configure these bits only after reset before enabling the external oscillators and PLL.
00: HSI clock selected
01: LSE oscillator selected
10: HSE oscillator clock selected
11: PLL clock selected */

#define RTCPRE (0x1f << 16)
/* HSE division factor for RTC clock
Set and cleared by software to divide the HSE clock input clock to generate a 1 MHz clock for RTC.
Caution: The software has to set these bits correctly to ensure that the clock supplied to the
RTC is 1 MHz. These bits must be configured if needed before selecting the RTC
clock source.
00000: no clock
00001: no clock
00010: HSE/2
00011: HSE/3
00100: HSE/4
...
11110: HSE/30
11111: HSE/31 */

#define PPRE2 (0x7 << 13)
#define PPRE2_1 (0x0 << 13)
#define PPRE2_2 (0x4 << 13)
#define PPRE2_4 (0x5 << 13)
#define PPRE2_8 (0x6 << 13)
#define PPRE2_16 (0x7 << 13)
/* APB high-speed prescaler (APB2)
Set and cleared by software to control APB high-speed clock division factor.
Caution: The software has to set these bits correctly not to exceed 60 MHz on this domain.
The clocks are divided with the new prescaler factor from 1 to 16 AHB cycles after
PPRE2 write.
0xx: AHB clock not divided
100: AHB clock divided by 2
101: AHB clock divided by 4
110: AHB clock divided by 8
111: AHB clock divided by 16 */

#define PPRE1 (0x7 << 10)
#define PPRE1_1 (0x0 << 10)
#define PPRE1_2 (0x4 << 10)
#define PPRE1_4 (0x5 << 10)
#define PPRE1_8 (0x6 << 10)
#define PPRE1_16 (0x7 << 10)
/* APB Low speed prescaler (APB1)
Set and cleared by software to control APB low-speed clock division factor.
Caution: The software has to set these bits correctly not to exceed 30 MHz on this domain.
The clocks are divided with the new prescaler factor from 1 to 16 AHB cycles after
PPRE1 write.
0xx: AHB clock not divided
100: AHB clock divided by 2
101: AHB clock divided by 4
110: AHB clock divided by 8
111: AHB clock divided by 16 */

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
Caution: The clocks are divided with the new prescaler factor from 1 to 16 AHB cycles after HPRE write.
Caution: The AHB clock frequency must be at least 25 MHz when the Ethernet is used.
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
#define SWS_HSI (0x0 << 2)
#define SWS_HSE (0x1 << 2)
#define SWS_PLL (0x2 << 2)
/* System clock switch status. Set and cleared by hardware to indicate which 
   clock source is used as the system clock.
00: HSI oscillator used as the system clock
01: HSE oscillator used as the system clock
10: PLL used as the system clock
11: not applicable */

#define SW (0x3 << 0)
#define SW_HSI (0x0 << 0)
#define SW_HSE (0x1 << 0)
#define SW_PLL (0x2 << 0)
/* System clock switch
Set and cleared by software to select the system clock source.
Set by hardware to force the HSI selection when leaving the Stop or Standby mode or in case of failure of the HSE oscillator used directly or indirectly as the system clock.
00: HSI oscillator selected as system clock
01: HSE oscillator selected as system clock
10: PLL selected as system clock
11: not allowed
*/


/* RCC AHB1 peripheral clock register */
#define RCC_AHB1ENR 0x30

#define OTGHSULPIEN (1 << 30)
/* USB OTG HSULPI clock enable
Set and cleared by software.
0: USB OTG HS ULPI clock disabled
1: USB OTG HS ULPI clock enabled*/
#define OTGHSEN (1 << 29)
/* USB OTG HS clock enable
Set and cleared by software.
0: USB OTG HS clock disabled
1: USB OTG HS clock enabled*/
#define ETHMACPTPEN (1 << 28)
/* Ethernet PTP clock enable
Set and cleared by software.
0: Ethernet PTP clock disabled
1: Ethernet PTP clock enabled*/
#define ETHMACRXEN (1 << 27)
/* Ethernet Reception clock enable
Set and cleared by software.
0: Ethernet Reception clock disabled
1: Ethernet Reception clock enabled*/
#define ETHMACTXEN (1 << 26)
/* Ethernet Transmission clock enable
Set and cleared by software.
0: Ethernet Transmission clock disabled
1: Ethernet Transmission clock enabled*/
#define ETHMACEN (1 << 25)
/* Ethernet MAC clock enable
Set and cleared by software.
0: Ethernet MAC clock disabled
1: Ethernet MAC clock enabled
Bits 24:23
Reserved, always read as 0.*/
#define DMA2EN (1 << 22)
/* DMA2 clock enable
Set and cleared by software.
0: DMA2 clock disabled
1: DMA2 clock enabled*/
#define DMA1EN (1 << 21)
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
100: Four wait states
101: Five wait states
110: Six wait states
111: Seven wait states */


/* Flash key register */
#define FLASH_KEYR 0x04
/* The Flash key register is used to allow access to the Flash control 
   register and so, to allow program and erase operations. */
#define FLASH_KEY1 0x45670123
#define FLASH_KEY2 0xcdef89ab

/* Flash option key register */
#define FLASH_OPTKEYR 0x08
/* The Flash option key register is used to allow program and erase 
   operations in the user configuration sector. */
#define OPTKEY1 0x08192a3b
#define OPTKEY2 0x4c5d6e7f



/* Flash status register */
#define FLASH_SR 0x0c
/* The Flash status register gives information on ongoing program and 
   erase operations. */

#define BSY (1 << 16)
/* Busy. This bit indicates that a Flash memory operation is in progress. 
   It is set at the beginning of a Flash memory operation and cleared 
   when the operation finishes or an error occurs.
	0: no Flash memory operation ongoing
	1: Flash memory operation ongoing */

#define PGSERR (1 << 7)
/* Programming sequence error. Set by hardware when a write access to 
   the Flash memory is performed by the code while the control register 
   has not been correctly configured.
   Cleared by writing 1.*/

#define PGPERR (1 << 6)
/* Programming parallelism error. Set by hardware when the size of the 
   access (byte, half-word, word, double word) during the program 
   sequence does not correspond to the parallelism configuration 
   PSIZE (x8, x16, x32, x64).
   Cleared by writing 1.*/

#define PGAERR (1 << 5)
/* Programming alignment error. Set by hardware when the data to program 
   cannot be contained in the same 128-bit Flash memory row.
   Cleared by writing 1.*/

#define WRPERR (1 << 4)
/* Write protection error. Set by hardware when an address to be 
   erased/programmed belongs to a write-protected part of the Flash memory.
   Cleared by writing 1.*/

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


/* Flash control register */
#define FLASH_CR 0x10
/* The Flash control register is used to configure and start 
   Flash memory operations. */

#define LOCK (1 << 31)
/* Lock. Write to 1 only. When it is set, this bit indicates that the 
   FLASH_CR register is locked. It is cleared by hardware after detecting 
   the unlock sequence. In the event of an unsuccessful unlock operation, 
   this bit remains set until the next reset. */

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

#define STRT (1 << 16)
/* Start. This bit triggers an erase operation when set. It is set only by 
   software and cleared when the BSY bit is cleared. */

#define PSIZE (0x3 << 8)
#define PSIZE_8 (0x0 << 8)
#define PSIZE_16 (0x1 << 8)
#define PSIZE_32 (0x2 << 8)
#define PSIZE_64 (0x3 << 8)
/* Program size. These bits select the program parallelism.
00 program x8
01 program x16
10 program x32
11 program x64 */

#define SNB(SECT) (((SECT) & 0xf) << 3)
/* Sector number. These bits select the sector to erase.
0000 sector 0
0001 sector 1
...
1011 sector 11
Others not allowed*/

#define MER (1 << 2)
/* Mass Erase. Erase activated for all user sectors.*/

#define SER (1 << 1)
/* Sector Erase. Sector Erase activated.*/

#define PG (1 << 0)
/* Programming. Flash programming activated. */



/* Flash option control register */
#define FLASH_OPTCR 0x14
/* The FLASH_OPTCR register is used to modify the user option bytes. */

#define NWRP (1 << 16)
/* Not write protect
These bits contain the value of the write-protection option bytes after reset. They can be
written to program a new write protect value into Flash memory.
0: Write protection active
1: Write protection not active */

#define RDP (1 << 15)
/* Read protect
These bits contain the value of the read-protection option level after reset. They can be
written to program a new read protection value into Flash memory.
0xAA: Level 0, read protection not active
0xCC: Level 2, chip read protection active
Others: Level 1, read protection of memories active */

#define USER (1 << 7)
/* User option bytes
These bits contain the value of the user option byte after reset. They can be written to
program a new user option byte value into Flash memory.
Bit 7: nRST_STDBY
Bit 6: nRST_STOP
Bit 5: WDG_SW
Note: When changing the WDG mode from hardware to software or from software to
hardware, a system reset is required to make the change effective.
Bit 4 Reserved, must be kept cleared. */

#define BOR_LEV (1 << 3)
/* BOR reset Level. These bits contain the supply level threshold that 
   activates/releases the reset. They can be written to program a new BOR 
   level. By default, BOR is off. When the supply voltage (VDD) drops below 
   the selected BOR level, a device reset is generated.
00: BOR Level 3 (VBOR3), reset threshold level for 2.70 to 3.60 V voltage range
01: BOR Level 2 (VBOR2), reset threshold level for 2.40 to 2.70 V voltage range
10: BOR Level 1 (VBOR1), reset threshold level for 2.10 to 2.40 V voltage range
11: BOR off (VBOR0), reset threshold level for 1.80 to 2.10 V voltage range
Note: For full details about BOR characteristics, refer to the "Electrical characteristics" section
in the device datasheet. */

#define OPTSTRT (1 << 1)
/* Option start. This bit triggers a user option operation when set. 
   It is set only by software and cleared when the BSY bit is cleared. */

#define OPTLOCK (1 << 0)
/* Option lock. Write to 1 only. When this bit is set, it indicates that 
   the FLASH_OPTCR register is locked. This bit is cleared by hardware 
   after detecting the unlock sequence. In the event of an unsuccessful 
   unlock operation, this bit remains set until the next reset. */

/*-------------------------------------------------------------------------
  RTC
  -------------------------------------------------------------------------*/

/* RTC time register */
#define RTC_TR 0x00
/* RTC date register */
#define RTC_DR 0x04
/* RTC control register */
#define RTC_CR 0x08
/* RTC initialization and status register */
#define RTC_ISR 0x0c
/* RTC prescaler register */
#define RTC_PRER 0x10
/* RTC wakeup timer register */
#define RTC_WUTR 0x14
/* RTC calibration register */
#define RTC_CALIBR 0x18
/* RTC alarm A register */
#define RTC_ALRMAR 0x1c
/* RTC alarm B register */
#define RTC_ALRMBR 0x20
/* RTC write protection register */
#define RTC_WPR 0x24
#define WPR_KEY1 0xca
#define WPR_KEY2 0x53

/* RTC time stamp time register */
#define RTC_TSTR 0x30
/* RTC time stamp date register */
#define RTC_TSDR 0x34
/* RTC tamper and alternate function configuration register */
#define RTC_TAFCR 0x40
/* RTC backup registers */
#define RTC_BKP0R 0x50
#define RTC_BKP1R 0x54
#define RTC_BKP2R 0x58
#define RTC_BKP19R 0x9c


int stm32f_on_init(FILE * f, const ice_drv_t * ice, 
					 ice_mem_entry_t * mem)
{
	uint32_t sr;
	uint32_t cr;
	uint32_t pll;
	uint32_t cfg;
	int again;

	ice_rd32(ice, STM32F_BASE_RCC + RCC_CFGR, &cfg);
	switch (cfg & SWS) {
	case SWS_HSI:
		fprintf(f, " %s: system clock is internal\n", __func__);
		break;
	case SWS_HSE:
		fprintf(f, " %s: system clock is external\n", __func__);
		break;
	case SWS_PLL:
		fprintf(f, " %s:  system clock is PLL\n", __func__);
		return 0;
	}

#if 0
	/* enable internal oscillator */
	cr = HSION;
	ice_wr32(ice, STM32F_BASE_RCC + RCC_CR, cr);

	for (again = 256; ; again--) {
		ice_rd32(ice, STM32F_BASE_RCC + RCC_CR, &cr);
		if (cr & HSIRDY)
			break;
		if (again == 0) {
			fprintf(f, " %s: internal oscillator startup fail!\n", __func__);
			DCC_LOG(LOG_WARNING, "internal oscillator fail!");
			return -1;
		}
	}

	/* switch to internal oscillator */
	cfg = MCO2_SYSCLK | MCO2PRE_2 | PPRE2_4 | PPRE1_8 | HPRE_1 | SW_HSI;
	ice_wr32(ice, STM32F_BASE_RCC + RCC_CFGR, cfg);
#endif

	/* Enable external oscillator */
	ice_rd32(ice, STM32F_BASE_RCC + RCC_CR, &cr);
	cr |= HSEON;
	ice_wr32(ice, STM32F_BASE_RCC + RCC_CR, cr);

	for (again = 256; ; again--) {
		ice_rd32(ice, STM32F_BASE_RCC + RCC_CR, &cr);
		if (cr & HSERDY)
			break;
		if (again == 0) {
			fprintf(f, " %s: external clock startup fail!\n", __func__);
			DCC_LOG(LOG_WARNING, "external clock fail!");
			return -1;
		}
	}

	/*
	   F_HSE = 24 MHz
	   F_VCO = 480 MHz
	   F_MAIN = 120 MHz
	   F_USB = 48 MHz
	 */
	pll = PLLSRC_HSE | PLLM(12) | PLLN(240) | PLLP(4) | PLLQ(10);
	ice_wr32(ice, STM32F_BASE_RCC + RCC_PLLCFGR, pll);

	/* enable PLL */
	cr |= PLLON;
	ice_wr32(ice, STM32F_BASE_RCC + RCC_CR, cr);

	for (again = 256; ; again--) {
		ice_rd32(ice, STM32F_BASE_RCC + RCC_CR, &cr);
		if (cr & PLLRDY)
			break;
		if (again == 0) {
			fprintf(f, " %s: PLL lock fail!\n", __func__);
			DCC_LOG(LOG_WARNING, "PLL lock fail!");
			return -1;
		}
	}

	for (again = 4096; ; again--) {
		ice_rd32(ice, STM32F_BASE_FLASH + FLASH_SR, &sr);
		if ((sr & BSY) == 0)
			break;
		if (again == 0) {
			fprintf(f, " %s: flash not ready!\n", __func__);
			DCC_LOG(LOG_WARNING, "flash not ready!");
			return -1;
		}
	}

	/* adjust flash wait states and enable caches */
	ice_wr32(ice, STM32F_BASE_FLASH + FLASH_ACR, 
			 DCEN | ICEN | PRFTEN | LATENCY(3));

	/* adjust flash wait states and enable caches */
	ice_rd32(ice, STM32F_BASE_FLASH + FLASH_CR, &cr);

	if (cr & LOCK) {
		fprintf(f, " %s: unlocking flash\n", __func__);
		/* unlock flash write */
		ice_wr32(ice, STM32F_BASE_FLASH + FLASH_KEYR, FLASH_KEY1);
		ice_wr32(ice, STM32F_BASE_FLASH + FLASH_KEYR, FLASH_KEY2);
	}

	/* switch to pll oscillator */
	cfg = MCO2_SYSCLK | MCO2PRE_2 | PPRE2_4 | PPRE1_8 | HPRE_1 | SW_PLL;
	ice_wr32(ice, STM32F_BASE_RCC + RCC_CFGR, cfg);


	return 0;
}

int stm32f_flash_erase(cm3ice_ctrl_t * ctrl, 
					   ice_mem_ref_t * addr, 
					   ice_size_t len)
{
	uint32_t sr;
	uint32_t acr;
	uint32_t cr;
	int again;
	int page;
	int sect;
	int size;

	page = addr->offs >> 14;
	switch (page) {
	case 0 ... 3:
		sect = page;
		size = 16384;
		break;
	case 4 ... 7:
		sect = 4;
		size = 65536;
		break;
	default:
		sect = ((page - 1) / 8) + 5;
		size = 131072;
		break;
	}

	DCC_LOG4(LOG_TRACE, "len=%d page=%d sect=%d size=%d", 
			 len, page, sect, size);

	cm3ice_rd32(ctrl, STM32F_BASE_FLASH + FLASH_CR, &cr);
	if (cr & LOCK) {
		DCC_LOG(LOG_TRACE, "unlocking flash...");
		/* unlock flash write */
		cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_KEYR, FLASH_KEY1);
		cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_KEYR, FLASH_KEY2);
	}


	cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_CR, SER | SNB(sect) | STRT);

	for (again = 4096 * 32; ; again--) {
		cm3ice_rd32(ctrl, STM32F_BASE_FLASH + FLASH_SR, &sr);
		if ((sr & BSY) == 0)
			break;
		if (again == 0) {
			DCC_LOG(LOG_WARNING, "flash not ready!");
			return -1;
		}
	}

	cm3ice_rd32(ctrl, STM32F_BASE_FLASH + FLASH_ACR, &acr);
	acr &= DCEN | ICEN;
	/* Disable cache */
	cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_ACR, acr);
	/* Flush cache */
	cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_ACR, acr | DCRST | ICRST);
	/* Enable cache */
	cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_ACR, acr | DCEN | ICEN);

	return size;
}

int stm32f_flash_wr32(cm3ice_ctrl_t * ctrl, uint32_t addr, uint32_t data)
{
	uint32_t sr;
	int again;

	DCC_LOG2(LOG_TRACE, "0x%08x <-- 0x%08x", addr, data);

	cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_CR, PSIZE_32 | PG);
	cm3ice_wr32(ctrl, addr, data);
	
	for (again = 4096; ; again--) {
		cm3ice_rd32(ctrl, STM32F_BASE_FLASH + FLASH_SR, &sr);
		if ((sr & BSY) == 0)
			break;
		if (again == 0) {
			DCC_LOG(LOG_WARNING, "flash not ready!");
			return -1;
		}
	}

	return 0;
}

int stm32f_flash_write(cm3ice_ctrl_t * ctrl, ice_mem_ref_t * mem, 
					 const void * buf, ice_size_t len)
{
	uint32_t data;
	uint32_t addr;
	uint8_t * ptr;
	uint32_t cr;
	int n;
	int i;

	n = (len + 3) / 4;

	ptr = (uint8_t *)buf;
	addr = mem->base + mem->offs;

	cm3ice_rd32(ctrl, STM32F_BASE_FLASH + FLASH_CR, &cr);
	if (cr & LOCK) {
		DCC_LOG(LOG_TRACE, "unlocking flash...");
		/* unlock flash write */
		cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_KEYR, FLASH_KEY1);
		cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_KEYR, FLASH_KEY2);
	}


	DCC_LOG2(LOG_TRACE, "0x%08x len=%d", addr, len);

	for (i = 0; i < n; i++) {
		data = ptr[0] | (ptr[1] << 8) | (ptr[2] << 16) | (ptr[3] << 24);
		stm32f_flash_wr32(ctrl, addr, data);
		ptr += 4;
		addr += 4;
	}
	
	return n * 4;
}



const struct cm3ice_cfg stm32f_cfg = {
	.endianness = LITTLE_ENDIAN,
	.work_addr = 0x20000000 + MEM_KiB(64) - MEM_KiB(4),
	.work_size = MEM_KiB(4),
};

const struct target_arch stm32f_arch = {
	.name = "STM32F207",
	.model = "generic",
	.vendor = "bobmittmann",
	.cpu = &cortex_m3_cpu,
	.sym = void_sym
};

/*
 * stm32f flash
 */
const struct ice_mem_oper flash_stm32f_oper = {
	.read = (ice_mem_read_t)cm3ice_mem_read,
	.write = (ice_mem_write_t)stm32f_flash_write, 
	.erase = (ice_mem_erase_t)stm32f_flash_erase
};

