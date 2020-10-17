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
#include <sys/dcclog.h>
#include <hexdump.h>

#include "target.h"
#include "debugger.h"
#include "sys/file.h"
#include "sys/tty.h"
#include "xmodem.h"
#include "board.h"

int flash_ymodem_recv(FILE * f, uint32_t offs)
{
	struct ymodem_rcv ry;
	struct comm_dev comm;
	struct file * raw;
	uint8_t buf[128];
	int ret;
	int cnt;

	fprintf(f, "Ymodem (^X to cancel)... ");
	fflush(f);
	raw = ftty_lowlevel(f);

	comm.arg = raw->data;
	comm.op.send = (int (*)(void *, const void *, unsigned int))raw->op->write;
	comm.op.recv = (int (*)(void *, void *, 
						  unsigned int, unsigned int))raw->op->read;

	DCC_LOG(LOG_TRACE, ".................................");

	ymodem_rcv_init(&ry, &comm, XMODEM_RCV_CRC);

	cnt = 0;
	do {
		if ((ret = ymodem_rcv_loop(&ry, buf, 128)) < 0) {
			DCC_LOG1(LOG_ERROR, "ret=%d", ret);
			return ret;
		}
		stm32_flash_write(offs, buf, ret);
		cnt += ret;
		offs += ret;
	} while (ret > 0);

	fprintf(f, "\n");

	return cnt;
}

int jtag3ctrl_init(const void * rbf, int size);

int cmd_fpga(FILE * f, int argc, char ** argv)
{
	uint32_t flash_offs = FLASH_BLK_RBF_OFFS;
	uint8_t * rbf = (uint8_t *)STM32_FLASH_MEM + FLASH_BLK_RBF_OFFS;
	bool erase = false;
	bool load = false;
	bool cfg = false;

	if (argc > 1) {
		int i = 1;
		do {
			if ((strcmp(argv[i], "erase") == 0) || 
				(strcmp(argv[i], "e") == 0)) {
				erase = true;
			} else if ((strcmp(argv[i], "load") == 0) || 
					   (strcmp(argv[i], "l") == 0)) {
				load = true;
			} else if ((strcmp(argv[i], "config") == 0) || 
					   (strcmp(argv[i], "cfg") == 0) || 
					   (strcmp(argv[i], "c") == 0)) {
				cfg = true;
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
		if (stm32_flash_erase(flash_offs, FLASH_BLK_RBF_SIZE) < 0) {
			fprintf(f, "stm32f_flash_erase() failed!\n");
			return -1;
		}
	};

	if (load) {
		fprintf(f, "Loading FPGA .rbf file at 0x%08x...\n", (uint32_t)rbf);
		if (flash_ymodem_recv(f, flash_offs) < 0) {
			fprintf(f, "fpga_xmodem_recv() failed!\n");
			return -1;
		}
	};

	if (cfg) {
		fprintf(f, "Configuring FPGA...\n");
		if (jtag3ctrl_init(rbf, 38177) < 0) {
			fprintf(f, "jtag3ctrl_init() failed!\n");
			return -1;
		}
	};

	return 0;
}

