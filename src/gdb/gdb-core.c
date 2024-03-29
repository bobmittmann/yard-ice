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

#define __GDB_RSP__
#include "gdb-rsp-i.h"

#include <sys/dcclog.h>
#include <sys/file.h>
#include <sys/null.h>
#include <string.h>
#define TRACE_LEVEL TRACE_LVL_DBG
#include <trace.h>

#ifndef GDB_DEBUG_PACKET
#define GDB_DEBUG_PACKET 1
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

static const char hextab[] = { 
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

int gdb_rsp_write(struct gdb_comm * comm, const void * buf, int len)
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

	return comm->op->send(comm->arg, pkt, n);
}

int gdb_rsp_read(struct gdb_comm * comm, const void * buf, int len)
{
	DCC_LOG1(LOG_TRACE, "len=%d", len);

	return 0;
}

static const struct fileop rsp_fileop = {
	.write = (void *)gdb_rsp_write,
	.read = (void *)gdb_rsp_read,
	.close = (void *)null_close,
	.flush = (void *)null_flush,
};


struct file * gdb_rsp_fopen(struct gdb_rsp * gdb)
{
	return file_alloc((void *)&gdb->comm, &rsp_fileop);
}

int gdb_rsp_fclose(struct file * f)
{
	fclose(f);
	return file_free(f);
}

static int rsp_get_g_thread(struct gdb_rsp * gdb)
{
	int thread_id;

	if (gdb->thread_id.g == THREAD_ID_ALL) {
		DCC_LOG(LOG_TRACE, "g thread set to ALL!!!");
		thread_id = gdb->target.op->thread_any(gdb->target.arg);
	} else if (gdb->thread_id.g == THREAD_ID_ANY) {
		DCC_LOG(LOG_TRACE, "g thread set to ANY");
		thread_id = gdb->target.op->thread_any(gdb->target.arg);
	} else {
		thread_id = gdb->thread_id.g;
	}

	if (thread_id < 0)
		thread_id = THREAD_ID_IDLE;

	return thread_id;
}

static int rsp_get_c_thread(struct gdb_rsp * gdb)
{
	int thread_id;

	if (gdb->thread_id.c == THREAD_ID_ALL) {
		DCC_LOG(LOG_TRACE, "c thread set to ALL!!!");
		thread_id = gdb->target.op->thread_any(gdb->target.arg);
	} else if (gdb->thread_id.c == THREAD_ID_ANY) {
		DCC_LOG(LOG_TRACE, "c thread set to ANY");
		thread_id = gdb->target.op->thread_any(gdb->target.arg);
	} else {
		thread_id = gdb->thread_id.c;
	}

	if (thread_id < 0)
		thread_id = THREAD_ID_IDLE;

	return thread_id;
}



/* -------------------------------------------------------------------------
 * Common response packets
 * ------------------------------------------------------------------------- */

static inline int rsp_ack(struct gdb_rsp * gdb)
{
#if GDB_DEBUG_PACKET
	DCC_LOG(LOG_INFO, "--> Ack.");
#endif
	return gdb->comm.op->send(gdb->comm.arg, "+", 1);
}

#if 0
static int rsp_nack(struct dmon_comm * comm)
{
	return gdb->target.op->comm_send(gdb->target.arg, gdb, "-", 1);
}
#endif

static inline int rsp_ok(struct gdb_rsp * gdb)
{
#if GDB_DEBUG_PACKET
	DCC_LOG(LOG_INFO, "--> Ok.");
#endif
	return gdb->comm.op->send(gdb->comm.arg, "$OK#9a", 6);
}

static int rsp_empty(struct gdb_rsp * gdb)
{
#if GDB_DEBUG_PACKET
	DCC_LOG(LOG_INFO, "--> Empty.");
#endif
	return gdb->comm.op->send(gdb->comm.arg, "$#00", 4);
}

enum gdb_error_code {
	GDB_ERR_THREAD_IS_DEAD = 1,
	GDB_ERR_REGISTER_NOT_KNOWN = 2,
	GDB_ERR_REGISTER_SET_FAIL = 3,
	GDB_ERR_MEMORY_READ_FAIL = 4,
	GDB_ERR_BREAKPOINT_SET_FAIL = 5,
	GDB_ERR_WATCHPOINT_SET_FAIL = 6,
	GDB_ERR_STEP_REQUEST_FAIL = 7,
	GDB_ERR_APP_EXEC_FAIL = 8,
};

static int rsp_error(struct gdb_rsp * gdb, unsigned int err)
{
	unsigned int sum;
	char pkt[8];

	pkt[0] = '$';
	pkt[1] = sum = 'E';
	sum += pkt[2] = __hextab[((err >> 4) & 0xf)];
	sum += pkt[3] = __hextab[(err & 0xf)];
	pkt[4] = '#';
	pkt[5] = __hextab[((sum >> 4) & 0xf)];
	pkt[6] = __hextab[sum & 0xf];

#if GDB_DEBUG_PACKET
	DCC_LOG1(LOG_WARNING, "--> Error(%d)!", err);
#endif

	return gdb->comm.op->send(gdb->comm.arg, pkt, 7);
}

int gdb_rsp_error(struct gdb_rsp * gdb, unsigned int err)
{
	return rsp_error(gdb, err);
}


#if GDB_ENABLE_RXMIT
static int rsp_pkt_rxmit(struct gdb_rsp * gdb)
{
	return gdb->comm.op->send(gdb->comm.arg, gdb->tx.pkt, gdb->tx.len);
}
#endif

