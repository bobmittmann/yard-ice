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

#ifndef __AT91_RTTC_H__
#define __AT91_RTTC_H__

/* Real Time Timer Controller Interface */

#define RTTC_RTMR       ( 0) /* Real-time Mode Register */
#define RTTC_RTAR       ( 4) /* Real-time Alarm Register */
#define RTTC_RTVR       ( 8) /* Real-time Value Register */
#define RTTC_RTSR       (12) /* Real-time Status Register */

/* RTTC_RTMR - Real-time Mode Register */
#define RTTC_RTPRES         (0xffff <<  0) /* Real-time Timer Prescaler Value */
#define RTTC_RTPRES_SET(X)  (((X) & 0xffff) <<  0) /* Real-time Timer Prescaler Value */
#define RTTC_RTPRES_GET(MR) (((MR) >> 0) & 0xffff) /* Real-time Timer Prescaler Value */
#define RTTC_ALMIEN         (0x1 << 16) /* Alarm Interrupt Enable */
#define RTTC_RTTINCIEN      (0x1 << 17) /* Real Time Timer Increment Interrupt Enable */
#define RTTC_RTTRST         (0x1 << 18) /* Real Time Timer Restart */

/* RTTC_RTAR - Real-time Alarm Register */
#define RTTC_ALMV           (0x0 <<  0) /* Alarm Value */

/* RTTC_RTVR - Current Real-time Value Register */
#define RTTC_CRTV           (0x0 <<  0) /* Current Real-time Value */

/* RTTC_RTSR - Real-time Status Register */
#define RTTC_ALMS           (0x1 <<  0) /* Real-time Alarm Status */
#define RTTC_RTTINC         (0x1 <<  1) /* Real-time Timer Increment */

#ifndef __ASSEMBLER__

#include <stdint.h>

struct at91_rttc {
	uint32_t rtmr; /* Real-time Mode Register */
	uint32_t rtar; /* Real-time Alarm Register */
	uint32_t rtvr; /* Real-time Value Register */
	uint32_t rtsr; /* Real-time Status Register */
};

#endif /* __ASSEMBLER__ */


#endif /* __AT91_RTTC_H__ */
