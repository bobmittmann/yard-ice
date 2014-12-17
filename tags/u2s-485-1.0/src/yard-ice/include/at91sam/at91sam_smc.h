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

#ifndef __AT91_SMC_H__
#define __AT91_SMC_H__

/* Static Memory Controller Interface */

#define SMC_SETUP0      ( 0) /* Setup Register for CS 0 */
#define SMC_PULSE0      ( 4) /* Pulse Register for CS 0 */
#define SMC_CYCLE0      ( 8) /* Cycle Register for CS 0 */
#define SMC_MODE0       (12) /* Control Register for CS 0 */
#define SMC_SETUP1      (16) /* Setup Register for CS 1 */
#define SMC_PULSE1      (20) /* Pulse Register for CS 1 */
#define SMC_CYCLE1      (24) /* Cycle Register for CS 1 */
#define SMC_MODE1       (28) /* Control Register for CS 1 */
#define SMC_SETUP2      (32) /* Setup Register for CS 2 */
#define SMC_PULSE2      (36) /* Pulse Register for CS 2 */
#define SMC_CYCLE2      (40) /* Cycle Register for CS 2 */
#define SMC_MODE2       (44) /* Control Register for CS 2 */
#define SMC_SETUP3      (48) /* Setup Register for CS 3 */
#define SMC_PULSE3      (52) /* Pulse Register for CS 3 */
#define SMC_CYCLE3      (56) /* Cycle Register for CS 3 */
#define SMC_MODE3       (60) /* Control Register for CS 3 */
#define SMC_SETUP4      (64) /* Setup Register for CS 4 */
#define SMC_PULSE4      (68) /* Pulse Register for CS 4 */
#define SMC_CYCLE4      (72) /* Cycle Register for CS 4 */
#define SMC_MODE4       (76) /* Control Register for CS 4 */
#define SMC_SETUP5      (80) /* Setup Register for CS 5 */
#define SMC_PULSE5      (84) /* Pulse Register for CS 5 */
#define SMC_CYCLE5      (88) /* Cycle Register for CS 5 */
#define SMC_MODE5       (92) /* Control Register for CS 5 */
#define SMC_SETUP6      (96) /* Setup Register for CS 6 */
#define SMC_PULSE6      (100) /* Pulse Register for CS 6 */
#define SMC_CYCLE6      (104) /* Cycle Register for CS 6 */
#define SMC_MODE6       (108) /* Control Register for CS 6 */
#define SMC_SETUP7      (112) /* Setup Register for CS 7 */
#define SMC_PULSE7      (116) /* Pulse Register for CS 7 */
#define SMC_CYCLE7      (120) /* Cycle Register for CS 7 */
#define SMC_MODE7       (124) /* Control Register for CS 7 */

/* SMC_SETUP - Setup Register for CS x */

/* (SMC) NWE Setup Length */
#define SMC_NWESETUP(VAL)   (((((VAL) >> 2) & 0x20) + ((VAL) & 0x1f)) << 0)
/* (SMC) NCS Setup Length in WRite Access */
#define SMC_NCSSETUPWR(VAL) (((((VAL) >> 2) & 0x20) + ((VAL) & 0x1f)) << 8)
/* (SMC) NRD Setup Length */
#define SMC_NRDSETUP(VAL)   (((((VAL) >> 2) & 0x20) + ((VAL) & 0x1f)) << 16)
/* (SMC) NCS Setup Length in ReaD Access */
#define SMC_NCSSETUPRD(VAL) (((((VAL) >> 2) & 0x20) + ((VAL) & 0x1f)) << 24)

/* SMC_PULSE - Pulse Register for CS x */
/* (SMC) NWE Pulse Length */
#define SMC_NWEPULSE(VAL)   (((((VAL) >> 2) & 0x40) + ((VAL) & 0x3f)) << 0)
/* (SMC) NCS Pulse Length in WRite Access */
#define SMC_NCSPULSEWR(VAL) (((((VAL) >> 2) & 0x40) + ((VAL) & 0x3f)) << 8)
/* (SMC) NRD Pulse Length */
#define SMC_NRDPULSE(VAL)   (((((VAL) >> 2) & 0x40) + ((VAL) & 0x3f)) << 16)
/* (SMC) NCS Pulse Length in ReaD Access */
#define SMC_NCSPULSERD(VAL) (((((VAL) >> 2) & 0x40) + ((VAL) & 0x3f)) << 24)

/* SMC_CYC - Cycle Register for CS x */
/* (SMC) Total Write Cycle Length */
#define SMC_NWECYCLE(VAL)   (((((VAL) >> 1) & 0x180) + ((VAL) & 0x07f)) << 0)
/* (SMC) Total Read Cycle Length */
#define SMC_NRDCYCLE(VAL)   (((((VAL) >> 1) & 0x180) + ((VAL) & 0x07f)) << 16)

