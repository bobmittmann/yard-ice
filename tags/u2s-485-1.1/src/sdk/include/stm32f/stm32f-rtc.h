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
 * @file stm32f-rtc.h
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __STM32F_RTC_H__
#define __STM32F_RTC_H__

/*-------------------------------------------------------------------------
  Real-time clock (RTC)
  -------------------------------------------------------------------------*/

/* RTC time register */
#define STM32F_RTC_TR 0x00
/* The RTC_TR is the calendar time shadow register. This register must be 
   written in initialization mode only. Refer to Calendar initialization and 
   configuration on page 546 and Reading the calendar on page 547. */

/* Bits 31-24 Reserved */

/* Bit 23 Reserved, always read as 0. */
/* Bit 22 - AM/PM notation */
#define RTC_PM (1 << 22)
/*	0: AM or 24-hour format
	1: PM */

/* Bits [21..20] - Hour tens in BCD format */
#define RTC_HT_MSK (((1 << (1 + 1)) - 1) << 20)
#define RTC_HT_SET(VAL) (((VAL) << 20) & RTC_HT_MSK)
#define RTC_HT_GET(REG) (((REG) & RTC_HT_MSK) >> 20)

/* Bits [16..16] - Hour units in BCD format */
#define RTC_HU_MSK (((1 << (3 + 1)) - 1) << 16)
#define RTC_HU_SET(VAL) (((VAL) << 16) & RTC_HU_MSK)
#define RTC_HU_GET(REG) (((REG) & RTC_HU_MSK) >> 16)

/* Bit 15 Reserved, always read as 0. */

/* Bits [14..12] - Minute tens in BCD format */
#define RTC_MNT_MSK (((1 << (2 + 1)) - 1) << 12)
#define RTC_MNT_SET(VAL) (((VAL) << 12) & RTC_MNT_MSK)
#define RTC_MNT_GET(REG) (((REG) & RTC_MNT_MSK) >> 12)

/* Bits [11..8] - Minute units in BCD format */
#define RTC_MNU_MSK (((1 << (3 + 1)) - 1) << 8)
#define RTC_MNU_SET(VAL) (((VAL) << 8) & RTC_MNU_MSK)
#define RTC_MNU_GET(REG) (((REG) & RTC_MNU_MSK) >> 8)

/* Bit 7 Reserved, always read as 0. */

/* Bits [6..4] - Second tens in BCD format */
#define RTC_ST_MSK (((1 << (2 + 1)) - 1) << 4)
#define RTC_ST_SET(VAL) (((VAL) << 4) & RTC_ST_MSK)
#define RTC_ST_GET(REG) (((REG) & RTC_ST_MSK) >> 4)

/* Bits [3..0] - Second units in BCD format */
#define RTC_SU_MSK (((1 << (3 + 1)) - 1) << 0)
#define RTC_SU_SET(VAL) (((VAL) << 0) & RTC_SU_MSK)
#define RTC_SU_GET(REG) (((REG) & RTC_SU_MSK) >> 0)
/* This register is write protected. The write access procedure is described 
   in RTC register write protection on page 545. */

/* RTC date register */
#define STM32F_RTC_DR 0x04
/* The RTC_DR is the calendar date shadow register. This register must be 
   written in initialization mode only. Refer to Calendar initialization and 
   configuration on page 546 and Reading the calendar on page 547. */

/* Bits 31-24 Reserved */

/* Bits [23..20] - Year tens in BCD format */
#define RTC_YT_MSK (((1 << (3 + 1)) - 1) << 20)
#define RTC_YT_SET(VAL) (((VAL) << 20) & RTC_YT_MSK)
#define RTC_YT_GET(REG) (((REG) & RTC_YT_MSK) >> 20)

/* Bits [19..16] - Year units in BCD format */
#define RTC_YU_MSK (((1 << (3 + 1)) - 1) << 16)
#define RTC_YU_SET(VAL) (((VAL) << 16) & RTC_YU_MSK)
#define RTC_YU_GET(REG) (((REG) & RTC_YU_MSK) >> 16)

/* Bits [15..13] - Week day units */
#define RTC_WDU_MSK (((1 << (2 + 1)) - 1) << 13)
#define RTC_WDU_SET(VAL) (((VAL) << 13) & RTC_WDU_MSK)
#define RTC_WDU_GET(REG) (((REG) & RTC_WDU_MSK) >> 13)
/*	000: forbidden
	001: Monday
	...
	111: Sunday */

/* Bit 12 - Month tens in BCD format */
#define RTC_MT (1 << 12)

