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
	default:
		fprintf(f, " +----- Cortex M3 ICE Tests ----------------+\n");
		fprintf(f, " | Test # | Parameters | Description        |\n");
		fprintf(f, " +--------+------------+--------------------+\n");
		fprintf(f, " |      1 | ADDR LEN   | Memory read        |\n");
		fprintf(f, " |      2 | ADDR LEN   | 32 bits mem access |\n");
		fprintf(f, " +--------+------------+--------------------+\n");
	}

	return 0;
}

