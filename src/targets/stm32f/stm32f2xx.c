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
 * @file stm32f2xx.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 


#define TRACE_LEVEL TRACE_LVL_DBG
#include <trace.h>

#include "target/stm32f.h"

#include <stdio.h>
#include <sys/dcclog.h>

#define STM32F2X
#include "stm32f_defs.h"

int stm32f2xx_flash_erase(cm3ice_ctrl_t * ctrl, 
						  ice_mem_ref_t * addr, 
						  ice_size_t len)
{
	uint32_t sr;
	uint32_t acr;
	uint32_t cr;
	int again;
	int page;
	int sect;
	int size;

	page = addr->offs >> 14;
	switch (page) {
	case 0 ... 3:
		sect = page;
		size = 16384;
		break;
	case 4 ... 7:
		sect = 4;
		size = 65536;
		break;
	default:
		sect = ((page - 1) / 8) + 5;
		size = 131072;
		break;
	}

	DCC_LOG4(LOG_TRACE, "len=%d page=%d sect=%d size=%d", 
			 len, page, sect, size);

	cm3ice_rd32(ctrl, STM32F_BASE_FLASH + FLASH_CR, &cr);
	if (cr & LOCK) {
		DCC_LOG(LOG_TRACE, "unlocking flash...");
		/* unlock flash write */
		cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_KEYR, FLASH_KEY1);
		cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_KEYR, FLASH_KEY2);
	}

	cm3ice_rd32(ctrl, STM32F_BASE_FLASH + FLASH_ACR, &acr);
	/* Disable cache & Flush cache */
	cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_ACR, 
				(acr & ~(DCEN | ICEN)) | DCRST | ICRST);


	cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_CR, SER | SNB(sect) | STRT);

	for (again = 4096 * 32; ; again--) {
		cm3ice_rd32(ctrl, STM32F_BASE_FLASH + FLASH_SR, &sr);
		if ((sr & BSY) == 0)
			break;
		if (again == 0) {
			DCC_LOG(LOG_WARNING, "flash not ready!");
			return -1;
		}
	}

	/* Restore ACR */
	cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_ACR, acr);

	return size;
}

int stm32f2_flash_wr32(cm3ice_ctrl_t * ctrl, uint32_t addr, uint32_t data)
{
	uint32_t sr;
	int again;

	DCC_LOG2(LOG_INFO, "0x%08x <-- 0x%08x", addr, data);

	cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_CR, PSIZE_32 | PG);
	cm3ice_wr32(ctrl, addr, data);
	
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

int stm32f2xx_flash_write(cm3ice_ctrl_t * ctrl, ice_mem_ref_t * mem, 
					 const void * buf, ice_size_t len)
{
	uint32_t data;
	uint32_t addr;
	uint8_t * ptr;
	uint32_t cr;
	int n;
	int i;

	n = (len + 3) / 4;

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
		data = ptr[0] | (ptr[1] << 8) | (ptr[2] << 16) | (ptr[3] << 24);
		stm32f2_flash_wr32(ctrl, addr, data);
		ptr += 4;
		addr += 4;
	}

	return n * 4;
}

/*
 * stm32f2xx flash
 */
const struct ice_mem_oper flash_stm32f2_oper = {
	.read = (ice_mem_read_t)cm3ice_mem_read,
	.write = (ice_mem_write_t)stm32f2xx_flash_write, 
	.erase = (ice_mem_erase_t)stm32f2xx_flash_erase
};