static int rsp_pkt_send(struct gdb_rsp * gdb, char * pkt, unsigned int len)
{
	unsigned int sum = 0;
	char c;
	int n;

	for (n = 1; n < len; ++n) {
		c = pkt[n];
		sum += c;
	}
	pkt[n++] = '#';
	pkt[n++] = __hextab[((sum >> 4) & 0xf)];
	pkt[n++] = __hextab[sum & 0xf];

	pkt[n] = '\0';

#if GDB_DEBUG_PACKET
	DCC_LOGSTR(LOG_INFO, "--> '%s'", pkt);
#endif

#if GDB_ENABLE_RXMIT
	gdb->tx.pkt = pkt;
	gdb->tx.len = n;
#endif

	return gdb->comm.op->send(gdb->comm.arg, pkt, n);
}

int gdb_rsp_monitor_write(struct gdb_rsp * gdb, const void * buf, 
						  unsigned int cnt)
{
	uint8_t * ptr = (uint8_t *)buf;
	char * pkt = gdb->pkt;
	char * cp = pkt;

	if (!gdb->session_valid)
		return 0;

	if (gdb->stopped)
		return 0;

	*cp++ = '$';
	*cp++ = 'O';
	cp += bin2hex(cp, ptr, cnt);
	rsp_pkt_send(gdb, pkt, cp - pkt);

	return cnt;
}

int gdb_rsp_puts(struct gdb_rsp * gdb, const char * msg)
{
	return gdb_rsp_monitor_write(gdb, msg, strlen(msg));
}

int decode_thread_id(char * s)
{
	char * cp = s;
	int thread_id;
#if GDB_ENABLE_MULTIPROCESS
	int pid;

	if (cp[0] == 'p') {
		cp++;
		pid = hex2int(cp, &cp);
		DCC_LOG1(LOG_TRACE, "pid=%d", pid);
		cp++;
	}
#endif

	if ((cp[0] == '-') && (cp[1] == '1'))
		thread_id = THREAD_ID_ALL;
	else
		thread_id = hex2int(cp, NULL);

	return thread_id;
}

static int rsp_thread_isalive(struct gdb_rsp * gdb, char * pkt)
{
	int ret = 0;
	int thread_id;

	thread_id = decode_thread_id(&pkt[1]);

	/* Find out if the thread thread-id is alive. 
	   'OK' thread is still alive 
	   'E NN' thread is dead */

	if (gdb->target.op->thread_isalive(gdb->target.arg, thread_id)) {
		DCC_LOG1(LOG_INFO, "thread %d is alive.", thread_id);
		ret = rsp_ok(gdb);
	} else {
		DCC_LOG1(LOG_INFO, "thread %d is dead!", thread_id);
		ret = rsp_error(gdb, GDB_ERR_THREAD_IS_DEAD);
	}

	return ret;
}

static int rsp_h_packet(struct gdb_rsp * gdb, char * pkt)
{
	int ret = 0;
	int thread_id;

	thread_id = decode_thread_id(&pkt[2]);

	/* set thread for subsequent operations */
	switch (pkt[1]) {
	case 'c':
		if (thread_id == THREAD_ID_ALL) {
			DCC_LOG(LOG_TRACE, "continue all threads");
		} else if (thread_id == THREAD_ID_ANY) {
			DCC_LOG(LOG_TRACE, "continue any thread");
		} else {
			DCC_LOG1(LOG_INFO, "continue thread %d", thread_id);
		}
		gdb->thread_id.c = thread_id;
		ret = rsp_ok(gdb);
		break;

	case 'g':
		if (thread_id == THREAD_ID_ALL) {
			DCC_LOG(LOG_TRACE, "get all threads");
		} else if (thread_id == THREAD_ID_ANY) {
			DCC_LOG(LOG_TRACE, "get any thread");
		} else {
			DCC_LOG1(LOG_INFO, "get thread %d", thread_id);
		}
		gdb->thread_id.g = thread_id;
		ret = rsp_ok(gdb);
		break;

	default:
		DCC_LOG2(LOG_WARNING, "unsupported 'H%c%d'", pkt[1], thread_id);
		ret = rsp_empty(gdb);
	}

	return ret;
}


int rsp_thread_extra_info(struct gdb_rsp * gdb, char * pkt)
{
	char * cp = pkt + sizeof("qThreadExtraInfo,") - 1;
	int thread_id;
	int n;

	/* qThreadExtraInfo */
	thread_id = decode_thread_id(cp);
	DCC_LOG1(LOG_INFO, "thread_id=%d", thread_id);

	cp = pkt;
	*cp++ = '$';
	cp += gdb->target.op->thread_info(gdb->target.arg, thread_id, cp);
	n = cp - pkt;

	return rsp_pkt_send(gdb, pkt, n);
}

int rsp_thread_info_first(struct gdb_rsp * gdb, char * pkt)
{
	char * cp = pkt;
	int thread_id;
	int n;

	/* get the first thread */
	if ((thread_id = gdb->target.op->thread_getnext(gdb->target.arg, 0)) < 0) {
		thread_id = THREAD_ID_IDLE;
		cp += str2str(cp, "$m");
		cp += uint2hex(cp, thread_id);
	} else {
		cp += str2str(cp, "$m");
		cp += uint2hex(cp, thread_id);
		while ((thread_id = gdb->target.op->thread_getnext(gdb->target.arg, thread_id)) > 0) {
			*cp++ = ',';
			cp += uint2hex(cp, thread_id);
		}
	}
	n = cp - pkt;

	return rsp_pkt_send(gdb, pkt, n);
}

int rsp_thread_info_next(struct gdb_rsp * gdb, char * pkt)
{
	int n;

	n = str2str(pkt, "$l");
	return rsp_pkt_send(gdb, pkt, n);
}

