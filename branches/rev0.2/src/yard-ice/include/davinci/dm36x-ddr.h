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
 * @file dm36x-ddr.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#ifndef __DM36X_DDR_H__
#define __DM36X_DDR_H__

/* DDR Memory Ctrl Register */

#define DDR_SDRSTAT 0x04
#define DDR_SDBCR 0x08
#define DDR_SDRCR 0x0c
#define DDR_SDTIMR 0x10
#define DDR_SDTIMR2 0x14
#define DDR_SDBCR2 0x1c
#define DDR_PBBPR 0x20
#define DDR_IRR 0xc0
#define DDR_IMR 0xc4
#define DDR_IMSR 0xc8
#define DDR_IMCR 0xcc
#define DDR_DDRPHYCR 0xe4
#define DDR_DDRPHYCR2 0xe8

#ifndef __ASSEMBLER__

#include <stdint.h>

struct dm36x_ddr {
	uint32_t pid;
};

#endif /* __ASSEMBLER__ */


#endif /* __DM36X_DDR_H__ */

