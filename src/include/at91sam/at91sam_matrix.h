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

#ifndef __AT91_MATRIX_H__
#define __AT91_MATRIX_H__

/* AHB Matrix Interface */
#define MATRIX_MCFG0    ( 0) /* Master Configuration Register 0 (ram96k) */
#define MATRIX_MCFG1    ( 4) /* Master Configuration Register 1 (rom) */
#define MATRIX_MCFG2    ( 8) /* Master Configuration Register 2 (hperiphs) */
#define MATRIX_MCFG3    (12) /* Master Configuration Register 3 (ebi) */
#define MATRIX_MCFG4    (16) /* Master Configuration Register 4 (bridge) */
#define MATRIX_MCFG5    (20) /* Master Configuration Register 5 (mailbox) */
#define MATRIX_SCFG0    (64) /* Slave Configuration Register 0 (ram96k) */
#define MATRIX_SCFG1    (68) /* Slave Configuration Register 1 (rom) */
#define MATRIX_SCFG2    (72) /* Slave Configuration Register 2 (hperiphs) */
#define MATRIX_SCFG3    (76) /* Slave Configuration Register 3 (ebi) */
#define MATRIX_SCFG4    (80) /* Slave Configuration Register 4 (bridge) */
#define MATRIX_PRAS0    (128) /* PRAS0 (ram0) */
#define MATRIX_PRAS1    (136) /* PRAS1 (ram1) */
#define MATRIX_PRAS2    (144) /* PRAS2 (ram2) */
#define MATRIX_PRAS3    (152) /* PRAS3 (ebi) */
#define MATRIX_PRAS4    (160) /* PRAS4 (periph) */
#define MATRIX_MRCR     (256) /* Master Remp Control Register */

/* -------- MATRIX_SCFG0 : (MATRIX Offset: 0x40) Slave Configuration Register 0 -------- */
#define MATRIX_SLOT_CYCLE   (0xff <<  0) /* (MATRIX) Maximum Number of Allowed Cycles for a Burst */
#define MATRIX_DEFMSTR_TYPE (0x3 << 16) /* (MATRIX) Default Master Type */
#define  MATRIX_DEFMSTR_TYPE_NO_DEFMSTR           (0x0 << 16) /* (MATRIX) No Default Master. At the end of current slave access, if no other master request is pending, the slave is deconnected from all masters. This results in having a one cycle latency for the first transfer of a burst. */
#define  MATRIX_DEFMSTR_TYPE_LAST_DEFMSTR         (0x1 << 16) /* (MATRIX) Last Default Master. At the end of current slave access, if no other master request is pending, the slave stay connected with the last master having accessed it. This results in not having the one cycle latency when the last master re-trying access on the slave. */
#define  MATRIX_DEFMSTR_TYPE_FIXED_DEFMSTR        (0x2 << 16) /* (MATRIX) Fixed Default Master. At the end of current slave access, if no other master request is pending, the slave connects with fixed which number is in FIXED_DEFMSTR field. This results in not having the one cycle latency when the fixed master re-trying access on the slave. */
#define MATRIX_FIXED_DEFMSTR0 (0x7 << 18) /* (MATRIX) Fixed Index of Default Master */
#define  MATRIX_FIXED_DEFMSTR0_ARM926I              (0x0 << 18) /* (MATRIX) ARM926EJ-S Instruction Master is Default Master */
#define  MATRIX_FIXED_DEFMSTR0_ARM926D              (0x1 << 18) /* (MATRIX) ARM926EJ-S Data Master is Default Master */
#define  MATRIX_FIXED_DEFMSTR0_HPDC3                (0x2 << 18) /* (MATRIX) HPDC3 Master is Default Master */
#define  MATRIX_FIXED_DEFMSTR0_LCDC                 (0x3 << 18) /* (MATRIX) LCDC Master is Default Master */
#define  MATRIX_FIXED_DEFMSTR0_DMA                  (0x4 << 18) /* (MATRIX) DMA Master is Default Master */

/* MATRIX_SCFG1 - Slave Configuration Register 1 */
#define MATRIX_FIXED_DEFMSTR1 (0x7 << 18) /* (MATRIX) Fixed Index of Default Master */
#define  MATRIX_FIXED_DEFMSTR1_ARM926I              (0x0 << 18) /* (MATRIX) ARM926EJ-S Instruction Master is Default Master */
#define  MATRIX_FIXED_DEFMSTR1_ARM926D              (0x1 << 18) /* (MATRIX) ARM926EJ-S Data Master is Default Master */
#define  MATRIX_FIXED_DEFMSTR1_HPDC3                (0x2 << 18) /* (MATRIX) HPDC3 Master is Default Master */
#define  MATRIX_FIXED_DEFMSTR1_LCDC                 (0x3 << 18) /* (MATRIX) LCDC Master is Default Master */
#define  MATRIX_FIXED_DEFMSTR1_DMA                  (0x4 << 18) /* (MATRIX) DMA Master is Default Master */

