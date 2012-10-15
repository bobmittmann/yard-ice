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
 * @file .c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/at91.h>

#include "jtag_adi.h"
#include "cm3ice.h"
#include "dbglog.h"
#include "uthreads.h"
#include "hexdump.h"


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


void print_stat(FILE * f, uint32_t stat)
{
	fprintf(f, "   ORUNDETECT=%c ", (stat & DP_ORUNDETECT) ? '1' : '0');
	fprintf(f, "STICKYORUN=%c ", (stat & DP_STICKYORUN) ? '1' : '0');
	fprintf(f, "STICKYCMP=%c ", (stat & DP_STICKYCMP) ? '1' : '0');
	fprintf(f, "STICKYERR=%c ", (stat & DP_STICKYERR) ? '1' : '0');
	fprintf(f, "READOK=%c ", (stat & DP_READOK) ? '1' : '0');
	fprintf(f, "WDATAERR=%c\n", (stat & DP_WDATAERR) ? '1' : '0');
}

void print_ctrl(FILE * f, uint32_t ctrl)
{
	fprintf(f, "   DBGRSTREQ=%c ", (ctrl & DP_CDBGRSTREQ) ? '1' : '0');
	fprintf(f, "DBGRSTACK=%c ", (ctrl & DP_CDBGRSTACK) ? '1' : '0');
	fprintf(f, "DBGPWRUPREQ=%c ", (ctrl & DP_CDBGPWRUPREQ) ? '1' : '0');
	fprintf(f, "DBGPWRUPACK=%c ", (ctrl & DP_CDBGPWRUPACK) ? '1' : '0');
	fprintf(f, "SYSPWRUPREQ=%c ", (ctrl & DP_CSYSPWRUPREQ) ? '1' : '0');
	fprintf(f, "SYSPWRUPACK=%c\n", (ctrl & DP_CSYSPWRUPACK) ? '1' : '0');
}

void print_csw(FILE * f, uint32_t csw)
{
	fprintf(f, "   DbgSwEnable=%c", (csw & CSW_DBGSW_ENABLE) ? '1' : '0');
	fprintf(f, ", MasterType=%c", (csw & CSW_MASTER_TYPE) ? '1' : '0');
	fprintf(f, ", Hprot1=%c", (csw & CSW_HPROT1) ? '1' : '0');
	fprintf(f, ", SPIDEN=%c", (csw & CSW_SPID_EN) ? '1' : '0');
	fprintf(f, ", TrInProg=%c", (csw & CSW_TR_IN_PROG) ? '1' : '0');
	fprintf(f, ", DeviceEn=%c", (csw & CSW_DEVICE_EN) ? '1' : '0');
	fprintf(f, ", AddrInc=%d", (csw & CSW_ADDR_INC) >> 4);
	fprintf(f, ", Size=%d\n", (csw & CSW_SIZE));
}

void print_idr(FILE * f, uint32_t idr)
{
	fprintf(f, "   Rev=%d", IDR_REVISION(idr));
	fprintf(f, ", JEP106=%03x", IDR_JEP106(idr));
	fprintf(f, ", Class=%d", IDR_CLASS(idr));
	fprintf(f, ", AP=(Variant=%x", IDR_AP_VARIANT(idr));
	fprintf(f, ", Type=%x)\n", IDR_AP_TYPE(idr));
}

void print_base(FILE * f, uint32_t base)
{
	fprintf(f, "   Address=%08x", BASE_ADDR(base));
	fprintf(f, ", Format=%s", BASE_FORMAT(base) ? "ADIv5" : "Legacy");
	fprintf(f, ", Present=%s\n", BASE_PRESENT(base) ? "Yes" : "No");
}


#define ID_SIGNATURE_OK 0xb105000d
#define ID_SIGNATURE_MASK 0xffff0fff
#define ID_CLASS(ID) (((ID) >> 12) & 0x0f)

#define CLASS_ROM_TABLE 0x1
#define CLASS_DEBUG 0x9
#define CLASS_GENERIC_IP 0xe
#define CLASS_GENERIC_VERIFICATION 0x0

#define JEP106_CODE_USED(ID) (((ID) >> 19) & 0x1)
#define JEP106_ID_CODE(ID) (((ID) >> 12) & 0x7f)
#define JEP106_ID_CONTINUATION_CODE(ID) (((ID) >> 32) & 0x0f)
#define BLOCKS_4KB(ID) (1 << (((ID) >> 36) & 0x0f))
#define REV_AND(ID) (((ID) >> 28) & 0x0f)
#define CUSTOMER_MODIFIED(ID) (((ID) >> 24) & 0x0f)
#define REVISION(ID) (((ID) >> 20) & 0x0f)
#define PART_NUMBER(ID) (((ID) >> 0) & 0xfff)


