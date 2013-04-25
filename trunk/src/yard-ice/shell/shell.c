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

struct shell_cmd * shell_lookup(const char * s, 
								const struct shell_cmd * cmd_tab)
{
	struct shell_cmd * cmd = (struct shell_cmd *)cmd_tab; 

	while (cmd->callback != NULL) {
		if ((strcmp(s, cmd->name) == 0) || (strcmp(s, cmd->alias) == 0)) {
			return cmd;
		}
		cmd++;
	}

	return NULL;
}

const struct shell_cmd cmd_tab[] = {

	{ cmd_cfgerase, "cfgerase", "ce", 
		"", "restore default configuration" },

	{ cmd_close, "close", "quit", 
		"", "cause the shell to close" },

	{ cmd_connect, "connect", "con", 
		"", "connect to target" },

	{ cmd_breakpoint, "breakpt", "bp", 
		"", "set/modify breakpoint" },
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

	{ cmd_reset, "reset", "rst", 
		"[auto|soft|hard|core|dbg]", "reset the target" },

	{ cmd_show_context, "context", "cpu", 
		"", "show target context" },

	{ cmd_step, "step", "s", 
		"", "single step execution" },

	{ cmd_target, "target", "tgt", 
		"", "configure the target system" },

	{ cmd_run, "run", "r", 
		"", "resume target execution" },

	{ cmd_mem_read, "read", "mr", 
		"ADDR [SIZE]", "read a memory region" },

#if (ENABLE_NAND)
	{ cmd_bb_scan, "bbscan", "bbs", 
		"", "NAND bad block scan" },
#endif

	{ cmd_reboot, "reboot", "rb", 
		"", "system restart" },

	{ cmd_set, "set", "", 
		"VAR EXPR", "set environement variable" },

	{ cmd_test, "test", "tst", 
		"", "perform target test" },

	{ cmd_ice_test, "icetest", "icet", 
		"", "perform ICE tests" },

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

	{ cmd_mem_write, "write", "mw", 
		"ADDR W0 [W1 .. Wn]", "write into a memory region" },

	{ cmd_tap, "tap", "", 
		"[reset | info | probe | select N]", "JTAG TAP operations." },

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

	{ cmd_sleep, "sleep", "", 
		"", "delay for a specific amount of time" },

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

	{ cmd_route, "route", "rt", 
		"", "show / manipulate the IP routing table" },

	{ cmd_tcpstat, "tcpstat", "ts", 
		"", "print TCP stack info" },
#endif
#endif

//#if (ENABLE_NETWORK)
#if 0
	{ cmd_tcp_recv, "rcvnet", "rn", 
		"[ADDR] [PORT]", "receive file trough network" },

	{ cmd_tcp_send, "sndnet", "sn", 
		"[ADDR] [SIZE] [PORT]", "send a memory block trough network" },
#endif

	{ NULL, "", "", NULL, NULL }
};

const char yard_ice_greeting[] = "\n"
	"YARD-ICE " VERSION_NUM " - " VERSION_DATE "\n"
	"(c) Copyright 2013 - Bob Mittmann (bobmittmann@gmail.com)\n\n";

