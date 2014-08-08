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
 * @file stm32l-flash.c
 * @brief STM32L flash access API
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <sys/stm32f.h>
#include <sys/delay.h>
#include <arch/cortex-m3.h>
#include <sys/dcclog.h>
#include <sys/param.h>

#define FLASH_ERR (FLASH_RDERR | FLASH_OPTVERRUSR | FLASH_OPTVERR | \
				   FLASH_SIZERR | FLASH_PGAERR | FLASH_WRPERR)

int __attribute__((section (".data#"))) 
	stm32l_flash_blk_erase(struct stm32_flash * flash, uint32_t volatile * addr)
{
	uint32_t sr = flash->sr;

	flash->pecr = FLASH_ERASE | FLASH_PROG;
	*addr = 0x00000000;

	do {
		sr = flash->sr;
	} while (sr & FLASH_BSY);

	flash->pecr = 0;

	return (sr & FLASH_ERR) ? -1 : 0;
}

#define FLASH_SECTOR_SIZE 256

int stm32_flash_erase(unsigned int offs, unsigned int len)
{
	struct stm32_flash * flash = STM32_FLASH;
	uint32_t addr;
	uint32_t pecr;
	int ret;
	int rem = len;
	int cnt;

//	uint32_t cr;

	offs &= ~(FLASH_SECTOR_SIZE - 1);

	addr = (uint32_t)STM32_MEM_FLASH + offs;

	DCC_LOG2(LOG_TRACE, "addr=0x%08x len=%d", addr, len);

	pecr = flash->pecr;
	DCC_LOG1(LOG_INFO, "PECR=0x%08x", pecr);
	if (pecr & FLASH_PRGLOCK) {
		DCC_LOG(LOG_INFO, "unlocking flash...");
		if (pecr & FLASH_PELOCK) {
			flash->pekeyr = FLASH_PEKEY1;
			flash->pekeyr = FLASH_PEKEY2;
		}
		flash->prgkeyr= FLASH_PRGKEYR1;
		flash->prgkeyr= FLASH_PRGKEYR2;
	}

	cnt = 0;
	rem = len;
	while (rem) {
		uint32_t pri;

		DCC_LOG1(LOG_INFO, "addr=0x%08x", addr);

		pri = cm3_primask_get();
		cm3_primask_set(1);
		ret = stm32l_flash_blk_erase(flash, (uint32_t *)addr);
		cm3_primask_set(pri);

		if (ret < 0) {
#if DEBUG
			uint32_t sr = flash->sr;
			DCC_LOG6(LOG_WARNING, "erase failed: %s%s%s%s%s%s", 
					 sr & FLASH_RDERR ? "RDERR" : "",
					 sr & FLASH_OPTVERRUSR ? "OPTVERRUSR" : "",
					 sr & FLASH_OPTVERR ? "OPTVERR " : "",
					 sr & FLASH_SIZERR ? "SIZERR " : "",
					 sr & FLASH_PGAERR ? "PGAERR" : "",
					 sr & FLASH_WRPERR ? "WRPERR" : "");
#endif
			cnt = ret;
			break;
		}
		addr += FLASH_SECTOR_SIZE;
		rem -= FLASH_SECTOR_SIZE;
		cnt += FLASH_SECTOR_SIZE;

	}

	return cnt;
}

int __attribute__((section (".data#"))) 
	stm32l_flash_pg_wr(struct stm32_flash * flash, 
					   uint32_t volatile dst[], uint32_t src[])
{
	uint32_t sr;
	int i;

	/* start half page write */
	flash->pecr = FLASH_FPRG | FLASH_PROG;

	do {
		sr = flash->sr;
	} while (sr & FLASH_BSY);

	if (sr & FLASH_ERR) {
		return -1;
	}

	for (i = 0; i < (128 / 4); ++i)
		dst[i] = src[i];

	do {
		sr = flash->sr;
	} while (sr & FLASH_BSY);

	flash->pecr = 0;

	if (sr & FLASH_ERR) {
		return -1;
	}

	return 0;
}

int stm32_flash_write(uint32_t offs, const void * buf, unsigned int len)
{
	struct stm32_flash * flash = STM32_FLASH;
	uint32_t base = (uint32_t)STM32_MEM_FLASH;
	uint8_t * ptr;
	uint32_t pecr;
	int rem;
	int ret;

	DCC_LOG2(LOG_TRACE, "addr=0x%08x len=%d", base + offs, len);

	pecr = flash->pecr;
	DCC_LOG1(LOG_INFO, "PECR=0x%08x", pecr);
	if (pecr & FLASH_PRGLOCK) {
		DCC_LOG(LOG_TRACE, "unlocking flash...");
		if (pecr & FLASH_PELOCK) {
			flash->pekeyr = FLASH_PEKEY1;
			flash->pekeyr = FLASH_PEKEY2;
		}
		flash->prgkeyr= FLASH_PRGKEYR1;
		flash->prgkeyr= FLASH_PRGKEYR2;
	}

	ptr = (uint8_t *)buf;
	rem = len;
	while (rem > 0) {
		uint32_t blk[128 / 4];
		uint32_t pri;
		int n;
		int i;

		if (((offs & 0x7f) != 0) || (rem < 128)) {
			uint32_t pos;
			uint32_t * src;
			uint8_t * dst;

			DCC_LOG(LOG_INFO, "Partial page write...");
			/* get the position inside the flash block where the 
			   writing should start */
			pos = offs - (offs & ~0x7f);
			offs -= pos;

			/* copy a full block from flash to buffer */
			src = (uint32_t *)(base + offs);
			for (i = 0; i < (128 / 4); ++i)
				blk[i] = src[i];

			/* partially override the buffer with input data */
			n = MIN(128 - pos, rem);
			dst = (uint8_t *)(blk) + pos;
			for (i = 0; i < n; ++i)
				dst[i] = ptr[i];

		} else {
			uint8_t * src;
			uint32_t data;
			/* start half page write */
			DCC_LOG(LOG_INFO, "Half-Page write start...");
			n = 128;
			src = ptr;
			for (i = 0; i < (128 / 4); ++i) {
				data = src[0] | (src[1] << 8) | (src[2] << 16)| (src[3] << 24);
				blk[i] = data;
				src += 4;
			}	
		}

		pri = cm3_primask_get();
		cm3_primask_set(1);
		ret = stm32l_flash_pg_wr(flash, (uint32_t *)(base + offs), blk);
		cm3_primask_set(pri);

		if (ret < 0) {
			DCC_LOG(LOG_WARNING, "Flash write failed!");
			return ret;
		}

		ptr += n;
		rem -= n;
		offs += 128;
	}

	return len;
}

