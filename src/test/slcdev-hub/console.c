/* 
 * File:	 usb-test.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2011 Bob Mittmann. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <sys/stm32f.h>

#include <sys/shell.h>
#include <sys/tty.h>
#include <sys/usb-cdc.h>
#include <sys/serial.h>
#include <xmodem.h>
#include <hexdump.h>

#include <thinkos.h>

#include <sys/dcclog.h>

#include "lattice.h"
#include "net.h"

extern const uint8_t * ice40lp384_bin;
extern const struct shell_cmd cmd_tab[];

/*****************************************************************************
 * Help
 *****************************************************************************/

int cmd_help(FILE *f, int argc, char ** argv)
{
	struct shell_cmd * cmd;

	if (argc > 2)
		return -1;

	if (argc > 1) {
		if ((cmd = cmd_lookup(argv[1], cmd_tab)) == NULL) {
			fprintf(f, " Not found: '%s'\n", argv[1]);
			return -1;
		}

		fprintf(f, "  %s, %s - %s\n", cmd->name, cmd->alias, cmd->desc);
		fprintf(f, "  usage: %s %s\n\n", argv[1], cmd->usage);

		return 0;
	}

	fprintf(f, "\n COMMAND:   ALIAS:  DESCIPTION: \n");
	for (cmd = (struct shell_cmd *)cmd_tab; cmd->callback != NULL; cmd++) {
		fprintf(f, "  %-10s %-4s   %s\n", cmd->name, cmd->alias, cmd->desc);
	}

	return 0;
}

/*****************************************************************************
 * Environment 
 *****************************************************************************/

int cmd_set(FILE * f, int argc, char ** argv)
{
	char * val;
	char * env;
	int i;

	if (argc > 1) {
		if (argc < 3) {
			fprintf(f, "Argument invalid or missing!\n");
			fprintf(f, "usage: %s [VAR VALUE]\n", argv[0]);
			return -1;
		}

		val = argv[2];

		fprintf(f, "%s=%s\n", argv[1], val);
	
		if (setenv(argv[1], val, 1) < 0) {
			fprintf(f, "setenv(): fail!\n");
			return -1;
		};

		return 0;
	}

	for (i = 0; (env =  environ[i]) != NULL; i++) {
		fprintf(f, "%s\n", env);
	};

	return 0;
}

int cmd_get(FILE * f, int argc, char ** argv)
{
	char * env;

	if (argc != 2) {
		fprintf(f, "Argument invalid or missing!\n");
		fprintf(f, "usage: %s VAR\n", argv[0]);
		return -1;
	}

	if ((env = getenv(argv[1])) == NULL) {
		fprintf(f, "getenv() fail!\n");
		return -1;
	};

	fprintf(f, "%s\n", env);

	return 0;
}

/*****************************************************************************
 * RS485 Network
 *****************************************************************************/

const char net_msg[] = "The qick brown fox jumps over the lazy dog!";
const uint8_t net_pattern[] = { 0x54, 0x38, 0x54, 0x38 };

int cmd_net(FILE * f, int argc, char ** argv)
{
	char msg[256];
	bool test = false;
	bool flood = false;
	bool conf = false;
	bool probe = false;
	bool normal = false;
	bool init = false;
	bool pattern = false;
	unsigned int val;
	(void)conf;

	if (argc > 1) {
		int i = 1;
		do {
			if ((strcmp(argv[i], "test") == 0) || 
				(strcmp(argv[i], "t") == 0)) {
				test = true;
			} else if ((strcmp(argv[i], "flood") == 0) || 
					   (strcmp(argv[i], "f") == 0)) {
				flood = true;
			} else if ((strcmp(argv[i], "pat") == 0) || 
					   (strcmp(argv[i], "p") == 0)) {
				pattern = true;
			} else if ((strcmp(argv[i], "conf") == 0) || 
					   (strcmp(argv[i], "c") == 0)) {
				conf = true;
			} else if ((strcmp(argv[i], "sup") == 0) || 
					   (strcmp(argv[i], "s") == 0)) {
				probe = true;
			} else if ((strcmp(argv[i], "auto") == 0) || 
					   (strcmp(argv[i], "a") == 0)) {
				normal = true;
			} else if ((strcmp(argv[i], "init") == 0) || 
					   (strcmp(argv[i], "i") == 0)) {
				init = true;
			} else {
				if ((strcmp(argv[i], "flood") == 0) || 
					   (strcmp(argv[i], "f") == 0)) {
				flood = true;
				fprintf(f, "Invalid argument: %s\n", argv[i]);
				return -1;
			}
		} while (++i < argc);
	} else {
		fprintf(f, "\n=== RS485 network ===\n");
		return 0;
	}

	if (init) {
		fprintf(f, "RS845 network init.\n");
		net_init();
	};

	if (pattern) {
		fprintf(f, "RS845 network pattern test.\n");
		net_send(net_pattern, sizeof(net_pattern));
	};

	if (test) {
		fprintf(f, "RS845 network message test.\n");
		net_send(net_msg, sizeof(net_msg));
	};

	if (flood) {
		int i;
		int n;
		fprintf(f, "RS845 network flooding test.\n");
		for (i = 0; i < 150; ++i) {
			thinkos_sleep(100);
			n = sprintf(msg, "%3d - %s", i, net_msg);
			net_send(msg, n);
		}
	};

	if (probe) {
		fprintf(f, "RS845 supervisory mode. (A: RX, B: TX)\n");
		net_probe_enable();
	};

	if (normal) {
		fprintf(f, "RS845 automatic mode.\n");
		net_probe_disable();
	};

	return 0;
}

/*****************************************************************************
 * FPGA
 *****************************************************************************/

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
		stm32_flash_write(offs, buf, ret);
		cnt += ret;
		offs += ret;
	} while (ret > 0);

	return cnt;
}

