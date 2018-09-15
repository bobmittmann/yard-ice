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
 * @file jtagtool3.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <arch/at91x40.h>

#include "target.h"
#include "target/arm.h"
#include "target/at91x40.h"
#include "armice.h"

int jtagtool3_on_init(FILE * f, const struct ice_drv * ice, 
					 ice_mem_entry_t * mem)
{
	fprintf(f, "- Watchdog disable...\n");
	ice_wr32(ice, AT91_BASE_WD + WD_OMR, WD_OKEY);
	fprintf(f, "- EBI configure...\n");
	ice_wr32(ice, 0xffe00000, 0x010020b5);
	ice_wr32(ice, 0xffe00004, 0x02002229);
	ice_wr32(ice, 0xffe00008, 0x02402021);
	ice_wr32(ice, 0xffe0000c, 0x02802021);
	ice_wr32(ice, 0xffe00020, 1);
	fprintf(f, "- PIO configure...\n");
	ice_wr32(ice, AT91_BASE_PIO + PIO_ODR, 0xffffffff);
	ice_wr32(ice, AT91_BASE_PIO + PIO_PER, 0xffffffff);
	ice_wr32(ice, AT91_BASE_PIO + PIO_PDR,
			  PIO_P9 | PIO_P10 | PIO_P11 | PIO_P12 | 
			  PIO_P14 | PIO_P15 | PIO_P21 | PIO_P22 | 
			  PIO_P25 | PIO_P26 | PIO_P27 | PIO_P28 | PIO_P29);

	return 0;
}

const struct target_arch jtagtool3_arch = {
	.name = "JTAGTOOL",
	.model = "III",
	.vendor = "BORESTE",
	.cpu = &at91r40008_cpu,
	.sym = void_sym
};

const struct ice_mem_entry jtagtool3_mem[] = {
	{ .name = "ram0", .flags = MEM_32_BITS,
		.addr = { .base = 0x00000000, .offs = 0 }, 
		.blk = {.count = 1, .size = MEM_KiB(8)},
		.op = &arm_ram_oper
	},
	{ .name = "ram1", .flags = MEM_32_BITS,
		.addr = { .base = 0x00000000, .offs = MEM_KiB(8) }, 
		.blk = {.count = 1, .size = MEM_KiB(256) - MEM_KiB(8)},
		.op = &arm_ram_oper
	},
	{ .name = "uboot", .flags = MEM_16_BITS,
		.addr = { .base = 0x01000000, .offs = 0 }, 
		.blk = {.count = 4, .size = MEM_KiB(8)},
		.op = &arm_cfi16_2_oper
	},
	{ .name = "config", .flags = MEM_16_BITS,
		.addr = { .base = 0x01000000, .offs = MEM_KiB(32) }, 
		.blk = {.count = 1, .size = MEM_KiB(32)},
		.op = &arm_cfi16_2_oper
	},
	{ .name = "app", .flags = MEM_16_BITS,
		.addr = { .base = 0x01000000, .offs = MEM_KiB(64) }, 
		.blk = {.count = 4, .size = MEM_KiB(64)},
		.op = &arm_cfi16_2_oper
	},
	{ .name = "rbf", .flags = MEM_16_BITS,
		.addr = { .base = 0x01000000, .offs = MEM_KiB(5 * 64) }, 
		.blk = {.count = 1, .size = MEM_KiB(64)},
		.op = &arm_cfi16_2_oper
	},
	{ .name = "data", .flags = MEM_16_BITS,
		.addr = { .base = 0x01000000, .offs = MEM_KiB(6 * 64) }, 
		.blk = {.count = 58, .size = MEM_KiB(64)},
		.op = &arm_cfi16_2_oper
	},
	{ .name = "dm9000", .flags = MEM_16_BITS,
		.addr = { .base = 0x02000000, .offs = 0 }, 
		.blk = {.count = 1, .size = 4},
		.op = &arm_ram_oper
	},
	{ .name = "fpga0", .flags = MEM_16_BITS,
		.addr = { .base = 0x02200000, .offs = 0 }, 
		.blk = {.count = 1, .size = 4096},
		.op = &arm_ram_oper
	},
	{ .name = "fpga1", .flags = MEM_16_BITS,
		.addr = { .base = 0x02300000, .offs = 0 }, 
		.blk = {.count = 1, .size = 4096},
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

const struct target_info jtagtool3 = {
	.name = "jtagtool-3",
	.arch = &jtagtool3_arch,
	.mem = (struct ice_mem_entry *)jtagtool3_mem,

	.ice_drv = &armice_drv,
	.ice_cfg = (void *)&at91x40_cfg,

	.jtag_clk_slow = 1000000,
	.jtag_clk_def = 8000000,
	.jtag_clk_max = 10000000,

	/* The target has a TRST connection */
	.has_trst = NO,
	/* The target has a nRST connection */
	.has_nrst = NO,
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

	/* The preferred reset method is software */
	.reset_mode = RST_SOFT,

	.start_addr = 0x1010000,

	.on_init = (target_script_t)jtagtool3_on_init,
	.on_halt = (target_script_t)at91x40_on_halt,
	.on_run = (target_script_t)at91x40_on_run,
	.reset_script = (target_script_t)at91x40_reset,
	.probe = (target_script_t)at91r40008_probe
};

