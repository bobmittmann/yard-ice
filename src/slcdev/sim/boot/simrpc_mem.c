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
#include "simrpc_svc.h"
#include "crc32.h"

int __simrpc_send(uint32_t opc, void * data, unsigned int cnt);
int __simrpc_send_int(uint32_t opc, int val);
int __simrpc_send_opc(uint32_t opc);

#define FLASH_MIN ((uint32_t)STM32_MEM_FLASH + FLASH_BLK_FIRMWARE_OFFS) 
#define FLASH_MAX ((uint32_t)STM32_MEM_FLASH + (256 * 1024))

#define SRAM_MIN ((uint32_t)STM32_MEM_SRAM + 2048)
#define SRAM_MAX ((uint32_t)STM32_MEM_SRAM + 32768)

#define EEPROM_MIN ((uint32_t)STM32_MEM_EEPROM)
#define EEPROM_MAX ((uint32_t)STM32_MEM_EEPROM + 8192)

#define SIMRPC_UNLOCK SIMRPC_ADDR_BCAST

struct {
	uint32_t base;
	uint32_t top;
	uint32_t ptr;
	uint32_t clk;
	uint8_t lock;
} simrpc_mem = {
	.base = 0,
	.top = 0,
	.ptr = 0,
	.clk = 0,
	.lock = SIMRPC_UNLOCK
};

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

	if (simrpc_mem.lock != SIMRPC_UNLOCK) {
		if ((int32_t)(clk - simrpc_mem.clk) < MEM_LOCK_TIMEOUT_MS) {
			DCC_LOG(LOG_WARNING, "locked");
			__simrpc_send_int(SIMRPC_REPLY_ERR(opc), -2);
			return;
		}	
		DCC_LOG(LOG_WARNING, "lock expired");
	}

	if ((base >= FLASH_MIN) && (base < FLASH_MAX)) {
		DCC_LOG(LOG_TRACE, "Flash");
		simrpc_mem.top = MIN(FLASH_MAX, base + size);
	} else if ((base >= EEPROM_MIN) && (base < EEPROM_MAX)) {
		DCC_LOG(LOG_TRACE, "EEPROM");
		simrpc_mem.top = MIN(EEPROM_MAX, base + size);
	} else if ((base >= SRAM_MIN) && (base < SRAM_MAX)) {
		DCC_LOG(LOG_TRACE, "SRAM");
		simrpc_mem.top = MIN(EEPROM_MAX, base + size);
	} else {
		DCC_LOG(LOG_TRACE, "Invalid");
		__simrpc_send_int(SIMRPC_REPLY_ERR(opc), -2);
		return;
	}

	simrpc_mem.base = base;
	simrpc_mem.ptr = simrpc_mem.base;
	simrpc_mem.lock = SIMRPC_SRC(opc);
	simrpc_mem.clk = __thinkos_ticks();
	__simrpc_send_opc(SIMRPC_REPLY_OK(opc));
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

	if (simrpc_mem.lock != SIMRPC_SRC(opc)) {
		DCC_LOG(LOG_WARNING, "Not yours");
		__simrpc_send_int(SIMRPC_REPLY_ERR(opc), -2);
		return;
	}

	if ((base != simrpc_mem.base) || (base + size != simrpc_mem.top)) {
		DCC_LOG(LOG_WARNING, "Invalid lock");
		__simrpc_send_int(SIMRPC_REPLY_ERR(opc), -3);
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
		__simrpc_send_int(SIMRPC_REPLY_ERR(opc), -4);
		return;
	}

	simrpc_mem.base = 0;
	simrpc_mem.top = 0;
	simrpc_mem.ptr = 0;
	simrpc_mem.lock = 0xff;
	__simrpc_send_opc(SIMRPC_REPLY_OK(opc));
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
	addr = simrpc_mem.base + offs;
	size = data[1];

	DCC_LOG2(LOG_TRACE, "addr=%08x size=%d", addr, size);
	if ((addr < simrpc_mem.base) || ((addr + size) > simrpc_mem.top)) {
		DCC_LOG(LOG_WARNING, "Invalid lock");
		__simrpc_send_int(SIMRPC_REPLY_ERR(opc), -2);
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
		__simrpc_send_int(SIMRPC_REPLY_ERR(opc), -3);
		return;
	}

	if (ret < 0) {
		DCC_LOG(LOG_WARNING, "Memory erase error");
		__simrpc_send_int(SIMRPC_REPLY_ERR(opc), -4);
		return;
	}

	__simrpc_send_opc(SIMRPC_REPLY_OK(opc));
	simrpc_mem.clk = __thinkos_ticks();
}

