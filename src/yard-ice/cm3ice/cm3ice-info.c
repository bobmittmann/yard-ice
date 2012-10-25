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

#include "jtag_adi.h"
#include "cm3ice.h"
#include "dbglog.h"
#include "hexdump.h"

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
	fprintf(f, ", Format=%s", BASE_FORMAT_ADIV5(base) ? "ADIv5" : "Legacy");
	fprintf(f, ", Present=%s\n", BASE_ENTRY_PRESENT(base) ? "Yes" : "No");
}



#define MEM_AP_COMPONENT_MAX 16

struct mem_ap_component {
	uint32_t component_id;
	uint64_t peripheral_id;
	uint32_t base_addr;
	uint32_t size;
};

typedef struct mem_ap_component mem_ap_component_t;

struct mem_ap_list {
	uint32_t cnt;
	mem_ap_component_t buf[MEM_AP_COMPONENT_MAX];  
};

typedef struct mem_ap_list mem_ap_list_t;

int mem_ap_component_probe(jtag_tap_t * tap, mem_ap_list_t * lst, 
						   uint32_t addr);

int mem_ap_rom_table_parse(jtag_tap_t * tap, mem_ap_list_t * lst, 
						   uint32_t base)
{
	int max;
	uint32_t buf[8];
	uint32_t memtype;
	uint32_t entry;
	uint32_t addr;
	int32_t offs;
	int i = 0;
	int format32bit;
	
	jtag_mem_ap_rd32(tap, base + 0xfcc, &memtype);

	if (SYSTEM_MEMORY_PRESENT(memtype)) {
		DCC_LOG(LOG_TRACE, "System memory present on bus");
	}

	addr = base;
	jtag_mem_ap_rd32(tap, addr, &entry);

	if ((format32bit = FORMAT_32_BIT(entry))) {
		DCC_LOG(LOG_TRACE, "32 bits format");
		/* maximum number of entries for 32bits table */
		max = 960;
	} else {
		DCC_LOG(LOG_TRACE, "8 bits format");
		jtag_mem_ap_read(tap, addr + 4, buf, 12);
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
			mem_ap_component_probe(tap, lst, base + offs);
		} else {
			DCC_LOG1(LOG_TRACE, "entry=0x%08x not present!", entry);
		}

		if (++i == max)
				break;
		if (format32bit) {
			addr += 4;
			jtag_mem_ap_rd32(tap, addr, &entry);
		} else {
			addr += 16;
			jtag_mem_ap_read(tap, addr, buf, 16);
			entry = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
		}
	}

	return 0;
}


void mem_ap_list_init(mem_ap_list_t * lst)
{
	lst->cnt = 0;
}

mem_ap_component_t * mem_ap_component_alloc(mem_ap_list_t * lst)
{
	if (lst->cnt < MEM_AP_COMPONENT_MAX)
		return &lst->buf[lst->cnt++]; 
	return NULL;
}


int mem_ap_component_probe(jtag_tap_t * tap, mem_ap_list_t * lst, 
						   uint32_t addr)
{
	mem_ap_component_t * cmp;
	uint32_t buf[8];
	uint32_t component_id;
	uint64_t peripheral_id;
	uint32_t class;
	uint32_t size;
	uint32_t base;

	jtag_mem_ap_read(tap, addr + 0xff0, buf, 16);
	component_id = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);

	if ((component_id & ID_SIGNATURE_MASK) == ID_SIGNATURE_OK) {
		class = ID_CLASS(component_id);
		if (class == CLASS_ROM_TABLE)
			return mem_ap_rom_table_parse(tap, lst, addr);

		jtag_mem_ap_read(tap, addr + 0xfd0, buf, 32);
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

	if ((cmp = mem_ap_component_alloc(lst)) == NULL) {
		DCC_LOG(LOG_TRACE, "mem_ap_component_alloc() fail!!");
		return -1;
	}

	cmp->component_id = component_id;
	cmp->peripheral_id = peripheral_id;
	cmp->base_addr = base;
	cmp->size = size;

	return 0;
}


