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
 * @file .h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */



#ifndef __DM36X_AINTC_H__
#define __DM36X_AINTC_H__

/* ARM Interrupt Controller register structure - 
   See sprufb3.pdf, Chapter 8 for more details. */
#define AINTC_FIQ0 0x00
#define AINTC_FIQ1 0x04
#define AINTC_IRQ0 0x08
#define AINTC_IRQ1 0x0c
#define AINTC_FIQENTRY 0x10
#define AINTC_IRQENTRY 0x14
#define AINTC_EINT0 0x18
#define AINTC_EINT1 0x1c
#define AINTC_INTCTL 0x20
#define AINTC_EABASE 0x24
#define AINTC_INTPRI0 0x30
#define AINTC_INTPRI1 0x34
#define AINTC_INTPRI2 0x38
#define AINTC_INTPRI3 0x3c
#define AINTC_INTPRI4 0x40
#define AINTC_INTPRI5 0x44
#define AINTC_INTPRI6 0x48
#define AINTC_INTPRI7 0x4c

#ifndef __ASSEMBLER__

#include <stdint.h>

struct dm36x_aintc {
	volatile uint32_t id; /* PRTCIF peripheral ID register */
	volatile uint32_t ctrl; /* PRTCIF control register */
	volatile uint32_t ldata; /* PRTCIF access lower data register */
	volatile uint32_t udata; /* PRTCIF access upper data register */
	volatile uint32_t inten; /* PRTCIF interrupt enable register */
	volatile uint32_t intflg; /* PRTCIF interrupt flag register */  
};

#endif /* __ASSEMBLER__ */


#endif /* __DM36X_AINTC_H__ */