#define SYSTEM_MEMORY_PRESENT(TYPE) (((TYPE) >> 0) & 1)

#define ENTRY_PRESENT(ENTRY) (((ENTRY) >> 0) & 1)
#define FORMAT_32_BIT(ENTRY) (((ENTRY) >> 1) & 1)
#define ADDRESS_OFFSET(ENTRY) ((ENTRY) & 0xfffff000)

int mem_ap_component_probe(jtag_tap_t * tap, uint32_t addr);

int mem_ap_rom_table_parse(jtag_tap_t * tap, uint32_t base)
{
	int max;
	uint32_t buf[8];
	uint32_t memtype;
	uint32_t entry;
	uint32_t addr;
	int32_t offs;
	int i = 0;
	int format32bit;
	
	jtag_adi_mem_ap_rd32(tap, base + 0xfcc, &memtype);

	if (SYSTEM_MEMORY_PRESENT(memtype)) {
		DCC_LOG(LOG_TRACE, "System memory present on bus");
	}

	addr = base;
	jtag_adi_mem_ap_rd32(tap, addr, &entry);

	if ((format32bit = FORMAT_32_BIT(entry))) {
		DCC_LOG(LOG_TRACE, "32 bits format");
		/* maximum number of entries for 32bits table */
		max = 960;
	} else {
		DCC_LOG(LOG_TRACE, "8 bits format");
		jtag_adi_mem_ap_rd(tap, addr + 4, buf, 12);
		/* get the remaining bytes of the entry */
		entry |= (buf[0] << 8) | (buf[1] << 16) | (buf[2] << 24);
		/* maximum number of entries for 8bits table */
		max = 240;
	}

	while (entry != 0) {

		if (ENTRY_PRESENT(entry)) {
			offs = ADDRESS_OFFSET(entry);
			DCC_LOG2(LOG_TRACE, "entry=0x%08x, addr=0x%08x", 
					 entry, base + offs);
			mem_ap_component_probe(tap, base + offs);
		} else {
			DCC_LOG1(LOG_TRACE, "entry=0x%08x not present!", entry);
		}

		if (++i == max)
				break;
		if (format32bit) {
			addr += 4;
			jtag_adi_mem_ap_rd32(tap, addr, &entry);
		} else {
			addr += 16;
			jtag_adi_mem_ap_rd(tap, addr, buf, 16);
			entry = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
		}
	}

	return 0;
}


#define MEM_AP_COMPONENT_MAX 16

struct mem_ap_component {
	uint32_t component_id;
	uint64_t peripheral_id;
	uint32_t base_addr;
	uint32_t size;
};

typedef struct mem_ap_component mem_ap_component_t;

mem_ap_component_t mem_ap_component_poll[MEM_AP_COMPONENT_MAX];  

uint8_t mem_ap_component_cnt = 0;

void mem_ap_component_poll_init(void)
{
	mem_ap_component_cnt  = 0;
}

mem_ap_component_t * mem_ap_component_alloc(void)
{
	if (mem_ap_component_cnt < MEM_AP_COMPONENT_MAX)
		return &mem_ap_component_poll[mem_ap_component_cnt++]; 
	return NULL;
}



int mem_ap_component_probe(jtag_tap_t * tap, uint32_t addr)
{
	mem_ap_component_t * cmp;
	uint32_t buf[8];
	uint32_t component_id;
	uint64_t peripheral_id;
	uint32_t class;
	uint32_t size;
	uint32_t base;

	jtag_adi_mem_ap_rd(tap, addr + 0xff0, buf, 16);
	component_id = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);

	if ((component_id & ID_SIGNATURE_MASK) == ID_SIGNATURE_OK) {
		class = ID_CLASS(component_id);
		if (class == CLASS_ROM_TABLE)
			return mem_ap_rom_table_parse(tap, addr);

		jtag_adi_mem_ap_rd(tap, addr + 0xfd0, buf, 32);
		peripheral_id = (uint64_t)(buf[4] | (buf[5] << 8) | 
								   (buf[6] << 16) | (buf[7] << 24)) |
			(((uint64_t)(buf[0] | (buf[1] << 8) | 
						 (buf[2] << 16) | (buf[3] << 24))) << 32);

		size = (int)BLOCKS_4KB(peripheral_id)* 4096;
		base = addr - size + 4096;
	} else {
		peripheral_id = 0;
		size = 4096;
		base = addr;
	}

	if ((cmp = mem_ap_component_alloc()) == NULL) {
		DCC_LOG(LOG_TRACE, "mem_ap_component_alloc() fail!!");
		return -1;
	}

	cmp->component_id = component_id;
	cmp->peripheral_id = peripheral_id;
	cmp->base_addr = base;
	cmp->size = size;

	return 0;
}


