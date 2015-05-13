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

struct gdb_rspd {
	uint8_t noack_mode : 1;
	uint8_t nonstop_mode : 1;
	uint8_t stopped : 1;
	uint8_t active_app : 1;
	uint8_t last_signal;
	uint8_t cont_signal;
	struct {
		int8_t g; 
		int8_t c;
	} thread_id;
	struct dmon_comm * comm;
	void (* shell_task)(struct dmon_comm * comm);
};

typedef enum {
     BP_WRITE   = 0, /* Common (write) HW watchpoint */
     BP_READ    = 1, /* Read    HW watchpoint */
     BP_ACCESS  = 2, /* Access (read or write) HW watchpoint */
     BP_EXECUTE = 3  /* Execute HW breakpoint */
} dbg_hw_bp_t;

struct mem_range {
	char name[8];
	uint32_t base;
	uint32_t size;
};

const struct mem_range target_mem_map[] = {
	{ .name = "boot", .base = 0x08000000, .size = 4 * 16 * 1024 },
	{ .name = "app", .base = 0x08010000, .size = 64 * 1024 },
	{ .name = "ffs", .base = 0x08020000, .size = 4 * 128 * 1024 },
	{ .name = "sram0", .base = 0x20000000, .size = 112 * 1024 },
	{ .name = "sram1", .base = 0x2001c000, .size = 16 * 1024 },
	{ .name = "sram2", .base = 0x10000000, .size = 64 * 1024 }
};


struct flash_map {
	uint32_t base;
	uint8_t blk[];
};

struct flash_blk {
	uint32_t base;
	uint32_t size: 30;
	uint32_t ro: 1;
};

#define SZ_128   7
#define SZ_256   8
#define SZ_1K   10
#define SZ_2K   11
#define SZ_4K   12
#define SZ_8K   13
#define SZ_16K  14
#define SZ_32K  15
#define SZ_64K  16
#define SZ_128K 17
#define SZ_256K 18

#define BLK_RW  (0 << 7)
#define BLK_RO  (1 << 7)

const struct flash_map stm32f407_flash = {
	.base = (uint32_t)STM32_FLASH_MEM,
	.blk = {
		BLK_RO + SZ_16K, 
		BLK_RO + SZ_16K, 
		BLK_RO + SZ_16K, 
		BLK_RO + SZ_16K, 
		BLK_RW + SZ_64K, 
		BLK_RW + SZ_128K, 
		BLK_RW + SZ_128K, 
		BLK_RW + SZ_128K, 
		0
	}
};

#define BLK_SZ(BLK) (1 << ((BLK) & 0x3f)) 

int flash_addr2block(const struct flash_map * map,
					 uint32_t addr, struct flash_blk * blk) 
{
	uint32_t base = map->base;
	uint32_t size;
	int i;

	if (addr < base)
		return -1;

	for (i = 0; map->blk[i] != 0; ++i) {
		size = BLK_SZ(map->blk[i]);
		if (addr < base + size) {
			if (blk != NULL) {
				blk->base = base;
				blk->size = size;
				blk->ro = (map->blk[i] & BLK_RO) ? 1 : 0;
			}
			return i;
		}
		base += size;
	}

	return -1;
}

/*
flash (rx) : ORIGIN = 0x08000000, LENGTH = 512K
sram0 (rw) : ORIGIN = 0x20000000, LENGTH = 112K
sram1 (rw) : ORIGIN = 0x2001c000, LENGTH = 16K
sram2 (rw) : ORIGIN = 0x10000000, LENGTH = 64K
*/

typedef struct dbg_bp_ctrl dbg_bp_ctrl_t;

#define DBG_BREAKPOINT_MAX 8

typedef enum {
	OK = 0,
	ERR_UNDEF = -1,
	ERR_PARM = -2,
	ERR_BUSY = -3,
	ERR_STATE = -4,
	ERR_DRV_FAIL = -5,
	ERR_TIMEOUT = -6,
	ERR_NULL_TARGET = -7,
	ERR_OUTOFSYNC = -8
} dbg_errno_t;

