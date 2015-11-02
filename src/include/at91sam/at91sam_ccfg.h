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

#ifndef __AT91_CCFG_H__
#define __AT91_CCFG_H__


/* Chip Configuration Registers */
#define CCFG_TCMR    (4) /* EBI 0 Chip Select Assignment Register */

#define CCFG_EBICSA         (12) /* EBI Chip Select Assignment Register */

#define CCFG_EBI0_CSA       (16) /* EBI 0 Chip Select Assignment Register */
#define CCFG_EBI1_CSA       (20) /* EBI 1 Chip Select Assignment Register */
#define CCFG_MATRIXVERSION  (236) /* Version Register */

/* CCFG_EBICSA - EBI Chip Select Assignment Register */
#define EBI_CS1A            (1 <<  1) /* (CCFG) Chip Select 1 Assignment */
#define EBI_CS1A_SMC        (0x0 <<  1) /* (CCFG) Chip Select 1 is assigned to the Static Memory Controller. */
#define EBI_CS1A_SDRAMC     (1 <<  1) /* (CCFG) Chip Select 1 is assigned to the SDRAM Controller. */
#define EBI_CS3A            (1 <<  3) /* (CCFG) Chip Select 3 Assignment */
#define EBI_CS3A_SMC        (0x0 <<  3) /* (CCFG) Chip Select 3 is only assigned to the Static Memory Controller and NCS3 behaves as defined by the SMC. */
#define EBI_CS3A_SM         (1 <<  3) /* (CCFG) Chip Select 3 is assigned to the Static Memory Controller and the SmartMedia Logic is activated. */
#define EBI_CS4A            (1 <<  4) /* (CCFG) Chip Select 4 Assignment */
#define EBI_CS4A_SMC        (0x0 <<  4) /* (CCFG) Chip Select 4 is only assigned to the Static Memory Controller and NCS4 behaves as defined by the SMC. */
#define EBI_CS4A_CF         (1 <<  4) /* (CCFG) Chip Select 4 is assigned to the Static Memory Controller and the CompactFlash Logic (first slot) is activated. */
#define EBI_CS5A            (1 <<  5) /* (CCFG) Chip Select 5 Assignment */
#define EBI_CS5A_SMC        (0x0 <<  5) /* (CCFG) Chip Select 5 is only assigned to the Static Memory Controller and NCS5 behaves as defined by the SMC */
#define EBI_CS5A_CF         (1 <<  5) /* (CCFG) Chip Select 5 is assigned to the Static Memory Controller and the CompactFlash Logic (second slot) is activated. */
#define EBI_DBPUC           (1 <<  8) /* (CCFG) Data Bus Pull-up Configuration */

#define EBI_VDDIOMSEL       (1 << 16)

#ifndef __ASSEMBLER__

#include <stdint.h>

struct at91_ccfg {
	uint32_t res0;
	volatile uint32_t tcmr; /* Bus Matrix TCM Configuration Register */
	uint32_t res1;
	volatile uint32_t ebicsa; /* EBI Chip Select Assignment Register */
	volatile uint32_t ebi0csa; /* EBI 0 Chip Select Assignment Register */
	volatile uint32_t ebi1csa; /* EBI 1 Chip Select Assignment Register */
	uint32_t res2[53];
	volatile uint32_t matrixversion; /* Version Register */
};

#endif /* __ASSEMBLER__ */

#endif /* __AT91_CCFG_H__ */

