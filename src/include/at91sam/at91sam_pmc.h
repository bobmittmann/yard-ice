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

#ifndef __AT91_PMC_H__
#define __AT91_PMC_H__

/* Power Management Controler */

#define PMC_SCER        ( 0) /* System Clock Enable Register */
#define PMC_SCDR        ( 4) /* System Clock Disable Register */
#define PMC_SCSR        ( 8) /* System Clock Status Register */

#define PMC_PCER        (16) /* Peripheral Clock Enable Register */
#define PMC_PCDR        (20) /* Peripheral Clock Disable Register */
#define PMC_PCSR        (24) /* Peripheral Clock Status Register */

#define PMC_MOR         (32) /* Main Oscillator Register */
#define PMC_MCFR        (36) /* Main Clock  Frequency Register */
#define PMC_PLLAR       (40) /* PLL A Register */
#define PMC_PLLBR       (44) /* PLL B Register */
#define PMC_PLLR        (44) /* PLL Register */
#define PMC_MCKR        (48) /* Master Clock Register */

#define PMC_PCKR        (64) /* Programmable Clock Register */

#define PMC_IER         (96) /* Interrupt Enable Register */
#define PMC_IDR         (100) /* Interrupt Disable Register */
#define PMC_SR          (104) /* Status Register */
#define PMC_IMR         (108) /* Interrupt Mask Register */

/* PMC_SCER - System Clock Enable Register */
/* PMC_SCDR - System Clock Disable Register */
/* PMC_SCSR - System Clock Status Register */
#define PMC_PCK             (0x1 <<  0) /* (PMC) Processor Clock */
#define PMC_UHP             (0x1 <<  6) /* (PMC) USB Host Port Clock */
#define PMC_UDP             (0x1 <<  7) /* (PMC) USB Device Port Clock */
#define PMC_PCK0            (0x1 <<  8) /* (PMC) Programmable Clock Output */
#define PMC_PCK1            (0x1 <<  9) /* (PMC) Programmable Clock Output */
#define PMC_HCK0            (0x1 << 16) /* (PMC) AHB UHP Clock Output */
#define PMC_HCK1            (0x1 << 17) /* (PMC) AHB LCDC Clock Output */


/* CKGR_MOR - Main Oscillator Register */

/* CKGR_MCFR - Main Clock Frequency Register */

/* CKGR_PLLAR - PLL A Register */

/* CKGR_PLLBR - PLL B Register */

/* PMC_MCKR - Master Clock Register */
#define PMC_CSS             (0x3 <<  0) /* (PMC) Programmable Clock Selection */
#define 	PMC_CSS_SLOW_CLK             (0x0) /* (PMC) Slow Clock is selected */
#define 	PMC_CSS_MAIN_CLK             (0x1) /* (PMC) Main Clock is selected */
#define 	PMC_CSS_PLLA_CLK             (0x2) /* (PMC) Clock from PLL A is selected */
#define 	PMC_CSS_PLLB_CLK             (0x3) /* (PMC) Clock from PLL B is selected */
#define 	PMC_CSS_PLL_CLK             (0x3) /* (PMC) Clock from PLL is selected */

#define PMC_PRES            (0x7 <<  2) /* (PMC) Programmable Clock Prescaler */
#define 	PMC_PRES_CLK    (0x0 <<  2) /* (PMC) Selected clock */
#define 	PMC_PRES_CLK_2  (0x1 <<  2) /* (PMC) Selected clock divided by 2 */
#define 	PMC_PRES_CLK_4  (0x2 <<  2) /* (PMC) Selected clock divided by 4 */
#define 	PMC_PRES_CLK_8  (0x3 <<  2) /* (PMC) Selected clock divided by 8 */
#define 	PMC_PRES_CLK_16 (0x4 <<  2) /* (PMC) Selected clock divided by 16 */
#define 	PMC_PRES_CLK_32 (0x5 <<  2) /* (PMC) Selected clock divided by 32 */
#define 	PMC_PRES_CLK_64 (0x6 <<  2) /* (PMC) Selected clock divided by 64 */
#define PMC_MDIV            (0x3 <<  8) /* (PMC) Master Clock Division */
#define 	PMC_MDIV_1      (0x0 <<  8) /* (PMC) The master clock and the processor clock are the same */
#define 	PMC_MDIV_2      (0x1 <<  8) /* (PMC) The processor clock is twice as fast as the master clock */
#define 	PMC_MDIV_3      (0x2 <<  8) /* (PMC) The processor clock is four times faster than the master clock */

/* PMC_PCKR - Programmable Clock Register */

/* PMC_IER - PMC Interrupt Enable Register */
/* PMC_IDR - PMC Interrupt Disable Register */
/* PMC_SR - PMC Status Register */
/* PMC_IMR - PMC Interrupt Mask Register */
#define PMC_MOSCS           (0x1 <<  0) /* (PMC) MOSC Status/Enable/Disable/Mask */
#define PMC_LOCKA           (0x1 <<  1) /* (PMC) PLL A Status/Enable/Disable/Mask */
#define PMC_LOCKB           (0x1 <<  2) /* (PMC) PLL B Status/Enable/Disable/Mask */
#define PMC_LOCK           (0x1 <<  2) /* (PMC) PLL Status/Enable/Disable/Mask */
#define PMC_MCKRDY          (0x1 <<  3) /* (PMC) Master Clock Status/Enable/Disable/Mask */
#define PMC_PCK0RDY         (0x1 <<  8) /* (PMC) PCK0_RDY Status/Enable/Disable/Mask */
#define PMC_PCK1RDY         (0x1 <<  9) /* (PMC) PCK1_RDY Status/Enable/Disable/Mask */

#ifndef __ASSEMBLER__

#include <stdint.h>

struct at91_pmc {
	volatile uint32_t scer; /* System Clock Enable Register */
	volatile uint32_t scdr; /* System Clock Disable Register */
	volatile uint32_t scsr; /* System Clock Status Register */
	uint32_t res0;
	volatile uint32_t pcer; /* Peripheral Clock Enable Register */
	volatile uint32_t pcdr; /* Peripheral Clock Disable Register */
	volatile uint32_t pcsr; /* Peripheral Clock Status Register */
	uint32_t res1;
	volatile uint32_t mor; /* Main Oscillator Register */
	volatile uint32_t mcfr; /* Main Clock  Frequency Register */
	volatile uint32_t pllar; /* PLL A Register */
	volatile uint32_t pllbr; /* PLL B Register */
	volatile uint32_t mckr; /* Master Clock Register */
	uint32_t res2[2];
	volatile uint32_t pck0; /* Programmable Clock Register */
	volatile uint32_t pck1; /* Programmable Clock Register */
	volatile uint32_t pck2; /* Programmable Clock Register */
	volatile uint32_t pck3; /* Programmable Clock Register */
	volatile uint32_t pck4; /* Programmable Clock Register */
	volatile uint32_t pck5; /* Programmable Clock Register */
	volatile uint32_t pck6; /* Programmable Clock Register */
	volatile uint32_t pck7; /* Programmable Clock Register */
	volatile uint32_t ier; /* Interrupt Enable Register */
	volatile uint32_t idr; /* Interrupt Disable Register */
	volatile uint32_t sr; /* Status Register */
	volatile uint32_t imr; /* Interrupt Mask Register */
};

#endif /* __ASSEMBLER__ */

#endif /* __AT91_PMC_H__ */

