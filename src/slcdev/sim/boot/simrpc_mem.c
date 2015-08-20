/* 
 * File:	 usb-test.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2011 Bob Mittmann. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <thinkos.h>
#include <sys/serial.h>
#include <sys/param.h>

#include "board.h"
#include "simlnk.h"
#include "simrpc.h"

int simrpc_send(uint32_t opc, void * data, unsigned int cnt);
int simrpc_send_int(uint32_t opc, int val);
int simrpc_send_opc(uint32_t opc);

#define FLASH_MIN ((uint32_t)STM32_MEM_FLASH + FLASH_BLK_FIRMWARE_OFFS) 
#define FLASH_MAX ((uint32_t)STM32_MEM_FLASH + (256 * 1024))

#define SRAM_MIN ((uint32_t)STM32_MEM_SRAM + 4096)
#define SRAM_MAX ((uint32_t)STM32_MEM_SRAM + 32768)

#define EEPROM_MIN ((uint32_t)STM32_MEM_EEPROM)
#define EEPROM_MAX ((uint32_t)STM32_MEM_EEPROM + 8192)

uint32_t mem_base;
uint32_t mem_top;
uint32_t mem_ptr;
uint8_t mem_lock = SIMRPC_BCAST;
uint32_t mem_clk;

#define MEM_LOCK_TIMEOUT_MS 1000

void simrpc_mem_lock_svc(uint32_t opc, uint32_t * data, unsigned int cnt)
{
	uint32_t base;
	uint32_t size;
	uint32_t clk;

	if (cnt != 8) {
		DCC_LOG(LOG_WARNING, "Invalid argument size");
		return;
	};

	base = data[0];
	size = data[1];

	DCC_LOG2(LOG_TRACE, "base=%08x size=%d", base, size);

	clk = __thinkos_ticks();

	if (mem_lock == SIMRPC_BCAST) {
		if ((int32_t)(clk - mem_clk) < MEM_LOCK_TIMEOUT_MS) {
			DCC_LOG(LOG_WARNING, "locked");
			simrpc_send_int(SIMRPC_REPLY_ERR(opc), -2);
			return;
		}	
		DCC_LOG(LOG_WARNING, "lock expired");
	}

	if ((base >= FLASH_MIN) && (base < FLASH_MAX)) {
		DCC_LOG(LOG_TRACE, "Flash");
		mem_top = MIN(FLASH_MAX, base + size);
	} else if ((base >= EEPROM_MIN) && (base < EEPROM_MAX)) {
		DCC_LOG(LOG_TRACE, "EEPROM");
		mem_top = MIN(EEPROM_MAX, base + size);
	} else if ((base >= SRAM_MIN) && (base < SRAM_MAX)) {
		DCC_LOG(LOG_TRACE, "SRAM");
		mem_top = MIN(EEPROM_MAX, base + size);
	} else {
		DCC_LOG(LOG_TRACE, "Invalid");
		simrpc_send_int(SIMRPC_REPLY_ERR(opc), -2);
		return;
	}

	mem_base = base;
	mem_ptr = mem_base;
	mem_lock = SIMRPC_SRC(opc);
	mem_clk = __thinkos_ticks();
	simrpc_send_opc(SIMRPC_REPLY_OK(opc));
}

void simrpc_mem_unlock_svc(uint32_t opc, uint32_t * data, unsigned int cnt)
{
	uint32_t base;
	uint32_t size;

	if (cnt != 8) {
		DCC_LOG(LOG_WARNING, "Invalid argument size");
		return;
	};

	base = data[0];
	size = data[1];

	DCC_LOG2(LOG_TRACE, "base=%08x size=%d", base, size);

	if (mem_lock != SIMRPC_SRC(opc)) {
		DCC_LOG(LOG_WARNING, "Not yours");
		simrpc_send_int(SIMRPC_REPLY_ERR(opc), -2);
		return;
	}

	if ((base != mem_base) || (base + size != mem_top)) {
		DCC_LOG(LOG_WARNING, "Invalid lock");
		simrpc_send_int(SIMRPC_REPLY_ERR(opc), -3);
		return;
	}

	/* Sanity check */
	if ((base >= FLASH_MIN) && (base < FLASH_MAX)) {
		DCC_LOG(LOG_TRACE, "Flash");
	} else if ((base >= EEPROM_MIN) && (base < EEPROM_MAX)) {
		DCC_LOG(LOG_TRACE, "EEPROM");
	} else if ((base >= SRAM_MIN) && (base < SRAM_MAX)) {
		DCC_LOG(LOG_TRACE, "SRAM");
	} else {
		DCC_LOG(LOG_WARNING, "Internal error");
		simrpc_send_int(SIMRPC_REPLY_ERR(opc), -4);
		return;
	}

	mem_base = 0;
	mem_top = 0;
	mem_ptr = 0;
	mem_lock = 0xff;
	simrpc_send_opc(SIMRPC_REPLY_OK(opc));
}

