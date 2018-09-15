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
 * @file dm36x-prtcss.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#ifndef __DM36X_PRTCSS_H__
#define __DM36X_PRTCSS_H__

/* PRTC Interface (PRTCIF) Registers */

#define PRTCIF_ID 0x00 /* PRTCIF peripheral ID register */
#define PRTCIF_CTRL 0x04 /* PRTCIF control register */
#define PRTCIF_LDATA 0x08 /* PRTCIF access lower data register */
#define PRTCIF_UDATA 0x0c /* PRTCIF access upper data register */
#define PRTCIF_INTEN 0x10 /* PRTCIF interrupt enable register */
#define PRTCIF_INTFLG 0x14 /* PRTCIF interrupt flag register */  

#define CTRL_BUSY (1 << 31)
#define CTRL_SIZE_4 (0 << 25)
#define CTRL_SIZE_8 (1 << 25)
#define CTRL_WR (0 << 24)
#define CTRL_RD (1 << 24)
#define CTRL_BENU(BITS) (((BITS) & 0xf) << 20)
#define CTRL_BENL(BITS) (((BITS) & 0xf) << 16)
#define CTRL_ADRS(ADDR) ((ADDR) & 0xff)

/* Power Management and Real Time Clock Subsystem (PRTCSS) Registers */

#define PRTCSS_GO_OUT 0x00 /* Global output pin output data register */
#define PRTCSS_GIO_OUT 0x01 /* Global input/output pin output data register */
#define PRTCSS_GIO_DIR 0x02 /* Global input/output pin direction register */
#define PRTCSS_GIO_IN 0x03 /* Global input/output pin input data register */
#define PRTCSS_GIO_FUNC 0x04 /* Global input/output pin function register */
#define PRTCSS_GIO_RISE_INT_EN 0x05 /* GIO rise interrupt enable register */
#define PRTCSS_GIO_FALL_INT_EN 0x06 /* GIO fall interrupt enable register */
#define PRTCSS_GIO_RISE_INT_FLG 0x07 /* GIO rise interrupt flag register */
#define PRTCSS_GIO_FALL_INT_FLG 0x08 /* GIO fall interrupt flag register */
#define PRTCSS_INTC_EXTENA0 0x0B /* EXT interrupt enable 0 register */
#define PRTCSS_INTC_EXTENA1 0x0C /* EXT interrupt enable 1 register */
#define PRTCSS_INTC_FLG0 0x0D /* Event interrupt flag 0 register */
#define PRTCSS_INTC_FLG1 0x0E /* Event interrupt flag 1 register */
#define PRTCSS_RTC_CTRL 0x10 /* RTC control register */
#define PRTCSS_RTC_WDT 0x11 /* Watchdog timer counter register */
#define PRTCSS_RTC_TMR0 0x12 /* Timer counter 0 register */
#define PRTCSS_RTC_TMR1 0x13 /* Timer counter 1 register */
#define PRTCSS_RTC_CCTRL 0x14 /* Calendar control register */
#define PRTCSS_RTC_SEC 0x15 /* Seconds register */
#define PRTCSS_RTC_MIN 0x16 /* Minutes register */
#define PRTCSS_RTC_HOUR 0x17 /* Hours register */
#define PRTCSS_RTC_DAY0 0x18 /* Days[[7:0] register */
#define PRTCSS_RTC_DAY1 0x19 /* Days[14:8] register */
#define PRTCSS_RTC_AMIN 0x1A /* Minutes Alarm register */
#define PRTCSS_RTC_AHOUR 0x1B /* Hour Alarm register */
#define PRTCSS_RTC_ADAY0 0x1C /* Days[7:0] Alarm register */
#define PRTCSS_RTC_ADAY1 0x1D /* Days[14:8] Alarm register */
#define PRTCSS_CLKC_CNT 0x20 /* Clock Control Register */

#define CTRL_WDTBUSY (1 <<7) /* WDT access busy flag. -  This bit value must 
								be a 0 before any WDT/Timer register 
								is to be written. */
#define CTRL_WEN (1 << 6) /* Watchdog timer enable */
#define CTRL_WDRT (1 << 5) /* Watchdog timer reset */
#define CTRL_WDTFLG (1 << 4)


#ifndef __ASSEMBLER__

#include <stdint.h>

struct dm36x_prtcif {
	volatile uint32_t id; /* PRTCIF peripheral ID register */
	volatile uint32_t ctrl; /* PRTCIF control register */
	volatile uint32_t ldata; /* PRTCIF access lower data register */
	volatile uint32_t udata; /* PRTCIF access upper data register */
	volatile uint32_t inten; /* PRTCIF interrupt enable register */
	volatile uint32_t intflg; /* PRTCIF interrupt flag register */  
};

#endif /* __ASSEMBLER__ */


#endif /* __DM36X_PRTCSS_H__ */

