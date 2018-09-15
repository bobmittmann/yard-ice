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

#define TRACE_LEVEL TRACE_LVL_DBG
#include <trace.h>

#include <crc.h>
#include <stdlib.h>
#include <sys/dcclog.h>

#include "target.h"
#include "script.h"
#include "cm3ice.h"
#include "target/stm32f.h"

#define STM32F2X
#include "stm32f_defs.h"

#define FLASH  1
#define EEPROM 2
#define SRAM   3
#define CCM    4

uint16_t stm32f10xxx_config(const ice_drv_t * ice, 
							target_info_t * target)
{
	ice_mem_entry_t * mem = (ice_mem_entry_t *)target->mem;
	uint16_t memsz;
	
	target->on_init = (target_script_t)stm32f1xx_on_init,
	ice_rd16(ice, 0x1ffff7e0, &memsz);

	mem[FLASH].op = &flash_stm32f1_oper;
	if (memsz <= 128) {
		mem[FLASH].blk.size = MEM_KiB(1);
		mem[FLASH].blk.count = memsz;
	} else {
		mem[FLASH].blk.size = MEM_KiB(2);
		mem[FLASH].blk.count = memsz / 2;
	}

	if (memsz == 16) 
		mem[SRAM].blk.count = 6;
	else if (memsz == 32)
		mem[SRAM].blk.count = 10;
	else if (memsz == 64) 
		mem[SRAM].blk.count = 20;
	else if (memsz == 128) 
		mem[SRAM].blk.count = 20;
	else if (memsz == 256) 
		mem[SRAM].blk.count = 48;
	else 
		mem[SRAM].blk.count = 64;

	mem[CCM].blk.count = 0;

	return memsz;
}

struct pkg {
	uint8_t id;
	char name[11];
};

const struct pkg stm32lpkg_tab[] = {
	{ 0, "LQFP64" },
	{ 1, "WLCSP64" },
	{ 2, "LQFP100" },
	{ 10, "UFQFPN48" },
	{ 11, "LQFP48" },
	{ 12, "WLCSP49" },
	{ 13, "UFBGA64" },
	{ 14, "UFBGA100" },
	{ 255, "" }
};

const char * stm32lpkg_name(unsigned int id) 
{
	const char * s = "Unknown";
	int i;

	for (i = 0; i < sizeof(stm32lpkg_tab) /  sizeof(struct pkg); ++i) {
		if (stm32lpkg_tab[i].id == id) {
			s = stm32lpkg_tab[i].name;
			break;
		}
	}

	return s;
}

