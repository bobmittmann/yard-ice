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
 * @file stm32l_defs.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#if defined(STM32L1XX)

#define STM32F_BASE_FSMC    0xa0000000
#define STM32F_BASE_AES     0x50060000
#define STM32F_BASE_DMA2    0x40026400
#define STM32F_BASE_DMA1    0x40026000
#define STM32F_BASE_FLASH   0x40023c00
#define STM32F_BASE_RCC     0x40023800
#define STM32F_BASE_CRC     0x40023000
#define STM32F_BASE_GPIOG   0x40021c00
#define STM32F_BASE_GPIOF   0x40021800
#define STM32F_BASE_GPIOH   0x40021400
#define STM32F_BASE_GPIOE   0x40021000
#define STM32F_BASE_GPIOD   0x40020c00
#define STM32F_BASE_GPIOC   0x40020800
#define STM32F_BASE_GPIOB   0x40020400
#define STM32F_BASE_GPIOA   0x40020000
#define STM32F_BASE_USART1  0x40013800
#define STM32F_BASE_SPI1    0x40013000
#define STM32F_BASE_SDIO    0x40012c00
#define STM32F_BASE_ADC     0x40012400
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

#define CMP_PD (1 << 0)
/* Compensation cell power-down
0: I/O compensation cell power-down mode
1: I/O compensation cell enabled */


/*-------------------------------------------------------------------------
  RCC
  -------------------------------------------------------------------------*/

/* ------------------------------------------------------------------------- */
/* RCC clock control register */ 
#define RCC_CR 0x00

#define RTC_PRE (0x3 << 29)

#define RTC_PRE_HSE_2 (0x0 << 29)
#define RTC_PRE_HSE_4 (0x1 << 29)
#define RTC_PRE_HSE_8 (0x2 << 29)
#define RTC_PRE_HSE_16 (0x3 << 29)
/* RTC/LCD prescaler
   These bits are set and reset by software to obtain a 1 MHz clock from HSE. 
   This prescaler cannot be modified if HSE is enabled (HSEON = 1).
   These bits are reset by power on reset, they keep their value after system 
   reset.
   00: HSE is divided by 2 for RTC/LCD clock
   01: HSE is divided by 4 for RTC/LCD clock
   10: HSE is divided by 8 for RTC/LCD clock
   11: HSE is divided by 16 for RTC/LCD clock
 */

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

#define MSIRDY (1 << 9)
/* MSI clock ready flag
   This bit is set by hardware to indicate that the MSI oscillator is stable.
0: MSI oscillator not ready
1: MSI oscillator ready  */

#define MSION (1 << 8)
/* MSI clock enable
   This bit is set and cleared by software.
   Set by hardware to force the MSI oscillator ON when exiting from Stop 
   or Standby mode, or in case of a failure of the HSE oscillator used 
   directly or indirectly as system clock. This bit cannot be cleared if the 
   MSI is used as system clock.
 */

#define HSICAL (1 << 8)
/* Internal high-speed clock calibration
   These bits are initialized automatically at startup.*/

#define HSITRIM (1 << 3)
/* Internal high-speed clock trimming
   These bits provide an additional user-programmable trimming value that is 
   added to the HSICAL[7:0] bits. It can be programmed to adjust to 
   variations in voltage and temperature that influence the frequency of 
   the internal HSI RC. */

/* Bit 2 Reserved, always read as 0.*/

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



/* ------------------------------------------------------------------------- */
/* RCC clock configuration register */

/* STM32F10x RCC clock configuration register */
#define RCC_CFGR 0x08

#define MCO_SYSCLK (0x1 << 24)
#define MCO_HSI    (0x2 << 24)
#define MCO_MSI    (0x3 << 24)
#define MCO_HSE    (0x4 << 24)
#define MCO_PLL    (0x5 << 24)
#define MCO_LSI    (0x6 << 24)
#define MCO_LSE    (0x7 << 24)

#define PLLDIV_2   (0x1 << 22)
#define PLLDIV_3   (0x2 << 22)
#define PLLDIV_4   (0x3 << 22)

#define PLLMUL_MASK   (0x0f << 18)
#define PLLMUL(M) ((((M) - 2) & 0x0f) << 18)

#define PLLXTPRE      (1 << 17)

#define PLLSRC     (1 << 16)
#define PLLSRC_HSI (0 << 16)
#define PLLSRC_HSE (1 << 16)

