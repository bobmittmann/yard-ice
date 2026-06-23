
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
 * @file gdb-i.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */

#ifndef __GDB_I_H__
#define __GDB_I_H__

#ifdef CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/param.h>
#include <sys/file.h>
#include <sys/null.h>

#include <tcpip/tcp.h>
#include <sys/util.h>

#include "debugger.h"
#include "command.h"

#include "signals.h"

#ifndef GDB_ENABLE_MEMWRITE
#define GDB_ENABLE_MEMWRITE 0
#endif

#define THREAD_ID_ALL -1
#define THREAD_ID_ANY 0
#define THREAD_ID_NONE -2
#define THREAD_ID_IDLE (THINKOS_THREAD_IDLE + THREAD_ID_OFFS) 
#define THREAD_ID_IRQ (THINKOS_THREAD_VOID + THREAD_ID_OFFS)

#ifndef RSP_BUFFER_LEN
#if THINKOS_ENABLE_FPU
#define RSP_BUFFER_LEN (1024 + 0)
#else
#define RSP_BUFFER_LEN 1024
#endif
#endif

#ifndef GDB_ENABLE_NOACK_MODE
#define GDB_ENABLE_NOACK_MODE 1
#endif

#ifndef GDB_ENABLE_NOSTOP_MODE
#define GDB_ENABLE_NOSTOP_MODE 0
#endif

#ifndef GDB_ENABLE_VCONT
#define GDB_ENABLE_VCONT 0
#endif

#ifndef GDB_ENABLE_VFLASH
#define GDB_ENABLE_VFLASH 0
#endif

#ifndef GDB_ENABLE_MULTIPROCESS
#define GDB_ENABLE_MULTIPROCESS 0
#endif

#ifndef GDB_ENABLE_QXFER_FEATURES
#define GDB_ENABLE_QXFER_FEATURES 0
#endif

#ifndef GDB_ENABLE_QXFER_MEMORY_MAP
#define GDB_ENABLE_QXFER_MEMORY_MAP 0
#endif

#ifndef GDB_ENABLE_QXFER_THREADS
#define GDB_ENABLE_QXFER_THREADS 0
#endif

#ifndef GDB_ENABLE_COSMETIC
#define GDB_ENABLE_COSMETIC 0
#endif

#ifndef GDB_ENABLE_RXMIT
#define GDB_ENABLE_RXMIT 0
#endif

#define CTRL_B 0x02
#define CTRL_C 0x03
#define CTRL_D 0x04
#define CTRL_E 0x05
#define CTRL_F 0x06
#define CTRL_G 0x07
#define CTRL_H 0x08
#define CTRL_I 0x09
#define CTRL_J 0x0a
#define CTRL_K 0x0b
#define CTRL_L 0x0c
#define CTRL_M 0x0d /* CR */
#define CTRL_N 0x0e
#define CTRL_O 0x0f
#define CTRL_P 0x10
#define CTRL_Q 0x11
#define CTRL_R 0x12
#define CTRL_S 0x13
#define CTRL_T 0x14
#define CTRL_U 0x15
#define CTRL_V 0x16
#define CTRL_W 0x17
#define CTRL_X 0x18
#define CTRL_Y 0x19
#define CTRL_Z 0x1a

struct gdbtcpd {
	bool noack_mode;
	volatile bool connected;
	volatile bool running;
	uint8_t mutex;
	uint8_t cond;
	int last_signal; 
	int thread_id; 
	struct tcp_pcb * svc;
	struct tcp_pcb * volatile tp;
};

struct rsp_pkt_decoder {
	union {
		int c;
		const char * s;
	};
	int (* decoder)(struct gdbtcpd * gdb, char * pkt, int len);
};

#define LST_LEN(LST) (sizeof(LST) / sizeof(struct rsp_pkt_decoder))

#ifdef __cplusplus
extern "C" {
#endif

int uint2dec(char * s, unsigned int val);
uint64_t hex2uint64_be(const char * __s, char ** __endp);
uint32_t hex2uint32_be(const char * __s, char ** __endp);
int hex2int(const char * __s, char ** __endp);
char * prefix(const char * __s, const char * __prefix);
int char2hex(char * pkt, int c);
int str2str(char * pkt, const char * s);
int str2hex(char * pkt, const char * s);
int bin2hex(char * pkt, const void * buf, int len);
int int2str2hex(char * pkt, unsigned int val);
int uint2hex(char * s, unsigned int val);
int long2hex_be(char * pkt, unsigned long val);
int longlong2hex_be(char * s, unsigned long long val);
int uint2hex2hex(char * pkt, unsigned int val);
int hex2char(char * hex);
extern const char __hextab[];

int rsp_send_ack(struct tcp_pcb * tp);

int rsp_send_nack(struct tcp_pcb * tp);

int rsp_send_ok(struct tcp_pcb * tp);

int rsp_send_empty(struct tcp_pcb * tp);

int rsp_send_error(struct tcp_pcb * tp, int err);

int rsp_send_msg(struct tcp_pcb * tp, const char * s);

int rsp_send_pkt(struct tcp_pcb * tp, char * pkt, unsigned int len);

int rsp_send_stop_thread(struct tcp_pcb * tp, int signal, int thread);

int rsp_send_stop_core(struct tcp_pcb * tp, int signal, int core);

int rsp_send_signal(struct tcp_pcb * tp, int signal);

struct file * rsp_fopen(struct tcp_pcb * tp);

int rsp_fclose(struct file * f);

void rsp_comm_loop(struct gdbtcpd * gdb);

int gdb_brk_start(struct gdbtcpd * gdbd);

int rsp_decoder_lookup(const struct rsp_pkt_decoder lst[], unsigned int len, int key);

int rsp_query_lookup(const struct rsp_pkt_decoder lst[], 
					   unsigned int len, char * key);

#ifdef __cplusplus
}
#endif

#endif /* __GDB_I_H__ */

