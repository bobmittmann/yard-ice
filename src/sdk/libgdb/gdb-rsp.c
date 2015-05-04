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

int thread_id = -1;

const char monitor_menu[] = "\r\n"
"-- ThinkOS Monitor Commands ---------\r\n"
" Ctrl+Q - Restart monitor\r\n"
" Ctrl+R - Resume all threads\r\n"
" Ctrl+P - Pause all threads\r\n"
" Ctrl+X - Show exception\r\n"
" Ctrl+I - ThinkOS info\r\n"
" Ctrl+N - Select Next Thread\r\n"
"-------------------------------------\r\n\r\n";

void monitor_show_help(struct dmon_comm * comm)
{
	dmon_comm_send(comm, monitor_menu, sizeof(monitor_menu) - 1);
}

void monitor_on_fault(struct dmon_comm * comm)
{
	const struct thinkos_except * xcpt = &thinkos_except_buf;

	DCC_LOG(LOG_TRACE, "DMON_THREAD_FAULT.");
	dmprintf(comm, "Fault at thread %d\r\n", xcpt->thread);
	dmon_print_context(comm, &xcpt->ctx, xcpt->sp);
}

void monitor_dump(struct dmon_comm * comm)
{
	dmon_print_thread(comm, thread_id);
}

void monitor_pause_all(struct dmon_comm * comm)
{
	dmprintf(comm, "\r\nPausing all threads...\r\n");
	__thinkos_pause_all();
	dmon_wait_idle();
	dmprintf(comm, "[Idle]\r\n");
}

void monitor_resume_all(struct dmon_comm * comm)
{
	dmprintf(comm, "\r\nResuming all threads...\r\n");
	__thinkos_resume_all();
	dmprintf(comm, "Restarting...\r\n");
}

struct gdb_rspd {
	uint8_t current_thread;
	struct {
		uint32_t noack_mode : 1;
		uint32_t nonstop_mode : 1;
		uint32_t stopped : 1;
		uint32_t active_app : 1;
	};
	struct {
		int8_t g; 
		int8_t c;
		int8_t p;
	} thread_id;
	uint8_t last_signal;
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
	.base = 0x08000000,
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

int thread_getnext(int th)
{
	return __thinkos_thread_getnext(th - 1) + 1;
}

void target_halt(struct gdb_rspd * gdb)
{
	__thinkos_pause_all();

	if (thinkos_rt.active == THINKOS_THREADS_MAX) {
		DCC_LOG(LOG_TRACE, "Current is Idle!");
		gdb->current_thread = thread_getnext(0);
	} else {
		gdb->current_thread = thinkos_rt.active + 1;
	}

	DCC_LOG1(LOG_TRACE, "current_thread=%d", gdb->current_thread);

	dmon_wait_idle();
	gdb->stopped = true;
}

int target_goto(uint32_t addr, int opt)
{
	return 0;
}

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


int thread_register_get(int thread, int reg, uint32_t * val)
{
	unsigned int idx = thread - 1;
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
	default:
		return -1;
	}

	*val = x;

	return 0;
}

int thread_register_set(int thread, int reg, uint32_t val)
{
	unsigned int idx = thread - 1;
	struct thinkos_context * ctx;

	if (idx > THINKOS_THREADS_MAX)
		return -1;

	ctx = thinkos_rt.ctx[idx];
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
	default:
		return -1;
	}

	return 0;
}

/***********************************************************************
 Breakpoint management
 ***********************************************************************/

#define BP_DEFSZ 2
#define BP_MAX 6

bool breakpoint_set(uint32_t addr, uint32_t size)
{
	struct cm3_fpb * fbp = CM3_FPB;
	uint32_t comp;
	int i;

	for (i = 0; i < BP_MAX; ++i) {
		if ((fbp->comp[i] & COMP_ENABLE) == 0) 
			break;
	}

	if (i == BP_MAX) {
		DCC_LOG(LOG_WARNING, "no more breakpoints");
		return false;
	}

	/* use default size if zero */
	size = (size == 0) ? BP_DEFSZ : size;

	if (size == 2) {
		if (addr & 0x00000002) {
			comp = COMP_BP_HIGH | (addr & 0xfffffffc) | COMP_ENABLE;
		} else {
			comp = COMP_BP_LOW | (addr & 0xfffffffc) | COMP_ENABLE;
		}
	} else {
		comp = COMP_BP_WORD | (addr & 0xfffffffc) | COMP_ENABLE;
	}

	fbp->comp[i] = comp;

	DCC_LOG4(LOG_TRACE, "bp=%d addr=0x%08x size=%d comp=0x%08x ", i, addr, 
			 size, fbp->comp[i]);

	return true;
}

