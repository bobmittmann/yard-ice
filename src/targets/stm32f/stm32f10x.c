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
 * @file stm32f10x..c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdlib.h>
#include <sys/dcclog.h>

#include "target.h"
#include "script.h"
#include "cm3ice.h"
#include "target/stm32f.h"

#define STM32F10X
#include "stm32f_defs.h"

int stm32f1_flash_erase(cm3ice_ctrl_t * ctrl, 
					   ice_mem_ref_t * mem, 
					   ice_size_t len)
{
	uint32_t sr;
	uint32_t addr;
	uint32_t cr;
	int again;

	addr = mem->base + mem->offs;

	DCC_LOG2(LOG_TRACE, "addr=0x%08x len=%d", addr, len);

	cm3ice_rd32(ctrl, STM32F_BASE_FLASH + FLASH_CR, &cr);
	if (cr & LOCK) {
		DCC_LOG(LOG_TRACE, "unlocking flash...");
		/* unlock flash write */
		cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_KEYR, FLASH_KEY1);
		cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_KEYR, FLASH_KEY2);
	}

	cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_CR, PER);

	cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_AR, addr);

	cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_CR, STRT | PER);

	for (again = 4096 * 32; ; again--) {
		cm3ice_rd32(ctrl, STM32F_BASE_FLASH + FLASH_SR, &sr);
		if ((sr & BSY) == 0)
			break;
		if (again == 0) {
			DCC_LOG(LOG_WARNING, "flash not ready!");
			return -1;
		}
	}

	return len;
}

int stm32f10x_flash_wr16(cm3ice_ctrl_t * ctrl, uint32_t addr, uint16_t data)
{
	uint32_t sr;
	int again;

	DCC_LOG2(LOG_INFO, "0x%08x <-- 0x%08x", addr, data);

	cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_CR, PG);
	cm3ice_wr16(ctrl, addr, data);
	
	for (again = 4096; ; again--) {
		cm3ice_rd32(ctrl, STM32F_BASE_FLASH + FLASH_SR, &sr);
		if ((sr & BSY) == 0)
			break;
		if (again == 0) {
			DCC_LOG(LOG_WARNING, "flash not ready!");
			return -1;
		}
	}

	return 0;
}

int stm32f1_flash_write(cm3ice_ctrl_t * ctrl, ice_mem_ref_t * mem, 
					 const void * buf, ice_size_t len)
{
	uint16_t data;
	uint32_t addr;
	uint8_t * ptr;
	uint32_t cr;
	int n;
	int i;

	n = (len + 1) / 2;

	ptr = (uint8_t *)buf;
	addr = mem->base + mem->offs;

	cm3ice_rd32(ctrl, STM32F_BASE_FLASH + FLASH_CR, &cr);
	if (cr & LOCK) {
		DCC_LOG(LOG_TRACE, "unlocking flash...");
		/* unlock flash write */
		cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_KEYR, FLASH_KEY1);
		cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_KEYR, FLASH_KEY2);
	}


	DCC_LOG2(LOG_TRACE, "0x%08x len=%d", addr, len);

	for (i = 0; i < n; i++) {
		data = ptr[0] | (ptr[1] << 8);
		stm32f10x_flash_wr16(ctrl, addr, data);
		ptr += 2;
		addr += 2;
	}
	
	return n * 2;
}

/*
 * stm32f1xxx flash
 */
const struct ice_mem_oper flash_stm32f1_oper = {
	.read = (ice_mem_read_t)cm3ice_mem_read,
	.write = (ice_mem_write_t)stm32f1_flash_write, 
	.erase = (ice_mem_erase_t)stm32f1_flash_erase
};


