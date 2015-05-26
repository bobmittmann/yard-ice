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
#include <stdbool.h>

#include <sys/dcclog.h>
#include <sys/stm32f.h>

#define __THINKOS_DMON__
#include <thinkos_dmon.h>
#include <thinkos.h>

#include "signals.h"

#define THREAD_ID_OFFS 64
#define THREAD_ID_ALL -1
#define THREAD_ID_ANY 0
#define THREAD_ID_NONE -2
#define THREAD_ID_IDLE (THINKOS_THREAD_IDLE + THREAD_ID_OFFS) 

int uint2dec(char * s, unsigned int val);
unsigned long hex2int(const char * __s, char ** __endp);
bool prefix(const char * __s, const char * __prefix);
int char2hex(char * pkt, int c);
int str2str(char * pkt, const char * s);
int str2hex(char * pkt, const char * s);
int bin2hex(char * pkt, const void * buf, int len);
int int2str2hex(char * pkt, unsigned int val);
int uint2hex(char * s, unsigned int val);
int hex2char(char * hex);
extern const char hextab[];

int thread_getnext(int thread_id);
int thread_active(void);
int thread_step_id(void);
int thread_break_id(void);
int thread_any(void);
bool thread_isalive(int thread_id);
int thread_register_get(int thread_id, int reg, uint32_t * val);
int thread_register_set(unsigned int thread_id, int reg, uint32_t val);
int thread_goto(unsigned int thread_id, uint32_t addr);
int thread_step_req(unsigned int thread_id);
int thread_continue(unsigned int thread_id);
int thread_info(unsigned int gdb_thread_id, char * buf);

int target_mem_write(uint32_t addr, const void * ptr, unsigned int len);
int target_mem_read(uint32_t addr, void * ptr, unsigned int len);

void target_halt(void);
int target_goto(uint32_t addr, int opt);

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
#define GDB_ENABLE_QXFER_MEMORY_MAP 0
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

struct gdb_rspd {
	uint8_t noack_mode : 1;
	uint8_t nonstop_mode : 1;
	uint8_t stopped : 1;
	uint8_t active_app : 1;
	uint8_t session_valid : 1;
	uint8_t last_signal;
	uint8_t cont_signal;
#if GDB_ENABLE_MULTIPROCESS
	uint16_t pid;
#endif
	struct {
		int8_t g; 
		int8_t c;
	} thread_id;
	struct dmon_comm * comm;
	void (* shell_task)(struct dmon_comm * comm);
};

/* -------------------------------------------------------------------------
 * Common response packets
 * ------------------------------------------------------------------------- */

static inline int rsp_ack(struct dmon_comm * comm)
{
	DCC_LOG(LOG_TRACE, "--> Ack.");
	return dmon_comm_send(comm, "+", 1);
}

#if 0
static int rsp_nack(struct dmon_comm * comm)
{
	return dmon_comm_send(comm, "-", 1);
}
#endif

static inline int rsp_ok(struct dmon_comm * comm)
{
	DCC_LOG(LOG_INFO, "--> Ok.");
	return dmon_comm_send(comm, "$OK#9a", 6);
}

static int rsp_empty(struct dmon_comm * comm)
{
	DCC_LOG(LOG_INFO, "--> Empty.");
	return dmon_comm_send(comm, "$#00", 4);
}

static int rsp_error(struct dmon_comm * comm, int err)
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

	DCC_LOG1(LOG_INFO, "--> Error(%d)!", err);

	return dmon_comm_send(comm, pkt, 7);
}

