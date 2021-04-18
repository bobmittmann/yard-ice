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

int cm3ice_test_memrd(FILE * f, jtag_tap_t * tap, 
						   uint32_t addr, uint32_t len)
{
	uint32_t buf[64];

	if (len > (64 * 4))
		len = 64 * 4;

	DCC_LOG2(LOG_TRACE, "addr=0x%08x len=%d", addr, len);
	jtag_mem_ap_read(tap, addr, buf, len);

	show_hex32(f, addr, buf, len);

	return 0;
}

int cm3ice_test_ap_rd32(FILE * f, jtag_tap_t * tap, 
						   uint32_t addr, uint32_t len)
{
	uint32_t buf[64];
	uint32_t base;
	int n;
	int i;

	if (len > (64 * 4))
		len = 64 * 4;

	n = len / 4;
	base = addr & ~0x00000003;

	for (i = 0; i < n; ++i) {
		if (jtag_mem_ap_rd32(tap, base + i * 4, &buf[i]) 
			!= JTAG_ADI_ACK_OK_FAULT) {
			DCC_LOG(LOG_WARNING, "jtag_mem_ap_rd32() failed!"); 
			return ICE_ERR_JTAG;
		}
	}

	show_hex32(f, base, buf, n * 4);

	return 0;
}


int cortex_system_reset(FILE * f, jtag_tap_t * tap)
{
	uint32_t dhcsr;
	bool halt = false;

	DCC_LOG(LOG_TRACE, ".");

	if (jtag_mem_ap_rd32(tap, ARMV7M_DHCSR, &dhcsr) != JTAG_ADI_ACK_OK_FAULT) {
		fprintf(f, "%s: jtag_mem_ap_rd32() failed!\n", __func__); 
		return ICE_ERR_JTAG;
	}

	if (dhcsr & DHCSR_S_HALT) { 
		fprintf(f, "%s: core halted\n", __func__);
		halt = true;
	}

	if (jtag_mem_ap_wr32(tap, ARMV7M_AIRCR, AIRCR_VECTKEY | 
						 AIRCR_SYSRESETREQ ) != JTAG_ADI_ACK_OK_FAULT) {
		fprintf(f, "%s: jtag_mem_ap_wr32() failed!\n", __func__); 
		return ICE_ERR_JTAG;
	}

	if (!halt)
		return ICE_OK;

	do {
		fprintf(f, "halt 2. ...");

		/* halt the core */
		if (jtag_mem_ap_wr32(tap, ARMV7M_DHCSR, DHCSR_DBGKEY | DHCSR_C_HALT | 
							 DHCSR_C_DEBUGEN) != JTAG_ADI_ACK_OK_FAULT) {
			fprintf(f, "%s: jtag_mem_ap_wr32() failed!\n", __func__); 
			return ICE_ERR_JTAG;
		}


		if (jtag_mem_ap_rd32(tap, ARMV7M_DHCSR, 
							 &dhcsr) != JTAG_ADI_ACK_OK_FAULT) {
			fprintf(f, "%s: jtag_mem_ap_rd32() failed!\n", __func__); 
			return ICE_ERR_JTAG;
		}
		fprintf(f, "S_RESET_ST=%d S_RETIRE_ST=%d S_LOCKUP=%d "\
				 "S_SLEEP=%d S_HALT=%d", (dhcsr & DHCSR_S_RESET_ST) ? 1 : 0,
				 (dhcsr & DHCSR_S_RETIRE_ST) ? 1 : 0,
				 (dhcsr & DHCSR_S_LOCKUP) ? 1 : 0,
				 (dhcsr & DHCSR_S_SLEEP) ? 1 : 0,
				 (dhcsr & DHCSR_S_HALT) ? 1 : 0);
	} while ((dhcsr & DHCSR_S_HALT) == 0);

	return ICE_OK;
}

