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
 * @file gdb-rsp-i.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */

#ifndef __GDB_RSP_I_H__
#define __GDB_RSP_I_H__

#ifndef __GDB_RSP__
#error "Never use <gdb-rsp-i.h> directly; include <gdb-rsp.h> instead."
#endif 

#ifdef CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdbool.h>
#include <gdb.h>
#include "signals.h"

#ifndef RSP_BUFFER_LEN
#define RSP_BUFFER_LEN 512
#endif

#ifndef GDB_ENABLE_NOACK_MODE
#define GDB_ENABLE_NOACK_MODE 1
#endif

#ifndef GDB_ENABLE_NOSTOP_MODE
#define GDB_ENABLE_NOSTOP_MODE 1
#endif

#ifndef GDB_ENABLE_VCONT
#define GDB_ENABLE_VCONT 1
#endif

#ifndef GDB_ENABLE_MULTIPROCESS
#define GDB_ENABLE_MULTIPROCESS 0
#endif

#ifndef GDB_ENABLE_QXFER_FEATURES
#define GDB_ENABLE_QXFER_FEATURES 1
#endif

#ifndef GDB_ENABLE_QXFER_MEMORY_MAP
#define GDB_ENABLE_QXFER_MEMORY_MAP 1
#endif

#ifndef GDB_ENABLE_RXMIT
#define GDB_ENABLE_RXMIT 0
#endif

#define THREAD_ID_ALL -1
#define THREAD_ID_ANY 0
#define THREAD_ID_IDLE 33

struct gdb_comm {
	void * arg;
	const struct gdb_comm_op * op;
};

struct gdb_target {
	void * arg;
	const struct gdb_target_op  * op;
};

struct gdb_rsp {
	struct gdb_comm comm;
	struct gdb_target target;
	uint8_t noack_mode    : 1;
	uint8_t nonstop_mode  : 1;
	uint8_t stopped       : 1;
	uint8_t active_app    : 1;
	uint8_t session_valid : 1;
	uint8_t last_signal;
#if GDB_ENABLE_MULTIPROCESS
	uint16_t pid;
#endif
	struct {
		int8_t g; 
		int8_t c;
	} thread_id;
#if GDB_ENABLE_RXMIT
	struct {
		char * pkt;
		uint16_t len;
	} tx;
#endif
	char * pkt;
};

extern const char __hextab[];

#ifdef __cplusplus
extern "C" {
#endif

int uint2dec(char * s, unsigned int val);
unsigned long hex2int(const char * __s, char ** __endp);
bool prefix(const char * __s, const char * __prefix);
int char2hex(char * pkt, int c);
int str2str(char * pkt, const char * s);
int str2hex(char * pkt, const char * s);
int bin2hex(char * pkt, const void * buf, int len);
int int2str2hex(char * pkt, unsigned int val);
int uint2hex(char * s, unsigned int val);
int long2hex_be(char * pkt, unsigned long val);
int hex2char(char * hex);

#ifdef __cplusplus
}
#endif

#endif /* __GDB_RSP_I_H__ */

