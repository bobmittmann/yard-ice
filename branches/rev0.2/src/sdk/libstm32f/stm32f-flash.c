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

