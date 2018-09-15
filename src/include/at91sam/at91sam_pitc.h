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

#ifndef __AT91_PITC_H__
#define __AT91_PITC_H__

/* Periodic Interval Timer Controller Interface */

#define PIMR       ( 0) /* Period Interval Mode Register */
#define PISR       ( 4) /* Period Interval Status Register */
#define PIVR       ( 8) /* Period Interval Value Register */
#define PIIR       (12) /* Period Interval Image Register */

/* PITC_PIMR - Periodic Interval Mode Register */
#define PITC_PIV(X) (0xfffff & (X)) /* (PITC) Periodic Interval Value */
#define PITC_PITEN  (0x1 << 24) /* (PITC) Periodic Interval Timer Enabled */
#define PITC_PITIEN (0x1 << 25) /* (PITC) Periodic Interval Timer Interrupt Enable */

/* PITC_PISR - Periodic Interval Status Register */
#define PITC_PITS   (0x1 <<  0) /* (PITC) Periodic Interval Timer Status */

/* PITC_PIVR - Periodic Interval Value Register */
#define PITC_CPIV   (0xfffff <<  0) /* (PITC) Current Periodic Interval Value */
#define PITC_PICNT  (0xfff << 20) /* (PITC) Periodic Interval Counter */


#define PICNT(PIVR)  ((PIVR) >> 20) /* (PITC) Periodic Interval Counter */

/* PITC_PIIR - Periodic Interval Image Register */

#ifndef __ASSEMBLER__

#include <stdint.h>

struct at91_pitc {
	volatile uint32_t mr; /* Periodic Interval Mode Register */
	volatile uint32_t sr; /* Periodic Interval Status Register */
	volatile uint32_t pivr; /* Periodic Interval Value Register */
	volatile uint32_t piir; /* Periodic Interval Image Register */
};

#endif /* __ASSEMBLER__ */


#endif /* __AT91_PITC_H__ */