/* Bits [11..8] - Month units in BCD format */
#define RTC_MU ((11 - 8) << 8)

/* [7..6] Reserved, always read as 0. */

/* Bits [5..4] - Date tens in BCD format */
#define RTC_DT_MSK (((1 << (1 + 1)) - 1) << 4)
#define RTC_DT_SET(VAL) (((VAL) << 4) & RTC_DT_MSK)
#define RTC_DT_GET(REG) (((REG) & RTC_DT_MSK) >> 4)

/* Bits [3..0] - Date units in BCD format */
#define RTC_DU_MSK (((1 << (3 + 1)) - 1) << 0)
#define RTC_DU_SET(VAL) (((VAL) << 0) & RTC_DU_MSK)
#define RTC_DU_GET(REG) (((REG) & RTC_DU_MSK) >> 0)
/* This register is write protected. The write access procedure is described 
   in RTC register write protection on page 545. */

/* RTC control register */
#define STM32F_RTC_CR 0x08

/* [31..24] Reserved, always read as 0. */

/* Bit 23 - Calibration output enable */
#define RTC_COE (1 << 23)
/* This bit enables the AFO_CALIB RTC output
	0: Calibration output disabled
	1: Calibration output enabled */

/* Bits [22..21] - Output selection */
#define RTC_OSEL_MSK (((1 << (1 + 1)) - 1) << 21)
#define RTC_OSEL_SET(VAL) (((VAL) << 21) & RTC_OSEL_MSK)
#define RTC_OSEL_GET(REG) (((REG) & RTC_OSEL_MSK) >> 21)
/* These bits are used to select the flag to be routed to AFO_ALARM RTC output
	00: Output disabled
	01: Alarm A output enabled
	10: Alarm B output enabled
	11: Wakeup output enabled */

/* Bit 20 - Output polarity */
#define RTC_POL (1 << 20)
/* This bit is used to configure the polarity of AFO_ALARM RTC output
	0: The pin is high when ALRAF/ALRBF/WUTF is 
		asserted (depending on OSEL[1:0])
	1: The pin is low when ALRAF/ALRBF/WUTF is 
		asserted (depending on OSEL[1:0]). */

/* Bit 19 Reserved, always read as 0. */

/* Bit 18 - Backup */
#define RTC_BKP (1 << 18)
/* This bit can be written by the user to memorize whether the daylight 
   saving time change has been performed or not. */

/* Bit 17 - Subtract 1 hour (winter time change) */
#define RTC_SUB1H (1 << 17)
/* When this bit is set outside initialization mode, 1 hour is subtracted 
   to the calendar time if the current hour is not 0. This bit is always 
   read as 0. Setting this bit has no effect when current hour is 0.
	0: No effect
	1: Subtracts 1 hour to the current time. This can be used for 
		winter time change. */

/* Bit 16 - Add 1 hour (summer time change) */
#define RTC_ADD1H (1 << 16)
/* When this bit is set outside initialization mode, 1 hour is added to 
   the calendar time. This bit is always read as 0.
	0: No effect
	1: Adds 1 hour to the current time. This can be used for 
		summer time change */

/* Bit 15 - Time-stamp interrupt enable */
#define RTC_TSIE (1 << 15)
/*	0: Time-stamp Interrupt disable
	1: Time-stamp Interrupt enable */

/* Bit 14 - Wakeup timer interrupt enable */
#define RTC_WUTIE (1 << 14)
/*	0: Wakeup timer interrupt disabled
	1: Wakeup timer interrupt enabled */

/* Bit 13 - Alarm B interrupt enable */
#define RTC_ALRBIE (1 << 13)
/*	0: Alarm B Interrupt disable
	1: Alarm B Interrupt enable */

/* Bit 12 - Alarm A interrupt enable */
#define RTC_ALRAIE (1 << 12)
/*	0: Alarm A interrupt disabled
	1: Alarm A interrupt enabled */

/* Bit 11 - Time stamp enable */
#define RTC_TSE (1 << 11)
/*	0: Time stamp disable
	1: Time stamp enable */

/* Bit 10 - Wakeup timer enable */
#define RTC_WUTE (1 << 10)
/*	0: Wakeup timer disabled
	1: Wakeup timer enabled */

/* Bit 9 - Alarm B enable */
#define RTC_ALRBE (1 << 9)
/*	0: Alarm B disabled
	1: Alarm B enabled */

/* Bit 8 - Alarm A enable */
#define RTC_ALRAE (1 << 8)
/*	0: Alarm A disabled
	1: Alarm A enabled */

