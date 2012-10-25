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

#include "target/at91x40.h"

int a7e10x2_on_init(FILE * f, const struct ice_drv * ice, 
					 ice_mem_entry_t * mem)
{
	/* disable the watchdog */
	ice_wr32(ice, AT91_BASE_WD + WD_OMR, WD_OKEY);
	ice_wr32(ice, 0xffe00000, 0x01002121);
	ice_wr32(ice, 0xffe00004, 0x02002202);
	ice_wr32(ice, 0xffe00008, 0x02102225);
	ice_wr32(ice, 0xffe0000c, 0x02202025);
	ice_wr32(ice, 0xffe00010, 0x02402202);
	ice_wr32(ice, 0xffe00020, 1);

	return 0;
}

const struct target_arch a7e10x2_arch = {
	.name = "A7E10",
	.model = "x2",
	.vendor = "BORESTE",
	.cpu = &at91m40800_cpu,
	.sym = void_sym
};

const struct ice_mem_entry a7e10x2_mem[] = {
	{ .name = "ram", .flags = MEM_32_BITS,
		.addr = { .base = 0x00000000, .offs = 0 }, 
		.blk = {.count = 1, .size = MEM_KiB(8)},
		.op = &arm_ram_oper
	},
	{ .name = "uboot", .flags = MEM_16_BITS,
		.addr = { .base = 0x01000000, .offs = 0 }, 
		.blk = {.count = 3, .size = MEM_KiB(8)},
		.op = &arm_cfi16_2_oper
	},
	{ .name = "config", .flags = MEM_16_BITS,
		.addr = { .base = 0x01000000, .offs = MEM_KiB(24) }, 
		.blk = {.count = 1, .size = MEM_KiB(8)},
		.op = &arm_cfi16_2_oper
	},
	{ .name = "app", .flags = MEM_16_BITS,
		.addr = { .base = 0x01000000, .offs = MEM_KiB(32) }, 
		.blk = {.count = 3, .size = MEM_KiB(32)},
		.op = &arm_cfi16_2_oper
	},
	{ .name = "data", .flags = MEM_16_BITS,
		.addr = { .base = 0x01000000, .offs = MEM_KiB(128) }, 
		.blk = {.count = 6, .size = MEM_KiB(64)},
		.op = &arm_cfi16_2_oper
	},
	{ .name = "cs1", .flags = MEM_8_BITS,
		.addr = { .base = 0x02000000, .offs = 0 }, 
		.blk = {.count = 1, .size = 16},
		.op = &arm_ram_oper
	},
	{ .name = "cs2", .flags = MEM_8_BITS,
		.addr = { .base = 0x02100000, .offs = 0 }, 
		.blk = {.count = 1, .size = 16},
		.op = &arm_ram_oper
	},
	{ .name = "cs8900", .flags = MEM_16_BITS,
		.addr = { .base = 0x02200000, .offs = 0 }, 
		.blk = {.count = 1, .size = 32},
		.op = &arm_ram_oper
	},
	{ .name = "cs4", .flags = MEM_8_BITS,
		.addr = { .base = 0x02400000, .offs = 0 }, 
		.blk = {.count = 1, .size = 16},
		.op = &arm_ram_oper
	},
	{ .name = "peripheral", .flags = MEM_32_BITS,
		.addr = { .base = 0xffc00000, .offs = 0 }, 
		.blk = {.count = 1, .size = 0x400000},
		.op = &arm_ram_oper
	},
	{ .name = "", .flags = 0, .addr = { .base = 0, .offs = 0 }, 
		.blk = {.count = 0, .size = 0},
		.op = NULL
	}
};

const struct target_info a7e10x2 = {
	.name = "a7e10x2",
	.arch = &a7e10x2_arch,
//	.cpu = &at91m40800_cpu,
	.mem = (struct ice_mem_entry *)a7e10x2_mem,

	.ice_drv = &armice_drv,
	.ice_cfg = (void *)&at91x40_cfg,

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

	.start_addr = 0x1010000,
	.on_init = (target_script_t)a7e10x2_on_init,
	.on_halt = (target_script_t)at91x40_on_halt,
	.on_run = (target_script_t)at91x40_on_run,
	.reset_script = (target_script_t)at91x40_reset,
	.probe = (target_script_t)at91m40800_probe
};


int a7e10x2_2m_on_init(FILE * f, const struct ice_drv * ice, 
					 ice_mem_entry_t * mem)
{
	/* disable the watchdog */
	ice_wr32(ice, AT91_BASE_WD + WD_OMR, WD_OKEY);
	ice_wr32(ice, 0xffe00000, 0x01002125);
	ice_wr32(ice, 0xffe00004, 0x02002202);
	ice_wr32(ice, 0xffe00008, 0x02102202);
	ice_wr32(ice, 0xffe0000c, 0x0220202d);
	ice_wr32(ice, 0xffe00010, 0x02402202);
	ice_wr32(ice, 0xffe00020, 1);

	return 0;
}

