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

#include <sys/serial.h>
#include <yard-ice/drv.h>

#include "target.h"
#include "debugger.h"
#include "version.h"
#include "command.h"
#include "eval.h"

#include <sys/dcclog.h>

#ifndef ENABLE_SHELL_HISTORY
#define ENABLE_SHELL_HISTORY 1
#endif

#ifndef ENABLE_SHELL_THREAD
#define EENABLE_SHELL_THREAD 0
#endif

#ifndef SHELL_LINE_MAX
#define SHELL_LINE_MAX 128
#endif

#ifndef SHELL_ARG_MAX 
#define SHELL_ARG_MAX 16
#endif

#ifndef SHELL_CMD_MAX 
#define SHELL_CMD_MAX 16
#endif

#ifndef SHELL_HISTORY_MAX
#define SHELL_HISTORY_MAX 32
#endif

#if ENABLE_TELNET || ENABLE_TFTP || ENABLE_GDB || ENABLE_DCC || ENABLE_VCOM
#ifndef ENABLE_NETWORK
#define ENABLE_NETWORK 1
#endif
#endif

struct shell_cmd * cmd_lookup(const struct shell_cmd cmd_tab[], char * line)
{
	struct shell_cmd * cmd = (struct shell_cmd *)cmd_tab; 
	char * s;
	char * cp;
	int n;

	if ((cp = line) == NULL)
		return NULL;

	/* remove leading spaces */
	for (; isspace(*cp); cp++);
	s = cp;

	/* get the command name lenght */
	for (; isalnum(*cp); cp++);
	n = cp - s;

	while (cmd->callback != NULL) {
		if ((cmd->name[n] == '\0' && strncmp(s, cmd->name, n) == 0) ||
			(cmd->alias[n] == '\0' && strncmp(s, cmd->alias, n) == 0)) {
			return cmd;
		}
		cmd++;
	}

	return NULL;
}

const char * yard_ice_greeting(void) 
{
	return "\nYARD-ICE " VERSION_NUM " - " VERSION_DATE "\n"
	"(c) Copyright 2013 - Bob Mittmann (bobmittmann@gmail.com)\n\n";
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

int shell_parseline(char * line, char ** argv, int argmax);
char * shell_stripline(char * line);

int exec(FILE * f, char * line, const struct shell_cmd * cmd_tab)
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
		printf(" '%s'", argv[n]);
	}
	printf("\n");
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

#define IN_BS      '\x8'
#define IN_TN_BS     0x7F /* TELNET back space */
#define IN_EOL      '\r'
#define IN_SKIP     '\3'
#define IN_EOF      '\x1A'
#define IN_ESC      '\033'

#define MK_IN_KEY(CODE)   (0x2000 + (CODE))
#define IN_CTRL        0x4000

#define IN_CURSOR_UP    MK_IN_KEY(0)
#define IN_CURSOR_DOWN  MK_IN_KEY(1)
#define IN_CURSOR_RIGHT MK_IN_KEY(2)
#define IN_CURSOR_LEFT  MK_IN_KEY(3)
#define IN_PAGE_UP      MK_IN_KEY(5)
#define IN_PAGE_DOWN    MK_IN_KEY(6)
#define IN_INSERT       MK_IN_KEY(7)
#define IN_DELETE       MK_IN_KEY(8)
#define IN_HOME         MK_IN_KEY(9)
#define IN_END          MK_IN_KEY(10)

#define IN_CTRL_CURSOR_UP    IN_CURSOR_UP + IN_CTRL 
#define IN_CTRL_CURSOR_DOWN  IN_CURSOR_DOWN + IN_CTRL   
#define IN_CTRL_CURSOR_RIGHT IN_CURSOR_RIGHT + IN_CTRL    
#define IN_CTRL_CURSOR_LEFT  IN_CURSOR_LEFT + IN_CTRL   
#define IN_CTRL_PAGE_UP      IN_PAGE_UP + IN_CTRL   
#define IN_CTRL_PAGE_DOWN    IN_PAGE_DOWN + IN_CTRL   

#define OUT_CURSOR_LEFT     "\x8"
#define OUT_BS              "\x8 \x8"
#define OUT_SKIP            "^C"
#define OUT_BEL             "\7"

char * freadline(FILE * f, const char * prompt, char * buf, int len);


#define MODE_ESC 1
#define MODE_ESC_VAL1 2
#define MODE_ESC_VAL2 3
#define MODE_ESC_O 4

struct cmd_history;

char * history_prev(struct cmd_history * ht);
char * history_next(struct cmd_history * ht);

#if (ENABLE_SHELL_THREAD) 
struct shell_context {
	FILE * f;
	char * line;
};

int shell_exec_task(struct shell_context * shell, uthread_id_t id)
{
		fprintf(shell->f, "Line='%s'\n", shell->line);
		return exec(shell->f, shell->line, cmd_tab);
}