/* target state */
typedef enum {
	DBG_ST_FAULT = -4,
	DBG_ST_ERROR = -3,
	DBG_ST_OUTOFSYNC = -2,
	DBG_ST_BUSY = -1,
	DBG_ST_UNDEF = 0,
	DBG_ST_UNCONNECTED = 1,
	DBG_ST_CONNECTED = 2,
	DBG_ST_RUNNING = 3,
	DBG_ST_HALTED = 4
} dbg_state_t;


#if (!THINKOS_ENABLE_PAUSE)
#error "Need THINKOS_ENABLE_PAUSE!"
#endif

#if (!THINKOS_ENABLE_DEBUG_STEP)
#error "Need THINKOS_ENABLE_DEBUG_STEP!"
#endif

#if (!THINKOS_ENABLE_DEBUG_FAULT)
#error "Need THINKOS_ENABLE_DEBUG_FAULT!"
#endif

/* -------------------------------------------------------------------------
 * Threads auxiliarly functions
 * ------------------------------------------------------------------------- */

#define THREAD_ID_OFFS 64
#define THREAD_ID_ALL -1
#define THREAD_ID_ANY 0
#define THREAD_ID_NONE -2

int thread_getnext(int thread_id)
{
	int id;

	id = __thinkos_thread_getnext(thread_id - THREAD_ID_OFFS);
	if (id >= 0)
		id += THREAD_ID_OFFS;

	return id;
}

int thread_active(void)
{
	if (thinkos_rt.active == THINKOS_THREAD_IDLE)
		DCC_LOG(LOG_TRACE, "IDLE");
//		return 0;
	return thinkos_rt.active + THREAD_ID_OFFS;
}

int thread_any(void)
{
	int thread_id;

	if ((unsigned int)thinkos_rt.active < THINKOS_THREADS_MAX) {
		DCC_LOG1(LOG_TRACE, "active=%d", thinkos_rt.active);
		return thinkos_rt.active + THREAD_ID_OFFS;
	}

	if (thinkos_rt.active == THINKOS_THREAD_IDLE)
		DCC_LOG(LOG_WARNING, "IDLE thread!");
	else if (thinkos_rt.active == THINKOS_THREAD_VOID)
		DCC_LOG(LOG_WARNING, "VOID thread!");
	else
		DCC_LOG1(LOG_WARNING, "active=%d is invalid!", thinkos_rt.active);

	/* Active thread is IDLE thread, try to get the first 
	   available thread */

	thread_id = __thinkos_thread_getnext(-1);
	
	if (thread_id < 0) {
		DCC_LOG(LOG_WARNING, "No threads!");
		return THREAD_ID_NONE;
	}

	return thread_id + THREAD_ID_OFFS;
}

bool thread_isalive(int thread_id)
{
	if (thread_id == 0)
		return true;

	return __thinkos_thread_isalive(thread_id - THREAD_ID_OFFS);
}

int thread_register_get(int thread_id, int reg, uint32_t * val)
{
	unsigned int idx = thread_id - THREAD_ID_OFFS;
	struct thinkos_context * ctx;
	uint32_t x;

	if (idx > THINKOS_THREADS_MAX)
		return -1;

	ctx = thinkos_rt.ctx[idx];
	switch (reg) {
	case 0:
		x = ctx->r0;
		break;
	case 1:
		x = ctx->r1;
		break;
	case 2:
		x = ctx->r2;
		break;
	case 3:
		x = ctx->r3;
		break;
	case 4:
		x = ctx->r4;
		break;
	case 5:
		x = ctx->r5;
		break;
	case 6:
		x = ctx->r6;
		break;
	case 7:
		x = ctx->r7;
		break;
	case 8:
		x = ctx->r8;
		break;
	case 9:
		x = ctx->r9;
		break;
	case 10:
		x = ctx->r10;
		break;
	case 11:
		x = ctx->r11;
		break;
	case 12:
		x = ctx->r12;
		break;
	case 13:
		x = (uint32_t)ctx;
		break;
	case 14:
		x = ctx->lr;
		break;
	case 15:
		x = ctx->pc;
		break;
	case 16:
		x = ctx->xpsr;
		break;
#if THINKOS_ENABLE_FPU
	case 17 ... 33 :
		x = ctx->s[reg - 17];
		break;
#endif
	default:
		return -1;
	}

	*val = x;

	return 0;
}