int stm32f1xx_on_init(FILE * f, const ice_drv_t * ice, 
					 ice_mem_entry_t * mem)
{
	uint32_t sr;
	uint32_t cr;
	uint32_t cfg;
	int again;

	ice_rd32(ice, STM32F_BASE_RCC + RCC_CFGR, &cfg);
	ice_rd32(ice, STM32F_BASE_RCC + RCC_CR, &cr);
	DCC_LOG2(LOG_TRACE, "cr=0x%08x cfg=0x%08x.", cr, cfg);

	switch (cfg & SWS) {
	case SWS_HSI:
		fprintf(f, " %s: system clock is internal\n", __func__);
		DCC_LOG(LOG_TRACE, "system clock is internal");
		break;
	case SWS_HSE:
		fprintf(f, " %s: system clock is external\n", __func__);
		DCC_LOG(LOG_TRACE, "system clock is external");
		break;
	case SWS_PLL:
		fprintf(f, " %s: system clock is PLL\n", __func__);
		DCC_LOG(LOG_TRACE, "system clock is PLL");
		break;
	default:
		fprintf(f, " %s: system clock switch invalid!\n", __func__);
		DCC_LOG(LOG_WARNING, "system clock switch invalid!");
		return -1;
	}

	if ((cfg & SWS) != SWS_HSI) {
		if (!(cr & HSION)) {
			/* enable internal oscillator */
			DCC_LOG(LOG_TRACE, "enabling internal oscillator...");
			cr |= HSION;
			ice_wr32(ice, STM32F_BASE_RCC + RCC_CR, cr);

			for (again = 256; ; again--) {
				ice_rd32(ice, STM32F_BASE_RCC + RCC_CR, &cr);
				if (cr & HSIRDY)
					break;
				if (again == 0) {
					fprintf(f, " %s: internal oscillator startup fail!\n", 
							__func__);
					DCC_LOG(LOG_WARNING, "internal oscillator fail!");
					return -1;
				}
			}
		}

		/* switch to internal oscillator */
		cfg = PPRE2_1 | PPRE1_1 | HPRE_1 | SW_HSI;
		ice_wr32(ice, STM32F_BASE_RCC + RCC_CFGR, cfg);
		DCC_LOG(LOG_TRACE, "swithed to internal oscillator.");
	} else {
		if (!(cr & HSIRDY)) {
			fprintf(f, " %s: internal oscillator not ready!\n", 
					__func__);
			DCC_LOG(LOG_WARNING, "internal oscillator not ready!");
			return -1;
		}	
	}


	cr &= ~PLLON;
	ice_wr32(ice, STM32F_BASE_RCC + RCC_CR, cr);

	/* Configure PLL
	   F_HSI = 4 MHz
	   F_VCO = 24 MHz
	   F_MAIN = 24 MHz
	 */
	cfg = PLLMUL(6) | PLLSRC_HSI | PPRE2_1 | PPRE1_1 | HPRE_1 | SW_HSI;
	ice_wr32(ice, STM32F_BASE_RCC + RCC_CFGR, cfg);

	/* enable PLL */
	cr |= PLLON;
	ice_wr32(ice, STM32F_BASE_RCC + RCC_CR, cr);

	for (again = 256; ; again--) {
		ice_rd32(ice, STM32F_BASE_RCC + RCC_CR, &cr);
		if (cr & PLLRDY)
			break;
		if (again == 0) {
			fprintf(f, " %s: PLL lock fail!\n", __func__);
			DCC_LOG(LOG_WARNING, "PLL lock fail!");
			return -1;
		}
	}

	for (again = 4096; ; again--) {
		ice_rd32(ice, STM32F_BASE_FLASH + FLASH_SR, &sr);
		if ((sr & BSY) == 0)
			break;
		if (again == 0) {
			fprintf(f, " %s: flash not ready!\n", __func__);
			DCC_LOG(LOG_WARNING, "flash not ready!");
			return -1;
		}
	}

	/* adjust flash wait states and enable caches */
	ice_wr32(ice, STM32F_BASE_FLASH + FLASH_ACR, PRFTBE | LATENCY(0));

	ice_rd32(ice, STM32F_BASE_FLASH + FLASH_CR, &cr);

	if (cr & LOCK) {
		fprintf(f, " %s: unlocking flash\n", __func__);
		/* unlock flash write */
		ice_wr32(ice, STM32F_BASE_FLASH + FLASH_KEYR, FLASH_KEY1);
		ice_wr32(ice, STM32F_BASE_FLASH + FLASH_KEYR, FLASH_KEY2);
	}

	/* switch to pll oscillator */
	cfg = (cfg & ~SW) | SW_PLL;
	ice_wr32(ice, STM32F_BASE_RCC + RCC_CFGR, cfg);

	return 0;
}

/* STM32F Medium density memory map */
const struct ice_mem_entry stm32f103med_mem[] = {
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
		.blk = {.count = 1, .size = MEM_KiB(20)},
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

const struct target_info stm32f10x = {
	.name = "stm32f10x",
	.arch = &stm32f_arch,
	.mem = (struct ice_mem_entry *)stm32f103med_mem,

	.ice_drv = &cm3ice_drv,
	.ice_cfg = (void *)&stm32f_cfg,

	.jtag_clk_slow = 4000000,
	.jtag_clk_def = 8000000,
	.jtag_clk_max = 8000000,

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
	/* auto probe scan path */
	.jtag_probe = YES,
	/* hardware reset before ICE configure */
	.reset_on_config = YES,

	/* The preferred reset method is core only */
	.reset_mode = RST_CORE,

	.start_addr = 0x00000000,

	.on_init = (target_script_t)stm32f1xx_on_init,
	.on_halt = (target_script_t)NULL,
	.on_run = NULL,
	.reset_script = (target_script_t)cm3_reset,
	.probe = (target_script_t)cm3_probe,
	.test = (target_test_t)NULL
};

