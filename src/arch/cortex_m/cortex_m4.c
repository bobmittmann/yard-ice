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
 * @file cm3.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <crc.h>
#include <stdlib.h>
#include <sys/dcclog.h>

#include "target.h"
#include "armice.h"
#include "script.h"
#include "target/cm3.h"
#include "cm3ice.h"


const struct target_cpu cortex_m4_cpu = {
	.family = "Cortex-M4",
	.model = "v7m",
	.vendor = "ARM",
	.irlength = 4,
	.idmask = 0x7fffffff,
	.idcomp = 0x4ba00477,
//	.idmask = 0xffffffff,
//	.idcomp = 0x3ba00477,
	.sym = NULL
};

const struct target_fpu cortex_m4_fpu = {
	.model = "FPv4-SP",
	.sp_regs = 32,
	.dp_regs = 0
};

