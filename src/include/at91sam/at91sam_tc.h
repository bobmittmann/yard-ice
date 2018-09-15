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

/* Timer Counter Channel Interface */

#define TC_CCR          ( 0) /* Channel Control Register */
#define TC_CMR          ( 4) /* Channel Mode Register (Capture Mode / Waveform Mode) */
#define TC_CV           (16) /* Counter Value */
#define TC_RA           (20) /* Register A */
#define TC_RB           (24) /* Register B */
#define TC_RC           (28) /* Register C */
#define TC_SR           (32) /* Status Register */
#define TC_IER          (36) /* Interrupt Enable Register */
#define TC_IDR          (40) /* Interrupt Disable Register */
#define TC_IMR          (44) /* Interrupt Mask Register */

/* TC_CCR - TC Channel Control Register */
#define TC_CLKEN            (0x1 <<  0) /* (TC) Counter Clock Enable Command */
#define TC_CLKDIS           (0x1 <<  1) /* (TC) Counter Clock Disable Command */
#define TC_SWTRG            (0x1 <<  2) /* (TC) Software Trigger Command */

/* TC_CMR - TC Channel Mode Register: Capture Mode / Waveform Mode */
#define TC_CLKS             (0x7 <<  0) /* (TC) Clock Selection */
#define TC_CLKS_TIMER_DIV1_CLOCK     (0x0) /* (TC) Clock selected: TIMER_DIV1_CLOCK */
#define TC_CLKS_TIMER_DIV2_CLOCK     (0x1) /* (TC) Clock selected: TIMER_DIV2_CLOCK */
#define TC_CLKS_TIMER_DIV3_CLOCK     (0x2) /* (TC) Clock selected: TIMER_DIV3_CLOCK */
#define TC_CLKS_TIMER_DIV4_CLOCK     (0x3) /* (TC) Clock selected: TIMER_DIV4_CLOCK */
#define TC_CLKS_TIMER_DIV5_CLOCK     (0x4) /* (TC) Clock selected: TIMER_DIV5_CLOCK */

#define TC_CLKS_MCK2        (0x0)
#define TC_CLKS_MCK8        (0x1)
#define TC_CLKS_MCK32       (0x2)
#define TC_CLKS_MCK128      (0x3)
#define TC_CLKS_MCK1024     (0x4)

#define TC_CLKS_XC0         (0x5) /* (TC) Clock selected: XC0 */
#define TC_CLKS_XC1         (0x6) /* (TC) Clock selected: XC1 */
#define TC_CLKS_XC2         (0x7) /* (TC) Clock selected: XC2 */

#define TC_CLKI             (0x1 <<  3) /* (TC) Clock Invert */

#define TC_BURST            (0x3 <<  4) /* (TC) Burst Signal Selection */
#define 	TC_BURST_NONE   (0x0 <<  4) /* (TC) The clock is not gated by an external signal */
#define 	TC_BURST_XC0    (0x1 <<  4) /* (TC) XC0 is ANDed with the selected clock */
#define 	TC_BURST_XC1    (0x2 <<  4) /* (TC) XC1 is ANDed with the selected clock */
#define 	TC_BURST_XC2    (0x3 <<  4) /* (TC) XC2 is ANDed with the selected clock */
#define TC_CPCSTOP          (0x1 <<  6) /* (TC) Counter Clock Stopped with RC Compare */
#define TC_LDBSTOP          (0x1 <<  6) /* (TC) Counter Clock Stopped with RB Loading */
#define TC_LDBDIS           (0x1 <<  7) /* (TC) Counter Clock Disabled with RB Loading */
#define TC_CPCDIS           (0x1 <<  7) /* (TC) Counter Clock Disable with RC Compare */
#define TC_ETRGEDG          (0x3 <<  8) /* (TC) External Trigger Edge Selection */
#define 	TC_ETRGEDG_NONE    (0x0 <<  8) /* (TC) Edge: None */
#define 	TC_ETRGEDG_RISING  (0x1 <<  8) /* (TC) Edge: rising edge */
#define 	TC_ETRGEDG_FALLING (0x2 <<  8) /* (TC) Edge: falling edge */
#define 	TC_ETRGEDG_BOTH    (0x3 <<  8) /* (TC) Edge: each edge */