int cortex_core_reset(FILE * f, jtag_tap_t * tap)
{
	uint32_t dhcsr;
	uint32_t demcr;
	bool halt = false;

	if (jtag_mem_ap_rd32(tap, ARMV7M_DHCSR, &dhcsr) != JTAG_ADI_ACK_OK_FAULT) {
		fprintf(f, "jtag_mem_ap_rd32() failed!"); 
		return ICE_ERR_JTAG;
	}

	if (jtag_mem_ap_rd32(tap, ARMV7M_DEMCR, &demcr) != JTAG_ADI_ACK_OK_FAULT) {
		fprintf(f, "jtag_mem_ap_rd32() failed!"); 
		return ICE_ERR_JTAG;
	}

	if (dhcsr & DHCSR_S_HALT) { 
		fprintf(f, "core halted");
		halt = true;

		if ((demcr & DEMCR_VC_CORERESET) == 0) { 
			if (jtag_mem_ap_wr32(tap, ARMV7M_DEMCR, demcr |
								 DEMCR_VC_CORERESET) != JTAG_ADI_ACK_OK_FAULT) {
				fprintf(f, "jtag_mem_ap_wr32() failed!"); 
				return ICE_ERR_JTAG;
			}
		}

		if ((dhcsr & DHCSR_C_DEBUGEN) == 0) { 
			if (jtag_mem_ap_wr32(tap, ARMV7M_DHCSR, DHCSR_DBGKEY | dhcsr | 
								 DHCSR_C_DEBUGEN) != JTAG_ADI_ACK_OK_FAULT) {
				fprintf(f, "jtag_mem_ap_wr32() failed!"); 
				return ICE_ERR_JTAG;
			}
		}

	} else {
		fprintf(f, "halting...");
		/* halt the core but leave the C_DEBUGEN set
		   To force the processor to enter Debug state as soon as it comes 
		   out of reset, a debugger set DHCSR.C_DEBUGEN to 1, to enable 
		   halting debut, and sets DEMCR.VC_CORERESET to 1 to enable vector 
		   catch on the Reset exception. When the processor comes out 
		   of reset it sets DHCSR.C_HALT to 1,
		   and enters Debug state.
		 */
		if (jtag_mem_ap_wr32(tap, ARMV7M_DHCSR, DHCSR_DBGKEY | DHCSR_C_HALT | 
							 DHCSR_C_DEBUGEN) != JTAG_ADI_ACK_OK_FAULT) {
			fprintf(f, "jtag_mem_ap_wr32() failed!"); 
			return ICE_ERR_JTAG;
		}

		if (demcr & DEMCR_VC_CORERESET) { 
			if (jtag_mem_ap_wr32(tap, ARMV7M_DEMCR, 
					 demcr & ~DEMCR_VC_CORERESET) != JTAG_ADI_ACK_OK_FAULT) {
				fprintf(f, "jtag_mem_ap_wr32() failed!"); 
				return ICE_ERR_JTAG;
			}
		}

		halt = false;
	}

	/* Local reset */
	if (jtag_mem_ap_wr32(tap, ARMV7M_AIRCR, AIRCR_VECTKEY | AIRCR_VECTRESET |
						 AIRCR_VECTCLRACTIVE) != JTAG_ADI_ACK_OK_FAULT) {
		fprintf(f, "jtag_mem_ap_wr32() failed!"); 
		return ICE_ERR_JTAG;
	}

	return ICE_OK;

	do {
		if (jtag_mem_ap_rd32(tap, ARMV7M_DHCSR, 
							 &dhcsr) != JTAG_ADI_ACK_OK_FAULT) {
			fprintf(f, "jtag_mem_ap_rd32() failed!"); 
			return ICE_ERR_JTAG;
		}
		fprintf(f, "S_RESET_ST=%d S_RETIRE_ST=%d S_LOCKUP=%d "\
				 "S_SLEEP=%d S_HALT=%d", (dhcsr & DHCSR_S_RESET_ST) ? 1 : 0,
				 (dhcsr & DHCSR_S_RETIRE_ST) ? 1 : 0,
				 (dhcsr & DHCSR_S_LOCKUP) ? 1 : 0,
				 (dhcsr & DHCSR_S_SLEEP) ? 1 : 0,
				 (dhcsr & DHCSR_S_HALT) ? 1 : 0);
	} while ((dhcsr & DHCSR_S_RESET_ST) == 0);

	if (!halt)
		return ICE_OK;

	
	do {
		fprintf(f, "halt 2. ...");

		/* halt the core */
		if (jtag_mem_ap_wr32(tap, ARMV7M_DHCSR, DHCSR_DBGKEY | DHCSR_C_HALT | 
							 DHCSR_C_DEBUGEN) != JTAG_ADI_ACK_OK_FAULT) {
			fprintf(f, "jtag_mem_ap_wr32() failed!"); 
			return ICE_ERR_JTAG;
		}


		if (jtag_mem_ap_rd32(tap, ARMV7M_DHCSR, 
							 &dhcsr) != JTAG_ADI_ACK_OK_FAULT) {
			fprintf(f, "jtag_mem_ap_rd32() failed!"); 
			return ICE_ERR_JTAG;
		}
		fprintf(f, "S_RESET_ST=%d S_RETIRE_ST=%d S_LOCKUP=%d "\
				 "S_SLEEP=%d S_HALT=%d", (dhcsr & DHCSR_S_RESET_ST) ? 1 : 0,
				 (dhcsr & DHCSR_S_RETIRE_ST) ? 1 : 0,
				 (dhcsr & DHCSR_S_LOCKUP) ? 1 : 0,
				 (dhcsr & DHCSR_S_SLEEP) ? 1 : 0,
				 (dhcsr & DHCSR_S_HALT) ? 1 : 0);
	} while ((dhcsr & DHCSR_S_HALT) == 0);

	return ICE_OK;

}


int cm3ice_test_3(FILE * f, jtag_tap_t * tap)
{

	cortex_system_reset(f, tap);
		
	return 0;
}


int cm3ice_test(cm3ice_ctrl_t * ctrl, FILE * f, uint32_t req, 
				uint32_t argc, uint32_t argv[])
{
	jtag_tap_t * tap = ctrl->tap;
	DCC_LOG1(LOG_TRACE, "req=%d", req);

	switch(req) {
	case 1:
		cm3ice_test_memrd(f, tap, argv[0], argv[1]);
		break;
	case 2:
		cm3ice_test_ap_rd32(f, tap, argv[0], argv[1]);
		break;
	case 3:
		cm3ice_test_3(f, tap);
		break;
	default:
		fprintf(f, " +----- Cortex M3 ICE Tests ----------------+\n");
		fprintf(f, " | Test # | Parameters | Description        |\n");
		fprintf(f, " +--------+------------+--------------------+\n");
		fprintf(f, " |      1 | ADDR LEN   | Memory read        |\n");
		fprintf(f, " |      2 | ADDR LEN   | 32 bits mem access |\n");
		fprintf(f, " |      3 | none       | ...                |\n");
		fprintf(f, " +--------+------------+--------------------+\n");
	}

	return 0;
}