int rsp_cmd(struct gdb_rsp * gdb, char * pkt)
{
	char * cp = pkt + 6;
	char * s = pkt;
	int c;
	int i;

	for (i = 0; (*cp != '#'); ++i) {
		c = hex2char(cp);
		cp += 2;
		s[i] = c;
	}
	s[i] = '\0';

	DCC_LOGSTR(LOG_TRACE, "cmd=\"%s\"", s);
/*
	if (prefix(s, "reset") || prefix(s, "rst")) {
		if (gdb->active_app) {
			dbgmon_soft_reset();
			gdb->active_app = false;
		}
		if (gdb->target.op->app_exec(gdb->target.arg, this_board.application.start_addr, true)) {
			gdb->active_app = true;
		}
	} else if (prefix(s, "os")) {
	} else if (prefix(s, "si")) {
		print_stack_usage(gdb, pkt);
	}
*/

	return rsp_ok(gdb);
}

static void rsp_decode_read(char * annex, unsigned int * offs, 
							unsigned int * size)
{
	char * cp = annex;

	while (*cp != ':')
		cp++;
	*cp = '\0';
	cp++; /* skip ':' */
	*offs = hex2int(cp, &cp);
	cp++; /* skip ',' */
	*size = hex2int(cp, NULL);
}

int rsp_features_read(struct gdb_rsp * gdb, char * pkt)
{
	unsigned int offs;
	unsigned int size;
	char * annex;
	int cnt;

	annex = pkt + sizeof("qXfer:features:read:") - 1;
	rsp_decode_read(annex, &offs, &size);

	cnt = gdb->target.op->file_read(gdb->target.arg, annex, 
									&pkt[2], offs, size);

	pkt[0] = '$';
	pkt[1] = (cnt == size) ? 'm' : 'l';

	return rsp_pkt_send(gdb, pkt, cnt + 2);
}

int rsp_memory_map_read(struct gdb_rsp * gdb, char * pkt)
{
	unsigned int offs;
	unsigned int size;
	char * fname;
	int cnt;

	fname = pkt + sizeof("qXfer:memory-map:read:") - 1;
	rsp_decode_read(fname, &offs, &size);

	cnt = gdb->target.op->file_read(gdb->target.arg, "memmap.xml", 
									&pkt[2], offs, size);

	pkt[0] = '$';
	pkt[1] = (cnt == size) ? 'm' : 'l';

	return rsp_pkt_send(gdb, pkt, cnt + 2);
}


static int rsp_query(struct gdb_rsp * gdb, char * pkt)
{
	int thread_id;
	char * cp;
	int n;

	if (prefix(pkt, "qRcmd,")) {
		DCC_LOG(LOG_TRACE, "qRcmd");
		return rsp_cmd(gdb, pkt);
	}

	if (prefix(pkt, "qCRC:")) {
		DCC_LOG(LOG_TRACE, "qCRC (not implemented!)");
		return rsp_empty(gdb);
	}

	if (prefix(pkt, "qC")) {
		cp = pkt + str2str(pkt, "$Q");
		thread_id = gdb->target.op->thread_active(gdb->target.arg);
//		thread_id = gdb->target.op->thread_any(gdb->target.arg);
//		gdb->thread_id.g = thread_id;
		cp += uint2hex(cp, thread_id);
		n = cp - pkt;
		return rsp_pkt_send(gdb, pkt, n);
	}

	if (prefix(pkt, "qAttached")) {
		/* Reply:
		   '1' - The remote server attached to an existing process. 
		   '0' - The remote server created a new process. 
		 */
		/* XXX: if there is no active application */
		if (!gdb->active_app) {
			DCC_LOG(LOG_WARNING, "no active application, "
					"calling gdb->target.op->app_exec(gdb->target.arg, )!");
			if (!gdb->target.op->app_exec(gdb->target.arg)) {
				n = str2str(pkt, "$1");
			} else {
				gdb->active_app = true;
				n = str2str(pkt, "$0");
			}
		} else {
			n = str2str(pkt, "$1");
		}

		/* XXX: After receiving 'qAttached' we declare the session as
		   valid */
		gdb->session_valid = true;
		return rsp_pkt_send(gdb, pkt, n);
	}

	if (prefix(pkt, "qOffsets")) {
		n = str2str(pkt, "$Text=0;Data=0;Bss=0");
		return rsp_pkt_send(gdb, pkt, n);
	}

	if (prefix(pkt, "qSymbol")) {
		DCC_LOG(LOG_TRACE, "qSymbol (not implemented!)");
		return rsp_empty(gdb);
	}

	if (prefix(pkt, "qSupported")) {
		if (pkt[10] == ':') {
		} 
		DCC_LOG(LOG_TRACE, "qSupported");
		cp = pkt + str2str(pkt, "$PacketSize=");
		cp += uint2hex(cp, RSP_BUFFER_LEN - 1);
		cp += str2str(cp, 
#if GDB_ENABLE_QXFER_FEATURES
					";qXfer:features:read+"
#else
					";qXfer:features:read-"
#endif

#if GDB_ENABLE_QXFER_MEMORY_MAP
					";qXfer:memory-map:read+"
#else
					";qXfer:memory-map:read-"
#endif

#if GDB_ENABLE_MULTIPROCESS
					";multiprocess+"
#else
					";multiprocess-"
#endif

					";qRelocInsn-"
#if 0
					";QPassSignals+"
#endif

#if GDB_ENABLE_NOACK_MODE
					";QStartNoAckMode+"
#else
					";QStartNoAckMode-"
#endif

#if GDB_ENABLE_NOSTOP_MODE
					";QNonStop+"
#endif
					);
		n = cp - pkt;
		return rsp_pkt_send(gdb, pkt, n);
	}

	if (prefix(pkt, "qfThreadInfo")) {
		DCC_LOG(LOG_MSG, "qfThreadInfo");
		/* First Thread Info */
		return rsp_thread_info_first(gdb, pkt);
	}

	if (prefix(pkt, "qsThreadInfo")) {
		DCC_LOG(LOG_MSG, "qsThreadInfo");
		/* Sequence Thread Info */
		return rsp_thread_info_next(gdb, pkt);
	}

	/* Get thread info from RTOS */
	if (prefix(pkt, "qThreadExtraInfo")) {
		DCC_LOG(LOG_MSG, "qThreadExtraInfo");
		return rsp_thread_extra_info(gdb, pkt);
	}

#if GDB_ENABLE_QXFER_FEATURES
	if (prefix(pkt, "qXfer:features:read:")) {
		DCC_LOG(LOG_TRACE, "qXfer:features:read:");
		return rsp_features_read(gdb, pkt);
	}
#endif

#if GDB_ENABLE_QXFER_MEMORY_MAP
	if (prefix(pkt, "qXfer:memory-map:read:")) {
		DCC_LOG(LOG_TRACE, "qXfer:memory-map:read:");
		return rsp_memory_map_read(gdb, pkt);
	}
#endif

	if (prefix(pkt, "QNonStop:")) {
		gdb->nonstop_mode = pkt[9] - '0';
		DCC_LOG1(LOG_TRACE, "Nonstop=%d +++++++++++++++", gdb->nonstop_mode);
		if (!gdb->nonstop_mode && !gdb->stopped) {
			gdb->target.op->cpu_halt(gdb->target.arg);
			gdb->stopped = true;
			gdb->last_signal = TARGET_SIGNAL_STOP;
		}
		return rsp_ok(gdb);
	}

	if (prefix(pkt, "QStartNoAckMode")) {
		DCC_LOG(LOG_TRACE, "QStartNoAckMode");
		gdb->noack_mode = 1;
		return rsp_ok(gdb);
	}

#if 0
	if (prefix(pkt, "qTStatus")) {
		/* Ask the stub if there is a trace experiment running right now. */
		DCC_LOG(LOG_TRACE, "qTStatus");
		return rsp_empty(gdb);
	}

	if (prefix(pkt, "QPassSignals:")) {
		int sig;
		cp = &pkt[12];
		do {
			cp++;
			sig = hex2int(cp, &cp);
			DCC_LOG1(LOG_TRACE, "sig=%d", sig);
		} while (*cp == ';');
		return rsp_ok(gdb);
	}
#endif

	DCC_LOGSTR(LOG_TRACE, "unsupported: \"%s\"", pkt);

	return rsp_empty(gdb);


}

