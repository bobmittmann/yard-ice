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

#ifndef __AT91_SHDWC_H__
#define __AT91_SHDWC_H__


/* Shut Down Controller Interface */

#define SHDWC_SHCR      ( 0) /* Shut Down Control Register */
#define SHDWC_SHMR      ( 4) /* Shut Down Mode Register */
#define SHDWC_SHSR      ( 8) /* Shut Down Status Register */

/* SHDWC_SHCR - Shut Down Control Register */
#define SHDWC_SHDW          (0x1 <<  0) /* (SHDWC) Processor Reset */
#define SHDWC_KEY(KEY)  (((KEY) & 0xff) << 24) /* (SHDWC) Shut down KEY Password */

/* SHDWC_SHMR - Shut Down Mode Register */
#define SHDWC_WKMODE0       (0x3 <<  0) /* (SHDWC) Wake Up 0 Mode Selection */
#define 	SHDWC_WKMODE0_NONE                 (0x0) /* (SHDWC) None. No detection is performed on the wake up input. */
#define 	SHDWC_WKMODE0_HIGH                 (0x1) /* (SHDWC) High Level. */
#define 	SHDWC_WKMODE0_LOW                  (0x2) /* (SHDWC) Low Level. */
#define 	SHDWC_WKMODE0_ANYLEVEL             (0x3) /* (SHDWC) Any level change. */
#define SHDWC_CPTWK0(X)  (((X) & 0xf) <<  4) /* (SHDWC) Counter On Wake Up 0 */
#define SHDWC_WKMODE1       (0x3 <<  8) /* (SHDWC) Wake Up 1 Mode Selection */
#define 	SHDWC_WKMODE1_NONE                 (0x0 <<  8) /* (SHDWC) None. No detection is performed on the wake up input. */
#define 	SHDWC_WKMODE1_HIGH                 (0x1 <<  8) /* (SHDWC) High Level. */
#define 	SHDWC_WKMODE1_LOW                  (0x2 <<  8) /* (SHDWC) Low Level. */
#define 	SHDWC_WKMODE1_ANYLEVEL             (0x3 <<  8) /* (SHDWC) Any level change. */
#define SHDWC_CPTWK1(X) (((X) & 0xf) << 12) /* (SHDWC) Counter On Wake Up 1 */
#define SHDWC_RTTWKEN       (0x1 << 16) /* (SHDWC) Real Time Timer Wake Up Enable */
#define SHDWC_RTCWKEN       (0x1 << 17) /* (SHDWC) Real Time Clock Wake Up Enable */

/* SHDWC_SHSR - Shut Down Status Register */
#define SHDWC_WAKEUP0       (0x1 <<  0) /* (SHDWC) Wake Up 0 Status */
#define SHDWC_WAKEUP1       (0x1 <<  1) /* (SHDWC) Wake Up 1 Status */
#define SHDWC_FWKUP         (0x1 <<  2) /* (SHDWC) Force Wake Up Status */
#define SHDWC_RTTWK         (0x1 << 16) /* (SHDWC) Real Time Timer wake Up */
#define SHDWC_RTCWK         (0x1 << 17) /* (SHDWC) Real Time Clock wake Up */

#ifndef __ASSEMBLER__

#include <stdint.h>

struct at91_shdwc {
	uint32_t shcr; /* Shut Down Control Register */
	uint32_t shmr; /* Shut Down Mode Register */
	uint32_t shsr; /* Shut Down Status Register */
};

#endif /* __ASSEMBLER__ */

#endif /* __AT91_SHDWC_H__ */

