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
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "config.h"
#include "jtag.h"
#include "eval.h"
#include "dbglog.h"
#include "debugger.h"
#include "jtag.h"

int cmd_detect(FILE * f, int argc, char ** argv)
{
	uint8_t irlen[32];
	jtag_tap_t * tap;
	unsigned int cnt;
	int i;
	
	if (argc > 1) {
		fprintf(f, "usage: detect\n");
		return -1;
	} 

	fprintf(f, " - TAP reset...\n");
	target_tap_reset();

	fprintf(f, " - JTAG TRST assert ...\n");
	target_tap_trst(TARGET_IO_PULSE);

	fprintf(f, " - Probing the JTAG scan chain...\n");


	if (jtag_chain_probe(irlen, 32, &cnt) != JTAG_OK) {
		fprintf(f, "jtag_chain_probe() fail!\n");
		DCC_LOG(LOG_ERROR, "jtag_chain_probe() fail!");
		return -1;
	}

	if (cnt == 0) {
		fprintf(f, " - No devices detected.\n");
		return 0;
	}

	if (cnt == 1)
		fprintf(f, " - 1 device in the scan path.\n");
	else
		fprintf(f, " - %d devices in the scan path.\n", cnt);

	fprintf(f, " - Initializing the JTAG chain...\n");

	/* initializing the jtag chain */
	if (jtag_chain_init(irlen, cnt)  != JTAG_OK) {
		fprintf(f, "jtag_chain_init() fail!\n");
		DCC_LOG(LOG_ERROR, "jtag_chain_init() fail!");
		return -1;
	}

	fprintf(f, "\n");
	fprintf(f, "  | SEQ | IR LEN | ID CODE    | \n");

	for (i = 0; i < cnt; i++) {
		if (jtag_tap_get(&tap, i) != JTAG_OK) {
			fprintf(f, "jtag_tap_get() fail!\n");
			DCC_LOG(LOG_ERROR, "jtag_tap_get() fail!");
			return -1;
		}
		fprintf(f, "  | %3d | %6d | 0x%08x |\n", i, tap->irlen, tap->idcode);
	}

	fprintf(f, "\n");

	return 0;
}