bool breakpoint_clear(uint32_t addr, uint32_t size)
{
	struct cm3_fpb * fbp = CM3_FPB;
	uint32_t comp;
	int i;

	size = (size == 0) ? BP_DEFSZ : size;

	if (size == 2) {
		if (addr & 0x00000002) {
			comp = COMP_BP_HIGH | (addr & 0xfffffffc) | COMP_ENABLE;
		} else {
			comp = COMP_BP_LOW | (addr & 0xfffffffc) | COMP_ENABLE;
		}
	} else {
		comp = COMP_BP_WORD | (addr & 0xfffffffc) | COMP_ENABLE;
	}

	DCC_LOG2(LOG_TRACE, "addr=0x%08x size=%d", addr, size);

	for (i = 0; i < BP_MAX; ++i) {
		if (fbp->comp[i] == comp) {
			fbp->comp[i] = 0;
			return true;
		}
	}

	DCC_LOG(LOG_WARNING, "breakpoint not found!");
	return false;
}

void breakpoint_clear_all(void)
{
	struct cm3_fpb * fbp = CM3_FPB;
	int i;

	for (i = 0; i < BP_MAX; ++i) {
		fbp->comp[i] = 0;
	}
}
/*
 * Common response packets
 */

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
	DCC_LOG(LOG_TRACE, "--> Ok.");
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

static int  rsp_offsets(struct dmon_comm * comm, unsigned int text,
							   unsigned int data, unsigned int bss)
{
	char s[128];
	int n;

	DCC_LOG3(LOG_TRACE, "text=%08x data=%08x bss=%08x", text, data, bss);

	/* FIXME: max id = 15 by the time */
	n = sprintf(s, "+$Text=%x;Data=%x;Bss=%x#", text, data, bss);
	rsp_fixup_sum(s + 2);
	return dmon_comm_send(comm, s, n + 2);
}
#endif

