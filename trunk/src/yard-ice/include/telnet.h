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
 * @file .h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#ifndef __TELNET_H__
#define __TELNET_H__

#include <arpa/inet.h>
#include <arpa/telnet.h>

struct telnet_state;

typedef int (* tn_exec_callback_t)(struct telnet_state *, void *, int);

#ifdef __cplusplus
extern "C" {
#endif

int telnetd_start(void (*callback)(struct telnet_state *), int backlog);

void * telnet_attach(struct telnet_state * tn,
						 void (*callback)(struct telnet_state *));

int telnet_getc(struct telnet_state * tn);

int telnet_putc(int c, struct telnet_state * tn);

int telnet_write(struct telnet_state * tn, const void * buf, int len);

int telnet_read(struct telnet_state * tn, void * buf, int len);

int telnet_puts(char * s, struct telnet_state * tn);

int telnet_close(struct telnet_state * tn);

int telnet_iterate(struct telnet_state * tn, 
				   int (*proc)(struct telnet_state *, int), int parm);

int telnet_exec(struct telnet_state * tn, tn_exec_callback_t callback, 
				void * parm);

void telnet_shell(struct telnet_state * tn);

char * telnet_gets(char * s, int size, struct telnet_state * tn);

#ifdef __cplusplus
}
#endif

#endif /* __TELNET_H__ */