static int rsp_all_registers_get(struct gdb_rsp * gdb, char * pkt)
{
	uint32_t val = 0;
	int thread_id;
	char * cp;
	int n;
	int r;

	thread_id = rsp_get_g_thread(gdb);

	cp = pkt;
	*cp++ = '$';

	/* all integer registers */
	for (r = 0; r < 15; r++) {
		gdb->target.op->thread_register_get(gdb->target.arg, thread_id, r, &val);
		DCC_LOG2(LOG_MSG, "R%d = 0x%08x", r, val);
		cp += long2hex_be(cp, val);
	}

	gdb->target.op->thread_register_get(gdb->target.arg, thread_id, 15, &val);
	cp += long2hex_be(cp, val);
	DCC_LOG2(LOG_TRACE, "thread_id=%d PC=%08x", thread_id, val);

#if 0
	/* all fp registers */
	for (r = 0; r < 8; r++) {
		*cp++ = '0' + r;
		*cp++ = '*';
		*cp++ = ' ' + 20;
	}
	/* fps */
	*cp++ = '0';
	*cp++ = '*';
	*cp++ = ' ' + 4;
#endif

	/* xpsr */
	gdb->target.op->thread_register_get(gdb->target.arg, thread_id, 25, &val);
	cp += long2hex_be(cp, val);

	n = cp - pkt;
	return rsp_pkt_send(gdb, pkt, n);
}

static int rsp_all_registers_set(struct gdb_rsp * gdb, char * pkt)
{
	DCC_LOG(LOG_WARNING, "not implemented");

	return rsp_empty(gdb);
}

static int rsp_register_get(struct gdb_rsp * gdb, char * pkt)
{
	unsigned int val;
	int thread_id;
	char * cp;
	int reg;
	int n;

	thread_id = rsp_get_g_thread(gdb);
	reg = hex2int(&pkt[1], NULL);

	cp = pkt;
	*cp++ = '$';

	gdb->target.op->thread_register_get(gdb->target.arg, thread_id, reg, &val);
	cp += long2hex_be(cp, val);

	n = cp - pkt;
	return rsp_pkt_send(gdb, pkt, n);
}

#ifndef NTOHL
#define NTOHL(x) \
	((uint32_t)((((uint32_t)(x) & 0x000000ffU) << 24) | \
	(((uint32_t)(x) & 0x0000ff00U) <<  8) | \
	(((uint32_t)(x) & 0x00ff0000U) >>  8) | \
	(((uint32_t)(x) & 0xff000000U) >> 24)))
#endif

static int rsp_register_set(struct gdb_rsp * gdb, char * pkt)
{
	uint32_t reg;
	uint32_t tmp;
	uint32_t val;
	int thread_id;
	char * cp;

	thread_id = rsp_get_g_thread(gdb);

	if (!gdb->target.op->thread_isalive(gdb->target.arg, thread_id)) {
		DCC_LOG1(LOG_WARNING, "thread %d is dead!", thread_id);
		return rsp_ok(gdb);
	}

	cp = &pkt[1];
	reg = hex2int(cp, &cp);
	cp++;
	tmp = hex2int(cp, &cp);
	val = NTOHL(tmp);

	/* FIXME: the register enumaration and details 
	   must be in the ICE driver not here! */
	/* cpsr */
	if (reg > 25) {
		DCC_LOG1(LOG_WARNING, "reg=%d (unsupported)", reg);
		return rsp_empty(gdb);
	}

	/* cpsr */
	if (reg == 25) {
		DCC_LOG(LOG_TRACE, "xPSR");
		reg = 16;
	}

	if (reg > 16) {
		return rsp_error(gdb, GDB_ERR_REGISTER_NOT_KNOWN);
	}

	DCC_LOG3(LOG_TRACE, "thread_id=%d reg=%d val=0x%08x", thread_id, reg, val);

	if (gdb->target.op->thread_register_set(gdb->target.arg, thread_id, reg, val) < 0) {
		DCC_LOG(LOG_WARNING, "gdb->target.op->thread_register_set(arg) failed!");
		return rsp_error(gdb, GDB_ERR_REGISTER_SET_FAIL);
	}

	return rsp_ok(gdb);
}

