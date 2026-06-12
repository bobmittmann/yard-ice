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
 * @file stm32.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#ifndef __TARGET_STM32F_H__
#define __TARGET_STM32F_H__

#include <stdint.h>
#include <cm3ice.h>
#include <target.h>
#include <target/cm3.h>

extern struct ice_mem_entry stm32f_mem[];

extern const struct ice_mem_entry stm32f20xxb_mem[];

extern const struct ice_mem_entry stm32f20xxc_mem[];

extern const struct ice_mem_entry stm32f20xxe_mem[];

extern const struct ice_mem_oper flash_stm32f1_oper;

extern const struct ice_mem_oper flash_stm32l1_oper;

extern const struct ice_mem_oper flash_stm32f2_oper;

extern const struct ice_mem_oper flash_stm32f3_oper;

extern const struct ice_mem_oper flash_stm32l4_oper;

extern const struct cm3ice_cfg stm32f_cfg;

extern const struct target_arch stm32f_arch;

extern const struct target_info stm32f205;

extern const struct target_info stm32f10x;

extern const struct target_info stm32f207ve;

extern struct target_info stm32f;

int stm32f1xx_on_init(FILE * f, const ice_drv_t * ice, 
					 ice_mem_entry_t * mem);

int stm32l1xx_on_init(FILE * f, const ice_drv_t * ice, 
					 ice_mem_entry_t * mem);

int stm32f2xx_on_init(FILE * f, const ice_drv_t * ice, 
					 ice_mem_entry_t * mem);

int stm32f3xx_on_init(FILE * f, const ice_drv_t * ice, 
					 ice_mem_entry_t * mem);

int stm32l4xx_on_init(FILE * f, const ice_drv_t * ice, 
					 ice_mem_entry_t * mem);

int stm32f1_flash_erase(cm3ice_ctrl_t * ctrl, 
					   ice_mem_ref_t * mem, 
					   ice_size_t len);

int stm32f1_flash_write(cm3ice_ctrl_t * ctrl, ice_mem_ref_t * mem, 
					 const void * buf, ice_size_t len);

int stm32l1_flash_erase(cm3ice_ctrl_t * ctrl, 
					   ice_mem_ref_t * mem, 
					   ice_size_t len);

int stm32l1_flash_write(cm3ice_ctrl_t * ctrl, ice_mem_ref_t * mem, 
					 const void * buf, ice_size_t len);

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
	}
#endif

#endif /* __TARGET_STM32F_H__ */