static int rsp_pkt_send(struct dmon_comm * comm, char * pkt, int len)
{
	int sum = 0;
	char c;
	int n;

	for (n = 1; n < len; ++n) {
		c = pkt[n];
		sum += c;
	}
	pkt[n++] = '#';
	pkt[n++] = hextab[((sum >> 4) & 0xf)];
	pkt[n++] = hextab[sum & 0xf];

	pkt[n] = '\0';
	DCC_LOGSTR(LOG_INFO, "--> '%s'", pkt);

	return dmon_comm_send(comm, pkt, n);
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

static int rsp_thread_isalive(struct gdb_rspd * gdb, 
							  struct dmon_comm * comm, char * pkt, int len)
{
	int ret = 0;
	int thread_id;

	thread_id = decode_thread_id(&pkt[1]);

	/* Find out if the thread thread-id is alive. 
	   'OK' thread is still alive 
	   'E NN' thread is dead */

	if (thread_isalive(thread_id)) {
		DCC_LOG1(LOG_TRACE, "thread %d is alive.", thread_id);
		ret = rsp_ok(comm);
	} else {
		DCC_LOG1(LOG_TRACE, "thread %d is dead!", thread_id);
		ret = rsp_error(comm, 1);
	}

#if 0
	/* XXX: if there is no active application run  */
	if (!gdb->active_app) {
		DCC_LOG(LOG_WARNING, "no active application, "
				"calling dmon_app_exec()!");
		if (dmon_app_exec(true) < 0) {
			return rsp_error(comm, 1);
		}
		gdb->active_app = true;
	}
#endif

	return ret;
}

static int rsp_h_packet(struct gdb_rspd * gdb, struct dmon_comm * comm, 
						char * pkt, int len)
{
	int ret = 0;
	int thread_id;

	thread_id = decode_thread_id(&pkt[2]);

#if 0	
	/* XXX: if there is no active application run  */
	if (!gdb->active_app) {
		DCC_LOG(LOG_WARNING, "no active application, "
				"calling dmon_app_exec()!");
		if (dmon_app_exec(true) < 0) {
			return rsp_error(comm, 1);
		}
		gdb->active_app = true;
	}
#endif

	/* set thread for subsequent operations */
	switch (pkt[1]) {
	case 'c':
		if (thread_id == THREAD_ID_ALL)
			DCC_LOG(LOG_TRACE, "continue all threads");
		else if (thread_id == THREAD_ID_ANY)
			DCC_LOG(LOG_TRACE, "continue any thread");
		else
			DCC_LOG1(LOG_TRACE, "continue thread %d", thread_id);
		gdb->thread_id.c = thread_id;
		ret = rsp_ok(comm);
		break;

	case 'g':
		if (thread_id == THREAD_ID_ALL)
			DCC_LOG(LOG_TRACE, "get all threads");
		else if (thread_id == THREAD_ID_ANY)
			DCC_LOG(LOG_TRACE, "get any thread");
		else
			DCC_LOG1(LOG_TRACE, "get thread %d", thread_id);
		gdb->thread_id.g = thread_id;
		ret = rsp_ok(comm);
		break;
/*
	case 'p':
		gdb->thread_id.p = thread_id;
		ret = rsp_ok(comm);
		break; */

	default:
		DCC_LOG2(LOG_WARNING, "unsupported 'H%c%d'", pkt[1], thread_id);
		ret = rsp_empty(comm);
	}

	return ret;
}


int rsp_thread_extra_info(struct dmon_comm * comm, char * pkt)
{
	char * cp = pkt + sizeof("qThreadExtraInfo,") - 1;
	int thread_id;
	int n;

	/* qThreadExtraInfo */
	thread_id = decode_thread_id(cp);
	DCC_LOG1(LOG_INFO, "thread_id=%d", thread_id);

	cp = pkt;
	*cp++ = '$';
	cp += thread_info(thread_id, cp);
	n = cp - pkt;

	return rsp_pkt_send(comm, pkt, n);
}

int rsp_thread_info_first(struct gdb_rspd * gdb,
						  struct dmon_comm * comm, char * pkt)
{
	char * cp = pkt;
	int thread_id;
	int n;

#if 0
	/* XXX: if there is no active application run  */
	if (!gdb->active_app) {
		DCC_LOG(LOG_WARNING, "no active application, "
				"calling dmon_app_exec()!");
		if (dmon_app_exec(true) < 0) {
			return rsp_error(comm, 1);
		}
		gdb->active_app = true;
	}
#endif

	/* get the first thread */
	if ((thread_id = thread_getnext(0)) < 0) {
		thread_id = THREAD_ID_IDLE;
		cp += str2str(cp, "$m");
		cp += uint2hex(cp, thread_id);
	} else {
		cp += str2str(cp, "$m");
		cp += uint2hex(cp, thread_id);
		while ((thread_id = thread_getnext(thread_id)) > 0) {
			*cp++ = ',';
			cp += uint2hex(cp, thread_id);
		}
	}
	n = cp - pkt;

	return rsp_pkt_send(comm, pkt, n);
}

int rsp_thread_info_next(struct dmon_comm * comm, char * pkt)
{
	int n;

	n = str2str(pkt, "$l");
	return rsp_pkt_send(comm, pkt, n);
}

int rsp_console_output(struct gdb_rspd * gdb, 
					   struct dmon_comm * comm, char * pkt)
{
	char * cp;
	int cnt;
	int n;

	if (!gdb->session_valid)
		return 0;

	if (gdb->stopped)
		return 0;

	cp = pkt;
	*cp++ = '$';
	*cp++ = 'O';
#if (THINKOS_ENABLE_CONSOLE)
	uint8_t * ptr;
	if ((cnt = __console_tx_pipe_ptr(&ptr)) > 0) {
		cp += bin2hex(cp, ptr, cnt);
		__console_tx_pipe_commit(cnt);
	}
#else
	cnt = 0;
#endif

	n = cp - pkt;
	rsp_pkt_send(comm, pkt, n);

	return cnt;
}

int rsp_read(struct dmon_comm * comm, const void * buf, int len)
{
	DCC_LOG1(LOG_TRACE, "len=%d", len);

	return 0;
}

int rsp_cmd(struct dmon_comm * comm, char * pkt, int len)
{
	char * cp = pkt + 6;
	char * s = pkt;
	int c;
	int i;

	len -= 6;
	DCC_LOG1(LOG_TRACE, "len=%d", len);

	for (i = 0; i < (len / 2); i++) {
		c = hex2char(cp);
		cp += 2;
		s[i] = c;
	}
	s[i] = '\0';

	DCC_LOGSTR(LOG_TRACE, "cmd=\"%s\"", s);

	return rsp_ok(comm);
}

const char target_xml[] = 
"<target>\n"
"<architecture>arm</architecture>\n"
"<feature name=\"org.gnu.gdb.arm.m-profile\">\n"
"<reg name=\"r0\" bitsize=\"32\"/>\n"
"<reg name=\"r1\" bitsize=\"32\"/>\n"
"<reg name=\"r2\" bitsize=\"32\"/>\n"
"<reg name=\"r3\" bitsize=\"32\"/>\n"
"<reg name=\"r4\" bitsize=\"32\"/>\n"
"<reg name=\"r5\" bitsize=\"32\"/>\n"
"<reg name=\"r6\" bitsize=\"32\"/>\n"
"<reg name=\"r7\" bitsize=\"32\"/>\n"
"<reg name=\"r8\" bitsize=\"32\"/>\n"
"<reg name=\"r9\" bitsize=\"32\"/>\n"
"<reg name=\"r10\" bitsize=\"32\"/>\n"
"<reg name=\"r11\" bitsize=\"32\"/>\n"
"<reg name=\"r12\" bitsize=\"32\"/>\n"
"<reg name=\"sp\" bitsize=\"32\" type=\"data_ptr\"/>\n"
"<reg name=\"lr\" bitsize=\"32\"/>\n"
"<reg name=\"pc\" bitsize=\"32\" type=\"code_ptr\"/>\n"
"<reg name=\"xpsr\" bitsize=\"32\"/>\n"
//"<reg name=\"xpsr\" bitsize=\"32\" regnum=\"25\"/>\n"
"</feature>\n"
#if 0
"<feature name=\"org.gnu.gdb.arm.vfp\">\n"
"<reg name=\"d0\" bitsize=\"64\" type=\"float\"/>\n"
"<reg name=\"d1\" bitsize=\"64\" type=\"float\"/>\n"
"<reg name=\"d2\" bitsize=\"64\" type=\"float\"/>\n"
"<reg name=\"d3\" bitsize=\"64\" type=\"float\"/>\n"
"<reg name=\"d4\" bitsize=\"64\" type=\"float\"/>\n"
"<reg name=\"d5\" bitsize=\"64\" type=\"float\"/>\n"
"<reg name=\"d6\" bitsize=\"64\" type=\"float\"/>\n"
"<reg name=\"d7\" bitsize=\"64\" type=\"float\"/>\n"
"<reg name=\"d8\" bitsize=\"64\" type=\"float\"/>\n"
"<reg name=\"d9\" bitsize=\"64\" type=\"float\"/>\n"
"<reg name=\"d10\" bitsize=\"64\" type=\"float\"/>\n"
"<reg name=\"d11\" bitsize=\"64\" type=\"float\"/>\n"
"<reg name=\"d12\" bitsize=\"64\" type=\"float\"/>\n"
"<reg name=\"d13\" bitsize=\"64\" type=\"float\"/>\n"
"<reg name=\"d14\" bitsize=\"64\" type=\"float\"/>\n"
"<reg name=\"d15\" bitsize=\"64\" type=\"float\"/>\n"
"<reg name=\"fpsid\" bitsize=\"32\" type=\"int\" group=\"float\"/>\n"
"<reg name=\"fpscr\" bitsize=\"32\" type=\"int\" group=\"float\"/>\n"
"<reg name=\"fpexc\" bitsize=\"32\" type=\"int\" group=\"float\"/>\n"
"</feature>\n"
#endif
"</target>";

const char memory_map_xml[] = 
"<memory-map>"
"<memory type=\"flash\" start=\"0x8000000\" length=\"0x100000\"/>"
"<memory type=\"flash\" start=\"0x8100000\" length=\"0x100000\"/>"
"<memory type=\"ram\" start=\"0x20000000\" length=\"0x30000\"/>"
"<memory type=\"ram\" start=\"0x10000000\" length=\"0x10000\"/>"
"</memory-map>";

/*
"<reg name=\"r0\" bitsize=\"32\" type=\"uint32\"/>
"<reg name=\"r1\" bitsize=\"32\" type=\"uint32\"/>
"<reg name=\"r2\" bitsize=\"32\" type=\"uint32\"/>
"<reg name=\"r3\" bitsize=\"32\" type=\"uint32\"/>
"<reg name=\"r4\" bitsize=\"32\" type=\"uint32\"/>
"<reg name=\"r5\" bitsize=\"32\" type=\"uint32\"/>
"<reg name=\"r6\" bitsize=\"32\" type=\"uint32\"/>
"<reg name=\"r7\" bitsize=\"32\" type=\"uint32\"/>
"<reg name=\"r8\" bitsize=\"32\" type=\"uint32\"/>
"<reg name=\"r9\" bitsize=\"32\" type=\"uint32\"/>
"<reg name=\"r10\" bitsize=\"32\" type=\"uint32\"/>
"<reg name=\"r11\" bitsize=\"32\" type=\"uint32\"/>
"<reg name=\"r12\" bitsize=\"32\" type=\"uint32\"/>
"<reg name=\"sp\" bitsize=\"32\" type=\"data_ptr\"/>
"<reg name=\"lr\" bitsize=\"32\"/>
"<reg name=\"pc\" bitsize=\"32\" type=\"code_ptr\"/>
"<reg name=\"xpsr\" bitsize=\"32\" regnum=\"25\"/>
"<feature name=\"org.gnu.gdb.arm.vfp\">"
"<reg name=\"d0\" bitsize=\"64\" type=\"float\"/>"
"<reg name=\"d1\" bitsize=\"64\" type=\"float\"/>"
"<reg name=\"d2\" bitsize=\"64\" type=\"float\"/>"
"<reg name=\"d3\" bitsize=\"64\" type=\"float\"/>"
"<reg name=\"d4\" bitsize=\"64\" type=\"float\"/>"
"<reg name=\"d5\" bitsize=\"64\" type=\"float\"/>"
"<reg name=\"d6\" bitsize=\"64\" type=\"float\"/>"
"<reg name=\"d7\" bitsize=\"64\" type=\"float\"/>"
"<reg name=\"d8\" bitsize=\"64\" type=\"float\"/>"
"<reg name=\"d9\" bitsize=\"64\" type=\"float\"/>"
"<reg name=\"d10\" bitsize=\"64\" type=\"float\"/>"
"<reg name=\"d11\" bitsize=\"64\" type=\"float\"/>"
"<reg name=\"d12\" bitsize=\"64\" type=\"float\"/>"
"<reg name=\"d13\" bitsize=\"64\" type=\"float\"/>"
"<reg name=\"d14\" bitsize=\"64\" type=\"float\"/>"
"<reg name=\"d15\" bitsize=\"64\" type=\"float\"/>"
"<reg name=\"fpsid\" bitsize=\"32\" type=\"int\" group=\"float\"/>"
"<reg name=\"fpscr\" bitsize=\"32\" type=\"int\" group=\"float\"/>"
"<reg name=\"fpexc\" bitsize=\"32\" type=\"int\" group=\"float\"/>"
"</feature>"
</target>";

*/

int rsp_features_read(struct dmon_comm * comm, char * pkt)
{
	char * cp = pkt + sizeof("qXfer:features:read:") - 1;
	char * annex;
	int offs;
	int size;
	int cnt;
	char * src;
	int n;
	int i;

	annex = cp;
	while (*cp != ':')
		cp++;
	*cp = '\0';
	/* skip ':' */
	cp++;
	
	offs = hex2int(cp, &cp);
	/* skip ',' */
	cp++;
	size = hex2int(cp, NULL);

	DCC_LOGSTR(LOG_INFO, "annex='%s'", annex);
	DCC_LOG2(LOG_INFO, "offs=%d size=%d", offs, size);

	cp = pkt;

	src = (char *)target_xml + offs;
	cnt = sizeof(target_xml) - 1 - offs;
	if (cnt > size) {
		cnt = size;
		cp += str2str(cp, "$m");
	} else
		cp += str2str(cp, "$l");

	for (i = 0; i < cnt; ++i)
		cp[i] = src[i];
	cp += cnt;

	n = cp - pkt;
	return rsp_pkt_send(comm, pkt, n);
}

int rsp_memory_map_read(struct dmon_comm * comm, char * pkt)
{
	char * cp = pkt + sizeof("qXfer:features:read:") - 1;
	char * annex;
	int offs;
	int size;
	int cnt;
	char * src;
	int n;
	int i;

	annex = cp;
	while (*cp != ':')
		cp++;
	*cp = '\0';
	/* skip ':' */
	cp++;
	
	offs = hex2int(cp, &cp);
	/* skip ',' */
	cp++;
	size = hex2int(cp, NULL);

	DCC_LOGSTR(LOG_INFO, "annex='%s'", annex);
	DCC_LOG2(LOG_INFO, "offs=%d size=%d", offs, size);

	cp = pkt;

	src = (char *)memory_map_xml + offs;
	cnt = sizeof(memory_map_xml) - 1 - offs;
	if (cnt > size) {
		cnt = size;
		cp += str2str(cp, "$m");
	} else
		cp += str2str(cp, "$l");

	for (i = 0; i < cnt; ++i)
		cp[i] = src[i];
	cp += cnt;

	n = cp - pkt;
	return rsp_pkt_send(comm, pkt, n);
}


static int rsp_query(struct gdb_rspd * gdb, struct dmon_comm * comm,
					 char * pkt, int len)
{
	int thread_id;
	char * cp;
	int n;

	if (prefix(pkt, "qRcmd,")) {
		DCC_LOG(LOG_TRACE, "qRcmd");
		return rsp_cmd(comm, pkt, len);
	}

	if (prefix(pkt, "qCRC:")) {
		DCC_LOG(LOG_TRACE, "qCRC (not implemented!)");
		return rsp_empty(comm);
	}

	if (prefix(pkt, "qC")) {
		cp = pkt + str2str(pkt, "$Q");
		thread_id = thread_active();
//		thread_id = thread_any();
//		gdb->thread_id.g = thread_id;
		cp += uint2hex(cp, thread_id);
		n = cp - pkt;
		return rsp_pkt_send(comm, pkt, n);
	}

	if (prefix(pkt, "qAttached")) {
		/* Reply:
		   '1' - The remote server attached to an existing process. 
		   '0' - The remote server created a new process. 
		 */
		/* XXX: if there is no active application */
		if (!gdb->active_app) {
			DCC_LOG(LOG_WARNING, "no active application, "
					"calling dmon_app_exec()!");
			if (dmon_app_exec(true) < 0) {
				return rsp_error(comm, 1);
			}

			gdb->active_app = true;
			n = str2str(pkt, "$0");
		} else {
			n = str2str(pkt, "$1");
		}

		/* XXX: After receiving 'qAttached' we declare the session as
		   valid */
		gdb->session_valid = true;
		return rsp_pkt_send(comm, pkt, n);
	}

	if (prefix(pkt, "qOffsets")) {
		n = str2str(pkt, "$Text=0;Data=0;Bss=0");
		return rsp_pkt_send(comm, pkt, n);
	}

	if (prefix(pkt, "qSymbol")) {
		DCC_LOG(LOG_TRACE, "qSymbol (not implemented!)");
		return rsp_empty(comm);
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
		return rsp_pkt_send(comm, pkt, n);
	}

	if (prefix(pkt, "qfThreadInfo")) {
		DCC_LOG(LOG_MSG, "qfThreadInfo");
		/* First Thread Info */
		return rsp_thread_info_first(gdb, comm, pkt);
	}

	if (prefix(pkt, "qsThreadInfo")) {
		DCC_LOG(LOG_MSG, "qsThreadInfo");
		/* Sequence Thread Info */
		return rsp_thread_info_next(comm, pkt);
	}

	/* Get thread info from RTOS */
	if (prefix(pkt, "qThreadExtraInfo")) {
		DCC_LOG(LOG_MSG, "qThreadExtraInfo");
		return rsp_thread_extra_info(comm, pkt);
	}

#if GDB_ENABLE_QXFER_FEATURES
	if (prefix(pkt, "qXfer:features:read:")) {
		DCC_LOG(LOG_TRACE, "qXfer:features:read:");
		return rsp_features_read(comm, pkt);
	}
#endif

#if GDB_ENABLE_QXFER_MEMORY_MAP
	if (prefix(pkt, "qXfer:memory-map:read:")) {
		DCC_LOG(LOG_TRACE, "qXfer:memory-map:read:");
		return rsp_memory_map_read(comm, pkt);
	}
#endif

	if (prefix(pkt, "qTStatus")) {
		DCC_LOG(LOG_TRACE, "qTStatus");
		return rsp_empty(comm);
	}

	if (prefix(pkt, "QNonStop:")) {
		gdb->nonstop_mode = pkt[9] - '0';
		DCC_LOG1(LOG_TRACE, "Nonstop=%d +++++++++++++++", gdb->nonstop_mode);
		if (!gdb->nonstop_mode && !gdb->stopped) {
			target_halt();
			gdb->stopped = true;
			gdb->last_signal = TARGET_SIGNAL_STOP;
		}
		return rsp_ok(comm);
	}

	if (prefix(pkt, "QStartNoAckMode")) {
		DCC_LOG(LOG_TRACE, "QStartNoAckMode");
		gdb->noack_mode = 1;
		return rsp_ok(comm);
	}

#if 0

	if (prefix(pkt, "QPassSignals:")) {
		int sig;
		cp = &pkt[12];
		do {
			cp++;
			sig = hex2int(cp, &cp);
			DCC_LOG1(LOG_TRACE, "sig=%d", sig);
		} while (*cp == ';');
		return rsp_ok(comm);
	}
#endif

	DCC_LOG(LOG_TRACE, "unsupported query");

	return rsp_empty(comm);


}

int rsp_get_g_thread(struct gdb_rspd * gdb)
{
	int thread_id;

	if (gdb->thread_id.g == THREAD_ID_ALL) {
		DCC_LOG(LOG_TRACE, "g thread set to ALL!!!");
		thread_id = thread_any();
	} else if (gdb->thread_id.g == THREAD_ID_ANY) {
		DCC_LOG(LOG_TRACE, "g thread set to ANY");
		thread_id = thread_any();
	} else {
		thread_id = gdb->thread_id.g;
	}

	if (thread_id < 0)
		thread_id = THREAD_ID_IDLE;

	return thread_id;
}

int rsp_get_c_thread(struct gdb_rspd * gdb)
{
	int thread_id;

	if (gdb->thread_id.c == THREAD_ID_ALL) {
		DCC_LOG(LOG_TRACE, "c thread set to ALL!!!");
		thread_id = thread_any();
	} else if (gdb->thread_id.c == THREAD_ID_ANY) {
		DCC_LOG(LOG_TRACE, "c thread set to ANY");
		thread_id = thread_any();
	} else {
		thread_id = gdb->thread_id.c;
	}

	if (thread_id < 0)
		thread_id = THREAD_ID_IDLE;

	return thread_id;
}



static int rsp_all_registers_get(struct gdb_rspd * gdb, 
								 struct dmon_comm * comm, 
								 char * pkt, int len)
{
	unsigned int val = 0;
	char * cp;
	int thread_id;
	int n;
	int r;

	thread_id = rsp_get_g_thread(gdb);

	DCC_LOG1(LOG_INFO, "thread_id=%d", thread_id);

	cp = pkt;
	*cp++ = '$';

	/* all integer registers */
	for (r = 0; r < 16; r++) {
		thread_register_get(thread_id, r, &val);
		DCC_LOG2(LOG_MSG, "R%d = 0x%08x", r, val);
		*cp++ = hextab[((val >> 4) & 0xf)];
		*cp++ = hextab[(val & 0xf)];
		*cp++ = hextab[((val >> 12) & 0xf)];
		*cp++ = hextab[((val >> 8) & 0xf)];
		*cp++ = hextab[((val >> 20) & 0xf)];
		*cp++ = hextab[((val >> 16) & 0xf)];
		*cp++ = hextab[((val >> 28) & 0xf)];
		*cp++ = hextab[((val >> 24) & 0xf)];
	}

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

	/* cpsr */
	thread_register_get(thread_id, 16, &val);

	*cp++ = hextab[((val >> 4) & 0xf)];
	*cp++ = hextab[(val & 0xf)];
	*cp++ = hextab[((val >> 12) & 0xf)];
	*cp++ = hextab[((val >> 8) & 0xf)];
	*cp++ = hextab[((val >> 20) & 0xf)];
	*cp++ = hextab[((val >> 16) & 0xf)];
	*cp++ = hextab[((val >> 28) & 0xf)];
	*cp++ = hextab[((val >> 24) & 0xf)];

	n = cp - pkt;
	return rsp_pkt_send(comm, pkt, n);
}

static int rsp_all_registers_set(struct gdb_rspd * gdb, 
								 struct dmon_comm * comm, char * pkt, int len)
{
	DCC_LOG(LOG_WARNING, "not implemented");

	return rsp_empty(comm);
}

static int rsp_register_get(struct gdb_rspd * gdb,
							struct dmon_comm * comm, 
							char * pkt, int len)
{
	unsigned int val;
	int sum = 0;
	int reg;
	int thread_id;
	int n;

	thread_id = rsp_get_g_thread(gdb);

	reg = hex2int(&pkt[1], NULL);

	/* cpsr */
	if (reg > 25) {
		DCC_LOG1(LOG_WARNING, "reg=%d (unsupported)", reg);
		return rsp_empty(comm);
	}

	if (reg == 25) {
		DCC_LOG1(LOG_TRACE, "reg=%d (cpsr)", reg);
		thread_register_get(thread_id, 16, &val);
	} else {
		if (reg > 15 ) {
			val = 0;
			DCC_LOG1(LOG_WARNING, "reg=%d (float)", reg);
		} else {
			thread_register_get(thread_id, reg, &val);
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

	return dmon_comm_send(comm, pkt, n);
}

#ifndef NTOHL
#define NTOHL(x) \
	((uint32_t)((((uint32_t)(x) & 0x000000ffU) << 24) | \
	(((uint32_t)(x) & 0x0000ff00U) <<  8) | \
	(((uint32_t)(x) & 0x00ff0000U) >>  8) | \
	(((uint32_t)(x) & 0xff000000U) >> 24)))
#endif

static int rsp_register_set(struct gdb_rspd * gdb, 
							struct dmon_comm * comm, 
							char * pkt, int len)
{
	uint32_t reg;
	uint32_t tmp;
	uint32_t val;
	int thread_id;
	char * cp;

	thread_id = rsp_get_g_thread(gdb);

	if (!thread_isalive(thread_id)) {
		DCC_LOG1(LOG_WARNING, "thread %d is dead!", thread_id);
		return rsp_ok(comm);
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
		return rsp_empty(comm);
	}

	/* cpsr */
	if (reg == 25) {
		DCC_LOG(LOG_TRACE, "xPSR");
		reg = 16;
	}

	if (reg > 16) {
		return rsp_error(comm, 2);
	}

	DCC_LOG3(LOG_TRACE, "thread_id=%d reg=%d val=0x%08x", thread_id, reg, val);

	if (thread_register_set(thread_id, reg, val) < 0) {
		DCC_LOG(LOG_WARNING, "thread_register_set() failed!");
		return rsp_error(comm, 2);
	}

	return rsp_ok(comm);
}

int rsp_memory_read(struct dmon_comm * comm, char * pkt, int len)
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
	addr = hex2int(cp, &cp);
	cp++;
	size = hex2int(cp, NULL);

	DCC_LOG2(LOG_MSG, "addr=0x%08x size=%d", addr, size);

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

		DCC_LOG2(LOG_MSG, "addr=%08x size=%d", addr, size);

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

	pkt[n] = '\0';
	DCC_LOGSTR(LOG_MSG, "pkt='%s'", pkt);
	return dmon_comm_send(comm, pkt, n);
}

static int rsp_memory_write(struct dmon_comm * comm, char * pkt, int len)
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
	return rsp_ok(comm);
}


static int rsp_breakpoint_insert(struct dmon_comm * comm, struct gdb_rspd * gdb,
								 char * pkt, int len)
{
	unsigned int addr;
	unsigned int size;
	char * cp;

	if (pkt[1] != '0')
		return rsp_empty(comm);

	cp = &pkt[3];
	addr = hex2int(cp, &cp);
	cp++;
	size = hex2int(cp, NULL);

	DCC_LOG2(LOG_TRACE, "addr=0x%08x size=%d", addr, size);

	if (dmon_breakpoint_set(addr, size))
		return rsp_ok(comm);

	return rsp_error(comm, 1);
}

static int rsp_breakpoint_remove(struct dmon_comm * comm, struct gdb_rspd * gdb,
								 char * pkt, int len)
{
	unsigned int addr;
	unsigned int size;
	char * cp;

	if (pkt[1] != '0')
		return rsp_empty(comm);

	cp = &pkt[3];
	addr = hex2int(cp, &cp);
	cp++;
	size = hex2int(cp, NULL);

	DCC_LOG2(LOG_TRACE, "addr=%08x size=%d", addr, size);

	dmon_breakpoint_clear(addr, size);

	return rsp_ok(comm);
}

static int rsp_step(struct gdb_rspd * gdb, struct dmon_comm * comm, 
					char * pkt, int len)
{
	unsigned int addr;
	int thread_id;

	thread_id = rsp_get_c_thread(gdb);

	/* step */
	if (pkt[1] != '#') {
		addr = hex2int(&pkt[1], 0);
		DCC_LOG1(LOG_TRACE, "Addr=%08x", addr);
		thread_goto(thread_id, addr);
	}

	DCC_LOG1(LOG_TRACE, "gdb_thread_id=%d.", thread_id);

	thread_step_req(thread_id);

	return 0;
}

static int rsp_stop_reply(struct gdb_rspd * gdb, 
						  struct dmon_comm * comm, char * pkt)
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
		*cp++ = 'O';
#if (THINKOS_ENABLE_CONSOLE)
		uint8_t * buf;
		if ((n = __console_tx_pipe_ptr(&buf)) > 0) {
			cp += bin2hex(cp, buf, n);
			__console_tx_pipe_commit(n);
		}
#endif
	}

	n = cp - pkt;
	return rsp_pkt_send(comm, pkt, n);
}

