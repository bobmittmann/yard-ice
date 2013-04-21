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
 * @file stellaris.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <crc.h>
#include <stdlib.h>

#include "target.h"
#include "script.h"
#include "cm3ice.h"
#include "target/stellaris.h"

#include <sys/dcclog.h>

/* Flash Memory Control */
#define STELLARIS_BASE_FC 0x400fd000
/* System Control */
#define STELLARIS_BASE_SC 0x400fe000

/* Flash Memory Control Registers (Flash Control Offset) */
#define FC_FMA 	0x000 /* Flash Memory Address */
#define FC_FMD 	0x004 /* Flash Memory Data */
#define FC_FMC 	0x008 /* Flash Memory Control*/
#define FC_FCRIS 	0x00c /* Flash Controller Raw Interrupt Status */
#define FC_FCIM 	0x010 /* Flash Controller Interrupt Mask */
#define FC_FCMISC 	0x014 /* Flash Controller Masked Interrupt Status and Clear */

/* Flash Memory Protection Registers (System Control Offset) */
#define SC_FMPRE0 	0x130 /* Flash Memory Protection Read Enable 0 */
#if 0
#define SC_FMPRE0 	0x200 /* Flash Memory Protection Read Enable 0 */
#endif
#define SC_FMPPE0 	0x134 /* Flash Memory Protection Program Enable 0 */
#if 0
#define SC_FMPPE0 	0x400 /* Flash Memory Protection Program Enable 0 */
#endif

#define SC_USECRL 	0x140 /* USec Reload */
#define SC_USER_DBG 	0x1d0 /* User Debug */
#define SC_USER_REG0 	0x1e0 /* User Register 0 */
#define SC_USER_REG1 	0x1e4 /* User Register 1 */
#define SC_FMPRE1 	0x204 /* Flash Memory Protection Read Enable 1 */
#define SC_FMPRE2 	0x208 /* Flash Memory Protection Read Enable 2 */
#define SC_FMPRE3 	0x20c /* Flash Memory Protection Read Enable 3 */
#define SC_FMPPE1 	0x404 /* Flash Memory Protection Program Enable 1 */
#define SC_FMPPE2 	0x408 /* Flash Memory Protection Program Enable 2 */
#define SC_FMPPE3 	0x40c /* Flash Memory Protection Program Enable 3 */

#define FMC_WRKEY 0xa4420000 /* (RO) Flash write key 
	This field contains a write key, which is used to minimize the 
	incidence of accidental flash writes. The value 0xA442 must be written 
	into this field for a write to occur. Writes to the FMC register 
	without this WRKEY value are ignored. A read of this field returns the 
	value 0. */

#define FMC_COMT (1 << 3) /* (R/W) Commit Register Value
	Commit (write) of register value to nonvolatile storage. A write of 0 has
	no effect on the state of this bit.
	If read, the state of the previous commit access is provided. If the
	previous commit access is complete, a 0 is returned; otherwise, if the
	commit access is not complete, a 1 is returned.
	This can take up to 50 μs. */

#define FMC_MERASE (1 << 2) /* (R/W) Mass Erase Flash Memory
	If this bit is set, the flash main memory of the device is all erased. A
	write of 0 has no effect on the state of this bit.
	If read, the state of the previous mass erase access is provided. If the
	previous mass erase access is complete, a 0 is returned; otherwise, if
	the previous mass erase access is not complete, a 1 is returned.
	This can take up to 250 ms. */

#define FMC_ERASE (1 << 1) /* (R/W)  Erase a Page of Flash Memory
	  If this bit is set, the page of flash main memory as specified by the
	  contents of FMA is erased. A write of 0 has no effect on the state of 
	  this bit.
	  If read, the state of the previous erase access is provided. 
	  If the previous erase access is complete, a 0 is returned; otherwise, 
	  if the previous erase access is not complete, a 1 is returned.
	  This can take up to 25 ms. */

#define FMC_WRITE (1 << 0) /* (R/W) Write a Word into Flash Memory
	  If this bit is set, the data stored in FMD is written into the 
	  location as specified by the contents of FMA. A write of 0 has no 
	  effect on the state of this bit.
	  If read, the state of the previous write update is provided. 
	  If the previous write access is complete, a 0 is returned; otherwise, 
	  if the write access
	  is not complete, a 1 is returned.
	  This can take up to 50 μs. */

int stellaris_flash_erase(cm3ice_ctrl_t * ctrl, uint32_t base, 
						  uint32_t offs, int len)
{
	uint32_t stat;
	int again;
	int page;

	page = (base + offs) & 0xfffffc00;

	DCC_LOG2(LOG_INFO, "%08x len=%d", page, len);

	cm3ice_wr32(ctrl, STELLARIS_BASE_FC + FC_FMA, page);

	cm3ice_wr32(ctrl, STELLARIS_BASE_FC + FC_FMC, FMC_WRKEY | FMC_ERASE);

	for (again = 4096; ; again--) {
		cm3ice_rd32(ctrl, STELLARIS_BASE_FC + FC_FMC, &stat);
		if ((stat & (FMC_MERASE | FMC_ERASE | FMC_WRITE)) == 0)
			break;
		if (again == 0) {
			DCC_LOG(LOG_WARNING, "flash not ready!");
			return -1;
		}
	}

	return 1024;
}

int stellaris_flash_wr32(cm3ice_ctrl_t * ctrl, uint32_t addr, uint32_t data)
{
	uint32_t stat;
	int again;

	DCC_LOG2(LOG_INFO, "0x%08x <-- 0x%08x", addr, data);

	cm3ice_wr32(ctrl, STELLARIS_BASE_FC + FC_FMA, addr);
	cm3ice_wr32(ctrl, STELLARIS_BASE_FC + FC_FMD, data);

	cm3ice_wr32(ctrl, STELLARIS_BASE_FC + FC_FMC, FMC_WRKEY | FMC_WRITE);
	
	for (again = 4096; ; again--) {
		cm3ice_rd32(ctrl, STELLARIS_BASE_FC + FC_FMC, &stat);
		if ((stat & (FMC_MERASE | FMC_ERASE | FMC_WRITE)) == 0)
			break;
		if (again == 0) {
			DCC_LOG(LOG_WARNING, "flash not ready!");
			return -1;
		}
	}

	return 0;
}


int stellaris_flash_write(cm3ice_ctrl_t * ctrl, uint32_t base, 
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
		stellaris_flash_wr32(ctrl, addr, data);
		ptr += 4;
		addr += 4;
	}
	
	return n * 4;
}

const struct cm3ice_cfg stellaris_cfg = {
	.endianness = LITTLE_ENDIAN,
};

/*
 * stellaris flash
 */
const struct ice_mem_oper flash_stellaris_oper = {
	.read = (ice_mem_read_t)cm3ice_mem_read,
	.write = (ice_mem_write_t)stellaris_flash_write, 
	.erase = (ice_mem_erase_t)stellaris_flash_erase
};