int mem_ap_topology_probe(jtag_tap_t * tap)
{
	uint32_t addr;
	uint32_t base;
	uint32_t stat;

	mem_ap_component_poll_init();
	jtag_adi_mem_ap_reg_rd(tap, ADI_AP_BASE, &base);

	if (!BASE_PRESENT(base)) {
		DCC_LOG(LOG_TRACE, "top component not present!!!");
		return 0;
	}

	if (BASE_FORMAT(base)) {
		DCC_LOG(LOG_TRACE, "ADI v5 format");
	} else {
		DCC_LOG(LOG_TRACE, "Legacy format");
	}

	addr = BASE_ADDR(base);
	DCC_LOG1(LOG_TRACE, "base adddr=0x%08x", addr);

	mem_ap_component_probe(tap, addr);

	stat = jtag_dp_ctrl_stat_get(tap);
	if (stat & DP_STICKYERR) {
		/* clear stick error */
		jtag_dp_ctrl_set(tap, DP_STICKYERR);
		return -1;
	}

	return 0;
}

int mem_ap_topology_show(FILE * f)
{
	mem_ap_component_t * cmp;
	uint32_t component_id;
	uint64_t peripheral_id;
	uint32_t class;
	int i;


	for (i = 0; i < mem_ap_component_cnt; i++) {
		cmp = &mem_ap_component_poll[i]; 

		fprintf(f, " - 0x%08x(%d): ", cmp->base_addr, cmp->size);

		component_id = cmp->component_id;

		if ((component_id & ID_SIGNATURE_MASK) != ID_SIGNATURE_OK) {
			fprintf(f, "non standard component\n");
		} else {
			class = ID_CLASS(component_id);
			peripheral_id = cmp->peripheral_id;

			fprintf(f, "PN=%03x", (int)PART_NUMBER(peripheral_id));
			fprintf(f, ", Rev=%d", (int)REVISION(peripheral_id));
			fprintf(f, ", RevAnd=%d", (int)REV_AND(peripheral_id));
			fprintf(f, ", Custom=%d", (int)CUSTOMER_MODIFIED(peripheral_id));

			if (JEP106_CODE_USED(peripheral_id)) {
				uint32_t jep106;
				jep106 = JEP106_ID_CODE(peripheral_id) |
					JEP106_ID_CONTINUATION_CODE(peripheral_id) << 7;
				fprintf(f, ", JEP106=%03x", jep106);
			}

			fprintf(f, "\n");
		}
	} 

	return 0;
}

int cm3ice_info(cm3ice_ctrl_t * ctrl, FILE * f, uint32_t which)
{
	jtag_tap_t * tap = ctrl->tap;
	uint32_t idr;
	uint32_t csw;
	uint32_t stat;
	uint32_t tar;
	uint32_t cfg;
	uint32_t buf[8 * 256];
	uint32_t addr = 0;

	DCC_LOG1(LOG_TRACE, "which=%d", which);

	fprintf(f, "== Cortex M3 ICE ==\n");

	jtag_adi_init(tap);

#if 0
	jtag_adi_dbg_reset(tap);
#endif

	stat = jtag_dp_ctrl_stat_get(tap);
	fprintf(f, " * DP Ctrl/Stat = 0x%08x:\n", stat);
	print_stat(f, stat);
	print_ctrl(f, stat);

	jtag_adi_mem_ap_reg_rd(tap, ADI_AP_CSW, &csw);
	fprintf(f, " * AP CSW = 0x%08x:\n", csw);
	stat = jtag_dp_ctrl_stat_get(tap);
	if (stat & DP_STICKYERR) {
		print_stat(f, stat);
		/* clear stick error */
		jtag_dp_ctrl_set(tap, DP_STICKYERR);
	} else
		print_csw(f, csw);

	jtag_adi_mem_ap_reg_rd(tap, ADI_AP_TAR, &tar);
	fprintf(f, " * AP TAR = 0x%08x:\n", tar);

	jtag_adi_mem_ap_reg_rd(tap, ADI_AP_CFG, &cfg);
	fprintf(f, " * AP CFG = 0x%08x:\n", cfg);

	jtag_adi_mem_ap_reg_rd(tap, ADI_AP_IDR, &idr);
	fprintf(f, " * AP IDR = 0x%08x:\n", idr);
	stat = jtag_dp_ctrl_stat_get(tap);
	if (stat & DP_STICKYERR) {
		print_stat(f, stat);
		/* clear stick error */
		jtag_dp_ctrl_set(tap, DP_STICKYERR);
	} else
		print_idr(f, idr);

	mem_ap_topology_probe(tap);

	mem_ap_topology_show(f);

//	jtag_adi_mem_ap_rd(tap, addr, buf, 8192);
//	show_hex32(f, addr, buf, 8192);

	return 0;
}