static int rsp_thread_stop_reply(struct gdb_rspd * gdb, 
								 struct dmon_comm * comm, 
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
	return rsp_pkt_send(comm, pkt, n);
}

static int rsp_on_step(struct gdb_rspd * gdb, struct dmon_comm * comm, 
					   char * pkt)
{
	int thread_id;

	if (gdb->stopped) {
		DCC_LOG(LOG_WARNING, "on step, suspended already!");
		return 0;
	}

	DCC_LOG(LOG_TRACE, "on step, suspending... ... ...");

	thread_id = thread_break_id();
	gdb->thread_id.g = thread_id; 
	target_halt();
	gdb->stopped = true;
	gdb->last_signal = TARGET_SIGNAL_TRAP;

	return rsp_thread_stop_reply(gdb, comm, pkt, thread_id);
}

static int rsp_on_breakpoint(struct gdb_rspd * gdb, struct dmon_comm * comm,
							 char * pkt)
{
	int thread_id;

	if (gdb->stopped) {
		DCC_LOG(LOG_WARNING, "on breakpoint, suspended already!");
		return 0;
	}

	DCC_LOG(LOG_TRACE, "on breakpoint, suspending... ... ...");

	thread_id = thread_break_id();
	gdb->thread_id.g = thread_id; 
	target_halt();
	gdb->stopped = true;
	gdb->last_signal = TARGET_SIGNAL_TRAP;

	return rsp_thread_stop_reply(gdb, comm, pkt, thread_id);
}

