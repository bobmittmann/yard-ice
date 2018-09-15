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


#ifndef __AT91_TC_H__
#define __AT91_TC_H__

/*****************************************************************************
 TC: Time Counter
 *****************************************************************************/

/* TC Block Control Register - Write Only */
#define TCB_BCR                      0x00c0
/* Synchro Command */
#define	TCB_SYNC                     1

/* TC Block Mode Register */
#define TCB_BMR                      0x00c4

/* External Clock Signal 0 Selection - Signal Connected to XC0 */
#define TCB_TC0XC0S_MASK                (~(0x03 << 0))
#define TCB_TC0XC0S_TCLK0                   (0x00 << 0)
#define TCB_TC0XC0S_NONE                    (0x01 << 0)
#define TCB_TC0XC0S_TIOA1                   (0x02 << 0)
#define TCB_TC0XC0S_TIOA2                   (0x03 << 0)
/* External Clock Signal 1 Selection - Signal Connected to XC1 */
#define TCB_TC1XC1S_MASK                (~(0x03 << 2))
#define TCB_TC1XC1S_TCLK1                   (0x00 << 2)
#define TCB_TC1XC1S_NONE                    (0x01 << 2)
#define TCB_TC1XC1S_TIOA0                   (0x02 << 2)
#define TCB_TC1XC1S_TIOA2                   (0x03 << 2)
/* ExternaBl Clock Signal 2 Selection - Signal Connected to XC2 */
#define TCB_TC2XC2S_MASK                (~(0x03 << 4))
#define TCB_TC2XC2S_TCLK2                   (0x00 << 4)
#define TCB_TC2XC2S_NONE                    (0x01 << 4)
#define TCB_TC2XC2S_TIOA0                   (0x02 << 4)
#define TCB_TC2XC2S_TIOA1                   (0x03 << 4)

/* TC Channel Control Register - Write Only */
#define TC_CCR                      0x00

/* Counter Clock Enable Command */
#define TC_CLKEN                    (1 << 0)
/* Counter Clock Disable Command */
#define TC_CLKDIS                   (1 << 1)
/* Software Trigger Command */
#define TC_SWTRG                    (1 << 2)

/* TC Channel Mode Register - Read/Write */
#define TC_CMR                      0x04

/* TC Channel Mode Register: Capture Mode */
/* Clock Selection */
#define TC_CLKS_MASK                (~0x07)
#define TC_CLKS_MCK2                0
#define TC_CLKS_MCK8                1
#define TC_CLKS_MCK32               2
#define TC_CLKS_MCK128              3
#define TC_CLKS_MCK1024             4
#define TC_CLKS_XC0                 5
#define TC_CLKS_XC1                 6
#define TC_CLKS_XC2                 7
/* Clock Invert */
#define TC_CLKI                     (1 << 3)
/* Burst Signal Selection */
#define TC_BURST_MASK               (~(3 << 4))
#define TC_BURST_NONE               (0 << 4)
#define TC_BURST_XC0                (1 << 4)
#define TC_BURST_XC1                (2 << 4)
#define TC_BURST_XC2                (3 << 4)
/* Counter Clock Stopped with RB Loading */
#define TC_LDBSTOP                  (1 << 6)
/* Counter Clock Disable with RB Loading */
#define TC_LDBDIS                   (1 << 7)
/* External Trigger Edge Selection */
#define TC_ETRGEDG_MASK             (~(3 << 8))
#define TC_ETRGEDG_EDGE_NONE        (0 << 8)
#define TC_ETRGEDG_RISING_EDGE      (1 << 8)
#define TC_ETRGEDG_FALLING_EDGE     (2 << 8)
#define TC_ETRGEDG_BOTH_EDGE        (3 << 8)
/* TIOA or TIOB External Trigger Selection */
#define TC_ABETRG_TIOB              (0 << 10)
#define TC_ABETRG_TIOA              (1 << 10)
/* RC Compare Trigger Enable */
#define TC_CPCTRG                   (1 << 14)
/* Waveform Mode Enable */
#define TC_WAVE                     (1 << 15)
/* RA Loading Selection */
#define TC_LDRA_MASK                (~(1 << 16))
#define TC_LDRA_EDGE_NONE           (0 << 16)
#define TC_LDRA_RISING_EDGE         (1 << 16)
#define TC_LDRA_FALLING_EDGE        (2 << 16)
#define TC_LDRA_BOTH_EDGE           (3 << 16)
/* RB Loading Selection */
#define TC_LDRB_MASK                (~(1 << 18))
#define TC_LDRB_EDGE_NONE           (0 << 18)
#define TC_LDRB_RISING_EDGE         (1 << 18)
#define TC_LDRB_FALLING_EDGE        (2 << 18)
#define TC_LDRB_BOTH_EDGE           (3 << 18)

