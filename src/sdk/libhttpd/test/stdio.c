/* 
 * File:	 stdio.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
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


#include <sys/stm32f.h>
#include <sys/serial.h>
#include <sys/delay.h>
#include <sys/tty.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/shell.h>

const struct file stm32_uart_file = {
	.data = STM32_UART5, 
	.op = &stm32_usart_fops 
};


void stdio_init(void)
{
	struct serial_dev * console;
	struct tty_dev * tty;
	FILE * f_tty;
	FILE * f_raw;

	console = stm32f_uart5_serial_init(115200, SERIAL_8N1);
//	console = stm32f_uart5_serial_dma_init(115200, SERIAL_8N1);
	f_raw = serial_fopen(console);
	tty = tty_attach(f_raw);
	f_tty = tty_fopen(tty);

	stderr = (struct file *)&stm32_uart_file;
	stdout = f_tty;
	stdin = f_tty;
}

/*****************************************************************************
 * Help
 *****************************************************************************/

extern const struct shell_cmd shell_cmd_tab[];

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

int cmd_netcat(FILE *f, int argc, char ** argv);
int cmd_oscheck(FILE * f, int argc, char ** argv);

const struct shell_cmd shell_cmd_tab[] = {

	{ cmd_arp, "arp", "arp", 
		"[IPADDR [HWADDR]]", "show / manipulate the system ARP cache" },

	{ cmd_echo, "echo", "", 
		"[STRING]...", "Echo the STRING(s) to terminal" },

	{ cmd_get, "get", "", 
		"VAR", "get environement variable" },

	{ cmd_help, "help", "?", 
		"[COMMAND]", "show command usage (help [CMD])" },

	{ cmd_ifconfig, "ifconfig", "if", 
		"", "configure a network interface" },

	{ cmd_netcat, "netcat", "nc", 
		"", "connect to remote host" },

	{ cmd_netstat, "netstat", "n", 
		"HOST PORT", "print network connections" },

	{ cmd_osinfo, "sys", "os", 
		"", "show OS status" },

	{ cmd_oscheck, "check", "ck", 
		"", "check OS sanity" },

	{ cmd_ping, "ping", "", 
		"DESTINATION", "send ICMP ECHO_REQUEST to network hosts" },

	{ cmd_reboot, "reboot", "rst", "", 
		"reboot system" },

	{ cmd_route, "route", "rt", 
		"<add|del|help> DEST [NETMASK [GATEWAY]] iface",
		"show / manipulate the IP routing table" },

	{ cmd_set, "set", "", 
		"VAR EXPR", "set environement variable" },

	{ cmd_thread, "thread", "th", 
		"[ID]", "show thread status" },

	{ cmd_memxxd, "xxd", "x", "ADDR [COUNT]", 
		"prints a hexdump of a memory block" },

	{ NULL, "", "", NULL, NULL }
};


const char * shell_prompt(void)
{
	return "[WEBSRV]$ ";
}

extern const char * version_str;
extern const char * copyright_str;

void shell_greeting(FILE * f) 
{
	fprintf(f, "\n%s", version_str);
	fprintf(f, "\n%s\n\n", copyright_str);
}

int stdio_shell(void)
{
	return shell(stdout, shell_prompt, shell_greeting, shell_cmd_tab);
}