static int rsp_on_fault(struct gdb_rspd * gdb, struct dmon_comm * comm,
						 char * pkt)
{
	int thread_id;

	if (gdb->stopped) {
		DCC_LOG(LOG_WARNING, "on fault, suspended already!");
		return 0;
	}

	DCC_LOG(LOG_TRACE, "on fault, suspending... ... ...");

	thread_id = thread_break_id();
	gdb->thread_id.g = thread_id; 
	target_halt();
	gdb->stopped = true;
	gdb->last_signal = TARGET_SIGNAL_SEGV;

	return rsp_thread_stop_reply(gdb, comm, pkt, thread_id);
}

static int rsp_on_break(struct gdb_rspd * gdb, struct dmon_comm * comm,
						char * pkt)
{
	int thread_id;

	DCC_LOG(LOG_TRACE, "on break, suspending... ... ...");

	//gdb->thread_id.g = thread_active();

	target_halt();
	thread_id = thread_any();
	gdb->thread_id.g = thread_id;
	gdb->stopped = true;
	gdb->last_signal = TARGET_SIGNAL_INT;
	
	return rsp_thread_stop_reply(gdb, comm, pkt, thread_id);
}


static int rsp_continue(struct gdb_rspd * gdb, struct dmon_comm * comm, 
						char * pkt, int len)
{
	unsigned int addr;
	int thread_id;

	DCC_LOG(LOG_TRACE, "...");

	thread_id = rsp_get_c_thread(gdb);

	if (pkt[1] != '#') {
		addr = hex2int(&pkt[1], 0);
		thread_goto(thread_id, addr);
	}

	dmon_app_continue();
	gdb->stopped = false;

	return rsp_stop_reply(gdb, comm, pkt);
}