/* Bit 7 - Coarse digital calibration enable */
#define RTC_DCE (1 << 7)
/*	0: Digital calibration disabled
	1: Digital calibration enabled
	PREDIV_A must be 6 or greater */

/* Bit 6 - Hour format */
#define RTC_FMT (1 << 6)
/*	0: 24 hour/day format
	1: AM/PM hour format */

/* Bit 5 Reserved, always read as 0. */

/* Bit 4 - Reference clock detection enable (50 or 60 Hz) */
#define RTC_REFCKON (1 << 4)
/*	0: Reference clock detection disabled
	1: Reference clock detection enabled
	Note: PREDIV_S must be 0x00FF. */

/* Bit 3 - Time-stamp event active edge */
#define RTC_TSEDGE (1 << 3)
/*	0: TIMESTAMP rising edge generates a time-stamp event
	1: TIMESTAMP falling edge generates a time-stamp event
	TSE must be reset when TSEDGE is changed to avoid unwanted TSF setting. */

/* Bits [2..0] - Wakeup clock selection */
#define RTC_WUCKSEL_MSK (((1 << (2 + 1)) - 1) << 0)
#define RTC_WUCKSEL_SET(VAL) (((VAL) << 0) & RTC_WUCKSEL_MSK)
#define RTC_WUCKSEL_GET(REG) (((REG) & RTC_WUCKSEL_MSK) >> 0)
/*	000: RTC/16 clock is selected
	001: RTC/8 clock is selected
	010: RTC/4 clock is selected
	011: RTC/2 clock is selected
	10x: ck_spre (usually 1 Hz) clock is selected
	11x: ck_spre (usually 1 Hz) clock is selected and 216 is added to 
	the WUT counter value (see note below)
Note:
	1 WUT = Wakeup unit counter value. WUT = (0x0000 to 0xFFFF) + 0x10000 
	added when WUCKSEL[2:1 = 11].
	2 Bits 7, 6 and 4 of this register can be written in initialization mode 
	only (RTC_ISR/INITF = 1).
	3 Bits 2 to 0 of this register can be written only when RTC_CR WUTE 
	bit = 0 and RTC_ISR WUTWF bit = 1.
	4 It is recommended not to change the hour during the calendar hour 
	increment as it could mask the incrementation of the calendar hour.
	5 ADD1H and SUB1H changes are effective in the next second.
	6 This register is write protected. The write access procedure is 
	described in RTC register write protection on page 545. */

/* RTC initialization and status register */
#define STM32F_RTC_ISR 0x0C

/* [31..14] Reserved */

/* Bit 13 - Tamper detection flag */
#define RTC_TAMP1F (1 << 13)
/* This flag is set by hardware when a tamper detection event is detected.
   It is cleared by software writing 0. */

/* Bit 12 - Time-stamp overflow flag */
#define RTC_TSOVF (1 << 12)
/* This flag is set by hardware when a time-stamp event occurs while TSF 
   is already set.
   This flag is cleared by software by writing 0. It is recommended to 
   check and then clear TSOVF only after clearing the TSF bit. Otherwise, 
   an overflow might not be noticed if a time-stamp event occurs 
   immediately before the TSF bit is cleared. */

/* Bit 11 - Time-stamp flag */
#define RTC_TSF (1 << 11)
/* This flag is set by hardware when a time-stamp event occurs.
   This flag is cleared by software by writing 0. */

/* Bit 10 - Wakeup timer flag */
#define RTC_WUTF (1 << 10)
/* This flag is set by hardware when the wakeup auto-reload counter reaches 0.
   This flag is cleared by software by writing 0.
   This flag must be cleared by software at least 1.5 RTCCLK periods before 
   WUTF is set to 1 again. */

/* Bit 9 - Alarm B flag */
#define RTC_ALRBF (1 << 9)
/* This flag is set by hardware when the time/date registers 
   (RTC_TR and RTC_DR) match the Alarm B register (RTC_ALRMBR).
   This flag is cleared by software by writing 0. */

/* Bit 8 - Alarm A flag */
#define RTC_ALRAF (1 << 8)
/* This flag is set by hardware when the time/date registers 
   (RTC_TR and RTC_DR) match the Alarm A register (RTC_ALRMAR).
   This flag is cleared by software by writing 0. */

/* Bit 7 - Initialization mode */
#define RTC_INIT (1 << 7)
/*	0: Free running mode
	1: Initialization mode used to program time and date register 
	(RTC_TR and RTC_DR), and prescaler register (RTC_PRER). Counters are 
	stopped and start counting from the new value when INIT is reset. */