int thread_register_set(unsigned int thread_id, int reg, uint32_t val)
{
	unsigned int idx = thread_id - THREAD_ID_OFFS;
	struct thinkos_context * ctx;

	if (idx > THINKOS_THREADS_MAX)
		return -1;

	ctx = thinkos_rt.ctx[idx];

	DCC_LOG2(LOG_TRACE, "ThinkOS thread=%d context=%08x!", idx, ctx);
	if (((uint32_t)ctx < 0x10000000) || ((uint32_t)ctx >= 0x30000000)) {
		DCC_LOG(LOG_ERROR, "Invalid context!");
		return -1;
	}

	switch (reg) {
	case 0:
		ctx->r0 = val;
		break;
	case 1:
		ctx->r1 = val;
		break;
	case 2:
		ctx->r2 = val;
		break;
	case 3:
		ctx->r3 = val;
		break;
	case 4:
		ctx->r4 = val;
		break;
	case 5:
		ctx->r5 = val;
		break;
	case 6:
		ctx->r6 = val;
		break;
	case 7:
		ctx->r7 = val;
		break;
	case 8:
		ctx->r8 = val;
		break;
	case 9:
		ctx->r9 = val;
		break;
	case 10:
		ctx->r10 = val;
		break;
	case 11:
		ctx->r11 = val;
		break;
	case 12:
		ctx->r12 = val;
		break;
	case 13:
		thinkos_rt.ctx[idx] = (struct thinkos_context *)val;
		break;
	case 14:
		ctx->lr = val;
		break;
	case 15:
		ctx->pc = val;
		break;
	case 16:
//		ctx->xpsr = val;
		break;
#if THINKOS_ENABLE_FPU
	case 17 ... 33 :
		ctx->s[reg - 17] = val;
		break;
#endif
	default:
		return -1;
	}

	return 0;
}

/* -------------------------------------------------------------------------
 * Memory auxiliarly functions
 * ------------------------------------------------------------------------- */

static bool is_addr_valid(uint32_t addr) 
{
	int i;
	int n = sizeof(target_mem_map) / sizeof(struct mem_range);

	for (i = 0; i < n; ++i) {
		const struct mem_range * mem = &target_mem_map[i];
		if ((addr >= mem->base) && (addr < (mem->base + mem->size))) {
			return true;
		}
	}
	return false;
}


int mem_write(uint32_t addr, const void * ptr, unsigned int len)
{
	const struct flash_map * flash = &stm32f407_flash;
	struct flash_blk blk;
	uint32_t offs;

	if (!is_addr_valid(addr))
		return -1;

	if (flash_addr2block(flash, addr, &blk) < 0) {
		uint8_t * dst = (uint8_t *)addr;
		uint8_t * src = (uint8_t *)ptr;;
		int i;

		for (i = 0; i < len; ++i)
			dst[i] = src[i];
		return len;
	}

	if (blk.ro) {
		DCC_LOG2(LOG_ERROR, "read only block base=0x%08x size=%d", 
				 blk.base, blk.size);
		return -1;
	}

	offs = addr - flash->base;
	if (blk.base == addr) {
		DCC_LOG2(LOG_TRACE, "block erase base=0x%08x size=%d", 
				 blk.base, blk.size);
		stm32_flash_erase(offs, blk.size);
	};

	return stm32_flash_write(offs, ptr, len);
}

static int mem_read(uint32_t addr, void * ptr, unsigned int len)
{
	uint8_t * dst = (uint8_t *)ptr;
	uint8_t * src = (uint8_t *)addr;;
	int i;

	if (!is_addr_valid(addr))
		return -1;

	for (i = 0; i < len; ++i)
		dst[i] = src[i];

	return len;
}

/* -------------------------------------------------------------------------
 * Core auxiliarly functions
 * ------------------------------------------------------------------------- */

void target_halt(struct gdb_rspd * gdb)
{
	dmon_app_suspend();
	gdb->stopped = true;
}

