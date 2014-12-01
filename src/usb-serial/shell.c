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
#include <sys/tty.h>
#include <sys/param.h>
#include <xmodem.h>
#include <thinkos.h>

#include <sys/dcclog.h>
#include <sys/delay.h>

#include "board.h"

extern const struct shell_cmd shell_cmd_tab[];
extern const char * version_str;
extern const char * copyright_str;

/* -------------------------------------------------------------------------
 * Help
 * ------------------------------------------------------------------------- */

int cmd_help(FILE *f, int argc, char ** argv)
{
	struct shell_cmd * cmd;

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

	fprintf(f, "\n Command:   Alias:  Desciption: \n");
	for (cmd = (struct shell_cmd *)shell_cmd_tab; 
		 cmd->callback != NULL; cmd++) {
		fprintf(f, "  %-10s %-4s   %s\n", cmd->name, cmd->alias, cmd->desc);
	}

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


int usb_xflash(uint32_t offs, uint32_t len);

int cmd_xflash(FILE * f, int argc, char ** argv)
{
	uint32_t offs = 0x00000;
	uint32_t size = 0x00000;
	uint32_t pri;
	int ret;

	if (argc < 2)
		return SHELL_ERR_ARG_MISSING;

	if (argc > 2)
		return SHELL_ERR_EXTRA_ARGS;

	do {
		if ( (strcmp(argv[1], "firm") == 0) ||
			 (strcmp(argv[1], "f") == 0)) {
			offs = 0;
			size = 32 * 1024;
			fprintf(f, "Firmware update...\n");
			break;;
		} 
		return SHELL_ERR_ARG_INVALID;
	} while (0);

	fflush(f);

	pri = cm3_primask_get();
	cm3_primask_set(1);
	ret = usb_xflash(offs, size);
	cm3_primask_set(pri);

	return ret;
}


const struct shell_cmd shell_cmd_tab[] = {

	{ cmd_help, "help", "?", 
		"[COMMAND]", "show command usage (help [CMD])" },

	{ cmd_version, "version", "ver", "", 
		"show version information" },

	{ cmd_xflash, "xflash", "xf", 
		"firm", "update firmware." },

	{ NULL, "", "", NULL, NULL }
};

