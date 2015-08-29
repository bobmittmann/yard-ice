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

#define STM32L1XX
#include "stm32l_defs.h"

#define FERR (RDERR | OPTVERRUSR | OPTVERR | SIZERR | PGAERR | WRPERR)

int stm32l1xx_flash_bsy_wait(cm3ice_ctrl_t * ctrl)
{
	int ret = -1;
	uint32_t sr;
	int again;

	for (again = 4096; again > 0; again--) {
		cm3ice_rd32(ctrl, STM32F_BASE_FLASH + FLASH_SR, &sr);
		if (sr & FERR) {
			DCC_LOG6(LOG_WARNING, "%s%s%s%s%s%s", 
					 sr & RDERR ? "RDERR" : "",
					 sr & OPTVERRUSR ? "OPTVERRUSR" : "",
					 sr & OPTVERR ? "OPTVERR " : "",
					 sr & SIZERR ? "SIZERR " : "",
					 sr & PGAERR ? "PGAERR" : "",
					 sr & WRPERR ? "WRPERR" : "");
			break;
		}
		if ((sr & BSY) == 0) {
			ret = 0;
			break;
		}
	}

	return ret;
}


int stm32l1_flash_erase(cm3ice_ctrl_t * ctrl, 
					   ice_mem_ref_t * mem, 
					   ice_size_t len)
{
	uint32_t pecr;
	uint32_t addr;

	addr = mem->base + mem->offs;

	DCC_LOG2(LOG_TRACE, "addr=0x%08x len=%d", addr, len);

	cm3ice_rd32(ctrl, STM32F_BASE_FLASH + FLASH_PECR, &pecr);
	DCC_LOG1(LOG_TRACE, "PECR=0x%08x", pecr);
	if (pecr & PRGLOCK) {
		DCC_LOG(LOG_TRACE, "unlocking flash...");
		if (pecr & PELOCK) {
			cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_PEKEYR, PEKEY1);
			cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_PEKEYR, PEKEY2);
		}
		cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_PRGKEYR, PRGKEYR1);
		cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_PRGKEYR, PRGKEYR2);
	}

	cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_PECR, ERASE | PROG);

	cm3ice_wr32(ctrl, addr, 0x00000000);

	if (stm32l1xx_flash_bsy_wait(ctrl) < 0)
		len = -1;

	cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_PECR, 0);

	return len;
}


int stm32l1_flash_write(cm3ice_ctrl_t * ctrl, ice_mem_ref_t * mem, 
					 const void * buf, ice_size_t len)
{
	uint32_t pecr;
	uint32_t data;
	uint8_t * ptr;
	uint32_t offs;
	uint32_t base;
	int rem;
	int ret;
#if 0
	uint32_t ts;
	int32_t dt;
#endif

	ptr = (uint8_t *)buf;
	offs = mem->offs;
	base = mem->base;
	
	cm3ice_rd32(ctrl, STM32F_BASE_FLASH + FLASH_PECR, &pecr);
	DCC_LOG1(LOG_TRACE, "PECR=0x%08x", pecr);
	if (pecr & PRGLOCK) {
		DCC_LOG(LOG_TRACE, "unlocking flash...");
		if (pecr & PELOCK) {
			cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_PEKEYR, PEKEY1);
			cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_PEKEYR, PEKEY2);
		}
		cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_PRGKEYR, PRGKEYR1);
		cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_PRGKEYR, PRGKEYR2);
	}

	DCC_LOG2(LOG_TRACE, "0x%08x len=%d", base + offs, len);

#if 0
	ts = trace_timestamp();
#endif

	len = (len + 3) & ~0x00000003;
	rem = len;

#if 0
	if ((offs & 0x7f) != 0) {
		if ((ret = stm32l1xx_flash_bsy_wait(ctrl)) < 0)
			return ret;
	}
#endif
	while (rem > 0) {
		if ((offs & 0x7f) == 0) { 
			if (rem >= 128) {
				/* start half page write */
				DCC_LOG(LOG_TRACE, "Half-Page write start...");
				cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_PECR, FPRG | PROG);
			} else {
				DCC_LOG(LOG_TRACE, "Partial block start...");
				cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_PECR, 0);
			}
			if ((ret = stm32l1xx_flash_bsy_wait(ctrl)) < 0)
				return ret;
		}
		data = ptr[0] | (ptr[1] << 8) | (ptr[2] << 16)| (ptr[3] << 24);
		cm3ice_wr32(ctrl, base + offs, data);
		ptr += 4;
		offs += 4;
		rem -= 4;
	}

	/* Clear flash program operation */
	cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_PECR, 0);

	return len;
}

/*
 * stm32l1xxx flash
 */