const struct ice_mem_entry a7e10x2_2m_top_mem[] = {
	{ .name = "ram", .flags = MEM_32_BITS,
		.addr = { .base = 0x00000000, .offs = 0 }, 
		.blk = {.count = 1, .size = MEM_KiB(8)},
		.op = &arm_ram_oper
	},
	{ .name = "uboot", .flags = MEM_16_BITS,
		.addr = { .base = 0x01000000, .offs = 0 }, 
		.blk = {.count = 1, .size = MEM_KiB(64)},
		.op = &arm_cfi16_2_oper
	},
	{ .name = "app", .flags = MEM_16_BITS,
		.addr = { .base = 0x01000000, .offs = MEM_KiB(64) }, 
		.blk = {.count = 1, .size = MEM_KiB(64)},
		.op = &arm_cfi16_2_oper
	},
	{ .name = "data", .flags = MEM_16_BITS,
		.addr = { .base = 0x01000000, .offs = 2 * MEM_KiB(64) }, 
		.blk = {.count = 29, .size = MEM_KiB(64)},
		.op = &arm_cfi16_2_oper
	},
	{ .name = "data1", .flags = MEM_16_BITS,
		.addr = { .base = 0x01000000, .offs = 31 * MEM_KiB(64) }, 
		.blk = {.count = 1, .size = MEM_KiB(16)},
		.op = &arm_cfi16_2_oper
	},
	{ .name = "config", .flags = MEM_16_BITS,
		.addr = { .base = 0x01000000, .offs = 31 * MEM_KiB(64) + MEM_KiB(16) }, 
		.blk = {.count = 2, .size = MEM_KiB(8)},
		.op = &arm_cfi16_2_oper
	},
	{ .name = "data2", .flags = MEM_16_BITS,
		.addr = { .base = 0x01000000, .offs = 31 * MEM_KiB(64) + MEM_KiB(32) }, 
		.blk = {.count = 1, .size = MEM_KiB(32)},
		.op = &arm_cfi16_2_oper
	},
	{ .name = "cs1", .flags = MEM_8_BITS,
		.addr = { .base = 0x02000000, .offs = 0 }, 
		.blk = {.count = 1, .size = 16},
		.op = &arm_ram_oper
	},
	{ .name = "cs2", .flags = MEM_8_BITS,
		.addr = { .base = 0x02100000, .offs = 0 }, 
		.blk = {.count = 1, .size = 16},
		.op = &arm_ram_oper
	},
	{ .name = "cs8900", .flags = MEM_16_BITS,
		.addr = { .base = 0x02200000, .offs = 0 }, 
		.blk = {.count = 1, .size = 32},
		.op = &arm_ram_oper
	},
	{ .name = "cs4", .flags = MEM_8_BITS,
		.addr = { .base = 0x02400000, .offs = 0 }, 
		.blk = {.count = 1, .size = 16},
		.op = &arm_ram_oper
	},
	{ .name = "peripheral", .flags = MEM_32_BITS,
		.addr = { .base = 0xffc00000, .offs = 0 }, 
		.blk = {.count = 1, .size = 0x400000},
		.op = &arm_ram_oper
	},
	{ .name = "", .flags = 0, .addr = { .base = 0, .offs = 0 }, 
		.blk = {.count = 0, .size = 0},
		.op = NULL
	}
};

const struct target_info a7e10x2_2m = {
	.name = "a7e10x2-2mb",
	.arch = &a7e10x2_arch,
//	.cpu = &at91m40800_cpu,

	.mem = (struct ice_mem_entry *)a7e10x2_2m_top_mem,

	.ice_drv = &armice_drv,
	.ice_cfg = (void *)&at91x40_cfg,

	.jtag_clk_slow = 1000000,
	.jtag_clk_def = 8000000,
	.jtag_clk_max = 16000000,

	/* The target has a TRST connection */
	.has_trst = NO,
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

	/* The preferred reset method is software */
	.reset_mode = RST_SOFT,

	.start_addr = 0x1000000,

	.on_init = (target_script_t)a7e10x2_2m_on_init,
	.on_halt = (target_script_t)at91x40_on_halt,
	.on_run = (target_script_t)at91x40_on_run,
	.reset_script = (target_script_t)at91x40_reset,
	.probe = (target_script_t)at91m40800_probe
};

