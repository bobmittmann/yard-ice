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
 * @file at91sam7.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */



#ifndef __TARGET_AT91SAM7_H__
#define __TARGET_AT91SAM7_H__

#include <stdint.h>
#include <target.h>
#include <target/arm.h>
#include "armice.h"

extern const struct target_info at91sam7s32;

extern const struct target_info at91sam7s64;

extern const struct target_info at91sam7s128;

extern const struct target_info at91sam7s256;


extern const struct target_info at91sam7x256;

extern const struct target_info at91sam7xc256;

extern const struct target_info at91sam7x512;

/* 
 *  generic at91sam7 target 
 * This target is not constant to allow dynamic chages
 * when probing a device
 */
extern const struct target_info at91sam7;

extern const struct target_sym at91sam7_sym[];

extern const struct target_cpu at91sam7_cpu;

extern const struct armice_cfg at91sam7_cfg;

extern const struct ice_mem_oper flash_at91sam7_128_oper;

extern const struct ice_mem_oper flash_at91sam7_256_oper;

extern const struct ice_mem_entry at91sam7s256_mem[]; 

#ifdef __cplusplus
extern "C" {
#endif

int at91sam7_on_init(FILE * f, const struct ice_drv * ice, 
					 ice_mem_entry_t * mem);

int at91sam7_on_halt(FILE * f, const struct ice_drv * ice, 
					 ice_mem_entry_t * mem);

int at91sam7_reset(FILE * f, const struct ice_drv * ice, 
				   ice_mem_entry_t * mem);

int at91sam7s_flash128_erase(armice_ctrl_t * ctrl, ice_mem_ref_t * addr,
							 ice_size_t len);

int at91sam7s_flash128_write(armice_ctrl_t * ctrl, ice_mem_ref_t * addr,
							 const void * ptr, ice_size_t len);

int at91sam7s_flash256_erase(armice_ctrl_t * ctrl, ice_mem_ref_t * addr,
							 ice_size_t len);

int at91sam7s_flash256_write(armice_ctrl_t * ctrl, ice_mem_ref_t * addr,
							 const void * ptr, ice_size_t len);

int at9sam7x256_probe(FILE * f, const struct ice_drv * ice, 
					 ice_mem_entry_t * mem);

int at9sam7s64_probe(FILE * f, const struct ice_drv * ice, 
					 ice_mem_entry_t * mem);

int at9sam7s256_probe(FILE * f, const struct ice_drv * ice, 
					 ice_mem_entry_t * mem);

int at9sam7xc256_probe(FILE * f, const struct ice_drv * ice, 
					 ice_mem_entry_t * mem);

int at9sam7_probe(FILE * f, const struct ice_drv * ice, 
				  ice_mem_entry_t * mem);

int at9sam7_test(FILE * f, const struct ice_drv * ice, 
				  ice_mem_entry_t * mem, uint32_t val);

#ifdef __cplusplus
	}
#endif

#endif /* __TARGET_AT91SAM7_H__ */

