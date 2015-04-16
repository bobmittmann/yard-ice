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
 * @file shell.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#ifdef CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <tcpip/telnetd.h>
#include <sys/shell.h>
#include <sys/tty.h>
#include <sys/stm32f.h>

#include <sys/serial.h>
#include <yard-ice/drv.h>

#include "target.h"
#include "debugger.h"
#include "version.h"
#include "command.h"
#include "eval.h"

#include <sys/dcclog.h>

#if ENABLE_TELNET || ENABLE_TFTP || ENABLE_GDB || ENABLE_DCC || ENABLE_VCOM
#ifndef ENABLE_NETWORK
#define ENABLE_NETWORK 1
#endif
#endif

void yard_ice_greeting(FILE * f) 
{
	fprintf(f, "\nYARD-ICE " VERSION_NUM " - " VERSION_DATE "\n"
	"(c) Copyright 2011-2014 - Bob Mittmann (bobmittmann@gmail.com)\n\n");
}

const char * const prompt_tab[] = {
	"[YARD-ICE <!>]$ ", /* DBG_ST_FAULT */
	"[YARD-ICE ERR]$ ", /* DBG_ST_ERROR */
	"[YARD-ICE SYN]$ ", /* DBG_ST_OUTOFSYNC */
	"[YARD-ICE ...]$ ", /* DBG_ST_BUSY */
	"[YARD-ICE (?)]$ ", /* DBG_ST_UNDEF */
	"[YARD-ICE < >]$ ", /* DBG_ST_UNCONNECTED */
	"[YARD-ICE <->]$ ", /* DBG_ST_CONNECTED */
	"[YARD-ICE RUN]$ ", /* DBG_ST_RUNNING */
	"[YARD-ICE BRK]$ "  /* DBG_ST_HALTED */
};

void show_uint32(FILE * f, uint32_t val)
{
	if (val < 10)
		fprintf(f, "%d\n", val);
	else
		if (val < 256)
			fprintf(f, "0x%02x (%d)\n", val, val);
		else 
			if (val < 65536)
				fprintf(f, "0x%04x (%d)\n", val, val);
			else
				fprintf(f, "0x%08x (%d)\n", val, val);
}

int exec(FILE * f, const struct shell_cmd * cmd_tab, char * line)
{
	char * argv[SHELL_ARG_MAX];
	struct shell_cmd * cmd;
	value_t val;
	int argc;
	int n;

/* if ((s = shell_stripline(line)) == NULL) {
		return 0;
	} */

	if ((argc = shell_parseline(line, argv, SHELL_ARG_MAX)) == 0) {
		return 0;
	}

#if 0
	for (n = 0; n < argc; n++) {
		fprintf(f, " '%s'", argv[n]);
	}
	fprintf(f, "\n");
#endif

	/* */
	if (argc > 2) {
		if (strcmp(argv[1], "=") == 0) {
			argv[1] = argv[0];
			argv[0] = "let";
		}
	}

	if ((cmd = cmd_lookup(cmd_tab, argv[0])) == NULL) {

		DCC_LOG(LOG_TRACE, "cmd_lookup() == NULL");

		if ((n = eval_uint32(&val, argc, argv)) < 0) {
			DCC_LOG(LOG_WARNING, "eval_uint32()");
			return n;
		}

		if (n != argc) {
			DCC_LOG(LOG_WARNING, "parse error");
			return -2;
		}

		show_uint32(f, val.uint32);
		return 0;
	}

	return cmd->callback(f, argc, argv);
}