int rsp_memory_read(struct gdb_rsp * gdb, char * pkt)
{
	uint32_t buf[((RSP_BUFFER_LEN - 8) / 2) / 4];
	unsigned int addr;
	uint8_t * data;
	char * cp;
	int size;
	int ret;
	int max;
	int n;
	int i;

	cp = &pkt[1];
	addr = hex2int(cp, &cp);
	cp++;
	size = hex2int(cp, NULL);

	DCC_LOG2(LOG_MSG, "addr=0x%08x size=%d", addr, size);

	max = (RSP_BUFFER_LEN - 8) / 2;

	if (size > max)
		size = max;

	if ((ret = gdb->target.op->mem_read(gdb->target.arg, addr, 
										buf, size)) <= 0) {
		DCC_LOG3(LOG_INFO, "%d addr=%08x size=%d", ret, addr, size);
		return rsp_error(gdb, GDB_ERR_MEMORY_READ_FAIL);
	}
	
	data = (uint8_t *)buf;
	cp = pkt;
	*cp++ = '$';

	for (i = 0; i < ret; ++i)
		cp += char2hex(cp, data[i]);
		
	n = cp - pkt;
	return rsp_pkt_send(gdb, pkt, n);
}

static int rsp_memory_write(struct gdb_rsp * gdb, char * pkt)
{
	unsigned int addr;
	char * cp;
	int size;

	cp = &pkt[1];
	addr = hex2int(cp, &cp);
	cp++;
	size = hex2int(cp, &cp);
	cp++;

	(void)addr;
	(void)size;

	DCC_LOG2(LOG_WARNING, "addr=0x%08x size=%d, not implemented!", addr, size);
	return rsp_ok(gdb);
}


static int rsp_breakpoint_insert(struct gdb_rsp * gdb, char * pkt)
{
	unsigned int addr;
	unsigned int size;
	int type;
	char * cp;

	type = pkt[1] - '0';
	cp = &pkt[3];
	addr = hex2int(cp, &cp);
	cp++;
	size = hex2int(cp, NULL);
	DCC_LOG3(LOG_TRACE, "type=%d addr=0x%08x size=%d", type, addr, size);
	if ((type == 0) || (type == 1)) {
		/* 0 - software-breakpoint */
		/* 1 - hardware-breakpoint */
		if (gdb->target.op->breakpoint_set(gdb->target.arg, addr, size))
			return rsp_ok(gdb);
		return rsp_error(gdb, GDB_ERR_BREAKPOINT_SET_FAIL);
	}
	if (type == 2) {
		/* write-watchpoint */
		if (gdb->target.op->watchpoint_set(gdb->target.arg, addr, size, 2))
			return rsp_ok(gdb);
		return rsp_error(gdb, GDB_ERR_WATCHPOINT_SET_FAIL);
	}
	if (type == 3) {
		/* read-watchpoint */
		if (gdb->target.op->watchpoint_set(gdb->target.arg, addr, size, 1))
			return rsp_ok(gdb);
		return rsp_error(gdb, GDB_ERR_WATCHPOINT_SET_FAIL);
	}
	if (type == 4) {
		/* access-watchpoint */
		if (gdb->target.op->watchpoint_set(gdb->target.arg, addr, size, 3))
			return rsp_ok(gdb);
		return rsp_error(gdb, GDB_ERR_WATCHPOINT_SET_FAIL);
	}

	DCC_LOG1(LOG_TRACE, "unsupported breakpoint type %d", type);

	return rsp_empty(gdb);
}

static int rsp_breakpoint_remove(struct gdb_rsp * gdb, char * pkt)
{
	unsigned int addr;
	unsigned int size;
	int type;
	char * cp;

	type = pkt[1] - '0';
	cp = &pkt[3];
	addr = hex2int(cp, &cp);
	cp++;
	size = hex2int(cp, NULL);
	DCC_LOG3(LOG_TRACE, "type=%d addr=0x%08x size=%d", type, addr, size);
	switch (type) {
	case 0:
	case 1:
		gdb->target.op->breakpoint_clear(gdb->target.arg, addr, size);
		break;
	case 2:
	case 3:
	case 4:
		gdb->target.op->watchpoint_clear(gdb->target.arg, addr, size);
		break;
	}

	return rsp_ok(gdb);
}

static int rsp_step(struct gdb_rsp * gdb, char * pkt)
{
	unsigned int addr;
	int thread_id;

	thread_id = rsp_get_c_thread(gdb);

	/* step */
	if (pkt[1] != '#') {
		addr = hex2int(&pkt[1], 0);
		DCC_LOG1(LOG_TRACE, "Addr=%08x", addr);
		gdb->target.op->thread_goto(gdb->target.arg, thread_id, addr);
	}

	DCC_LOG1(LOG_TRACE, "gdb_thread_id=%d.", thread_id);

	return gdb->target.op->thread_step_req(gdb->target.arg, thread_id);
}

static int rsp_stop_reply(struct gdb_rsp * gdb, char * pkt)
{
	char * cp;
	int n;

	cp = pkt;
	*cp++ = '$';

	if (gdb->stopped) {
		DCC_LOG1(LOG_TRACE, "last_signal=%d", gdb->last_signal);
		*cp++ = 'S';
		cp += char2hex(cp, gdb->last_signal);
	} else if (gdb->nonstop_mode) {
		DCC_LOG(LOG_WARNING, "nonstop mode!!!");
	} else {
#if 0
#if (THINKOS_ENABLE_CONSOLE)
		uint8_t * buf;

		if ((n = __console_tx_pipe_ptr(&buf)) > 0) {
			*cp++ = 'O';
			cp += bin2hex(cp, buf, n);
			__console_tx_pipe_commit(n);
		} else
#endif
#endif
			return 0;
	}

	n = cp - pkt;
	return rsp_pkt_send(gdb, pkt, n);
}