static int rsp_continue_sig(struct gdb_rspd * gdb, struct dmon_comm * comm, 
						char * pkt, int len)
{
	unsigned int addr;
	unsigned int sig;
	char * cp;


	sig = hex2int(&pkt[1], &cp);
	DCC_LOG1(LOG_TRACE, "sig=%d", sig);
	if (*cp == ':') {
		cp++;
		addr = hex2int(cp, &cp);
		DCC_LOG1(LOG_TRACE, "addr=%08x", addr);
		target_goto(addr, 0);
	}

	gdb->cont_signal = sig;
	dmon_app_continue();
	gdb->stopped = false;

	return rsp_stop_reply(gdb, comm, pkt);
}


static int rsp_v_packet(struct gdb_rspd * gdb, 
						struct dmon_comm * comm, char * pkt, int len)
{
#if GDB_ENABLE_VCONT
	unsigned int sig = 0;
	int thread_id = THREAD_ID_ALL;
	int n;
	char * cp;
	int action ;

	pkt[len] = '\0';
	DCC_LOG(LOG_INFO, "vCont ==================");

	if (prefix(pkt, "vCont?")) {
		DCC_LOG(LOG_MSG, "vCont?");
		n = str2str(pkt, "$vCont;c;C;s;S;t");
		return rsp_pkt_send(comm, pkt, n);
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
								"calling dmon_app_exec()!");
						if (dmon_app_exec(true) < 0) {
							return rsp_error(comm, 1);
						}
						gdb->active_app = true;
					}
					dmon_app_continue();
					gdb->stopped = false;
				} else {
					DCC_LOG1(LOG_TRACE, "Continue %d", thread_id);
					thread_continue(thread_id);
				}
				break;
			case 'C':
				DCC_LOG2(LOG_TRACE, "Continue %d sig=%d", thread_id, sig);
				if (thread_id == THREAD_ID_ALL) {
					DCC_LOG(LOG_TRACE, "Continue all!");
					dmon_app_continue();
					gdb->stopped = false;
				} else {
					DCC_LOG1(LOG_TRACE, "Continue %d", thread_id);
					thread_continue(thread_id);
				}
				gdb->last_signal = sig;
				break;
			case 's':
				DCC_LOG1(LOG_TRACE, "Step %d", thread_id);
				thread_step_req(thread_id);
				break;
			case 'S':
				DCC_LOG2(LOG_TRACE, "Step %d sig=%d", thread_id, sig);
				break;
			case 't':
				DCC_LOG1(LOG_TRACE, "Stop %d", thread_id);
				break;
			default:
				DCC_LOG(LOG_TRACE, "Unsupported!");
				return rsp_empty(comm);
			}
		}

		return rsp_stop_reply(gdb, comm, pkt);
	}

	/* signal that we are now running */
	DCC_LOG(LOG_WARNING, "v???");
	return rsp_empty(comm);