int shell(FILE * f, const char * (* prompt)(void), 
		  void (* greeting)(FILE *), 
		  const struct shell_cmd * cmd_tab)
{
	char hist_buf[SIZEOF_CMD_HISTORY + SHELL_HISTORY_MAX * SHELL_LINE_MAX];
	char line[SHELL_LINE_MAX];
	struct cmd_history * history;
	int ret = 0;

	DCC_LOG(LOG_TRACE, "history_init()");
	history = history_init(hist_buf, sizeof(hist_buf), SHELL_LINE_MAX);

	if (greeting)
		greeting(f);

	do {
		char * stat;
		char * cp;

		fprintf(f, "%s", prompt());

		if (history_readline(history, f, line, SHELL_LINE_MAX) == NULL)
			return -1;

		if ((cp = shell_stripline(line)) == NULL)
			continue;

		history_add(history, cp);

		cp = line;

		ret = 0;

		while ((stat = cmd_get_next(&cp)) != NULL) {
#if 0
			struct shell_cmd * cmd;

			if ((cmd = cmd_lookup(cmd_tab, stat)) == NULL) {
				fprintf(f, "Command not found!\n");
				break;
			}

			ret = cmd_exec(f, cmd, stat);
#else
			ret = exec(f, cmd_tab, stat);
#endif
			if ((ret < 0) && (ret !=  SHELL_ABORT)) {
				fprintf(f, "Error: %d\n", -ret);
				break;
			}
			
		}
	} while (ret != SHELL_ABORT); 

	return 0;
}

const char * yard_ice_get_prompt(void)
{
	int status;

	status = target_status();

	return (char * )prompt_tab[status + 4];
}

