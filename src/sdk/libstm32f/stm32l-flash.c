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

uint32_t __attribute__((section (".data#"))) 
	stm32l_flash_blk_erase(struct stm32_flash * flash, uint32_t volatile * addr)
{
	uint32_t sr = flash->sr;

	flash->pecr = FLASH_ERASE | FLASH_PROG;
	*addr = 0x00000000;

	do {
		sr = flash->sr;
	} while (sr & FLASH_BSY);

	flash->pecr = 0;

	return sr;
}

#define FLASH_PAGE_SIZE 256

int stm32_flash_erase(unsigned int offs, unsigned int len)
{
	struct stm32_flash * flash = STM32_FLASH;
	uint32_t addr;
	uint32_t pecr;
	int rem = len;
	int cnt;

	offs &= ~(FLASH_PAGE_SIZE - 1);

	addr = (uint32_t)STM32_MEM_FLASH + offs;

	DCC_LOG2(LOG_INFO, "addr=0x%08x len=%d", addr, len);

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
		uint32_t sr;

		DCC_LOG1(LOG_INFO, "addr=0x%08x", addr);

		pri = cm3_primask_get();
		cm3_primask_set(1);
		sr = stm32l_flash_blk_erase(flash, (uint32_t *)addr);
		cm3_primask_set(pri);

		if (sr & FLASH_ERR) {
#if DEBUG
			DCC_LOG6(LOG_WARNING, "erase failed: %s%s%s%s%s%s", 
					 sr & FLASH_RDERR ? "RDERR" : "",
					 sr & FLASH_OPTVERRUSR ? "OPTVERRUSR" : "",
					 sr & FLASH_OPTVERR ? "OPTVERR " : "",
					 sr & FLASH_SIZERR ? "SIZERR " : "",
					 sr & FLASH_PGAERR ? "PGAERR" : "",
					 sr & FLASH_WRPERR ? "WRPERR" : "");
#endif
			cnt = -1;
			break;
		}
		addr += FLASH_PAGE_SIZE;
		rem -= FLASH_PAGE_SIZE;
		cnt += FLASH_PAGE_SIZE;

	}

	return cnt;
}

/* 
   write half flash page .
   offs must be half page aligned.
 */

uint32_t __attribute__((section (".data#"))) 
stm32l_flash_pg_wr(uint32_t offs, uint8_t * src,  uint32_t pos, uint32_t cnt)
{
	struct stm32_flash * flash = STM32_FLASH;
	uint32_t base = (uint32_t)STM32_MEM_FLASH;
	uint32_t * dst;
	uint32_t sr;
	int i;

	/* start half page write */
	flash->pecr = FLASH_FPRG | FLASH_PROG;

	do {
		sr = flash->sr;
	} while (sr & FLASH_BSY);

	if (sr & FLASH_ERR)
		return sr;

	dst = (uint32_t *)(base + offs);

	/* zero the initial words */
	for (i = 0; i < (pos / 4); ++i)
		*dst++ = 0;

	/* adjust counter to count words */
	for (; i < ((pos + cnt) / 4); ++i) {
		*dst++ = src[0] | (src[1] << 8) | (src[2] << 16)| (src[3] << 24);
		src += 4;
	}

	for (; i < ((FLASH_PAGE_SIZE / 2) / 4); ++i)
		*dst++ = 0;

	do {
		sr = flash->sr;
	} while (sr & FLASH_BSY);

	flash->pecr = 0;

	return sr;
}

int stm32_flash_write(uint32_t offs, const void * buf, unsigned int len)
{
	struct stm32_flash * flash = STM32_FLASH;
	uint8_t * ptr;
	uint32_t pecr;
	int rem;

	DCC_LOG2(LOG_INFO, "offs=0x%06x len=%d", offs, len);

	if (offs & 3) {
		DCC_LOG(LOG_ERROR, "offset must be word aligned!");
		return -1;
	}

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
	while (rem > 3) {
		uint32_t pos;
		uint32_t cnt;
		uint32_t pri;
		uint32_t sr;

		/* adjust offset for half page alignement */
		pos = offs - (offs & ~((FLASH_PAGE_SIZE / 2) - 1));
		offs -= pos;

		/* make sure we don't exceed the page boundary */
		cnt = MIN((FLASH_PAGE_SIZE / 2) - pos, rem);

		DCC_LOG3(LOG_INFO, "1. offs=0x%06x pos=%d cnt=%d", offs, pos, cnt);

		/* adjust counter to count words */
		cnt &= ~3;

		pri = cm3_primask_get();
		cm3_primask_set(1);
		/* start half page write */
		sr = stm32l_flash_pg_wr(offs, ptr, pos, cnt);

		cm3_primask_set(pri);

		if (sr & FLASH_ERR) {
			DCC_LOG(LOG_WARNING, "Flash write failed!");
			return -1;
		}

		ptr += cnt;
		rem -= cnt;
		offs += pos + cnt;
	}

	if (rem) {
		uint32_t pos;
		uint32_t pri;
		uint8_t data[4];
		int ret;
		int i;

		/* FIXME: single word writing.... */

		/* adjust offset for half page alignement */
		pos = offs - (offs & ~((FLASH_PAGE_SIZE / 2) - 1));
		offs -= pos;

		/* copy data to temporary buffer */ 
		for (i = 0; i < rem; ++i)
			data[i] = ptr[i];
		/* fill the remaining buffer */
		for (; i < 4; ++i)
			data[i] = 0;

		DCC_LOG3(LOG_INFO, "2. offs=0x%06x pos=%d cnt=%d", offs, pos, 4);

		pri = cm3_primask_get();
		cm3_primask_set(1);
		/* start half page write */
		ret = stm32l_flash_pg_wr(offs, data, pos, 4);
		cm3_primask_set(pri);

		if (ret < 0) {
			DCC_LOG(LOG_WARNING, "Flash write failed!");
			return ret;
		}
	};

	return len;
}