#define TC_EEVTEDG          (0x3 <<  8) /* (TC) External Event Edge Selection */
#define 	TC_EEVTEDG_NONE    (0x0 <<  8) /* (TC) Edge: None */
#define 	TC_EEVTEDG_RISING  (0x1 <<  8) /* (TC) Edge: rising edge */
#define 	TC_EEVTEDG_FALLING (0x2 <<  8) /* (TC) Edge: falling edge */
#define 	TC_EEVTEDG_BOTH    (0x3 <<  8) /* (TC) Edge: each edge */

#define TC_ABETRG           (0x1 << 10) /* (TC) TIOA or TIOB External Trigger Selection */

/* (TC) External Event  Selection */
#define TC_EEVT             (0x3 << 10) 
#define 	TC_EEVT_TIOB    (0x0 << 10) /* (TC) Signal selected as external event: TIOB TIOB direction: input */
#define 	TC_EEVT_XC0     (0x1 << 10) /* (TC) Signal selected as external event: XC0 TIOB direction: output */
#define 	TC_EEVT_XC1     (0x2 << 10) /* (TC) Signal selected as external event: XC1 TIOB direction: output */
#define 	TC_EEVT_XC2     (0x3 << 10) /* (TC) Signal selected as external event: XC2 TIOB direction: output */
#define TC_ENETRG           (0x1 << 12) /* (TC) External Event Trigger enable */

/* (TC) Waveform  Selection */
#define TC_WAVESEL          (0x3 << 13) 
#define 	TC_WAVESEL_UP                   (0x0 << 13) /* (TC) UP mode without atomatic trigger on RC Compare */
#define 	TC_WAVESEL_UPDOWN               (0x1 << 13) /* (TC) UPDOWN mode without automatic trigger on RC Compare */
#define 	TC_WAVESEL_UP_AUTO              (0x2 << 13) /* (TC) UP mode with automatic trigger on RC Compare */
#define 	TC_WAVESEL_UPDOWN_AUTO          (0x3 << 13) /* (TC) UPDOWN mode with automatic trigger on RC Compare */
#define TC_CPCTRG           (0x1 << 14) /* (TC) RC Compare Trigger Enable */
#define TC_WAVE             (0x1 << 15) /* (TC) */

/* (TC) RA Loading Selection */
#define TC_LDRA             (0x3 << 16) 
#define 	TC_LDRA_NONE                 (0x0 << 16) /* (TC) Edge: None */
#define 	TC_LDRA_RISING               (0x1 << 16) /* (TC) Edge: rising edge of TIOA */
#define 	TC_LDRA_FALLING              (0x2 << 16) /* (TC) Edge: falling edge of TIOA */
#define 	TC_LDRA_BOTH                 (0x3 << 16) /* (TC) Edge: each edge of TIOA */

/* (TC) RA Compare Effect on TIOA */
#define TC_ACPA             (0x3 << 16) 
#define 	TC_ACPA_NONE                 (0x0 << 16) /* (TC) Effect: none */
#define 	TC_ACPA_SET                  (0x1 << 16) /* (TC) Effect: set */
#define 	TC_ACPA_CLEAR                (0x2 << 16) /* (TC) Effect: clear */
#define 	TC_ACPA_TOGGLE               (0x3 << 16) /* (TC) Effect: toggle */

/* (TC) RB Loading Selection */
#define TC_LDRB             (0x3 << 18) 
#define 	TC_LDRB_NONE                 (0x0 << 18) /* (TC) Edge: None */
#define 	TC_LDRB_RISING               (0x1 << 18) /* (TC) Edge: rising edge of TIOA */
#define 	TC_LDRB_FALLING              (0x2 << 18) /* (TC) Edge: falling edge of TIOA */
#define 	TC_LDRB_BOTH                 (0x3 << 18) /* (TC) Edge: each edge of TIOA */