const struct ice_mem_oper flash_stm32l1_oper = {
	.read = (ice_mem_read_t)cm3ice_mem_read,
	.write = (ice_mem_write_t)stm32l1_flash_write, 
	.erase = (ice_mem_erase_t)stm32l1_flash_erase
};

#define OPTION_BYTE_BASE 0x1ff80000

int stm32l1xx_on_init(FILE * f, const ice_drv_t * ice, 
					 ice_mem_entry_t * mem)
{
//	uint32_t sr;
	uint32_t pecr;
	uint32_t obr;
	uint32_t opt;
	uint32_t cr;
	uint32_t cfg;
	int again;

	ice_rd32(ice, STM32F_BASE_RCC + RCC_CFGR, &cfg);
	ice_rd32(ice, STM32F_BASE_RCC + RCC_CR, &cr);
	DCC_LOG2(LOG_TRACE, "cr=0x%08x cfg=0x%08x.", cr, cfg);

	switch (cfg & SWS) {
	case SWS_MSI:
		fprintf(f, " %s: system clock is RC internal (MSI)\n", __func__);
		DCC_LOG(LOG_TRACE, "system clock is internal");
		break;
	case SWS_HSI:
		fprintf(f, " %s: system clock is internal (HSI)\n", __func__);
		DCC_LOG(LOG_TRACE, "system clock is internal");
		break;
	case SWS_HSE:
		fprintf(f, " %s: system clock is external (HSE)\n", __func__);
		DCC_LOG(LOG_TRACE, "system clock is external");
		break;
	case SWS_PLL:
		fprintf(f, " %s: system clock is PLL\n", __func__);
		DCC_LOG(LOG_TRACE, "system clock is PLL");
		break;
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
					fprintf(f, " %s: HSI oscillator startup fail!\n", 
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
			fprintf(f, " %s: HSI oscillator not ready!\n", 
					__func__);
			DCC_LOG(LOG_WARNING, "internal oscillator not ready!");
			return -1;
		}	
	}


	/*******************************************************************
	 * Adjust core voltage regulator
	 *******************************************************************/
	ice_wr32(ice, STM32F_BASE_PWR + PWR_CR, VOS_1_8V);
	/* wait for voltage to stabilize */
	for (again = 8192; ; again--) {
		uint32_t csr;

		ice_rd32(ice, STM32F_BASE_PWR + PWR_CSR, &csr);
		if ((csr & VOSF) == 0)
			break;
		if (again == 0) {
			fprintf(f, " %s: voltage regulator fail!\n", __func__);
			DCC_LOG(LOG_WARNING, "voltage regulator fail!");
			return -1;
		}
	}

	ice_rd32(ice, STM32F_BASE_FLASH + FLASH_PECR, &pecr);
	DCC_LOG1(LOG_TRACE, "PECR=0x%08x", pecr);
	if (pecr & PRGLOCK) {
		if (pecr & PELOCK) {
			fprintf(f, " %s: unlocking PECR register\n", __func__);
			ice_wr32(ice, STM32F_BASE_FLASH + FLASH_PEKEYR, PEKEY1);
			ice_wr32(ice, STM32F_BASE_FLASH + FLASH_PEKEYR, PEKEY2);
		}
		fprintf(f, " %s: unlocking program memory\n", __func__);
		ice_wr32(ice, STM32F_BASE_FLASH + FLASH_PRGKEYR, PRGKEYR1);
		ice_wr32(ice, STM32F_BASE_FLASH + FLASH_PRGKEYR, PRGKEYR2);
	}
#if 0
	if (pecr & OPTLOCK) {
		fprintf(f, " %s: unlocking otion byte block \n", __func__);
		ice_wr32(ice, STM32F_BASE_FLASH + FLASH_OPTKEYR, OPTKEY1);
		ice_wr32(ice, STM32F_BASE_FLASH + FLASH_OPTKEYR, OPTKEY2);
		ice_rd32(ice, STM32F_BASE_FLASH + FLASH_PECR, &pecr);
	}
#endif

	ice_rd32(ice, STM32F_BASE_FLASH + FLASH_OBR, &obr);
	DCC_LOG1(LOG_TRACE, "OBR=0x%08x", obr);
	ice_rd32(ice, OPTION_BYTE_BASE + 0, &opt);
	DCC_LOG1(LOG_TRACE, "OPT[0]=0x%08x", opt);
	ice_rd32(ice, OPTION_BYTE_BASE + 4, &opt);
	DCC_LOG1(LOG_TRACE, "OPT[1]=0x%08x", opt);

	/* adjust flash wait states and enable caches */
	ice_wr32(ice, STM32F_BASE_FLASH + FLASH_ACR, PRFTEN);

#if 0
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
#endif

	return 0;
}

