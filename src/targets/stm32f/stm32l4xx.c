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
#define LOG_LEVEL LOG_INFO
#include <sys/dcclog.h>

#define TRACE_LEVEL TRACE_LVL_DBG
#include <trace.h>

#include "target.h"
#include "script.h"
#include "cm3ice.h"
#include "target/stm32f.h"

#define STM32L4XX
#include "stm32l4_defs.h"

/*
 * stm32l4xxx flash
 */

#define FERR (OPTVERR | RDERR | FASTERR | MISERR | PGSERR | PGSIZERR | \
			  PGAERR | WRPERR | PROGERR | OPERR)

int stm32l4xx_flash_bsy_wait(cm3ice_ctrl_t * ctrl)
{
	uint32_t sr;
	int again;

	for (again = 4096; again > 0; again--) {
		cm3ice_rd32(ctrl, STM32F_BASE_FLASH + FLASH_SR, &sr);
		if (sr & FERR) {
			/* clear errors */
			cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_SR, FERR);
			WARN("FLASH error:%s%s%s%s%s%s%s%s%s%s", 
					 sr & OPTVERR ? " OPTVERR" : "",
					 sr & RDERR ? " RDERR" : "",
					 sr & FASTERR ? " FASTERR" : "",
					 sr & MISERR ? " MISERR" : "",
					 sr & PGSERR ? " PGSERR" : "",
					 sr & PGSIZERR ? " PGSIZERR" : "",
					 sr & PGAERR ? " PGAERR" : "",
					 sr & WRPERR ? " WRPER" : "",
					 sr & PROGERR ? " PROGERR" : "",
					 sr & OPERR ? " OPERR" : "");
			return sr & FERR;
		}
		if ((sr & BSY) == 0) {
			return 0;
		}
	}

	return -1;
}

void stm32l4xxx_flash_clr(cm3ice_ctrl_t * ctrl)
{
	uint32_t sr;
	uint32_t cr;
	
	cm3ice_rd32(ctrl, STM32F_BASE_FLASH + FLASH_CR, &cr);
	if (cr & LOCK) {
		DCC_LOG(LOG_TRACE, "unlocking flash...");
		INF("unlocking flash...");
		/* unlock flash write */
		cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_KEYR, FLASH_KEY1);
		cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_KEYR, FLASH_KEY2);
	}

	cm3ice_rd32(ctrl, STM32F_BASE_FLASH + FLASH_SR, &sr);
	if (sr & FERR) {
		INF("Clearing errors...");
		cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_SR, FERR);
	}
}

/*
Page erase
To erase a page (2 Kbyte), follow the procedure below:
1. Check that no Flash memory operation is ongoing by checking the BSY bit in the Flash
status register (FLASH_SR).
2. Check and clear all error programming flags due to a previous programming. If not,
	PGSERR is set.
	3. Set the PER bit and select the page you wish to erase (PNB) in the Flash control
	register (FLASH_CR).
	4. Set the STRT bit in the FLASH_CR register.
	5. Wait for the BSY bit to be cleared in the FLASH_SR register.
*/

int stm32l4_flash_erase(cm3ice_ctrl_t * ctrl, 
					   ice_mem_ref_t * mem, 
					   ice_size_t len)
{
	uint32_t acr;
	int page;

	stm32l4xxx_flash_clr(ctrl);

	/* 2KiB pages */
	page = mem->offs >> 11;
	INF("page=%d", page);

	/* Sabe ACR */
	cm3ice_rd32(ctrl, STM32F_BASE_FLASH + FLASH_ACR, &acr);
	/* Disable cache & Flush cache */
	cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_ACR, 
				(acr & ~(DCEN | ICEN)) | DCRST | ICRST);

	cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_CR, PER | PNB(page) | STRT);

	if (stm32l4xx_flash_bsy_wait(ctrl) < 0)

	/* Restore ACR */
	cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_ACR, acr);

	return 2048;
}

int stm32l4_flash_write(cm3ice_ctrl_t * ctrl, ice_mem_ref_t * mem, 
					 const void * buf, ice_size_t len)
{
	uint32_t data0;
	uint32_t data1;
	uint8_t * ptr;
	uint32_t offs;
	uint32_t base;
	int rem;
	int ret;

	stm32l4xxx_flash_clr(ctrl);

	ptr = (uint8_t *)buf;
	offs = mem->offs;
	base = mem->base;
	
	len = (len + 7) & ~0x00000007;
	rem = len;

 /*
   Standard programming
   The Flash memory programming sequence in standard mode is as follows:
   1. Check that no Flash main memory operation is ongoing by checking the BSY bit in the
   Flash status register (FLASH_SR).
   2. Check and clear all error programming flags due to a previous programming. If not,
   PGSERR is set.
   3. Set the PG bit in the Flash control register (FLASH_CR).
   4. Perform the data write operation at the desired memory address, inside main memory
   block or OTP area. Only double word can be programmed.
   – Write a first word in an address aligned with double word
   – Write the second word
   5. Wait until the BSY bit is cleared in the FLASH_SR register.
   6. Check that EOP flag is set in the FLASH_SR register (meaning that the programming
   operation has succeed), and clear it by software.
   7. Clear the PG bit in the FLASH_SR register if there no more programming request
   anymore.
   */

	while (rem > 0) {
		if ((offs & 7) != 0) {
			WARN("Unaligned offs=0x%08x", offs);
			data0 = 0;
			data1 = ptr[0] | (ptr[1] << 8) | (ptr[2] << 16)| (ptr[3] << 24);
			offs -= 4;
		} else {
			data0 = ptr[0] | (ptr[1] << 8) | (ptr[2] << 16)| (ptr[3] << 24);
			data1 = ptr[4] | (ptr[5] << 8) | (ptr[6] << 16)| (ptr[7] << 24);
		}

		INF("offs=0x%08x", offs);

		/* start half page write */
		DCC_LOG(LOG_TRACE, "Half-Page write start...");
		cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_CR, PG);

		cm3ice_wr32(ctrl, base + offs, data0);
		cm3ice_wr32(ctrl, base + offs + 4, data1);

		if ((ret = stm32l4xx_flash_bsy_wait(ctrl)) != 0) {
			return -1;
		}

		/* Clear flash program operation */
		cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_CR, 0);

		offs += 8;
		ptr += 8;
		rem -= 8;
	}


