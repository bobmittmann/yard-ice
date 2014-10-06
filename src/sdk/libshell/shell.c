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

#ifndef SHELL_HISTORY_MAX
#define SHELL_HISTORY_MAX 5
#endif

int shell(FILE * f, const char * (* prompt)(void), 
		  const char * (* greeting)(void), 
		  const struct shell_cmd cmd_tab[])
{
//	char line[SHELL_LINE_MAX];
	char * line;
	char hist_buf[5 + SHELL_HISTORY_MAX * SHELL_LINE_MAX];
	struct cmd_history * history;
	struct shell_cmd * cmd;
	char * cp;
	char * stat;
	int ret = 0;

	DCC_LOG(LOG_TRACE, "history_init()");
	history = history_init(hist_buf, sizeof(hist_buf), SHELL_LINE_MAX);

	if (greeting != NULL)
		fprintf(f, greeting());

	for (;;) {
		fprintf(f, "%s", prompt());

		line = history_head(history);

		if (history_readline(history, f, line, SHELL_LINE_MAX) == NULL)
			return -1;

		if ((cp = shell_stripline(line)) == NULL)
			continue;

		history_add(history, cp);
		cp = line;

		while ((stat = cmd_get_next(&cp)) != NULL) {
			if ((cmd = cmd_lookup(cmd_tab, stat)) == NULL) {
				fprintf(f, "Command not found!\n");
				break;
			}
			if ((ret = cmd_exec(f, cmd, stat)) < 0) {
				fprintf(f, "Error: %d\n", -ret);
				break;
			}
			
		}
	}
}

