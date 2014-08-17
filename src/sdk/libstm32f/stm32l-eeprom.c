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
 * @file stm32l-eeprom.c
 * @brief STM32L EEPROM access API
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <sys/stm32f.h>
#include <errno.h>

#include <sys/dcclog.h>

void stm32_eeprom_unlock(void)
{
	struct stm32_flash * flash = STM32_FLASH;
	uint32_t pecr;

	pecr = flash->pecr;
	DCC_LOG1(LOG_INFO, "PECR=0x%08x", pecr);
	while (pecr & FLASH_PELOCK) {
		DCC_LOG(LOG_TRACE, "unlocking EEPROM...");
		flash->pekeyr = FLASH_PEKEY1;
		flash->pekeyr = FLASH_PEKEY2;
		pecr = flash->pecr;
	}

	/* select fixed time data write */
//	flash->pecr = pecr | FLASH_FTDW;
}
	
	
/*	Bit 8 FTDW: Fixed time data write for Byte, Half Word and Word programming
		This bit is writable by software when no program or erase process is ongoing.
		This bit is used for the data EEPROM only.
		It is cleared when PELOCK is set.
		0: Programming of a Byte, Half Word or word is performed without 
		any previous erase
		operation. This is possible if the word being written to is 0x0000 0000.
		1: Before the programming of a Byte, Half Word and word an erase 
		phase is automatically
		performed. So the time of programming is fixed and lasts two tprog
		
		Bit 4 DATA: Data EEPROM selection
		This bit is writable by software when no program or erase process is ongoing.
		This bit has to be set prior to data EEPROM double word erase/programming.
		This bit is cleared when PELOCK is set.
		0: Data EEPROM not selected
		1: Data EEPROM selected;
		*/

int stm32_eeprom_wr32(uint32_t offs, uint32_t val)
{
	uint32_t * volatile eeprom = STM32_MEM_EEPROM;
	unsigned int pos;

	if (offs & 0x3) {
		DCC_LOG(LOG_ERROR, "offset must be word aligned!");
		return -EINVAL;
	}

	pos = offs >> 2;
	DCC_LOG2(LOG_INFO, "addr=0x%08x val=%08x", &eeprom[pos], val);
	eeprom[pos] = val;

	return 0;
}

int stm32_eeprom_rd32(uint32_t offs, uint32_t * val)
{
	uint32_t * eeprom = STM32_MEM_EEPROM;

	if (offs & 0x3) {
		DCC_LOG(LOG_ERROR, "offset must be word aligned!");
		return -EINVAL;
	}

	*val = eeprom[offs >> 2];

	return 0;
}

#if 0
int stm32_eeprom_wr16(uint32_t offs, uint16_t val)
{
	uint16_t * volatile eeprom = STM32_MEM_EEPROM;
	unsigned int pos;

	if (offs & 0x1) {
		DCC_LOG(LOG_ERROR, "offset must be half word aligned!");
		return -EINVAL;
	}

	pos = offs >> 1;
	DCC_LOG2(LOG_TRACE, "addr=0x%08x val=%08x", &eeprom[pos], val);
	eeprom[pos] = val;

	return 0;
}
#endif
