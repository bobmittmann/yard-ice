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
 * @file exec.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/shell.h>

#ifndef SHELL_ARG_MAX 
#define SHELL_ARG_MAX 8
#endif

int shell_parseline(char * line, char ** argv, int argmax);
char * shell_stripline(char * line);

int shell_exec(FILE * f, char * line, const struct shell_cmd * cmd_tab)
{
	char * argv[SHELL_ARG_MAX];
	struct shell_cmd * cmd;
	int argc;
	char * s;

	if ((s = shell_stripline(line)) == NULL) {
		return 0;
	}

	if ((argc = shell_parseline(s, argv, SHELL_ARG_MAX)) == 0) {
		return 0;
	}

	if ((cmd = cmd_lookup(argv[0], cmd_tab)) == NULL) {
//		fprintf(f, "Command invalid. Type 'help'\n");
		return -1;
	}

	return cmd->callback(f, argc, argv);
}