/* (TC) RC Compare Effect on TIOA */
#define TC_ACPC             (0x3 << 18) 
#define 	TC_ACPC_NONE                 (0x0 << 18) /* (TC) Effect: none */
#define 	TC_ACPC_SET                  (0x1 << 18) /* (TC) Effect: set */
#define 	TC_ACPC_CLEAR                (0x2 << 18) /* (TC) Effect: clear */
#define 	TC_ACPC_TOGGLE               (0x3 << 18) /* (TC) Effect: toggle */

/* (TC) External Event Effect on TIOA */
#define TC_AEEVT            (0x3 << 20) 
#define 	TC_AEEVT_NONE                 (0x0 << 20) /* (TC) Effect: none */
#define 	TC_AEEVT_SET                  (0x1 << 20) /* (TC) Effect: set */
#define 	TC_AEEVT_CLEAR                (0x2 << 20) /* (TC) Effect: clear */
#define 	TC_AEEVT_TOGGLE               (0x3 << 20) /* (TC) Effect: toggle */

/* (TC) Software Trigger Effect on TIOA */
#define TC_ASWTRG           (0x3 << 22) 
#define 	TC_ASWTRG_NONE                 (0x0 << 22) /* (TC) Effect: none */
#define 	TC_ASWTRG_SET                  (0x1 << 22) /* (TC) Effect: set */
#define 	TC_ASWTRG_CLEAR                (0x2 << 22) /* (TC) Effect: clear */
#define 	TC_ASWTRG_TOGGLE               (0x3 << 22) /* (TC) Effect: toggle */

/* (TC) RB Compare Effect on TIOB */
#define TC_BCPB             (0x3 << 24) 
#define 	TC_BCPB_NONE                 (0x0 << 24) /* (TC) Effect: none */
#define 	TC_BCPB_SET                  (0x1 << 24) /* (TC) Effect: set */
#define 	TC_BCPB_CLEAR                (0x2 << 24) /* (TC) Effect: clear */
#define 	TC_BCPB_TOGGLE               (0x3 << 24) /* (TC) Effect: toggle */

/* (TC) RC Compare Effect on TIOB */
#define TC_BCPC             (0x3 << 26) 
#define 	TC_BCPC_NONE                 (0x0 << 26) /* (TC) Effect: none */
#define 	TC_BCPC_SET                  (0x1 << 26) /* (TC) Effect: set */
#define 	TC_BCPC_CLEAR                (0x2 << 26) /* (TC) Effect: clear */
#define 	TC_BCPC_TOGGLE               (0x3 << 26) /* (TC) Effect: toggle */

/* (TC) External Event Effect on TIOB */
#define TC_BEEVT            (0x3 << 28) 
#define 	TC_BEEVT_NONE                 (0x0 << 28) /* (TC) Effect: none */
#define 	TC_BEEVT_SET                  (0x1 << 28) /* (TC) Effect: set */
#define 	TC_BEEVT_CLEAR                (0x2 << 28) /* (TC) Effect: clear */
#define 	TC_BEEVT_TOGGLE               (0x3 << 28) /* (TC) Effect: toggle */

/* (TC) Software Trigger Effect on TIOB */
#define TC_BSWTRG           (0x3 << 30) 
#define 	TC_BSWTRG_NONE                 (0x0 << 30) /* (TC) Effect: none */
#define 	TC_BSWTRG_SET                  (0x1 << 30) /* (TC) Effect: set */
#define 	TC_BSWTRG_CLEAR                (0x2 << 30) /* (TC) Effect: clear */
#define 	TC_BSWTRG_TOGGLE               (0x3 << 30) /* (TC) Effect: toggle */

