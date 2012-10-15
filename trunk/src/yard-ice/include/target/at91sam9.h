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



#ifndef __TARGET_AT91SAM9_H__
#define __TARGET_AT91SAM9_H__

#include <stdint.h>
#include <target.h>
#include <target/arm.h>

extern const struct target_info at91sam7s32;

extern const struct target_info at91sam7s64;

extern const struct target_info at91sam7s128;

extern const struct target_info at91sam7s256;

extern const struct target_info at91sam7x256;

extern const struct target_info at91sam7xc256;

/* 
 *  generic at91sam9 target 
 * This target is not constant to allow dynamic chages
 * when probing a device
 */
extern const struct target_info at91sam9;
extern const struct target_info at91sam9260;

extern const struct ice_mem_entry at91sam9260_mem[]; 

extern const struct target_sym at91sam9_sym[];

extern const struct target_cpu at91sam9_cpu;

extern const struct armice_cfg at91sam9_cfg;

#ifdef __cplusplus
extern "C" {
#endif

int at91sam9_on_init(FILE * f, const struct ice_drv * ice, 
					 ice_mem_entry_t * mem);

int at91sam9_on_halt(FILE * f, const struct ice_drv * ice, 
					 ice_mem_entry_t * mem);

int at91sam9_reset(FILE * f, const struct ice_drv * ice, 
				   ice_mem_entry_t * mem);

int at9sam9260_probe(FILE * f, const struct ice_drv * ice, 
					 ice_mem_entry_t * mem);

int at9sam9_probe(FILE * f, const struct ice_drv * ice, 
				  ice_mem_entry_t * mem);

#ifdef __cplusplus
	}
#endif

#endif /* __TARGET_AT91SAM9_H__ */

