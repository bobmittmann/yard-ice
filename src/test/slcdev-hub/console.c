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
#include <sys/null.h>
#include <xmodem.h>
#include <hexdump.h>

#include <thinkos.h>

#include <sys/dcclog.h>
#include <sys/delay.h>

#include "trace.h"

#include "lattice.h"
#include "net.h"


extern const uint8_t * ice40lp384_bin;
extern const struct shell_cmd shell_cmd_tab[];
extern const char * version_str;
extern const char * copyright_str;

/*****************************************************************************
 * Help
 *****************************************************************************/

int cmd_help(FILE *f, int argc, char ** argv)
{
	const struct shell_cmd * cmd;

	if (argc > 2)
		return -1;

	if (argc > 1) {
		if ((cmd = cmd_lookup(shell_cmd_tab, argv[1])) == NULL) {
			fprintf(f, " Not found: '%s'\n", argv[1]);
			return -1;
		}

		fprintf(f, "  %s, %s - %s\n", cmd->name, cmd->alias, cmd->desc);
		fprintf(f, "  usage: %s %s\n\n", argv[1], cmd->usage);

		return 0;
	}

	fprintf(f, "\n COMMAND:   ALIAS:  DESCIPTION: \n");
	for (cmd = shell_cmd_tab; cmd->callback != NULL; cmd++) {
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

int cmd_stat(FILE * f, int argc, char ** argv)
{
	struct netstats stat;
	bool clear = false;
	
	if (argc > 2)
		return SHELL_ERR_EXTRA_ARGS;

	if (argc > 1) {
		if ((strcmp(argv[1], "clear") == 0) || 
			(strcmp(argv[1], "clr") == 0) ||
			(strcmp(argv[1], "c") == 0)) {
			clear = true;
		} 
	}

	net_get_stats(&stat, clear);

	fprintf(f, "    |     octets | packets |  errors | seq err |\n");
	fprintf(f, " TX | %10u | %7u | %7u |         |\n", 
			stat.tx.octet_cnt, stat.tx.pkt_cnt, stat.tx.err_cnt);
	fprintf(f, " RX | %10u | %7u | %7u | %7u |\n", 
			stat.rx.octet_cnt, stat.rx.pkt_cnt, 
			stat.rx.err_cnt, stat.rx.seq_err_cnt);

	return 0;
}


const char net_msg[] = "The quick brown fox jumps over the lazy dog!";
const uint8_t net_pattern[] = { 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,

	0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};


/* ---------------------------------------------------------------------------
   -- Continuous packet transmission thread
   -------------------------------------------------------------------------- */

int pkt_xmt_flag;
volatile bool pkt_xmt_itv_rand = false;
volatile uint16_t pkt_xmt_itv_ms = 10;
volatile uint16_t pkt_xmt_len = 256;
volatile uint32_t pkt_xmt_cnt = 0;

void __attribute__((noreturn)) pkt_xmt_task(void)
{
	uint32_t data[128];
	uint32_t x = 0;
	uint32_t itv = 0x55555555;
	int i;
	
	DCC_LOG1(LOG_TRACE, "thread=%d", thinkos_thread_self());

	DBG("<%d> started...", thinkos_thread_self());

	for (;;) {
		int len;
		thinkos_flag_watch(pkt_xmt_flag);

		len = pkt_xmt_len;
		if (len > sizeof(data))
			len = sizeof(data);

		for (i = 0; i < (len + 3) / 4; ++i) {
			/* random packet data */
			x = (1103515245 * x + 12345) & 0x7fffffff;
			data[i] = x;
		}

		if (net_pkt_send(data, len) < 0) {
			DBG_S("net_pkt_send() failed!");
			thinkos_sleep(100);
			continue;
		}

		pkt_xmt_cnt++;

		if (pkt_xmt_itv_rand) {
			/* random packet interval */
			itv = (1103515245 * itv + 12345) & 0x7fffffff;
			thinkos_sleep(itv & 0xf);
			udelay((itv >> 16) & 0xf);
		} else {
			/* fixed packet interval */
			thinkos_sleep(pkt_xmt_itv_ms);
		}	
	}
}

uint32_t __attribute__((aligned(8))) pkt_xmt_stack[1024];

void pkt_xmt_init(void)
{
	static int8_t thread = -1;

	if (thread >= 0)
		return;

	pkt_xmt_flag = thinkos_flag_alloc();

	thread = thinkos_thread_create((void *)pkt_xmt_task, (void *)NULL,
								   pkt_xmt_stack, sizeof(pkt_xmt_stack) |
								   THINKOS_OPT_PRIORITY(8) | THINKOS_OPT_ID(8));
}

int cmd_pkt(FILE * f, int argc, char ** argv)
{
	unsigned int val;

	if (argc > 1) {
		if ((strcmp(argv[1], "rx") == 0) || 
			(strcmp(argv[1], "r") == 0)) {
			fprintf(f, "Packet reception thread enabled.\n");
			net_pkt_mode(true);
			return 0;
		}

		if ((strcmp(argv[1], "tx") == 0) || 
			(strcmp(argv[1], "t") == 0)) {
			fprintf(f, "Packet transmission started.\n");
			net_pkt_mode(true);
			pkt_xmt_cnt = 0;
			thinkos_flag_set(pkt_xmt_flag);
			return 0;
		}

		if ((strcmp(argv[1], "stop") == 0) || 
			(strcmp(argv[1], "s") == 0)) {
			fprintf(f, "Packet transmission stopped.\n");
			thinkos_flag_clr(pkt_xmt_flag);
			return 0;
		}

		if (argc > 2) {
			if ((strcmp(argv[1], "len") == 0) || 
				(strcmp(argv[1], "l") == 0)) {
				val = strtoul(argv[2], NULL, 0);
				fprintf(f, "Packet data length = %u.\n", val);
				pkt_xmt_len = val;
				return 0;
			}
			if ((strcmp(argv[1], "itv") == 0) || 
				(strcmp(argv[1], "i") == 0)) {

				if ((strcmp(argv[2], "rand") == 0) || 
					(strcmp(argv[2], "r") == 0)) {
					fprintf(f, "Random packet interval.\n");
					pkt_xmt_itv_rand = true;
				} else {
					val = strtoul(argv[2], NULL, 0);
					fprintf(f, "Packet interval = %u ms.\n", val);
					pkt_xmt_itv_ms = val;
					pkt_xmt_itv_rand = false;
				}
				return 0;
			}
		}

		return SHELL_ERR_ARG_INVALID;
	}

#if 0
	fprintf(f, "- Packet transmission %s.\n", 
			thinkos_flag_val(pkt_xmt_flag) ? "enabled" : "disabled");
#endif
	fprintf(f, "  - data length = %u.\n", pkt_xmt_len);
	if (pkt_xmt_itv_rand)
		fprintf(f, "  - interval = [random].\n");
	else
		fprintf(f, "  - interval = %u ms.\n", pkt_xmt_itv_ms);
	fprintf(f, "  - count = %u.\n", pkt_xmt_cnt);

	return 0;
}


int cmd_net(FILE * f, int argc, char ** argv)
{
	char msg[256];
	bool test = false;
	bool flood = false;
	bool conf = false;
	bool probe = false;
	bool supv = false;
	bool normal = false;
	bool init = false;
	bool pattern = false;
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
			} else if ((strcmp(argv[i], "msg") == 0) || 
					   (strcmp(argv[i], "m") == 0)) {
				pattern = true;
			} else if ((strcmp(argv[i], "conf") == 0) || 
					   (strcmp(argv[i], "c") == 0)) {
				conf = true;
			} else if ((strcmp(argv[i], "sup") == 0) || 
					   (strcmp(argv[i], "s") == 0)) {
				supv = true;
			} else if ((strcmp(argv[i], "probe") == 0) || 
					   (strcmp(argv[i], "p") == 0)) {
				probe = true;
			} else if ((strcmp(argv[i], "auto") == 0) || 
					   (strcmp(argv[i], "a") == 0)) {
				normal = true;
			} else if ((strcmp(argv[i], "init") == 0) || 
					   (strcmp(argv[i], "i") == 0)) {
				init = true;
			} else {
				fprintf(f, "Invalid argument: %s\n", argv[i]);
				return -1;
			}
		} while (++i < argc);
	} else {
		fprintf(f, "\n=== RS485 network ===\n");
		return 0;
	}

	if (init) {
		fprintf(f, "Initializing RS845 HUB... ");
		if (net_init() < 0) {
			fprintf(f, "FAILED!\n");
			return -1;
		}
		fprintf(f, "OK.\n");
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
		for (i = 0; i < 100; ++i) {
			thinkos_sleep(10);
			n = sprintf(msg, "%3d - %s", i, net_msg);
			net_send(msg, n);
		}
	};

	if (supv) {
		fprintf(f, "RS845 supervisory mode. (A: RX, B: TX)\n");
		net_probe_enable();
	};

	if (normal) {
		fprintf(f, "RS845 automatic mode.\n");
		net_probe_disable();
	};

	if (probe) {
		fprintf(f, "Probing RS845 HUB... ");
		if (net_probe() < 0) {
			fprintf(f, "FAILED!\n");
			return -1;
		}
		fprintf(f, "OK.\n");
	}

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
		fflush(f);
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

