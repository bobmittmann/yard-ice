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

struct shell_cmd * cmd_lookup(const struct shell_cmd cmd_tab[], char * line)
{
	struct shell_cmd * cmd = (struct shell_cmd *)cmd_tab; 
	char * s;
	char * cp;
	int n;

	if ((cp = line) == NULL) {
		DCC_LOG(LOG_WARNING, "NULL pointer...");
		return NULL;
	}

	/* remove leading spaces */
	for (; isspace(*cp); cp++);
	s = cp;

	/* get the command name lenght */
	for (; (*cp != '\0') && (*cp != ';') && !isspace(*cp); cp++);
	n = cp - s;

	if (n == 0) {
		DCC_LOG(LOG_WARNING, "empty line...");
		return NULL;
	}

	if (n == 1)
		DCC_LOG1(LOG_INFO, "%c", s[0]);
	else if (n == 2)
		DCC_LOG2(LOG_INFO, "%c%c", s[0], s[1]);
	else
		DCC_LOG3(LOG_INFO, "%c%c%c...", s[0], s[1], s[2]);

	while (cmd->callback != NULL) {
		DCC_LOG1(LOG_MSG, "'%s'", cmd->name);
		if ((cmd->name[n] == '\0' && strncmp(s, cmd->name, n) == 0) ||
			(cmd->alias[n] == '\0' && strncmp(s, cmd->alias, n) == 0)) {
			return cmd;
		}
		cmd++;
	}

	DCC_LOG(LOG_WARNING, "not found...");

	if (n == 1)
		DCC_LOG1(LOG_TRACE, "%c", s[0]);
	else if (n == 2)
		DCC_LOG2(LOG_TRACE, "%c%c", s[0], s[1]);
	else
		DCC_LOG3(LOG_TRACE, "%c%c%c...", s[0], s[1], s[2]);

	cmd = (struct shell_cmd *)cmd_tab; 

	while (cmd->callback != NULL) {
		DCC_LOG1(LOG_TRACE, "'%s'", cmd->name);
		if ((cmd->name[n] == '\0' && strncmp(s, cmd->name, n) == 0) ||
			(cmd->alias[n] == '\0' && strncmp(s, cmd->alias, n) == 0)) {
			return cmd;
		}
		cmd++;
	}

	return NULL;
}

int cmd_exec(FILE * f,  const struct shell_cmd * cmd, char * line)
{
	char * argv[SHELL_ARG_MAX];
	int argc;

	if ((argc = shell_parseline(line, argv, SHELL_ARG_MAX)) == 0)
		return 0;

	return cmd->callback(f, argc, argv);
}

char * cmd_get_next(char ** linep)
{
	char * cp = *linep;
	char * cmd;
	int c;
	
	/* remove leading spaces */
	for (; isspace(c = *cp); cp++);

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

