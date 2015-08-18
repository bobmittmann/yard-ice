/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the libstm32f.
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
 * @file stm32-crc.h
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __STM32_CRC_H__
#define __STM32_CRC_H__

/*-------------------------------------------------------------------------
 * CRC calculation unit
 *-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------
 * Data register (CRC_DR) */
#define STM32_CRC_DR 0x00

/*-------------------------------------------------------------------------
 * Independent data register (CRC_IDR) */
#define STM32_CRC_IDR 0x04

/*-------------------------------------------------------------------------
 * Control register (CRC_CR) */
#define STM32_CRC_CR 0x01

/* Bit 0 RESET */
#define CRC_RESET (1 << 0)
/* Resets the CRC calculation unit and sets the data register to 0xFFFF FFFF.
This bit can only be set, it is automatically cleared by hardware. */

#ifndef __ASSEMBLER__

#include <stdint.h>

struct stm32_crc {
	volatile uint32_t dr;
	volatile uint32_t idr;
	volatile uint32_t cr;
};

#endif /* __ASSEMBLER__ */

#endif /* __STM32_CRC_H__ */
