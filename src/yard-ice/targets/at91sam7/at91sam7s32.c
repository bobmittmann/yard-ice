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
 * @file .c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include "target/at91sam7.h"
#include "armice.h"

const struct target_arch at91sam7s32_arch = {
	.name = "AT91SAM7S32",
	.model = "generic",
	.vendor = "BORESTE",
	.cpu = &at91sam7_cpu,
	.sym = void_sym
};

const struct ice_mem_entry at91sam7s32_mem[] = {
	{ .name = "m0", .flags = MEM_32_BITS,
		.addr = { .base = 0x00000000, .offs = 0 }, 
		.blk = {.count = 1, .size = MEM_KiB(8)},
		.op = &arm_ram_oper
	},
	{ .name = "ram", .flags = MEM_32_BITS,
		.addr = { .base = 0x00200000, .offs = 0 }, 
		.blk = {.count = 1, .size = MEM_KiB(8)},
		.op = &arm_ram_oper
	},
	/* 256 pages of 128 bytes */
	{ .name = "flash", .flags = MEM_32_BITS,
		.addr = { .base = 0x00100000, .offs = 0 }, 
		.blk = {.count = 256, .size = 128},
		.op = &flash_at91sam7_128_oper
	},
	{ .name = "peripheral", .flags = MEM_32_BITS,
		.addr = { .base = 0xf0000000, .offs = 0 }, 
		.blk = {.count = 1, .size = 0x10000000},
		.op = &arm_ram_oper
	},
	{ .name = "", .flags = 0, .addr = { .base = 0, .offs = 0 }, 
		.blk = {.count = 0, .size = 0},
		.op = NULL
	}
};

const struct target_info at91sam7s32 = {
	.name = "at91sam7s32",
	.arch = &at91sam7s32_arch,
//	.cpu = &at91sam7_cpu,
	.mem = (struct ice_mem_entry *)at91sam7s32_mem,

	.ice_drv = &armice_drv,
	.ice_cfg = (void *)&at91sam7_cfg,

	.jtag_clk_slow = 1000000,
	.jtag_clk_def = 8000000,
	.jtag_clk_max = 16000000,

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
	.clk_def_on_init = YES,

	/* The preferred reset method is software */
	.reset_mode = RST_SOFT,

	.start_addr = 0x00000000,

	.on_init = (target_script_t)at91sam7_on_init,
	.on_halt = (target_script_t)at91sam7_on_halt,
	.on_run = NULL,
	.reset_script = (target_script_t)at91sam7_reset,
	.probe = (target_script_t)dummy_probe
};

