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
 * @file gdb-rsp.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/file.h>
#include <sys/fd.h>
#include <sys/null.h>
#include <command.h>


#include <netinet/in.h>
#include <tcpip/tcp.h>
#include <unistd.h>
#include <sys/signal.h>

#include "gdb_rspd.h"
#include "debugger.h"

#ifdef ARM7ICE_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif

#include <debug.h>

#include <sys/dcclog.h>

#ifndef RSP_BUFFER_LEN
#define RSP_BUFFER_LEN 512
#endif

int8_t gdb_noack_mode = 0;
int gdb_target_run_sem;


static const char hextab[] = { 
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};


/*
 * Common response packets
 */

static inline int rsp_ack(struct tcp_pcb * tp)
{
	return tcp_send(tp, "+", 1, TCP_SEND_NOWAIT);
}

#if 0
static int rsp_nack(struct tcp_pcb * tp)
{
	return tcp_send(tp, "-", 1, TCP_SEND_NOWAIT);
}
#endif

static inline int rsp_ok(struct tcp_pcb * tp)
{
	return tcp_send(tp, "$OK#9a", 6, TCP_SEND_NOWAIT);
}

static int rsp_empty(struct tcp_pcb * tp)
{
	return tcp_send(tp, "$#00", 4, TCP_SEND_NOWAIT);
}

static int rsp_error(struct tcp_pcb * tp, int err)
{
	char pkt[32];
	int sum;

	pkt[0] = '$';
	pkt[1] = sum = 'E';
	sum += pkt[2] = hextab[((err >> 4) & 0xf)];
	sum += pkt[3] = hextab[(err & 0xf)];
	pkt[4] = '#';
	pkt[5] = hextab[((sum >> 4) & 0xf)];
	pkt[6] = hextab[sum & 0xf];

	return tcp_send(tp, pkt, 7, TCP_SEND_NOWAIT);
}

static int rsp_msg(struct tcp_pcb * tp, char * pkt, const char * s)
{
	int sum;
	int n;
	char c;

	pkt[0] = '$';
	pkt[1] = sum = 'O';
	n = 2;
	while (*s) {
		c = hextab[((*s >> 4) & 0xf)];
		pkt[n++] = c;
		sum += c;
		c = hextab[*s & 0xf];
		pkt[n++] = c;
		sum += c;
		s++;
	}
	pkt[n++] = '#';
	pkt[n++] = hextab[((sum >> 4) & 0xf)];
	pkt[n++] = hextab[sum & 0xf];

	return tcp_send(tp, pkt, n, TCP_SEND_NOWAIT);
}

static int rsp_send_pkt(struct tcp_pcb * tp, char * pkt_buf, 
						const char * buf, int len)
{
	int sum = 0;
	char c;
	int n;
	int i;

	pkt_buf[0] = '$';
	n = 1;
	for (i = 0; i < len; i++) {
		c = buf[i];
		pkt_buf[n++] = c;
		sum += c;
	}
	pkt_buf[n++] = '#';
	pkt_buf[n++] = hextab[((sum >> 4) & 0xf)];
	pkt_buf[n++] = hextab[sum & 0xf];

	return tcp_send(tp, pkt_buf, n, TCP_SEND_NOWAIT);
}

static int rsp_signal(struct tcp_pcb * tp, char * pkt, int sig)
{
	int sum;;

	pkt[0] = '$';
	pkt[1] = sum = 'S';
	sum += pkt[2] = hextab[((sig >> 4) & 0xf)];
	sum += pkt[3] = hextab[(sig & 0xf)];
	pkt[4] = '#';
	pkt[5] = hextab[((sum >> 4) & 0xf)];
	pkt[6] = hextab[sum & 0xf];

	return tcp_send(tp, pkt, 7, TCP_SEND_NOWAIT);
}

static int rsp_break_signal(struct tcp_pcb * tp, char * pkt)
{
//	int state;

	DCC_LOG(LOG_TRACE, "break received, stopping...");

	if (target_halt(0) < 0) {
		return rsp_error(tp, 1);
	}

	/* signal that we are now running */
	uthread_sem_post(gdb_target_run_sem);

	return 0;
#if 0
	if ((state = target_halt_wait(500)) == ERR_TIMEOUT) {
		DCC_LOG(LOG_TRACE, "timeout...");
	}

	if (state == DBG_ST_HALTED) {
		DCC_LOG(LOG_TRACE, "halted");
		return rsp_signal(tp, pkt, SIGTRAP);
	}

	return rsp_msg(tp, pkt, "JTAGTOOL: target_halt failed!");
#endif
}

#if 0
static inline void rsp_fixup_sum(char * s)
{
	int sum;

	sum = 0;
	while (*s != '#')
		sum += *s++;

	s[1] = hextab[((sum >> 4) & 0xf)];
	s[2] = hextab[sum & 0xf];
}