/* Bit 6 - Initialization flag */
#define RTC_INITF (1 << 6)
/* When this bit is set to 1, the RTC is in initialization state, and the 
   time, date and prescaler registers can be updated.
	0: Calendar registers update is not allowed
	1: Calendar registers update is allowed. */

/* Bit 5 - Registers synchronization flag */
#define RTC_RSF (1 << 5)
/* This bit is set by hardware each time the calendar registers are copied 
   into the shadow registers (RTC_TRx and RTC_DRx).
   It is cleared either by software or by hardware in initialization mode.
	0: Calendar shadow registers not yet synchronized
	1: Calendar shadow registers synchronized */

/* Bit 4 - Initialization status flag */
#define RTC_INITS (1 << 4)
/* This bit is set by hardware when the calendar year field is different 
   from 0 (power-on reset state).
	0: Calendar has not been initialized
	1: Calendar has been initialized */

/* Bit 3 Reserved, always read as 0. */

/* Bit 2 - Wakeup timer write flag */
#define RTC_WUTWF (1 << 2)
/* This bit is set by hardware when the wakeup timer values can be changed, after the WUTE bit has been set to 0 in RTC_CR.
	0: Wakeup timer configuration update not allowed
	1: Wakeup timer configuration update allowed */

/* Bit 1 - Alarm B write flag */
#define RTC_ALRBWF (1 << 1)
/* This bit is set by hardware when Alarm B values can be changed, after 
   the ALRBE bit has been set to 0 in RTC_CR.
   It is cleared by hardware in initialization mode.
	0: Alarm B update not allowed
	1: Alarm B update allowed. */

/* Bit 0 - Alarm A write flag */
#define RTC_ALRAWF (1 << 0)
/* This bit is set by hardware when Alarm A values can be changed, after 
   the ALRAE bit has been set to 0 in RTC_CR.
   It is cleared by hardware in initialization mode.
	0: Alarm A update not allowed
	1: Alarm A update allowed
Note:
	1 The ALRAF, ALRBF, WUTF and TSF bits are cleared 2 APB clock cycles 
	after programming them to 0.
	2 This register is write protected (except for RTC_ISR[13:8] bits). The 
	write access procedure is described in RTC register write protection 
	on page 545. */

/* RTC prescaler register */
#define STM32F_RTC_PRER 0x10

/* [31..24] Reserved */

/* Bit 23 Reserved, always read as 0. */

/* Bits [22..16] - Asynchronous prescaler factor */
#define RTC_PREDIV_A_MSK (((1 << (6 + 1)) - 1) << 16)
#define RTC_PREDIV_A_SET(VAL) (((VAL) << 16) & PREDIV_A_MSK)
#define RTC_PREDIV_A_GET(REG) (((REG) & PREDIV_A_MSK) >> 16)
/* This is the asynchronous division factor: 
   ck_apre frequency = RTCCLK frequency/(PREDIV_A+1)
   Note: PREDIV_A [6:0]= 000000 is a prohibited value. */

/* [15..13] Reserved, always read as 0. */

/* Bits [12..0] - Synchronous prescaler factor */
#define RTC_PREDIV_S_MSK (((1 << (12 + 1)) - 1) << 0)
#define RTC_PREDIV_S_SET(VAL) (((VAL) << 0) & PREDIV_S_MSK)
#define RTC_PREDIV_S_GET(REG) (((REG) & PREDIV_S_MSK) >> 0)
/* This is the synchronous division factor:
   ck_spre frequency = ck_apre frequency/(PREDIV_S+1)
Note:
	1 This register must be written in initialization mode only. The 
	initialization must be performed in two separate write accesses. Refer to 
	Calendar initialization and configuration on page 546
	2 This register is write protected. The write access procedure is 
	described in RTC register write protection on page 545. */

/* RTC wakeup timer register */
#define STM32F_RTC_WUTR 0x14

/* Bits [15..0] - Wakeup auto-reload value bits */
#define RTC_WUT_MSK (((1 << (15 + 1)) - 1) << 0)
#define RTC_WUT_SET(VAL) (((VAL) << 0) & RTC_WUT_MSK)
#define RTC_WUT_GET(REG) (((REG) & RTC_WUT_MSK) >> 0)
/* When the wakeup timer is enabled (WUTE set to 1), the WUTF flag is set 
   every (WUT[15:0] + 1) ck_wut cycles. The ck_wut period is selected through 
   WUCKSEL[2:0] bits of the RTC_CR register
   When WUCKSEL[2] = 1, the wakeup timer becomes 17-bits and WUCKSEL[1] 
   effectively becomes WUT[16] the most-significant bit to be reloaded into 
   the timer.
Note: The first assertion of WUTF occurs (WUT+1) ck_wut cycles after WUTE 
   is set. Setting WUT[15:0] to 0x0000 with WUCKSEL[2:0] =011 (RTCCLK/2) is 
   forbidden.
Note:
   1 This register can be written only when WUTWF is set to 1 in RTC_ISR.
   2 This register is write protected. The write access procedure is described 
   in RTC register write protection on page 545. */

