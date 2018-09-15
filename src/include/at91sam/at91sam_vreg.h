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

#ifndef __AT91_VREG_H__
#define __AT91_VREG_H__

/* Voltage Regulator Mode Controller Interface */

#define VREG_MR   (0) /* (VREG) Voltage Regulator Mode Register */

/* Voltage Regulator Mode Register*/ 
#define VREG_PSTDBY     (0x1 <<  0) /* Voltage Regulator Power Standby Mode */

#ifndef __ASSEMBLER__

#include <stdint.h>

struct at91_vreg {
	volatile uint32_t Gmr; /* Voltage Regulator Mode Register */
};

#endif /* __ASSEMBLER__ */

#endif /* __AT91_VREG_H__ */
