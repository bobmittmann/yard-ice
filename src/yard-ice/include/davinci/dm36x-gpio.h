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



#ifndef __DM36X_GPIO_H__
#define __DM36X_GPIO_H__

#define GPIO_PID 0x00
#define GPIO_RSVD1 0x04
#define GPIO_BINTEN 0x08
#define GPIO_RSVD2 0x0c
#define GPIO_DIR01 0x10
#define GPIO_OUTDATA01 0x14
#define GPIO_SETDATA01 0x18
#define GPIO_CLRDATA01 0x1c
#define GPIO_INDTATA01 0x20
#define GPIO_SETRIS01 0x24
#define GPIO_CLRRIS01 0x28
#define GPIO_SETFAL01 0x2c
#define GPIO_CLRFAL01 0x30
#define GPIO_INTSTAT01 0x34

#define GPIO_DIR23 0x38
#define GPIO_OUTDATA23 0x3c
#define GPIO_SETDATA23 0x40
#define GPIO_CLRDATA23 0x44
#define GPIO_INDTATA23 0x48
#define GPIO_SETRIS23 0x4c
#define GPIO_CLRRIS23 0x50
#define GPIO_SETFAL23 0x54
#define GPIO_CLRFAL23 0x58
#define GPIO_INTSTAT23 0x5c

#define GPIO_DIR45 0x60
#define GPIO_OUTDATA45 0x64
#define GPIO_SETDATA45 0x68
#define GPIO_CLRDATA45 0x6c
#define GPIO_INDTATA45 0x70
#define GPIO_SETRIS45 0x74
#define GPIO_CLRRIS45 0x78
#define GPIO_SETFAL45 0x7c
#define GPIO_CLRFAL45 0x80
#define GPIO_INTSTAT45 0x84


#ifndef __ASSEMBLER__

#include <stdint.h>

struct dm36x_gpio {
	volatile uint32_t id; /* PRTCIF peripheral ID register */
	volatile uint32_t ctrl; /* PRTCIF control register */
	volatile uint32_t ldata; /* PRTCIF access lower data register */
	volatile uint32_t udata; /* PRTCIF access upper data register */
	volatile uint32_t inten; /* PRTCIF interrupt enable register */
	volatile uint32_t intflg; /* PRTCIF interrupt flag register */  
};

#endif /* __ASSEMBLER__ */


#endif /* __DM36X_GPIO_H__ */