#define ADCPRE   (0x3 << 14)
#define ADCPRE_2 (0x0 << 14)
#define ADCPRE_4 (0x1 << 14)
#define ADCPRE_6 (0x2 << 14)
#define ADCPRE_8 (0x3 << 14)

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
#define STM32F10_RCC_CIR      (STM32F10_BASE_RCC + 0x08)

/* ------------------------------------------------------------------------- */
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
#define RUN_PD (1 << 4)
#define SLEEP_PD (1 << 3)
#define ACC64 (1 << 2)
#define PRFTEN (1 << 1)
#define LATENCY (1 << 0)


/* Program/erase control register */
#define FLASH_PECR 0x04

/* Bit 18 OBL_LAUNCH: Launch the option byte loading */
#define OBL_LAUNCH ( 1<< 18)
/* This bit is set by software to launch the option byte loading. 
   This bit is cleared only when the option byte loading has completed. 
   It cannot be written if OPTLOCK is set. 
When this bit is set, a reset is generated.
0: Option byte loading complete
1: Option byte has to be loaded */

/* Bit 17 ERRIE: Error interrupt enable
0: interrupt disabled
1: interrupt enabled */

/* Bit 16 EOPIE: End of programming interrupt enable
0: interrupt disabled
1: interrupt enabled */

/* Bit 15 PARALLBANK: Parallel bank mode.
This bit can be set and cleared by software when no program or erase process is on-going.
When this bit is set, 2 half pages can be programmed or 2 pages can be erased in parallel
(the first one in the first bank and the second one in the second bank).
0: Parallel bank mode disabled
1: Parallel bank mode enabled
Note: This bit is available in Cat.4 and Cat.5 devices only. */

/* Bit 10 FPRG: Half Page/Double Word programming mode */
#define FPRG (1 << 10)
/* This bit can be written by software when no program or erase 
   process is ongoing.
   It is used to enable/disable Half Page Programming to the program 
   memory or Double Word Programming to the data EEPROM.
   32 loadings are required to program half a page to the program memory.
   2 loadings are required to program a double word to the data EEPROM.
   This bit is cleared when PELOCK is set.
0: Half Page/Double Word programming disabled
1: Half Page/Double Word programming enabled */

/* Bit 9 ERASE: Page or Double Word erase mode */
#define ERASE (1 << 9)
/* This bit can be written by software when no program or erase process is 
   on going. It is used to enable/disable Page Erase on the program memory or 
   Double Word Erase on the data EEPROM and the option byte block.
   This bit is cleared when PELOCK is set.
0: Page or Double Word Erase disabled
1: Page or Double Word Erase enabled */

/* Bit 8 FTDW: Fixed time data write for Byte, Half Word and Word programming */
#define FTDW (1 << 8)
/* This bit is writable by software when no program or erase process is 
   ongoing. This bit is used for the data EEPROM only.
   It is cleared when PELOCK is set.
   0: Programming of a Byte, Half Word or word is performed without any 
      previous erase operation. This is possible if the word being written 
	  to is 0x0000 0000.
   1: Before the programming of a Byte, Half Word and word an erase phase 
      is automatically performed. So the time of programming is fixed and 
	  lasts two t prog */

/* Bit 4 DATA: Data EEPROM selection */
#define DATA (1 << 4)
/* This bit is writable by software when no program or erase process is 
   ongoing. This bit has to be set prior to data EEPROM double word 
   erase/programming. 
   This bit is cleared when PELOCK is set.
0: Data EEPROM not selected
1: Data EEPROM selected */

/* Bit 3 PROG: Program memory selection */
#define PROG (1 << 3)
/* This bit is writable by software when no program or erase process is 
   ongoing. This bit has to be set to gain write access to the program 
   memory, except in the case of word programming.
This bit is cleared when PELOCK is set.
0: Program memory not selected
1: Program memory selected */

/* Bit 2 OPTLOCK: Option bytes block lock */
#define OPTLOCK (1 << 2)

/* This bit can only be written to 1. When it is set, it indicates that 
   the option byte block is locked.
   It is cleared by hardware after detecting the unlock sequence. In the 
   event of an unsuccessful unlock operation or a third access to 
   OPTKEYR, a bus error (Cortex ® -M3 hardfault or Busfault) is generated 
   and this bit remains set until the next reset. This bit is set when 
   PELOCK is set.
0: option unlocked
1: option locked */