int target_goto(uint32_t addr, int opt)
{
	return 0;
}

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
	DCC_LOG(LOG_TRACE, "--> Empty.");
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

	DCC_LOG1(LOG_TRACE, "--> Error(%d)!", err);

	return dmon_comm_send(comm, pkt, 7);
}

static int rsp_send_pkt(struct dmon_comm * comm, char * pkt, int len)
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
	DCC_LOGSTR(LOG_TRACE, "--> '%s'", pkt);

	return dmon_comm_send(comm, pkt, n);
}


static void rsp_break_signal(struct gdb_rspd * gdb, struct dmon_comm * comm)
{
	DCC_LOG(LOG_TRACE, "break received, stopping...");

	target_halt(gdb);
	gdb->last_signal = TARGET_SIGNAL_INT;
}

static void rsp_breakpoint(struct gdb_rspd * gdb, struct dmon_comm * comm)
{
	DCC_LOG(LOG_TRACE, "breakpoint, stopping...");

	target_halt(gdb);
	gdb->last_signal = TARGET_SIGNAL_TRAP;
}

int rsp_thread_extra_info(struct dmon_comm * comm, char * pkt)
{
	char * cp = pkt + sizeof("qThreadExtraInfo,") - 1;
	int idx;
	int thread_id;
	int oid;
	int type;
	bool tmw;
	int n;

	/* qThreadExtraInfo */
	thread_id = hex2int(cp, NULL);
	DCC_LOG1(LOG_INFO, "thread_id=%d", thread_id);
	idx = thread_id - THREAD_ID_OFFS;

	cp = pkt;
	*cp++ = '$';
	if (idx == THINKOS_THREAD_IDLE) {
		n = str2hex(cp, "IDLE");
	} else {
#if THINKOS_ENABLE_THREAD_INFO
		if (thinkos_rt.th_inf[idx] != NULL)
			n = str2hex(cp, thinkos_rt.th_inf[idx]->tag);
		else
			n = int2str2hex(cp, idx);
#else
		n = int2str2hex(cp, idx);
#endif
	}
	cp += n;
	n = char2hex(cp, ' ');
	cp += n;

	if (idx == THINKOS_THREAD_IDLE) {
		oid = 0;
		tmw = false;
	} else {
#if THINKOS_ENABLE_THREAD_STAT
		oid = thinkos_rt.th_stat[idx] >> 1;
		tmw = thinkos_rt.th_stat[idx] & 1;
#else
		oid = 0; /* FIXME */
		tmw = (thinkos_rt.wq_clock & (1 << idx)) ? true : false;
#endif
	}
	if (tmw) {
		if (oid == 0) {
			n = str2hex(cp, "time wait");
		} else {
			n = str2hex(cp, "timedwait on ");
		}
	} else {
		if (oid == 0) {
			n = str2hex(cp, "ready");
		} else {
			n = str2hex(cp, "wait on ");
		}
	}
	cp += n;
	if (oid != 0) {
		type = thinkos_obj_type_get(oid);
		if (type == THINKOS_OBJ_PAUSED) {
			DCC_LOG1(LOG_ERROR, "thread %d is paused!!!", thread_id);
		}
		n = str2hex(cp, thinkos_type_name_lut[type]);
		cp += n;
		n = char2hex(cp, '(');
		cp += n;
		n = int2str2hex(cp, oid);
		cp += n;
		n = char2hex(cp, ')');
		cp += n;
	}
	n = cp - pkt;

	return rsp_send_pkt(comm, pkt, n);
}

int rsp_thread_get_first(struct dmon_comm * comm, char * pkt)
{
	char * cp;
	int thread_id;
	int n;

	/* get the first thread */
	if ((thread_id = thread_getnext(0)) < 0) {
		n = str2str(pkt, "$l");
	} else {
		cp = pkt;
		cp += str2str(cp, "$m");
		cp += uint2hex(cp, thread_id);
		while ((thread_id = thread_getnext(thread_id)) > 0) {
			*cp++ = ',';
			cp += uint2hex(cp, thread_id);
		}
		n = cp - pkt;
	}

	return rsp_send_pkt(comm, pkt, n);
}

int rsp_thread_get_next(struct dmon_comm * comm, char * pkt)
{
	int n;

	n = str2str(pkt, "$l");
	return rsp_send_pkt(comm, pkt, n);
}

