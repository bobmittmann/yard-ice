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
 * @file dm36x-pll.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */



#ifndef __DM36X_PLL_H__
#define __DM36X_PLL_H__

#define PLL_PID 0x00
#define PLL_RSTYPE 0xe4
#define PLL_PLLCTL 0x100
#define PLL_SECCTL 0x108
#define PLL_RV 0x10c
#define PLL_PLLM 0x110
#define PLL_PREDIV 0x114
#define PLL_PLLDIV1 0x118
#define PLL_PLLDIV2 0x11c
#define PLL_PLLDIV3 0x120
#define PLL_OSCDIV1 0x124
#define PLL_POSTDIV 0x128
#define PLL_BPDIV 0x12c
#define PLL_PLLCMD 0x138
#define PLL_PLLSTAT 0x13c
#define PLL_ALNCTL 0x140
#define PLL_DCHANGE 0x144
#define PLL_CKEN 0x148
#define PLL_CKSTAT 0x14c
#define PLL_SYSTAT 0x150
#define PLL_PLLDIV4 0x160
#define PLL_PLLDIV5 0x164
#define PLL_PLLDIV6 0x168
#define PLL_PLLDIV7 0x16c
#define PLL_PLLDIV8 0x170
#define PLL_PLLDIV9 0x174

/*****************************************************************************
 * Peripheral ID Register (PID) Register
 ****************************************************************************/
#define PID_TYPE_MASK  (0xff << 16)
#define PID_CLASS_MASK (0xff << 8)
#define PID_REV_MASK   (0xff << 0)

#define DM36X_PLL_PID 0x0001080e

/*****************************************************************************
 * Reset Type Status (RSTYPE) Register
 ****************************************************************************/
#define RSTYPE_SRST (1 << 3) /* System Reset */
#define RSTYPE_MRST (1 << 2) /* Maximum Reset */
#define RSTYPE_XWRST (1 << 1) /* External Warm Reset */
#define RSTYPE_POR (1 << 0) /* Power On Reset */

/*****************************************************************************
 * PLL Control (PLLCTL) Register
 ****************************************************************************/

/* PLL enable source. This bit must be cleared 
   to 0 before PLLEN will have any effect.
   0 PLL enable is controlled by internal test hardware
   1 PLL enable is controlled by the register bit PLLEN */
#define PLLCTL_PLLENSRC (1 << 5) 

/* PLL reset 
   0 PLL reset de-assert
   1 PLL reset assert */
#define PLLCTL_PLLRST (1 << 3)

/* PLL power-down
   0 PLL operating, not powered down
   1 PLL power-down */
#define PLLCTL_PLLPWRDN (1 << 1)

/* PLL Mode Enable. Bit PLLENSRC must be cleared to 0 before 
   PLLEN will have any effect.
   0 Bypass mode
   1 PLL mode, not bypassed */
#define PLLCTL_PLLEN (1 << 0)

/*****************************************************************************
 * OBSCLK Select (OCSEL) Register 
 ****************************************************************************/

/* OBSCLK source.
   0 Oscillator divider output enabled
   1 Oscillator divider output disabled */
#define OCSEL_OCSRC  (1 << 4)

/*****************************************************************************
 * PLL Secondary Control (PLLSECCTL) Register
 ****************************************************************************/

/* Force recalibration on code limits (active high) */
#define PLLSECCTL_LIMITRECALEN (1 << 23)

/* Stop /Limp select (active high/low) */
#define PLLSECCTL_STOPMODE (1 << 22)

/* Low current stand by select(active high) */
#define PLLSECCTL_LOWCURRSTDBY (1 << 21)

/* Low input frequency control (active high) */
#define PLLSECCTL_SLOWCLKLOCK (1 << 20)

/* Temperature, Drift recalibration enable (active high) */
#define PLLSECCTL_DRIFTGUARDEN (1 << 19)

/* Core Register M2/N2 load enable (low-high) */
#define PLLSECCTL_TENABLEDIV (1 << 18)

/* Core Register M/N load enable (low - high) */
#define PLLSECCTL_TENABLE (1 << 17)

/* Core soft reset lock sequence initialization ( high-low-high) */
#define PLLSECCTL_TINITZ (1 << 16)

/*****************************************************************************
 * PLL Multiplier Control (PLLM) Register
 ****************************************************************************/

/* PLL Multiplier. Multiplier value = 2xPLLM */
#define PLLM_MASK (0x1ff << 0)
#define PLLM_VAL(VAL) (((VAL) / 2) & PLLM_MASK)