int cmd_fpga(FILE * f, int argc, char ** argv)
{
	uint8_t * bin = (uint8_t *)ice40lp384_bin;
	uint32_t flash_offs = (uint8_t *)bin - STM32_FLASH_MEM;
	bool erase = false;
	bool load = false;
	bool conf = false;

	if (argc > 1) {
		int i = 1;
		do {
			if ((strcmp(argv[i], "erase") == 0) || 
				(strcmp(argv[i], "e") == 0)) {
				erase = true;
			} else if ((strcmp(argv[i], "load") == 0) || 
					   (strcmp(argv[i], "l") == 0)) {
				load = true;
			} else if ((strcmp(argv[i], "conf") == 0) || 
					   (strcmp(argv[i], "c") == 0)) {
				conf = true;
			} else {
				fprintf(f, "Invalid argument: %s\n", argv[i]);
				return -1;
			}
		} while (++i < argc);
	} else {
		fprintf(f, "FPGA sector: 0x%08x\n", (uint32_t)bin);
		show_hex8(f, (uint32_t)bin, bin, 1024);
		return 0;
	}

	if (erase) {
		fprintf(f, "Erasing sector: 0x%08x...\n", (uint32_t)bin);
		if (stm32_flash_erase(flash_offs, 0x20000) < 0) {
			fprintf(f, "stm32f_flash_erase() failed!\n");
			return -1;
		}
	};

	if (load) {
		fprintf(f, "Loading FPGA file at 0x%08x...\n", (uint32_t)bin);
		if (flash_xmodem_recv(f, flash_offs) < 0) {
			fprintf(f, "fpga_xmodem_recv() failed!\n");
			return -1;
		}
	};

	if (conf) {
		fprintf(f, "Configuring FPGA ...\n");
		if (lattice_ice40_configure(bin, 32768) < 0) {
			fprintf(f, "lattice_ice40_configure() failed!\n");
			return -1;
		}
	};

	return 0;
}


const struct shell_cmd cmd_tab[] = {

	{ cmd_get, "get", "", 
		"VAR", "get environement variable" },

	{ cmd_set, "set", "", 
		"VAR EXPR", "set environement variable" },

	{ cmd_help, "help", "?", 
		"[COMMAND]", "show command usage (help [CMD])" },

	{ cmd_fpga, "fpga", "f", 
		"[erase] [load] [conf]", "update FPGA program." },

	{ cmd_net, "net", "n", 
		"[test]", "RS485 network." },

	{ NULL, "", "", NULL, NULL }
};


#define VERSION_NUM "0.1"
#define VERSION_DATE "Jun, 2014"

const char shell_greeting[] = "\n"
	"SLCDEV-HUB" VERSION_NUM " - " VERSION_DATE "\n"
	"(c) Copyright 2014 - Bob Mittmann (bobmittmann@gmail.com)\n\n";

const char * get_prompt(void)
{
	return (char *)"[HUB]$ ";
}

int stdio_shell(void)
{
	DCC_LOG(LOG_TRACE, "...");

	return shell(stdout, get_prompt, shell_greeting, cmd_tab);
}

int usb_shell(usb_cdc_class_t * cdc)
{
	struct tty_dev * tty;
	FILE * f_tty;
	FILE * f_raw;

	f_raw = usb_cdc_fopen(cdc);
	tty = tty_attach(f_raw);
	f_tty = tty_fopen(tty);

	stdout = f_tty;
	stdin = f_tty;

	return shell(f_tty, get_prompt, shell_greeting, cmd_tab);
}

