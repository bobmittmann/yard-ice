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

#include "gdbtcp-i.h"

#define TRACE_LEVEL TRACE_LVL_DBG
#include <trace.h>

#include <sys/dcclog.h>

int gdb_brk_start(struct gdbtcpd * gdbd);

static int rsp_break_signal(struct gdbtcpd * gdb, char * pkt)
{
	struct tcp_pcb * tp = gdb->tp; 
	int state;

	INFS("GDB: break received ...");

	if ((state = target_halt(0)) < 0) {
		ERRS("GDB: target_halt()!");
		return rsp_send_error(tp, 1);
	}

	if (state != DBG_ST_HALTED) {
		INFS("GDB: target_halt_wait()");
		if ((state = target_halt_wait(50)) == ERR_TIMEOUT) {
			WARNS("GDB: timeout...");
		}
		if (state != DBG_ST_HALTED) {
			return rsp_send_msg(tp, "YARD-ICE: target_halt failed!");
		}
	}

	gdb->running = false;
	target_halt_wait_break();

	INFS("GDB: halted.");
//	return rsp_send_stop_thread(tp, TARGET_SIGNAL_TRAP, 0);
	return rsp_send_stop_core(tp, TARGET_SIGNAL_TRAP, 1);
}

#if 0
static inline void rsp_fixup_sum(char * s)
{
	int sum;

	sum = 0;
	while (*s != '#')
		sum += *s++;

	s[1] = __hextab[((sum >> 4) & 0xf)];
	s[2] = __hextab[sum & 0xf];
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

int rsp_xfer(struct gdbtcpd * gdb, char * pkt, int len)
{
	struct tcp_pcb * tp = gdb->tp;
	char * cp = pkt;

	DBG("Xfer %s", cp);

	return rsp_send_empty(tp);
}

int rsp_thread_get_first(struct gdbtcpd * gdb, char * pkt, int len)
{
	struct tcp_pcb * tp = gdb->tp;
	char * cp = pkt;
	int thread_id;
	int n;

	/* get the first thread */
	thread_id = gdb->thread_id;
	cp += str2str(cp, "$m");
	cp += uint2hex(cp, thread_id);
	n = cp - pkt;

	return rsp_send_pkt(tp, pkt, n);
}

int rsp_thread_get_next(struct gdbtcpd * gdb, char * pkt, int len)
{
	struct tcp_pcb * tp = gdb->tp;
	int sum;;

	DCC_LOG(LOG_MSG, ".");

	pkt[0] = '$';
	pkt[1] = sum = 'l';
	pkt[2] = '#';
	pkt[3] = __hextab[((sum >> 4) & 0xf)];
	pkt[4] = __hextab[sum & 0xf];

	return tcp_send(tp, pkt, 5, TCP_SEND_NOWAIT);
}

static int rsp_last_signal(struct gdbtcpd * gdb, 
						   char * pkt, int len)
{
	struct tcp_pcb * tp = gdb->tp; 
	int state;

	state = target_status();
	if (state < DBG_ST_CONNECTED) {
		WARNS("GDB: target not connected!");
		DCC_LOG(LOG_WARNING, "target not connected!");
		return rsp_send_error(tp, state);
	}

	if (state != DBG_ST_HALTED) {
		DBGS("GDB: last signal running...");

		if ((state = target_halt(0)) < 0) {
			WARNS("GDB: target_halt() fail!");
			rsp_send_msg(tp, "YARD-ICE: halt fail\n");
			return rsp_send_error(tp, 1);
		}
	
		if (state != DBG_ST_HALTED) {
			WARNS("GDB: last signal running...");
#if 0
		if ((state = target_halt_wait(50)) == ERR_TIMEOUT) {
			WARNS("GDB: target_halt_wait() fail!");
			rsp_send_msg(tp, "YARD-ICE: target_halt failed!");
			return rsp_send_error(tp, 1);
		}
#endif
		}
	 } 
	
	if (state == DBG_ST_HALTED) {
		gdb->running = false;
		target_halt_wait_break();
		DBGS("GDB: last signal halted");
	//	return rsp_send_signal(tp, TARGET_SIGNAL_TRAP);
		return rsp_send_stop_core(tp, TARGET_SIGNAL_TRAP, 1);
	}

	switch (state) {
	case DBG_ST_ERROR:
		rsp_send_msg(tp, "YARD-ICE: error state\n");
		break;
	case DBG_ST_OUTOFSYNC:
		DCC_LOG(LOG_TRACE, "out of sync");
		rsp_send_msg(tp, "YARD-ICE: Out of sync\n");
		break;
	case DBG_ST_BUSY:
		DCC_LOG(LOG_TRACE, "busy...");
		rsp_send_msg(tp, "YARD-ICE: busy ... \n");
		break;
	case DBG_ST_UNDEF:
		rsp_send_msg(tp, "YARD-ICE: undefined state\n");
		break;
	case DBG_ST_UNCONNECTED:
		DCC_LOG(LOG_TRACE, "unconnected");
		rsp_send_msg(tp, "YARD-ICE: unconnected ?\n");
		break;
	case DBG_ST_CONNECTED:
		DCC_LOG(LOG_TRACE, "connected");
		rsp_send_msg(tp, "YARD-ICE: connected (busy)\n");
		break;
	case DBG_ST_RUNNING:
		DCC_LOG(LOG_TRACE, "running");
		rsp_send_msg(tp, "YARD-ICE: running\n");
		break;
	default:
		DCC_LOG1(LOG_WARNING, "unknown state: %d", state);
		rsp_send_msg(tp, "YARD-ICE: unknown state, bailing out!\n");
		return -1;
	}

	return rsp_send_error(tp, 1);
}

int rsp_exec_cmd(struct gdbtcpd * gdb, char * pkt, int len)
{
	struct tcp_pcb * tp = gdb->tp;
	char * cp = pkt;
	char * s = pkt;
	FILE * f;
	int ret;
	int c;
	int i;

	if ((f = rsp_fopen(tp)) == NULL) {
		DCC_LOG(LOG_ERROR, "rsp_fopen() failed!");
		return rsp_send_error(tp, -1);
	}

	for (i = 0; i < (len / 2); i++) {
		c = hex2char(cp);
		cp += 2;
		s[i] = c;
	}
	s[i] = '\0';

	INF("GDB qRcmd: \"%s\"", s);

	if ((ret = shell_exec(f, yard_ice_cmd_tab, s)) < 0) {
		DCC_LOG1(LOG_ERROR, "shell_exec(): %d", ret);
		rsp_fclose(f);
		return rsp_send_error(tp, -ret);
	}

	rsp_fclose(f);
	return rsp_send_ok(tp);
}

int rsp_extended_mode(struct gdbtcpd * gdb, char * pkt, int len)
{
	struct tcp_pcb * tp = gdb->tp;
	return rsp_send_ok(tp);
}

#define OPT_MULTIPROCESS   0
#define OPT_SWBREAK        1
#define OPT_HWBREAK        2
#define OPT_QRELOCINSN     3
#define OPT_FORK_EVENTS    4
#define OPT_VFORK_EVENTS   5
#define OPT_EXEC_EVENTS    6 
#define OPT_VCONTSUPPORTED 7
#define OPT_QTHREADEVENTS  8
#define OPT_QTHREADOPTIONS 9
#define OPT_NO_RESUMED     10
#define OPT_MEMORY_TAGGING 11
#define OPT_ERROR_MESSAGE  12
#define OPT_LIST_LEN       13

const struct {
	const char opt[15];
	int8_t flag;
} opt_supported[OPT_LIST_LEN] = {
	{ .opt = "multiprocess",   .flag = OPT_MULTIPROCESS },
	{ .opt = "swbreak",        .flag = OPT_SWBREAK },
	{ .opt = "hwbreak",        .flag = OPT_HWBREAK },
	{ .opt = "qRelocInsn",     .flag = OPT_QRELOCINSN },
	{ .opt = "fork-events",    .flag = OPT_FORK_EVENTS },
	{ .opt = "vfork-events",   .flag = OPT_VFORK_EVENTS },
	{ .opt = "exec-events",    .flag = OPT_EXEC_EVENTS },
	{ .opt = "vContSupported", .flag = OPT_VCONTSUPPORTED },
	{ .opt = "QThreadEvents",  .flag = OPT_QTHREADEVENTS },
	{ .opt = "QThreadOptions", .flag = OPT_QTHREADOPTIONS },
	{ .opt = "no-resumed",     .flag = OPT_NO_RESUMED },
	{ .opt = "memory-tagging", .flag = OPT_MEMORY_TAGGING },
	{ .opt = "error-message",  .flag = OPT_ERROR_MESSAGE },
};

static int rsp_qsupported(struct gdbtcpd * gdb, char * pkt, int len)
{
	struct tcp_pcb * tp = gdb->tp;
	uint32_t pflags = 0;
	uint32_t nflags = 0;
	char * cp = pkt;
	int n;
	int i;

	INFS("RSP: Supported:");

	for (;;) {
		for (i = 0; i < OPT_LIST_LEN; ++i) {       
			if ((cp = prefix(pkt, opt_supported[i].opt))) {
				break;
			}
		}
		if (cp == NULL) {
			break;
		}
		if (*cp == '+') {
			DBG("option: %s+", opt_supported[i].opt);
			pflags |= (1 << opt_supported[i].flag);
		} else if (*cp == '-') {
			DBG("option: %s-", opt_supported[i].opt);
			nflags |= (1 << opt_supported[i].flag);
		}
		pkt = cp += 2;
	}

	for (i = 0; i < OPT_LIST_LEN; ++i) {       
		if (pflags & (1 << i)) {
			DBG("req: %s", opt_supported[i].opt);
		}
	}

	cp = pkt + str2str(pkt, "$PacketSize=");
	cp += uint2hex(cp, RSP_BUFFER_LEN - 1);
	cp += str2str(cp, 
#if GDB_ENABLE_QXFER_FEATURES
				  ";qXfer:features:read+"
#else
				  ";qXfer:features:read-"
#endif

#if (GDB_ENABLE_QXFER_MEMORY_MAP) 
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

#if (GDB_ENABLE_NOACK_MODE)
				  ";QStartNoAckMode+"
#else
				  ";QStartNoAckMode-"
#endif

#if GDB_ENABLE_NOSTOP_MODE
				  ";QNonStop+"
#endif
#if GDB_ENABLE_QXFER_THREADS
				  ";qXfer:threads:read+"
#endif
#if GDB_ENABLE_VFLASH
#endif
#if GDB_ENABLE_VCONT
				  ";vContSupported+"
#else
				  ";vContSupported-"
#endif
				  );
	n = cp - pkt;
	return rsp_send_pkt(tp, pkt, n);
}

int rsp_unsupported(struct gdbtcpd * gdb, char * pkt, int len)
{
	struct tcp_pcb * tp = gdb->tp;
	return rsp_send_empty(tp);
}

int rsp_qcontinue(struct gdbtcpd * gdb, char * pkt, int len)
{
	struct tcp_pcb * tp = gdb->tp;
	int thread_id = gdb->thread_id;
	char * cp;
	int n;
	cp = pkt + str2str(pkt, "$Q");
	cp += uint2hex(cp, thread_id);
	n = cp - pkt;
	return rsp_send_pkt(tp, pkt, n);
}

int rsp_qstartnoackmode(struct gdbtcpd * gdb, char * pkt, int len)
{
	struct tcp_pcb * tp = gdb->tp;
#if (GDB_ENABLE_NOACK_MODE)
	gdb->noack_mode = true;
	return rsp_send_ok(tp);
#else
	return rsp_send_empty(tp);
#endif
}

const struct rsp_pkt_decoder q_decoder_lst[] = {
	{ .s = "QP",                    .decoder = rsp_unsupported },
	{ .s = "QStartNoAckMode",       .decoder = rsp_qstartnoackmode },
	{ .s = "qAttached",             .decoder = rsp_unsupported },
	{ .s = "qC",                    .decoder = rsp_qcontinue },
	{ .s = "qCRC",                  .decoder = rsp_unsupported },
	{ .s = "qOffsets",              .decoder = rsp_unsupported },
	{ .s = "qRcmd",                 .decoder = rsp_exec_cmd },
	{ .s = "qSupported",            .decoder = rsp_qsupported },
	{ .s = "qSymbol",               .decoder = rsp_unsupported },
	{ .s = "qTStatus",              .decoder = rsp_unsupported },
	{ .s = "qThreadExtraInfo",      .decoder = rsp_unsupported },
	{ .s = "qXfer",                 .decoder = rsp_xfer },
	{ .s = "qfThreadInfo",          .decoder = rsp_unsupported },
	{ .s = "qsThreadInfo",          .decoder = rsp_unsupported },
};

static int rsp_query(struct gdbtcpd * gdb, char * pkt, int len)
{
	struct tcp_pcb * tp = gdb->tp;
	char * cp;
	int i;

	if ((cp = strchr(pkt, ':')) == NULL) {
		/* If the ':' fails we look for a ',' to handle qExtraInfo and Rcmd */
		if ((cp = strchr(pkt, ',')) == NULL) {
			cp = pkt;
		} else
			*cp++ = '\0';
	} else {
		*cp++ = '\0';
	}

	i = rsp_query_lookup(q_decoder_lst, LST_LEN(q_decoder_lst), pkt);
	if (i < 0) {
		INF("unsupported: %s", pkt);
		return rsp_send_empty(tp);
	}

	DBG("query: %s", q_decoder_lst[i].s);
	len -= cp - pkt;
	return q_decoder_lst[i].decoder(gdb, cp, len);
}

static int rsp_all_registers_get(struct gdbtcpd * gdb, char * pkt, int len)
{
	struct tcp_pcb * tp = gdb->tp;
	uint32_t val = 0;
	char * cp;
	int n;
	int r;

	DCC_LOG(LOG_MSG, ".");

	cp = pkt;
	*cp++ = '$';

	/* all integer registers */
	for (r = 0; r < 16; r++) {
		target_register_get(r, &val);
		DCC_LOG2(LOG_MSG, "R%d = 0x%08x", r, val);
		cp += long2hex_be(cp, val);
	}

	/* xpsr */
	target_register_get(16, &val);
	cp += long2hex_be(cp, val);

#if THINKOS_ENABLE_FPU
	for (r = 26; r < 42; r++) {
		if (thread_register_get(thread_id, r, &val) < 0)
			break;
		cp += longlong2hex_be(cp, val);
	}
	/* fpscr */
	if (thread_register_get(thread_id, 42, &val) >= 0) {
		cp += long2hex_be(cp, val);
	}
#endif

	n = cp - pkt;
	return rsp_send_pkt(tp, pkt, n);
}

static int rsp_all_registers_set(struct gdbtcpd * gdb, char * pkt, int len)
{
	struct tcp_pcb * tp = gdb->tp;
	DCC_LOG(LOG_WARNING, "not implemented");

	return rsp_send_empty(tp);
}

static int rsp_register_get(struct gdbtcpd * gdb, char * pkt, int len)
{
	struct tcp_pcb * tp = gdb->tp;
	unsigned int val;
	int reg;
	int n;

	reg = strtoul(&pkt[1], NULL, 16);

	/* FIXME: the register enumaration and details 
	   must be in the ICE driver not here! */
	/* cpsr */
	if (reg > 25) {
		DCC_LOG1(LOG_WARNING, "reg=%d (unsupported)", reg);
		return rsp_send_empty(tp);
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
	n += long2hex_be(&pkt[1], val);

	return rsp_send_pkt(tp, pkt, n);
}

static int rsp_register_set(struct gdbtcpd * gdb, char * pkt, int len)
{
	struct tcp_pcb * tp = gdb->tp;
	uint32_t reg;
	uint32_t val;
	char * cp;

	cp = &pkt[1];
	reg = hex2int(cp, &cp);
	cp++;
	val = hex2uint32_be(cp, &cp);

	/* FIXME: the register enumaration and details 
	   must be in the ICE driver not here! */
	/* cpsr */
	if (reg > 25) {
		DCC_LOG1(LOG_WARNING, "reg=%d (unsupported)", reg);
		return rsp_send_empty(tp);
	}

	/* cpsr */
	if (reg == 25) {
		reg = 16;
	}

	if (reg > 16) {
		DCC_LOG(LOG_TRACE, "CPSR");
		return rsp_send_error(tp, 2);
	}

	DCC_LOG2(LOG_TRACE, "reg=%d val=0x%08x", reg, val);

	if (target_register_set(reg, val) < 0) {
		DCC_LOG(LOG_WARNING, "target_register_set() failed!");
		return rsp_send_error(tp, 2);
	}

	return rsp_send_ok(tp);
}

static int rsp_memory_read(struct gdbtcpd * gdb, char * pkt, int len)
{
	struct tcp_pcb * tp = gdb->tp;
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

	if ((ret = target_mem_read(addr, buf, size)) <= 0) {
		DCC_LOG3(LOG_WARNING, "ERR: %d addr=%08x size=%d", ret, addr, size);
#if 1
		pkt[0] = '$';
		pkt[1] = sum = 'E';
		n = 2;
		sum += pkt[n++] = __hextab[(((-ret) >> 4) & 0xf)];
		sum += pkt[n++] = __hextab[((-ret) & 0xf)];
		pkt[n++] = '#';
		pkt[n++] = __hextab[((sum >> 4) & 0xf)];
		pkt[n++] = __hextab[sum & 0xf];
#endif
#if 0
		pkt[0] = '$';
		n = 1;
		for (i = 0; i < size; i++) {
			sum += pkt[n++] = __hextab[0];
			sum += pkt[n++] = __hextab[0];
		}
		pkt[n++] = '#';
		pkt[n++] = __hextab[((sum >> 4) & 0xf)];
		pkt[n++] = __hextab[sum & 0xf];
#endif
	} else {
		DCC_LOG2(LOG_INFO, "addr=%08x size=%d", addr, size);

		pkt[0] = '$';
		n = 1;
		for (i = 0; i < ret; i++) {
			sum += pkt[n++] = __hextab[((buf[i] >> 4) & 0xf)];
			sum += pkt[n++] = __hextab[(buf[i] & 0xf)];
		}
		pkt[n++] = '#';
		pkt[n++] = __hextab[((sum >> 4) & 0xf)];
		pkt[n++] = __hextab[sum & 0xf];
	}

	return tcp_send(tp, pkt, n, TCP_SEND_NOWAIT);
}

static int rsp_memory_write(struct gdbtcpd * gdb, char * pkt, int len)
{
	struct tcp_pcb * tp = gdb->tp;
	unsigned int addr;
	char * cp;
	int size;

	cp = &pkt[1];
	addr = strtoul(cp, &cp, 16);
	cp++;
	size = strtoul(cp, &cp, 16);
	cp++;

	(void)addr;
	(void)size;

	DCC_LOG2(LOG_WARNING, "addr=0x%08x size=%d, not implemented!", addr, size);
	return rsp_send_ok(tp);
}

static int rsp_breakpoint_insert(struct gdbtcpd * gdb, char * pkt, int len)
{
	struct tcp_pcb * tp = gdb->tp;
	unsigned int addr;
	unsigned int size;
	char * cp;

	if (pkt[1] != '0')
		return rsp_send_ok(tp);

	cp = &pkt[3];
	addr = strtoul(cp, &cp, 16);
	cp++;
	size = strtoul(cp, NULL, 16);

	DCC_LOG2(LOG_MSG, "addr=0x%08x size=%d", addr, size);

	target_breakpoint_set(addr, size);

	DCC_LOG(LOG_MSG, "target_breakpoint_set() done.");

	return rsp_send_ok(tp);
}

static int rsp_breakpoint_remove(struct gdbtcpd * gdb, char * pkt, int len)
{
	struct tcp_pcb * tp = gdb->tp;
	unsigned int addr;
	unsigned int size;
	char * cp;

	if (pkt[1] != '0')
		return rsp_send_ok(tp);

	cp = &pkt[3];
	addr = strtoul(cp, &cp, 16);
	cp++;
	size = strtoul(cp, NULL, 16);

	DCC_LOG2(LOG_MSG, "addr=%08x size=%d", addr, size);

	target_breakpoint_clear(addr, size);

	return rsp_send_ok(tp);
}

static int rsp_step(struct gdbtcpd * gdb, char * pkt, int len)
{
	struct tcp_pcb * tp = gdb->tp;
	int err;

	if (pkt[1] != '\0') {
		uint32_t addr;
		addr = hex2int(&pkt[1], 0);
		err = target_goto(addr, 0);
		WARN("GDB: step(addr=%08x)", addr);
	} else  {
		err = target_step();
		INFS("GDB: step");
	} 
	if (err < 0) {
		return rsp_send_error(tp, 1);
	}

	gdb->running = false;
	target_halt_wait_break();

	return rsp_send_stop_core(tp, TARGET_SIGNAL_TRAP, 1);
//	return rsp_send_stop_thread(tp, TARGET_SIGNAL_TRAP, 0);
//	return rsp_send_signal(tp, TARGET_SIGNAL_TRAP);
}

static int rsp_continue(struct gdbtcpd * gdb, char * pkt, int len)
{
	struct tcp_pcb * tp = gdb->tp;
	unsigned int addr;

	DCC_LOG(LOG_MSG, "...");

	if (pkt[1] != '\0') {
		addr = strtoul(&pkt[1], 0, 16);
		DBG("RSP: <-- c(%d)", addr);
		target_goto(addr, 0);
	}

	if (target_run() < 0) {
		WARNS("GDB: target_run() fail.");
		/* FIXME: I think that the reply for the
		   continue packet could not be an error packet */
		return rsp_send_error(tp, 1);
	} 

	INFS("GDB: running!");
	/* signal that we are now running */
	gdb->running = true;
	thinkos_cond_signal(gdb->cond);

	return rsp_send_ack(tp);
}

static int rsp_h_packet(struct gdbtcpd * gdb, char * pkt, int len)
{
	struct tcp_pcb * tp = gdb->tp;
	int state;
	int ret;

	state = target_status();

	if (state == DBG_ST_UNCONNECTED) {
		DCC_LOG(LOG_TRACE, "unconnected");
		if (target_connect(1) < 0) {
			DCC_LOG(LOG_WARNING, "target_connect() failed!");
			rsp_send_msg(tp, "YARD-ICE: unconnect fail");
			return -1;
		}
		state = target_status();
	}

	if (state != DBG_ST_HALTED) {
		DCC_LOG(LOG_TRACE, "running");
		if (target_halt(0) < 0) {
			DCC_LOG(LOG_WARNING, "target_halt() failed!");
			return rsp_send_error(tp, 1);
		}

		if ((state = target_halt_wait(50)) == ERR_TIMEOUT) {
			DCC_LOG(LOG_TRACE, "timeout...");
			rsp_send_msg(tp, "YARD-ICE: target_halt failed!");
			return rsp_send_error(tp, 1);
		}
	}

	/* set thread for subsequent operations */
	switch (pkt[1]) {
	case 'c':
		DBGS("RSP: <-- Hc");
		ret = rsp_send_ok(tp);
		break;
	case 'g':
		DBG("RSP: <-- Hg%c", pkt[2]);
		ret = rsp_send_ok(tp);
		break;
	default:
		WARN("RSP: <-- H%c", pkt[1]);
		/* we don't have threads, empty replay */
		ret = rsp_send_empty(tp);
	}

	(void)ret;
	return 0;
}


static int rsp_v_packet(struct gdbtcpd * gdb, char * pkt, int len)
{
	struct tcp_pcb * tp = gdb->tp;

#if GDB_ENABLE_VCONT
	if (prefix(pkt, "vCont?")) {
		int n;
		DCC_LOG(LOG_MSG, "vCont?");
		n = str2str(pkt, "$vCont;c;C;s;S;t");
		return rsp_send_pkt(tp, pkt, n);
	}
#endif

	return rsp_send_empty(tp);
}

static int rsp_detach(struct gdbtcpd * gdb, char * pkt, int len)
{
	struct tcp_pcb * tp = gdb->tp;

	DCC_LOG(LOG_TRACE, "[DETACH]");
	/* detach - just reply OK */
	return rsp_send_ok(tp);
}

static int rsp_kill(struct gdbtcpd * gdb, char * pkt, int len)
{
	struct tcp_pcb * tp = gdb->tp;
	DCC_LOG(LOG_TRACE, "[KILL]");
	return rsp_send_ok(tp);
}
static int rsp_memory_write_bin(struct gdbtcpd * gdb, char * pkt, int len)
{
	struct tcp_pcb * tp = gdb->tp;
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
		return rsp_send_error(tp, 1);
	}

	DCC_LOG2(LOG_INFO, "addr=%08x size=%d", addr, size);
	return rsp_send_ok(tp);
}

static int rsp_pkt_recv(struct gdbtcpd * gdb, char * pkt, int max)
{
	struct tcp_pcb * tp = gdb->tp;
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
				cp[i] = '\0';
				return pos + i;
			}
			sum += c;
		}

		rem -= n;
		if (rem == 0)
			return -1;
	}
}

