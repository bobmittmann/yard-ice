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
 * @file arm.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */

#ifndef __TARGET_ARM_H__
#define __TARGET_ARM_H__

#include <target.h>
#include <stdint.h>

/*
 * generic ARM RAM operations
 */
extern const struct ice_mem_oper arm_ram_oper;

/*
 * external flash CFI 16bit bus operations
 */
extern const struct ice_mem_oper arm_cfi16_oper;

extern const struct ice_mem_oper arm_cfi16_2_oper;

extern const struct target_info target_arm7le;
extern const struct target_info target_arm7be;

extern const struct ice_mem_entry target_arm7_mem[];

extern const struct target_info target_arm9le;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
	}
#endif

#endif

