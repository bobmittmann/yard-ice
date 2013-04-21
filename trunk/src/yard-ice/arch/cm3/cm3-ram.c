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
 * @file cm3-ram..c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include "target.h"
#include "target/arm.h"
#include "ice_drv.h"
#include "cm3ice.h"

/* 
 * embedded ram methods
 */
const struct ice_mem_oper cm3_ram_oper = {
	.read = (ice_mem_read_t)cm3ice_mem_read,
	.write = (ice_mem_write_t)cm3ice_mem_write, 
	.erase = (ice_mem_erase_t)cm3ice_mem_zero
};