const struct rsp_pkt_decoder pkt_decoder_lst[] = {
	{ .c = '!', rsp_extended_mode },
	{ .c = '?', rsp_last_signal },
	{ .c = 'D', rsp_detach },
	{ .c = 'G', rsp_all_registers_set },
	{ .c = 'H', rsp_h_packet },
	{ .c = 'M', rsp_memory_write },
	{ .c = 'P', rsp_register_set },
	{ .c = 'Q', rsp_query },
	{ .c = 'X', rsp_memory_write_bin },
	{ .c = 'Z', rsp_breakpoint_insert },
	{ .c = 'c', rsp_continue },
	{ .c = 'g', rsp_all_registers_get },
	{ .c = 'i', rsp_step },
	{ .c = 'k', rsp_kill },
	{ .c = 'm', rsp_memory_read },
	{ .c = 'p', rsp_register_get },
	{ .c = 'q', rsp_query },
	{ .c = 's', rsp_step },
	{ .c = 'v', rsp_v_packet },
	{ .c = 'z', rsp_breakpoint_remove },
};

void rsp_comm_loop(struct gdbtcpd * gdb)
{
	struct tcp_pcb * tp = gdb->tp;
	char pkt[RSP_BUFFER_LEN];
	char buf[4];
	int state;
	int len;
	int c;
	int i;
/*
	gdb->nonstop_mode = false;
	gdb->session_valid = false;
	gdb->last_signal = TARGET_SIGNAL_0;
*/

//	gdb->target.op->breakpoint_clear_all(gdb->target.arg);
//	gdb->target.op->watchpoint_clear_all(gdb->target.arg);
	thinkos_mutex_lock(gdb->mutex);

	gdb->noack_mode = false;
	gdb->thread_id = 0; 
	gdb->last_signal = TARGET_SIGNAL_TRAP;
	gdb->connected = true;

	state = target_status();
	if (state == DBG_ST_RUNNING) {
		DCC_LOG(LOG_TRACE, "running");
		/* wakeup the break wait thread */
		gdb->running = true;
	} else {
		gdb->running = false;
	} 
	thinkos_cond_signal(gdb->cond);

	thinkos_mutex_unlock(gdb->mutex);

	for (;;) {

		len = tcp_recv(tp, buf, 1);
	
		if (len  <= 0) {
			INF("tcp_recv() failed!");
			DCC_LOG1(LOG_WARNING, "tcp_recv(): %d", len);
			break;
		}
		//	tracef("%s(): tcp_recv: %d", __func__, len);

		c = buf[0];

		if (c == '+') {
			DCC_LOG(LOG_INFO, "[ACK]");
			continue;
		}

		if (c == '-') {
			DCC_LOG(LOG_TRACE, "[NACK]");
			continue;

		}

		if (c == CTRL_C) {
			DCC_LOG(LOG_TRACE, "[BREAK]");
			if (rsp_break_signal(gdb, pkt) < 0) {
				DCC_LOG(LOG_WARNING, "rsp_break_signal() failed!");
				break;
			}
			continue;
		}

		if (c != '$') {
			DCC_LOG1(LOG_WARNING, "invalid: %02x", c);
			continue;
		}

		if ((len = rsp_pkt_recv(gdb, pkt, RSP_BUFFER_LEN)) <= 0) {
			DCC_LOG1(LOG_WARNING, "rsp_pkt_recv(): %d", len);
			break;
		}


		if (!gdb->noack_mode)
			rsp_send_ack(tp);

#ifdef ENABLE_LOG_PKT
		log_pkt(pkt, len);
#endif

		c = pkt[0];
		i = rsp_decoder_lookup(pkt_decoder_lst, LST_LEN(pkt_decoder_lst), c);
		if (i < 0) {
			DCC_LOG1(LOG_WARNING, "unsupported: '%c'", c);
			rsp_send_empty(tp);
			continue;
		}

		thinkos_mutex_lock(gdb->mutex);
		pkt_decoder_lst[i].decoder(gdb, pkt, len);
		thinkos_mutex_unlock(gdb->mutex);
	}

	gdb->running = false;
	gdb->connected = false;
	target_halt_wait_break();
}