#else
	DCC_LOG(LOG_WARNING, "vCont unsupported!");
	return rsp_empty(comm);
#endif
}

static int rsp_detach(struct gdb_rspd * gdb, struct dmon_comm * comm)
{
	DCC_LOG(LOG_TRACE, "[DETACH]");

#if 0
	if (gdb->stopped)
		dmon_app_continue();
#endif

	/* reply OK */
	rsp_ok(comm);

#if 1
	dmon_exec(gdb->shell_task);
#endif
	return 0;
}

static int rsp_kill(struct gdb_rspd * gdb, struct dmon_comm * comm)
{
	DCC_LOG(LOG_TRACE, "[KILL]");

#if 0
	if (gdb->stopped)
		dmon_app_continue();
#endif

	rsp_ok(comm);

#if 0
	dmon_exec(gdb->shell_task);
#endif

	return 0;
}


static int rsp_memory_write_bin(struct gdb_rspd * gdb, 
								struct dmon_comm * comm, char * pkt, int len)
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
			dmon_soft_reset(comm);
			gdb->active_app = false;
		}
		return rsp_ok(comm);
	}
	if (gdb->active_app) {
		DCC_LOG(LOG_WARNING, "active application!");
	}

	DCC_LOG2(LOG_TRACE, "addr=%08x size=%d", addr, size);

	if (target_mem_write(addr, cp, size) < 0) {
		return rsp_error(comm, 1);
	}

	return rsp_ok(comm);
}

