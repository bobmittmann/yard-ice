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

#ifndef __STM32F_PWR_H__
#define __STM32F_PWR_H__

/*-------------------------------------------------------------------------
  Power Control (PWR)
  -------------------------------------------------------------------------*/

/* PWR power control register */
#define PWR_CR 0x00 

/* Flash power down in Stop mode. */
#define PWR_FPDS (1 << 9)
/* When set, the Flash memory enters power down mode when the device enters 
   Stop mode. This allows to achieve a lower consumption in stop mode but a 
   longer restart time.
	0: Flash memory not in power down when the device is in Stop mode
	1: Flash memory in power down when the device is in Stop mode */

/* Disable backup domain write protection. */
#define PWR_DBP (1 << 8)
/* In reset state, the RCC_BDCR register, the RTC registers (including the 
   backup registers), and the BRE bit of the PWR_CSR register, are protected 
   against parasitic write access. This bit must be set to enable write access 
   to these registers.
	0: Access to RTC and RTC Backup registers and backup SRAM disabled
	1: Access to RTC and RTC Backup registers and backup SRAM enabled */

/* PVD level selection. */
#define PWR_PLS (0x7 << 5)
#define PWR_PLS_2_0V (0x0 << 5)
#define PWR_PLS_2_1V (0x1 << 5)
#define PWR_PLS_2_3V (0x2 << 5)
#define PWR_PLS_2_5V (0x3 << 5)
#define PWR_PLS_2_6V (0x4 << 5)
#define PWR_PLS_2_7V (0x5 << 5)
#define PWR_PLS_2_8V (0x6 << 5)
#define PWR_PLS_2_9V (0x7 << 5)
/* These bits are written by software to select the voltage threshold detected 
   by the Power Voltage Detector
	000: 2.0 V
	001: 2.1 V
	010: 2.3 V
	011: 2.5 V
	100: 2.6 V
	101: 2.7 V
	110: 2.8 V
	111: 2.9 V
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

/* Low-power deep sleep */
#define PWR_LPDS (1 << 0)
/* This bit is set and cleared by software. It works together with the PDDS bit.
0: Voltage regulator on during Stop mode
1: Voltage regulator in low-power mode during Stop mode */


/* PWR power control/status register */
#define PWR_CSR 0x04

/* Backup regulator enable */
#define PWR_BRE (1 << 9)
/* When set, the Backup regulator (used to maintain backup SRAM content in 
   Standby and VBAT modes) is enabled. If BRE is reset, the backup regulator is 
   switched off. The backup SRAM can still be used but its content will be 
   lost in the Standby and VBAT modes. Once set, the application must wait 
   that the Backup Regulator Ready flag (BRR) is set to indicate that the
   data written into the RAM will be maintained in the Standby and VBAT modes.
0: Backup regulator disabled
1: Backup regulator enabled
Note: This bit is not reset when the device wakes up from Standby mode, 
by a system reset, or by a power reset. */

/* Enable WKUP pin */
#define PWR_EWUP (1 << 8)
/*   This bit is set and cleared by software.
0: WKUP pin is used for general purpose I/O. An event on the WKUP 
pin does not wakeup the device from Standby mode.
1: WKUP pin is used for wakeup from Standby mode and forced in input pull 
down configuration (rising edge on WKUP pin wakes-up the system from 
Standby mode).
Note: This bit is reset by a system reset. */

/* Backup regulator ready */
#define PWR_BRR (1 << 3)
/* Set by hardware to indicate that the Backup Regulator is ready.
0: Backup Regulator not ready
1: Backup Regulator ready
Note: This bit is not reset when the device wakes up from Standby 
mode or by a system reset or power reset. */

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


#endif /* __STM32F_PWR_H__ */
