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
 * @file cmd_ymodem_rcv.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/clock.h>
#include <sys/tty.h>
#include <xmodem.h>

#include "config.h"
#include "target.h"
#include "debugger.h"
#include "eval.h"

#include <trace.h>
#include <thinkos.h>

#include <sys/dcclog.h>

#if ENABLE_YMODEM

#define __THINKOS_DBGMON__
#include <thinkos/dbgmon.h>

#define YMODEM_RCV_BUF_LEN 1024

int cmd_ymodem_recv(FILE * f, int argc, char ** argv)
{
	uint8_t buf[YMODEM_RCV_BUF_LEN];
	struct comm_dev comm;
	struct debugger * dbg = &debugger;
	struct xmodem_rcv rx;
	struct file * raw;
	value_t val;
	uint32_t addr;
	uint32_t size;
	uint32_t start;
	unsigned int cnt;
	unsigned int rem;
	unsigned int offs;
	int ret;

	int n;

	argc--;
	argv++;

	if (argc) {
		if ((n = eval_uint32(&val, argc, argv)) < 0) {
			return n;
		}
		addr = val.uint32;
		argc -= n;
		argv += n;
	} else {
		addr = (uint32_t)dbg->transf.base;
	}

	if (argc) {
		if ((n = eval_uint32(&val, argc, argv)) < 0) {
			return n;
		}
		size = val.uint32;
		argc -= n;
		argv += n;
	} else {
		size = dbg->transf.size;
	}

	if (argc) {
		fprintf(f, "Too many arguments...\n");
		return -1;
	}

	fprintf(f, " - Write address: 0x%08x\n", addr);
	fprintf(f, " - Maxmum size: %d\n", size);

	raw = ftty_lowlevel(f);

	comm.arg = raw->data;
	comm.op.send = (int (*)(void *, const void *, unsigned int))raw->op->write;
	comm.op.recv = (int (*)(void *, void *, 
						  unsigned int, unsigned int))raw->op->read;

	DBGS(".................................");

	xmodem_rcv_init(&rx, &comm, XMODEM_RCV_CRC);

	cnt = 0;
	offs = 0;
	rem = size;
	start = thinkos_clock();
	do {
		if ((ret = xmodem_rcv_loop(&rx, buf, YMODEM_RCV_BUF_LEN)) < 0) {
			ERR("ret=%d", ret);
			break;
		}

		n = ret;
		if ((n = target_mem_write(addr, buf, n)) <= 0) {
			fprintf(f, "!");
		} else {
			fprintf(f, ".");
			size += n;
			addr += n;
		}
		addr += n;
		cnt += n;
		offs += n;
		rem -= n;
	} while (rem > 0);

	if (size) {
		uint32_t ms = (int32_t)(thinkos_clock() - start);
		fprintf(f, "\n - EOT: size=%d tm=%d[ms] speed=%d[bytes/sec]\n", 
				size, ms, (size * 1000) / ms);
	}

	dbg->transf.size = size;

	return 0;
}

#endif /* ENABLE_TCP_RCV */