/* RTC calibration register */
#define STM32F_RTC_CALIBR 0x18

/* [31..8] Reserved */

/* Bit 7 - Digital calibration sign */
#define RTC_DCS (1 << 7)
/*	0: Positive calibration: calendar update frequency is increased
	1: Negative calibration: calendar update frequency is decreased */

/* [6..5] Reserved, always read as 0. */

/* Bits [4..0] - Digital calibration */
#define RTC_DC_MSK (((1 << (4 + 1)) - 1) << 0)
#define RTC_DC_SET(VAL) (((VAL) << 0) & RTC_DC_MSK)
#define RTC_DC_GET(REG) (((REG) & RTC_DC_MSK) >> 0)
/* DCS = 0 (positive calibration)
	00000: + 0 ppm
	00001: + 4 ppm
	00010: + 8 ppm
	..
	11111: + 126 ppm
	DCS = 1 (negative calibration)
	00000: −0 ppm
	00001: −2 ppm
	00010: −4 ppm
	..
	11111: −63 ppm
Note:
	1 This register can be written in initialization mode only 
	(RTC_ISR/INITF = ‘1’).
	2 This register is write protected. The write access procedure is 
	described in RTC register write protection on page 545. */

/* RTC alarm A register */
#define STM32F_RTC_ALRMAR 0x1C

/* Bit 31 - Alarm A date mask */
#define RTC_RTC_MSK4 (1 << 31)
/*	0: Alarm A set if the date/day match
	1: Date/day don’t care in Alarm A comparison */

/* Bit 30 - Week day selection */
#define RTC_WDSEL (1 << 30)
/* 0: DU[3:0] represents the date units
   1: DU[3:0] represents the week day. DT[1:0] is don’t care. */

/* Bits [29..28] - Date tens in BCD format. */
#define RTC_ADT_MSK (((1 << (1 + 1)) - 1) << 28)
#define RTC_ADT_SET(VAL) (((VAL) << 28) & RTC_ADT_MSK)
#define RTC_ADT_GET(REG) (((REG) & RTC_ADT_MSK) >> 28)

/* Bits [27..24] - Date units or day in BCD format. */
#define RTC_ADU_MSK (((1 << (3 + 1)) - 1) << 24)
#define RTC_ADU_SET(VAL) (((VAL) << 24) & RTC_ADU_MSK)
#define RTC_ADU_GET(REG) (((REG) & RTC_ADU_MSK) >> 24)

/* Bit 23 - Alarm A hours mask */
#define RTC_RTC_MSK3 (1 << 23)
/* 0: Alarm A set if the hours match
   1: Hours don’t care in Alarm A comparison */

/* Bit 22 - AM/PM notation */
#define RTC_PM (1 << 22)
/* 0: AM or 24-hour format
   1: PM */

/* Bits [21..20] - Hour tens in BCD format. */
#define RTC_HT_MSK (((1 << (1 + 1)) - 1) << 20)
#define RTC_HT_SET(VAL) (((VAL) << 20) & RTC_HT_MSK)
#define RTC_HT_GET(REG) (((REG) & RTC_HT_MSK) >> 20)

/* Bits [19..16] - Hour units in BCD format. */
#define RTC_HU_MSK (((1 << (3 + 1)) - 1) << 16)
#define RTC_HU_SET(VAL) (((VAL) << 16) & RTC_HU_MSK)
#define RTC_HU_GET(REG) (((REG) & RTC_HU_MSK) >> 16)

/* Bit 15 - Alarm A minutes mask */
#define RTC_RTC_MSK2 (1 << 15)
/* 0: Alarm A set if the minutes match
   1: Minutes don’t care in Alarm A comparison */

/* Bits [14..12] - Minute tens in BCD format. */
#define RTC_MNT_MSK (((1 << (2 + 1)) - 1) << 12)
#define RTC_MNT_SET(VAL) (((VAL) << 12) & RTC_MNT_MSK)
#define RTC_MNT_GET(REG) (((REG) & RTC_MNT_MSK) >> 12)

