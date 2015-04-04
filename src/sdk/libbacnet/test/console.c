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
#include <thinkos.h>

#include <sys/shell.h>

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

int cmd_bacnet(FILE * f, int argc, char ** argv)
{
	if (argc > 1)
		return SHELL_ERR_EXTRA_ARGS;

	return SHELL_ABORT;
}

int cmd_stdout(FILE * f, int argc, char ** argv)
{
	int i;

	fprintf(stdout, "----------------------\n");

	for (i = 1; i < argc; ++i) {
		if (i != 1)
			fprintf(stdout, " ");
		fprintf(stdout, argv[i]);
	}

	fprintf(stdout, "\n");

	return 0;
}

int cmd_stderr(FILE * f, int argc, char ** argv)
{
	int i;

	fprintf(stderr, "----------------------\n");

	for (i = 1; i < argc; ++i) {
		if (i != 1)
			fprintf(stderr, " ");
		fprintf(stderr, argv[i]);
	}

	fprintf(stderr, "\n");

	return 0;
}

int cmd_busy(FILE * f, int argc, char ** argv)
{
	uint32_t tmo;
	uint32_t dt = 100;

	if (argc > 1)
		dt = strtoul(argv[1], NULL, 0);

	fprintf(f, "Keeping busy for %d ms ....\n", dt);

	tmo = thinkos_clock() + dt;

	while ((int32_t)(thinkos_clock() - tmo) < 0) {
	}


	return 0;
}

const struct shell_cmd shell_cmd_tab[] = {

	{ cmd_bacnet, "BACnet", "", "", 
		"Start BACnet Data Link Connection" },

	{ cmd_echo, "echo", "", 
		"[STRING]...", "Echo the STRING(s) to terminal" },

	{ cmd_get, "get", "", 
		"VAR", "get environement variable" },

	{ cmd_help, "help", "?", 
		"[COMMAND]", "show command usage (help [CMD])" },

	{ cmd_osinfo, "sys", "os", 
		"", "show OS status" },

	{ cmd_reboot, "reboot", "r", "", 
		"reboot system" },

	{ cmd_set, "set", "", 
		"VAR EXPR", "set environement variable" },

	{ cmd_stdout, "stdout", ">", "", "" },

	{ cmd_stderr, "stderr", "&", "", "" },

	{ cmd_busy, "busy", "b", "", "" },

	{ cmd_memxxd, "xxd", "x", "", "" },

#if 0
	{ cmd_ifconfig, "ifconfig", "if", 
		"", "configure a network interface" },

	{ cmd_netstat, "netstat", "n", 
		"", "print network connections" },

	{ cmd_ifconfig, "ifconfig", "if", 
		"", "configure a network interface" },

	{ cmd_netstat, "netstat", "n", 
		"", "print network connections" },

	{ cmd_thread, "thread", "th", 
		"[ID]", "show thread status" },
#endif


	{ NULL, "", "", NULL, NULL }
};

