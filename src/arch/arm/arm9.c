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
 * @file arm9.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 


#include <stdlib.h>
#include <string.h>

#include "target.h"
#include "target/arm.h"
#include "armice.h"

int arm9_probe(FILE * f, ice_drv_t * ice, ice_mem_entry_t * mem)
{
#if 0
	ice_oid_t oid;
	ice_val_t id_code;

	/* configure the ICE driver */
	oid = ice->lookup(ice->arg, "id");
	ice->get(ice->arg, oid, &id_code);

	if ((id_code == 0x1f0f0f0f) || (id_code == 0x3f0f0f0f))
		return 1;
#endif

	/* return FAIL */
	return 0;
}

const struct ice_mem_entry target_arm9_mem[] = {
	{ .name = "vectors", .flags = MEM_32_BITS,
		.addr = { .base = 0x00000000, .offs = 0} , 
		.blk = {.count = 1, .size = 32},
		.op = &arm_ram_oper
	},
	{ .name = "mem", .flags = MEM_32_BITS,
		.addr = { .base = 0x00000000, .offs = 32} , 
		.blk = {.count = 1, .size = 0xffffffe0 },
		.op = &arm_ram_oper
	},
	{ .name = "", .flags = 0, 
		.addr = { .base = 0x00000000, .offs = 0} , 
		.blk = {.count = 0, .size = 0},
		.op = NULL
	}
};


const struct target_cpu target_arm9_cpu = {
	.family = "ARM9",
	.model = "v5t",
	.vendor = "ARM",
	.irlength = 4,
	.idmask = 0x0ff0000f,
	.idcomp = 0x0790000f,
	.sym = void_sym
};

const struct target_arch target_arm9le_arch = {
	.name = "ARM9-GENERIC",
	.model = "LTL ENDIAN",
	.vendor = "BORESTE",
	.cpu = &target_arm9_cpu,
	.sym = void_sym
};
const struct armice_cfg target_arm9le_cfg = {
	.endianness = LITTLE_ENDIAN,
	.work_addr = 0x00000000,
	.work_size = MEM_KiB(4)
};

const struct target_info target_arm9le = {
	.name = "arm9-le",
	.arch = &target_arm9le_arch,
//	.cpu = &target_arm9_cpu,
	.mem = target_arm9_mem,

	.ice_drv = &armice_drv,
	.ice_cfg = (void *)&target_arm9le_cfg,

	.jtag_clk_slow = 5000,
	.jtag_clk_def = 5000000,
	.jtag_clk_max = 12500000,

	/* The target has a TRST connection */
	.has_trst = YES,
	/* The target has a nRST connection */
	.has_nrst = YES,
	/* Target supports adaptive clock */
	.has_rtck = YES,
	/* The preferred clock method is adaptive (RTCK) */
	.prefer_rtck = YES,
	/* Start with slow clock */
	.clk_slow_on_connect = NO,
	/* Set default clock after init  */
	.clk_def_on_init = YES,

	/* The preferred reset method is nRST */
	.reset_mode = RST_HARD,

	.start_addr = 0x00000000,
	.on_init = (target_script_t)NULL,
	.on_halt = (target_script_t)NULL,
	.on_run = (target_script_t)NULL,
	.reset_script = (target_script_t)NULL,
	.probe = (target_script_t)arm9_probe
};