/* Bits [11..8] - Minute units in BCD format. */
#define RTC_MNU_MSK (((1 << (3 + 1)) - 1) << 8)
#define RTC_MNU_SET(VAL) (((VAL) << 8) & RTC_MNU_MSK)
#define RTC_MNU_GET(REG) (((REG) & RTC_MNU_MSK) >> 8)

/* Bit 7 - Alarm A seconds mask */
#define RTC_RTC_MSK1 (1 << 7)
/* 0: Alarm A set if the seconds match
   1: Seconds don’t care in Alarm A comparison */

/* Bits [6..4] - Second tens in BCD format. */
#define RTC_ST_MSK (((1 << (2 + 1)) - 1) << 4)
#define RTC_ST_SET(VAL) (((VAL) << 4) & RTC_ST_MSK)
#define RTC_ST_GET(REG) (((REG) & RTC_ST_MSK) >> 4)

/* Bits [3..0] - Second units in BCD format. */
#define RTC_SU_MSK (((1 << (3 + 1)) - 1) << 0)
#define RTC_SU_SET(VAL) (((VAL) << 0) & RTC_SU_MSK)
#define RTC_SU_GET(REG) (((REG) & RTC_SU_MSK) >> 0)
/* Note:
	1 This register can be written only when ALRAWF is set to 1 in RTC_ISR, 
	or in initialization mode.
	2 This register is write protected. The write access procedure is 
	described in RTC register write protection on page 545. */

/* RTC alarm B register */
#define STM32F_RTC_ALRMBR 0x20

/* Bit 31 - Alarm B date mask */
#define RTC_RTC_MSK4 (1 << 31)
/* 0: Alarm B set if the date and day match
   1: Date and day don’t care in Alarm B comparison */

/* Bit 30 - Week day selection */
#define RTC_WDSEL (1 << 30)
/* 0: DU[3:0] represents the date units
   1: DU[3:0] represents the week day. DT[1:0] is don’t care. */

/* Bits [29..28] - Date tens in BCD format */
#define RTC_ADT_MSK (((1 << (1 + 1)) - 1) << 28)
#define RTC_ADT_SET(VAL) (((VAL) << 28) & RTC_ADT_MSK)
#define RTC_ADT_GET(REG) (((REG) & RTC_ADT_MSK) >> 28)

/* Bits [27..24] - Date units or day in BCD format */
#define RTC_ADU_MSK (((1 << (3 + 1)) - 1) << 24)
#define RTC_ADU_SET(VAL) (((VAL) << 24) & RTC_ADU_MSK)
#define RTC_ADU_GET(REG) (((REG) & RTC_ADU_MSK) >> 24)

/* Bit 23 - Alarm B hours mask */
#define RTC_RTC_MSK3 (1 << 23)
/* 0: Alarm B set if the hours match
   1: Hours don’t care in Alarm B comparison */

/* Bit 22 - AM/PM notation */
#define RTC_PM (1 << 22)
/* 0: AM or 24-hour format
   1: PM */

/* Bits [21..20] - Hour tens in BCD format */
#define RTC_HT_MSK (((1 << (1 + 1)) - 1) << 20)
#define RTC_HT_SET(VAL) (((VAL) << 20) & RTC_HT_MSK)
#define RTC_HT_GET(REG) (((REG) & RTC_HT_MSK) >> 20)

/* Bits [19..16] - Hour units in BCD format */
#define RTC_HU_MSK (((1 << (3 + 1)) - 1) << 16)
#define RTC_HU_SET(VAL) (((VAL) << 16) & RTC_HU_MSK)
#define RTC_HU_GET(REG) (((REG) & RTC_HU_MSK) >> 16)

/* Bit 15 - Alarm B minutes mask */
#define RTC_RTC_MSK2 (1 << 15)
/* 0: Alarm B set if the minutes match
   1: Minutes don’t care in Alarm B comparison */

/* Bits [14..12] - Minute tens in BCD format */
#define RTC_MNT_MSK (((1 << (2 + 1)) - 1) << 12)
#define RTC_MNT_SET(VAL) (((VAL) << 12) & RTC_MNT_MSK)
#define RTC_MNT_GET(REG) (((REG) & RTC_MNT_MSK) >> 12)

/* Bits [11..8] - Minute units in BCD format */
#define RTC_MNU_MSK (((1 << (3 + 1)) - 1) << 8)
#define RTC_MNU_SET(VAL) (((VAL) << 8) & RTC_MNU_MSK)
#define RTC_MNU_GET(REG) (((REG) & RTC_MNU_MSK) >> 8)

/* Bit 7 - Alarm B seconds mask */
#define RTC_RTC_MSK1 (1 << 7)
/* 0: Alarm B set if the seconds match
   1: Seconds don’t care in Alarm B comparison */

