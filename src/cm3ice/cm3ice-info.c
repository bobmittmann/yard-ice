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
 * @file cm3ice-info.c.c
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
#include "arm-fpb.h"
#include "arm-dwt.h"

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

static bool dp_stickyerr_check(FILE * f, jtag_tap_t * tap)
{
	uint32_t stat;

	stat = jtag_dp_ctrl_stat_get(tap);

	if (stat & DP_STICKYERR) {
		fprintf(f, " * clearing STICKYERR!\n");
		/* clear stick error */
		jtag_dp_ctrl_set(tap, DP_STICKYERR);
		return true;
	}

	return false;
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
			(void)class;
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

static void debug_info(FILE * f, jtag_tap_t * tap)
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
	if (demcr & DEMCR_VC_HARDERR)
		fprintf(f, " VC_HARDERR");
	if (demcr & DEMCR_VC_INTERR)
		fprintf(f, " VC_INTERR");
	if (demcr & DEMCR_VC_BUSERR)
		fprintf(f, " VC_BUSERR");
	if (demcr & DEMCR_VC_STATERR)
		fprintf(f, " VC_STATERR");
	if (demcr & DEMCR_VC_CHKERR)
		fprintf(f, " VC_CHKERR");
	if (demcr & DEMCR_VC_NOCPERR)
		fprintf(f, " VC_NOCPERR");
	if (demcr & DEMCR_VC_MMERR)
		fprintf(f, " VC_MMERR");
	if (demcr & DEMCR_VC_CORERESET)
		fprintf(f, " VC_CORERESET");

/*
	if (demcr & (DEMCR_VC_HARDERR | DEMCR_VC_INTERR | DEMCR_VC_BUSERR |
				 DEMCR_VC_STATERR  | DEMCR_VC_CHKERR  | DEMCR_VC_NOCPERR |
				 DEMCR_VC_MMERR  | DEMCR_VC_CORERESET)) {
		fprintf(f, " VC_EN");
	}
*/
	fprintf(f, "\n");
}

void csb_misc_info(FILE * f, jtag_tap_t * tap)
{
	uint32_t cpuid;
	uint32_t icsr;
	uint32_t vtor;
	uint32_t aircr;
	uint32_t scr;
	uint32_t ccr;
	uint32_t shpr1;
	uint32_t shpr2;
	uint32_t shpr3;
	uint32_t shcsr;

	fprintf(f, "- CSB registers:\n");

	jtag_mem_ap_rd32(tap, ARMV7M_CPUID, &cpuid);
	fprintf(f, " * CPUID = 0x%08x\n", cpuid);

	jtag_mem_ap_rd32(tap, ARMV7M_ICSR, &icsr);
	fprintf(f, " *  ICSR = 0x%08x:", icsr);
	fprintf(f, "%s%s%s%s%s%s VECTPENDING=%d VECTACTIVE=%d\n", 
			(icsr & ICSR_NMIPENDSET) ? " NMIPEND" : "",
			(icsr & ICSR_PENDSVSET) ? " PENDSV" : "",
			(icsr & ICSR_PENDSTSET) ? " PENDST" : "",
			(icsr & ICSR_ISRPREEMPT) ? " ISRPREEMPT" : "",
			(icsr & ICSR_ISRPENDING) ? " ISRPENDING" : "",
			(icsr & ICSR_RETTOBASE) ? " RETTOBASE" : "",
			(icsr & ICSR_VECTPENDING) >> 12,
			(icsr & ICSR_VECTACTIVE));

	jtag_mem_ap_rd32(tap, ARMV7M_VTOR, &vtor);
	fprintf(f, " *  VTOR = 0x%08x\n", vtor);

	jtag_mem_ap_rd32(tap, ARMV7M_AIRCR, &aircr);
	fprintf(f, " * AIRCR = 0x%08x\n", aircr);

	jtag_mem_ap_rd32(tap, ARMV7M_SCR, &scr);
	fprintf(f, " *   SCR = 0x%08x\n", scr);

	jtag_mem_ap_rd32(tap, ARMV7M_CCR, &ccr);
	fprintf(f, " *   CCR = 0x%08x\n", ccr);

	jtag_mem_ap_rd32(tap, ARMV7M_SHPR1, &shpr1);
	fprintf(f, " * SHPR1 = 0x%08x\n", shpr1);

	jtag_mem_ap_rd32(tap, ARMV7M_SHPR2, &shpr2);
	fprintf(f, " * SHPR2 = 0x%08x\n", shpr2);

	jtag_mem_ap_rd32(tap, ARMV7M_SHPR3, &shpr3);
	fprintf(f, " * SHPR3 = 0x%08x\n", shpr3);

	jtag_mem_ap_rd32(tap, ARMV7M_SHCSR, &shcsr);
	fprintf(f, " * SHCSR = 0x%08x:", shcsr);
	fprintf(f, "%s%s%s%s%s%s%s\n", 
			(shcsr & SHCSR_SYSTICKACT) ? " SYSTICKACT" : "",
			(shcsr & SHCSR_PENDSVACT) ? " PENDSVACT" : "",
			(shcsr & SHCSR_MONITORACT) ? " MONITORACT" : "",
			(shcsr & SHCSR_SVCALLACT) ? " SVCALLACT" : "",
			(shcsr & SHCSR_USGFAULTACT) ?  " USGFAULTACT" : "",
			(shcsr & SHCSR_BUSFAULTACT) ?  " BUSFAULTACT" : "",
			(shcsr & SHCSR_MEMFAULTACT) ?  " MEMFAULTACT" : "");
}



