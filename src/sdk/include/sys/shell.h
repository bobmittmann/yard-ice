/* 
 * Copyright(c) 2004-2012 BORESTE (www.boreste.com). All Rights Reserved.
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
 * @file sys/shell.h
 * @brief YARD-ICE libshell
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __SYS_SHELL_H__
#define __SYS_SHELL_H__

#include <stdio.h>

enum {
	SHELL_OK = 0,
	SHELL_ERR_GENERAL = -1,
	SHELL_ERR_CMD_INVALID = -2,
	SHELL_ERR_ARG_MISSING = -3,
	SHELL_ERR_ARG_INVALID = -4,
	SHELL_ERR_EXTRA_ARGS = -5,
	SHELL_ABORT = -6, 
};

typedef int (* shell_callback_t)(FILE * f, int argc, char ** argv);

struct shell_cmd {
	shell_callback_t callback;
	char name[11];
	char alias[5];
	char * const usage;
	char * const desc;
};

struct cmd_history;

#define SIZEOF_CMD_HISTORY 5

#ifdef __cplusplus
extern "C" {
#endif

struct shell_cmd * cmd_lookup(const struct shell_cmd cmd_tab[], char * line);

int cmd_exec(FILE * f, const struct shell_cmd * cmd, char * line);

char * cmd_get_next(char ** linep);

int shell(FILE * f, const char * (* prompt)(void), 
		  void (* greeting)(FILE *), 
		  const struct shell_cmd * cmd_tab);

void history_add(struct cmd_history * ht, char * s);

char * history_readline(struct cmd_history * ht, FILE * f, 
						char * buf, int max);

struct cmd_history * history_init(void * buf, unsigned int buf_len,
								  unsigned int line_len);


char * history_head(struct cmd_history * ht);

int shell_parseline(char * line, char ** argv, int argmax);

char * shell_stripline(char * line);

/* --------------------------------------------------------------------------
   Predefined commands
   --------------------------------------------------------------------------*/

int cmd_close(FILE * f, int argc, char ** argv);

int cmd_echo(FILE * f, int argc, char ** argv);

int cmd_clear(FILE * f, int argc, char ** argv);


int cmd_set(FILE * f, int argc, char ** argv);

int cmd_unset(FILE * f, int argc, char ** argv);

int cmd_get(FILE * f, int argc, char ** argv);

int cmd_cfgerase(FILE * f, int argc, char ** argv);

int cmd_reboot(FILE * f, int argc, char ** argv);

int cmd_sleep(FILE * f, int argc, char ** argv);


int cmd_osinfo(FILE * f, int argc, char ** argv);

int cmd_oscheck(FILE * f, int argc, char ** argv);

int cmd_thread(FILE * f, int argc, char ** argv);


int cmd_ifconfig(FILE * f, int argc, char ** argv);

int cmd_arp(FILE * f, int argc, char ** argv);

int cmd_route(FILE * f, int argc, char ** argv);

int cmd_netstat(FILE * f, int argc, char ** argv);

int cmd_ping(FILE * f, int argc, char ** argv);

int cmd_tcpstat(FILE * f, int argc, char ** argv);

int cmd_ipcfg(FILE * f, int argc, char ** argv);

int cmd_memxxd(FILE *f, int argc, char ** argv);

#ifdef __cplusplus
}
#endif

#endif /* __SYS_SHELL_H__ */