/* Channel Mode Register: Waveform Mode */
/* Counter Clock Stopped with RC Compare */
#define TC_CPCSTOP                  (1 << 6)
/* Counter Clock Disable with RC Compare */
#define TC_CPCDIS                   (1 << 7)
/* External Event Edge Selection */
#define TC_EEVTEDG_MASK             (~(3 << 8))
#define TC_EEVTEDG_EDGE_NONE        (0 << 8)
#define TC_EEVTEDG_RISING_EDGE      (1 << 8)
#define TC_EEVTEDG_FALLING_EDGE     (2 << 8)
#define TC_EEVTEDG_BOTH_EDGE        (3 << 8)
/* External Event Selection */
#define TC_EEVT_MASK                (~(3 << 10))
#define TC_EEVT_TIOB                (0 << 10)
#define TC_EEVT_XC0                 (1 << 10)
#define TC_EEVT_XC1                 (2 << 10)
#define TC_EEVT_XC2                 (3 << 10)
/* External Event Trigger Enable */ 
#define TC_ENETRG                   (1 << 12)
/* RA Compare Effect on TIOA */
#define TC_ACPA_MASK                (~(3 << 16))
#define TC_ACPA_NONE         (0 << 16)
#define TC_ACPA_SET          (1 << 16)
#define TC_ACPA_CLEAR        (2 << 16)
#define TC_ACPA_TOGGLE       (3 << 16)
/* RC Compare Effect on TIOA */
#define TC_ACPC_MASK                (~(3 << 18))
#define TC_ACPC_NONE         (0 << 18)
#define TC_ACPC_SET          (1 << 18)
#define TC_ACPC_CLEAR        (2 << 18)
#define TC_ACPC_TOGGLE       (3 << 18)
/* External Event Effect on TIOA */
#define TC_AEEVT_MASK               (~(3 << 20))
#define TC_AEEVT_NONE        (0 << 20)
#define TC_AEEVT_SET         (1 << 20)
#define TC_AEEVT_CLEAR       (2 << 20)
#define TC_AEEVT_TOGGLE      (3 << 20)
/* Software Trigger Effect on TIOA */
#define TC_ASWTRG_MASK              (~(3 << 22))
#define TC_ASWTRG_NONE       (0 << 22)
#define TC_ASWTRG_SET        (1 << 22)
#define TC_ASWTRG_CLEAR      (2 << 22)
#define TC_ASWTRG_TOGGLE     (3 << 22)
/* RB Compare Effect on TIOB */
#define TC_BCPB_MASK                (~(3 << 24))
#define TC_BCPB_NONE         (0 << 24)
#define TC_BCPB_SET          (1 << 24)
#define TC_BCPB_CLEAR        (2 << 24)
#define TC_BCPB_TOGGLE       (3 << 24)
/* RC Compare Effect on TIOB */
#define TC_BCPC_MASK                (~(3 << 26))
#define TC_BCPC_NONE         (0 << 26)
#define TC_BCPC_SET          (1 << 26)
#define TC_BCPC_CLEAR        (2 << 26)
#define TC_BCPC_TOGGLE       (3 << 26)
/* External Event Effect on TIOB */
#define TC_BEEVT_MASK               (~(3 << 28))
#define TC_BEEVT_NONE        (0 << 28)
#define TC_BEEVT_SET         (1 << 28)
#define TC_BEEVT_CLEAR       (2 << 28)
#define TC_BEEVT_TOGGLE      (3 << 28)
/* Software Trigger Effect on TIOB */
#define TC_BSWTRG_MASK              (~(3 << 30))
#define TC_BSWTRG_NONE       (0 << 30)
#define TC_BSWTRG_SET        (1 << 30)
#define TC_BSWTRG_CLEAR      (2 << 30)
#define TC_BSWTRG_TOGGLE     (3 << 30)

/* Counter Value - Read/Write */
#define TC_CV                       0x10
 
/* Register A - Read/Write */
#define TC_RA                       0x14
 
/* Register B - Read/Write */
#define TC_RB                       0x18
 
/* Register C - Read/Write */
#define TC_RC                       0x1c
 
/* Status Register - Read Only */
#define TC_SR                       0x20

/* Clock Enabling */
#define TC_CLKSTA                   (1 << 16)
/* TIOA Mirror */
#define TC_MTIOA                    (1 << 17)
/* TIOB Mirror */
#define TC_MTIOB                    (1 << 18)

/* Interrupt Enable Register - Write Only */
#define TC_IER                      0x24

/* Interrupt Disable Register - Write Only */
#define TC_IDR                      0x28

/* Interrupt Mask Register - Read Only */
#define TC_IMR                      0x2c

/* Counter Overflow */
#define TC_COVFS                    (1 << 0)
/* Load Overrun */
#define TC_LOVRS                    (1 << 1)
/* RA Compare */
#define TC_CPAS                     (1 << 2)
/* RB Compare */
#define TC_CPBS                     (1 << 3)
/* RC Compare */
#define TC_CPCS                     (1 << 4)
/* RA Loading */
#define TC_LDRAS                    (1 << 5)
/* RB Loading */
#define TC_LDRBS                    (1 << 6)
/* External Trigger */
#define TC_ETRGS                    (1 << 7)

#ifndef __ASSEMBLER__
                      
#include <stdint.h>

struct at91_tc {
	volatile uint32_t ccr; /* Channel Control Register */
	volatile uint32_t cmr; /* Channel Mode Register 
							  (Capture Mode / Waveform Mode) */
	uint32_t res0;
	uint32_t res1;
	volatile uint32_t cv; /* Counter Value */
	uint32_t ra; /* Register A */
	uint32_t rb; /* Register B */
	uint32_t rc; /* Register C */
	volatile uint32_t sr; /* Status Register */
	uint32_t ier; /* Interrupt Enable Register */
	uint32_t idr; /* Interrupt Disable Register */
	uint32_t imr; /* Interrupt Mask Register */
	uint32_t unused[4];
};

struct at91_tcb {
	struct at91_tc tc[3];
	uint32_t bcr;
	uint32_t bmr;
	uint32_t res[4046];
};

#endif /* __ASSEMBLER__ */

#endif /* __AT91_TC_H__ */

