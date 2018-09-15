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
 * @file dummy.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 


#include <stdlib.h>
#include <string.h>

#include "target.h"

int dummy_probe(FILE * f, const struct ice_drv * ice, ice_mem_entry_t * mem)
{
	/* return probe FAIL */
	return 0;
}

const struct ice_mem_entry target_null_mem[] = {
	{ .name = "", .flags = 0, 
		.addr = { .base = 0, .offs = 0 }, 
		.blk = {.count = 0, .size = 0},
		.op = NULL
	}
};

const struct target_cpu target_null_cpu = {
	.family = "NONE",
	.model = "0",
	.vendor = "BORESTE",
	.irlength = 0,
	.idmask = 0x00000000,
	.idcomp = 0x00000000,
	.sym = void_sym
};

const struct target_arch target_null_arch = {
	.name = "DUMMY",
	.model = "none",
	.vendor = "BORESTE",
	.cpu = &target_null_cpu,
	.sym = void_sym
};

const struct target_info target_null = {
	.name = "null",
	.arch = &target_null_arch,
//	.cpu = &target_null_cpu,
	.mem = target_null_mem,

	.ice_drv = &ice_drv_null,
	.ice_cfg = NULL,

	.jtag_clk_slow = 10000,
	.jtag_clk_def = 100000,
	.jtag_clk_max = 1000000,

	/* The target has a TRST connection */
	.has_trst = NO,
	/* The target has a nRST connection */
	.has_nrst = NO,
	/* Target supports adaptive clock */
	.has_rtck = NO,
	/* The preferred clock method is adaptive (RTCK) */
	.prefer_rtck = NO,
	/* Start with slow clock */
	.clk_slow_on_connect = NO,
	/* Set default clock after init  */
	.clk_def_on_init = NO,

	/* No preferred reset method */
	.reset_mode = RST_AUTO,

	.start_addr = 0x00000000,
	.on_init = (target_script_t)NULL,
	.on_halt = (target_script_t)NULL,
	.on_run = (target_script_t)NULL,
	.reset_script = (target_script_t)NULL,
	.probe = (target_script_t)dummy_probe
};

