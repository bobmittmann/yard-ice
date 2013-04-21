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
 * @file stm32f.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <crc.h>
#include <stdlib.h>
#include <sys/dcclog.h>

#include "target.h"
#include "script.h"
#include "cm3ice.h"
#include "target/stm32f.h"

#define STM32F2X
#include "stm32f_defs.h"

#define FLASH 1
#define SRAM 2

int stm32f_pos_config(FILE * f, const ice_drv_t * ice,
					  target_info_t * target)
{
	ice_mem_entry_t * mem = (ice_mem_entry_t *)target->mem;
	uint16_t memsz;
	uint32_t id;
	uint32_t dev_id;

	ice_rd32(ice, 0xe0042000, &id);

	fprintf(f, " - MCU device id: 0x%08x\n", id); 
	dev_id = id & 0x0fff;

	fprintf(f, "   - "); 

	switch (dev_id) {
	case 0x412:
		ice_rd16(ice, 0x1ffff7e0, &memsz);
		fprintf(f, "STM32F10X, low-density\n"); 
		break;
	case 0x410:
		ice_rd16(ice, 0x1ffff7e0, &memsz);
		fprintf(f, "STM32F10X, medium-density\n"); 
		break;
	case 0x414:
		ice_rd16(ice, 0x1ffff7e0, &memsz);
		fprintf(f, "STM32F10X, high-density\n"); 
		break;
	case 0x430:
		ice_rd16(ice, 0x1ffff7e0, &memsz);
		fprintf(f, "STM32F10X, XL-density\n"); 
		break;
	case 0x418:
		ice_rd16(ice, 0x1ffff7e0, &memsz);
		fprintf(f, "STM32F10X, connectivity\n"); 
		break;
	case 0x411:
		ice_rd16(ice, 0x1fff7a22, &memsz);
		fprintf(f, "STM32F2XX\n"); 
		target->on_init = (target_script_t)stm32f2xx_on_init,
		mem[FLASH].op = &flash_stm32f2_oper;
		mem[FLASH].blk.size = MEM_KiB(16);
		mem[FLASH].blk.count = memsz / 16;
		mem[SRAM].blk.count = 128;
		break;
	case 0x413:
		ice_rd16(ice, 0x1fff7a22, &memsz);
		fprintf(f, "STM32F4XX\n"); 
		target->on_init = (target_script_t)stm32f2xx_on_init,
		mem[FLASH].op = &flash_stm32f2_oper;
		mem[FLASH].blk.size = MEM_KiB(16);
		mem[FLASH].blk.count = memsz / 16;
		mem[SRAM].blk.count = 192;
		break;
	case 0x420:
		ice_rd16(ice, 0x1ffff7e0, &memsz);
		fprintf(f, "STM32F100\n"); 
		target->on_init = (target_script_t)stm32f1xx_on_init,
		mem[FLASH].op = &flash_stm32f1_oper;
		mem[FLASH].blk.size = MEM_KiB(1);
		mem[FLASH].blk.count = memsz;
		if ((memsz == 16) || (memsz == 32))
			mem[SRAM].blk.count = 4;
		else if ((memsz == 64) || (memsz == 128))
			mem[SRAM].blk.count = 8;
		break;
	default:
		fprintf(f, "Unknown device: 0x%03x!\n", dev_id); 
		return -1;
	}

	fprintf(f, "   - Flash size = %dKiB\n", memsz & 0xffff); 

	return 0;
}



const struct cm3ice_cfg stm32f_cfg = {
	.endianness = LITTLE_ENDIAN
};

const struct target_arch stm32f_arch = {
	.name = "STM32F",
	.model = "generic",
	.vendor = "bobmittmann",
	.cpu = &cortex_m3_cpu,
	.sym = void_sym
};

/* STM32F Generic */
struct ice_mem_entry stm32f_mem[] = {
	{ .name = "boot", .flags = MEM_32_BITS,
		.addr = { .base = 0x00000000, .offs = 0 }, 
		.blk = {.count = 64, .size = MEM_KiB(64)},
		.op = &cm3_ram_oper
	},
	{ .name = "flash", .flags = MEM_32_BITS,
		.addr = { .base = 0x08000000, .offs = 0 }, 
		.blk = {.count = 128, .size = MEM_KiB(1)},
		.op = &flash_stm32f1_oper
	},
	{ .name = "sram", .flags = MEM_32_BITS,
		.addr = { .base = 0x20000000, .offs = 0 }, 
		.blk = {.count = 16, .size = MEM_KiB(1)},
		.op = &cm3_ram_oper 
	},
	{ .name = "sys", .flags = MEM_32_BITS,
		.addr = { .base = 0x40000000, .offs = 0x00000000 }, 
		.blk = {.count = 0x10000, .size = 0x1000},
		.op = &cm3_ram_oper 
	},
	{ .name = "scs", .flags = MEM_32_BITS,
		.addr = { .base = 0xe000e000, .offs = 0x00000000 }, 
		.blk = {.count = 1, .size = 0x1000},
		.op = &cm3_ram_oper 
	},
	{ .name = "", .flags = 0, .addr = { .base = 0, .offs = 0 }, 
		.blk = {.count = 0, .size = 0},
		.op = NULL
	}
};

const struct target_info stm32f = {
	.name = "stm32f",
	.arch = &stm32f_arch,
	.ice_drv = &cm3ice_drv,
	.ice_cfg = (void *)&stm32f_cfg,

	.mem = (struct ice_mem_entry *)stm32f_mem,

	.jtag_clk_slow = 12000000,
	.jtag_clk_def = 12000000,
	.jtag_clk_max = 16000000,

	/* The target has a TRST connection */
	.has_trst = YES,
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
	/* auto probe scan path */
	.jtag_probe = YES,
	/* hardware reset before ICE configure */
	.reset_on_config = YES,

	/* The prefered reset method is system */
	.reset_mode = RST_SYS,

	.start_addr = 0x00000000,

	.pos_config = (target_config_t)stm32f_pos_config,
	.on_init = (target_script_t)stm32f1xx_on_init,
	.reset_script = (target_script_t)cm3_reset,
	.probe = (target_script_t)cm3_probe,
};

