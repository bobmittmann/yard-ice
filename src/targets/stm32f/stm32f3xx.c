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
 * @file stm32l1xx..c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdlib.h>
#include <sys/dcclog.h>

#include "target.h"
#include "script.h"
#include "cm3ice.h"
#include "trace.h"
#include "target/stm32f.h"

#define STM32F10X
#include "stm32f_defs.h"


/*
 * stm32l1xxx flash
 */
const struct ice_mem_oper flash_stm32f3_oper = {
	.read = (ice_mem_read_t)cm3ice_mem_read,
	.write = (ice_mem_write_t)stm32f1_flash_write, 
	.erase = (ice_mem_erase_t)stm32f1_flash_erase
};

#define OPTION_BYTE_BASE 0x1ff80000

int stm32f3xx_on_init(FILE * f, const ice_drv_t * ice, 
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