const char * const prompt_tab[] = {
	"[JTAGTOOL ERR]$ ",
	"[JTAGTOOL <!>]$ ",
	"[JTAGTOOL ...]$ ",
	"[JTAGTOOL (?)]$ ",
	"[JTAGTOOL < >]$ ",
	"[JTAGTOOL <->]$ ",
	"[JTAGTOOL RUN]$ ",
	"[JTAGTOOL BRK]$ "
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

int exec(FILE * f, char * line)
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

	if ((cmd = shell_lookup(argv[0], cmd_tab)) == NULL) {

		DCC_LOG(LOG_TRACE, "shell_lookup() == NULL");

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

struct cmd_history {
	uint8_t pos;
	uint8_t tail;
	uint8_t head;
	uint8_t max;
	char buf[SHELL_HISTORY_MAX][SHELL_LINE_MAX];
};

typedef struct cmd_history cmd_history_t;

void history_init(cmd_history_t * ht)
{
	ht->pos = 0;
	ht->head = 0;
	ht->tail = 0;
	ht->max = SHELL_HISTORY_MAX;
}

char * history_prev(cmd_history_t * ht)
{
	char * cp;

	if (ht->tail == ht->head)
		return NULL;

	if (ht->pos != ht->tail) {
		if (ht->pos == 0)
			ht->pos = ht->max;
		ht->pos--;
	}

	cp = ht->buf[ht->pos];

	DCC_LOG1(LOG_MSG, "pos=%d", ht->pos);

	return cp;
}

char * history_next(cmd_history_t * ht)
{
	char * cp;

	if (ht->tail == ht->head)
		return NULL;

	if (ht->pos != ht->head) {
		ht->pos++;
		if (ht->pos == ht->max)
			ht->pos = 0;
	}

	cp = ht->buf[ht->pos];

	DCC_LOG1(LOG_MSG, "pos=%d", ht->pos);

	return cp;
}

void history_add(cmd_history_t * ht, char * s)
{
	if ((s == NULL) || (*s == '\0'))
		return;

	if (ht->head != ht->tail) {
		int n;

		n = (ht->head == 0) ? ht->max : ht->head;
		n--;
	
		if (strcmp(s, ht->buf[n]) == 0) {
			/* don't repeat the last insertion,
			 just reposition the current pointer */
			ht->pos = ht->head;
			return;
		}
	}

	strcpy(ht->buf[ht->head], s);

	ht->head++;

	if (ht->head == ht->max)
		ht->head = 0;

	if (ht->head == ht->tail) {
		/* override the last line */
		ht->tail++;
		if (ht->tail == ht->max)
			ht->tail = 0;
		DCC_LOG2(LOG_MSG, "override tail=%d head=%d", ht->tail, ht->head);
	} else {
		DCC_LOG2(LOG_MSG, "tail=%d head=%d", ht->tail, ht->head);
	}
	
	/* invalidate the head position */
	*ht->buf[ht->head] = '\0';
	ht->pos = ht->head;
}

char * freadline_history(cmd_history_t * ht, FILE * f, 
						 char * buf, int max)
{
	int mode;
	int pos;
	int val;
	int len;
	int ctrl;
	int c;
	int i;
	char * s;

	if (isfatty(f)) {
		f = ftty_lowlevel(f);
	}

	mode = 0;
	pos = 0;
	val = 0;
	ctrl = 0;
	len = 0;
	/* reserve space for NULL termination */
	max--;
	for (;;) {
		if ((c = fgetc(f)) == EOF) {
			DCC_LOG(LOG_TRACE, "EOF");
			return NULL;
		}

		DCC_LOG1(LOG_MSG, "[%02x]", c);

		switch (mode) {
		case MODE_ESC:
			switch (c) {
			case '[':
				mode = MODE_ESC_VAL1;
				val = 0;
				ctrl = 0;
				break;
			case 'O':
				mode = MODE_ESC_O;
				break;
			default:
				DCC_LOG1(LOG_INFO, "ESC:'%c'", c);
				mode = 0;
			};
			continue;

		case MODE_ESC_VAL1:
		case MODE_ESC_VAL2:
			switch (c) {
			case '0'...'9':
				val = val * 10 + c - '0';
				continue;
			case 'A':
				/* cursor up */
				DCC_LOG2(LOG_MSG, "up %d, %d", ctrl, val);
				c = IN_CURSOR_UP + ctrl;
				break;
			case 'B':
				/* cursor down */
				DCC_LOG2(LOG_MSG, "down %d, %d", ctrl, val);
				c = IN_CURSOR_DOWN + ctrl;
				break;
			case 'C':
				/* cursor right */
				DCC_LOG2(LOG_MSG, "right %d, %d", ctrl, val);
				c = IN_CURSOR_RIGHT + ctrl;
				break;
			case 'D':
				/* cursor left */
				DCC_LOG2(LOG_MSG, "left %d, %d", ctrl, val);
				c = IN_CURSOR_LEFT + ctrl;
				break;
			case '~':
				switch (val) {
				case 1:
					DCC_LOG1(LOG_MSG, "home %d", ctrl);
					c = IN_HOME + ctrl;
					break;
				case 2:
					DCC_LOG1(LOG_MSG, "insert %d", ctrl);
					c = IN_INSERT + ctrl;
					break;
				case 3:
					/* delete */
					DCC_LOG1(LOG_MSG, "delete %d", ctrl);
					c = IN_DELETE + ctrl;
					break;
				case 5:
					DCC_LOG1(LOG_MSG, "pg up %d", ctrl);
					c = IN_PAGE_UP + ctrl;
					break;
				case 6:
					DCC_LOG1(LOG_MSG, "pg down %d", ctrl);
					c = IN_PAGE_DOWN + ctrl;
					break;
				default:
					DCC_LOG1(LOG_INFO, "seq %d", val);
					mode = 0;
					continue;
				}
				break;
			case ';':
				mode = MODE_ESC_VAL2;
				ctrl = IN_CTRL;
				val = 0;
				continue;
			default:
				DCC_LOG1(LOG_MSG, "VAL:'%c'", c);
				mode = 0;
				continue;
			};
			mode = 0;
			break;

		case MODE_ESC_O:
			switch (c) {
			case 'F':
				/* end */
				DCC_LOG(LOG_MSG, "end");
				c = IN_END;
				break;
			case 'H':
				/* end */
				DCC_LOG(LOG_MSG, "home");
				c = IN_HOME;
				break;
			default:
				DCC_LOG1(LOG_INFO, "ESC O:'%c'", c);
				mode = 0;
				continue;
			}
			mode = 0;
			break;
		} 

		switch (c) {
		case IN_ESC:
			DCC_LOG(LOG_MSG, "ESC");
			mode = MODE_ESC;
			continue;

		case IN_CURSOR_UP:
			if ((s = history_prev(ht)) == NULL)
				continue;
			goto set_buf;

		case IN_CURSOR_DOWN:
			if ((s = history_next(ht)) == NULL)
				continue;
			goto set_buf;

		case IN_CURSOR_RIGHT:
			if (pos < len)
				fputc(buf[pos++], f);
			else
				fputs(OUT_BEL, f);
			continue;

		case IN_CURSOR_LEFT:
			if (pos > 0) {
				fputs(OUT_CURSOR_LEFT, f);
				pos--;
			} else {
				fputs(OUT_BEL, f);
			}
			continue;

		case IN_CTRL_CURSOR_RIGHT:
			while (pos < len) {
				fputc(buf[pos++], f);
				if ((buf[pos - 1] != ' ') && (buf[pos] == ' '))
					break;
			} 
			continue;

		case IN_CTRL_CURSOR_LEFT:
			if (pos > 0) {
				do {
					fputs(OUT_CURSOR_LEFT, f);
					pos--;
					if ((buf[pos - 1] == ' ') && (buf[pos] != ' '))
						break;
				} while (pos > 0);
			}
			continue;

		case IN_PAGE_UP:
			continue;

		case IN_PAGE_DOWN:
			continue;

		case IN_INSERT:
			continue;

		case IN_HOME:
			while (pos > 0) {
				fputs(OUT_CURSOR_LEFT, f);
				pos--;
			} 
			continue;

		case IN_END:
			while (pos < len) {
				fputc(buf[pos++], f);
			} 
			continue;

		case IN_TN_BS:     
			DCC_LOG(LOG_MSG, "IN_TN_BS");
		case IN_BS:
			if (pos == 0) {
				fputs(OUT_BEL, f);
				continue;
			}
			if (len == pos) {
				pos--;
				len--;
				fputs(OUT_BS, f);
				continue;
			}

			fputs(OUT_CURSOR_LEFT, f);
			pos--;
			/* fall back */

		case IN_DELETE:

			if (len == pos) {
				continue;
			}
			len--;

			for (i = pos; i < len; i++) {
				buf[i] = buf[i + 1];
			}
			buf[len] = '\0';
			fputs(&buf[pos], f);
			fputc(' ', f);
			for (i = len + 1; i > pos; i--)
				fputs(OUT_CURSOR_LEFT, f);
			continue;

		case IN_EOL:
			buf[len] = '\0';
			fputs("\r\n", f);
			return buf;

		case IN_SKIP:
			fputs(OUT_SKIP, f);
			return NULL;
		}

		if (len == max) {
			fputs(OUT_BEL, f);
			continue;
		}

		if (len == pos) {
			fputc(c, f);
			buf[pos] = c;
			pos++;
			len++;
			continue;
		}

		for (i = len; i > pos; i--) {
			buf[i] = buf[i - 1];
		}

		len++;
		buf[pos] = c;
		buf[len] = '\0';

		fputs(&buf[pos], f);
		pos++;

		for (i = len; i > pos; i--)
			fputs(OUT_CURSOR_LEFT, f);

		continue;

set_buf:
		for (i = pos; i > 0; i--)
			fputs(OUT_CURSOR_LEFT, f);

		strcpy(buf, s);
		fputs(s, f);

		pos = strlen(s);
		if (pos < len) {
			for (i = pos; i < len; i++)
				fputc(' ', f);
			for (i = pos; i < len; i++)
				fputs(OUT_CURSOR_LEFT, f);
		}
		len = pos;

	}
}


#if (ENABLE_SHELL_THREAD) 
struct shell_context {
	FILE * f;
	char * line;
};

int shell_exec_task(struct shell_context * shell, uthread_id_t id)
{
		fprintf(shell->f, "Line='%s'\n", shell->line);
		return exec(shell->f, shell->line);
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
		c = *cp;
	} while (c != '\0');

	*linep = cp;

	return cmd;
}

int shell(FILE * f, const char * (* get_prompt)(void), 
			  const char * greeting)
{
	char line[SHELL_LINE_MAX];
	char * cp;
	char * cmd;
	char * prompt;
	int ret = 0;
#if ENABLE_SHELL_HISTORY
	cmd_history_t history;

	history_init(&history);
#endif

	if (greeting)
		fprintf(f, greeting);

	for (;;) {
		prompt = (char *)get_prompt();

#if ENABLE_SHELL_HISTORY
		fprintf(f, "%s", prompt);

		if (freadline_history(&history, f, line, SHELL_LINE_MAX) == NULL)
			return -1;

		if ((cp = shell_stripline(line)) == NULL)
			continue;

		history_add(&history, cp);
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
			if ((ret = exec(f, cmd)) < 0) {
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

	return (char * )prompt_tab[status + 3];
}

int console_shell(void)
{
	return shell(stdin, yard_ice_get_prompt, yard_ice_greeting);
}

