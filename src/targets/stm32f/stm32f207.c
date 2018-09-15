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
 * @file stm32f207.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 


#include "target/stm32f.h"

#include <stdio.h>
#include <sys/dcclog.h>

#define STM32F2X
#include "stm32f_defs.h"

int stm32f207_on_init(FILE * f, const ice_drv_t * ice, 
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

	switch (cfg & SWS) {
	case SWS_HSI:
		fprintf(f, " %s: system clock is internal\n", __func__);
		break;
	case SWS_HSE:
		fprintf(f, " %s: system clock is external\n", __func__);
		break;
	case SWS_PLL:
		fprintf(f, " %s:  system clock is PLL\n", __func__);
		break;
	}

	if ((cfg & SWS) != SWS_HSI) {
		if (!(cr & HSION)) {
			/* enable internal oscillator */
			cr = HSION;
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
		cfg = MCO2_SYSCLK | MCO2PRE_2 | PPRE2_4 | PPRE1_8 | HPRE_1 | SW_HSI;
		ice_wr32(ice, STM32F_BASE_RCC + RCC_CFGR, cfg);
		DCC_LOG(LOG_TRACE, "switched to internal oscillator.");
	} else {
		if (!(cr & HSIRDY)) {
			fprintf(f, " %s: internal oscillator not ready!\n", 
					__func__);
			DCC_LOG(LOG_WARNING, "internal oscillator not ready!");
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
//	ice_wr32(ice, STM32F_BASE_FLASH + FLASH_ACR, 
//			 DCEN | ICEN | PRFTEN | LATENCY(7));

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


const struct target_info stm32f207ve = {
	.name = "stm32f207ve",
	.arch = &stm32f_arch,
	.ice_drv = &cm3ice_drv,
	.ice_cfg = (void *)&stm32f_cfg,

	.mem = (struct ice_mem_entry *)stm32f20xxe_mem,

	.jtag_clk_slow = 12000000,
	.jtag_clk_def = 12000000,
	.jtag_clk_max = 16000000,

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


	.on_init = (target_script_t)stm32f207_on_init,
	.reset_script = (target_script_t)cm3_reset,
	.probe = (target_script_t)cm3_probe
};

