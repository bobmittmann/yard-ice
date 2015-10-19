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

#define STM32F2X
#include <stm32f/stm32f-flash.h>
#include <sys/param.h>

#define STM32_BASE_FLASH 0x40023c00
#define STM32_FLASH ((struct stm32_flash *)STM32_BASE_FLASH)
#define STM32_FLASH_ADDR 0x08000000

#define FLASH_ERR (FLASH_PGSERR | FLASH_PGPERR | FLASH_PGAERR | FLASH_WRPERR | \
				   FLASH_OPERR)

void flash_unlock(void)
{
	struct stm32_flash * flash = STM32_FLASH;
	uint32_t cr;

	cr = flash->cr;
	if (cr & FLASH_LOCK) {
		/* unlock flash write */
		flash->keyr = FLASH_KEY1;
		flash->keyr = FLASH_KEY2;
	}
}


static int stm32f2x_flash_sect_erase(struct stm32_flash * flash, 
									 unsigned int sect)
{
	uint32_t sr;

	flash->cr = FLASH_STRT | FLASH_SER | FLASH_SNB(sect);

	do {
		sr = flash->sr;
	} while (sr & FLASH_BSY);

	if (sr & FLASH_ERR)
		return -1;

	return 0;
}

int flash_erase(uint32_t offs, unsigned int len)
{
	struct stm32_flash * flash = STM32_FLASH;
	unsigned int page;
	unsigned int sect;
	unsigned int size;
	unsigned int cnt;

	page = offs >> 14;

	/* must start at a page boundary */
	if ((page << 14) != (offs)) {
		return -1;
	};

	cnt = 0;
	while (cnt < len) {

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
			sect = ((page - 7) / 8) + 5;
			size = 131072;
			break;
		}

		if (stm32f2x_flash_sect_erase(flash, sect) < 0) {
			return -1;
		}

		cnt += size;
		offs += size;
		page = offs >> 14;
	}

	return cnt;
}


static int stm32f2x_flash_wr32(struct stm32_flash * flash,
							uint32_t volatile * addr, uint32_t data)
{
	uint32_t sr;

	flash->cr = FLASH_PG | FLASH_PSIZE_32;
	*addr = data;
	
	do {
		sr = flash->sr;
	} while (sr & FLASH_BSY);

	if (sr & FLASH_ERR)
		return -1;

	return 0;
}

int flash_write(uint32_t offs, const void * buf, unsigned int len)
{
	struct stm32_flash * flash = STM32_FLASH;
	uint32_t data;
	uint32_t * addr;
	uint8_t * ptr;
	int n;
	int i;

	n = (len + 3) / 4;

	ptr = (uint8_t *)buf;
	addr = (uint32_t *)(STM32_FLASH_ADDR + offs);

	for (i = 0; i < n; i++) {
		data = ptr[0] | (ptr[1] << 8) | (ptr[2] << 16) | (ptr[3] << 24);
		if (stm32f2x_flash_wr32(flash, addr, data) < 0)
			return -1;
		ptr += 4;
		addr++;
	}
	
	return n * 4;
}
