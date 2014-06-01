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

#define STM32F_FLASH_MEM ((uint32_t *)0x08000000)

#ifdef STM32F1X

const uint32_t __flash_base = (uint32_t)STM32F_FLASH_MEM;
const uint32_t __flash_size;

int __attribute__((section (".data#"))) 
	stm32f10x_flash_blk_erase(struct stm32f_flash * flash, uint32_t addr)
{
	uint32_t pri;
	uint32_t sr;
	int again;
	int ret = -1;

	pri = cm3_basepri_get();
	cm3_basepri_set(0x01);

	flash->cr = FLASH_SER;
	flash->ar = addr;
	flash->cr = FLASH_STRT | FLASH_SER;

	for (again = 4096 * 1024; again > 0; again--) {
		sr = flash->sr;
		if ((sr & FLASH_BSY) == 0) {
			ret = 0;
			break;
		}
	}

	cm3_basepri_set(pri);

	return ret;
}

int stm32f_flash_erase(unsigned int offs, unsigned int len)
{
	struct stm32f_flash * flash = STM32F_FLASH;
	uint32_t addr;
	uint32_t cr;

	addr = __flash_base + offs;

	DCC_LOG2(LOG_TRACE, "addr=0x%08x len=%d", addr, len);

	cr = flash->cr;
	if (cr & FLASH_LOCK) {
		DCC_LOG(LOG_TRACE, "unlocking flash...");
		/* unlock flash write */
		flash->keyr = FLASH_KEY1;
		flash->keyr = FLASH_KEY2;
	}

	if (stm32f10x_flash_blk_erase(flash, addr) < 0) {
		DCC_LOG(LOG_WARNING, "stm32f10x_flash_blk_erase() failed!");
		return -1;
	}

	return len;
}

int __attribute__((section (".data#"))) 
	stm32f10x_flash_wr16(struct stm32f_flash * flash,
						 uint16_t volatile * addr, uint16_t data)
{
	uint32_t pri;
	uint32_t sr;
	int again;
	int ret = -1;

	pri = cm3_basepri_get();
	cm3_basepri_set(0x01);

	flash->cr = FLASH_PG;
	*addr = data;
	
	for (again = 4096 * 1024; again > 0; --again) {
		sr = flash->sr;
		if ((sr & FLASH_BSY) == 0) {
			ret = 0;
			break;
		}
	}

	cm3_basepri_set(pri);

	return ret;
}

int stm32f_flash_write(uint32_t offs, const void * buf, unsigned int len)
{
	struct stm32f_flash * flash = STM32F_FLASH;
	uint16_t data;
	uint16_t * addr;
	uint8_t * ptr;
	uint32_t cr;
	int n;
	int i;

	if (offs & 0x00000001) {
		DCC_LOG(LOG_ERROR, "offset must be 16bits aligned!");
		return -1;
	}

	n = (len + 1) / 2;

	ptr = (uint8_t *)buf;
	addr = (uint16_t *)(__flash_base + offs);

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
		if (stm32f10x_flash_wr16(flash, addr, data) < 0) {
			DCC_LOG(LOG_WARNING, "stm32f10x_flash_wr16() failed!");
			return -1;
		}
		ptr += 2;
		addr++;
	}
	
	return n * 2;
}

#endif


#ifdef STM32F2X

const uint32_t __flash_base = (uint32_t)STM32F_FLASH_MEM;
const uint32_t __flash_size;

int __attribute__((section (".data#"))) 
	stm32f10x_flash_sect_erase(struct stm32f_flash * flash, unsigned int sect)
{
	uint32_t pri;
	uint32_t sr;
	int again;
	int ret = -1;

	pri = cm3_basepri_get();
	cm3_basepri_set(0x01);

	flash->cr = FLASH_STRT | FLASH_SER | FLASH_SNB(sect) ;

	for (again = 4096 * 1024; again > 0; again--) {
		sr = flash->sr;
		if ((sr & FLASH_BSY) == 0) {
			ret = 0;
			break;
		}
	}

	cm3_basepri_set(pri);

	return ret;
}

int stm32f_flash_erase(unsigned int offs, unsigned int len)
{
	struct stm32f_flash * flash = STM32F_FLASH;
	uint32_t cr;
	unsigned int page;
	unsigned int sect;
	unsigned int size;
	unsigned int cnt;

	cr = flash->cr;
	if (cr & FLASH_LOCK) {
		DCC_LOG(LOG_TRACE, "unlocking flash...");
		/* unlock flash write */
		flash->keyr = FLASH_KEY1;
		flash->keyr = FLASH_KEY2;
	}

	page = offs >> 14;
	
	if ((page << 14) != (offs)) {
		DCC_LOG(LOG_ERROR, "offset must be a aligned to a sector boundary.");
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

		if (stm32f10x_flash_sect_erase(flash, sect) < 0) {
			DCC_LOG(LOG_WARNING, "stm32f10x_flash_blk_erase() failed!");
			return -1;
		}

		cnt += size;
	}

	return cnt;
}

int __attribute__((section (".data#"))) 
	stm32f_flash_wr32(struct stm32f_flash * flash,
						 uint32_t volatile * addr, uint32_t data)
{
	uint32_t pri;
	uint32_t sr;
	int again;
	int ret = -1;

	pri = cm3_basepri_get();
	cm3_basepri_set(0x01);

	flash->cr = FLASH_PG | FLASH_PSIZE_32;
	*addr = data;
	
	for (again = 4096 * 1024; again > 0; --again) {
		sr = flash->sr;
		if ((sr & FLASH_BSY) == 0) {
			ret = 0;
			break;
		}
	}

	cm3_basepri_set(pri);

	return ret;
}

int stm32f_flash_write(uint32_t offs, const void * buf, unsigned int len)
{
	struct stm32f_flash * flash = STM32F_FLASH;
	uint32_t data;
	uint32_t * addr;
	uint8_t * ptr;
	uint32_t cr;
	int n;
	int i;

	if (offs & 0x00000003) {
		DCC_LOG(LOG_ERROR, "offset must be 32bits aligned!");
		return -1;
	}

	n = (len + 3) / 4;

	ptr = (uint8_t *)buf;
	addr = (uint32_t *)(__flash_base + offs);

	cr = flash->cr;
	if (cr & FLASH_LOCK) {
		DCC_LOG(LOG_TRACE, "unlocking flash...");
		/* unlock flash write */
		flash->keyr = FLASH_KEY1;
		flash->keyr = FLASH_KEY2;
	}

	DCC_LOG2(LOG_INFO, "0x%08x len=%d", addr, len);

	for (i = 0; i < n; i++) {
		data = ptr[0] | (ptr[1] << 8) | (ptr[2] << 16) | (ptr[3] << 24);
		DCC_LOG2(LOG_MSG, "0x%08x data=0x%04x", addr, data);
		if (stm32f_flash_wr32(flash, addr, data) < 0) {
			DCC_LOG(LOG_WARNING, "stm32f10x_flash_wr16() failed!");
			return -1;
		}
		ptr += 4;
		addr++;
	}
	
	return n * 4;
}

#endif