static int  rsp_offsets(struct tcp_pcb * tp, unsigned int text,
							   unsigned int data, unsigned int bss)
{
	char s[128];
	int n;

	DCC_LOG3(LOG_TRACE, "text=%08x data=%08x bss=%08x", text, data, bss);

	/* FIXME: max id = 15 by the time */
	n = sprintf(s, "+$Text=%x;Data=%x;Bss=%x#", text, data, bss);
	rsp_fixup_sum(s + 2);
	return tcp_send(tp, s, n + 2, TCP_SEND_NOWAIT);
}
#endif

static int rsp_thread_get_first(struct tcp_pcb * tp)
{
	char pkt[32];
	int sum;;

	DCC_LOG(LOG_TRACE, ".");

	pkt[0] = '$';
	pkt[1] = sum = 'm';
	sum += pkt[2] = '0';
	pkt[3] = '#';
	pkt[4] = hextab[((sum >> 4) & 0xf)];
	pkt[5] = hextab[sum & 0xf];

	return tcp_send(tp, pkt, 6, TCP_SEND_NOWAIT);
}

static int rsp_thread_get_next(struct tcp_pcb * tp)
{
	char pkt[32];
	int sum;;

	DCC_LOG(LOG_TRACE, ".");

	pkt[0] = '$';
	pkt[1] = sum = 'l';
	pkt[2] = '#';
	pkt[3] = hextab[((sum >> 4) & 0xf)];
	pkt[4] = hextab[sum & 0xf];

	return tcp_send(tp, pkt, 5, TCP_SEND_NOWAIT);
}

static int rsp_last_signal(struct tcp_pcb * tp, char * pkt, int len)
{
	int state;

	state = target_status();
	if (state < DBG_ST_CONNECTED) {
		DCC_LOG(LOG_WARNING, "target not connected!");
		return rsp_error(tp, state);
	}

	if (state != DBG_ST_HALTED) {
		DCC_LOG(LOG_TRACE, "running");

		if ((state = target_halt(0)) < 0) {
			DCC_LOG(LOG_WARNING, "target_halt() failed!");
			rsp_msg(tp, pkt, "JTAGTOOL: halt fail\n");
			return rsp_error(tp, 1);
		}

		if ((state = target_halt_wait(500)) == ERR_TIMEOUT) {
			DCC_LOG(LOG_TRACE, "timeout...");
			rsp_msg(tp, pkt, "JTAGTOOL: target_halt failed!");
			return rsp_error(tp, 1);
		}
	 } 
	
	if (state == DBG_ST_HALTED) {
		DCC_LOG(LOG_TRACE, "halted");
		return rsp_signal(tp, pkt, SIGTRAP);
	}

	switch (state) {
	case DBG_ST_ERROR:
		rsp_msg(tp, pkt, "JTAGTOOL: error state\n");
		break;
	case DBG_ST_OUTOFSYNC:
		DCC_LOG(LOG_TRACE, "out of sync");
		rsp_msg(tp, pkt, "JTAGTOOL: Out of sync\n");
		break;
	case DBG_ST_BUSY:
		DCC_LOG(LOG_TRACE, "busy...");
		rsp_msg(tp, pkt, "JTAGTOOL: busy ... \n");
		break;
	case DBG_ST_UNDEF:
		rsp_msg(tp, pkt, "JTAGTOOL: undefined state\n");
		break;
	case DBG_ST_UNCONNECTED:
		DCC_LOG(LOG_TRACE, "unconnected");
		rsp_msg(tp, pkt, "JTAGTOOL: unconnected ?\n");
		break;
	case DBG_ST_CONNECTED:
		DCC_LOG(LOG_TRACE, "connected");
		rsp_msg(tp, pkt, "JTAGTOOL: connected (busy)\n");
		break;
	case DBG_ST_RUNNING:
		DCC_LOG(LOG_TRACE, "running");
		rsp_msg(tp, pkt, "JTAGTOOL: running\n");
		break;
	default:
		DCC_LOG1(LOG_WARNING, "unknown state: %d", state);
		rsp_msg(tp, pkt, "JTAGTOOL: unknown state, bailing out!\n");
		return -1;
	}

	return rsp_error(tp, 1);
}

#if 0
static int rsp_thread_id(struct tcp_pcb * tp, int id)
{
	char pkt[32];
	int sum;
	int n;

	DCC_LOG1(LOG_TRACE, "id=%d", id);

	pkt[0] = '$';
	pkt[1] = 'Q';
	pkt[2] = 'C';
	n = 3;
	sum = 'Q' + 'C';
	if (id > 15) {
		sum += pkt[n++] = hextab[((id >> 4) & 0xf)];
	}
	sum += pkt[n++] = hextab[(id & 0xf)];
	pkt[n++] = '#';
	pkt[n++] = hextab[((sum >> 4) & 0xf)];
	pkt[n++] = hextab[sum & 0xf];

	return tcp_send(tp, pkt, n, TCP_SEND_NOWAIT);
}
#endif


