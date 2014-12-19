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

#define STM32F1X
#include <stm32f/stm32f-flash.h>
#include <sys/param.h>

#define STM32_BASE_FLASH   0x40022000
#define STM32_FLASH ((struct stm32_flash *)STM32_BASE_FLASH)

#define STM32_FLASH_ADDR 0x08000000
#define FLASH_BLOCK_SIZE 1024
//#define FLASH_BLOCK_SIZE 512

#define FLASH_ERR (FLASH_WRPRTERR | FLASH_PGPERR)

static int stm32f10x_flash_blk_erase(struct stm32_flash * flash, uint32_t addr)
{
	uint32_t sr;

	flash->cr = FLASH_SER;
	do {
		sr = flash->sr;
	} while (sr & FLASH_BSY);
	flash->ar = addr;
	flash->cr = FLASH_STRT | FLASH_SER;

	do {
		sr = flash->sr;
	} while (sr & FLASH_BSY);

	flash->cr = 0;

	if (sr & FLASH_ERR)
		return -1;

	return 0;
}

int flash_erase(uint32_t offs, unsigned int len)
{
	struct stm32_flash * flash = STM32_FLASH;
	uint32_t addr;
	uint32_t blk_offs;
	int ret;
	int rem;
	int cnt;

	blk_offs = offs & ~(FLASH_BLOCK_SIZE - 1);
	addr = STM32_FLASH_ADDR + blk_offs;

	cnt = 0;
	rem = len + (offs - blk_offs);

	while (rem > 0) {

		ret = stm32f10x_flash_blk_erase(flash, addr);

		if (ret < 0) {
			cnt = ret;
			break;
		}

		addr += FLASH_BLOCK_SIZE;
		rem -= FLASH_BLOCK_SIZE;
		cnt += FLASH_BLOCK_SIZE;
	}

	return cnt;
}


static int stm32f10x_flash_wr16(struct stm32_flash * flash,
						 uint16_t volatile * addr, uint16_t data)
{
	uint32_t sr;

	flash->cr = FLASH_PG;
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
	uint16_t data;
	uint16_t * addr;
	uint8_t * ptr;
	int n;
	int i;

	n = (len + 1) / 2;

	ptr = (uint8_t *)buf;
	addr = (uint16_t *)(STM32_FLASH_ADDR + offs);

	for (i = 0; i < n; i++) {
		data = ptr[0] | (ptr[1] << 8);
		if (stm32f10x_flash_wr16(flash, addr, data) < 0)
			return -1;
		ptr += 2;
		addr++;
	}
	
	return n * 2;
}

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