/* TC_SR - TC Channel Status Register */
#define TC_COVFS            (0x1 <<  0) /* (TC) Counter Overflow */
#define TC_LOVRS            (0x1 <<  1) /* (TC) Load Overrun */
#define TC_CPAS             (0x1 <<  2) /* (TC) RA Compare */
#define TC_CPBS             (0x1 <<  3) /* (TC) RB Compare */
#define TC_CPCS             (0x1 <<  4) /* (TC) RC Compare */
#define TC_LDRAS            (0x1 <<  5) /* (TC) RA Loading */
#define TC_LDRBS            (0x1 <<  6) /* (TC) RB Loading */
#define TC_ETRGS            (0x1 <<  7) /* (TC) External Trigger */
#define TC_CLKSTA           (0x1 << 16) /* (TC) Clock Enabling */
#define TC_MTIOA            (0x1 << 17) /* (TC) TIOA Mirror */
#define TC_MTIOB            (0x1 << 18) /* (TC) TIOA Mirror */

/* TC_IER - TC Channel Interrupt Enable Register */

/* TC_IDR - TC Channel Interrupt Disable Register */

/* TC_IMR - TC Channel Interrupt Mask Register */

/* Timer Counter Block */

#define TCB_TC0         ( 0) /* TC Channel 0 */
#define TCB_TC1         (64) /* TC Channel 1 */
#define TCB_TC2         (128) /* TC Channel 2 */
#define TCB_BCR         (192) /* TC Block Control Register */
#define TCB_BMR         (196) /* TC Block Mode Register */

/* TCB_BCR - TC Block Control Register */
#define TCB_SYNC            (0x1 <<  0) /* (TCB) Synchro Command */

/* TCB_BMR - TC Block Mode Register */
/* (TCB) External Clock Signal 0 Selection */
#define TCB_TC0XC0S         (0x3 <<  0)
#define TCB_TC0XC0S_TCLK0   (0x0) /* (TCB) TCLK0 connected to XC0 */
#define TCB_TC0XC0S_NONE    (0x1) /* (TCB) None signal connected to XC0 */
#define TCB_TC0XC0S_TIOA1   (0x2) /* (TCB) TIOA1 connected to XC0 */
#define TCB_TC0XC0S_TIOA2   (0x3) /* (TCB) TIOA2 connected to XC0 */

/* (TCB) External Clock Signal 1 Selection */
#define TCB_TC1XC1S (0x3 <<  2)

#define TCB_TC1XC1S_TCLK1 (0x0 <<  2) /* (TCB) TCLK1 connected to XC1 */
#define TCB_TC1XC1S_NONE  (0x1 <<  2) /* (TCB) None signal connected to XC1 */
#define TCB_TC1XC1S_TIOA0 (0x2 <<  2) /* (TCB) TIOA0 connected to XC1 */
#define TCB_TC1XC1S_TIOA2 (0x3 <<  2) /* (TCB) TIOA2 connected to XC1 */

/* (TCB) External Clock Signal 2 Selection */
#define TCB_TC2XC2S (0x3 <<  4)
#define TCB_TC2XC2S_TCLK2 (0x0 <<  4) /* (TCB) TCLK2 connected to XC2 */
#define TCB_TC2XC2S_NONE  (0x1 <<  4) /* (TCB) None signal connected to XC2 */
#define TCB_TC2XC2S_TIOA0 (0x2 <<  4) /* (TCB) TIOA0 connected to XC2 */
#define TCB_TC2XC2S_TIOA1 (0x3 <<  4) /* (TCB) TIOA2 connected to XC2 */

#ifndef __ASSEMBLER__

#include <stdint.h>

struct at91_tc {
	volatile uint32_t ccr; /* Channel Control Register */
	volatile uint32_t cmr; /* Channel Mode Register 
							  (Capture Mode / Waveform Mode) */
	uint32_t res0;
	uint32_t res1;
	volatile uint32_t cv; /* Counter Value */
	volatile uint32_t ra; /* Register A */
	volatile uint32_t rb; /* Register B */
	volatile uint32_t rc; /* Register C */
	volatile uint32_t sr; /* Status Register */
	volatile uint32_t ier; /* Interrupt Enable Register */
	volatile uint32_t idr; /* Interrupt Disable Register */
	volatile uint32_t imr; /* Interrupt Mask Register */
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
