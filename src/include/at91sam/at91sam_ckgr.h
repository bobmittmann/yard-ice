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

#ifndef __AT91C_CKGR_H__
#define __AT91C_CKGR_H__

/* Clock Generator Controler */

#define CKGR_MOR        ( 0) /* Main Oscillator Register */
#define CKGR_MCFR       ( 4) /* Main Clock  Frequency Register */
#define CKGR_PLLAR      ( 8) /* PLL A Register */
#define CKGR_PLLBR      (12) /* PLL B Register */

#define CKGR_PLLR      (12) /* Single PLL Systems PLL Register */

/* CKGR_MOR - Main Oscillator Register */
#define CKGR_MOSCEN         (0x1 <<  0) /* (CKGR) Main Oscillator Enable */
#define CKGR_OSCBYPASS      (0x1 <<  1) /* (CKGR) Main Oscillator Bypass */
#define CKGR_OSCOUNT(VAL)   (((VAL) & 0xff) <<  8) /* (CKGR) Main Oscillator Start-up Time */

/* CKGR_MCFR - Main Clock Frequency Register */
#define CKGR_MAINF          (0xffff <<  0) /* (CKGR) Main Clock Frequency */
#define CKGR_MAINRDY        (0x1 << 16) /* (CKGR) Main Clock Ready */

/* CKGR_PLLAR - PLL A Register */
#define CKGR_DIVA(VAL)           (((VAL) & 0xff) <<  0) /* (CKGR) Divider A Selected */
#define 	CKGR_DIVA_0                    (0x0) /* (CKGR) Divider A output is 0 */
#define 	CKGR_DIVA_BYPASS               (0x1) /* (CKGR) Divider A is bypassed */
#define CKGR_PLLACOUNT(VAL)      ((VAL & 0x3f) <<  8) /* (CKGR) PLL A Counter */

#define CKGR_OUTA         (0x3 << 14) /* (CKGR) PLL A Output Frequency Range */
#define 	CKGR_OUTA_0   (0x0 << 14) /* (CKGR) Please refer to the PLLA datasheet */
#define 	CKGR_OUTA_1   (0x1 << 14) /* (CKGR) Please refer to the PLLA datasheet */
#define 	CKGR_OUTA_2   (0x2 << 14) /* (CKGR) Please refer to the PLLA datasheet */
#define 	CKGR_OUTA_3   (0x3 << 14) /* (CKGR) Please refer to the PLLA datasheet */
#define CKGR_MULA(VAL)    (((VAL - 1) & 0x7ff) << 16) /* (CKGR) PLL A Multiplier */
#define CKGR_SRCA         (0x1 << 29) /* (CKGR) */


/* CKGR_PLLBR - PLL B Register */
#define CKGR_DIVB           (0xff <<  0) /* (CKGR) Divider B Selected */
#define 	CKGR_DIVB_0                    (0x0) /* (CKGR) Divider B output is 0 */
#define 	CKGR_DIVB_BYPASS               (0x1) /* (CKGR) Divider B is bypassed */
#define CKGR_PLLBCOUNT      (0x3f <<  8) /* (CKGR) PLL B Counter */
#define CKGR_OUTB           (0x3 << 14) /* (CKGR) PLL B Output Frequency Range */
#define 	CKGR_OUTB_0                    (0x0 << 14) /* (CKGR) Please refer to the PLLB datasheet */
#define 	CKGR_OUTB_1                    (0x1 << 14) /* (CKGR) Please refer to the PLLB datasheet */
#define 	CKGR_OUTB_2                    (0x2 << 14) /* (CKGR) Please refer to the PLLB datasheet */
#define 	CKGR_OUTB_3                    (0x3 << 14) /* (CKGR) Please refer to the PLLB datasheet */
#define CKGR_MULB(VAL)    (((VAL - 1) & 0x7ff) << 16) /* (CKGR) PLL A Multiplier */
/* (CKGR) Divider for USB Clocks */
#define CKGR_USBDIV(VAL) ((((VAL) / 2) & 0x3) << 28) 
#define 	CKGR_USBDIV_0                    (0x0 << 28) /* (CKGR) Divider output is PLL clock output */
#define 	CKGR_USBDIV_1                    (0x1 << 28) /* (CKGR) Divider output is PLL clock output divided by 2 */
#define 	CKGR_USBDIV_2                    (0x2 << 28) /* (CKGR) Divider output is PLL clock output divided by 4 */

/* Aliases */
#define CKGR_MUL(VAL) CKGR_MULA(VAL) /* (CKGR) PLL Multiplier */
#define CKGR_PLLCOUNT(VAL) CKGR_PLLACOUNT(VAL) /* (CKGR) PLL Counter */
#define CKGR_DIV(VAL) CKGR_DIVA(VAL) /* (CKGR) Divider Selected */

#define CKGR_PLLMUL(VAL) CKGR_MUL(VAL)
#define CKGR_PLLDIV(VAL) CKGR_DIV(VAL)

#ifndef __ASSEMBLER__

#include <stdint.h>

struct at91_ckgr {
	volatile uint32_t mor; /* Main Oscillator Register */
	volatile uint32_t mcfr; /* Main Clock  Frequency Register */
	volatile uint32_t pllar; /* PLL A Register */
	volatile uint32_t ckgr_pllbr; /* PLL B Register */
};

#endif /* __ASSEMBLER__ */


#endif /* __AT91C_CKGR_H__ */