static int rsp_thread_stop_reply(struct gdb_rsp * gdb, 
								 char * pkt, int thread_id)
{
	char * cp;
	int n;

	cp = pkt;
	*cp++ = '$';
	*cp++ = 'T';
	cp += char2hex(cp, gdb->last_signal);
	cp += str2str(cp, "thread:");
	cp += uint2hex(cp, thread_id);
	*cp++ = ';';

	n = cp - pkt;
	return rsp_pkt_send(gdb, pkt, n);
}

int gdb_rsp_on_step(struct gdb_rsp * gdb)
{
	char * pkt = gdb->pkt;
	int thread_id;

	if (gdb->stopped) {
		DCC_LOG(LOG_WARNING, "on step, suspended already!");
		return 0;
	}

	DCC_LOG(LOG_TRACE, "on step, suspending... ... ...");

	thread_id = gdb->target.op->thread_break_id(gdb->target.arg);
	gdb->thread_id.g = thread_id; 
	gdb->target.op->cpu_halt(gdb->target.arg);
	gdb->stopped = true;
	gdb->last_signal = TARGET_SIGNAL_TRAP;

	return rsp_thread_stop_reply(gdb, pkt, thread_id);
}

int rsp_on_breakpoint(struct gdb_rsp * gdb)
{
	char * pkt = gdb->pkt;
	int thread_id;

	if (gdb->stopped) {
		DCC_LOG(LOG_WARNING, "on breakpoint, suspended already!");
		return 0;
	}

	DCC_LOG(LOG_TRACE, "on breakpoint, suspending... ... ...");

	thread_id = gdb->target.op->thread_break_id(gdb->target.arg);
	gdb->thread_id.g = thread_id; 
	gdb->target.op->cpu_halt(gdb->target.arg);
	gdb->stopped = true;
	gdb->last_signal = TARGET_SIGNAL_TRAP;

	return rsp_thread_stop_reply(gdb, pkt, thread_id);
}

int gdb_rsp_on_fault(struct gdb_rsp * gdb)
{
	char * pkt = gdb->pkt;
	int thread_id;

	if (gdb->stopped) {
		DCC_LOG(LOG_WARNING, "on fault, suspended already!");
		return 0;
	}

	thread_id = gdb->target.op->thread_break_id(gdb->target.arg);
	gdb->thread_id.g = thread_id; 

	DCC_LOG1(LOG_TRACE, "suspending (current=%d) ... ...", thread_id);

	gdb->target.op->cpu_halt(gdb->target.arg);
	gdb->stopped = true;
	gdb->last_signal = TARGET_SIGNAL_SEGV;

	return rsp_thread_stop_reply(gdb, pkt, thread_id);
}

static int rsp_on_break(struct gdb_rsp * gdb, char * pkt)
{
	int thread_id;

	DCC_LOG(LOG_TRACE, "on break, suspending... ... ...");

	//gdb->thread_id.g = gdb->target.op->thread_active(gdb->target.arg);

	gdb->target.op->cpu_halt(gdb->target.arg);
	thread_id = gdb->target.op->thread_any(gdb->target.arg);
	gdb->thread_id.g = thread_id;
	gdb->stopped = true;
	gdb->last_signal = TARGET_SIGNAL_INT;
	
	return rsp_thread_stop_reply(gdb, pkt, thread_id);
}

int gdb_rsp_on_halt(struct gdb_rsp * gdb, int signal)
{
	return 0;
}

static int rsp_continue(struct gdb_rsp * gdb, char * pkt)
{
	unsigned int addr;
	int thread_id;

	DCC_LOG(LOG_TRACE, "...");

	thread_id = rsp_get_c_thread(gdb);

	if (pkt[1] != '#') {
		addr = hex2int(&pkt[1], 0);
		gdb->target.op->thread_goto(gdb->target.arg, thread_id, addr);
	}

	gdb->target.op->cpu_continue(gdb->target.arg);
	gdb->stopped = false;

	return rsp_stop_reply(gdb, pkt);
}

static int rsp_continue_with_sig(struct gdb_rsp * gdb, char * pkt)
{
	unsigned int addr;
	unsigned int sig;
	char * cp;


	sig = hex2int(&pkt[1], &cp);
	(void)sig;
	DCC_LOG1(LOG_TRACE, "sig=%d", sig);
	if (*cp == ':') {
		cp++;
		addr = hex2int(cp, &cp);
		DCC_LOG1(LOG_TRACE, "addr=%08x", addr);
		gdb->target.op->cpu_goto(gdb->target.arg, addr, 0);
	}

	gdb->target.op->cpu_continue(gdb->target.arg);
	gdb->stopped = false;

	return rsp_stop_reply(gdb, pkt);
}