int rsp_output(struct gdb_rspd * gdb, struct dmon_comm * comm, char * pkt)
{
	char * cp;
	int n;

	cp = pkt;
	*cp++ = '$';
	*cp++ = 'O';
#if (THINKOS_ENABLE_CONSOLE)
	uint8_t * ptr;
	int cnt;
	if ((cnt = __console_tx_pipe_ptr(&ptr)) > 0) {
		cp += bin2hex(cp, ptr, cnt);
		__console_tx_pipe_commit(cnt);
	}
#endif

	n = cp - pkt;
	return rsp_send_pkt(comm, pkt, n);
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
	return rsp_send_pkt(comm, pkt, n);
}


static int rsp_query(struct gdb_rspd * gdb, struct dmon_comm * comm,
					 char * pkt, int len)
{
	char * cp;
	int n;

	pkt[len] = '\0';
	DCC_LOGSTR(LOG_INFO, "'%s'", pkt);

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
		cp += uint2hex(cp, thread_active());
		n = cp - pkt;
		return rsp_send_pkt(comm, pkt, n);
	}

	if (prefix(pkt, "qAttached")) {
		/* Reply:
		   '1' - The remote server attached to an existing process. 
		   '0' - The remote server created a new process. 
		 */
		/* XXX: if there is no active application run  */
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
		return rsp_send_pkt(comm, pkt, n);
	}

	if (prefix(pkt, "qOffsets")) {
		n = str2str(pkt, "$Text=0;Data=0;Bss=0");
		return rsp_send_pkt(comm, pkt, n);
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
		cp += str2str(cp, ";qXfer:features:read-"
					";qRelocInsn-"
					";multiprocess-"
#if 0
					";QPassSignals+"
#endif
#if GDB_ENABLE_NOACK_MODE
					";QStartNoAckMode+"
#endif
#if GDB_ENABLE_NOSTOP_MODE
					";QNonStop+"
#endif
					);
		n = cp - pkt;
		return rsp_send_pkt(comm, pkt, n);
	}

	if (prefix(pkt, "qfThreadInfo")) {
		DCC_LOG(LOG_INFO, "qfThreadInfo");
		/* First Thread Info */
		return rsp_thread_get_first(comm, pkt);
	}

	if (prefix(pkt, "qsThreadInfo")) {
		DCC_LOG(LOG_INFO, "qsThreadInfo");
		/* Sequence Thread Info */
		return rsp_thread_get_next(comm, pkt);
	}

	/* Get thread info from RTOS */
	if (prefix(pkt, "qThreadExtraInfo")) {
		DCC_LOG(LOG_INFO, "qThreadExtraInfo");
		return rsp_thread_extra_info(comm, pkt);
	}

	if (prefix(pkt, "qXfer:memory-map:read::")) {
		DCC_LOG(LOG_TRACE, "qXfer:memory-map:read::");
		return rsp_empty(comm);
	}

	if (prefix(pkt, "qXfer:features:read:")) {
		DCC_LOG(LOG_TRACE, "qXfer:features:read:");
		return rsp_empty(comm);
	}

	if (prefix(pkt, "qTStatus")) {
		DCC_LOG(LOG_TRACE, "qTStatus");
		return rsp_empty(comm);
	}

	if (prefix(pkt, "QNonStop:")) {
		gdb->nonstop_mode = pkt[9] - '0';
		DCC_LOG1(LOG_TRACE, "Nonstop=%d +++++++++++++++", gdb->nonstop_mode);
		if (!gdb->nonstop_mode && !gdb->stopped) {
			target_halt(gdb);
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
static int rsp_all_registers_get(struct gdb_rspd * gdb, 
								 struct dmon_comm * comm, 
								 char * pkt, int len)
{
	unsigned int val = 0;
	char * cp;
	int thread_id;
	int n;
	int r;

	if (gdb->thread_id.g == THREAD_ID_ALL) {
		DCC_LOG(LOG_TRACE, "g thread set to ALL!!!");
		thread_id = thread_any();
	} else if (gdb->thread_id.g == THREAD_ID_ANY) {
		DCC_LOG(LOG_TRACE, "g thread set to ANY");
		thread_id = thread_any();
	} else {
		thread_id = gdb->thread_id.g;
	}

	DCC_LOG1(LOG_TRACE, "thread_id=%d", thread_id);

	if (thread_id < 0) {
		return rsp_error(comm, 4);
	} 

	if (!(__thinkos_thread_ispaused(thread_id - THREAD_ID_OFFS) || 
		  __thinkos_thread_isfaulty(thread_id - THREAD_ID_OFFS))) {
		return rsp_error(comm, 5);
	} 

	cp = pkt;
	*cp++ = '$';

	/* all integer registers */
	for (r = 0; r < 16; r++) {
		thread_register_get(thread_id, r, &val);
		DCC_LOG2(LOG_INFO, "R%d = 0x%08x", r, val);
		*cp++ = hextab[((val >> 4) & 0xf)];
		*cp++ = hextab[(val & 0xf)];
		*cp++ = hextab[((val >> 12) & 0xf)];
		*cp++ = hextab[((val >> 8) & 0xf)];
		*cp++ = hextab[((val >> 20) & 0xf)];
		*cp++ = hextab[((val >> 16) & 0xf)];
		*cp++ = hextab[((val >> 28) & 0xf)];
		*cp++ = hextab[((val >> 24) & 0xf)];
	}

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
	return rsp_send_pkt(comm, pkt, n);
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

	if (gdb->thread_id.g == THREAD_ID_ALL) {
		DCC_LOG(LOG_TRACE, "g thread set to ALL!!!");
		thread_id = thread_any();
	} else if (gdb->thread_id.g == THREAD_ID_ANY) {
		DCC_LOG(LOG_TRACE, "g thread set to ANY");
		thread_id = thread_any();
	} else {
		thread_id = gdb->thread_id.g;
	}

	if (thread_id < 0) {
		return rsp_error(comm, 4);
	} 

	if (!thread_isalive(thread_id)) {
		DCC_LOG1(LOG_WARNING, "thread %d is dead!", thread_id);
		return rsp_ok(comm);
	}

	reg = hex2int(&pkt[1], NULL);

	/* FIXME: the register enumaration and details 
	   must be in the ICE driver not here! */
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

	if (gdb->thread_id.g == THREAD_ID_ALL) {
		DCC_LOG(LOG_TRACE, "'g' thread set to ALL!!!");
		thread_id = thread_any();
	} else if (gdb->thread_id.g == THREAD_ID_ANY) {
		thread_id = thread_any();
		DCC_LOG1(LOG_TRACE, "'g' thread set to ANY -> thread_id=%d", thread_id);
	} else {
		thread_id = gdb->thread_id.g;
	}

	if (thread_id < 0) {
		DCC_LOG(LOG_WARNING, "no valid threads!");
		return rsp_ok(comm);
	} 

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

	DCC_LOG2(LOG_INFO, "addr=0x%08x size=%d", addr, size);

	max = (RSP_BUFFER_LEN - 5) >> 1;

	if (size > max)
		size = max;

	if ((ret = mem_read(addr, buf, size)) < 0) {
		DCC_LOG3(LOG_WARNING, "ERR: %d addr=%08x size=%d", ret, addr, size);

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

	pkt[n] = '\0';
	DCC_LOGSTR(LOG_INFO, "pkt='%s'", pkt);
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

	if (gdb->thread_id.c == THREAD_ID_ALL) {
		DCC_LOG(LOG_TRACE, "c thread set to ALL!!!");
		thread_id = thread_any();
	} else if (gdb->thread_id.c == THREAD_ID_ANY) {
		DCC_LOG(LOG_TRACE, "c thread set to ANY");
		thread_id = thread_any();
	} else {
		thread_id = gdb->thread_id.c;
	}

	if (thread_id < 0) {
		return rsp_error(comm, 4);
	} 

	/* step */
	if (pkt[1] != '#') {
		addr = hex2int(&pkt[1], 0);
		DCC_LOG1(LOG_TRACE, "Addr=%08x", addr);
		target_goto(addr, 0);
	}

	DCC_LOG1(LOG_TRACE, "gdb_thread_id=%d.", thread_id);

	dmon_thread_step(thread_id - THREAD_ID_OFFS, false);
	return 0;
}

static int rsp_step_done(struct gdb_rspd * gdb, struct dmon_comm * comm, 
						 char * pkt)
{
	int thread_id = thinkos_rt.step_id + THREAD_ID_OFFS;

	(void)thread_id;
	DCC_LOG1(LOG_TRACE, "thread_id=%d.", thread_id);

	dmon_app_suspend();
	gdb->stopped = true;

	return rsp_stop_reply(gdb, comm, pkt);
}

static int rsp_continue(struct gdb_rspd * gdb, struct dmon_comm * comm, 
						char * pkt, int len)
{
	unsigned int addr;
	int thread_id;

	DCC_LOG(LOG_TRACE, "...");


	if (gdb->thread_id.c == THREAD_ID_ALL) {
		DCC_LOG(LOG_TRACE, "c thread set to ALL!!!");
		thread_id = thread_any();
	} else if (gdb->thread_id.c == THREAD_ID_ANY) {
		DCC_LOG(LOG_TRACE, "c thread set to ANY");
		thread_id = thread_any();
	} else {
		thread_id = gdb->thread_id.c;
	}

	if (thread_id < 0) {
		return rsp_error(comm, 4);
	} 

	if (pkt[1] != '#') {
		addr = hex2int(&pkt[1], 0);
		target_goto(addr, 0);
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


static int rsp_thread_isalive(struct gdb_rspd * gdb, 
							  struct dmon_comm * comm, char * pkt, int len)
{
	int ret = 0;
	int thread_id;

	thread_id = hex2int(&pkt[1], NULL);
	DCC_LOG1(LOG_INFO, "thread_id = %d", thread_id);

	/* XXX: if there is no active application run  */
	if (!gdb->active_app) {
		DCC_LOG(LOG_WARNING, "no active application, "
				"calling dmon_app_exec()!");
		if (dmon_app_exec(true) < 0) {
			return rsp_error(comm, 1);
		}
		gdb->active_app = true;
	}

	/* Find out if the thread thread-id is alive. 
	   'OK' thread is still alive 
	   'E NN' thread is dead */

	if (thread_isalive(thread_id))
		ret = rsp_ok(comm);
	else 
		ret = rsp_error(comm, 1);

	return ret;
}

static int rsp_h_packet(struct gdb_rspd * gdb, struct dmon_comm * comm, 
						char * pkt, int len)
{
	int ret = 0;
	int thread_id;

	if ((pkt[2] == '-') && (pkt[3] == '1')) {
		thread_id = THREAD_ID_ALL;
		DCC_LOG(LOG_TRACE, "all threads");
	} else {
		thread_id = hex2int(&pkt[2], NULL);
		DCC_LOG1(LOG_TRACE, "thread_id=%d", thread_id);
	}

	/* XXX: if there is no active application run  */
	if (!gdb->active_app) {
		DCC_LOG(LOG_WARNING, "no active application, "
				"calling dmon_app_exec()!");
		if (dmon_app_exec(true) < 0) {
			return rsp_error(comm, 1);
		}
		gdb->active_app = true;
	}

	/* set thread for subsequent operations */
	switch (pkt[1]) {
	case 'c':
		gdb->thread_id.c = thread_id;
		ret = rsp_ok(comm);
		break;

	case 'g':
		gdb->thread_id.g = thread_id;
		ret = rsp_ok(comm);
		break;
/*
	case 'p':
		gdb->thread_id.p = thread_id;
		ret = rsp_ok(comm);
		break; */

	default:
		DCC_LOG2(LOG_WARNING, "Unsupported 'H%c%d'", pkt[1], thread_id);
		ret = rsp_empty(comm);
	}

	return ret;
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
	DCC_LOG(LOG_TRACE, "vCont ==================");

	if (prefix(pkt, "vCont?")) {
		DCC_LOG(LOG_TRACE, "vCont?");
		n = str2str(pkt, "$vCont;c;C;s;S;t");
		return rsp_send_pkt(comm, pkt, n);
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
					__thinkos_thread_resume(thread_id - THREAD_ID_OFFS);
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
					__thinkos_thread_resume(thread_id - THREAD_ID_OFFS);
				}
				gdb->last_signal = sig;
				break;
			case 's':
				DCC_LOG1(LOG_TRACE, "Step %d", thread_id);
				dmon_thread_step(thread_id - THREAD_ID_OFFS, false);
				break;
			case 'S':
				DCC_LOG2(LOG_TRACE, "Step %d sig=%d", thread_id, sig);
				break;
			case 't':
				DCC_LOG1(LOG_TRACE, "Stop %d", thread_id);
				break;
			default:
				DCC_LOG(LOG_TRACE, "Unsupported!");
				/* we don't have threads, empty replay */
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

	if (gdb->stopped)
		dmon_app_continue();

	/* reply OK */
	rsp_ok(comm);

	dmon_exec(gdb->shell_task);

	return 0;
}

static int rsp_kill(struct gdb_rspd * gdb, struct dmon_comm * comm)
{
	DCC_LOG(LOG_TRACE, "[KILL]");

	if (gdb->stopped)
		dmon_app_continue();

	rsp_ok(comm);

	dmon_exec(gdb->shell_task);

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

	if (mem_write(addr, cp, size) < 0) {
		return rsp_error(comm, 1);
	}

	return rsp_ok(comm);
}

static int rsp_pkt_input(struct gdb_rspd * gdb, struct dmon_comm * comm, 
						 char * pkt, unsigned int len)
{
	int ret;

	pkt[len] = 0;
//	if ((pkt[0] != 'X'))
	if ((pkt[0] != 'X') && (pkt[0] != 'm'))
		DCC_LOGSTR(LOG_TRACE, "'%s'", pkt);

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
		ret = rsp_stop_reply(gdb, comm, pkt);
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
	int ret = -1;
	char * cp;
	int pos;
	int rem;
	int sum;
	int c;
	int n;
	int i;
	int j;
	bool esc = false;

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
			sum += c;
			if (esc) {
				esc = false;
				cp[j++] = c ^ 0x20;
			} else {
				if (c == '#') {
					dmon_alarm_stop();
					return pos + i;
					/* FIXME: check the sum!!! */
				} else if (c == '}') {
					esc = true;
				} else {
					cp[j++] = c;
				}
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
	uint8_t * ptr;
	int cnt;
	int len;

	DCC_LOG(LOG_TRACE, "GDB start...");

	gdb->nonstop_mode = 0;
	gdb->noack_mode = 0;
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
		}

		if (sigset & (1 << DMON_THREAD_STEP)) {
			DCC_LOG(LOG_TRACE, "Thread step.");
			dmon_clear(DMON_THREAD_STEP);
			rsp_step_done(gdb, comm, pkt);
		}

		if (sigset & (1 << DMON_BREAKPOINT)) {
			DCC_LOG(LOG_TRACE, "Breakpoint.");
			dmon_clear(DMON_BREAKPOINT);
			rsp_breakpoint(gdb, comm);
			rsp_stop_reply(gdb, comm, pkt);
		}

		if (sigset & (1 << DMON_COMM_CTL)) {
			DCC_LOG(LOG_TRACE, "Comm Ctl.");
			dmon_clear(DMON_COMM_CTL);
			if (!dmon_comm_isconnected(comm))	
				dmon_exec(gdb->shell_task);
		}

		if (sigset & (1 << DMON_TX_PIPE)) {
			DCC_LOG(LOG_TRACE, "TX Pipe.");
			if ((cnt = __console_tx_pipe_ptr(&ptr)) > 0) {
				rsp_output(gdb, comm, pkt);
			} else {
				DCC_LOG(LOG_INFO, "TX Pipe empty!!!");
				dmon_clear(DMON_TX_PIPE);
			}
		}

		if (sigset & (1 << DMON_COMM_RCV)) {
			DCC_LOG(LOG_INFO, "Comm RX.");
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
				rsp_break_signal(gdb, comm);
				rsp_stop_reply(gdb, comm, pkt);
				break;


			default:
				DCC_LOG1(LOG_WARNING, "invalid: %02x", buf[0]);
				break;
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