int rsp_thread_extra_info(struct dmon_comm * comm, char * pkt)
{
	char * cp = pkt + sizeof("qThreadExtraInfo,") - 1;
	int idx;
	int id;
	int oid;
	int type;
	bool tmw;
	int n;

	/* qThreadExtraInfo */
	id = hex2int(cp, NULL);
	DCC_LOG1(LOG_INFO, "thread_id=%d", id);
	idx = id - 1;

	cp = pkt;
	*cp++ = '$';
#if THINKOS_ENABLE_THREAD_INFO
	if (thinkos_rt.th_inf[idx] != NULL)
		n = str2hex(cp, thinkos_rt.th_inf[idx]->tag);
	else
		n = int2str2hex(cp, idx);
#else
	n = int2str2hex(cp, idx);
#endif
	cp += n;
	n = char2hex(cp, ' ');
	cp += n;
#if THINKOS_ENABLE_THREAD_STAT
	oid = thinkos_rt.th_stat[idx] >> 1;
	tmw = thinkos_rt.th_stat[idx] & 1;
#else
	oid = 0; /* FIXME */
	tmw = (thinkos_rt.wq_clock & (1 << idx)) ? true : false;
#endif
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
			DCC_LOG1(LOG_ERROR, "thread %d is paused!!!", id);
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
	int id;
	int n;

	/* get the first thread */
	if ((id = thread_getnext(0)) < 0) {
		n = str2str(pkt, "$l");
	} else {
		cp = pkt;
		cp += str2str(cp, "$m");
		cp += uint2hex(cp, id);
		while ((id = thread_getnext(id)) > 0) {
			*cp++ = ',';
			cp += uint2hex(cp, id);
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

#if 0
static int rsp_thread_id(struct dmon_comm * comm, int id)
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

	return dmon_comm_send(comm, pkt, n);
}
#endif

int rsp_write(struct dmon_comm * comm, const void * buf, int len)
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

	return dmon_comm_send(comm, pkt, n);
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
		cp = pkt + str2str(pkt, "$QC");
		cp += uint2hex(cp, gdb->current_thread);
		n = cp - pkt;
		return rsp_send_pkt(comm, pkt, n);
	}

	if (prefix(pkt, "qAttached")) {
		/* Reply:
		   '1' - The remote server attached to an existing process. 
		   '0' - The remote server created a new process. 
		 */
		n = str2str(pkt, "$1");
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

	DCC_LOG(LOG_TRACE, "unsupported query");

	return rsp_empty(comm);


}

static int rsp_all_registers_get(struct gdb_rspd * gdb, 
								 struct dmon_comm * comm, 
								 char * pkt, int len)
{
	unsigned int val = 0;
	int sum = 0;
	int th;
	int n;
	int r;

	if (gdb->thread_id.g <= 0)
		th = gdb->current_thread;
	else
		th = gdb->thread_id.g;

	DCC_LOG1(LOG_TRACE, "thread_id=%d", th);

	if (!(__thinkos_thread_ispaused(th - 1) || 
		  __thinkos_thread_isfaulty(th - 1))) {
		return rsp_error(comm, 5);
	} 

	pkt[0] = '$';
	n = 1;

	/* all integer registers */
	for (r = 0; r < 16; r++) {
		thread_register_get(th, r, &val);
	
		DCC_LOG2(LOG_INFO, "R%d = 0x%08x", r, val);

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
	thread_register_get(th, 16, &val);

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

	return dmon_comm_send(comm, pkt, n);
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
	int th;
	int n;

	th = gdb->thread_id.p;

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
		thread_register_get(th, 16, &val);
	} else {
		if (reg > 15 ) {
			val = 0;
			DCC_LOG1(LOG_WARNING, "reg=%d (float)", reg);
		} else {
			thread_register_get(th, reg, &val);
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
	int th;
	char * cp;

	if (gdb->thread_id.g <= 0)
		th = gdb->current_thread;
	else
		th = gdb->thread_id.g;

	if (!gdb->active_app) {
		DCC_LOG(LOG_WARNING, "no active application!");
		if (dmon_app_exec(true) < 0) {
			return rsp_error(comm, 8);
		}
		gdb->active_app = true;
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

	DCC_LOG2(LOG_TRACE, "reg=%d val=0x%08x", reg, val);

	if (thread_register_set(th, reg, val) < 0) {
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
		DCC_LOG3(LOG_TRACE, "ERR: %d addr=%08x size=%d", ret, addr, size);

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

	if (breakpoint_set(addr, size))
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

	if (breakpoint_clear(addr, size))
		return rsp_ok(comm);

	return rsp_error(comm, 1);
}

static int rsp_step(struct gdb_rspd * gdb, struct dmon_comm * comm, 
					char * pkt, int len)
{
	unsigned int addr;
	int thread_id;

	if (gdb->thread_id.g <= 0)
		thread_id = gdb->current_thread;
	else
		thread_id = gdb->thread_id.g;

	/* step */
	if (pkt[1] != '#') {
		addr = hex2int(&pkt[1], 0);
		DCC_LOG1(LOG_TRACE, "Addr=%08x", addr);
		target_goto(addr, 0);
	}

	DCC_LOG(LOG_TRACE, ".");

	dmon_thread_step(thread_id - 1, false);
	return 0;
}

static int rsp_step_done(struct gdb_rspd * gdb, struct dmon_comm * comm, 
						 char * pkt)
{
	int thread_id = thinkos_rt.step_id + 1;
	unsigned int val;
	int sum;
	int n;

	(void)thread_id;
	DCC_LOG1(LOG_TRACE, "thread_id=%d.", thread_id);

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

	return dmon_comm_send(comm, pkt, n);
}

static int rsp_continue(struct gdb_rspd * gdb, struct dmon_comm * comm, 
						char * pkt, int len)
{
	unsigned int addr;

	DCC_LOG(LOG_TRACE, "...");

	if (pkt[1] != '#') {
		addr = hex2int(&pkt[1], 0);
		target_goto(addr, 0);
	}

	__thinkos_resume_all();
	gdb->stopped = false;

	/* signal that we are now running */
	return dmon_comm_send(comm, "+", 1);
}

static int rsp_thread_isalive(struct dmon_comm * comm, char * pkt, int len)
{
	int ret = 0;
	int id;

	id = hex2int(&pkt[1], NULL);
	DCC_LOG1(LOG_INFO, "T%d", id);

	/* Find out if the thread thread-id is alive. 
	   ‘OK’ thread is still alive 
	   ‘E NN’ thread is dead */

	if (__thinkos_thread_isalive(id - 1))
		ret = rsp_ok(comm);
	else 
		ret = rsp_error(comm, 1);

	return ret;
}

static int rsp_h_packet(struct gdb_rspd * gdb, struct dmon_comm * comm, 
						char * pkt, int len)
{
	int ret = 0;
	int id;

	id = hex2int(&pkt[2], NULL);
	DCC_LOG2(LOG_TRACE, "H%c%d", pkt[1], id);

	/* set thread for subsequent operations */
	switch (pkt[1]) {
	case 'c':
		gdb->thread_id.c = id;
		ret = rsp_ok(comm);
		break;
	case 'g':
		gdb->thread_id.g = id;
		ret = rsp_ok(comm);
		break;
	case 'p':
		gdb->thread_id.p = id;
		ret = rsp_ok(comm);
		break;
	default:
		DCC_LOG2(LOG_WARNING, "Unsupported 'H%c%d'", pkt[1], id);
		ret = rsp_empty(comm);
	}

	return ret;
}

static int rsp_v_packet(struct dmon_comm * comm, char * pkt, int len)
{
	int n;
	int id;
	int ret;

	pkt[len] = '\0';

	if (prefix(pkt, "vCont?")) {
		DCC_LOG(LOG_TRACE, "vCont?");
		n = str2str(pkt, "$vCont;c;s;t");
		return rsp_send_pkt(comm, pkt, n);
	}

	if (prefix(pkt, "vCont;")) {
		if (pkt[7] == ':') { 
			id = hex2int(&pkt[8], NULL);
	 	} else {
			id = 0;
		}
		(void)id;
		switch (pkt[6]) {
		case 'c':
			DCC_LOG1(LOG_TRACE, "Continue %d", id);
			ret = rsp_ok(comm);
			break;
		case 's':
			DCC_LOG1(LOG_TRACE, "Step %d", id);
			ret = rsp_ok(comm);
			break;
		case 't':
			DCC_LOG1(LOG_TRACE, "Stop %d", id);
			ret = rsp_ok(comm);
			break;
		default:
			DCC_LOG(LOG_TRACE, "Unsupported!");
			/* we don't have threads, empty replay */
			ret = rsp_empty(comm);
		}
		return ret;
	}

	return rsp_empty(comm);
}

static int rsp_detach(struct gdb_rspd * gdb, struct dmon_comm * comm)
{
	DCC_LOG(LOG_TRACE, "[DETACH]");

	if (gdb->stopped)
		__thinkos_resume_all();

	/* reply OK */
	rsp_ok(comm);

	dmon_exec(gdb->shell_task);

	return 0;
}

static int rsp_kill(struct gdb_rspd * gdb, struct dmon_comm * comm)
{
	DCC_LOG(LOG_TRACE, "[KILL]");

	if (gdb->stopped)
		__thinkos_resume_all();

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
		ret = rsp_thread_isalive(comm, pkt, len);
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
	case 'v':
		ret = rsp_v_packet(comm, pkt, len);
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
	int len;

	DCC_LOG(LOG_TRACE, "GDB start...");

	gdb->nonstop_mode = 0;
	gdb->noack_mode = 0;
	gdb->stopped = __thinkos_suspended();
	gdb->active_app = __thinkos_active();
	gdb->last_signal = TARGET_SIGNAL_0;
	breakpoint_clear_all();

	if (gdb->shell_task == NULL)
		gdb->shell_task = gdb_task;

	dmon_comm_connect(comm);

	DCC_LOG(LOG_TRACE, "Comm connected..");

	sigmask = (1 << DMON_THREAD_FAULT);
	sigmask |= (1 << DMON_THREAD_STEP);
	sigmask |= (1 << DMON_COMM_RCV);
	sigmask |= (1 << DMON_COMM_CTL);
	sigmask |= (1 << DMON_BREAKPOINT);
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

			case CTRL_Q:
				DCC_LOG(LOG_TRACE, "Ctrl+Q");
				dmon_reset();
				break;

			case CTRL_P:
				DCC_LOG(LOG_TRACE, "Ctrl+P");
				monitor_pause_all(comm);
				break;

			case CTRL_R:
				DCC_LOG(LOG_TRACE, "Ctrl+R");
				monitor_resume_all(comm);
				break;

			case CTRL_I:
				DCC_LOG(LOG_TRACE, "Ctrl+I");
				dmon_print_osinfo(comm);
				break;

			case CTRL_N:
				DCC_LOG(LOG_TRACE, "Ctrl+N");
				thread_id = __thinkos_thread_getnext(thread_id);
				if (thread_id == - 1)
					thread_id = __thinkos_thread_getnext(thread_id);
				dmprintf(comm, "Current thread = %d\r\n", thread_id);
				dmon_print_thread(comm, thread_id);
				break;

			case CTRL_X:
				DCC_LOG(LOG_TRACE, "Ctrl+X");
				monitor_on_fault(comm);
				break;

			case CTRL_D:
				DCC_LOG(LOG_TRACE, "Ctrl+D");
				monitor_dump(comm);
				break;

			case CTRL_V:
				DCC_LOG(LOG_TRACE, "Ctrl+V");
				monitor_show_help(comm);
				break;

			case CTRL_Y:
				DCC_LOG(LOG_TRACE, "Ctrl+Y");
				dmon_print_stack_usage(comm);
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