static inline int toint(int c)
{
	if (isalpha(c))
		return c - (isupper(c) ? 'A' - 10 : 'a' - 10);
	return c - '0';
}

static inline int tochar(char * hex)
{
	return (toint(hex[0]) << 4) + toint(hex[1]);
}


int rsp_write(struct tcp_pcb * tp, const void * buf, int len)
{
	char pkt[(len * 2) + 8];
	char * s = (char *)buf;
	int sum;
	int n;
	char c;
	int i;

	DCC_LOG1(LOG_TRACE, "len=%d", len);

	pkt[0] = '$';
	pkt[1] = sum = 'O';
	n = 2;
	for (i = 0; i < len; i++) {
		c = hextab[((s[i] >> 4) & 0xf)];
		pkt[n++] = c;
		sum += c;
		c = hextab[s[i] & 0xf];
		pkt[n++] = c;
		sum += c;
	}
	pkt[n++] = '#';
	pkt[n++] = hextab[((sum >> 4) & 0xf)];
	pkt[n++] = hextab[sum & 0xf];

	return tcp_send(tp, pkt, n, TCP_SEND_NOWAIT);
}

int rsp_read(struct tcp_pcb * tp, const void * buf, int len)
{
	DCC_LOG1(LOG_TRACE, "len=%d", len);

	return 0;
}

const struct file_op rsp_fileop = {
	.write = (file_write_t)rsp_write,
	.read = (file_read_t)rsp_read,
	.select = (file_select_t)null_select,
	.ioctl = (file_ioctl_t)null_ioctl,
	.close = (file_close_t)null_close,
	.flush = (file_flush_t)null_flush,
	.sync = (file_sync_t)null_sync,
	.seek = (file_seek_t)null_seek,
	.mmap = (file_mmap_t)null_mmap,
	.munmap = (file_munmap_t)null_munmap,
	.readdir = (file_readdir_t)null_readdir
};

int rsp_open(struct tcp_pcb * tp)
{
	struct file * file;
	int fd;

	if ((fd = fd_alloc()) < 0) {
		DCC_LOG(LOG_WARNING, "fd_alloc() failed!");
		return fd;
	}

	DCC_LOG1(LOG_MSG, "fd=%d", fd);

	file = file_from_fd(fd);
	file->data = (void *)tp;
	file->op = (struct file_op *)&rsp_fileop;
	
	return fd;
}


int rsp_cmd(struct tcp_pcb * tp, char * pkt, int len)
{
	char * cp = pkt + 6;
	char * s = pkt;
	int c;
	int i;
	FILE * f;
	int fd;
	int ret;

	if ((fd = rsp_open(tp)) < 0) {
		return rsp_error(tp, -fd);
	}

	f = fdopen(fd, "r+");

	len -= 6;
	DCC_LOG1(LOG_INFO, "len=%d", len);

	for (i = 0; i < (len / 2); i++) {
		c = tochar(cp);
		cp += 2;
		s[i] = c;
	}
	s[i] = '\0';

	if ((ret = exec(f, s)) < 0) {
		DCC_LOG1(LOG_ERROR, "shell_exec(): %d", ret);
		close(fd);
		return rsp_error(tp, -ret);
	}

	close(fd);
	return rsp_ok(tp);
}

static int rsp_query(struct tcp_pcb * tp, char * pkt, int len)
{
	char s[128];
	int n;

	pkt[len] = '\0';

	if (strstr(pkt, "qRcmd,")) {
		DCC_LOG(LOG_TRACE, "qRcmd");
		return rsp_cmd(tp, pkt, len);
	}

	if (strstr(pkt, "qCRC:")) {
		DCC_LOG(LOG_TRACE, "qCRC");
		return rsp_empty(tp);
	}

	if (strstr(pkt, "qC")) {
		DCC_LOG(LOG_TRACE, "qC");
		return rsp_empty(tp);
	}

	if (strstr(pkt, "qAttached")) {
		DCC_LOG(LOG_TRACE, "qAttached");
		return rsp_empty(tp);
	}

	if (strstr(pkt, "qOffsets")) {
		DCC_LOG(LOG_TRACE, "qOffsets");
		return rsp_empty(tp);
	}

	if (strstr(pkt, "qSymbol")) {
		DCC_LOG(LOG_TRACE, "qSymbol");
		return rsp_empty(tp);
	}

	if (strstr(pkt, "qSupported")) {
		DCC_LOG(LOG_TRACE, "qSupported");
		n = sprintf(s, "PacketSize=%x;"
					  "qXfer:features:read-;"
					  "QStartNoAckMode+",
					  RSP_BUFFER_LEN - 1);
		return rsp_send_pkt(tp, pkt, s, n);
	}

	if (strstr(pkt, "qfThreadInfo")) {
		DCC_LOG(LOG_TRACE, "qfThreadInfo");
		/* First Thread Info */
		return rsp_thread_get_first(tp);
	}

	if (strstr(pkt, "qsThreadInfo")) {
		DCC_LOG(LOG_TRACE, "qsThreadInfo");
		/* Sequence Thread Info */
		return rsp_thread_get_next(tp);
	}

			/* Get thread info from RTOS */
			/* qThreadExtraInfo */
	if (strstr(pkt, "qXfer:memory-map:read::")) {
		DCC_LOG(LOG_TRACE, "qXfer:memory-map:read::");
		return rsp_empty(tp);
	}

	if (strstr(pkt, "qXfer:features:read:")) {
		DCC_LOG(LOG_TRACE, "qXfer:features:read:");
		return rsp_empty(tp);
	}

	if (strstr(pkt, "qTStatus")) {
		DCC_LOG(LOG_TRACE, "qTStatus");
		return rsp_empty(tp);
	}


	if (strstr(pkt, "QStartNoAckMode")) {
		DCC_LOG(LOG_TRACE, "QStartNoAckMode");
		gdb_noack_mode = 1;
		return rsp_ok(tp);
	}

	DCC_LOG(LOG_TRACE, "unsupported query");

	return rsp_empty(tp);


}

