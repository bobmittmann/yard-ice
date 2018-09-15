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


#ifndef __AT91_PS_H__
#define __AT91_PS_H__

/*****************************************************************************
 PS: Power Saving
 *****************************************************************************/
/* Power Saving Contol Register */
#define PS_CR                  0x00
#define PS_CPU                 0x00000001
/* Peripheral Clock Enable Register */
#define PS_PCER                0x04 
/* Peripheral Clock Disable Register */
#define PS_PCDR                0x08

/* Peripheral Clock Status Register */
#define PS_PCSR                0x0c

#ifndef __ASSEMBLER__

struct at91_ps {
	uint32_t cr;
	volatile uint32_t pcer;
	volatile uint32_t pcdr;
	volatile uint32_t pcsr;
	uint32_t res[4092];
};

#endif /* __ASSEMBLER__ */

#endif /* __AT91_PS_H__ */

