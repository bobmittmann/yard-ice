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

#include <stm32f/stm32l-flash.h>
#include <sys/param.h>

#define STM32_BASE_FLASH   0x40023c00
#define STM32_FLASH ((struct stm32_flash *)STM32_BASE_FLASH)

#define STM32_FLASH_ADDR 0x08000000
#define FLASH_PAGE_SIZE 256


#define FLASH_ERR (FLASH_RDERR | FLASH_OPTVERRUSR | FLASH_OPTVERR | \
				   FLASH_SIZERR | FLASH_PGAERR | FLASH_WRPERR)

int stm32l_flash_page_erase(struct stm32_flash * flash, 
							uint32_t volatile * addr)
{
	uint32_t sr;

	flash->pecr = FLASH_ERASE | FLASH_PROG;
	*addr = 0x00000000;

	do {
		sr = flash->sr;
	} while (sr & FLASH_BSY);

	flash->pecr = 0;

	return (sr & FLASH_ERR) ? -1 : 0;
}

int flash_erase(uint32_t offs, unsigned int len)
{
	struct stm32_flash * flash = STM32_FLASH;
	uint32_t addr;
	uint32_t pg_offs;
	int ret;
	int rem;
	int cnt;

	pg_offs = offs & ~(FLASH_PAGE_SIZE - 1);
	addr = STM32_FLASH_ADDR + pg_offs;

	cnt = 0;
	rem = len + (offs - pg_offs);

	while (rem > 0) {

		ret = stm32l_flash_page_erase(flash, (uint32_t *)addr);

		if (ret < 0) {
			cnt = ret;
			break;
		}

		addr += FLASH_PAGE_SIZE;
		rem -= FLASH_PAGE_SIZE;
		cnt += FLASH_PAGE_SIZE;
	}

	return cnt;
}

static int stm32l_flash_pg_wr(struct stm32_flash * flash, 
							  uint32_t volatile dst[], uint32_t src[])
{
	uint32_t sr;
	int i;

	/* start half page write */
	flash->pecr = FLASH_FPRG | FLASH_PROG;

/*	do {
		sr = flash->sr;
	} while (sr & FLASH_BSY);

	if (sr & FLASH_ERR)
		return -1; */

	for (i = 0; i < (128 / 4); ++i)
		dst[i] = src[i];

	do {
		sr = flash->sr;
	} while (sr & FLASH_BSY);

	flash->pecr = 0;

	if (sr & FLASH_ERR)
		return -1;

	return 0;
}

void flash_unlock(void)
{
	struct stm32_flash * flash = STM32_FLASH;
	uint32_t pecr;

	pecr = flash->pecr;
	if (pecr & FLASH_PRGLOCK) {
		if (pecr & FLASH_PELOCK) {
			flash->pekeyr = FLASH_PEKEY1;
			flash->pekeyr = FLASH_PEKEY2;
		}
		flash->prgkeyr= FLASH_PRGKEYR1;
		flash->prgkeyr= FLASH_PRGKEYR2;
	}
}

int flash_write(uint32_t offs, const void * buf, unsigned int len)
{
	struct stm32_flash * flash = STM32_FLASH;
	uint32_t base = STM32_FLASH_ADDR;
	uint8_t * ptr;
	int rem;
	int ret;

	ptr = (uint8_t *)buf;
	rem = len;
	while (rem > 0) {
		uint32_t blk[128 / 4];
		int n;
		int i;

		if (((offs & 0x7f) != 0) || (rem < 128)) {
			uint32_t pos;
			uint32_t * src;
			uint8_t * dst;

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
			n = 128;
			src = ptr;
			for (i = 0; i < (128 / 4); ++i) {
				data = src[0] | (src[1] << 8) | (src[2] << 16)| (src[3] << 24);
				blk[i] = data;
				src += 4;
			}	
		}

		ret = stm32l_flash_pg_wr(flash, (uint32_t *)(base + offs), blk);

		if (ret < 0) {
			return ret;
		}

		ptr += n;
		rem -= n;
		offs += 128;
	}

	return len;
}

