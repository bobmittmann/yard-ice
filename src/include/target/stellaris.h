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
 * @file stellaris.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */



#ifndef __TARGET_STELLARIS_H__
#define __TARGET_STELLARIS_H__

#include <stdint.h>
#include <cm3ice.h>
#include <target.h>
#include <target/cm3.h>

extern const struct cm3ice_cfg stellaris_cfg;

extern const struct target_info stellaris_lm3s6965;

const struct ice_mem_oper flash_stellaris_oper;

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
	}
#endif

#endif /* __TARGET_STELLARIS_H__ */