static int rsp_pkt_input(struct gdb_rspd * gdb, struct dmon_comm * comm, 
						 char * pkt, unsigned int len)
{
	int thread_id;
	int ret;

	switch (pkt[0]) {
	case 'H':
		ret = rsp_h_packet(gdb, comm, pkt, len);
		break;
	case 'q':
	case 'Q':
		ret = rsp_query(gdb, comm, pkt, len);
		break; 
	case 'g':
		ret = rsp_all_registers_get(gdb, comm, pkt, len);
		break;
	case 'G':
		ret = rsp_all_registers_set(gdb, comm, pkt, len);
		break;
	case 'p':
		ret = rsp_register_get(gdb, comm, pkt, len);
		break;
	case 'P':
		ret = rsp_register_set(gdb, comm, pkt, len);
		break;
	case 'm':
		ret = rsp_memory_read(comm, pkt, len);
		break;
	case 'M':
		ret = rsp_memory_write(comm, pkt, len);
		break;
	case 'T':
		ret = rsp_thread_isalive(gdb, comm, pkt, len);
		break;
	case 'z':
		/* remove breakpoint */
		ret = rsp_breakpoint_remove(comm, gdb, pkt, len);
		break;
	case 'Z':
		/* insert breakpoint */
		ret = rsp_breakpoint_insert(comm, gdb, pkt, len);
		break;
	case '?':
		thread_id = thread_any();
		gdb->thread_id.g = thread_id;
		ret = rsp_thread_stop_reply(gdb, comm, pkt, thread_id);
		break;
	case 'i':
	case 's':
		ret = rsp_step(gdb, comm, pkt, len);
		break;
	case 'c':
		/* continue */
		ret = rsp_continue(gdb, comm, pkt, len);
		break;
	case 'C':
		/* continue with signal */
		ret = rsp_continue_sig(gdb, comm, pkt, len);
		break;
	case 'v':
		ret = rsp_v_packet(gdb, comm, pkt, len);
		break;
	case 'D':
		ret = rsp_detach(gdb, comm);
		break;
	case 'X':
		ret = rsp_memory_write_bin(gdb, comm, pkt, len);
		break;
	case 'k':
		/* kill */
		ret = rsp_kill(gdb, comm);
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
		DCC_LOGSTR(LOG_TRACE, "unsupported: '%s'", pkt);
		ret = rsp_empty(comm);
		break;
	}

	return ret;
}

