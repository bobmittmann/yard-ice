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
 * @file armv7m-dbg.c.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "jtag_adi.h"
#include "cm3ice.h"
#include "dbglog.h"


/* In the ARMv7-M system address map, debug resources are in the 
   Private Peripheral Bus (PPB) region. Except for the resources in the 
   System Control Space (SCS), each debug component occupies a fixed 4KB 
   address region. The resources are:

   - Debug resources in the SCS:
      — the Debug Control Block (DCB)
	  - debug controls in the System Control Block (SCB).
   - Debug components: 
	  - The Instrumentation Trace Macrocell (ITM), for profiling software. 
	    This uses non-blocking register accesses, with a fixed low-intrusion 
		overhead, and can be added to a Real-Time Operating System (RTOS), 
		application, or exception handler. If necessary, product code can 
		retain the register access instructions, avoiding probe effects.
      — The Debug Watchpoint and Trace (DWT) block. This provides watchpoint 
	     support, program counter sampling for performance monitoring, and 
		 embedded trace trigger control
	  — The Flash Patch and Breakpoint (FPB) block. This block can remap 
	    sections of ROM, typically Flash memory, to regions of RAM, and 
		can set breakpoints on code in ROM. This block can be used for debug, 
		and to provide a code or data patch to an application that 
		requires a field updates to a product ROM.
	  — The optional Embedded Trace Macrocell (ETM). This provides 
	    instruction tracing.
	  - The optional Trace Port Interface Unit (TPIU). This provides 
	    the external interface for the ITM, DWT and ETM.
   - The ROM table. A table of entries providing a mechanism to identify 
     the debug infrastructure supported by the implementation.

   PPB debug releated regions

   | Debug resource                        |  Address range        |
   +---------------------------------------+-----------------------+
   | Instrumentation Trace Macrocell (ITM) | 0xE0000000-0xE0000FFF | 
   | Data Watchpoint and Trace (DWT)       | 0xE0001000-0xE0001FFF |
   | Flash Patch and Breakpoint (FPB)      | 0xE0002000-0xE0002FFF | 
   | System Control Space (SCS)            | 0xE000ED00-0xE000EFFF |
   |   System Control Block (SCB)          | 0xE000ED00-0xE000ED8F |
   |   Debug Control Block (DCB)           | 0xE000EDF0-0xE000EEFF |
   | Trace Port Interface Unit (TPIU)      | 0xE0040000-0xE0040FFF |
   | Embedded Trace Macrocell (ETM)        | 0xE0041000-0xE0041FFF |
   | ROM table                             | 0xE00FF000-0xE00FFFFF |
   +---------------------------------------+-----------------------+
*/

#define ARMV7M_ROMSCS 0x000
#define ARMV7M_ROMSCS 0x000

/* ROM Table */
#define ARMV7M_ROM_TABLE_ADDR 0xe00ff000
/* Instrumentation Trace Macrocell */
#define ARMV7M_ITM_ADDR 0xe0000000
/* Data Watchpoint and Trace */
#define ARMV7M_DWT_ADDR 0xe0001000
/* Flash Patch and Breakpoint */
#define ARMV7M_FPB_ADDR 0xe0002000
/* System Control Space */
#define ARMV7M_SCS_ADDR 0xe000ed00
/*   System Control Block */
#define ARMV7M_SCB_ADDR 0xe000ed00
/*   Debug Control Block */
#define ARMV7M_DCB_ADDR 0xe000edf0
/* Trace Port Interface Unit */
#define ARMV7M_TPIU_ADDR 0xe0040000
/* Embedded Trace Macrocell  */
#define ARMV7M_ETM_ADDR 0xe0041000

/* check if the debug resources are implemented and 
   corresponds to the ARMv7-M profile */ 