/*****************************************************************************
 * PLL Pre-Divider Control (PREDIV) Register
 ****************************************************************************/

/* Divider ratio for pre divider. Ratio value = RATIO + 1 */
#define PREDIV_RATIO (0x1f << 0)
#define PREDIV_MASK (0x1f << 0)
#define PREDIV_VAL(VAL) (((VAL) - 1) & PREDIV_MASK)

/*****************************************************************************
 * PLL Controller Divider 1 (PLLDIV1) Register
 * PLL Controller Divider 2 (PLLDIV2) Register
 * PLL Controller Divider 3 (PLLDIV3) Register
 * PLL Controller Divider 4 (PLLDIV4) Register
 * PLL Controller Divider 5 (PLLDIV5) Register
 * PLL Controller Divider 6 (PLLDIV6) Register
 * PLL Controller Divider 7 (PLLDIV7) Register
 * PLL Controller Divider 8 (PLLDIV8) Register
 * PLL Controller Divider 9 (PLLDIV9) Register
 * Oscillator Divider 1 (OSCDIV1) Register for OBSCLK
 * PLL Post-Divider Control (POSTDIV) Register
 * Bypass Divider (BPDIV) Register
 ****************************************************************************/

/*  Divider enable .... */
#define PLLDIV_EN (1 << 15) 
#define PLLDIV_RATIO (0x1f << 0)
#define PLLDIV_MASK (0x1f << 0)
#define PLLDIV_VAL(VAL) (((VAL) - 1) & PLLDIV_MASK)

/*****************************************************************************
 * PLL Controller Command (PLLCMD) Register
 ****************************************************************************/

/* GO operation command for SYSCLKn ratio change and/or phase alignment. 
   Before setting this bit to 1 to initiate a GO operation, check the 
   GOSTAT bit in the PLLSTAT register to ensure all 
   previous GO operations have completed. 
   Write of 1 initiates GO operation. Once set, GOSET remains set but further
   writes of 1 can initiate the GO operation. */
#define PLLCMD_GOSET (1 << 0)

/*****************************************************************************
 * PLL Controller Status (PLLSTAT) Register
 ****************************************************************************/

/* OSCIN Stable */
#define PLLSTAT_STABLE (1 << 2)

/* PLL Core STATUS */
#define PLLSTAT_LOCK (1 << 1)

/* GO status
 GO operation is in progress. 
 SYSCLK divider ratios and/or phase alignment are changing. */
#define PLLSTAT_GOSTAT (1 << 0)

/*****************************************************************************
 * PLLC Clock Align Control (ALNCTL) Register
 ****************************************************************************/

/*****************************************************************************
 * PLLDIV Ratio Change Status (DCHANGE) Register
 ****************************************************************************/

/*****************************************************************************
 * Clock Enable Control (CKEN) Register
 ****************************************************************************/

/*****************************************************************************
 * Clock Status (CKSTAT) Register
 ****************************************************************************/

/*****************************************************************************
 * SYSCLK Status (SYSTAT) Register
 ****************************************************************************/


#ifndef __ASSEMBLER__

#include <stdint.h>

struct dm36x_pll {
	uint32_t pid;
	uint32_t res0[56];
	volatile uint32_t rstype;
	uint32_t res1[6];
	volatile uint32_t pllctl;
	uint32_t res2;
	volatile uint32_t secctl;
	volatile uint32_t rv;
	volatile uint32_t pllm;
	volatile uint32_t prediv;
	volatile uint32_t plldiv1;
	volatile uint32_t plldiv2;
	volatile uint32_t plldiv3;
	volatile uint32_t oscdiv1;
	volatile uint32_t postdiv;
	volatile uint32_t bpdiv;
	uint32_t res3[2];
	volatile uint32_t pllcmd;
	volatile uint32_t pllstat;
	volatile uint32_t alnctl;
	volatile uint32_t dchange;
	volatile uint32_t cken;
	volatile uint32_t ckstat;
	volatile uint32_t systat;
	uint32_t res4[3];
	volatile uint32_t plldiv4;
	volatile uint32_t plldiv5;
	volatile uint32_t plldiv6;
	volatile uint32_t plldiv7;
	volatile uint32_t plldiv8;
	volatile uint32_t plldiv9;
};

#endif /* __ASSEMBLER__ */


#endif /* __DM36X_PLL_H__ */

