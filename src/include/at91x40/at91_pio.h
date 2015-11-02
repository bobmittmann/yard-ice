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

/*****************************************************************************
 PIO: Parallel I/O Controller 
 *****************************************************************************/
/* PIO Enable Register */
#define PIO_PER                 0x00 
/* PIO Disable Register */
#define PIO_PDR                 0x04 
/* PIO Status Register */
#define PIO_PSR                 0x08 
/* Output Enable Register */
#define PIO_OER                 0x10 
/* Output Disable Register */
#define PIO_ODR                 0x14 
/* Output Status Register */
#define PIO_OSR                 0x18 
/* Input Filter Enable Register */
#define PIO_IFER                0x20 
/* Input Filter Disable Register */
#define PIO_IFDR                0x24 
/* Input Filter Status Register */
#define PIO_IFSR                0x28 
/* Set Output Data Register */
#define PIO_SODR                0x30 
/* Clear Output Data Register */
#define PIO_CODR                0x34 
/* Output Data Status Register */
#define PIO_ODSR                0x38 
/* Pin Data Status Register */
#define PIO_PDSR                0x3c 
/* Interrupt Enable Register */
#define PIO_IER                 0x40 
/* Interrupt Disable Register */
#define PIO_IDR                 0x44
/* Interrupt Mask Register */
#define PIO_IMR                 0x48
/* Interrupt Status Register */
#define PIO_ISR                 0x4c

#ifndef __ASSEMBLER__

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
	uint32_t res[4076];
};

#endif /* __ASSEMBLER__ */

#endif /* __AT91_PIO_H__ */