int armv7m_probe(jtag_tap_t * tap, struct armv7m_map * map)
{
	uint32_t component_id;
	uint32_t memtype;
	uint32_t entry;
	uint32_t rom_addr;
	uint32_t cmp_addr;
	uint32_t base;
	int i = 0;

	memset(map, 0, sizeof(struct armv7m_map)); 

	/* read the debug base address register from the Access port */
	jtag_mem_ap_reg_rd(tap, ADI_AP_BASE, &base);

	if (!BASE_ENTRY_PRESENT(base)) {
		DCC_LOG(LOG_ERROR, "ROM table not present!");
		return -1;
	}

	if (!BASE_FORMAT_ADIV5(base)) {
		DCC_LOG(LOG_ERROR, "Legacy base address register format!");
		return -1;
	}

	/* get the base address */
	base = BASE_ADDR(base);
	DCC_LOG1(LOG_TRACE, "base adddr=0x%08x", base);

	if (base != ARMV7M_ROM_TABLE_ADDR) {
		DCC_LOG(LOG_ERROR, "Invalid ARMv7-M ROM table address!");
		return -1;
	}

	jtag_mem_ap_component_id_rd(tap, base, &component_id);

	if ((component_id & ID_SIGNATURE_MASK) != ID_SIGNATURE_OK) {
		DCC_LOG(LOG_ERROR, "Invalid component ID signature");
		return -1;
	}

	if (ID_CLASS(component_id) != CLASS_ROM_TABLE) {
		DCC_LOG(LOG_ERROR, "Component ID Class invalid (should be ROM table)");
		return -1;
	}

	jtag_mem_ap_rd32(tap, base + REG_MEMTYPE, &memtype);
	if (SYSTEM_MEMORY_PRESENT(memtype)) {
		DCC_LOG(LOG_TRACE, "System memory present on bus");
	}

	map->rom = base;

	rom_addr = base;
	for (i = 0; i < 16; i++) {
		jtag_mem_ap_rd32(tap, rom_addr, &entry);

		if (entry == 0)
			break;

		rom_addr += 4;

		if (!ENTRY_PRESENT(entry)) {
			DCC_LOG1(LOG_TRACE, "entry=0x%08x not present!", entry);
			continue;
		}

		cmp_addr = base + ADDRESS_OFFSET(entry);
		DCC_LOG2(LOG_TRACE, "entry=0x%08x, addr=0x%08x", entry, cmp_addr);

		switch (cmp_addr) {
		case ARMV7M_ITM_ADDR:
			/* Instrumentation Trace Macrocell */
			DCC_LOG(LOG_TRACE, "ITM");
			map->itm = cmp_addr;
			break;
		case ARMV7M_DWT_ADDR:
			/* Data Watchpoint and Trace */
			DCC_LOG(LOG_TRACE, "DWT");
			map->dwt = cmp_addr;
			break;
		case ARMV7M_FPB_ADDR:
			/* Flash Patch and Breakpoint */
			DCC_LOG(LOG_TRACE, "FPB");
			map->fpb = cmp_addr;
			break;
		case ARMV7M_SCB_ADDR:
			/* System Control Block */
			DCC_LOG(LOG_TRACE, "SCB");
			map->scb = cmp_addr;
			break;
		case ARMV7M_DCB_ADDR:
			/* Debug Control Block */
			DCC_LOG(LOG_TRACE, "DCB");
			map->dcb = cmp_addr;
			break;
		case ARMV7M_TPIU_ADDR:
			/* Trace Port Interface Unit */
			DCC_LOG(LOG_TRACE, "TPIU");
			map->tpiu = cmp_addr;
			break;
		case ARMV7M_ETM_ADDR:
			/* Embedded Trace Macrocell  */
			DCC_LOG(LOG_TRACE, "ETM");
			map->etm = cmp_addr;
			break;
		}
	}

	return 0;
}

int armv7m_info(jtag_tap_t * tap)
{
	uint32_t cpuid;
	uint32_t icsr;
	uint32_t vtor;
	uint32_t aircr;
	uint32_t scr;
	uint32_t ccr;

	jtag_mem_ap_rd32(tap, ARMV7M_CPUID, &cpuid);
	jtag_mem_ap_rd32(tap, ARMV7M_ICSR, &icsr);
	jtag_mem_ap_rd32(tap, ARMV7M_VTOR, &vtor);
	jtag_mem_ap_rd32(tap, ARMV7M_AIRCR, &aircr);
	jtag_mem_ap_rd32(tap, ARMV7M_SCR, &scr);
	jtag_mem_ap_rd32(tap, ARMV7M_CCR, &ccr);


	DCC_LOG1(LOG_TRACE, "CPUID=0x%08x", cpuid);
	DCC_LOG1(LOG_TRACE, "ICSR=0x%08x", icsr);
	DCC_LOG1(LOG_TRACE, "VTOR=0x%08x", vtor);
	DCC_LOG1(LOG_TRACE, "AIRCR=0x%08x", aircr);
	DCC_LOG1(LOG_TRACE, "SCR=0x%08x", scr);
	DCC_LOG1(LOG_TRACE, "CCR=0x%08x", ccr);

	return 0;
}

