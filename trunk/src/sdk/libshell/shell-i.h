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
 * @file shell-i.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


/*****************************************************************************
 * CHIME internal (private) header file
 *****************************************************************************/

#ifndef __SHELL_I_H__
#define __SHELL_I_H__

#ifndef __SHELL_I__
#error "Never use <chime-i.h> directly; include <chime.h> instead."
#endif

#ifdef CONFIG_H
#include "config.h"
#endif

#ifndef SHELL_LINE_MAX
#define SHELL_LINE_MAX 128
#endif

#ifndef SHELL_ARG_MAX 
#define SHELL_ARG_MAX 16
#endif

#ifndef SHELL_HISTORY_MAX
#define SHELL_HISTORY_MAX 32
#endif

struct cmd_history {
	uint8_t pos;
	uint8_t tail;
	uint8_t head;
	uint8_t max;
	char buf[SHELL_HISTORY_MAX][SHELL_LINE_MAX];
};

void history_init(struct cmd_history * ht);

char * history_prev(struct cmd_history * ht);

char * history_next(struct cmd_history * ht);

void history_add(struct cmd_history * ht, char * s);

char * history_readline(struct cmd_history * ht, FILE * f, 
						char * buf, int max);

int shell_parseline(char * line, char ** argv, int argmax);

char * shell_stripline(char * line);

#ifdef __cplusplus
}
#endif

#endif /* __SHELL_I_H__ */