int mem_ap_topology_probe(jtag_tap_t * tap, mem_ap_list_t * lst)
{
	uint32_t addr;
	uint32_t base;
	uint32_t stat;

	mem_ap_list_init(lst);

	jtag_mem_ap_reg_rd(tap, ADI_AP_BASE, &base);

	if (!BASE_ENTRY_PRESENT(base)) {
		DCC_LOG(LOG_TRACE, "top component not present!!!");
		return 0;
	}

	if (BASE_FORMAT_ADIV5(base)) {
		DCC_LOG(LOG_TRACE, "ADI v5 format");
	} else {
		DCC_LOG(LOG_TRACE, "Legacy format");
	}

	addr = BASE_ADDR(base);
	DCC_LOG1(LOG_TRACE, "base adddr=0x%08x", addr);

	mem_ap_component_probe(tap, lst, addr);

	stat = jtag_dp_ctrl_stat_get(tap);
	if (stat & DP_STICKYERR) {
		/* clear stick error */
		jtag_dp_ctrl_set(tap, DP_STICKYERR);
		return -1;
	}

	return 0;
}

int mem_ap_topology_show(FILE * f, mem_ap_list_t * lst)
{
	mem_ap_component_t * cmp;
	uint32_t component_id;
	uint64_t peripheral_id;
	uint32_t class;
	int i;


	for (i = 0; i < lst->cnt; i++) {
		cmp = &lst->buf[i]; 

		fprintf(f, "  - 0x%08x(%d): ", cmp->base_addr, cmp->size);

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

void cpu_info(FILE * f, jtag_tap_t * tap)
{
	uint32_t cpuid;

	jtag_mem_ap_rd32(tap, ARMV7M_CPUID, &cpuid);
	fprintf(f, " * CPUID = 0x%08x\n", cpuid);
}

void dcb_info(FILE * f, jtag_tap_t * tap)
{
	uint32_t dfsr;
	uint32_t dhcsr;
	uint32_t demcr;

	jtag_mem_ap_rd32(tap, ARMV7M_DFSR, &dfsr);
	jtag_mem_ap_rd32(tap, ARMV7M_DHCSR, &dhcsr);
	jtag_mem_ap_rd32(tap, ARMV7M_DEMCR, &demcr);

	fprintf(f, "- Debug status:\n");

	fprintf(f, " *  DFSR = 0x%08x:", dfsr);
	if (dfsr & DFSR_EXTERNAL)
		fprintf(f, " EXTERNAL");
	if (dfsr & DFSR_VCATCH)
		fprintf(f, " VCATCH");
	if (dfsr & DFSR_DWTTRAP)
		fprintf(f, " DWTTRAP");
	if (dfsr & DFSR_BKPT)
		fprintf(f, " BKPT");
	if (dfsr & DFSR_HALTED)
		fprintf(f, " HALTED");
	fprintf(f, "\n");

	fprintf(f, " * DHCSR = 0x%08x:", dhcsr);
	if (dhcsr & DHCSR_S_RESET_ST)
		fprintf(f, " S_RESET_ST");
	if (dhcsr & DHCSR_S_RETIRE_ST)
		fprintf(f, " S_RETIRE_ST");
	if (dhcsr & DHCSR_S_LOCKUP)
		fprintf(f, " S_LOCKUP");
	if (dhcsr & DHCSR_S_SLEEP)
		fprintf(f, " S_SLEEP");
	if (dhcsr & DHCSR_S_HALT)
		fprintf(f, " S_HALT");
	if (dhcsr & DHCSR_S_REGRDY)
		fprintf(f, " S_REGRDY");
	if (dhcsr & DHCSR_C_SNAPSTALL)
		fprintf(f, " C_SNAPSTALL");
	if (dhcsr & DHCSR_C_MASKINTS)
		fprintf(f, " C_MASKINTS");
	if (dhcsr & DHCSR_C_STEP)
		fprintf(f, " C_STEP");
	if (dhcsr & DHCSR_C_HALT)
		fprintf(f, " C_HALT");
	if (dhcsr & DHCSR_C_DEBUGEN)
		fprintf(f, " C_DEBUGEN");
	fprintf(f, "\n");

	fprintf(f, " * DEMCR = 0x%08x:", demcr);
	if (demcr & DEMCR_TRCENA)
		fprintf(f, " TRCENA");
	if (demcr & DEMCR_MON_REQ)
		fprintf(f, " MON_REQ");
	if (demcr & DEMCR_MON_STEP)
		fprintf(f, " MON_STEP");
	if (demcr & DEMCR_MON_PEND)
		fprintf(f, " MON_PEND");
	if (demcr & DEMCR_MON_EN)
		fprintf(f, " MON_EN");

	if (demcr & (DEMCR_VC_HARDERR | DEMCR_VC_INTERR | DEMCR_VC_BUSERR |
				 DEMCR_VC_STATERR  | DEMCR_VC_CHKERR  | DEMCR_VC_NOCPERR |
				 DEMCR_VC_MMERR  | DEMCR_VC_CORERESET)) {
		fprintf(f, " VC_EN");
	}

	fprintf(f, "\n");
}

void fault_info(FILE * f, jtag_tap_t * tap)
{
	uint32_t cfsr;
	uint32_t ufsr;
	uint32_t bfsr;
	uint32_t bfar;

	fprintf(f, "- Fault status:\n");

	jtag_mem_ap_rd32(tap, ARMV7M_CFSR, &cfsr);

	ufsr = CFSR_UFSR_GET(cfsr);

	fprintf(f, " * UFSR = 0x%04x:", ufsr);
	if (ufsr & UFSR_DIVBYZERO)  
		fprintf(f, " DIVBYZERO");
	if (ufsr & UFSR_UNALIGNED)  
		fprintf(f, " UNALIGNED");
	if (ufsr & UFSR_NOCP)
		fprintf(f, " NOCP");
	if (ufsr & UFSR_INVPC)
		fprintf(f, " INVPC");
	if (ufsr & UFSR_INVSTATE)  
		fprintf(f, " INVSTATE");
	if (ufsr & UFSR_UNDEFINSTR)  
		fprintf(f, " UNDEFINSTR");
	fprintf(f, "\n");


	bfsr = CFSR_BFSR_GET(cfsr);

	fprintf(f, " * BFSR = 0x%04x:", bfsr);
	if (bfsr & BFSR_BFARVALID)  
		fprintf(f, " BFARVALID");
	if (bfsr & BFSR_LSPERR)
		fprintf(f, " LSPERR");
	if (bfsr & BFSR_STKERR)  
		fprintf(f, " STKERR");
	if (bfsr & BFSR_UNSTKERR)  
		fprintf(f, " INVPC");
	if (bfsr & BFSR_IMPRECISERR)  
		fprintf(f, " IMPRECISERR");
	if (bfsr & BFSR_PRECISERR)
		fprintf(f, " PRECISERR");
	if (bfsr & BFSR_IBUSERR)  
		fprintf(f, " IBUSERR");
	fprintf(f, "\n");

	if (bfsr & BFSR_BFARVALID) {
		jtag_mem_ap_rd32(tap, ARMV7M_BFAR, &bfar);
		fprintf(f, " * BFAR = 0x%04x\n", bfar);
	}
}

void dap_info(FILE * f, jtag_tap_t * tap)
{
	uint32_t idr;
	uint32_t csw;
	uint32_t stat;
	uint32_t tar;
	uint32_t cfg;

	fprintf(f, "- Debug AP:\n");

	stat = jtag_dp_ctrl_stat_get(tap);
	fprintf(f, " * DP Ctrl/Stat = 0x%08x:\n", stat);
	print_stat(f, stat);
	print_ctrl(f, stat);

	jtag_mem_ap_reg_rd(tap, ADI_AP_CSW, &csw);
	fprintf(f, " * AP CSW = 0x%08x:\n", csw);
	stat = jtag_dp_ctrl_stat_get(tap);
	if (stat & DP_STICKYERR) {
		print_stat(f, stat);
		/* clear stick error */
		jtag_dp_ctrl_set(tap, DP_STICKYERR);
	} else
		print_csw(f, csw);

	jtag_mem_ap_reg_rd(tap, ADI_AP_TAR, &tar);
	fprintf(f, " * AP TAR = 0x%08x\n", tar);

	jtag_mem_ap_reg_rd(tap, ADI_AP_CFG, &cfg);
	fprintf(f, " * AP CFG = 0x%08x\n", cfg);

	jtag_mem_ap_reg_rd(tap, ADI_AP_IDR, &idr);
	fprintf(f, " * AP IDR = 0x%08x\n", idr);
	stat = jtag_dp_ctrl_stat_get(tap);
	if (stat & DP_STICKYERR) {
		print_stat(f, stat);
		/* clear stick error */
		jtag_dp_ctrl_set(tap, DP_STICKYERR);
	} else
		print_idr(f, idr);
}

void mem_ap_info(FILE * f, jtag_tap_t * tap)
{
	struct mem_ap_list lst;

	fprintf(f, "- Mem AP:\n");

	mem_ap_topology_probe(tap, &lst);
	mem_ap_topology_show(f, &lst);
}

/* Enable vector capture */
void cm3_vc_enable(FILE * f, jtag_tap_t * tap)
{
	uint32_t demcr;

	jtag_mem_ap_rd32(tap, ARMV7M_DEMCR, &demcr);
	demcr |= DEMCR_VC_HARDERR | DEMCR_VC_INTERR | DEMCR_VC_BUSERR |
			 DEMCR_VC_STATERR  | DEMCR_VC_CHKERR  | DEMCR_VC_NOCPERR |
			 DEMCR_VC_MMERR  | DEMCR_VC_CORERESET;
	jtag_mem_ap_wr32(tap, ARMV7M_DEMCR, demcr);
	jtag_mem_ap_wr32(tap, ARMV7M_DFSR, 0x1f);

	fprintf(f, "- Vector catch enabled.\n");
}

void cm3_vc_disable(FILE * f, jtag_tap_t * tap)
{
	uint32_t demcr;

	jtag_mem_ap_rd32(tap, ARMV7M_DEMCR, &demcr);
	demcr &= ~(DEMCR_VC_HARDERR | DEMCR_VC_INTERR | DEMCR_VC_BUSERR |
			   DEMCR_VC_STATERR  | DEMCR_VC_CHKERR  | DEMCR_VC_NOCPERR |
			   DEMCR_VC_MMERR  | DEMCR_VC_CORERESET);
	jtag_mem_ap_wr32(tap, ARMV7M_DEMCR, demcr);
	jtag_mem_ap_wr32(tap, ARMV7M_DFSR, 0x1f);

	fprintf(f, "- Vector catch disabled.\n");
}



int cm3ice_info(cm3ice_ctrl_t * ctrl, FILE * f, uint32_t which)
{
	jtag_tap_t * tap = ctrl->tap;
	DCC_LOG1(LOG_TRACE, "which=%d", which);

	switch(which) {
	case 1:
		mem_ap_info(f, tap);
		break;
	case 2:
		dap_info(f, tap);
		break;
	case 3:
		cpu_info(f, tap);
		break;
	case 4:
		dcb_info(f, tap);
		break;
	case 5:
		fault_info(f, tap);
		break;
	case 6:
		cm3_vc_enable(f, tap);
		break;
	case 7:
		cm3_vc_disable(f, tap);
		break;
	default:
		fprintf(f, "== Cortex M3 ICE ==\n");
		cpu_info(f, tap);
		dcb_info(f, tap);
		fault_info(f, tap);
	}

#if 0
	jtag_mem_ap_read(tap, addr, buf, 8192);
	show_hex32(f, addr, buf, 8192);
#endif

//	jtag_adi_init(tap);

#if 0
	jtag_adi_dbg_reset(tap);
#endif

	return 0;
}

