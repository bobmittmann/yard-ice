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
 * @file stm32f205.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include "target/stm32f.h"


const struct target_arch stm32f205_arch = {
	.name = "STM32F205",
	.model = "generic",
	.vendor = "bobmittmann",
	.cpu = &cortex_m3_cpu,
	.sym = void_sym
};

const struct ice_mem_entry stm32f205_mem[] = {
	{ .name = "flash", .flags = MEM_32_BITS,
		.addr = { .base = 0x00000000, .offs = 0 }, 
		.blk = {.count = 256, .size = MEM_KiB(1)},
		.op = &flash_stm32f2_oper
	},
	{ .name = "sram", .flags = MEM_32_BITS,
		.addr = { .base = 0x20000000, .offs = 0 }, 
		.blk = {.count = 1, .size = MEM_KiB(64)},
		.op = &cm3_ram_oper 
	},
	{ .name = "", .flags = 0, .addr = { .base = 0, .offs = 0 }, 
		.blk = {.count = 0, .size = 0},
		.op = NULL
	}
};

const struct target_info stm32f205 = {
	.name = "STM32F205",
	.arch = &stm32f205_arch,
	.mem = (struct ice_mem_entry *)stm32f205_mem,

	.ice_drv = &cm3ice_drv,
	.ice_cfg = (void *)&stm32f_cfg,

	.jtag_clk_slow = 1000000,
	.jtag_clk_def = 8000000,
	.jtag_clk_max = 8000000,

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
	.on_halt = (target_script_t)NULL,
	.on_run = NULL,
	.reset_script = (target_script_t)cm3_reset,
	.probe = (target_script_t)cm3_probe,
	.test = (target_test_t)NULL
};

