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



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/shell.h>
#include <sys/tty.h>

#include <sys/serial.h>

#define __SHELL_I__
#include "shell-i.h"

#include <sys/dcclog.h>

struct shell_cmd * cmd_lookup(const char * s, 
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

int cmd_exec(FILE * f, char * line, const struct shell_cmd * cmd_tab)
{
	char * argv[SHELL_ARG_MAX];
	struct shell_cmd * cmd;
	int argc;

	if ((argc = shell_parseline(line, argv, SHELL_ARG_MAX)) == 0) {
		return 0;
	}

	if ((cmd = cmd_lookup(argv[0], cmd_tab)) == NULL) {
		DCC_LOG(LOG_TRACE, "cmd_lookup() == NULL");
		return SHELL_ERR_CMD_INVALID;
	}

	return cmd->callback(f, argc, argv);
}

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

int shell(FILE * f, const char * (* prompt)(void), 
		  const char * (* greeting)(void), const struct shell_cmd * cmd_tab)
{
	char line[SHELL_LINE_MAX];
	char * cp;
	char * cmd;
	int ret = 0;
	struct cmd_history history;

	DCC_LOG(LOG_TRACE, "history_init()");
	history_init(&history);

	if (greeting != NULL)
		fprintf(f, greeting());

	for (;;) {
		fprintf(f, "%s", prompt());

		if (history_readline(&history, f, line, SHELL_LINE_MAX) == NULL)
			return -1;

		if ((cp = shell_stripline(line)) == NULL)
			continue;

		history_add(&history, cp);
		cp = line;

		while ((cmd = get_cmd_next(&cp)) != NULL) {
			if ((ret = cmd_exec(f, cmd, cmd_tab)) < 0) {
				fprintf(f, "Error: %d\n", -ret);
				break;
			}
			
		}
	}
}

