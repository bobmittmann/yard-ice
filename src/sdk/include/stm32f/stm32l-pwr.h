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
 * @file stm32f-pwr.h
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __STM32L_PWR_H__
#define __STM32L_PWR_H__

/*-------------------------------------------------------------------------
  Power Control (PWR)
  -------------------------------------------------------------------------*/

/* PWR power control register */
#define PWR_CR 0x00 

/* Low power run mode */
#define PWR_LPRUN (1 << 10)
/* When LPRUN bit is set together with the LPSDSR bit, the regulator 
   is switched from main mode to low power mode. Otherwise, it remains 
   in main mode. The regulator goes back to operate in main mode when 
   LPRUN is reset.
   It is forbidden to reset LPSDSR when the MCU is in Low power run 
   mode. LPSDSR is used as a prepositioning for the entry into low power 
   mode, indicating to the system which configuration of the regulator 
   will be selected when entering Low power mode. The LPSDSR bit must be 
   set before the LPRUN bit is set. LPSDSR can be reset only when LPRUN bit=0.
   0: Voltage regulator in main mode in Low power run mode
   1: Voltage regulator in low power mode in Low power run mode */

/* Voltage scaling range selection */
#define PWR_VOS      (3 << 11)
#define PWR_VOS_1_8V (1 << 11)
#define PWR_VOS_1_5V (2 << 11)
#define PWR_VOS_1_2V (3 << 11)
/* These bits are used to select the internal regulator voltage range.
   Before resetting the power interface by resetting the PWRRST bit in 
   the RCC_APB1RSTR register, these bits have to be set to "10" and the 
   frequency of the system has to be configured accordingly.
   00: forbidden (bits are unchanged and keep the previous value, no 
   voltage scaling range changed)
   01: 1.8 V (range 1)
   10: 1.5 V (range 2)
   11: 1.2 V (range 3) */


/* Fast wakeup */
#define PWR_FWU (1 << 10)
/* This bit works in conjunction with ULP bit.
   If ULP = 0, FWU is ignored
   If ULP = 1 and FWU = 1: V REFINT startup time is ignored when 
   exiting from low power mode.
   The VREFINTRDYF flag in the PWR_CSR register indicates when the 
   V REFINT is ready again.
   If ULP=1 and FWU = 0: Exiting from low power mode occurs only when 
   the V REFINT is ready (after its startup time). This bit is not reset 
   by resetting the PWRRST bit in the
   RCC_APB1RSTR register.
   0: Low power modes exit occurs only when V REFTINT is ready
   1: V REFTINT start up time is ignored when exiting low power modes */


/* Ultralow power mode */
#define PWR_ULP (1 << 9)
/* When set, the V REFINT is switched off in low power mode. This bit is 
   not reset by resetting the PWRRST bit in the RCC_APB1RSTR register.
   0: V REFTINT is on in low power mode
   1: V REFTINT is off in low power mode */

/* Disable backup write protection */
#define PWR_DBP (1 << 8)
/* In reset state, the RTC, RTC backup registers and RCC CSR register 
   are protected against parasitic write access. This bit must be set 
   to enable write access to these registers.
0: Access to RTC, RTC Backup and RCC CSR registers disabled
1: Access to RTC, RTC Backup and RCC CSR registers enabled
Note: If the HSE divided by 2, 4, 8 or 16 is used as the RTC clock, this bit must remain set to 1. */


/* PVD level selection. */
#define PWR_PLS (0x7 << 5)
#define PWR_PLS_1_9V (0x0 << 5)
#define PWR_PLS_2_1V (0x1 << 5)
#define PWR_PLS_2_3V (0x2 << 5)
#define PWR_PLS_2_5V (0x3 << 5)
#define PWR_PLS_2_7V (0x4 << 5)
#define PWR_PLS_2_9V (0x5 << 5)
#define PWR_PLS_3_1V (0x6 << 5)
#define PWR_PLS_EXT  (0x7 << 5)
/* These bits are written by software to select the voltage threshold detected 
   by the Power Voltage Detector
	000: 1.9 V
	001: 2.1 V
	010: 2.3 V
	011: 2.5 V
	100: 2.7 V
	101: 2.9 V
	110: 3.1 V
    111: External input analog voltage (Compare internally to V REFINT)
	PVD_IN input (PB7) has to be configured as analog input 
	when PLS[2:0] = 111. 
    Note: Refer to the electrical characteristics of the datasheet for 
	more details. */

/* Power voltage detector enable */
#define PWR_PVDE (1 << 4)
/* This bit is set and cleared by software.
0: PVD disabled
1: PVD enabled */

/* Clear standby flag */
#define PWR_CSBF (1 << 3)
/* This bit is always read as 0.
0: No effect
1: Clear the SBF Standby Flag (write). */

/* Clear wakeup flag */
#define PWR_CWUF (1 << 2)
/* This bit is always read as 0.
0: No effect
1: Clear the WUF Wakeup Flag after 2 System clock cycles */

/* Power down deepsleep. */
#define PWR_PDDS (1 << 1)
/* This bit is set and cleared by software. 
   It works together with the LPDS bit. 
0: Enter Stop mode when the CPU enters deepsleep. 
	The regulator status depends on the LPDS bit.
1: Enter Standby mode when the CPU enters deepsleep. */