static int rsp_all_registers_get(struct tcp_pcb * tp, char * pkt, int len)
{
	unsigned int val;
	int sum = 0;
	int n;
	int r;

	DCC_LOG(LOG_TRACE, ".");

	pkt[0] = '$';
	n = 1;

	/* all integer registers */
	for (r = 0; r < 16; r++) {
		target_register_get(r, &val);

		sum += pkt[n++] = hextab[((val >> 4) & 0xf)];
		sum += pkt[n++] = hextab[(val & 0xf)];
		sum += pkt[n++] = hextab[((val >> 12) & 0xf)];
		sum += pkt[n++] = hextab[((val >> 8) & 0xf)];
		sum += pkt[n++] = hextab[((val >> 20) & 0xf)];
		sum += pkt[n++] = hextab[((val >> 16) & 0xf)];
		sum += pkt[n++] = hextab[((val >> 28) & 0xf)];
		sum += pkt[n++] = hextab[((val >> 24) & 0xf)];
	}

	/* all fp registers */
	for (r = 0; r < 8; r++) {
		sum += pkt[n++] = '0' + r;
		sum += pkt[n++] = '*';
		sum += pkt[n++] = ' ' + 20;
	}
	/* fps */
	sum += pkt[n++] = '0';
	sum += pkt[n++] = '*';
	sum += pkt[n++] = ' ' + 4;

	/* cpsr */
	target_register_get(16, &val);

	sum += pkt[n++] = hextab[((val >> 4) & 0xf)];
	sum += pkt[n++] = hextab[(val & 0xf)];
	sum += pkt[n++] = hextab[((val >> 12) & 0xf)];
	sum += pkt[n++] = hextab[((val >> 8) & 0xf)];
	sum += pkt[n++] = hextab[((val >> 20) & 0xf)];
	sum += pkt[n++] = hextab[((val >> 16) & 0xf)];
	sum += pkt[n++] = hextab[((val >> 28) & 0xf)];
	sum += pkt[n++] = hextab[((val >> 24) & 0xf)];

	pkt[n++] = '#';
	pkt[n++] = hextab[((sum >> 4) & 0xf)];
	pkt[n++] = hextab[sum & 0xf];

	return tcp_send(tp, pkt, n, TCP_SEND_NOWAIT);
}

static int rsp_all_registers_set(struct tcp_pcb * tp, char * pkt, int len)
{
	DCC_LOG(LOG_WARNING, "not implemented");

	return rsp_empty(tp);
}

static int rsp_register_get(struct tcp_pcb * tp, char * pkt, int len)
{
	unsigned int val;
	int sum = 0;
	int reg;
	int n;

	reg = strtoul(&pkt[1], NULL, 16);

	/* FIXME: the register enumaration and details 
	   must be in the ICE driver not here! */
	/* cpsr */
	if (reg > 25) {
		DCC_LOG1(LOG_WARNING, "reg=%d (unsupported)", reg);
		return rsp_empty(tp);
	}

	if (reg == 25) {
		DCC_LOG1(LOG_TRACE, "reg=%d (cpsr)", reg);
		target_register_get(16, &val);
	} else {
		if (reg > 15 ) {
			val = 0;
			DCC_LOG1(LOG_WARNING, "reg=%d (float)", reg);
		} else {
			target_register_get(reg, &val);
			DCC_LOG1(LOG_TRACE, "reg=%d", reg);
		}
	}

	pkt[0] = '$';
	n = 1;

	sum += pkt[n++] = hextab[((val >> 4) & 0xf)];
	sum += pkt[n++] = hextab[(val & 0xf)];
	sum += pkt[n++] = hextab[((val >> 12) & 0xf)];
	sum += pkt[n++] = hextab[((val >> 8) & 0xf)];
	sum += pkt[n++] = hextab[((val >> 20) & 0xf)];
	sum += pkt[n++] = hextab[((val >> 16) & 0xf)];
	sum += pkt[n++] = hextab[((val >> 28) & 0xf)];
	sum += pkt[n++] = hextab[((val >> 24) & 0xf)];

	pkt[n++] = '#';
	pkt[n++] = hextab[((sum >> 4) & 0xf)];
	pkt[n++] = hextab[sum & 0xf];
	pkt[n] = '\0';

	return tcp_send(tp, pkt, n, TCP_SEND_NOWAIT);
}

