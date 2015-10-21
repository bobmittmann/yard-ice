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
 * @file stm32f-flash.c
 * @brief STM32F flash access API
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <sys/stm32f.h>
#include <sys/delay.h>
#include <arch/cortex-m3.h>
#include <sys/dcclog.h>

#if defined(STM32F1X) || defined(STM32F3X)

#if defined(STM32F1X)
#define FLASH_BLOCK_SIZE 1024
#elif defined(STM32F3X)
#define FLASH_BLOCK_SIZE 2048
#endif

#define FLASH_ERR (FLASH_WRPRTERR | FLASH_PGPERR)

int __attribute__((section (".data#"))) 
	stm32f10x_flash_blk_erase(struct stm32_flash * flash, uint32_t addr)
{
	uint32_t sr;

	flash->cr = FLASH_SER;
	flash->ar = addr;
	flash->cr = FLASH_STRT | FLASH_SER;

#if defined(STM32F1X) 
/* STM32F10xx4 STM32F10xx6 Errata sheet
  2.11 Flash memory BSY bit delay versus STRT bit setting
  Description
  When the STRT bit in the Flash memory control register is set (to launch 
  an erase operation), the BSY bit in the Flash memory status register 
  goes high one cycle later.
  Therefore, if the FLASH_SR register is read immediately after the FLASH_CR 
  register is written (STRT bit set), the BSY bit is read as 0.
  Workaround
  Read the BSY bit at least one cycle after setting the STRT bit.
*/
	__nop();
	__nop();
#endif

	do {
		sr = flash->sr;
	} while (sr & FLASH_BSY);

	return sr;
}

int stm32_flash_erase(unsigned int offs, unsigned int len)
{
	struct stm32_flash * flash = STM32_FLASH;
	unsigned int cnt;
	uint32_t addr;
	uint32_t cr;
	uint32_t sr;
	uint32_t pri;

	if ((offs & ~(FLASH_BLOCK_SIZE - 1)) != offs) {
		/* make sure we are block aligned */
		DCC_LOG(LOG_ERROR, "offset must be block aligned!");
		return -1;
	}

	addr = (uint32_t)STM32_FLASH_MEM + offs;

	DCC_LOG2(LOG_TRACE, "addr=0x%08x len=%d", addr, len);

	cr = flash->cr;
	if (cr & FLASH_LOCK) {
		DCC_LOG(LOG_TRACE, "unlocking flash...");
		/* unlock flash write */
		flash->keyr = FLASH_KEY1;
		flash->keyr = FLASH_KEY2;
	}

	cnt = 0;
	while (cnt < len) {
		pri = cm3_primask_get();
		cm3_primask_set(1);
		sr = stm32f10x_flash_blk_erase(flash, addr);
		cm3_primask_set(pri);

		if (sr & FLASH_ERR) {
			DCC_LOG(LOG_WARNING, "stm32f10x_flash_blk_erase() failed!");
			return -1;
		}

		cnt += FLASH_BLOCK_SIZE;
		addr += FLASH_BLOCK_SIZE;
	}

	return cnt;
}

int __attribute__((section (".data#"))) 
	stm32f10x_flash_wr16(struct stm32_flash * flash,
						 uint16_t volatile * addr, uint16_t data)
{
	uint32_t sr;

	flash->cr = FLASH_PG;
	*addr = data;

	do {
		sr = flash->sr;
	} while (sr & FLASH_BSY);

	return sr;
}

int stm32_flash_write(uint32_t offs, const void * buf, unsigned int len)
{
	struct stm32_flash * flash = STM32_FLASH;
	uint16_t data;
	uint16_t * addr;
	uint8_t * ptr;
	uint32_t pri;
	uint32_t cr;
	uint32_t sr;
	int n;
	int i;

	if (offs & 0x00000001) {
		DCC_LOG(LOG_ERROR, "offset must be 16bits aligned!");
		return -1;
	}

	n = (len + 1) / 2;

	ptr = (uint8_t *)buf;
	addr = (uint16_t *)((uint32_t)STM32_FLASH_MEM + offs);

	cr = flash->cr;
	if (cr & FLASH_LOCK) {
		DCC_LOG(LOG_TRACE, "unlocking flash...");
		/* unlock flash write */
		flash->keyr = FLASH_KEY1;
		flash->keyr = FLASH_KEY2;
	}

	DCC_LOG2(LOG_INFO, "0x%08x len=%d", addr, len);

	for (i = 0; i < n; i++) {
		data = ptr[0] | (ptr[1] << 8);
		DCC_LOG2(LOG_MSG, "0x%08x data=0x%04x", addr, data);
		pri = cm3_primask_get();
		cm3_primask_set(1);
		sr = stm32f10x_flash_wr16(flash, addr, data);
		cm3_primask_set(pri);
		if (sr & FLASH_ERR) {
			DCC_LOG(LOG_WARNING, "stm32f10x_flash_wr16() failed!");
			return -1;
		}
		ptr += 2;
		addr++;
	}
	
	return n * 2;
}