int stm32f_pos_config(FILE * f, const ice_drv_t * ice,
					  target_info_t * target)
{
	ice_mem_entry_t * mem = (ice_mem_entry_t *)target->mem;
	uint16_t memsz;
	uint16_t pkg;
	uint32_t id;
	uint32_t dev_id;

	DCC_LOG1(LOG_TRACE, "target=0x%p", target);

	/* Make sure we are using the internal oscillator */
//	if (ice_wr32(ice, STM32F_BASE_RCC + RCC_CFGR, 0) < 0) {
//		fprintf(f, " - ice_wr32 error!\n"); 
//		return -1;
//	}

	if (ice_rd32(ice, 0xe0042000, &id) < 0) {
		fprintf(f, " - ice_rd32 error!\n"); 
		return -1;
	}

	INF("STM32: [0xe0042000] MCU_ID=0x%08x", id);
	fprintf(f, " - MCU device id: 0x%08x\n", id); 
	dev_id = id & 0x0fff;

	fprintf(f, "   - "); 

	switch (dev_id) {
	case 0x412:
		fprintf(f, "STM32F10X, low-density\n"); 
		memsz = stm32f10xxx_config(ice, target);
		break;

	case 0x410:
		fprintf(f, "STM32F10X, medium-density\n"); 
		memsz = stm32f10xxx_config(ice, target);
		break;

	case 0x414:
		fprintf(f, "STM32F10X, high-density\n"); 
		memsz = stm32f10xxx_config(ice, target);
		break;

	case 0x430:
		fprintf(f, "STM32F10X, XL-density\n"); 
		memsz = stm32f10xxx_config(ice, target);
		break;

	case 0x418:
		fprintf(f, "STM32F10X, connectivity\n"); 
		memsz = stm32f10xxx_config(ice, target);
		break;

	case 0x420:
		fprintf(f, "STM32F100\n"); 
		memsz = stm32f10xxx_config(ice, target);
		break;

	case 0x411:
		ice_rd16(ice, 0x1fff7a22, &memsz);
		fprintf(f, "STM32F2XX\n"); 
		target->on_init = (target_script_t)stm32f2xx_on_init,
		mem[FLASH].op = &flash_stm32f2_oper;
		mem[FLASH].blk.size = MEM_KiB(16);
		mem[FLASH].blk.count = memsz / 16;
		mem[EEPROM].blk.count = 0;
		mem[SRAM].blk.count = 128;
		mem[CCM].blk.count = 0;
		break;

	case 0x413:
		ice_rd16(ice, 0x1fff7a22, &memsz);
		fprintf(f, "STM32F40X\n"); 
		INF("STM32: [0x1fff7a22] FLASH_SIZE=0x%04x", memsz);
		target->on_init = (target_script_t)stm32f2xx_on_init,
		mem[FLASH].op = &flash_stm32f2_oper;
		mem[FLASH].blk.size = MEM_KiB(16);
		mem[FLASH].blk.count = memsz / 16;
		mem[EEPROM].blk.count = 0;
		mem[SRAM].blk.count = 112 + 16;
		mem[CCM].blk.count = 64;
		break;

	case 0x419:
		ice_rd16(ice, 0x1fff7a22, &memsz);
		fprintf(f, "STM32F42X\n"); 
		target->on_init = (target_script_t)stm32f2xx_on_init,
		mem[FLASH].op = &flash_stm32f2_oper;
		mem[FLASH].blk.size = MEM_KiB(16);
		mem[FLASH].blk.count = (memsz & 0xffff) / 16;
		mem[EEPROM].blk.count = 0;
		mem[SRAM].blk.count = 112 + 16 + 64;
		mem[CCM].blk.count = 64;
		break;


	case 0x416:
		ice_rd16(ice, 0x1ff8004c, &memsz);
		fprintf(f, "STM32L1XX Cat.1\n"); 
		target->on_init = (target_script_t)stm32l1xx_on_init,
		mem[FLASH].op = &flash_stm32l1_oper;
		mem[FLASH].blk.size = 256;
		mem[FLASH].blk.count = memsz * 4;
		mem[EEPROM].blk.count = 1024;
		if ((memsz == 32) || (memsz == 64))
			mem[SRAM].blk.count = 10;
		else if (memsz == 128)
			mem[SRAM].blk.count = 16;
		mem[CCM].blk.count = 0;
		break;

	case 0x429:
		ice_rd16(ice, 0x1ff8004c, &memsz);
		/* Note: For DEV_ID = 0x429, only LSB part of F_SIZE: F_SIZE[7:0] is 
		   valid. The MSB part F_SIZE[15:8] is reserved and must be ignored. */
		memsz &= 0xff;
		fprintf(f, "STM32L1XX Cat.2\n"); 
		target->on_init = (target_script_t)stm32l1xx_on_init,
		mem[FLASH].op = &flash_stm32l1_oper;
		mem[FLASH].blk.size = 256;
		mem[FLASH].blk.count = memsz * 4;
		mem[EEPROM].blk.count = 1024;
		/* FIXME: configure SRAM */
		mem[SRAM].blk.count = 16;
		mem[CCM].blk.count = 0;
		break;

	case 0x427:
		ice_rd16(ice, 0x1ff800cc, &memsz);
		fprintf(f, "STM32L1XX Cat.3\n"); 
		target->on_init = (target_script_t)stm32l1xx_on_init,
		mem[FLASH].op = &flash_stm32l1_oper;
		mem[FLASH].blk.size = 256;
		mem[FLASH].blk.count = memsz * 4;
		mem[EEPROM].blk.count = 2048;
		mem[SRAM].blk.count = 32;
		mem[CCM].blk.count = 0;
		break;

	case 0x436:
		ice_rd16(ice, 0x1ff800cc, &memsz);
		/* For DEV_ID = 0x436, the field value can be ‘0’ or ‘1’, with ‘0’ 
		   for 384 Kbytes and ‘1’ for 256 Kbytes. */
		memsz = memsz == 0 ? 384 : 256;
		fprintf(f, "STM32L1XX Cat.4\n"); 
		target->on_init = (target_script_t)stm32l1xx_on_init,
		mem[FLASH].op = &flash_stm32l1_oper;
		mem[FLASH].blk.size = 256;
		mem[FLASH].blk.count = memsz * 4;
		mem[EEPROM].blk.count = 3072;
		/* FIXME: configure SRAM */
		mem[SRAM].blk.count = 80;
		mem[CCM].blk.count = 0;
		break;

	case 0x437:
		ice_rd16(ice, 0x1ff800cc, &memsz);
		fprintf(f, "STM32L1XX Cat.5\n"); 
		target->on_init = (target_script_t)stm32l1xx_on_init,
		mem[FLASH].op = &flash_stm32l1_oper;
		mem[FLASH].blk.size = MEM_KiB(16);
		mem[FLASH].blk.count = memsz / 16;
		mem[EEPROM].blk.count = 4096;
		/* FIXME: configure SRAM */
		mem[SRAM].blk.count = 80;
		mem[CCM].blk.count = 0;
		break;

	case 0x422:
		ice_rd16(ice, 0x1ffff7cc, &memsz);
		fprintf(f, "STM32F3XX\n"); 
		/* This bitfield indicates the size of the device Flash memory 
		   expressed in Kbytes.As an example, 0x040 corresponds 
		   to 64 Kbytes. */
		target->on_init = (target_script_t)stm32f3xx_on_init,
		mem[FLASH].op = &flash_stm32f3_oper;
		mem[FLASH].blk.size = MEM_KiB(2);
		mem[FLASH].blk.count = memsz / 2;
		mem[EEPROM].blk.count = 0;
		/* FIXME: configure SRAM */
		mem[SRAM].blk.count = 40;
		mem[CCM].blk.count = 8;
		break;

	case 0x421:
		ice_rd16(ice, 0x1fff7a22, &memsz);
		ice_rd16(ice, 0x1fff7500, &pkg);
		fprintf(f, "STM32F466X\n"); 
		fprintf(f, "   - Package: %s\n", stm32lpkg_name(pkg));
		/* This bitfield indicates the size of the device Flash memory 
		   expressed in Kbytes.As an example, 0x040 corresponds 
		   to 64 Kbytes. */
		target->on_init = (target_script_t)stm32f2xx_on_init,
		mem[FLASH].op = &flash_stm32f2_oper;
		mem[FLASH].blk.size = MEM_KiB(16);
		mem[FLASH].blk.count = memsz / 16;
		mem[EEPROM].blk.count = 0;
		mem[SRAM].blk.count = 128;
		mem[CCM].blk.count = 0;
		break;

	case 0x435:
		ice_rd16(ice, 0x1fff75e0, &memsz);
		ice_rd16(ice, 0x1fff7500, &pkg);
		INF("STM32: [0x1fff75e0] FLASH_SIZE=0x%04x", memsz);
		fprintf(f, "STM32L43XXX or STM32L44XXX\n"); 
		fprintf(f, "   - Package: %s\n", stm32lpkg_name(pkg));
		target->on_init = (target_script_t)stm32l4xx_on_init,
		mem[FLASH].op = &flash_stm32l4_oper;
		mem[FLASH].blk.size = MEM_KiB(2);
		mem[FLASH].blk.count = memsz / 2;
		mem[EEPROM].blk.count = 0;
		mem[SRAM].blk.count = 64;
		mem[CCM].blk.count = 0;
		break;

	case 0x462:
		fprintf(f, "STM32L45XXX or STM32L46XXX\n"); 
		ice_rd16(ice, 0x1fff75e0, &memsz);
		target->on_init = (target_script_t)stm32l4xx_on_init,
		mem[FLASH].op = &flash_stm32l4_oper;
		mem[FLASH].blk.size = MEM_KiB(2);
		mem[FLASH].blk.count = memsz / 2;
		mem[EEPROM].blk.count = 0;
		mem[SRAM].blk.count = 128;
		mem[CCM].blk.count = 0;
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
		.blk = {.count = 2048, .size = MEM_KiB(1)},
		.op = &flash_stm32f1_oper
	},
	{ .name = "eeprom", .flags = 0, 
		.addr = { .base = 0x08080000, .offs = 0 }, 
		.blk = {.count = 0, .size = 4},
		.op = &flash_stm32f1_oper
	},
	{ .name = "sram", .flags = MEM_32_BITS,
		.addr = { .base = 0x20000000, .offs = 0 }, 
		.blk = {.count = 256, .size = MEM_KiB(1)},
		.op = &cm3_ram_oper 
	},
	{ .name = "ccm", .flags = MEM_32_BITS,
		.addr = { .base = 0x10000000, .offs = 0 }, 
		.blk = {.count = 0, .size = MEM_KiB(1)},
		.op = &cm3_ram_oper 
	},
	{ .name = "sys", .flags = MEM_32_BITS,
		.addr = { .base = 0x40000000, .offs = 0x00000000 }, 
		.blk = {.count = 0x10000, .size = 0x1000},
		.op = &cm3_ram_oper 
	},
	{ .name = "scs", .flags = MEM_32_BITS,
		.addr = { .base = 0xe000e000, .offs = 0x00000000 }, 
		.blk = {.count = 52, .size = 0x1000},
		.op = &cm3_ram_oper 
	},
	{ .name = "dbg", .flags = MEM_32_BITS,
		.addr = { .base = 0xe0042000, .offs = 0x00000000 }, 
		.blk = {.count = 62, .size = 0x2000},
		.op = &cm3_ram_oper 
	},
	{ .name = "id", .flags = MEM_32_BITS | MEM_RO,
		.addr = { .base = 0x1fff0000, .offs = 0x00000000 }, 
		.blk = {.count = 64, .size = 0x400},
		.op = &cm3_ram_oper 
	},
	{ .name = "", .flags = 0, .addr = { .base = 0, .offs = 0 }, 
		.blk = {.count = 0, .size = 0},
		.op = NULL
	}
};

struct target_info stm32f = {
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
	.reset_on_config = NO,
	/* connect_on_reset*/
	.connect_on_reset = YES,

	/* The prefered reset method is system */
	.reset_mode = RST_SYS,

	.start_addr = 0x00000000,

	.pos_config = (target_config_t)stm32f_pos_config,
	.on_init = (target_script_t)stm32f1xx_on_init,
	.reset_script = (target_script_t)cm3_reset,
	.probe = (target_script_t)cm3_probe,
};

