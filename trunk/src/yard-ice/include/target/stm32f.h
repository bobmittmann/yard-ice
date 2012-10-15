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


#ifndef __TARGET_STM32F_H__
#define __TARGET_STM32F_H__

#include <stdint.h>
#include <cm3ice.h>
#include <target.h>
#include <target/cm3.h>

extern const struct cm3ice_cfg stm32f_cfg;

const struct target_arch stm32f_arch;

extern const struct target_info stm32f205;

extern const struct target_info stm32f207ve;

const struct ice_mem_oper flash_stm32f_oper;

int stm32f_on_init(FILE * f, const ice_drv_t * ice, 
					 ice_mem_entry_t * mem);

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
	}
#endif

#endif /* __TARGET_STM32F_H__ */

