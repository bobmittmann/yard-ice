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
 * @file at91sam9260.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include "target/at91sam9.h"
#include "armice.h"

const struct target_arch at91sam9260_arch = {
	.name = "AT91SAM9260",
	.model = "generic",
	.vendor = "BORESTE",
	.cpu = &at91sam9_cpu,
	.sym = void_sym
};

const struct ice_mem_entry at91sam9260_mem[] = {
	{ .name = "base", .flags = MEM_32_BITS,
		.addr = { .base = 0x00000000, .offs = 0 }, 
		.blk = {.count = 1, .size = MEM_MiB(1)},
		.op = &arm_ram_oper
	},
	{ .name = "rom", .flags = MEM_32_BITS,
		.addr = { .base = 0x00100000, .offs = 0 }, 
		.blk = {.count = 1, .size = MEM_KiB(32)},
		.op = &arm_ram_oper
	},
	{ .name = "sram0", .flags = MEM_32_BITS,
		.addr = { .base = 0x00200000, .offs = 0 }, 
		.blk = {.count = 1, .size = MEM_KiB(4)},
		.op = &arm_ram_oper
	},
	{ .name = "sram1", .flags = MEM_32_BITS,
		.addr = { .base = 0x00300000, .offs = 0 }, 
		.blk = {.count = 1, .size = MEM_KiB(4)},
		.op = &arm_ram_oper
	},
	{ .name = "boot", .flags = MEM_16_BITS,
		.addr = { .base = 0x10000000, .offs = 0 },
		.blk = {.count = 8, .size = MEM_KiB(8)},
		.op = &arm_cfi16_2_oper
	},
	{ .name = "flash", .flags = MEM_16_BITS,
		.addr = { .base = 0x10000000, .offs = MEM_KiB(64) }, 
		.blk = {.count = 128, .size = MEM_KiB(64)},
		.op = &arm_cfi16_2_oper
	},
	{ .name = "sdram", .flags = MEM_32_BITS,
		.addr = { .base = 0x20000000, .offs = 0 },
		.blk = {.count = 256, .size = MEM_MiB(1)},
		.op = &arm_ram_oper
	},
	{ .name = "nand", .flags = MEM_32_BITS,
		.addr = { .base = 0x50000000, .offs = 0 },
		.blk = {.count = 256, .size = MEM_MiB(1)},
		.op = &arm_ram_oper
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

const struct target_info at91sam9260 = {
	.name = "at91sam9260",
	.arch = &at91sam9260_arch,
//	.cpu = &at91sam9_cpu,
	.mem = (struct ice_mem_entry *)at91sam9260_mem,

	.ice_drv = &armice_drv,
	.ice_cfg = (void *)&at91sam9_cfg,

	.jtag_clk_slow = 5400,
	.jtag_clk_def = 5000000,
	.jtag_clk_max = 5000000,

	/* The target has a TRST connection */
	.has_trst = YES,
	/* The target has a nRST connection */
	.has_nrst = YES,
	/* Target supports adaptive clock */
	.has_rtck = NO,
	/* The preferred clock method is adaptive (RTCK) */
	.prefer_rtck = YES,
	/* Start with slow clock */
	.clk_slow_on_connect = YES,
	/* Set default clock after init  */
	.clk_def_on_init = YES,

	/* The preferred reset method is nRST */
	.reset_mode = RST_HARD,

	.start_addr = 0x00000000,

	.on_init = (target_script_t)at91sam9_on_init,
	.on_halt = (target_script_t)at91sam9_on_halt,
	.on_run = NULL,
	.reset_script = (target_script_t)at91sam9_reset,
	.probe = (target_script_t)at9sam9260_probe
};

