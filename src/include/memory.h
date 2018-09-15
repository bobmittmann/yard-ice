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
 * @file memory.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stdint.h>

#define MEM_BUS_8_BITS (0 << 0)
#define MEM_BUS_16_BITS (1 << 0)
#define MEM_BUS_32_BITS (2 << 0)
#define MEM_BUS_64_BITS (3 << 0)
#define MEM_BUS_MASK (3 << 0)

#define MEM_ALIGN_8_BITS (0 << 2)
#define MEM_ALIGN_16_BITS (1 << 2)
#define MEM_ALIGN_32_BITS (2 << 2)
#define MEM_ALIGN_64_BITS (3 << 2)
#define MEM_ALIGN_MASK (3 << 2)

#define MEM_RW (0 << 7)
#define MEM_RO (1 << 7)

#define MEM_8_BITS (MEM_BUS_8_BITS | MEM_ALIGN_8_BITS)
#define MEM_16_BITS (MEM_BUS_16_BITS | MEM_ALIGN_16_BITS)
#define MEM_32_BITS (MEM_BUS_32_BITS | MEM_ALIGN_32_BITS)
#define MEM_64_BITS (MEM_BUS_64_BITS | MEM_ALIGN_64_BITS)

#define MEM_KiB(X) ((X) * 1024)
#define MEM_MiB(X) ((X) * 1024 * 1024)

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
	}
#endif

#endif /* __MEMORY_H__ */

