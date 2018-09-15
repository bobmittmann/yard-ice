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
 * @file at91x40.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#ifndef __TARGET_AT91X40_H__
#define __TARGET_AT91X40_H__

extern const struct target_cpu at91m40800_cpu;

extern const struct target_cpu at91r40008_cpu;

extern const struct armice_cfg at91x40_cfg;

#ifdef __cplusplus
extern "C" {
#endif

int at91x40_reset(FILE * f, const struct ice_drv * ice, 
				  ice_mem_entry_t * mem);

int at91x40_on_halt(FILE * f, const struct ice_drv * ice, 
					ice_mem_entry_t * mem);

int at91x40_on_run(FILE * f, const struct ice_drv * ice, 
				   ice_mem_entry_t * mem);

int at91r40008_probe(FILE * f, const struct ice_drv * ice, 
				   ice_mem_entry_t * mem);

int at91m40800_probe(FILE * f, const struct ice_drv * ice, 
				   ice_mem_entry_t * mem);

#ifdef __cplusplus
	}
#endif

#endif /* __TARGET_AT91X40_H__ */