/* SMC_MODE - Mode Register for CS x */
#define SMC_READMODE        (0x1 <<  0) /* (SMC) Read Mode */
#define SMC_WRITEMODE       (0x1 <<  1) /* (SMC) Write Mode */
#define SMC_NWAITM          (0x3 <<  5) /* (SMC) NWAIT Mode */
#define 	SMC_NWAITM_NWAIT_DISABLE        (0x0 <<  5) /* (SMC) External NWAIT disabled. */
#define 	SMC_NWAITM_NWAIT_ENABLE_FROZEN  (0x2 <<  5) /* (SMC) External NWAIT enabled in frozen mode. */
#define 	SMC_NWAITM_NWAIT_ENABLE_READY   (0x3 <<  5) /* (SMC) External NWAIT enabled in ready mode. */
#define SMC_BAT             (0x1 <<  8) /* (SMC) Byte Access Type */
#define 	SMC_BAT_BYTE_SELECT          (0x0 <<  8) /* (SMC) Write controled by ncs, nbs0, nbs1, nbs2, nbs3. Read controled by ncs, nrd, nbs0, nbs1, nbs2, nbs3. */
#define 	SMC_BAT_BYTE_WRITE           (0x1 <<  8) /* (SMC) Write controled by ncs, nwe0, nwe1, nwe2, nwe3. Read controled by ncs and nrd. */
#define SMC_DBW             (0x3 << 12) /* (SMC) Data Bus Width */
#define 	SMC_DBW_WIDTH_8_BITS     (0x0 << 12) /* (SMC) 8 bits. */
#define 	SMC_DBW_WIDTH_16_BITS   (0x1 << 12) /* (SMC) 16 bits. */
#define 	SMC_DBW_WIDTH_32_BITS (0x2 << 12) /* (SMC) 32 bits. */
#define SMC_TDF_CYCLES(VAL) (((VAL) & 0xf) << 16) /* (SMC) Data Float Time. */
#define SMC_TDF_MODE         (0x1 << 20) /* (SMC) TDF Enabled. */
#define SMC_PMEN            (0x1 << 24) /* (SMC) Page Mode Enabled. */
#define SMC_PS              (0x3 << 28) /* (SMC) Page Size */
#define 	SMC_PS_SIZE_4_BYTES      (0x0 << 28) /* (SMC) 4 bytes. */
#define 	SMC_PS_SIZE_8_BYTES     (0x1 << 28) /* (SMC) 8 bytes. */
#define 	SMC_PS_SIZE_16_BYTES   (0x2 << 28) /* (SMC) 16 bytes. */
#define 	SMC_PS_SIZE_32_BYTES (0x3 << 28) /* (SMC) 32 bytes. */

#ifndef __ASSEMBLER__

#include <stdint.h>

struct at91_smc {
	volatile uint32_t setup0; /* Setup Register for CS 0 */
	volatile uint32_t pulse0; /* Pulse Register for CS 0 */
	volatile uint32_t cycle0; /* Cycle Register for CS 0 */
	volatile uint32_t mode0; /* Control Register for CS 0 */
	volatile uint32_t setup1; /* Setup Register for CS 1 */
	volatile uint32_t pulse1; /* Pulse Register for CS 1 */
	volatile uint32_t cycle1; /* Cycle Register for CS 1 */
	volatile uint32_t mode1; /* Control Register for CS 1 */
	volatile uint32_t setup2; /* Setup Register for CS 2 */
	volatile uint32_t pulse2; /* Pulse Register for CS 2 */
	volatile uint32_t cycle2; /* Cycle Register for CS 2 */
	volatile uint32_t mode2; /* Control Register for CS 2 */
	volatile uint32_t setup3; /* Setup Register for CS 3 */
	volatile uint32_t pulse3; /* Pulse Register for CS 3 */
	volatile uint32_t cycle3; /* Cycle Register for CS 3 */
	volatile uint32_t mode3; /* Control Register for CS 3 */
	volatile uint32_t setup4; /* Setup Register for CS 4 */
	volatile uint32_t pulse4; /* Pulse Register for CS 4 */
	volatile uint32_t cycle4; /* Cycle Register for CS 4 */
	volatile uint32_t mode4; /* Control Register for CS 4 */
	volatile uint32_t setup5; /* Setup Register for CS 5 */
	volatile uint32_t pulse5; /* Pulse Register for CS 5 */
	volatile uint32_t cycle5; /* Cycle Register for CS 5 */
	volatile uint32_t mode5; /* Control Register for CS 5 */
	volatile uint32_t setup6; /* Setup Register for CS 6 */
	volatile uint32_t pulse6; /* Pulse Register for CS 6 */
	volatile uint32_t cycle6; /* Cycle Register for CS 6 */
	volatile uint32_t mode6; /* Control Register for CS 6 */
	volatile uint32_t setup7; /* Setup Register for CS 7 */
	volatile uint32_t pulse7; /* Pulse Register for CS 7 */
	volatile uint32_t cycle7; /* Cycle Register for CS 7 */
	volatile uint32_t mode7; /* Control Register for CS 7 */
};

#endif /* __ASSEMBLER__ */


#endif /* __AT91_SMC_H__ */