int shell_thread;
uint32_t shell_stack[512];
#endif


static char * get_cmd_next(char ** linep)
{
	char * cp = *linep;
	char * cmd;
	int c;
	
	while ((c = *cp) == ' ')
		cp++;

	if (c == '\0')
		return NULL;

	cmd = cp;

	do {
		if (c == ';') {
			*cp = '\0';
			cp++;
			break;
		}

		cp++;

		/* Quotes */
		if ((c == '\'') || (c == '\"')) {
			int qt = c;
			for (; ((c = *cp) != '\0'); cp++) {
				if (c == qt) {
					cp++;
					break;
				}	
			}
		}

		c = *cp;
	} while (c != '\0');

	*linep = cp;

	return cmd;
}

int shell(FILE * f, const char * (* get_prompt)(void), 
		  const char * (* greeting)(void), const struct shell_cmd * cmd_tab)
{
#if ENABLE_SHELL_HISTORY
	char * line;
#else
	char line[SHELL_LINE_MAX];
#endif
	char * cp;
	char * cmd;
	char * prompt;
	int ret = 0;
#if ENABLE_SHELL_HISTORY
	char hist_buf[5 + SHELL_HISTORY_MAX * SHELL_LINE_MAX];
	struct cmd_history * history;

	DCC_LOG(LOG_TRACE, "history_init()");
	history = history_init(hist_buf, sizeof(hist_buf), SHELL_LINE_MAX);
#endif

	if (greeting)
		fprintf(f, greeting());

	for (;;) {
		prompt = (char *)get_prompt();

#if ENABLE_SHELL_HISTORY
		fprintf(f, "%s", prompt);

		/* use the history head as line buffer */
		line = history_head(history);

		if (history_readline(history, f, line, SHELL_LINE_MAX) == NULL)
			return -1;

		if ((cp = shell_stripline(line)) == NULL)
			continue;

		history_add(history, cp);
#else
		if (freadline(f, prompt, line, SHELL_LINE_MAX) == NULL)
			return -1;
#endif

#if (ENABLE_SHELL_THREAD) 
		struct shell_context shell;

		shell.f = f;
		shell.line = line;

		shell_thread = uthread_create(shell_stack, sizeof(shell_stack), 
									  (uthread_task_t)shell_exec_task, 
									  (void *)&shell, 1, NULL);
		if ((ret = uthread_join(shell_thread) ) < 0) {
			fprintf(f, "Error: %d\n", -ret);
		}
#else
		cp = line;

		while ((cmd = get_cmd_next(&cp)) != NULL) {
			if ((ret = exec(f, cmd, cmd_tab)) < 0) {
				fprintf(f, "Error: %d\n", -ret);
				break;
			}
			
		}
#endif
	}
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
		"[monitor | flush]", "handle the trace ring..." },

	{ cmd_sleep, "sleep", "", 
		"", "delay for a specific amount of time" },

	{ cmd_os, "sys", "os", 
		"", "show OS status" },

	{ cmd_thread, "thread", "th", 
		"[ID]", "show thread status" },

	{ cmd_fpga, "fpga", "rbf", 
		"[erase] [load]", "update FPGA program." },

	{ cmd_xflash, "xflash", "xf", 
		"[offs]", "update firmware." },

#if (ENABLE_NETWORK)
	{ cmd_ifconfig, "ifconfig", "if", 
		"", "configure a network interface" },

	{ cmd_ipcfg, "ipcfg", "ipcf", 
		"", "ip configuration utility" },

	{ cmd_ping, "ping", "", 
		"DESTINATION", "send ICMP ECHO_REQUEST to network hosts" },

	{ cmd_netstat, "netstat", "n", 
		"", "print network connections" },

#if 0
	{ cmd_arp, "arp", "arp", 
		"", "show / manipulate the system ARP cache" },
#endif
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
		"[ADDR] [PORT]", "receive file trough network" },
#endif
#if (ENABLE_TCP_SEND)
	{ cmd_tcp_send, "sndnet", "sn", 
		"[ADDR] [SIZE] [PORT]", "send a memory block trough network" },
#endif
#endif

	{ NULL, "", "", NULL, NULL }
};

int console_shell(void)
{
	struct uart_console_dev * dev;
	struct tty_dev * tty;
	FILE * f_tty;
	FILE * f_raw;

	dev = uart_console_init(115200, SERIAL_8N1);
	f_raw = uart_console_fopen(dev);
	tty = tty_attach(f_raw);
	f_tty = tty_fopen(tty);

	stdout = f_tty;
	stdin = stdout;

	return shell(f_tty, yard_ice_get_prompt, 
				 yard_ice_greeting, yard_ice_cmd_tab);
}


