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

#ifndef __AT91_PWMC_H__
#define __AT91_PWMC_H__

/* Register definition for PWMC peripheral */
#define PWMC_CH0_CMR (0x00) /* (PWMC_CH0) Channel Mode Register */
#define PWMC_CH0_CDTYR (0x04) /* (PWMC_CH0) Channel Duty Cycle Register */
#define PWMC_CH0_CPRDR (0x08) /* (PWMC_CH0) Channel Period Register */
#define PWMC_CH0_CCNTR (0x0C) /* (PWMC_CH0) Channel Counter Register */
#define PWMC_CH0_CUPDR (0x10) /* (PWMC_CH0) Channel Update Register */
#define PWMC_CH0_Reserved (0x14) /* (PWMC_CH0) Reserved */

/* Register definition for PWMC peripheral */
#define PWMC_MR   (0x00) /* (PWMC) PWMC Mode Register */
#define PWMC_ENA  (0x04) /* (PWMC) PWMC Enable Register */
#define PWMC_DIS  (0x08) /* (PWMC) PWMC Disable Register */
#define PWMC_SR   (0x0C) /* (PWMC) PWMC Status Register */
#define PWMC_IER  (0x10) /* (PWMC) PWMC Interrupt Enable Register */
#define PWMC_IDR  (0x14) /* (PWMC) PWMC Interrupt Disable Register */
#define PWMC_IMR  (0x18) /* (PWMC) PWMC Interrupt Mask Register */
#define PWMC_ISR  (0x1C) /* (PWMC) PWMC Interrupt Status Register */
#define PWMC_VR   (0xFC) /* (PWMC) PWMC Version Register */

/* PWMC Channel Interface */

/* PWMC Channel Mode Register*/ 
#define PWMC_CPRE(X) (((X) & 0xf) <<  0) /* Channel Pre-scaler : PWMC_CLKx */
#define 	PWMC_CPRE_MCK(X)  (X) /*  */
#define 	PWMC_CPRE_MCKA    (0xb) /*  */
#define 	PWMC_CPRE_MCKB    (0xc) /*  */
#define PWMC_CALG       (0x1 <<  8) /* Channel Alignment */
#define PWMC_CPOL       (0x1 <<  9) /* Channel Polarity */
#define PWMC_CPD        (0x1 << 10) /* Channel Update Period */
/* PWMC Channel Duty Cycle Register*/ 
#define PWMC_CDTY       (0x0 <<  0) /* Channel Duty Cycle */
/* PWMC Channel Period Register*/ 
#define PWMC_CPRD       (0x0 <<  0) /* Channel Period */
/* PWMC Channel Counter Register*/ 
#define PWMC_CCNT       (0x0 <<  0) /* Channel Counter */
/* PWMC Channel Update Register*/ 
#define PWMC_CUPD       (0x0 <<  0) /* Channel Update */

/* Pulse Width Modulation Controller Interface */
#define PWMC_MCK        0
#define PWMC_MCK_2      1
#define PWMC_MCK_4      2
#define PWMC_MCK_8      3
#define PWMC_MCK_16     4
#define PWMC_MCK_32     5
#define PWMC_MCK_64     6
#define PWMC_MCK_128    7
#define PWMC_MCK_256    8
#define PWMC_MCK_512    9
#define PWMC_MCK_1024  10

/* PWMC Mode Register*/ 
#define PWMC_DIVA(X) (((X) & 0xff) <<  0) /* CLKA divide factor. */
#define PWMC_PREA(X) (((X) & 0xf) <<  8) /* Divider Input Clock Prescaler A */
#define PWMC_PREA_MCK (0x0 <<  8) /*  */

#define PWMC_DIVB(X) (((X) & 0xff) << 16) /* CLKB divide factor. */
#define PWMC_PREB(X) (((X) & 0xf) << 24) /* Divider Input Clock Prescaler B */
#define PWMC_PREB_MCK (0x0 << 24) /*  */

/* PWMC Enable Register*/ 
#define PWMC_CHID(ID)   (0x1 << (ID)) /* Channel ID */
#define PWMC_CHID0      (0x1 <<  0) /* Channel ID 0 */
#define PWMC_CHID1      (0x1 <<  1) /* Channel ID 1 */
#define PWMC_CHID2      (0x1 <<  2) /* Channel ID 2 */
#define PWMC_CHID3      (0x1 <<  3) /* Channel ID 3 */

/* PWMC Disable Register*/ 
/* PWMC Status Register*/ 
/* PWMC Interrupt Enable Register*/ 
/* PWMC Interrupt Disable Register*/ 
/* PWMC Interrupt Mask Register*/ 
/* PWMC Interrupt Status Register*/ 

#ifndef __ASSEMBLER__

#include <stdint.h>

/* PWMC Channel Interface */

struct at91_pwmc_ch {
	volatile uint32_t cmr; /* channel mode register */
	volatile uint32_t cdty; /* channel duty cycle register */
	volatile uint32_t cprd; /* channel period register */
	volatile uint32_t ccnt; /* channel counter register */
	volatile uint32_t cupd; /* channel update register */
	volatile uint32_t reserved[3]; /* reserved */
};

/* pulse width modulation controller interface */

struct at91_pwmc {
	volatile uint32_t mr; /* pwmc mode register */
	volatile uint32_t ena; /* pwmc enable register */
	volatile uint32_t dis; /* pwmc disable register */
	volatile uint32_t sr; /* pwmc status register */
	volatile uint32_t ier; /* pwmc interrupt enable register */
	volatile uint32_t idr; /* pwmc interrupt disable register */
	volatile uint32_t imr; /* pwmc interrupt mask register */
	volatile uint32_t isr; /* pwmc interrupt status register */
	volatile uint32_t reserved0[55]; /*  */
	volatile uint32_t vr; /* pwmc version register */
	volatile uint32_t reserved1[64]; /*  */
	struct at91_pwmc_ch  ch[4]; /* pwmc channel */
};

#endif /* __ASSEMBLER__ */

#endif /* __AT91_PWMC_H__ */