/* Bit 1 PRGLOCK: Program memory lock */
#define PRGLOCK (1 << 1)
/* This bit can only be written to 1. When it is set, it indicates that 
   the program memory cannot be written. It is cleared by hardware after 
   detecting the unlock sequence. In the event of an unsuccessful unlock 
   operation or a third access to PRGKEYR, a bus error (Cortex ® -M3 
   hardfault or Busfault) is generated and this bit remains set until 
   the next reset.
This bit is set when PELOCK is set.
0: program memory unlocked
1: program memory locked */

/* Bit 0 PELOCK: FLASH_PECR and data EEPROM lock */
#define PELOCK (1 << 0)
/* This bit can only be written to 1. When it is set, it indicates that 
   the FLASH_PECR register and data EEPROM are locked. It is cleared by 
   hardware after detecting the unlock sequence. In the event of unsuccessful 
   unlock operation or a third access to PEKEYR, a bus error (Cortex ® -M3 
   hardfault or Busfault) is generated and this bit remains set until 
   the next reset. When this bit is cleared, write access to the 
   data EEPROM is allowed.
0: FLASH_PECR is unlocked
1: FLASH_PECR is locked */


/* ------------------------------------------------------------------------- */
/* Power down key register (FLASH_PDKEYR) */
#define FLASH_PDKEYR 0x08
/* The Power down key register is used to unlock 
   the RUN_PD bit in FLASH_ACR. */
#define PDKEY1 0x04152637
#define PDKEY2 0xfafbfcfd

/* Program/erase key register (FLASH_PEKEYR) */
#define FLASH_PEKEYR 0x0c
/* The Program/erase key register is used to allow access to FLASH_PECR 
   and so, to allow program and erase operations in the data EEPROM. */
#define PEKEY1 0x89abcdef
#define PEKEY2 0x02030405

/* Program memory key register (FLASH_PRGKEYR) */
#define FLASH_PRGKEYR 0x10
/*	The Program memory key register is used to allow program and erase 
	operations in the Program memory. It is write accessible only after 
	a correct write sequence has been executed in FLASH_PEKER */
#define PRGKEYR1 0x8c9daebf
#define PRGKEYR2 0x13141516

/* Option byte key register (FLASH_OPTKEYR) */
#define FLASH_OPTKEYR 0x14
/* The Option key register is used to allow program and erase operations 
   in the option byte block. It is write accessible only after the good 
   write sequence has been executed in FLASH_PEKEYR. */
#define OPTKEY1 0xfbead9c8
#define OPTKEY2 0x24252627

/* ------------------------------------------------------------------------- */
/* Flash status register */
#define FLASH_SR 0x18

/* Bit 13 RDERR: Read protected error */
#define RDERR (1 << 13)
/* Set by hardware when an address to be read through the Dbus belongs 
   to a read-protected (PCROP) part of the memory.
   Cleared by writing 1. */

/* Bit 12 OPTVERRUSR: Option UserValidity Error. */
#define OPTVERRUSR (1 << 12)
/* Set by hardware when the user options read may not be the ones 
   configured by the user.
   Cleared by writing 1.
   If option have not been properly loaded, each time there is a system 
   reset, OPTVERRUSR is set again. Consequently, an interrupt is 
   generated as soon as ERRIE is set.
   Note: This bit is available in Cat.3, Cat.4 and Cat.5 devices only. */

/* Bit 11 OPTVERR: Option validity error */
#define OPTVERR (1 << 11)
/* Set by hardware when the options read may not be the ones configured by 
   the software.
   Cleared by writing 1.
   If the options have not been properly loaded, each time a system reset 
   occurs, OPTVERR reverts to logical level 1. Consequently, an 
   interrupt is generated whenever ERRIE is set. */

/* Bit 10 SIZERR: Size error */
#define SIZERR (1 << 10)
/* Set by hardware when the size of the data to program is prohibited.
Cleared by writing it to 1. */

/* Bit 9 PGAERR: Programming alignment error */
#define PGAERR (1 << 9)
/* Set by hardware when the data to program cannot be contained in a 
   given half page or double word.
   Cleared by writing it to 1. */

/* Bit 8 WRPERR: Write protected error */
#define WRPERR (1 << 8)
/* Set by hardware when an address to be erased/programmed belongs to 
   a write-protected part of the memory.
   Cleared by writing it to 1. */