#endif


#if defined(STM32F2X) || defined(STM32F4X)

#define FLASH_ERR (FLASH_PGSERR | FLASH_PGPERR | FLASH_PGAERR | FLASH_WRPERR | \
				   FLASH_OPERR)

uint32_t __attribute__((section (".data#"), noinline)) 
	stm32f2x_flash_sect_erase(struct stm32_flash * flash, uint32_t cr)
{
	uint32_t sr;

	flash->cr = cr;

	do {
		sr = flash->sr;
	} while (sr & FLASH_BSY);

	return sr;
}

int stm32_flash_erase(unsigned int offs, unsigned int len)
{
	struct stm32_flash * flash = STM32_FLASH;
	unsigned int cnt;
	uint32_t pri;
	uint32_t cr;
	uint32_t sr;

	pri = cm3_primask_get();

	cr = flash->cr;
	if (cr & FLASH_LOCK) {
		DCC_LOG(LOG_TRACE, "unlocking flash...");
		/* unlock flash write */
		flash->keyr = FLASH_KEY1;
		flash->keyr = FLASH_KEY2;
	}

	cnt = 0;
	while (cnt < len) {
		unsigned int page;
		unsigned int sect;
		unsigned int size;

		page = offs >> 14;
		if ((page << 14) != (offs)) {
			DCC_LOG(LOG_ERROR, "offset must be a aligned to a page boundary.");
			return -1;
		};

		if (page < 4) {
			sect = page;
			size = 16384;
		} else if (page == 4) {
			sect = 4;
			size = 65536;
		} else if ((page % 8) == 0) {
			sect = ((page - 7) / 8) + 5;
			size = 131072;
		} else {
			DCC_LOG(LOG_ERROR, "offset must be a aligned to a "
					"sector boundary.");
			return -1;
		}

		cr = FLASH_STRT | FLASH_SER | FLASH_SNB(sect);
		cm3_primask_set(1);
		sr = stm32f2x_flash_sect_erase(flash, cr);
		cm3_primask_set(pri);

		if (sr & FLASH_ERR) {
			DCC_LOG1(LOG_WARNING, "stm32f2x_flash_sect_erase() failed"
					 " sr=%08x!", sr);
			return -1;
		}

		cnt += size;
		offs += size;
	}

	return cnt;
}

uint32_t __attribute__((section (".data#"), noinline)) 
	stm32f2x_flash_wr32(struct stm32_flash * flash,
						 uint32_t volatile * addr, uint32_t data)
{
	uint32_t sr;

	flash->cr = FLASH_PG | FLASH_PSIZE_32;
	*addr = data;
	
	do {
		sr = flash->sr;
	} while (sr & FLASH_BSY);

	return sr;
}

int stm32_flash_write(uint32_t offs, const void * buf, unsigned int len)
{
	struct stm32_flash * flash = STM32_FLASH;
	uint32_t data;
	uint32_t * addr;
	uint8_t * ptr;
	uint32_t cr;
	uint32_t sr;
	uint32_t pri;
	int n;
	int i;

	if (offs & 0x00000003) {
		DCC_LOG(LOG_ERROR, "offset must be 32bits aligned!");
		return -1;
	}

	n = (len + 3) / 4;

	ptr = (uint8_t *)buf;
	addr = (uint32_t *)((uint32_t)STM32_FLASH_MEM + offs);

	cr = flash->cr;
	if (cr & FLASH_LOCK) {
		DCC_LOG(LOG_TRACE, "unlocking flash...");
		/* unlock flash write */
		flash->keyr = FLASH_KEY1;
		flash->keyr = FLASH_KEY2;
	}

	DCC_LOG2(LOG_INFO, "0x%08x len=%d", addr, len);

	pri = cm3_primask_get();
	for (i = 0; i < n; i++) {
		data = ptr[0] | (ptr[1] << 8) | (ptr[2] << 16) | (ptr[3] << 24);
		DCC_LOG2(LOG_MSG, "0x%08x data=0x%04x", addr, data);
		cm3_primask_set(1);
		sr = stm32f2x_flash_wr32(flash, addr, data);
		cm3_primask_set(pri);
		if (sr & FLASH_ERR) {
			DCC_LOG(LOG_WARNING, "stm32f2x_flash_wr32() failed!");
			return -1;
		}
		ptr += 4;
		addr++;
	}
	
	return n * 4;
}

#endif

