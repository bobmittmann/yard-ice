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
 * @file cm3.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <crc.h>
#include <stdlib.h>
#include <sys/dcclog.h>

#include "target.h"
#include "armice.h"
#include "script.h"
#include "target/cm3.h"
#include "cm3ice.h"

int cm3_reset(FILE * f, const ice_drv_t * ice, ice_mem_entry_t * mem)
{
#if 0
	uint32_t aircr;

	ice_rd32(ice, ARMV7M_AIRCR, &aircr);

	if (AIRCR_VECTKEY_DEC(aircr) != AIRCR_VECTKEYSTAT) {
		fprintf(f, "#ERROR: AIRCR @ 0x%08x --> 0x%08x!\n", 
				ARMV7M_AIRCR, aircr);
		return -1;
	}
#endif

//	fprintf(f, " - System reset request...");
//	ice_wr32(ice, ARMV7M_AIRCR, AIRCR_VECTKEY | AIRCR_SYSRESETREQ);
	fprintf(f, " - ARMv7-M Local reset ...");
	/* Enable entering debug state on leaving reset state */
	ice_wr32(ice, ARMV7M_DHCSR, DHCSR_DBGKEY | DHCSR_C_DEBUGEN);
	ice_wr32(ice, ARMV7M_DEMCR, DEMCR_VC_CORERESET);
	/* Local reset */
	ice_wr32(ice, ARMV7M_AIRCR, AIRCR_VECTKEY | AIRCR_VECTRESET);

	fprintf(f, "\n");

	return 0;
}

int cm3_probe(FILE * f, const ice_drv_t * ice, ice_mem_entry_t * mem)
{
	uint32_t cpuid;
	int ret;

	if ((ret = ice_rd32(ice, ARMV7M_CPUID, &cpuid) < 0))
		return ret;

	fprintf(f, " - CPUID(0x%08x): 0x%08x\n", ARMV7M_CPUID, cpuid);

	if (cpuid == 0x411fc231)
		return 1;

	return 0;
}

const struct target_cpu cortex_m3_cpu = {
	.family = "Cortex-M3",
	.model = "v7t",
	.vendor = "ARM",
	.irlength = 4,
	.idmask = 0x0fffffff,
	.idcomp = 0x0ba00477,
//	.idmask = 0xffffffff,
//	.idcomp = 0x3ba00477,
	.sym = NULL
};

const struct cm3ice_cfg cortex_m3_cfg = {
	.endianness = LITTLE_ENDIAN,
};

const struct ice_mem_entry cortex_m3_mem[] = {
	{ .name = "rom", .flags = MEM_32_BITS,
		.addr = { .base = 0x00000000, .offs = 0} , 
		.blk = {.count = 1024, .size = MEM_KiB(1)},
		.op = &cm3_ram_oper
	},
	{ .name = "sram", .flags = MEM_32_BITS,
		.addr = { .base = 0x20000000, .offs = 0} , 
		.blk = {.count = 1024, .size = MEM_KiB(1)},
		.op = &cm3_ram_oper 
	},
	{ .name = "", .flags = 0, 
		.addr = { .base = 0x00000000, .offs = 0} , 
		.blk = {.count = 0, .size = 0},
		.op = NULL
	}
};

const struct target_arch cortex_m3_arch = {
	.name = "cortex-m3",
	.model = "generic",
	.vendor = "bobmittmann",
	.cpu = &cortex_m3_cpu,
	.sym = void_sym
};

const struct target_info cortex_m3 = {
	.name = "cortex-m3",
	.arch = &cortex_m3_arch,
	.mem = (struct ice_mem_entry *)cortex_m3_mem,

	.ice_drv = &cm3ice_drv,
	.ice_cfg = (void *)&cortex_m3_cfg,

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

	.reset_script = (target_script_t)cm3_reset,
	.probe = (target_script_t)cm3_probe,
};