void simrpc_mem_read_svc(uint32_t opc, uint32_t * data, unsigned int cnt)
{
	uint32_t addr;
	uint32_t size;

	if (cnt != 4) {
		DCC_LOG(LOG_WARNING, "Invalid argument size");
	};

	size = data[0];
	addr = simrpc_mem.ptr;

	DCC_LOG2(LOG_TRACE, "addr=%08x size=%d", addr, size);

	if ((addr < simrpc_mem.base) || (addr > simrpc_mem.top)) {
		DCC_LOG(LOG_WARNING, "Internal error");
		__simrpc_send_int(SIMRPC_REPLY_ERR(opc), -2);
		return;
	}


	size = MIN(size, simrpc_mem.top - addr);
	__simrpc_send(SIMRPC_REPLY_OK(opc), (void *)simrpc_mem.ptr, size);
	simrpc_mem.ptr += size;
	simrpc_mem.clk = __thinkos_ticks();
}

void simrpc_mem_write_svc(uint32_t opc, uint32_t * data, unsigned int cnt)
{
	uint32_t addr;
	int ret = 0;

	addr = simrpc_mem.ptr;
	DCC_LOG2(LOG_TRACE, "addr=%08x cnt=%d", addr, cnt);

	if ((addr < simrpc_mem.base) || (addr > simrpc_mem.top)) {
		DCC_LOG(LOG_WARNING, "Internal error");
		__simrpc_send_int(SIMRPC_REPLY_ERR(opc), -2);
		return;
	}

	cnt = MIN(cnt, simrpc_mem.top - addr);

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
		__simrpc_send_int(SIMRPC_REPLY_ERR(opc), -3);
		return;
	}

	if (ret < 0) {
		DCC_LOG(LOG_WARNING, "Memory write error");
		__simrpc_send_int(SIMRPC_REPLY_ERR(opc), -4);
		return;
	}

	__simrpc_send_int(SIMRPC_REPLY_OK(opc), cnt);
	simrpc_mem.ptr += cnt;
	simrpc_mem.clk = __thinkos_ticks();
}

void simrpc_mem_seek_svc(uint32_t opc, uint32_t * data, unsigned int cnt)
{
	uint32_t addr;
	uint32_t offs;

	if (cnt != 4) {
		DCC_LOG(LOG_WARNING, "Invalid argument size");
		return;
	};

	offs = data[0];
	addr = simrpc_mem.base + offs;
	DCC_LOG1(LOG_TRACE, "addr=%08x", addr);

	if ((addr < simrpc_mem.base) || (addr > simrpc_mem.top)) {
		DCC_LOG(LOG_WARNING, "Invalid");
		__simrpc_send_int(SIMRPC_REPLY_ERR(opc), -2);
		return;
	}

	simrpc_mem.ptr = addr;
	__simrpc_send_opc(SIMRPC_REPLY_OK(opc));
	simrpc_mem.clk = __thinkos_ticks();
}

void simrpc_mem_crc_svc(uint32_t opc, uint32_t * data, unsigned int cnt)
{
	uint32_t addr;
	uint32_t offs;
	uint32_t size;
	uint32_t crc;

	if (cnt != 8) {
		DCC_LOG(LOG_WARNING, "Invalid argument size");
	};

	offs = data[0];
	addr = simrpc_mem.base + offs;
	size = data[1];

	if ((addr < simrpc_mem.base) || (addr > simrpc_mem.top)) {
		DCC_LOG(LOG_WARNING, "Invalid");
		__simrpc_send_int(SIMRPC_REPLY_ERR(opc), -2);
		return;
	}

	DCC_LOG2(LOG_TRACE, "addr=%08x size=%d", addr, size);
	if ((addr < simrpc_mem.base) || ((addr + size) > simrpc_mem.top)) {
		DCC_LOG(LOG_WARNING, "Invalid lock");
		__simrpc_send_int(SIMRPC_REPLY_ERR(opc), -2);
		return;
	}

	/* Sanity check */
	if ((addr >= FLASH_MIN) && (addr < FLASH_MAX)) {
		DCC_LOG(LOG_TRACE, "Flash");
	} else if ((addr >= EEPROM_MIN) && (addr < EEPROM_MAX)) {
		DCC_LOG(LOG_TRACE, "EEPROM");
	} else if ((addr >= SRAM_MIN) && (addr < SRAM_MAX)) {
		DCC_LOG(LOG_TRACE, "SRAM");
	} else {
		DCC_LOG(LOG_WARNING, "Internal error");
		__simrpc_send_int(SIMRPC_REPLY_ERR(opc), -4);
		return;
	}

	if (size > (simrpc_mem.top - addr))
		size = (simrpc_mem.top - addr);

	if ((addr & 0x3) == 0)
		crc = crc32_align((void *)addr, size);
	else
		crc = crc32((void *)addr, size);

	__simrpc_send_int(SIMRPC_REPLY_OK(opc), crc);

	simrpc_mem.clk = __thinkos_ticks();
}

