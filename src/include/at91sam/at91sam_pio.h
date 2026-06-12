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

#ifndef __AT91_PIO_H__
#define __AT91_PIO_H__

/* Parallel Input Output Controler */

#define PIO_PER         ( 0) /* PIO Enable Register */
#define PIO_PDR         ( 4) /* PIO Disable Register */
#define PIO_PSR         ( 8) /* PIO Status Register */

#define PIO_OER         (16) /* Output Enable Register */
#define PIO_ODR         (20) /* Output Disable Registerr */
#define PIO_OSR         (24) /* Output Status Register */

#define PIO_IFER        (32) /* Input Filter Enable Register */
#define PIO_IFDR        (36) /* Input Filter Disable Register */
#define PIO_IFSR        (40) /* Input Filter Status Register */

#define PIO_SODR        (48) /* Set Output Data Register */
#define PIO_CODR        (52) /* Clear Output Data Register */
#define PIO_ODSR        (56) /* Output Data Status Register */
#define PIO_PDSR        (60) /* Pin Data Status Register */
#define PIO_IER         (64) /* Interrupt Enable Register */
#define PIO_IDR         (68) /* Interrupt Disable Register */
#define PIO_IMR         (72) /* Interrupt Mask Register */
#define PIO_ISR         (76) /* Interrupt Status Register */
#define PIO_MDER        (80) /* Multi-driver Enable Register */
#define PIO_MDDR        (84) /* Multi-driver Disable Register */
#define PIO_MDSR        (88) /* Multi-driver Status Register */

#define PIO_PUDR       (96) /* Pull-up Disable Register */
#define PIO_PUER       (100) /* Pull-up Enable Register */
#define PIO_PUSR       (104) /* Pull-up Status Register */

#define PIO_ASR         (112) /* Select A Register */
#define PIO_BSR         (116) /* Select B Register */
#define PIO_ABSR        (120) /* AB Select Status Register */

#define PIO_OWER        (160) /* Output Write Enable Register */
#define PIO_OWDR        (164) /* Output Write Disable Register */
#define PIO_OWSR        (168) /* Output Write Status Register */

#ifndef __ASSEMBLER__

#include <stdint.h>

struct at91_pio {
	volatile uint32_t per; /* PIO Enable Register */
	volatile uint32_t pdr; /* PIO Disable Register */
	volatile uint32_t psr; /* PIO Status Register */
	uint32_t res0;
	volatile uint32_t oer; /* Output Enable Register */
	volatile uint32_t odr; /* Output Disable Registerr */
	volatile uint32_t osr; /* Output Status Register */
	uint32_t res1;
	volatile uint32_t ifer; /* Input Filter Enable Register */
	volatile uint32_t ifdr; /* Input Filter Disable Register */
	volatile uint32_t ifsr; /* Input Filter Status Register */
	uint32_t res2;
	volatile uint32_t sodr; /* Set Output Data Register */
	volatile uint32_t codr; /* Clear Output Data Register */
	volatile uint32_t odsr; /* Output Data Status Register */
	volatile uint32_t pdsr; /* Pin Data Status Register */
	volatile uint32_t ier; /* Interrupt Enable Register */
	volatile uint32_t idr; /* Interrupt Disable Register */
	volatile uint32_t imr; /* Interrupt Mask Register */
	volatile uint32_t isr; /* Interrupt Status Register */
	volatile uint32_t mder; /* Multi-driver Enable Register */
	volatile uint32_t mddr; /* Multi-driver Disable Register */
	volatile uint32_t mdsr; /* Multi-driver Status Register */
	uint32_t res3;
	volatile uint32_t pudr; /* Pull-up Disable Register */
	volatile uint32_t puer; /* Pull-up Enable Register */
	volatile uint32_t pusr; /* Pull-up Status Register */
	uint32_t res4;
	volatile uint32_t asr; /* Select A Register */
	volatile uint32_t bsr; /* Select B Register */
	volatile uint32_t absr; /* AB Select Status Register */
	uint32_t res5[36];
	volatile uint32_t ower; /* Output Write Enable Register */
	volatile uint32_t owdr; /* Output Write Disable Register */
	volatile uint32_t owsr; /* Output Write Status Register */
	uint32_t res6[58];
};

#endif /* __ASSEMBLER__ */

#endif /* __AT91_PIO_H__ */

