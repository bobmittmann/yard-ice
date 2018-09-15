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
 * @file arch/at91_.h
 * @brief ARM CPU macros and definitions 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#ifndef __AT91_WDT_H__
#define __AT91_WDT_H__

/* Watchdog Timer Controller Interface */

#define WDTC_WDCR       ( 0) /* Watchdog Control Register */
#define WDTC_WDMR       ( 4) /* Watchdog Mode Register */
#define WDTC_WDSR       ( 8) /* Watchdog Status Register */

/* WDTC_WDCR - Periodic Interval Image Register */
#define WDTC_WDRSTT         (0x1 <<  0) /* (WDTC) Watchdog Restart */
#define WDTC_KEY            (0xff << 24) /* (WDTC) Watchdog KEY Password */

/* WDTC_WDMR - Watchdog Mode Register */
#define WDTC_WDV            (0xfff <<  0) /* (WDTC) Watchdog Timer Restart */
#define WDTC_WDFIEN         (0x1 << 12) /* (WDTC) Watchdog Fault Interrupt Enable */
#define WDTC_WDRSTEN        (0x1 << 13) /* (WDTC) Watchdog Reset Enable */
#define WDTC_WDRPROC        (0x1 << 14) /* (WDTC) Watchdog Timer Restart */
#define WDTC_WDDIS          (0x1 << 15) /* (WDTC) Watchdog Disable */
#define WDTC_WDD            (0xfff << 16) /* (WDTC) Watchdog Delta Value */
#define WDTC_WDDBGHLT       (0x1 << 28) /* (WDTC) Watchdog Debug Halt */
#define WDTC_WDIDLEHLT      (0x1 << 29) /* (WDTC) Watchdog Idle Halt */

/* WDTC_WDSR - Watchdog Status Register */
#define WDTC_WDUNF          (0x1 <<  0) /* (WDTC) Watchdog Underflow */
#define WDTC_WDERR          (0x1 <<  1) /* (WDTC) Watchdog Error */

#ifndef __ASSEMBLER__

struct at91_wdtc {
	volatile uint32_t wdcr; /* Watchdog Control Register */
	volatile uint32_t wdmr; /* Watchdog Mode Register */
	volatile uint32_t wdsr; /* Watchdog Status Register */
};

#endif /* __ASSEMBLER__ */


#endif /* __AT91_WDT_H__ */