int stm32f2xx_on_init(FILE * f, const ice_drv_t * ice, 
					 ice_mem_entry_t * mem)
{
	uint32_t sr;
	uint32_t cr;
	uint32_t pll;
	uint32_t cfg;
	int again;

	ice_rd32(ice, STM32F_BASE_RCC + RCC_CFGR, &cfg);
	ice_rd32(ice, STM32F_BASE_RCC + RCC_CR, &cr);
	DCC_LOG2(LOG_TRACE, "cr=0x%08x cfg=0x%08x.", cr, cfg);

	INF("STM32: cr=0x%08x cfg=0x%08x.", cr, cfg);

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
					WARN("internal oscillator fail!");
					return -1;
				}
			}
		}

		/* switch to internal oscillator */
		cfg = MCO2_SYSCLK | MCO2PRE_2 | PPRE2_4 | PPRE1_8 | HPRE_1 | SW_HSI;
		ice_wr32(ice, STM32F_BASE_RCC + RCC_CFGR, cfg);
		DCC_LOG(LOG_TRACE, "switched to internal oscillator.");
	} else {
		if (!(cr & HSIRDY)) {
			fprintf(f, " %s: internal oscillator not ready!\n", 
					__func__);
			DCC_LOG(LOG_WARNING, "internal oscillator not ready!");
			WARN("internal oscillator not ready!");
			return -1;
		}	
	}

	/*
	   F_HSI = 16 MHz
	   F_VCO = 480 MHz
	   F_MAIN = 120 MHz
	   F_USB = 48 MHz
	 */
	pll = PLLSRC_HSI | PLLM(8) | PLLN(240) | PLLP(4) | PLLQ(10);
	ice_wr32(ice, STM32F_BASE_RCC + RCC_PLLCFGR, pll);

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
			WARN("PLL lock fail!");
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
			WARN("flash not ready!");
			return -1;
		}
	}

	/* adjust flash wait states and enable caches */
	ice_wr32(ice, STM32F_BASE_FLASH + FLASH_ACR, PRFTEN | LATENCY(7));

	ice_rd32(ice, STM32F_BASE_FLASH + FLASH_CR, &cr);
	if (cr & LOCK) {
		fprintf(f, " %s: unlocking flash\n", __func__);
		/* unlock flash write */
		ice_wr32(ice, STM32F_BASE_FLASH + FLASH_KEYR, FLASH_KEY1);
		ice_wr32(ice, STM32F_BASE_FLASH + FLASH_KEYR, FLASH_KEY2);
	}

	/* switch to pll oscillator */
	cfg = MCO2_SYSCLK | MCO2PRE_2 | PPRE2_4 | PPRE1_8 | HPRE_1 | SW_PLL;
	ice_wr32(ice, STM32F_BASE_RCC + RCC_CFGR, cfg);


	return 0;
}

const struct ice_mem_entry stm32f20xxb_mem[] = {
	{ .name = "flash", .flags = MEM_32_BITS,
		.addr = { .base = 0x00000000, .offs = 0 }, 
		.blk = {.count = 128, .size = MEM_KiB(1)},
		.op = &flash_stm32f2_oper
	},
	{ .name = "sram", .flags = MEM_32_BITS,
		.addr = { .base = 0x20000000, .offs = 0 }, 
		.blk = {.count = 8, .size = MEM_KiB(16)},
		.op = &cm3_ram_oper 
	},
	{ .name = "", .flags = 0, .addr = { .base = 0, .offs = 0 }, 
		.blk = {.count = 0, .size = 0},
		.op = NULL
	}
};

const struct ice_mem_entry stm32f20xxc_mem[] = {
	{ .name = "flash", .flags = MEM_32_BITS,
		.addr = { .base = 0x08000000, .offs = 0 }, 
		.blk = {.count = 256, .size = MEM_KiB(1)},
		.op = &flash_stm32f2_oper
	},
	{ .name = "sram0", .flags = MEM_32_BITS,
		.addr = { .base = 0x20000000, .offs = 0 }, 
		.blk = {.count = 1, .size = MEM_KiB(112)},
		.op = &cm3_ram_oper 
	},
	{ .name = "sram1", .flags = MEM_32_BITS,
		.addr = { .base = 0x20000000, .offs = MEM_KiB(112) }, 
		.blk = {.count = 1, .size = MEM_KiB(16)},
		.op = &cm3_ram_oper 
	},
	{ .name = "", .flags = 0, .addr = { .base = 0, .offs = 0 }, 
		.blk = {.count = 0, .size = 0},
		.op = NULL
	}
};

const struct ice_mem_entry stm32f20xxe_mem[] = {
	{ .name = "boot", .flags = MEM_32_BITS,
		.addr = { .base = 0x00000000, .offs = 0 }, 
		.blk = {.count = 64, .size = MEM_KiB(64)},
		.op = &cm3_ram_oper
	},
	{ .name = "flash", .flags = MEM_32_BITS,
		.addr = { .base = 0x08000000, .offs = 0 }, 
		.blk = {.count = 32, .size = MEM_KiB(16)},
		.op = &flash_stm32f2_oper
	},
	{ .name = "sram0", .flags = MEM_32_BITS,
		.addr = { .base = 0x20000000, .offs = 0 }, 
		.blk = {.count = 1, .size = MEM_KiB(112)},
		.op = &cm3_ram_oper 
	},
	{ .name = "sram1", .flags = MEM_32_BITS,
		.addr = { .base = 0x20000000, .offs = MEM_KiB(112) }, 
		.blk = {.count = 1, .size = MEM_KiB(16)},
		.op = &cm3_ram_oper 
	},
	{ .name = "bkpsram", .flags = MEM_32_BITS,
		.addr = { .base = 0x40000000, .offs = 0x00024000 }, 
		.blk = {.count = 1, .size = MEM_KiB(4)},
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