static int rsp_v_packet(struct gdb_rsp * gdb, char * pkt)
{
#if GDB_ENABLE_VCONT
	unsigned int sig = 0;
	int thread_id = THREAD_ID_ALL;
	int n;
	char * cp;
	int action ;

	if (prefix(pkt, "vCont?")) {
		DCC_LOG(LOG_MSG, "vCont?");
		n = str2str(pkt, "$vCont;c;C;s;S;t");
		return rsp_pkt_send(gdb, pkt, n);
	}

	if (prefix(pkt, "vCont;")) {
		cp = &pkt[5];

		while (*cp == ';') {
			sig = 0;
			thread_id = THREAD_ID_ALL;

			++cp;
			action = *cp++;
			if ((action == 'C') || (action == 'S')) {
				sig = hex2int(cp, &cp);
			}
			if (*cp == ':') { 
				cp++;
				thread_id = hex2int(cp, &cp);
			}

			switch (action) {
			case 'c':
				if (thread_id == THREAD_ID_ALL) {
					DCC_LOG(LOG_TRACE, "Continue all!");
					/* XXX: if there is no active application run  */
					if (!gdb->active_app) {
						DCC_LOG(LOG_WARNING, "no active application, "
								"calling gdb->target.op->app_exec(gdb->target.arg, )!");
						if (!gdb->target.op->app_exec(gdb->target.arg)) {
							return rsp_error(gdb, GDB_ERR_APP_EXEC_FAIL);
						}
						gdb->active_app = true;
					}
					gdb->target.op->cpu_continue(gdb->target.arg);
					gdb->stopped = false;
				} else {
					DCC_LOG1(LOG_TRACE, "Continue %d", thread_id);
					gdb->target.op->thread_continue(gdb->target.arg, thread_id);
				}
				break;
			case 'C':
				DCC_LOG2(LOG_TRACE, "Continue %d sig=%d", thread_id, sig);
				if (thread_id == THREAD_ID_ALL) {
					DCC_LOG(LOG_TRACE, "Continue all!");
					gdb->target.op->cpu_continue(gdb->target.arg);
					gdb->stopped = false;
				} else {
					DCC_LOG1(LOG_TRACE, "Continue %d", thread_id);
					gdb->target.op->thread_continue(gdb->target.arg, thread_id);
				}
				gdb->last_signal = sig;
				break;
			case 's':
				DCC_LOG1(LOG_TRACE, "vCont step %d", thread_id);
				if (!gdb->target.op->thread_isalive(gdb->target.arg, thread_id)) {
					DCC_LOG(LOG_WARNING, "thread is dead!");
					return rsp_error(gdb, GDB_ERR_THREAD_IS_DEAD);
				}
				if (gdb->target.op->thread_step_req(gdb->target.arg, thread_id) < 0) {
					DCC_LOG(LOG_WARNING, "gdb->target.op->thread_step_req(arg) failed!");
					return rsp_error(gdb, GDB_ERR_STEP_REQUEST_FAIL);
				}
				gdb->stopped = false;
				break;
			case 'S':
				DCC_LOG2(LOG_TRACE, "Step %d sig=%d", thread_id, sig);
				break;
			case 't':
				DCC_LOG1(LOG_TRACE, "Stop %d", thread_id);
				break;
			default:
				DCC_LOG(LOG_TRACE, "Unsupported!");
				return rsp_empty(gdb);
			}
		}

		return rsp_stop_reply(gdb, pkt);
	}

	/* signal that we are now running */
	DCC_LOG(LOG_WARNING, "v???");
	return rsp_empty(gdb);
#else
	DCC_LOG(LOG_WARNING, "vCont unsupported!");
	return rsp_empty(gdb);
#endif
}

#define GDB_RSP_QUIT -0x80000000

static int rsp_detach(struct gdb_rsp * gdb)
{
	DCC_LOG(LOG_TRACE, "[DETACH]");

#if 0
	if (gdb->stopped)
		gdb->target.op->cpu_continue(gdb->target.arg);
#endif

	/* reply OK */
	rsp_ok(gdb);

	return GDB_RSP_QUIT;
}

static int rsp_kill(struct gdb_rsp * gdb)
{
	DCC_LOG(LOG_TRACE, "[KILL]");

#if 0
	if (gdb->stopped)
		gdb->target.op->cpu_continue(gdb->target.arg);
#endif

	rsp_ok(gdb);

	return GDB_RSP_QUIT;
}

static int rsp_memory_write_bin(struct gdb_rsp * gdb, char * pkt)
{
	unsigned int addr;
	char * cp;
	int size;

	/* binary write */
	cp = &pkt[1];
	addr = hex2int(cp, &cp);
	cp++;
	size = hex2int(cp, &cp);
	cp++;

	if (size == 0) {
		DCC_LOG(LOG_TRACE, "write probe!");
		/* XXX: if there is an active application, even if it is suspended,
		   writing over it may cause errors */
		if (gdb->active_app) {
			DCC_LOG(LOG_WARNING, "active application!");
			gdb->target.op->cpu_reset(gdb->target.arg);
			gdb->active_app = false;
		}
		return rsp_ok(gdb);
	}
	if (gdb->active_app) {
		DCC_LOG(LOG_WARNING, "active application!");
	}

	DCC_LOG3(LOG_INFO, "addr=%08x size=%d cp=%08x", addr, size, cp);

	if (gdb->target.op->mem_write(gdb->target.arg, addr, cp, size) < 0) {
		/* XXX: silently ignore writing errors ...
		return rsp_error(gdb, 1);
		*/
	}

	return rsp_ok(gdb);
}


