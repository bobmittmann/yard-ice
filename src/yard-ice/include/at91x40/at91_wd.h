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


#ifndef __AT9_WD_H__
#define __AT9_WD_H__


/*****************************************************************************
 WDC: Watchdog Timer
 *****************************************************************************/

/* Overflow Mode Register - Read/Write */
#define WD_OMR                      0x00
/* Watch Dog Enable */
#define WD_WDEN                     (1 << 0)
/* Reset Enable */
#define WD_RSTEN                    (1 << 1)
/* Interrupt Enable */
#define WD_IRQEN                    (1 << 2)
/* External Signal Enable */
#define WD_EXTEN                    (1 << 3)
/* Overflow Access Key */
#define WD_OKEY                     (0x234 << 4)

/* Clock Mode Register - Read/Write */
#define WD_CMR                      0x04
/* Clock Selection */
#define WD_WDCLKS_MCK8              (0 << 0)
#define WD_WDCLKS_MCK32             (1 << 0)
#define WD_WDCLKS_MCK128            (2 << 0)
#define WD_WDCLKS_MCK1024           (3 << 0)
/* High Preload Counter Value */
#define WD_HPCV(COUNT)              (((COUNT) >> 10) & 0x003c)

#define WD_HPCV_1                   (1 << 2)
#define WD_HPCV_2                   (2 << 2)
#define WD_HPCV_4                   (4 << 2)
#define WD_HPCV_8                   (8 << 2)
#define WD_HPCV_MAX                 (0x0f << 2)

/* Clock Access Key */
#define WD_CKEY                     (0x06e << 7)

/* Control Register - Write Only */
#define WD_CR                      0x08
/* Restart Key */
#define WD_RSTKEY                  0xc071

/* Status Register - Read Only */
#define WD_SR                      0x0C
/* Watchdog Overflow */
#define WD_WDOVF                    1

#ifndef __ASSEMBLER__

struct at91_wd {
	uint32_t omr;
	uint32_t cmr;
	uint32_t cr;
	volatile uint32_t sr;
	uint32_t res[4092];
};

#endif /* __ASSEMBLER__ */

#endif /* __AT91_WD_H__ */