/*
	Fast programming
		This mode allows to program a row (32 double word) and to reduce the page programming
		time by eliminating the need for verifying the flash locations before they are programmed
		and to avoid rising and falling time of high voltage for each double word. During fast
		programming, the CPU clock frequency (HCLK) must be at least 8 MHz.
		Only the main memory can be programmed in Fast programming mode.
		The Flash main memory programming sequence in standard mode is as follows:
		1. Perform a mass erase of the bank to program. If not, PGSERR is set.
		2. Check that no Flash main memory operation is ongoing by checking the BSY bit in the
		Flash status register (FLASH_SR).
		3. Check and clear all error programming flag due to a previous programming.
		4. Set the FSTPG bit in Flash control register (FLASH_CR).
		5. Write the 32 double words to program a row. Only double words can be programmed:
		– Write a first word in an address aligned with double word
		– Write the second word.
		6. Wait until the BSY bit is cleared in the FLASH_SR register.
		7. Check that EOP flag is set in the FLASH_SR register (meaning that the programming
																operation has succeed), and clear it by software.
		8. Clear the FSTPG bit in the FLASH_SR register if there no more programming request
		anymore.
		*/

#if 0
	while (rem > 0) {
		if ((offs & 0xff) == 0) { 
			/* page boundary */
			if (rem >= 256) {
				/* start half page write */
				DCC_LOG(LOG_TRACE, "Half-Page write start...");
				cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_CR, FPRG | PROG);
			} else {
				DCC_LOG(LOG_TRACE, "Partial block start...");
				cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_CR, 0);
			}
			if ((ret = stm32l4xx_flash_bsy_wait(ctrl)) < 0)
				return ret;
		}
		data = ptr[0] | (ptr[1] << 8) | (ptr[2] << 16)| (ptr[3] << 24);
		cm3ice_wr32(ctrl, base + offs, data);
		ptr += 4;
		offs += 4;
		rem -= 4;
	}
#endif

	return len;
}

const struct ice_mem_oper flash_stm32l4_oper = {
	.read = (ice_mem_read_t)cm3ice_mem_read,
	.write = (ice_mem_write_t)stm32l4_flash_write, 
	.erase = (ice_mem_erase_t)stm32l4_flash_erase
};

#define OPTION_BYTE_BASE 0x1ff80000

int stm32l4xx_on_init(FILE * f, const ice_drv_t * ice, 
					 ice_mem_entry_t * mem)
{
//	uint32_t obr;
//	uint32_t opt[2];
	uint32_t cr;
	uint32_t sr;
	uint32_t cfg;
	uint32_t enr;
	int again;

	if (ice_rd32(ice, STM32F_BASE_RCC + RCC_CFGR, &cfg)) {
		WARN("can't read from target");
		return -1;
	}

	ice_rd32(ice, STM32F_BASE_RCC + RCC_CR, &cr);
	DCC_LOG2(LOG_TRACE, "cr=0x%08x cfg=0x%08x", cr, cfg);
	INF("cr=0x%08x cfg=0x%08x", cr, cfg);

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
			cr |= HSION | HSIKERON; 
			ice_wr32(ice, STM32F_BASE_RCC + RCC_CR, cr);

			for (again = 1256; ; again--) {
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

#if 0
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
#endif

	/*******************************************************************
	 * Configure flash
	 *******************************************************************/
#define RCC_AHB1ENR 0x48

	ice_rd32(ice, STM32F_BASE_RCC + RCC_AHB1ENR, &enr);
	INF("RCC_AHB1ENR=0x%08x", enr);

	if ((enr & FLASHEN) == 0) {
		WARN("FLASH is disabled!");
		enr |= FLASHEN;
		ice_wr32(ice, STM32F_BASE_RCC + RCC_AHB1ENR, enr);
	}

	ice_rd32(ice, STM32F_BASE_FLASH + FLASH_SR, &sr);
	INF("FLASH SR=0x%08x", sr);

	if (sr & PEMPTY) {
		fprintf(f, " %s: main flash empty!\n", __func__);
	}

	/* adjust flash wait states and enable caches */
	ice_wr32(ice, STM32F_BASE_FLASH + FLASH_ACR, PRFTEN | LATENCY(4));

	ice_rd32(ice, STM32F_BASE_FLASH + FLASH_CR, &cr);
	DCC_LOG1(LOG_TRACE, "CR=0x%08x", cr);
	INF("FLASH CR=0x%08x", cr);
	if (cr & LOCK) {
		fprintf(f, " %s: unlocking flash\n", __func__);
		/* unlock flash write */
		ice_wr32(ice, STM32F_BASE_FLASH + FLASH_KEYR, FLASH_KEY1);
		ice_wr32(ice, STM32F_BASE_FLASH + FLASH_KEYR, FLASH_KEY2);
	}


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