void fault_info(FILE * f, jtag_tap_t * tap)
{
	uint32_t hfsr;
	uint32_t cfsr;
	uint32_t ufsr;
	uint32_t bfsr;
	uint32_t bfar;
	uint32_t mmfsr;
	uint32_t mmfar;
	uint32_t afsr;

	fprintf(f, "- Fault status:\n");

	jtag_mem_ap_rd32(tap, ARMV7M_HFSR, &hfsr);
	fprintf(f, " *  HFSR = 0x%08x:", hfsr);
	fprintf(f, "%s%s%s\n", 
			(hfsr & HFSR_DEBUGEVT) ? " DEBUGEVT" : "",
			(hfsr & HFSR_FORCED) ?  " FORCED" : "",
			(hfsr & HFSR_VECTTBL) ? " VECTTBL" : "");

	jtag_mem_ap_rd32(tap, ARMV7M_CFSR, &cfsr);
	ufsr = CFSR_UFSR_GET(cfsr);

	fprintf(f, " *  UFSR = 0x%04x:", ufsr);
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

	fprintf(f, " *  BFSR = 0x%04x:", bfsr);
	if (bfsr & BFSR_BFARVALID)  
		fprintf(f, " BFARVALID");
	if (bfsr & BFSR_LSPERR)
		fprintf(f, " LSPERR");
	if (bfsr & BFSR_STKERR)  
		fprintf(f, " STKERR");
	if (bfsr & BFSR_UNSTKERR)  
		fprintf(f, " UNSTKERR");
	if (bfsr & BFSR_IMPRECISERR)  
		fprintf(f, " IMPRECISERR");
	if (bfsr & BFSR_PRECISERR)
		fprintf(f, " PRECISERR");
	if (bfsr & BFSR_IBUSERR)  
		fprintf(f, " IBUSERR");
	fprintf(f, "\n");

	if (bfsr & BFSR_BFARVALID) {
		jtag_mem_ap_rd32(tap, ARMV7M_BFAR, &bfar);
		fprintf(f, " *  BFAR = 0x%08x\n", bfar);
	}

	mmfsr = CFSR_MMFSR_GET(cfsr);

	fprintf(f, " * MMFSR = 0x%02x:", mmfsr);
	if (mmfsr & MMFSR_MMARVALID)  
		fprintf(f, " MMARVALID");
	if (mmfsr & MMFSR_MLSPERR)  
		fprintf(f, " MLSPERR");
	if (mmfsr & MMFSR_MSTKERR)  
		fprintf(f, " MSTKERR");
	if (mmfsr & MMFSR_MUNSTKERR)  
		fprintf(f, " MUNSTKERR");
	if (mmfsr & MMFSR_DACCVIOL)  
		fprintf(f, " DACCVIOL");
	if (mmfsr & MMFSR_IACCVIOL)  
		fprintf(f, " IACCVIOL");
	fprintf(f, "\n");

	if (mmfsr & MMFSR_MMARVALID) {
		jtag_mem_ap_rd32(tap, ARMV7M_MMFAR, &mmfar);
		fprintf(f, " * MMFAR = 0x%08x\n", mmfar);
	}

	jtag_mem_ap_rd32(tap, ARMV7M_AFSR, &afsr);
	fprintf(f, " *  AFSR = 0x%08x\n", afsr);
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
	if (stat & DP_STICKYERR) {
		/* clear stick error */
		jtag_dp_ctrl_set(tap, DP_STICKYERR);
	} 

	jtag_mem_ap_reg_rd(tap, ADI_AP_CSW, &csw);
	stat = jtag_dp_ctrl_stat_get(tap);
	if (stat & DP_STICKYERR) {
		fprintf(f, " * AP CSW acess error!\n");
		/* clear stick error */
		jtag_dp_ctrl_set(tap, DP_STICKYERR);
	} else {
		fprintf(f, " * AP CSW = 0x%08x:\n", csw);
		print_csw(f, csw);
	}

	jtag_mem_ap_reg_rd(tap, ADI_AP_TAR, &tar);
	fprintf(f, " * AP TAR = 0x%08x\n", tar);

	jtag_mem_ap_reg_rd(tap, ADI_AP_CFG, &cfg);
	fprintf(f, " * AP CFG = 0x%08x\n", cfg);

	jtag_mem_ap_reg_rd(tap, ADI_AP_IDR, &idr);
	stat = jtag_dp_ctrl_stat_get(tap);
	if (stat & DP_STICKYERR) {
		fprintf(f, " * AP IDR acess error!\n");
		/* clear stick error */
		jtag_dp_ctrl_set(tap, DP_STICKYERR);
	} else {
		fprintf(f, " * AP IDR = 0x%08x\n", idr);
		print_idr(f, idr);
	}
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

#define FP_CTRL_OFFS  0x00000000 /* FlashPatch Control Register */
#define FP_REMAP_OFFS 0x00000004 /* FlashPatch Remap register */
#define FP_COMP0_OFFS 0x00000008 /* FlashPatch Comparator register 0 */
#define FP_LSR_OFFS   0x00000fb4 /*  */
void fpb_info(FILE * f, jtag_tap_t * tap)
{
	uint32_t ctrl;
	uint32_t remap;
	uint32_t lsr;
	int i;

	fprintf(f, "- FPB (Flash Patch and Breakpoint):\n");

	if (jtag_mem_ap_rd32(tap, ARMV7M_FPB_ADDR + FP_CTRL_OFFS, 
						 &ctrl) != JTAG_ADI_ACK_OK_FAULT)
		return;
	if (jtag_mem_ap_rd32(tap, ARMV7M_FPB_ADDR + FP_REMAP_OFFS, 
						 &remap) != JTAG_ADI_ACK_OK_FAULT)
		return;
	if (jtag_mem_ap_rd32(tap, ARMV7M_FPB_ADDR + FP_LSR_OFFS, 
						 &lsr) != JTAG_ADI_ACK_OK_FAULT)
		return;

	fprintf(f, " *  CTRL = 0x%08x\n", ctrl);
	fprintf(f, " * REMAP = 0x%08x\n", remap);
	fprintf(f, " *   LSR = 0x%08x\n", lsr);

	fprintf(f, "   ID       COMP TYPE\n");
	for (i = 0; i < FP_NUM_CODE(ctrl) + FP_NUM_LIT(ctrl); ++i) {
		uint32_t comp;

		if (jtag_mem_ap_rd32(tap, ARMV7M_FPB_ADDR + FP_COMP0_OFFS + 4 * i, 
							 &comp) != JTAG_ADI_ACK_OK_FAULT)
			return;
		fprintf(f, "   %2d 0x%08x %s\n", i, comp, 
				i < FP_NUM_CODE(ctrl) ? "code" : "literal" );
	}
}

#define DWT_CTRL_OFFS     0
#define DWT_CYCCNT_OFFS   4
#define DWT_CPICNT_OFFS   8
#define DWT_EXCCNT_OFFS   12
#define DWT_SLEEPCNT_OFFS 16 
#define DWT_LSUCNT_OFFS   20
#define DWT_FOLDCNT_OFFS  24 
#define DWT_PCSR_OFFS     28
#define DWT_COMP0_OFFS    32

#define COMP_OFFS 0
#define MASK_OFFS 4
#define FUNC_OFFS 8
void dwt_info(FILE * f, jtag_tap_t * tap)
{
	uint32_t ctrl; /* RW - Control Register */
	uint32_t cyccnt; /* RW -Cycle Count Register */
	uint32_t cpicnt; /* RW - CPI Count Register */
	uint32_t exccnt; /* RW - Exception Overhead Count Register */
	uint32_t sleepcnt; /* RW - Sleep Count Register */
	uint32_t lsucnt; /* RW - LSU Count Register */
	uint32_t foldcnt; /* RW - Folded-instruction Count Register */
	uint32_t pcsr; /* RO - Program Counter Sample Register */
	int i;

	fprintf(f, "- DWT (Data Watchpoint and Trace Unit):\n");

	if (jtag_mem_ap_rd32(tap, ARMV7M_DWT_ADDR + DWT_CTRL_OFFS, 
						 &ctrl) != JTAG_ADI_ACK_OK_FAULT)
		return;
	if (jtag_mem_ap_rd32(tap, ARMV7M_DWT_ADDR + DWT_CYCCNT_OFFS, 
						 &cyccnt) != JTAG_ADI_ACK_OK_FAULT)
		return;
	if (jtag_mem_ap_rd32(tap, ARMV7M_DWT_ADDR + DWT_CPICNT_OFFS, 
						 &cpicnt ) != JTAG_ADI_ACK_OK_FAULT)
		return;
	if (jtag_mem_ap_rd32(tap, ARMV7M_DWT_ADDR + DWT_EXCCNT_OFFS, 
						 &exccnt) != JTAG_ADI_ACK_OK_FAULT)
		return;
	if (jtag_mem_ap_rd32(tap, ARMV7M_DWT_ADDR + DWT_SLEEPCNT_OFFS, 
						 &sleepcnt) != JTAG_ADI_ACK_OK_FAULT)
		return;
	if (jtag_mem_ap_rd32(tap, ARMV7M_DWT_ADDR + DWT_LSUCNT_OFFS, 
						 &lsucnt) != JTAG_ADI_ACK_OK_FAULT)
		return;
	if (jtag_mem_ap_rd32(tap, ARMV7M_DWT_ADDR + DWT_FOLDCNT_OFFS, 
						 &foldcnt) != JTAG_ADI_ACK_OK_FAULT)
		return;
	if (jtag_mem_ap_rd32(tap, ARMV7M_DWT_ADDR + DWT_PCSR_OFFS, 
						 &pcsr) != JTAG_ADI_ACK_OK_FAULT)
		return;
	fprintf(f, " *     CTRL = 0x%08x\n", ctrl);
	fprintf(f, " *   CYCCNT = 0x%08x\n", cyccnt);
	fprintf(f, " *   CPICNT = 0x%08x\n", cpicnt );
	fprintf(f, " *   EXCCNT = 0x%08x\n", exccnt);
	fprintf(f, " * SLEEPCNT = 0x%08x\n", sleepcnt);
	fprintf(f, " *   LSUCNT = 0x%08x\n", lsucnt);
	fprintf(f, " *  FOLDCNT = 0x%08x\n", foldcnt);
	fprintf(f, " *     PCSR = 0x%08x\n", pcsr);
	fprintf(f, "   ID       COMP       MASK       FUNC\n");
	
	for (i = 0; i < DWT_NUMCOMP(ctrl); ++i) {
		uint32_t base;
		uint32_t comp;
		uint32_t mask;
		uint32_t func;

		base = ARMV7M_DWT_ADDR + DWT_COMP0_OFFS + 16 * i;
		/* Reset the function register */
		if (jtag_mem_ap_rd32(tap, base + COMP_OFFS, 
							 &comp) != JTAG_ADI_ACK_OK_FAULT) {
			return;
		}
		if (jtag_mem_ap_rd32(tap, base + MASK_OFFS, 
							 &mask) != JTAG_ADI_ACK_OK_FAULT) {
			return;
		}
		if (jtag_mem_ap_rd32(tap, base + FUNC_OFFS, 
							 &func) != JTAG_ADI_ACK_OK_FAULT) {
			return;
		}
		fprintf(f, "   %2d 0x%08x 0x%08x 0x%08x\n", i, comp, mask, func);
	}
}

void mpu_info(FILE * f, jtag_tap_t * tap)
{
	uint32_t type;
	uint32_t ctrl;
	uint32_t rnr;
	uint32_t rbar;
	uint32_t rasr;
	int i;

	fprintf(f, "- MPU (Memory Protection Unit):\n");

	if (jtag_mem_ap_rd32(tap, ARMV7M_MPU_TYPE, &type) != JTAG_ADI_ACK_OK_FAULT)
		return;

	if (jtag_mem_ap_rd32(tap, ARMV7M_MPU_CTRL, &ctrl) != JTAG_ADI_ACK_OK_FAULT)
		return;

	/* TODO: decode type and control */
	fprintf(f, " * TYPE = 0x%08x\n", type);
	fprintf(f, " * CTRL = 0x%08x\n", ctrl);

	fprintf(f, " *  Reg |       RBAR |       RASR |\n");
	for (i = 0; i < MPU_TYPE_DREGION(type); ++i) {
		rnr = i;
		if (jtag_mem_ap_wr32(tap, ARMV7M_MPU_RNR, rnr) != 
			JTAG_ADI_ACK_OK_FAULT)
			return;

		if (jtag_mem_ap_rd32(tap, ARMV7M_MPU_RBAR, &rbar) != 
			JTAG_ADI_ACK_OK_FAULT)
			return;

		if (jtag_mem_ap_rd32(tap, ARMV7M_MMPU_RASR, &rasr) != 
			JTAG_ADI_ACK_OK_FAULT)
			return;

		/* TODO: decode and memory regions */
		fprintf(f, " *  %3d | ", i);
		fprintf(f, "0x%08x | ", rbar);
		fprintf(f, "0x%08x |\n", rasr);
	}

}

#define NVIC_INT_MAX 496

void nvic_info(FILE * f, jtag_tap_t * tap)
{
	uint16_t lst[512];
	unsigned int cnt;
	uint32_t ictr;
	uint32_t votr;
	uint32_t n;
	int dump = 0;
	int i;
	int j;

	fprintf(f, "- NVIC (Nested Vectored Interrupt Controller):\n");

	/* Interrupt Controller Type Register */
	if (jtag_mem_ap_rd32(tap, ARMV7M_ICTR, &ictr) != JTAG_ADI_ACK_OK_FAULT)
		return;

	/* Vector Table Offset Register */
	if (jtag_mem_ap_rd32(tap, ARMV7M_VTOR, &votr) != JTAG_ADI_ACK_OK_FAULT)
		return;

	n = 32 * ((ictr & 0xf) + 1);
	if (n > NVIC_INT_MAX)
		n = NVIC_INT_MAX;
	fprintf(f, " * ICTR = 0x%08x (%d IRQ lines)\n", ictr, n);
	fprintf(f, " * VOTR = 0x%08x\n", votr);

	cnt = 0;
	for (j = 0; j < n/32; ++j) {
		uint32_t iser;
		if (jtag_mem_ap_rd32(tap, ARMV7M_NVIC_ISER0 + j * 4, &iser) != 
			JTAG_ADI_ACK_OK_FAULT) return;
		for (i = 0; i < 32; ++i) {
			if (iser & (1 << i))
				lst[cnt++] = 32 * j + i;
		}
	}
	if (cnt == 0) {
		fprintf(f, " * No enabled IRQs.\n");
	} else {
		dump++;
		fprintf(f, " * Enabled IRQs:");
		for (i = 0; i < cnt; ++i) {
			fprintf(f, " %d", lst[i]);
		}
		fprintf(f, "\n");
	}

	cnt = 0;
	for (j = 0; j < n/32; ++j) {
		uint32_t ispr;
		if (jtag_mem_ap_rd32(tap, ARMV7M_NVIC_ISPR0 + j * 4, &ispr) != 
			JTAG_ADI_ACK_OK_FAULT) return;
		for (i = 0; i < 32; ++i) {
			if (ispr & (1 << i))
				lst[cnt++] = 32 * j + i;
		}
	}
	if (cnt == 0) {
		fprintf(f, " * No pending IRQs.\n");
	} else {
		dump++;
		fprintf(f, " * Pending IRQs:");
		for (i = 0; i < cnt; ++i) {
			fprintf(f, " %d", lst[i]);
		}
		fprintf(f, "\n");
	}

	cnt = 0;
	for (j = 0; j < n/32; ++j) {
		uint32_t iabr;
		if (jtag_mem_ap_rd32(tap, ARMV7M_NVIC_IABR0 + j * 4, &iabr) != 
			JTAG_ADI_ACK_OK_FAULT) return;
		for (i = 0; i < 32; ++i) {
			if (iabr & (1 << i))
				lst[cnt++] =  32 * j + i;
		}
	}
	if (cnt == 0) {
		fprintf(f, " * No active IRQs.\n");
	} else {
		dump++;
		fprintf(f, " * Active IRQs:");
		for (i = 0; i < cnt; ++i) {
			fprintf(f, " %d", lst[i]);
		}
		fprintf(f, "\n");
	}

	if (dump)
		fprintf(f, "    IRQ |       Addr | Pri |  En | Act | Pend |\n");

	/* Skip exception handlers */
	votr += 4 * 16;

	for (j = 0; j < n; ++j) {
		uint32_t ivec;
		uint32_t iesr;
		uint32_t ipr;
		uint32_t ispr;
		uint32_t iabr;
		int pri;
		int en;
		int pend;
		int act;

		if (jtag_mem_ap_rd32(tap, votr + j * 4, &ivec) != 
			JTAG_ADI_ACK_OK_FAULT) return;

		if (jtag_mem_ap_rd32(tap, (ARMV7M_NVIC_IPR0 + j) & ~3, &ipr) != 
			JTAG_ADI_ACK_OK_FAULT) return;

		pri = (ipr >> ((j & 3) * 4)) & 0xff;

		if (jtag_mem_ap_rd32(tap, ARMV7M_NVIC_ISER0 + (j >> 3), &iesr) != 
			JTAG_ADI_ACK_OK_FAULT) return;

		en = (iesr >> (j & 0x1f)) & 1;

		if (jtag_mem_ap_rd32(tap, ARMV7M_NVIC_ISPR0 + (j >> 3), &ispr) != 
			JTAG_ADI_ACK_OK_FAULT) return;

		pend = (ispr >> (j & 0x1f)) & 1;

		if (jtag_mem_ap_rd32(tap, ARMV7M_NVIC_IABR0 + (j >> 3), &iabr) != 
			JTAG_ADI_ACK_OK_FAULT) return;

		act = (iabr >> (j & 0x1f)) & 1;

		if (en || pend || act) {
			fprintf(f, "    %3d | 0x%08x | %3d | %3s | %3s | %4s |\n", 
					j, ivec, pri, en ? "yes" : "no", 
					pend ? "yes" : "no", act ? "yes" : "no");
		}
	}

	fprintf(f, "\n");
}

#define DEV_SYNC 0x43
#define DEV_CONNECTED 0x65

#define DBG_CONNECTED 1
#define DBG_SYNC 2

void comm_info(FILE * f, cm3ice_ctrl_t * ctrl)
{
	fprintf(f, "- CM3ICE COMM --------\n");
	/* FIXME: poll_enabled seems to be displayed wrong, removing 
	   to aoid confusion  
	fprintf(f, " ICE polling %s.\n", 
			ctrl->poll_enabled ? "enabled" : "disabled"); */
	fprintf(f, " ADDR=0x%08x\n", ctrl->comm_addr);
	fprintf(f, " dev(0x%02x)=%s\n", ctrl->cc.ro.dev,
			(ctrl->cc.ro.dev == DEV_CONNECTED) ? "CONNECTED" :
			((ctrl->cc.ro.dev == DEV_SYNC) ? "SYNC" : "Undef"));
	fprintf(f, " dbg(0x%02x)=%s\n", ctrl->cc.rw.dbg,
			(ctrl->cc.rw.dbg == DBG_CONNECTED) ? "CONNECTED" :
			((ctrl->cc.rw.dbg == DBG_SYNC) ? "SYNC" : "Undef"));
	fprintf(f, " rw.tx_tail=%d ro.tx_head=%d\n", 
			 ctrl->cc.rw.tx_tail, ctrl->cc.ro.tx_head);
	fprintf(f, " ro.rx_tail=%d rw.rx_head=%d\n", 
			 ctrl->cc.ro.rx_tail, ctrl->cc.rw.rx_head);
	fprintf(f, "\n");
}

int cm3ice_info(cm3ice_ctrl_t * ctrl, FILE * f, unsigned int which)
{
	jtag_tap_t * tap = ctrl->tap;
	DCC_LOG1(LOG_TRACE, "which=%d", which);

	switch(which) {
	case 1:
		dp_stickyerr_check(f, tap);
		csb_misc_info(f, tap);
		debug_info(f, tap);
		fault_info(f, tap);
		break;
	case 2:
		mem_ap_info(f, tap);
		dap_info(f, tap);
		break;
	case 3:
		cpu_info(f, tap);
		break;
	case 4:
		debug_info(f, tap);
		break;
	case 5:
		fault_info(f, tap);
		break;
	case 6:
		fpb_info(f, tap);
		break;
	case 7:
		dwt_info(f, tap);
		break;
	case 8:
		mpu_info(f, tap);
		break;
	case 9:
		nvic_info(f, tap);
		break;
	case 10:
		comm_info(f, ctrl);
		break;
	case 11:
		cm3_vc_disable(f, tap);
		break;
	case 12:
		cm3_vc_enable(f, tap);
		break;
	default:
		fprintf(f, "== Cortex M3 ICE ==\n");
		fprintf(f, "1 - CSB\n");
		fprintf(f, "2 - DBG/AP MEM/AP\n");
		fprintf(f, "3 - CPUID\n");
		fprintf(f, "4 - Debug status\n");
		fprintf(f, "5 - Fault status\n");
		fprintf(f, "6 - FPB\n");
		fprintf(f, "7 - DWT\n");
		fprintf(f, "8 - MPU\n");
		fprintf(f, "9 - NIVC\n");
		fprintf(f, "10 - COMM status\n");
		fprintf(f, "11 - VC disable\n");
		fprintf(f, "12 - VC enable\n");
		break;
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

