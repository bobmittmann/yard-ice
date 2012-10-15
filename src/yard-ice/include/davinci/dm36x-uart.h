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



#ifndef __DM36X_UART_H__
#define __DM36X_UART_H__

/* UART Registers */
#define UART_RBR 0x00
#define UART_IER 0x04
#define UART_IIR 0x08
#define UART_LCR 0x0c
#define UART_MCR 0x10
#define UART_LSR 0x14
#define UART_MSR 0x18
#define UART_DLL 0x20
#define UART_DLH 0x24
#define UART_PID1 0x28
#define UART_PID2 0x2c
#define UART_PWREMU_MGNT 0x30
#define UART_MDR 0x34

#define UART_THR UART_RBR
#define UART_FCR UART_IIR


#ifndef __ASSEMBLER__

#include <stdint.h>

struct dm36x_uart {
	uint32_t pid;
};

#endif /* __ASSEMBLER__ */


#endif /* __DM36X_UART_H__ */