static int rsp_register_set(struct tcp_pcb * tp, char * pkt, int len)
{
	uint32_t reg;
	uint32_t val;
	char * cp;

	cp = &pkt[1];
	reg = strtoul(cp, &cp, 16);
	cp++;
	val = strtoul(cp, &cp, 16);

	/* FIXME: the register enumaration and details 
	   must be in the ICE driver not here! */
	/* cpsr */
	if (reg > 25) {
		DCC_LOG1(LOG_WARNING, "reg=%d (unsupported)", reg);
		return rsp_empty(tp);
	}

	/* cpsr */
	if (reg == 25) {
		reg = 16;
	}

	if (reg > 16) {
		DCC_LOG(LOG_TRACE, "CPSR");
		return rsp_error(tp, 2);
	}

	DCC_LOG2(LOG_TRACE, "reg=%d val=0x%08x", reg, val);

	if (target_register_set(reg, val) < 0) {
		DCC_LOG(LOG_WARNING, "target_register_set() failed!");
		return rsp_error(tp, 2);
	}

	return rsp_ok(tp);
}

static int rsp_memory_read(struct tcp_pcb * tp, char * pkt, int len)
{
	uint8_t buf[(RSP_BUFFER_LEN - 5) / 2];
	unsigned int addr;
	char * cp;
	int sum = 0;
	int size;
	int ret;
	int max;
	int n;
	int i;

	cp = &pkt[1];
	addr = strtoul(cp, &cp, 16);
	cp++;
	size = strtoul(cp, NULL, 16);

	DCC_LOG2(LOG_INFO, "addr=0x%08x size=%d", addr, size);

	max = (RSP_BUFFER_LEN - 5) >> 1;

	if (size > max)
		size = max;

	if ((ret = target_mem_read(addr, buf, size)) < 0) {
		DCC_LOG3(LOG_INFO, "ERR: %d addr=%08x size=%d", ret, addr, size);

		pkt[0] = '$';
		pkt[1] = sum = 'E';
		n = 2;
		sum += pkt[n++] = hextab[(((-ret) >> 4) & 0xf)];
		sum += pkt[n++] = hextab[((-ret) & 0xf)];
		pkt[n++] = '#';
		pkt[n++] = hextab[((sum >> 4) & 0xf)];
		pkt[n++] = hextab[sum & 0xf];
	} else {

		DCC_LOG2(LOG_INFO, "addr=%08x size=%d", addr, size);

		pkt[0] = '$';
		n = 1;
		for (i = 0; i < ret; i++) {
			sum += pkt[n++] = hextab[((buf[i] >> 4) & 0xf)];
			sum += pkt[n++] = hextab[(buf[i] & 0xf)];
		}
		pkt[n++] = '#';
		pkt[n++] = hextab[((sum >> 4) & 0xf)];
		pkt[n++] = hextab[sum & 0xf];
	}

	return tcp_send(tp, pkt, n, TCP_SEND_NOWAIT);
}

static int rsp_memory_write(struct tcp_pcb * tp, char * pkt, int len)
{
	unsigned int addr;
	char * cp;
	int size;

	cp = &pkt[1];
	addr = strtoul(cp, &cp, 16);
	cp++;
	size = strtoul(cp, &cp, 16);
	cp++;

	DCC_LOG2(LOG_WARNING, "addr=0x%08x size=%d, not implemented!", addr, size);
	return rsp_ok(tp);
}

static int rsp_breakpoint_insert(struct tcp_pcb * tp, char * pkt, int len)
{
	unsigned int addr;
	unsigned int size;
	char * cp;

	if (pkt[1] != '0')
		return rsp_ok(tp);

	cp = &pkt[3];
	addr = strtoul(cp, &cp, 16);
	cp++;
	size = strtoul(cp, NULL, 16);

	DCC_LOG2(LOG_TRACE, "addr=0x%08x size=%d", addr, size);

	target_breakpoint_set(addr, size);

	DCC_LOG(LOG_TRACE, "target_breakpoint_set() done.");

	return rsp_ok(tp);
}

