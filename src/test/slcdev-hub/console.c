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

#include <sys/shell.h>

#include <xmodem.h>

#include <thinkos.h>

extern const struct shell_cmd cmd_tab[];

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


const struct shell_cmd cmd_tab[] = {

	{ cmd_help, "help", "?", 
		"[COMMAND]", "show command usage (help [CMD])" },


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

int console_shell(void)
{
	return shell(stdout, get_prompt, shell_greeting, cmd_tab);
}

#if 0
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

	return shell(f_tty, get_prompt, shell_greeting);
}
#endif