void simrpc_mem_erase_svc(uint32_t opc, uint32_t * data, unsigned int cnt)
{
	uint32_t addr;
	uint32_t offs;
	uint32_t size;
	int ret = 0;

	if (cnt != 8) {
		DCC_LOG(LOG_WARNING, "Invalid argument size");
	};

	offs = data[0];
	addr = mem_base + offs;
	size = data[1];

	DCC_LOG2(LOG_TRACE, "addr=%08x size=%d", addr, size);
	if ((addr < mem_base) || ((addr + size) > mem_top)) {
		DCC_LOG(LOG_WARNING, "Invalid lock");
		simrpc_send_int(SIMRPC_REPLY_ERR(opc), -2);
		return;
	}

	if ((addr >= FLASH_MIN) && (addr < FLASH_MAX)) {
		DCC_LOG(LOG_TRACE, "Flash");
		ret = stm32_flash_erase(addr - (uint32_t)STM32_MEM_FLASH, size);
	} else if ((addr >= EEPROM_MIN) && (addr < EEPROM_MAX)) {
		DCC_LOG(LOG_TRACE, "EEPROM");
	} else if ((addr >= SRAM_MIN) && (addr < SRAM_MAX)) {
		__thinkos_memset32((void *)addr, 0, size);
		DCC_LOG(LOG_TRACE, "SRAM");
	} else {
		DCC_LOG(LOG_WARNING, "Internal error");
		simrpc_send_int(SIMRPC_REPLY_ERR(opc), -3);
		return;
	}

	if (ret < 0) {
		DCC_LOG(LOG_WARNING, "Memory erase error");
		simrpc_send_int(SIMRPC_REPLY_ERR(opc), -4);
		return;
	}

	simrpc_send_opc(SIMRPC_REPLY_OK(opc));
}

void simrpc_mem_read_svc(uint32_t opc, uint32_t * data, unsigned int cnt)
{
	uint32_t addr;
	uint32_t size;

	if (cnt != 4) {
		DCC_LOG(LOG_WARNING, "Invalid argument size");
	};

	size = data[0];
	addr = mem_ptr;

	DCC_LOG2(LOG_TRACE, "addr=%08x size=%d", addr, size);

	if ((addr < mem_base) || (addr > mem_top)) {
		DCC_LOG(LOG_WARNING, "Internal error");
		simrpc_send_int(SIMRPC_REPLY_ERR(opc), -2);
		return;
	}


	size = MIN(size, mem_top - addr);
	simrpc_send(SIMRPC_REPLY_OK(opc), (void *)mem_ptr, size);
	mem_ptr += size;
}

void simrpc_mem_write_svc(uint32_t opc, uint32_t * data, unsigned int cnt)
{
	uint32_t addr;
	int ret = 0;

	addr = mem_ptr;
	DCC_LOG2(LOG_TRACE, "addr=%08x cnt=%d", addr, cnt);

	if ((addr < mem_base) || (addr > mem_top)) {
		DCC_LOG(LOG_WARNING, "Internal error");
		simrpc_send_int(SIMRPC_REPLY_ERR(opc), -2);
		return;
	}

	cnt = MIN(cnt, mem_top - addr);

	if ((addr >= FLASH_MIN) && (addr < FLASH_MAX)) {
		DCC_LOG(LOG_TRACE, "Flash");
		ret = stm32_flash_write(addr - (uint32_t)STM32_MEM_FLASH, 
								data, cnt);
	} else if ((addr >= EEPROM_MIN) && (addr < EEPROM_MAX)) {
		DCC_LOG(LOG_TRACE, "EEPROM");
	} else if ((addr >= SRAM_MIN) && (addr < SRAM_MAX)) {
		DCC_LOG(LOG_TRACE, "SRAM");
		__thinkos_memcpy((void *)addr, data, cnt);
		ret = cnt;
	} else {
		DCC_LOG(LOG_WARNING, "Invalid memory");
		simrpc_send_int(SIMRPC_REPLY_ERR(opc), -3);
		return;
	}

	if (ret < 0) {
		DCC_LOG(LOG_WARNING, "Memory write error");
		simrpc_send_int(SIMRPC_REPLY_ERR(opc), -4);
		return;
	}

	simrpc_send_int(SIMRPC_REPLY_OK(opc), cnt);
	mem_ptr += cnt;
}

void simrpc_mem_seek_svc(uint32_t opc, uint32_t * data, unsigned int cnt)
{
	uint32_t addr;

	if (cnt != 4) {
		DCC_LOG(LOG_WARNING, "Invalid argument size");
	};

	addr = data[0];
	DCC_LOG1(LOG_TRACE, "addr=%08x", addr);

	if ((addr < mem_base) || (addr > mem_top)) {
		DCC_LOG(LOG_WARNING, "Invalid");
		simrpc_send_int(SIMRPC_REPLY_ERR(opc), -2);
		return;
	}

	mem_ptr = addr;
	simrpc_send_opc(SIMRPC_REPLY_OK(opc));
}