int usb_xflash(uint32_t offs, uint32_t len);

int usart_xflash(void * uart, uint32_t offs, uint32_t len);

int cmd_xflash(FILE * f, int argc, char ** argv)
{
	uint32_t offs = 0x00000;
	uint32_t size = 0x00000;
	uint32_t pri;
	FILE * raw;
	int ret;

	if (argc < 2)
		return SHELL_ERR_ARG_MISSING;

	if (argc > 2)
		return SHELL_ERR_EXTRA_ARGS;

	do {
		if ((strcmp(argv[1], "firmware") == 0) || 
			(strcmp(argv[1], "firm") == 0) ||
			(strcmp(argv[1], "f") == 0)) {
			offs = 0;
			size = 256 * 1024;
			fprintf(f, "Firmware update...\n");
			break;
		} 

		return SHELL_ERR_ARG_INVALID;
	} while (0);

	fflush(f);

	raw = isfatty(f) ? ftty_lowlevel(f) : f;

	if (usb_cdc_is_usb_file(raw)) {
		pri = cm3_primask_get();
		cm3_primask_set(1);
		ret = usb_xflash(offs, size);
		cm3_primask_set(pri);
		return ret;
	} 

	if (is_serial(raw)) {
		pri = cm3_primask_get();
		cm3_primask_set(1);
		ret = usart_xflash(STM32_UART5, offs, size);
		cm3_primask_set(pri);

		return ret;
	}
		  
	fprintf(f, "Operation not permited in this terminal.\n");
	return -1;
}