static int rsp_breakpoint_remove(struct tcp_pcb * tp, char * pkt, int len)
{
	unsigned int addr;
	unsigned int size;
	char * cp;

	if (pkt[1] != '0')
		return rsp_ok(tp);

	cp = &pkt[3];
	addr = strtoul(cp, &cp, 16);
	cp++;
	size = strtoul(cp, NULL, 16);

	DCC_LOG2(LOG_TRACE, "addr=%08x size=%d", addr, size);

	target_breakpoint_clear(addr, size);

	return rsp_ok(tp);
}

static int rsp_step(struct tcp_pcb * tp, char * pkt, int len)
{
	unsigned int val;
	unsigned int addr;
	int err;
	int sum;
	int n;

	/* step */
	if (pkt[1] != '#') {
		addr = strtoul(&pkt[1], 0, 16);
		target_goto(addr, 0);
	}

	DCC_LOG(LOG_TRACE, ".");

	/* FIXME: handle errors */	
	if ((err = target_step()) < 0) {
		DCC_LOG(LOG_WARNING, "target_step() failed!");
		return rsp_error(tp, 1);
	}

	pkt[0] = '$';
	pkt[1] = sum = 'T';
	n = 2;
	/* signal */
	val = 5;
	sum += pkt[n++] = hextab[((val >> 4) & 0xf)];
	sum += pkt[n++] = hextab[(val & 0xf)];

	pkt[n++] = '#';
	pkt[n++] = hextab[((sum >> 4) & 0xf)];
	pkt[n++] = hextab[sum & 0xf];

	return tcp_send(tp, pkt, n, TCP_SEND_NOWAIT);
}

static int rsp_continue(struct tcp_pcb * tp, char * pkt, int len)
{
	unsigned int addr;

	DCC_LOG(LOG_TRACE, "...");

	if (pkt[1] != '#') {
		addr = strtoul(&pkt[1], 0, 16);
		target_goto(addr, 0);
	}

	if (target_run() < 0) {
		/* FIXME: I think that the reply for the
		   continue packet could not be an error packet */
		return rsp_error(tp, 1);
	} 

	/* signal that we are now running */
	uthread_sem_post(gdb_target_run_sem);

	return tcp_send(tp, "+", 1, TCP_SEND_NOWAIT);
}

static int rsp_h_packet(struct tcp_pcb * tp, char * pkt, int len)
{
	int state;
	int ret;

	state = target_status();

	if (state == DBG_ST_UNCONNECTED) {
		DCC_LOG(LOG_TRACE, "unconnected");
		if (target_connect(1) < 0) {
			DCC_LOG(LOG_WARNING, "target_connect() failed!");
			rsp_msg(tp, pkt, "JTAGTOOL: unconnect fail");
			return -1;
		}
		state = target_status();
	}

	if (state != DBG_ST_HALTED) {
		DCC_LOG(LOG_TRACE, "running");
		if (target_halt(0) < 0) {
			DCC_LOG(LOG_WARNING, "target_halt() failed!");
			return rsp_error(tp, 1);
		}

		if ((state = target_halt_wait(500)) == ERR_TIMEOUT) {
			DCC_LOG(LOG_TRACE, "timeout...");
			rsp_msg(tp, pkt, "JTAGTOOL: target_halt failed!");
			return rsp_error(tp, 1);
		}
	}

	/* set thread for subsequent operations */
	switch (pkt[1]) {
	case 'c':
		DCC_LOG(LOG_TRACE, "Hc");
		ret = rsp_ok(tp);
		break;
	case 'g':
		DCC_LOG1(LOG_TRACE, "Hg%c", pkt[2]);
		ret = rsp_ok(tp);
		break;
	default:
		DCC_LOG(LOG_TRACE, "H.");
		/* we don't have threads, empty replay */
		ret = rsp_empty(tp);
	}

	return 0;
}


static int rsp_v_packet(struct tcp_pcb * tp, char * pkt, int len)
{
	return rsp_empty(tp);
}

static int rsp_detach(struct tcp_pcb * tp)
{
	DCC_LOG(LOG_TRACE, "[DETACH]");
	/* detach - just reply OK */
	return rsp_ok(tp);
}

static int rsp_memory_write_bin(struct tcp_pcb * tp, char * pkt, int len)
{
	unsigned int addr;
	char * cp;
	int size;

	/* binary write */
	cp = &pkt[1];
	addr = strtoul(cp, &cp, 16);
	cp++;
	size = strtoul(cp, &cp, 16);
	cp++;

	if (target_mem_write(addr, cp, size) < 0) {
		return rsp_error(tp, 1);
	}

	DCC_LOG2(LOG_INFO, "addr=%08x size=%d", addr, size);
	return rsp_ok(tp);
}