static int rsp_pkt_recv(struct dmon_comm * comm, char * pkt, int max)
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

	dmon_alarm(1000);

	for (;;) {
		cp = &pkt[pos];
		if ((n = dmon_comm_recv(comm, cp, rem)) < 0) {
			DCC_LOG(LOG_WARNING, "dmon_comm_recv() failed!");
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
				dmon_alarm_stop();
				/* FIXME: check the sum!!! */
				pos += j;

#ifdef DEBUG
				if (pkt[0] == 'X') 
					DCC_LOG(LOG_MSG, "<-- '$X ...'");
				else if (pkt[0] == 'm')
					DCC_LOG(LOG_MSG, "<-- '$m ...'");
				else {
					pkt[pos] = 0;
					DCC_LOGSTR(LOG_TRACE, "<-- '$%s'", pkt);
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

	dmon_alarm_stop();
	return ret;
}

struct gdb_rspd gdb_rspd;

void __attribute__((noreturn)) gdb_task(struct dmon_comm * comm)
{
	struct gdb_rspd * gdb = &gdb_rspd;
	char buf[RSP_BUFFER_LEN];
	char * pkt = buf;
	uint32_t sigmask;
	uint32_t sigset;
	int len;

	DCC_LOG(LOG_TRACE, "GDB start...");

	gdb->nonstop_mode = false;
	gdb->noack_mode = false;
	gdb->session_valid = false;
	gdb->stopped = __thinkos_suspended();
	gdb->active_app = __thinkos_active();
	gdb->last_signal = TARGET_SIGNAL_0;
	dmon_breakpoint_clear_all();

	if (gdb->shell_task == NULL)
		gdb->shell_task = gdb_task;

	dmon_comm_connect(comm);

	DCC_LOG(LOG_TRACE, "Comm connected..");

	sigmask = (1 << DMON_THREAD_FAULT);
	sigmask |= (1 << DMON_THREAD_STEP);
	sigmask |= (1 << DMON_COMM_RCV);
	sigmask |= (1 << DMON_COMM_CTL);
	sigmask |= (1 << DMON_BREAKPOINT);
	sigmask |= (1 << DMON_TX_PIPE);
	for(;;) {
		
		sigset = dmon_select(sigmask);
		if (sigset & (1 << DMON_THREAD_FAULT)) {
			DCC_LOG(LOG_TRACE, "Thread fault.");
			dmon_clear(DMON_THREAD_FAULT);
			rsp_on_fault(gdb, comm, pkt);
		}

		if (sigset & (1 << DMON_THREAD_STEP)) {
			DCC_LOG(LOG_INFO, "DMON_THREAD_STEP");
			dmon_clear(DMON_THREAD_STEP);
			rsp_on_step(gdb, comm, pkt);
		}

		if (sigset & (1 << DMON_BREAKPOINT)) {
			DCC_LOG(LOG_INFO, "DMON_BREAKPOINT");
			dmon_clear(DMON_BREAKPOINT);
			rsp_on_breakpoint(gdb, comm, pkt);
		}

		if (sigset & (1 << DMON_COMM_RCV)) {
			DCC_LOG(LOG_MSG, "Comm RX.");
			if (dmon_comm_recv(comm, buf, 1) != 1) {
				DCC_LOG(LOG_WARNING, "dmon_comm_recv() failed!");
				continue;
			}

			DCC_LOG1(LOG_MSG, "%02x", buf[0]);

			switch (buf[0]) {

			case '+':
				DCC_LOG(LOG_INFO, "[ACK]");
				break;

			case '-':
				DCC_LOG(LOG_WARNING, "[NACK]");
				dmon_exec(gdb->shell_task);
				break;

			case '$':
				if ((len = rsp_pkt_recv(comm, pkt, RSP_BUFFER_LEN)) <= 0) {
					DCC_LOG1(LOG_WARNING, "rsp_pkt_recv(): %d", len);
					dmon_exec(gdb->shell_task);
				} else {
					if (!gdb->noack_mode)
						rsp_ack(comm);
					rsp_pkt_input(gdb, comm, pkt, len);
				}
				break;

			case CTRL_C:
				DCC_LOG(LOG_TRACE, "[BREAK]");
				rsp_on_break(gdb, comm, pkt);
				break;


			default:
				DCC_LOG1(LOG_WARNING, "invalid: %02x", buf[0]);
				break;
			}

		}

		if (sigset & (1 << DMON_COMM_CTL)) {
			DCC_LOG(LOG_TRACE, "Comm Ctl.");
			dmon_clear(DMON_COMM_CTL);
			if (!dmon_comm_isconnected(comm))	
				dmon_exec(gdb->shell_task);
		}

		if (sigset & (1 << DMON_TX_PIPE)) {
			DCC_LOG(LOG_INFO, "TX Pipe.");
			if (rsp_console_output(gdb, comm, pkt) <= 0) {
				DCC_LOG(LOG_INFO, "TX Pipe empty!!!");
				dmon_clear(DMON_TX_PIPE);
			}
		}

	}

}

void gdb_init(void (* shell)(struct dmon_comm * ))
{
	struct gdb_rspd * gdb = &gdb_rspd;

	DCC_LOG(LOG_TRACE, "..... !!!!! ......");

	if (shell == NULL)
		shell = gdb_task;
	gdb->shell_task = shell;

	/* enable the FPB unit */
	CM3_FPB->ctrl = FP_KEY | FP_ENABLE;
}