/* Low-power deepsleep/sleep/low power run */
#define PWR_LPSDSR (1 << 0)
/* – DeepSleep/Sleep modes
When this bit is set, the regulator switches in low power mode when the CPU enters sleep or
deepsleep mode. The regulator goes back to main mode when the CPU exits from these
modes.
– Low power run mode
When this bit is set, the regulator switches in low power mode when the bit LPRUN is set.
The regulator goes back to main mode when the bit LPRUN is reset.
This bit is set and cleared by software.
0: Voltage regulator on during deepsleep/Sleep/Low power run mode
1: Voltage regulator in low power mode during deepsleep/Sleep/Low power run mode */



/* PWR power control/status register */
#define PWR_CSR 0x04

/* Enable WKUP pin 3 */
#define PWR_EWUP3 (1 << 10)
/* This bit is set and cleared by software.
   0: WKUP pin 3 is used for general purpose I/Os. An event on the WKUP 
   pin 3 does not wakeup the device from Standby mode.
   1: WKUP pin 3 is used for wakeup from Standby mode and forced in input 
   pull down configuration (rising edge on WKUP pin 3 wakes-up the system 
   from Standby mode).
   Note: This bit is reset by a system reset. */

/* Enable WKUP pin 2 */
#define PWR_EWUP2 (1 << 9)
/* This bit is set and cleared by software.
   0: WKUP pin 2 is used for general purpose I/Os. An event on the WKUP pin 
   2 does not wakeup the device from Standby mode.
   1: WKUP pin 2 is used for wakeup from Standby mode and forced in input 
   pull down configuration (rising edge on WKUP pin 2 wakes-up the system 
   from Standby mode).
   Note: This bit is reset by a system reset. */

/* Enable WKUP pin 1 */
#define PWR_EWUP1 (1 << 8)
/* This bit is set and cleared by software.
0: WKUP pin 1 is used for general purpose I/Os. An event on the WKUP pin 1 does not
wakeup the device from Standby mode.
1: WKUP pin 1 is used for wakeup from Standby mode and forced in input pull down
configuration (rising edge on WKUP pin 1 wakes-up the system from Standby mode).
Note: This bit is reset by a system reset. */


/* Regulator LP flag */
#define PWR_REGLPF (1 << 5)
/* This bit is set by hardware when the MCU is in Low power run mode.
When the MCU exits from Low power run mode, this bit stays at 1 until the regulator is ready in
main mode. A polling on this bit is recommended to wait for the regulator main mode. This bit is
reset by hardware when the regulator is ready.
0: Regulator is ready in main mode
1: Regulator voltage is in low power mode */

/* Voltage Scaling select flag */
#define PWR_VOSF (1 << 4)
/* A delay is required for the internal regulator to be ready after the voltage range is changed.
The VOSF bit indicates that the regulator has reached the voltage level defined with bits VOS
of PWR_CR register.
This bit is reset when VOS[1:0] in PWR_CR register change.
It is set once the regulator is ready.
0: Regulator is ready in the selected voltage range
1: Regulator voltage output is changing to the required VOS level. */

/* VREFINTRDYF: Internal voltage reference (V REFINT ) ready flag */
#define PWR_VREFINTRDYF (1 << 3)
/* This bit indicates the state of the internal voltage reference, V REFINT .
0: V REFINT is OFF
1: V REFINT is ready */

/* PVD output */
#define PWR_PVDO (1 << 2)
/* This bit is set and cleared by hardware. It is valid only if PVD is 
   enabled by the PVDE bit.
0: VDD/VDDA is higher than the PVD threshold selected with the PLS[2:0] bits.
1: VDD/VDDA is lower than the PVD threshold selected with the PLS[2:0] bits.
Note: The PVD is stopped by Standby mode. For this reason, this bit is 
equal to 0 after Standby or reset until the PVDE bit is set. */

/* Standby flag */
#define PWR_SBF (1 << 1)
/* This bit is set by hardware and cleared only by a POR/PDR (power-on 
   reset/power-down reset) or by setting the CSBF bit in the PWR power 
   control register (PWR_CR)
0: Device has not been in Standby mode
1: Device has been in Standby mode */

/* Wakeup flag */
#define PWR_WUF (1 << 0)
/* This bit is set by hardware and cleared only by a POR/PDR (power-on 
   reset/power-down reset) or by setting the CWUF bit in the PWR power 
   control register (PWR_CR)
0: No wakeup event occurred
1: A wakeup event was received from the WKUP pin or from the RTC alarm 
(Alarm A or Alarm B), RTC Tamper event, RTC TimeStamp event or RTC Wakeup).
Note: An additional wakeup event is detected if the WKUP pin is enabled 
(by setting the EWUP bit) when the WKUP pin level is already high. */

#ifndef __ASSEMBLER__

#include <stdint.h>

struct stm32_pwr {
	volatile uint32_t cr; /* Control Register */
	volatile uint32_t csr; /* control/status register */
};

#endif /* __ASSEMBLER__ */


#endif /* __STM32L_PWR_H__ */