static int rsp_pkt_recv(struct tcp_pcb * tp, char * pkt, int max)
{
	char * cp;
	int pos;
	int rem;
	int sum;
	int c;
	int n;
	int i;

	rem = max;
	sum = 0;
	pos = 0;

	for (;;) {
		if ((n = tcp_recv(tp, &pkt[pos], rem)) < 0) {
			return n;
		}

		cp = &pkt[pos];
	
		for (i = 0;  i < n; i++) {
			c = cp[i];

			if (c == '#') {
				return pos + i;
			}

			sum += c;
		}

		rem -= n;

		if (rem == 0)
			return -1;
	}
}

#ifdef ENABLE_LOG_PKT
static inline void log_pkt(char * pkt, int len)
{
	switch (len) {
	case 1:
		DCC_LOG1(LOG_TRACE, "[PKT] (1) '%c'", pkt[0]);
		break;
	case 2:
		DCC_LOG2(LOG_TRACE, "[PKT] (2) '%c%c'", pkt[0], pkt[1]);
		break;
	case 3:
		DCC_LOG3(LOG_TRACE, "[PKT] (3) '%c%c%c'", pkt[0], pkt[1], pkt[2]);
		break;
	case 4:
		DCC_LOG4(LOG_TRACE, "[PKT] (4) '%c%c%c%c'", pkt[0], pkt[1], 
				 pkt[2], pkt[3]);
		break;
	case 5:
		DCC_LOG5(LOG_TRACE, "[PKT] (5) '%c%c%c%c%c'", pkt[0], pkt[1], pkt[2], 
				 pkt[3], pkt[4]);
		break;
	case 6:
		DCC_LOG6(LOG_TRACE, "[PKT] (6) '%c%c%c%c%c%c'", pkt[0], pkt[1], 
				 pkt[2], pkt[3], pkt[4], pkt[5]);
		break;
	case 7:
		DCC_LOG7(LOG_TRACE, "[PKT] (7) '%c%c%c%c%c%c%c'", pkt[0], pkt[1], 
				 pkt[2], pkt[3], pkt[4], pkt[5], pkt[6]);
		break;
	case 8:
		DCC_LOG8(LOG_TRACE, "[PKT] (8) '%c%c%c%c%c%c%c%c'", pkt[0], pkt[1], 
				 pkt[2], pkt[3], pkt[4], pkt[5], pkt[6], pkt[7]);
		break;
	case 9:
		DCC_LOG9(LOG_TRACE, "[PKT] (9) '%c%c%c%c%c%c%c%c%c'", pkt[0], pkt[1], 
				 pkt[2], pkt[3], pkt[4], pkt[5], pkt[6], pkt[7], pkt[8]);
		break;
	default:
		DCC_LOG10(LOG_TRACE, "[PKT] (%d) '%c%c%c%c%c%c%c%c%c'", len, pkt[0], 
				  pkt[1], pkt[2], pkt[3], pkt[4], pkt[5], pkt[6], 
				  pkt[7], pkt[8]);
	}
}
#endif

int __attribute__((noreturn)) gdb_brk_task(struct tcp_pcb * tp, 
										   uthread_id_t id)
{
	char pkt[32];
	int sum;;
	int sig = 5;
	int state;

	DCC_LOG1(LOG_TRACE, "thread id=%d", id);

	for (;;) {
		do {
			/* wait for a 'target run' indication */
			DCC_LOG(LOG_TRACE, "waiting run...");
			uthread_sem_wait(gdb_target_run_sem);

			DCC_LOG(LOG_TRACE, "waiting halt...");
			while ((state = target_halt_wait(5000)) == ERR_TIMEOUT) {
				DCC_LOG(LOG_TRACE, "waiting...");
			}
		} while (state != DBG_ST_HALTED);

		DCC_LOG(LOG_TRACE, "halted");
		pkt[0] = '$';
		pkt[1] = sum = 'S';
		sum += pkt[2] = hextab[((sig >> 4) & 0xf)];
		sum += pkt[3] = hextab[(sig & 0xf)];
		pkt[4] = '#';
		pkt[5] = hextab[((sum >> 4) & 0xf)];
		pkt[6] = hextab[sum & 0xf];

		tcp_send(tp, pkt, 8, TCP_SEND_NOWAIT);
	}
}

uint32_t gdb_brk_stack[(RSP_BUFFER_LEN / 3) + 128];