/* Bits [6..4] - Second tens in BCD format */
#define RTC_ST_MSK (((1 << (2 + 1)) - 1) << 4)
#define RTC_ST_SET(VAL) (((VAL) << 4) & RTC_ST_MSK)
#define RTC_ST_GET(REG) (((REG) & RTC_ST_MSK) >> 4)

/* Bits [3..0] - Second units in BCD format */
#define RTC_SU_MSK (((1 << (3 + 1)) - 1) << 0)
#define RTC_SU_SET(VAL) (((VAL) << 0) & RTC_SU_MSK)
#define RTC_SU_GET(REG) (((REG) & RTC_SU_MSK) >> 0)
/* 1 This register can be written only when ALRBWF is set to 1 in RTC_ISR, 
   or in initialization mode.
   2 This register is write protected. The write access procedure is 
   described in RTC register write protection on page 545. */

/* RTC write protection register */
#define STM32F_RTC_WPR 0x24

/* [31..8] Reserved, always read as 0. */

/* Bits [7..0] - Write protection key */
#define RTC_KEY ((7 - 0) << 0)
/* This byte is written by software.
   Reading this byte always returns 0x00.
   Refer to RTC register write protection for a description of how to 
   unlock RTC register write protection. */

/* RTC time stamp time register */
#define STM32F_RTC_TSTR 0x30

/* [31..23] Reserved, always read as 0. */

/* Bit 22 - AM/PM notation */
#define RTC_PM (1 << 22)
/* 0: AM or 24-hour format
   1: PM */

/* Bits [21..20] - Hour tens in BCD format. */
#define RTC_HT_MSK (((1 << (1 + 1)) - 1) << 20)
#define RTC_HT_SET(VAL) (((VAL) << 20) & RTC_HT_MSK)
#define RTC_HT_GET(REG) (((REG) & RTC_HT_MSK) >> 20)

/* Bits [19..16] - Hour units in BCD format. */
#define RTC_HU_MSK (((1 << (3 + 1)) - 1) << 16)
#define RTC_HU_SET(VAL) (((VAL) << 16) & RTC_HU_MSK)
#define RTC_HU_GET(REG) (((REG) & RTC_HU_MSK) >> 16)

/* Bit 15 Reserved, always read as 0. */

/* Bits [14..12] - Minute tens in BCD format. */
#define RTC_MNT_MSK (((1 << (2 + 1)) - 1) << 12)
#define RTC_MNT_SET(VAL) (((VAL) << 12) & RTC_MNT_MSK)
#define RTC_MNT_GET(REG) (((REG) & RTC_MNT_MSK) >> 12)

/* Bits [11..8] - Minute units in BCD format. */
#define RTC_MNU_MSK (((1 << (3 + 1)) - 1) << 8)
#define RTC_MNU_SET(VAL) (((VAL) << 8) & RTC_MNU_MSK)
#define RTC_MNU_GET(REG) (((REG) & RTC_MNU_MSK) >> 8)

/* Bit 7 Reserved, always read as 0. */

/* Bits [6..4] - Second tens in BCD format. */
#define RTC_ST_MSK (((1 << (2 + 1)) - 1) << 4)
#define RTC_ST_SET(VAL) (((VAL) << 4) & RTC_ST_MSK)
#define RTC_ST_GET(REG) (((REG) & RTC_ST_MSK) >> 4)

/* Bits [3..0] - Second units in BCD format. */
#define RTC_SU_MSK (((1 << (3 + 1)) - 1) << 0)
#define RTC_SU_SET(VAL) (((VAL) << 0) & RTC_SU_MSK)
#define RTC_SU_GET(REG) (((REG) & RTC_SU_MSK) >> 0)
/* Note:
   The content of this register is valid only when TSF is set to 1 in 
   RTC_ISR. It is cleared when TSF bit is reset. */

/* RTC time stamp date register */
#define STM32F_RTC_TSDR 0x34

/* [31..16] Reserved, always read as 0. */

/* Bits [15..13] - Week day units */
#define RTC_WDU_MSK (((1 << (2 + 1)) - 1) << 13)
#define RTC_WDU_SET(VAL) (((VAL) << 13) & RTC_WDU_MSK)
#define RTC_WDU_GET(REG) (((REG) & RTC_WDU_MSK) >> 13)

/* Bit 12 - Month tens in BCD format */
#define RTC_MT (1 << 12)

