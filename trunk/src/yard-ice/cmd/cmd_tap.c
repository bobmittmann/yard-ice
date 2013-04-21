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

#include "config.h"
#include "target.h"
#include "debugger.h"
#include "dbglog.h"
#include "jtag.h"
#include "eval.h"

static int tap_reset(FILE * f)
{
	int err;

	if ((err = target_tap_reset()) < 0)
		fprintf(f, "ERROR: reset: %s.\n", target_strerror(err));

	return err;
}

static int tap_purge(FILE * f)
{
	unsigned int cnt;
	
	cnt = jtag_tap_tell();

	if (cnt == 0) {
		fprintf(f, " - No devices in the scan path.\n");
		return 0;
	}

	/* remove all taps from the chain */
	jtag_tap_purge();

	fprintf(f, " - %d devices removed from the scan path.\n", cnt);

	return 0;
}

static int tap_info(FILE * f)
{
	jtag_tap_t * tap;
	unsigned int cnt;
	int i;
	
	cnt = jtag_tap_tell();

	if (cnt == 0) {
		fprintf(f, " - No devices in the scan path.\n");
		return 0;
	}

	if (cnt == 1)
		fprintf(f, " - 1 device in the scan path.\n");
	else
		fprintf(f, " - %d devices in the scan path.\n", cnt);

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

static int tap_probe(FILE * f)
{
	uint8_t irlen[32];
	unsigned int cnt;

	fprintf(f, " - JTAG TRST assert ...\n");
	target_tap_trst(TARGET_IO_PULSE);

	fprintf(f, " - TAP reset...\n");
	target_tap_reset();

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

	fprintf(f, " - TAP reset...\n");
	/* reset the TAPs to put the IDCODE in the DR scan */
	target_tap_reset();

	/* initializing the jtag chain */
	if (jtag_chain_init(irlen, cnt)  != JTAG_OK) {
		fprintf(f, "jtag_chain_init() fail!\n");
		DCC_LOG(LOG_ERROR, "jtag_chain_init() fail!");
		return -1;
	}

	return tap_info(f);
}

static int tap_select(FILE * f, int argc, char ** argv)
{
	jtag_tap_t * tap;
	value_t val;
	int n;
	
	if (argc) {
		if ((n = eval_uint32(&val, argc, argv)) < 0) {
			DCC_LOG(LOG_WARNING, "arg_eval(), addr");
			return n;
		}
		if (n != argc) {
			fprintf(f, "usage: tap select [POSITION]\n");
			return -3;
		}

		n = val.int32;
	}  else {
		n = 0;
	}

	if (jtag_tap_get(&tap, n) != JTAG_OK) {
		return -4;
	}

	if (jtag_tap_select(tap) != JTAG_OK) {
		return n;
	}

	return 0;
}

int cmd_tap(FILE * f, int argc, char ** argv)
{

	if (argc == 1)
		return tap_info(f);

	argv++;
	if (argc == 2) {
		if ((strcmp(*argv, "reset") == 0) || (strcmp(*argv, "rst") == 0))
			return tap_reset(f);

		if ((strcmp(*argv, "probe") == 0) || (strcmp(*argv, "det") == 0))
			return tap_probe(f);

		if (strcmp(*argv, "purge") == 0)
			return tap_purge(f);

		if (strcmp(*argv, "info") == 0)
			return tap_info(f);
	}

	if (argc > 2) {
		if ((strcmp(*argv, "select") == 0) || (strcmp(*argv, "sel") == 0))
			return tap_select(f, argc, argv);
	}

	if (argc > 2) {
//		printf(msg_reset_usage);
		return -1;
	}


	return -1;
}

