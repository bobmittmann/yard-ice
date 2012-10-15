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


#ifndef __DM36X_I2C_H__
#define __DM36X_I2C_H__

/* I2C Registers */
#define I2C_ICOAR 0x00
#define I2C_ICIMR 0x04
#define I2C_ICSTR 0x08
#define I2C_ICCLKL 0x0c
#define I2C_ICCLKH 0x10
#define I2C_ICCNT 0x14
#define I2C_ICDRR 0x18
#define I2C_ICSAR 0x1c
#define I2C_ICDXR 0x20
#define I2C_ICMDR 0x24
#define I2C_ICIVR 0x28
#define I2C_ICEMDR 0x2c
#define I2C_ICPSC 0x30
#define I2C_ICPID1 0x34
#define I2C_ICPID2 0x38


#define I2C_TARGET_FREQ 200000
#define I2C_OWN_ADDRESS 0x10

#define I2C_ICMDR_NACKMOD 0x00008000
#define I2C_ICMDR_FRE 0x00004000
#define I2C_ICMDR_STT 0x00002000
#define I2C_ICMDR_STP 0x00000800
#define I2C_ICMDR_MST 0x00000400
#define I2C_ICMDR_TRX 0x00000200
#define I2C_ICMDR_XA 0x00000100
#define I2C_ICMDR_RM 0x00000080
#define I2C_ICMDR_DLB 0x00000040
#define I2C_ICMDR_IRS 0x00000020
#define I2C_ICMDR_STB 0x00000010
#define I2C_ICMDR_FDF 0x00000008
#define I2C_ICMDR_BC8 0x00000007

#define I2C_ICSTR_AL_MSK 0x00000001
#define I2C_ICSTR_NACK_MSK 0x00000002
#define I2C_ICSTR_ARDY_MSK 0x00000004
#define I2C_ICSTR_ICRRDY_MSK 0x00000008
#define I2C_ICSTR_ICXRDY_MSK 0x00000010
#define I2C_ICSTR_SCD_MSK 0x00000020
#define I2C_ICSTR_BB_MSK 0x00001000

#define I2C_ICEMDR_EXTMODE 0x00000000

#ifndef __ASSEMBLER__

#include <stdint.h>

struct dm36x_i2c {
	uint32_t pid;
};

#endif /* __ASSEMBLER__ */

#endif /* __DM36X_I2C_H__ */