/* Bit 3 READY: Flash memory module ready after low power mode */
#define READY (1 << 3)
/* This bit is set and cleared by hardware.
0: Flash memory module is not ready
1: Flash memory module is ready */

/* Bit 2 ENDHV: End of high voltage */
#define ENDHV (1 << 2)
/* This bit is set and cleared by hardware.
0: High voltage still applied during write/erase operations
1: End of high voltage */

/* Bit 1 EOP: End of operation */
#define EOP (1 << 1)
/* This bit is set by hardware if the high voltage stops being applied 
   and programming has not been aborted. It is cleared by software 
   (by writing it to 1).
0: No EOP event occurred
1: An EOP event occured. An interrupt is generated if EOPIE is set */

/* Bit 0 BSY: Write/erase operations in progress */
#define BSY (1 << 0)
/* 0: Write/erase operation not in progress
1: Write/erase operation in progress */


/* Option byte register (FLASH_OBR) */
#define FLASH_OBR 0x1c
/*	Address offset: 0x1C 
	Reset value: depends on RDP and USER option byte, 
	on virgin part initial value is 0x00F80000 */

/* Bit 23 BFB2: Boot From Bank 2 */
#define BFB2 (1 << 23)
/* This bit contains the user option byte loaded by the OBL.
This bit is used to boot from Bank2 . Actually this bit indicates wether a boot from system
memory or from bank 1 has been selected by option programming. Then, the jump to Bank
1 or Bank 2 is done by software depending on the value of @08000000 and @08030000.
0: BOOT from system memory
1: BOOT from Bank 1
This bit is read only.
Note: This bit is available in Cat.4 and Cat.5 devices only.
*/

/* 
Bits 22:16 User option byte
These bits contain the user option byte loaded by the OBL.
Bit 22: nRST_STDBY
Bit 21: nRST_STOP
Bit 20: IWDG_SW */

/* Bits 19:16:BOR_LEV[3:0]: Brownout reset threshold level */ 
#define BOR_LEV (0xf << 16)
#define BOR_LEVEL_1 (0x8 << 16)
#define BOR_LEVEL_2 (0x9 << 16)
#define BOR_LEVEL_3 (0xa << 16)
#define BOR_LEVEL_4 (0xb << 16)
#define BOR_LEVEL_5 (0xc << 16)

/* 
0xxx: BOR OFF: Reset threshold level for the 1.45 V-1.55 V voltage range (power down
																		  only)
In this particular case, V DD33 must have been above BOR LEVEL 1 to start the device OBL
sequence in order to disable the BOR. The power-down is then monitored by the PDR.
Note: If the BOR is disabled, a “grey zone” exists between 1.65 V and the V PDR threshold
(this means that V DD33 may be below the minimum operating voltage (1.65 V) without
 causing a reset until it crosses the V PDR threshold)
1000: BOR LEVEL 1: Reset threshold level for 1.69 V-1.8 V voltage range (power on)
	1001: BOR LEVEL 2: Reset threshold level for 1.94 V-2.1 V voltage range (power on)
	1010: BOR LEVEL 3: Reset threshold level for 2.3 V-2.49 V voltage range (power on)
	1011: BOR LEVEL 4: Reset threshold level for 2.54 V-2.74 V voltage range (power on)
	1100: BOR LEVEL 5: Reset threshold level for 2.77 V-3.0 V voltage range (power on)
	These bits are read only.
*/

/* Bit 8 SPRMOD: Sector protection mode selection */
#define SPRMOD (1 << 8)
/* 0: WRP[i]=1 bit in FLASH_WRPRx registers defines sector write protection
   1: WRP[i]=0 bit in FLASH_WRPRx registers defines sector write and 
   read (PCROP) protection. 
   See Section 3.7.4: PCROP for details.
 */

/* Bits 7:0 RDPRT[7:0]: Read protection */
#define RDPRT 0xff
#define RDPRT_LEVEL0 0xaa
#define RDPRT_LEVEL1 0x00
#define RDPRT_LEVEL2 0xcc

/*	These bits contain the read protection option level loaded by the OBL.
	0xAA: Level 0, read protection not active
	0xCC: Level 2, read protection active
	Others: Level 1, read protection of memories active. 
	Default configuration after option byte erase. */