/* MATRIX_SCFG2 - Slave Configuration Register 2 */
#define MATRIX_FIXED_DEFMSTR2 (0x1 << 18) /* (MATRIX) Fixed Index of Default Master */
#define  MATRIX_FIXED_DEFMSTR2_ARM926I              (0x0 << 18) /* (MATRIX) ARM926EJ-S Instruction Master is Default Master */
#define  MATRIX_FIXED_DEFMSTR2_ARM926D              (0x1 << 18) /* (MATRIX) ARM926EJ-S Data Master is Default Master */

/* MATRIX_SCFG3 - Slave Configuration Register 3 */
#define MATRIX_FIXED_DEFMSTR3 (0x7 << 18) /* (MATRIX) Fixed Index of Default Master */
#define  MATRIX_FIXED_DEFMSTR3_ARM926I              (0x0 << 18) /* (MATRIX) ARM926EJ-S Instruction Master is Default Master */
#define  MATRIX_FIXED_DEFMSTR3_ARM926D              (0x1 << 18) /* (MATRIX) ARM926EJ-S Data Master is Default Master */
#define  MATRIX_FIXED_DEFMSTR3_HPDC3                (0x2 << 18) /* (MATRIX) HPDC3 Master is Default Master */
#define  MATRIX_FIXED_DEFMSTR3_LCDC                 (0x3 << 18) /* (MATRIX) LCDC Master is Default Master */
#define  MATRIX_FIXED_DEFMSTR3_DMA                  (0x4 << 18) /* (MATRIX) DMA Master is Default Master */

/* MATRIX_SCFG4 - Slave Configuration Register 4 */
#define MATRIX_FIXED_DEFMSTR4 (0x3 << 18) /* (MATRIX) Fixed Index of Default Master */
#define  MATRIX_FIXED_DEFMSTR4_ARM926I              (0x0 << 18) /* (MATRIX) ARM926EJ-S Instruction Master is Default Master */
#define  MATRIX_FIXED_DEFMSTR4_ARM926D              (0x1 << 18) /* (MATRIX) ARM926EJ-S Data Master is Default Master */
#define  MATRIX_FIXED_DEFMSTR4_HPDC3                (0x2 << 18) /* (MATRIX) HPDC3 Master is Default Master */

/* MATRIX_MRCR - MRCR Register */
#define MATRIX_RCA926I      (0x1 <<  0) /* (MATRIX) Remap Command for ARM926EJ-S Instruction Master */
#define MATRIX_RCA926D      (0x1 <<  1) /* (MATRIX) Remap Command for ARM926EJ-S Data Master */


#ifndef __ASSEMBLER__

#include <stdint.h>

struct at91_matrix {
	volatile uint32_t mcfg0; /* Master Configuration Register 0 (ram96k) */
	volatile uint32_t mcfg1; /* Master Configuration Register 1 (rom) */
	volatile uint32_t mcfg2; /* Master Configuration Register 2 (hperiphs) */
	volatile uint32_t mcfg3; /* Master Configuration Register 3 (ebi) */
	volatile uint32_t mcfg4; /* Master Configuration Register 4 (bridge) */
	volatile uint32_t mcfg5; /* Master Configuration Register 5 (mailbox) */
	volatile uint32_t scfg0; /* Slave Configuration Register 0 (ram96k) */
	volatile uint32_t scfg1; /* Slave Configuration Register 1 (rom) */
	volatile uint32_t scfg2; /* Slave Configuration Register 2 (hperiphs) */
	volatile uint32_t scfg3; /* Slave Configuration Register 3 (ebi) */
	volatile uint32_t scfg4; /* Slave Configuration Register 4 (bridge) */
	volatile uint32_t pras0; /* PRAS0 (ram0) */
	volatile uint32_t pras1; /* PRAS1 (ram1) */
	volatile uint32_t pras2; /* PRAS2 (ram2) */
	volatile uint32_t pras3; /* PRAS3 (ebi) */
	volatile uint32_t pras4; /* PRAS4 (periph) */
	volatile uint32_t mrcr; /* Master Remp Control Register */
};

#endif /* __ASSEMBLER__ */

#endif /* __AT91_MATRIX_H__ */
