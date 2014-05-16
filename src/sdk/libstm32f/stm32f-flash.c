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

#ifdef STM32F1X

#define STM32F_FLASH_MEM ((uint32_t *)0x08000000)

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

	for (again = 4096 * 32; again > 0; again--) {
		sr = flash->sr;
		if ((sr & FLASH_BSY) == 0) {
			ret = 0;
			break;
		}
	}

	cm3_basepri_set(pri);

	return ret;
}

int stm32f_flash_erase(unsigned int offs, int len)
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

	stm32f10x_flash_blk_erase(flash, addr);

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
	
	for (again = 4096 * 32; again > 0; again--) {
		sr = flash->sr;
		if ((sr & FLASH_BSY) == 0) {
			ret = 0;
			break;
		}
	}

	cm3_basepri_set(pri);

	return ret;
}

int stm32f_flash_write(uint32_t offs, const void * buf, int len)
{
	struct stm32f_flash * flash = STM32F_FLASH;
	uint16_t data;
	uint32_t addr;
	uint8_t * ptr;
	uint32_t cr;
	int n;
	int i;

	n = (len + 1) / 2;

	ptr = (uint8_t *)buf;
	addr = __flash_base + offs;

	cr = flash->cr;
	if (cr & FLASH_LOCK) {
		DCC_LOG(LOG_TRACE, "unlocking flash...");
		/* unlock flash write */
		flash->keyr = FLASH_KEY1;
		flash->keyr = FLASH_KEY2;
	}


	DCC_LOG2(LOG_TRACE, "0x%08x len=%d", addr, len);

	for (i = 0; i < n; i++) {
		data = ptr[0] | (ptr[1] << 8);
		stm32f10x_flash_wr16(flash, (void *)addr, data);
		ptr += 2;
		addr += 2;
	}
	
	return n * 2;
}

#endif

#if 0
int stm32f_flash_erase(cm3ice_ctrl_t * ctrl, uint32_t base, 
						  uint32_t offs, int len)
{
	uint32_t sr;
	int again;
	int page;
	int sect;
	int size;

	page = (base + offs) >> 14;
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
		sect = ((page - 1) / 8) + 5;
		size = 131072;
		break;
	}

	DCC_LOG4(LOG_TRACE, "len=%d page=%d sect=%d size=%d", 
			 len, page, sect, size);

	cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_CR, SER | SNB(sect) | STRT);

	for (again = 4096 * 32; ; again--) {
		cm3ice_rd32(ctrl, STM32F_BASE_FLASH + FLASH_SR, &sr);
		if ((sr & BSY) == 0)
			break;
		if (again == 0) {
			DCC_LOG(LOG_WARNING, "flash not ready!");
			return -1;
		}
	}

	return size;
}

int stm32f_flash_wr32(cm3ice_ctrl_t * ctrl, uint32_t addr, uint32_t data)
{
	uint32_t sr;
	int again;

	DCC_LOG2(LOG_TRACE, "0x%08x <-- 0x%08x", addr, data);

	cm3ice_wr32(ctrl, STM32F_BASE_FLASH + FLASH_CR, PSIZE_32  | PG);
	cm3ice_wr32(ctrl, addr, data);
	
	for (again = 4096; ; again--) {
		cm3ice_rd32(ctrl, STM32F_BASE_FLASH + FLASH_SR, &sr);
		if ((sr & BSY) == 0)
			break;
		if (again == 0) {
			DCC_LOG(LOG_WARNING, "flash not ready!");
			return -1;
		}
	}

	return 0;
}

int stm32f_flash_write(cm3ice_ctrl_t * ctrl, uint32_t base, 
						  uint32_t offs, const void * buf, int len)
{
	uint32_t data;
	uint32_t addr;
	uint8_t * ptr;
	int n;
	int i;

	n = (len + 3) / 4;

	DCC_LOG2(LOG_INFO, "0x%08x len=%d", base + offs, len);

	ptr = (uint8_t *)buf;
	addr = base + offs;

	for (i = 0; i < n; i++) {
		data = ptr[0] | (ptr[1] << 8) | (ptr[2] << 16) | (ptr[3] << 24);
		stm32f_flash_wr32(ctrl, addr, data);
		ptr += 4;
		addr += 4;
	}
	
	return n * 4;
}

#endif