static int rsp_pkt_input(struct gdb_rsp * gdb, char * pkt, unsigned int len)
{
	int thread_id;
	int ret;

	switch (pkt[0]) {
	case 'H':
		ret = rsp_h_packet(gdb, pkt);
		break;
	case 'q':
	case 'Q':
		ret = rsp_query(gdb, pkt);
		break; 
	case 'g':
		ret = rsp_all_registers_get(gdb, pkt);
		break;
	case 'G':
		ret = rsp_all_registers_set(gdb, pkt);
		break;
	case 'p':
		ret = rsp_register_get(gdb, pkt);
		break;
	case 'P':
		ret = rsp_register_set(gdb, pkt);
		break;
	case 'm':
		ret = rsp_memory_read(gdb, pkt);
		break;
	case 'M':
		ret = rsp_memory_write(gdb, pkt);
		break;
	case 'T':
		ret = rsp_thread_isalive(gdb, pkt);
		break;
	case 'z':
		/* remove breakpoint */
		ret = rsp_breakpoint_remove(gdb, pkt);
		break;
	case 'Z':
		/* insert breakpoint */
		ret = rsp_breakpoint_insert(gdb, pkt);
		break;
	case '?':
		thread_id = gdb->target.op->thread_any(gdb->target.arg);
		gdb->thread_id.g = thread_id;
		ret = rsp_thread_stop_reply(gdb, pkt, thread_id);
		break;
	case 'i':
	case 's':
		ret = rsp_step(gdb, pkt);
		break;
	case 'c':
		/* continue */
		ret = rsp_continue(gdb, pkt);
		break;
	case 'C':
		/* continue with signal */
		ret = rsp_continue_with_sig(gdb, pkt);
		break;
	case 'v':
		ret = rsp_v_packet(gdb, pkt);
		break;
	case 'D':
		ret = rsp_detach(gdb);
		break;
	case 'X':
		ret = rsp_memory_write_bin(gdb, pkt);
		break;
	case 'k':
		/* kill */
		ret = rsp_kill(gdb);
		break;
	default:
		DCC_LOGSTR(LOG_WARNING, "unsupported: '%s'", pkt);
		ret = rsp_empty(gdb);
		break;
	}

	return ret;
}

static int rsp_pkt_recv(struct gdb_comm * comm, char * pkt, int max)
{
	enum {
		RSP_DATA = 0,
		RSP_ESC,
		RSP_HASH,
		RSP_SUM,
	};
	int state = RSP_DATA;
	int ret = -1;
	char * cp;
	int pos;
	int rem;
	int sum;
	int c;
	int n;
	int i;
	int j;

	rem = max;
	sum = 0;
	pos = 0;

	for (;;) {
		cp = &pkt[pos];
		if ((n = comm->op->recv(comm->arg, cp, rem)) < 0) {
			DCC_LOG(LOG_WARNING, "gdb->target.op->comm_recv() failed!");
			ret = n;
			break;
		}

		for (i = 0, j = 0; i < n; ++i) {
			c = cp[i];
			if (state == RSP_DATA) {
				sum += c;
				if (c == '}') {
					state = RSP_ESC;
				} else if (c == '#') {
					state = RSP_HASH;
					cp[j++] = c;
				} else {
					cp[j++] = c;
				}
			} else if (state == RSP_ESC) {
				state = RSP_DATA;
				cp[j++] = c ^ 0x20;
			} else if (state == RSP_HASH) {
				state = RSP_SUM;
				cp[j++] = c;
			} else if (state == RSP_SUM) {
				cp[j++] = c;
				/* FIXME: check the sum!!! */
				pos += j;
				pkt[pos] = '\0';
#if GDB_DEBUG_PACKET
				if (pkt[0] == 'X') 
					DCC_LOG(LOG_MSG, "<-- '$X ...'");
				else if (pkt[0] == 'm')
					DCC_LOG(LOG_MSG, "<-- '$m ...'");
				else {
					DCC_LOGSTR(LOG_INFO, "<-- '$%s'", pkt);
				}
#endif
				return pos - 3;
			}
		}
		pos += j;
		rem -= n;

		if (rem <= 0) {
			DCC_LOG(LOG_ERROR, "packet too big!");
			break;
		}
	}

	return ret;
}

struct gdb_rsp gdb_rsp_singleton;

int gdb_rsp_comm_init(struct gdb_rsp * gdb, const struct gdb_comm_op * op, 
					  void * arg)
{
	gdb->comm.op = op;
	gdb->comm.arg = arg;
	return gdb->comm.op->init(gdb->comm.arg);
}

int gdb_rsp_target_init(struct gdb_rsp * gdb, const struct gdb_target_op * op, 
					  void * arg)
{
	gdb->target.op = op;
	gdb->target.arg = arg;
	return gdb->target.op->init(gdb->target.arg);
}

void gdb_rsp_comm_loop(struct gdb_rsp * gdb)
{
	struct gdb_comm * comm = &gdb->comm;
	char pkt[RSP_BUFFER_LEN];
	char buf[4];
	int len;

	gdb->pkt = pkt;
	gdb->nonstop_mode = false;
	gdb->noack_mode = false;
	gdb->session_valid = false;
//	gdb->stopped = __thinkos_suspended();
//	gdb->active_app = __thinkos_active();
	gdb->last_signal = TARGET_SIGNAL_0;

	INF("GDB: [stopped=%s active_app=%s]", 
			 gdb->stopped ? "true" : "false",
			 gdb->active_app ? "true" : "false");

	gdb->target.op->breakpoint_clear_all(gdb->target.arg);
	gdb->target.op->watchpoint_clear_all(gdb->target.arg);

	for(;;) {
		if (gdb->comm.op->recv(gdb->comm.arg, buf, 1) != 1) {
			DBGS("GDB: comm_recv() failed!");
			break;
		}

		switch (buf[0]) {

		case '+':
			DBGS("[ACK]");
			break;

		case '-':
#if GDB_ENABLE_RXMIT
			DBGS("[NACK] rxmit!");
			rsp_pkt_rxmit(gdb);
#else
			DBGS("[NACK]!");
			//				return;
#endif
			break;

		case '$':
//			DBGS("Comm RX: '$'");

			if ((len = rsp_pkt_recv(comm, pkt, RSP_BUFFER_LEN)) <= 0) {
				DCC_LOG1(LOG_WARNING, "rsp_pkt_recv(): %d", len);
				return;
			} else {
				if (!gdb->noack_mode)
					rsp_ack(gdb);
				if (rsp_pkt_input(gdb, pkt, len) == GDB_RSP_QUIT)
					return;
			}
			break;

		case CTRL_C:
			DBGS("[BREAK]");
			rsp_on_break(gdb, pkt);
			break;


		default:
			WARN("GDB: invalid: %02x", buf[0]);
			return;
		}

	}
}

