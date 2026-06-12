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
 * @file dm36x-timer.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */



#ifndef __DM36X_TIMER_H__
#define __DM36X_TIMER_H__

/* Timer Registers */
#define TMR_PID12 0x00
#define TMR_EMUMGT_CLKSPD 0x04
#define TMR_TIM12 0x10
#define TMR_TIM34 0x14
#define TMR_PRD12 0x18
#define TMR_PRD34 0x1c
#define TMR_TCR 0x20
#define TMR_TGCR 0x24
#define TMR_WDTCR 0x28
#define TMR_REL12 0x34
#define TMR_REL34 0x38
#define TMR_CAP12 0x3c
#define TMR_CAP34 0x40
#define TMR_INTCTL_STAT 0x44

#ifndef __ASSEMBLER__

#include <stdint.h>

struct dm36x_timer {
	uint32_t pid;
};

#endif /* __ASSEMBLER__ */


#endif /* __DM36X_TIMER_H__ */