const struct shell_cmd yard_ice_cmd_tab[] = {

	{ cmd_beep, "beep", "", 
		"[tone [time]]", "play a tone" },

	{ cmd_breakpoint, "breakpt", "bp", 
		"", "set/modify breakpoint" },

	{ cmd_cfgerase, "cfgerase", "ce", 
		"", "restore default configuration" },

	{ cmd_close, "close", "quit", 
		"", "cause the shell to close" },

	{ cmd_connect, "connect", "con", 
		"", "connect to target" },

	{ cmd_show_context, "context", "cpu", 
		"", "show target context" },

#if 0
	{ cmd_dint, "dint", "di", 
		"", "disable interrupts" },
#endif
	{ cmd_disassemble, "dasm", "d", 
		"", "disassemble" },
#if 0
	{ cmd_eint, "eint", "ei", 
		"", "enable interrupts" },
#endif
	{ cmd_mem_erase, "erase", "er", 
		"ADDR [SIZE]", "erase a memory region" },

	{ cmd_get, "get", "", 
		"VAR", "get environement variable" },

	{ cmd_goto, "goto", "g", 
		"", "move the program counter" },

	{ cmd_halt, "halt", "h", 
		"", "halt (break) the target" },

	{ cmd_help, "help", "?", 
		"[COMMAND]", "show command usage (help [CMD])" },

	{ cmd_info, "info", "inf", 
		"", "show Target and ICE information" },

	{ cmd_init, "init", "ini", 
		"", "perform target initialization" },

	{ cmd_let, "let", "@", 
		"VAR EXPR", "assign a value to variable" },

	{ cmd_meminfo, "meminfo", "mi", 
		"", "show memory info" },

	{ cmd_probe, "probe", "pb", 
		"", "probe for target" },

	{ cmd_release, "release", "rel", 
		"", "release/disconnect from target" },

	{ cmd_mem_read, "read", "mr", 
		"ADDR [SIZE]", "read a memory region" },

	{ cmd_reset, "reset", "rst", 
		"[auto|soft|hard|core|dbg]", "reset the target" },

	{ cmd_run, "run", "r", 
		"", "resume target execution" },

	{ cmd_step, "step", "s", 
		"", "single step execution" },

	{ cmd_target, "target", "tgt", 
		"", "configure the target system" },

#if (ENABLE_NAND)
	{ cmd_bb_scan, "bbscan", "bbs", 
		"", "NAND bad block scan" },
#endif

#if (ENABLE_NAND)
	{ cmd_scrub, "scrub", "scb", 
		"", "NAND block forced erase" },
#endif

	{ cmd_reboot, "reboot", "rb", 
		"", "system restart" },

	{ cmd_set, "set", "", 
		"VAR EXPR", "set environement variable" },

	{ cmd_test, "test", "tst", 
		"", "perform target test" },

	{ cmd_ice_test, "icetest", "icet", 
		"REQ [ARG1 .. ARGn]", "perform ICE tests" },

	{ cmd_ice_info, "ice_info", "ice", 
		"", "show ICE details" },

	{ cmd_unset, "unset", "", 
		"VAR", "clear environement variable" },

	{ cmd_version, "version", "ver", 
		"", "show version" },

	{ cmd_watchpoint, "watchpt", "wp", 
		"", "set/modify watchpoint" },

	{ cmd_power, "power", "pwr", 
		"[on | off | cycle]", "control the target power ..." },

	{ cmd_relay, "relay", "rly", 
		"[on | off | cycle]", "control on board relay..." },

	{ cmd_mem_write, "write", "mw", 
		"ADDR W0 [W1 .. Wn]", "write into a memory region" },

	{ cmd_tap, "tap", "", 
		"[reset | info | probe | select N]", "JTAG TAP operations." },

	{ cmd_i2c, "i2c", "", 
		"[reset | info | probe | select N]", "I2C operations." },

	{ cmd_enable, "enable", "en", 
		"[poll | comm ]", "enable feature." },

	{ cmd_disable, "disable", "dis", 
		"[poll | comm ]", "disable feature." },

	{ cmd_trst, "trst", "tr", 
		"[set | clr | [pulse TIME(ms)]]", "assert the TRST signal ..." },

	{ cmd_nrst, "nrst", "nr", 
		"[set | clr | [pulse TIME(ms)]]", "assert the nRST signal ..." },

	{ cmd_irscan, "irscan", "ir", 
		"VECTOR LEN", "scan a vector int the JTAG instruction register" },

	{ cmd_drscan, "drscan", "dr", 
		"VECTOR LEN", "scan a vector int the JTAG instruction register" },

	{ cmd_irpause, "irpause", "ip", 
		"COUNT", "" },

	{ cmd_drpause, "drpause", "dp", 
		"COUNT", "" },

	{ cmd_runtest, "runtest", "idle", 
		"COUNT", "cycle the JTAG TCK <COUNT> times" },

	{ cmd_idcode, "idcode", "id", 
		"[TAP]", "get the JTAG IDCODE for a device in the chain" },

	{ cmd_var, "variable", "var", 
		"", "show global variables" },

	{ cmd_trace, "trace", "tc", 
		"[monitor | flush | auto | keep]", "handle the trace ring..." },

	{ cmd_sleep, "sleep", "", 
		"", "delay for a specific amount of time" },

	{ cmd_osinfo, "sys", "os", 
		"", "show OS status" },

	{ cmd_oscheck, "check", "ck", 
		"", "perform OS sanity check" },

	{ cmd_thread, "thread", "th", 
		"[ID]", "show thread status" },

	{ cmd_fpga, "fpga", "rbf", 
		"[erase] [load]", "update FPGA program." },

	{ cmd_xflash, "xflash", "xf", 
		"firmware", "update firmware." },

#if (ENABLE_NETWORK)
	{ cmd_ifconfig, "ifconfig", "if", 
		"", "configure a network interface" },

	{ cmd_ipcfg, "ipcfg", "ipcf", 
		"", "ip configuration utility" },

	{ cmd_ping, "ping", "", 
		"DESTINATION", "send ICMP ECHO_REQUEST to network hosts" },

	{ cmd_netstat, "netstat", "n", 
		"", "print network connections" },

	{ cmd_arp, "arp", "arp", 
		"", "show / manipulate the system ARP cache" },

	{ cmd_route, "route", "rt", 
		"", "show / manipulate the IP routing table" },

#if 0
	{ cmd_tcpstat, "tcpstat", "ts", 
		"", "print TCP stack info" },
#endif
#endif

#if (ENABLE_NETWORK)
#if (ENABLE_TCP_RECV)
	{ cmd_tcp_recv, "rcvnet", "rn", 
		"[ADDR] [PORT]", "receive file through network" },
#endif
#if (ENABLE_TCP_SEND)
	{ cmd_tcp_send, "sndnet", "sn", 
		"[ADDR] [SIZE] [PORT]", "send a memory block through network" },
#endif
#endif

	{ NULL, "", "", NULL, NULL }
};

int console_shell(void)
{
	struct serial_dev * console;
	struct tty_dev * tty;
	FILE * f_tty;
	FILE * f_raw;

	console = stm32f_uart5_serial_init(115200, SERIAL_8N1);
	f_raw = serial_fopen(console);
	tty = tty_attach(f_raw);
	f_tty = tty_fopen(tty);

	stdout = f_tty;
	stdin = f_tty;
	
	return shell(f_tty, yard_ice_get_prompt, 
				 yard_ice_greeting, yard_ice_cmd_tab);
}

