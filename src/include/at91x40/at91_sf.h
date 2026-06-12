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



#ifndef __AT91_SF_H__
#define __AT91_SF_H__

/*****************************************************************************
 SF: Special Function Registers
 *****************************************************************************/
#define AT91_SF_BASE               0xfff00000

/* Chip ID Register - Read Only Hardwired */
#define SF_CIDR                   0x00 
/* Version of the chip */
#define SF_VERSION_MASK            (~(0x1f << 0))
/* Non Volatile Program Memory Size */
#define SF_NVPSIZ_MASK             (~(0x0f << 8))
#define SF_NVPSIZ_NONE             (0 << 8)
#define SF_NVPSIZ_32K              (3 << 8)
#define SF_NVPSIZ_64K              (5 << 8)
#define SF_NVPSIZ_128K             (7 << 8)
#define SF_NVPSIZ_256K             (9 << 8)
/* Volatile Data Memory Size */
#define SF_NVDSIZ_MASK             (~(0x0f << 12))
#define SF_NVDSIZ_NONE             (0 << 12)
/* Volatile Data Memory Size */
#define SF_VDSIZ_MASK              (~(0x0f << 16))
#define SF_VDSIZ_NONE              (0 << 16)
#define SF_VDSIZ_1K                (0 << 16)
#define SF_VDSIZ_2K                (0 << 16)
#define SF_VDSIZ_4K                (0 << 16)
#define SF_VDSIZ_8K                (0 << 16)
/* Chip Architecture */
#define	SF_ARCH_MASK               (~(0xff << 20))
#define	SF_ARCH_AT91x40            (0x40 << 20)
/* Non Volatile Program Memory Type */
#define SF_NVPTYP_MASK             (~(7 << 28))
#define SF_NVPTYP_M                (1 << 28)
#define SF_NVPTYP_R                (4 << 28)
/* Extension Flag */
#define SF_EXT                     (1 << 31)

/* Chip ID Extension Register - Read Only Hardwired */
#define SF_EXID                    0x04

/* Reset Status Register - Read Only */
#define SF_RSR                     0x08

#define SF_EXT_RESET               0x6c
#define SF_WD_RESET                0x53

/* Memory Mode Register - Read/Write */
#define SF_MMR                     0x0C

#define SF_RAMWU                   1

/* Protect Mode Register - Read/Write */
#define SF_PMR                     0x18

#define SF_PMRKEY                  (0x27a8 << 16)
#define SF_AIC                     (1 << 5)

#ifndef __ASSEMBLER__

#include <stdint.h>

struct at91_sf {
	uint32_t cidr;
	uint32_t exid;
	uint32_t rsr;
	uint32_t mmr;
	uint32_t res0;
	uint32_t res1;
	uint32_t pmr;
	uint32_t res[4089];
};

#define AT91_SF ((struct at91_sf *)AT91_SF_BASE)

#endif /* __ASSEMBLER__ */

#endif /* __AT91_SF_H__ */

