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

#ifndef __AT91_MC_H__
#define __AT91_MC_H__

/* Memory Controller Interface */

/* Register definition for MC peripheral  */
#define MC_RCR    (0x00) /* MC Remap Control Register */
#define MC_ASR    (0x04) /* MC Abort Status Register */
#define MC_AASR   (0x08) /* MC Abort Address Status Register */
#define MC_FMR    (0x60) /* MC Flash Mode Register */
#define MC_FCR    (0x64) /* MC Flash Command Register */
#define MC_FSR    (0x68) /* MC Flash Status Register */

/* MC Remap Control Register*/ 
#define MC_RCB          (0x1 <<  0) /* Remap Command Bit */

/* MC Abort Status Register*/ 
#define MC_UNDADD       (0x1 <<  0) /* Undefined Addess Abort Status */
#define MC_MISADD       (0x1 <<  1) /* Misaligned Addess Abort Status */
#define MC_ABTSZ        (0x3 <<  8) /* Abort Size Status */
#define 	MC_ABTSZ_BYTE                 (0x0 <<  8) /* Byte */
#define 	MC_ABTSZ_HWORD                (0x1 <<  8) /* Half-word */
#define 	MC_ABTSZ_WORD                 (0x2 <<  8) /* Word */
#define MC_ABTTYP       (0x3 << 10) /* Abort Type Status */
#define 	MC_ABTTYP_DATAR                (0x0 << 10) /* Data Read */
#define 	MC_ABTTYP_DATAW                (0x1 << 10) /* Data Write */
#define 	MC_ABTTYP_FETCH                (0x2 << 10) /* Code Fetch */
#define MC_MST0         (0x1 << 16) /* Master 0 Abort Source */
#define MC_MST1         (0x1 << 17) /* Master 1 Abort Source */
#define MC_SVMST0       (0x1 << 24) /* Saved Master 0 Abort Source */
#define MC_SVMST1       (0x1 << 25) /* Saved Master 1 Abort Source */

/* MC Flash Mode Register*/ 
#define MC_FRDY         (0x1 <<  0) /* Flash Ready */
#define MC_LOCKE        (0x1 <<  2) /* Lock Error */
#define MC_PROGE        (0x1 <<  3) /* Programming Error */
#define MC_NEBP         (0x1 <<  7) /* No Erase Before Programming */

/* (MC) Flash Wait State */
#define MC_FWS(VAL) ((VAL) << 8)
#define 	MC_FWS_0FWS                 (0x0 <<  8) /* 1 cycle for Read, 2 for Write operations */
#define 	MC_FWS_1FWS                 (0x1 <<  8) /* 2 cycles for Read, 3 for Write operations */
#define 	MC_FWS_2FWS                 (0x2 <<  8) /* 3 cycles for Read, 4 for Write operations */
#define 	MC_FWS_3FWS                 (0x3 <<  8) /* 4 cycles for Read, 4 for Write operations */
/* (MC) Flash Microsecond Cycle Number */
#define MC_FMCN(VAL) ((VAL) << 16)


/* MC Flash Command Register*/ 
#define MC_FCMD         (0xF <<  0) /* Flash Command */
#define 	MC_FCMD_START_PROG           (0x1) /* Starts the programming of th epage specified by PAGEN. */
#define 	MC_FCMD_LOCK                 (0x2) /* Starts a lock sequence of the sector defined by the bits 4 to 7 of the field PAGEN. */
#define 	MC_FCMD_PROG_AND_LOCK        (0x3) /* The lock sequence automatically happens after the programming sequence is completed. */
#define 	MC_FCMD_UNLOCK               (0x4) /* Starts an unlock sequence of the sector defined by the bits 4 to 7 of the field PAGEN. */
#define 	MC_FCMD_ERASE_ALL            (0x8) /* Starts the erase of the entire flash.If at least a page is locked, the command is cancelled. */
#define 	MC_FCMD_SET_GP_NVM           (0xB) /* Set General Purpose NVM bits. */
#define 	MC_FCMD_CLR_GP_NVM           (0xD) /* Clear General Purpose NVM bits. */
#define 	MC_FCMD_SET_SECURITY         (0xF) /* Set Security Bit. */
#define MC_PAGEN(PAGE) (((PAGE) & 0x3ff) <<  8) /* Page Number */
#define MC_KEY(KEY)    (((KEY) & 0xff) << 24) /* Writing Protect Key */

/* MC Flash Command Register*/ 
#define MC_SECURITY     (0x1 <<  4) /* Security Bit Status */
#define MC_GPNVM0       (0x1 <<  8) /* General-purpose Bit 0 Status */
#define MC_GPNVM1       (0x1 <<  9) /* General-purpose Bit 1 Status */
#define MC_GPNVM2       (0x1 << 10) /* General-purpose Bit 2 Status */
#define MC_GPNVM3       (0x1 << 11) /* General-purpose Bit 3 Status */
#define MC_GPNVM4       (0x1 << 12) /* General-purpose Bit 4 Status */
#define MC_GPNVM5       (0x1 << 13) /* General-purpose Bit 5 Status */
#define MC_GPNVM6       (0x1 << 14) /* General-purpose Bit 6 Status */
#define MC_GPNVM7       (0x1 << 15) /* General-purpose Bit 7 Status */
#define MC_LOCKS0       (0x1 << 16) /* Sector 0 Lock Status */
#define MC_LOCKS1       (0x1 << 17) /* Sector 1 Lock Status */
#define MC_LOCKS2       (0x1 << 18) /* Sector 2 Lock Status */
#define MC_LOCKS3       (0x1 << 19) /* Sector 3 Lock Status */
#define MC_LOCKS4       (0x1 << 20) /* Sector 4 Lock Status */
#define MC_LOCKS5       (0x1 << 21) /* Sector 5 Lock Status */
#define MC_LOCKS6       (0x1 << 22) /* Sector 6 Lock Status */
#define MC_LOCKS7       (0x1 << 23) /* Sector 7 Lock Status */
#define MC_LOCKS8       (0x1 << 24) /* Sector 8 Lock Status */
#define MC_LOCKS9       (0x1 << 25) /* Sector 9 Lock Status */
#define MC_LOCKS10      (0x1 << 26) /* Sector 10 Lock Status */
#define MC_LOCKS11      (0x1 << 27) /* Sector 11 Lock Status */
#define MC_LOCKS12      (0x1 << 28) /* Sector 12 Lock Status */
#define MC_LOCKS13      (0x1 << 29) /* Sector 13 Lock Status */
#define MC_LOCKS14      (0x1 << 30) /* Sector 14 Lock Status */
#define MC_LOCKS15      (0x1 << 31) /* Sector 15 Lock Status */

#ifndef __ASSEMBLER__

#include <stdint.h>

struct at91_mc {
	volatile uint32_t rcr; /* remap control register */
	volatile uint32_t asr; /* abort status register */
	volatile uint32_t aasr; /* abort address status register */
	uint32_t reserved0[21];
	volatile uint32_t fmr; /* flash mode register */
	volatile uint32_t fcr; /* flash command register */
	volatile uint32_t fsr; /* flash status register */
};

#endif /* __ASSEMBLER__ */

#endif /* __AT91_MC_H__ */