int cmd_reboot(FILE * f, int argc, char ** argv)
{
	if (argc > 1)
		return SHELL_ERR_EXTRA_ARGS;

	fprintf(f, "Restarting in 2 seconds...\n");

	thinkos_sleep(2000);

	cm3_sysrst();

	return 0;
}

extern FILE * monitor_stream;
extern bool monitor_auto_flush;

int cmd_trace(FILE * f, int argc, char ** argv)
{
	argc--;
	argv++;

	if (argc) {
		if ((strcmp(*argv, "flush") == 0) || (strcmp(*argv, "f") == 0)) {
			fprintf(f, "flush\n");
//			trace_flush();
			return 0;
		} 

		if ((strcmp(*argv, "monitor") == 0) || (strcmp(*argv, "m") == 0)) {
			fprintf(f, "monitor\n");
			monitor_stream = f;
			return 0;
		}

		if ((strcmp(*argv, "null") == 0) || (strcmp(*argv, "n") == 0)) {
			fprintf(f, "null\n");
			monitor_stream = null_fopen(NULL);
			return 0;
		}

		if ((strcmp(*argv, "auto") == 0) || (strcmp(*argv, "a") == 0)) {
			fprintf(f, "Auto flush\n");
			monitor_auto_flush = true;
			return 0;
		}

		if ((strcmp(*argv, "keep") == 0) || (strcmp(*argv, "k") == 0)) {
			fprintf(f, "Keep trace (don't flush)\n");
			monitor_auto_flush = false;
			return 0;
		}

		return SHELL_ERR_ARG_INVALID;
	}

	fprintf(f, "---------\n");
//	trace_fprint(f, TRACE_ALL);

	return 0;
}

int cmd_version(FILE *f, int argc, char ** argv)
{
	if (argc > 1)
		return SHELL_ERR_EXTRA_ARGS;

	fprintf(f, "%s\n", version_str);
	fprintf(f, "%s\n", copyright_str);

	return 0;
}

int cmd_echo(FILE *f, int argc, char ** argv)
{
	int i;

	for (i = 1; i < argc; ++i) {
		if (i != 1)
			fprintf(f, " ");
		fprintf(f, argv[i]);
	}

	fprintf(f, "\n");

	return 0;
}

const struct shell_cmd shell_cmd_tab[] = {

	{ cmd_echo, "echo", "", 
		"[STRING]...", "Echo the STRING(s) to terminal" },

	{ cmd_get, "get", "", 
		"VAR", "get environement variable" },

	{ cmd_set, "set", "", 
		"VAR EXPR", "set environement variable" },

	{ cmd_help, "help", "?", 
		"[COMMAND]", "show command usage (help [CMD])" },

	{ cmd_fpga, "fpga", "f", 
		"[erase] [load] [conf]", 
		"update FPGA program." },

	{ cmd_net, "net", "n", 
		"[test|flood|pat|conf|sup|auto|init]", 
		"RS485 network." },

	{ cmd_xflash, "xf", "", "", 
		"update firmware." },

	{ cmd_reboot, "reboot", "rst", "", 
		"reboot system" },

	{ cmd_trace, "trace", "t", 
		"[flush | auto | monitor | null | keep]", "handle the trace ring" },

	{ cmd_stat, "stat", "s", 
		"[clear]", "show network statistics info" },

	{ cmd_pkt, "pkt", "p", 
		"[rx | tx | itv [rand | <TIME ms>] | len <PKT_LEN>]", 
		"transfer packets over the network." },

	{ cmd_version, "version", "ver", "", 
		"show version information" },

	{ NULL, "", "", NULL, NULL }
};


const char * shell_prompt(void)
{
	return "[HUB]$ ";
}

void shell_greeting(FILE * f) 
{
	fprintf(f, "\n%s", version_str);
	fprintf(f, "\n%s\n\n", copyright_str);
}

int stdio_shell(void)
{
	DCC_LOG(LOG_TRACE, "...");

	return shell(stdout, shell_prompt, shell_greeting, shell_cmd_tab);
}

int usb_shell(usb_cdc_class_t * cdc)
{
	struct tty_dev * tty;
	FILE * f_tty;
	FILE * f_raw;

	f_raw = usb_cdc_fopen(cdc);
	tty = tty_attach(f_raw);
	f_tty = tty_fopen(tty);

//	stdout = f_tty;
//	stdin = f_tty;

	return shell(f_tty, shell_prompt, shell_greeting, shell_cmd_tab);
}

