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
 * @file cmd_fpga.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stm32f.h>
#include <hexdump.h>

#include "target.h"
#include "debugger.h"
#include "sys/file.h"
#include "sys/tty.h"
#include "xmodem.h"

union {
	struct xmodem_rcv rx;
	struct xmodem_snd sx;
} xmodem;

int flash_xmodem_recv(FILE * f, uint32_t offs)
{
	struct comm_dev comm;
	struct file * raw;
	uint8_t buf[128];
	int ret;
	int cnt;

	raw = ftty_lowlevel(f);

	comm.arg = raw->data;
	comm.op.send = (int (*)(void *, const void *, unsigned int))raw->op->write;
	comm.op.recv = (int (*)(void *, void *, 
						  unsigned int, unsigned int))raw->op->read;

	DCC_LOG(LOG_TRACE, ".................................");

	xmodem_rcv_init(&xmodem.rx, &comm, XMODEM_RCV_CRC);

	cnt = 0;
	do {
		if ((ret = xmodem_rcv_loop(&xmodem.rx, buf, 128)) < 0) {
			DCC_LOG1(LOG_ERROR, "ret=%d", ret);
			return ret;
		}
		stm32f_flash_write(offs, buf, ret);
		cnt += ret;
		offs += ret;
	} while (ret > 0);

	return cnt;
}


int cmd_fpga(FILE * f, int argc, char ** argv)
{
	uint8_t * rbf = (uint8_t *)0x08060000;
	bool erase = false;
	bool load = false;

	if (argc > 1) {
		int i = 1;
		do {
			if ((strcmp(argv[i], "erase") == 0) || 
				(strcmp(argv[i], "e") == 0)) {
				erase = true;
			} else if ((strcmp(argv[i], "load") == 0) || 
					   (strcmp(argv[i], "l") == 0)) {
				load = true;
			} else {
				fprintf(f, "Invalid argument: %s\n", argv[i]);
				return -1;
			}
		} while (++i < argc);
	} else {
		fprintf(f, "FPG sector: 0x%08x\n", (uint32_t)rbf);
		show_hex8(f, (uint32_t)rbf, rbf, 1024);
		return 0;
	}

	debugger_except("FPGA update, reboot required!");

	if (erase) {
		fprintf(f, "Erasing sector: 0x%08x...\n", (uint32_t)rbf);
		if (stm32f_flash_erase(0x60000, 0x20000) < 0) {
			fprintf(f, "stm32f_flash_erase() failed!\n");
			return -1;
		}
	};

	if (load) {
		fprintf(f, "Loading FPGA .rbf file at 0x%08x...\n", (uint32_t)rbf);
		if (flash_xmodem_recv(f, 0x60000) < 0) {
			fprintf(f, "fpga_xmodem_recv() failed!\n");
			return -1;
		}
	};

	return 0;
}