int __attribute__((noreturn)) gdb_task(struct tcp_pcb * svc, uthread_id_t id)
{
	struct tcp_pcb * tp;
	char buf[RSP_BUFFER_LEN];
	char * pkt = buf;
	int len;
	int ret;
	int c;
	int brk_th;

	for (;;) {
		if ((tp = tcp_accept(svc)) == NULL) {
			DCC_LOG(LOG_ERROR, "tcp_accept().");
			break;
		}

		gdb_noack_mode = 0;

		DCC_LOG2(LOG_TRACE, "%I:%d accepted.", tp->t_faddr, ntohs(tp->t_fport));

		gdb_target_run_sem = uthread_sem_alloc(0);

		brk_th = uthread_create(gdb_brk_stack, sizeof(gdb_brk_stack), 
								(uthread_task_t)gdb_brk_task, (void *)tp, 
								0, NULL); 

		for (;;) {
			if ((len = tcp_recv(tp, buf, 1)) <= 0) {
				DCC_LOG1(LOG_WARNING, "tcp_recv(): %d", len);
				break;
			}

			c = buf[0];

			if (c == '+') {
				DCC_LOG(LOG_INFO, "[ACK]");
				continue;
			}

			if (c == '-') {
				DCC_LOG(LOG_TRACE, "[NACK]");
				continue;

			}

			if (c == 0x03) {
				DCC_LOG(LOG_TRACE, "[BREAK]");
				if (rsp_break_signal(tp, pkt) < 0) {
					DCC_LOG(LOG_WARNING, "rsp_break_signal() failed!");
					break;
				}
				continue;
			}

			if (c != '$') {
				DCC_LOG1(LOG_WARNING, "invalid: %02x", c);
				continue;
			}

			if ((len = rsp_pkt_recv(tp, pkt, RSP_BUFFER_LEN)) <= 0) {
				DCC_LOG1(LOG_WARNING, "rsp_pkt_recv(): %d", len);
				break;
			}

			if (!gdb_noack_mode)
				rsp_ack(tp);

#ifdef ENABLE_LOG_PKT
			log_pkt(pkt, len);
#endif

			switch (pkt[0]) {
			case 'H':
				ret = rsp_h_packet(tp, pkt, len);
				break;
			case 'q':
			case 'Q':
				ret = rsp_query(tp, pkt, len);
				break; 
			case 'g':
				ret = rsp_all_registers_get(tp, pkt, len);
				break;
			case 'G':
				ret = rsp_all_registers_set(tp, pkt, len);
				break;
			case 'p':
				ret = rsp_register_get(tp, pkt, len);
				break;
			case 'P':
				ret = rsp_register_set(tp, pkt, len);
				break;
			case 'm':
				ret = rsp_memory_read(tp, pkt, len);
				break;
			case 'M':
				ret = rsp_memory_write(tp, pkt, len);
				break;
			case 'z':
				/* remove breakpoint */
				ret = rsp_breakpoint_remove(tp, pkt, len);
				break;
			case 'Z':
				/* insert breakpoint */
				ret = rsp_breakpoint_insert(tp, pkt, len);
				break;
			case '?':
				ret = rsp_last_signal(tp, pkt, len);
				break;
			case 'i':
			case 's':
				ret = rsp_step(tp, pkt, len);
				break;
			case 'c':
				/* continue */
				ret = rsp_continue(tp, pkt, len);
				break;
			case 'v':
				ret = rsp_v_packet(tp, pkt, len);
				break;
			case 'D':
				ret = rsp_detach(tp);
				break;
			case 'X':
				ret = rsp_memory_write_bin(tp, pkt, len);
				break;
			case 'k':
/*				if (extended_protocol != 0)
					break; */
				/* kill */
				ret = rsp_ok(tp);
				break;
#if 0
			case '!':
				/* handle extended remote protocol */
				extended_protocol = 1;
				gdb_put_packet(connection, "OK", 2);
				break;
			case 'R':
				/* handle extended restart packet */
				breakpoint_clear_target(gdb_service->target);
				watchpoint_clear_target(gdb_service->target);
				command_run_linef(connection->cmd_ctx,
								  "ocd_gdb_restart %s",
								  target_name(target));
				break;
#endif
			default:
				DCC_LOG(LOG_WARNING, "unsupported");
				ret = rsp_empty(tp);
				break;
			}

			if (ret < 0)
				break;
		}

		DCC_LOG2(LOG_TRACE, "%I:%d close...", tp->t_faddr, ntohs(tp->t_fport));

		tcp_close(tp);

		uthread_sem_free(gdb_target_run_sem);
		uthread_cancel(brk_th);
		uthread_join(brk_th);
	}

	for (;;);
}

#ifdef DEBUG
uint32_t gdb_stack[512 + 512];
#else
uint32_t gdb_stack[512 + 256];
#endif

int gdb_rspd_start(void)
{  
	struct tcp_pcb * svc;
	int th;

	svc = tcp_alloc();

	tcp_bind(svc, INADDR_ANY, htons(1000));

	if (tcp_listen(svc, 1) != 0) {
		printf("Can't register the TCP listner!\n");
		return -1;
	}

	th = uthread_create(gdb_stack, sizeof(gdb_stack), 
						(uthread_task_t)gdb_task, (void *)svc, 0, NULL); 

	printf("<%d> ", th);

	return 0;
}

