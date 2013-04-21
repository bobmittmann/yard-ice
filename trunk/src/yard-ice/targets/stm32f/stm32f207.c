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
 * @file stm32f207.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 


#include "target/stm32f.h"

#include <stdio.h>
#include <sys/dcclog.h>

#define STM32F2X
#include "stm32f_defs.h"


const struct target_info stm32f207ve = {
	.name = "stm32f207ve",
	.arch = &stm32f_arch,
	.ice_drv = &cm3ice_drv,
	.ice_cfg = (void *)&stm32f_cfg,

	.mem = (struct ice_mem_entry *)stm32f20xxe_mem,

	.jtag_clk_slow = 12000000,
	.jtag_clk_def = 12000000,
	.jtag_clk_max = 16000000,

	/* The target has a TRST connection */
	.has_trst = YES,
	/* The target has a nRST connection */
	.has_nrst = YES,
	/* Target supports adaptive clock */
	.has_rtck = NO,
	/* The preferred clock method is adaptive (RTCK) */
	.prefer_rtck = NO,
	/* Start with slow clock */
	.clk_slow_on_connect = YES,
	/* Set default clock after init  */
	.clk_def_on_init = YES,
	/* auto probe scan path */
	.jtag_probe = YES,
	/* hardware reset before ICE configure */
	.reset_on_config = YES,

	/* The preferred reset method is core only */
	.reset_mode = RST_CORE,

	.start_addr = 0x00000000,


	.on_init = (target_script_t)stm32f2xx_on_init,
	.reset_script = (target_script_t)cm3_reset,
	.probe = (target_script_t)cm3_probe
};

