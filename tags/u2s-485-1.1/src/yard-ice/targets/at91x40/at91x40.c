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
 * @file at91x40.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <arch/at91x40.h>

#include "target.h"
#include "armice.h"
#include "ice_drv.h"

#include <sys/dcclog.h>

const struct target_sym at91x40_sym[] = {
	{ 0xffe00000, "ebi_csr0", 4, _RW_},
	{ 0xffe00004, "ebi_csr1", 4, _RW_},
	{ 0xffe00008, "ebi_csr2", 4, _RW_},
	{ 0xffe0000c, "ebi_csr3", 4, _RW_},
	{ 0xffe00010, "ebi_csr4", 4, _RW_},
	{ 0xffe00014, "ebi_csr5", 4, _RW_},
	{ 0xffe00018, "ebi_csr6", 4, _RW_},
	{ 0xffe0001c, "ebi_rcr", 4, _WO_},
	{ 0xffff4000, "ps_cr", 4, _RW_},
	{ 0xffff4004, "ps_pcer", 4, _WO_},
	{ 0xffff4008, "ps_pcdr", 4, _WO_},
	{ 0xffff400c, "ps_pcsr", 4, _RO_},
	{ AT91_BASE_WD + WD_OMR, "wd_omr", 4, _RW_},
	{ 0xffff8000, "wd_cmr", 4, _RW_},
	{ 0xffff8000, "wd_cr", 4, _RW_},
	{ 0xffff8000, "wd_sr", 4, _RW_},
	{ AT91_BASE_SF + SF_CIDR, "sf_cidr", 4, _RO_},
	{ 0, "", 0, 0 } 
};

const struct target_cpu at91m40800_cpu = {
	.vendor = "ATMEL",
	.family = "AT91x40",
	.model = "AT91M40800",
	.idmask = 0xffffffff,
	.idcomp = 0x1f0f0f0f, 
	.sym = at91x40_sym
};

const struct target_cpu at91r40008_cpu = {
	.vendor = "ATMEL",
	.family = "AT91x40",
	.model = "AT91R40008",
	.idmask = 0xffffffff,
	.idcomp = 0x1f0f0f0f, 
	.sym = at91x40_sym
};

const struct armice_cfg at91x40_cfg = {
	.endianness = LITTLE_ENDIAN,
	.work_addr = 0x00000000,
	.work_size = MEM_KiB(8)
};

int at91x40_reset(FILE * f, const struct ice_drv * ice, ice_mem_entry_t * mem)
{
	DCC_LOG(LOG_TRACE, ".");

	fprintf(f, "- Disable the watchdog...\n");
	/* disable the watchdog */
	ice_wr32(ice, AT91_BASE_WD + WD_OMR, WD_OKEY | WD_RSTEN);
	fprintf(f, "- Configuring the watchdog to reset...\n");
	/* configure WD clock */
	ice_wr32(ice, AT91_BASE_WD + WD_CMR, WD_CKEY | WD_WDCLKS_MCK8); 
	/* restart the timer */
	ice_wr32(ice, AT91_BASE_WD + WD_CR, WD_RSTKEY); 

	fprintf(f, "- Clear the vector's table...\n");
	/* fill vectors table with nops */
	ice_wr32(ice, 0x00000000, 0xe1a00000); 
	ice_wr32(ice, 0x00000004, 0xe1a00000); 
	ice_wr32(ice, 0x00000008, 0xe1a00000); 
	ice_wr32(ice, 0x0000000c, 0xe1a00000); 
	ice_wr32(ice, 0x00000010, 0xe1a00000); 
	ice_wr32(ice, 0x00000014, 0xe1a00000); 
	ice_wr32(ice, 0x00000018, 0xe1a00000); 
	/* b ... */
	ice_wr32(ice, 0x0000001c, 0xeafffff7); 

	fprintf(f, "- Branch to address 0...\n");
	DCC_LOG(LOG_TRACE, "goto 0");

	if (ice_goto(ice, 0x00000000) < 0) {
		DCC_LOG(LOG_WARNING, "ice_goto() fail!");
	}

	DCC_LOG2(LOG_TRACE, "watchdog reset: %08x=%08x",
		AT91_BASE_WD + WD_OMR, WD_OKEY | WD_RSTEN | WD_WDEN); 

	fprintf(f, "- Enable watchdog...\n");
	/* enable the watchdog */
	ice_wr32(ice, AT91_BASE_WD + WD_OMR, WD_OKEY | WD_RSTEN | WD_WDEN); 

	return 0;
}

int at91x40_on_halt(FILE * f, const struct ice_drv * ice, 
					ice_mem_entry_t * mem)
{
	uint32_t wd_omr;

	ice_rd32(ice,  AT91_BASE_WD + WD_OMR, &wd_omr);

	if ((wd_omr & (WD_RSTEN | WD_WDEN)) == (WD_RSTEN | WD_WDEN)) {
		/* disable the watchdog */
		ice_wr32(ice, AT91_BASE_WD + WD_OMR, WD_OKEY | WD_RSTEN);
	}

	return 0;
}

int at91x40_on_run(FILE * f, const struct ice_drv * ice, 
				   ice_mem_entry_t * mem)
{
	/* reenable the watchdog */
//	ice_wr32(ice,  AT91_BASE_WD + WD_OMR, WD_OKEY | WD_RSTEN | WD_WDEN);

	return 0;
}

#define AT91M40800 0x14080044
#define AT91R40807 0x44080746
#define AT91M40807 0x14080745
#define AT91R40008 0x44000840

int at91m40800_probe(FILE * f, const struct ice_drv * ice, 
					 ice_mem_entry_t * mem)
{
	uint32_t idr;

	ice_rd32(ice,  AT91_BASE_SF + SF_CIDR, &idr);

	DCC_LOG1(LOG_TRACE, "SF_CIDR=0X%08X", idr);

	return (idr == AT91M40800) ? 1 : 0;
}

int at91r40008_probe(FILE * f, const struct ice_drv * ice, 
					 ice_mem_entry_t * mem)
{
	uint32_t idr;

	ice_rd32(ice,  AT91_BASE_SF + SF_CIDR, &idr);

	DCC_LOG1(LOG_TRACE, "SF_CIDR=0X%08X", idr);

	return (idr == AT91R40008) ? 1 : 0;
}

