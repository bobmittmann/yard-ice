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
		DCC_LOG1(LOG_MSG, "\"%c\"", s[0]);
	else if (n == 2)
		DCC_LOG2(LOG_MSG, "\"%c%c\"", s[0], s[1]);
	else if (n == 3)
		DCC_LOG3(LOG_MSG, "\"%c%c%c\"", s[0], s[1], s[2]);
	else if (n == 4)
		DCC_LOG4(LOG_MSG, "\"%c%c%c%c\"", s[0], s[1], s[2], s[3]);
	else if (n == 5)
		DCC_LOG5(LOG_MSG, "\"%c%c%c%c%c\"", s[0], s[1], s[2], s[3], s[4]);
	else if (n == 6)
		DCC_LOG6(LOG_MSG, "\"%c%c%c%c%c%c\"", 
				 s[0], s[1], s[2], s[3], s[4], s[5]);
	else if (n == 7)
		DCC_LOG7(LOG_MSG, "\"%c%c%c%c%c%c%c\"", 
				 s[0], s[1], s[2], s[3], s[4], s[5], s[6]);
	else 
		DCC_LOG8(LOG_MSG, "\"%c%c%c%c%c%c%c%c\" ...", 
				 s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7]);


	while (cmd->callback != NULL) {
		if ((cmd->name[n] == '\0' && strncmp(s, cmd->name, n) == 0) ||
			(cmd->alias[n] == '\0' && strncmp(s, cmd->alias, n) == 0)) {
			DCC_LOG1(LOG_MSG, "\"%s\"", cmd->name);
			return cmd;
		}
		cmd++;
	}

	DCC_LOG(LOG_WARNING, "not found...");

	return NULL;
}