/* Bits [11..8] - Month units in BCD format */
#define RTC_MU_MSK (((1 << (3 + 1)) - 1) << 8)
#define RTC_MU_SET(VAL) (((VAL) << 8) & RTC_MU_MSK)
#define RTC_MU_GET(REG) (((REG) & RTC_MU_MSK) >> 8)

/* [7..6] Reserved, always read as 0. */

/* Bits [5..4] - Date tens in BCD format */
#define RTC_DT_MSK (((1 << (1 + 1)) - 1) << 4)
#define RTC_DT_SET(VAL) (((VAL) << 4) & RTC_DT_MSK)
#define RTC_DT_GET(REG) (((REG) & RTC_DT_MSK) >> 4)

/* Bits [3..0] - Date units in BCD format */
#define RTC_DU_MSK (((1 << (3 + 1)) - 1) << 0)
#define RTC_DU_SET(VAL) (((VAL) << 0) & RTC_DU_MSK)
#define RTC_DU_GET(REG) (((REG) & RTC_DU_MSK) >> 0)
/* The content of this register is valid only when TSF is set to 1 in 
   RTC_ISR. It is cleared when TSF bit is reset. */

/* RTC tamper and alternate function configuration register */
#define STM32F_RTC_TAFCR 0x40

/* [31..19] Reserved. Always read as 0. */

/* Bit 18 - AFO_ALARM output type */
#define RTC_ALARMOUTTYPE (1 << 18)
/* 0: ALARM_AF0 is a push-pull output
   1: ALARM_AF0 is an open-drain output */

/* Bit 17 - TIMESTAMP mapping */
#define RTC_TSINSEL (1 << 17)
/* 0: RTC_AF1 used as TIMESTAMP
   1: RTC_AF2 used as TIMESTAMP */

/* Bit 16 - TAMPER1 mapping */
#define RTC_TAMP1INSEL (1 << 16)
/* 0: RTC_AF1 used as TAMPER
   1: RTC_AF2 used as TAMPER
Note: TAMP1E must be reset when TAMP1INSEL is changed to avoid unwanted 
   setting of TAMP1F. */

/* [15..3] Reserved. Always read as 0. */

/* Bit 2 - Tamper interrupt enable */
#define RTC_TAMPIE (1 << 2)
/* 0: Tamper interrupt disabled
   1: Tamper interrupt enabled */

/* Bit 1 - Active level for tamper 1 */
#define RTC_TAMP1TRG (1 << 1)
/* 0: TAMPER1 rising edge triggers a tamper detection event.
   1: TAMPER1 falling edge triggers a tamper detection event. */

/* Bit 0 - Tamper 1 detection enable */
#define RTC_TAMP1E (1 << 0)
/* 0: Tamper 1 detection disabled
   1: Tamper 1 detection enabled */

/* RTC backup registers */
#define STM32F_RTC_BKPXR 0x50

/* The application can write or read data to and from these registers.
   They are powered-on by VBAT when VDD is switched off, so that they are not 
   reset by System reset, and their contents remain valid when the device 
   operates in low-power mode. This register is reset on a tamper detection 
   event. or when the Flash readout protection is disabled. */

#ifndef __ASSEMBLER__

#include <stdint.h>

struct stm32f_rtc {
	volatile uint32_t tr;
	volatile uint32_t dr;
	volatile uint32_t cr;
	volatile uint32_t isr;
	volatile uint32_t prer;
	volatile uint32_t calibr;
	volatile uint32_t alarmar;
	volatile uint32_t alarmbr;
	volatile uint32_t wpr;
	volatile uint32_t reserved0[2];
	volatile uint32_t tstr;
	volatile uint32_t tsdr;
	volatile uint32_t reserved1[2];
	volatile uint32_t tafcr;
	volatile uint32_t reserved2[3];
	union {
		struct {
			volatile uint32_t bk0r;
			volatile uint32_t bk1r;
			volatile uint32_t bk2r;
			volatile uint32_t bk3r;
			volatile uint32_t bk4r;
			volatile uint32_t bk5r;
			volatile uint32_t bk6r;
			volatile uint32_t bk7r;
			volatile uint32_t bk8r;
			volatile uint32_t bk9r;
			volatile uint32_t bk10r;
			volatile uint32_t bk11r;
			volatile uint32_t bk12r;
			volatile uint32_t bk13r;
			volatile uint32_t bk14r;
			volatile uint32_t bk15r;
			volatile uint32_t bk16r;
			volatile uint32_t bk17r;
			volatile uint32_t bk18r;
			volatile uint32_t bk19r;
		};
		volatile uint32_t bkr[20];
	};
};

#endif /* __ASSEMBLER__ */

#endif /* __STM32F_RTC_H__ */

