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

typedef int (* shell_callback_t)(FILE * f, int argc, char ** argv);

struct shell_cmd {
	shell_callback_t callback;
	char name[11];
	char alias[5];
	char * const usage;
	char * const desc;
};

#ifdef __cplusplus
extern "C" {
#endif

char * shell_stripline(char * line);

int shell_parseline(char * line, char ** argv, int argmax);

struct shell_cmd * shell_lookup(const char * s, 
								const struct shell_cmd * cmd_tab);

/*! \brief Run a shell environement on file descriptor
 *
 *	\param fd \n
 *  File descriptor openned for both reading and writing
 *
 *	\param prompt \n
 *  A function callback to get the prompt indication ...
 *
 *	\param cmd_tab \n
 *  An array of shell_cmd structures. The last entry of the list
 *  must have all fields set to NULL or empty strings.
 *
 *	\return
 *  Returns ....
 */
int shell(int fd, const char * (* get_prompt)(void), 
		  const struct shell_cmd * cmd_tab);

char * freadline(FILE * f, const char * prompt, char * buf, int len);

int shell_exec(FILE * f, char * line, const struct shell_cmd * cmd_tab);

#ifdef __cplusplus
}
#endif

#endif /* __SYS_SHELL_H__ */

