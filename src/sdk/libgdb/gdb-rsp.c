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
#include <stdbool.h>
#include <sys/file.h>
#include <sys/null.h>
#include <sys/tty.h>

#include <sys/shell.h>

#include <sys/dcclog.h>

#include <thinkos.h>


#define	SIGTRAP	5	/* trace trap (not reset when caught) */

#ifndef RSP_BUFFER_LEN
#define RSP_BUFFER_LEN 512
#endif

#define ENABLE_LOG_PKT

struct dbg_bp {
	union {
		struct dbg_bp * next;
		struct {
			int16_t hw_id;
			uint8_t type;
			uint8_t active:1;
			uint8_t enabled:1;
			uint32_t addr;
			uint32_t size;
		};
	};
};

struct dbg_bp_ctrl {
	uint16_t cnt;
	struct dbg_bp ** lst;
	struct dbg_bp * free;
};

struct gdb_rspd {
	int8_t noack_mode;
	int8_t run_flag;
	int8_t con_flag;
	FILE * f;
	struct dbg_bp_ctrl bp_ctrl;
};

static const char hextab[] = { 
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

typedef enum {
     BP_WRITE   = 0, /* Common (write) HW watchpoint */
     BP_READ    = 1, /* Read    HW watchpoint */
     BP_ACCESS  = 2, /* Access (read or write) HW watchpoint */
     BP_EXECUTE = 3  /* Execute HW breakpoint */
} dbg_hw_bp_t;

struct mem_range {
	uint32_t base;
	uint32_t size;
};

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


int target_halt(int x)
{
	thinkos_sleep(100);
	return 0;
}

int target_halt_wait(int x)
{
	thinkos_sleep(100);
	return 0;
}

int target_connect(int x)
{
	return 0;
}

int target_status(void)
{
	return 0;
}

int target_mem_write(uint32_t addr, const void * ptr, int len)
{
	return 0;
}

int target_mem_read(uint32_t addr, void * ptr, int len)
{
	return 0;
}

int target_run(void)
{
	return 0;
}

int target_step(void)
{
	return 0;
}

int target_goto(uint32_t addr, int opt)
{
	return 0;
}

int target_register_get(int reg, uint32_t * val)
{
	return 0;
}

int target_register_set(int reg, uint32_t val)
{
	return 0;
}

/***********************************************************************
 Breakpoint management
 ***********************************************************************/

/* Breakpoint allocation data */
static struct dbg_bp dbg_bp_pool[DBG_BREAKPOINT_MAX];

/* Breakpoint management data */
/* This is a list of breakpoints ordered from the most to the least recently
   used.
   In most cases the target can support a limited number of active 
   breakpoints, in this case the top of this list will 
   correspond to the target's hardware ones.
 */
static struct dbg_bp * dbg_bp_list[DBG_BREAKPOINT_MAX];

static void dbg_bp_init(dbg_bp_ctrl_t * bpctl)
{
	struct dbg_bp * bp;
	int i;

	/* initialize the breakpoint poll.
	 The breakpoint poll consist of a linked list of free
	 breakpoint structures */
	bpctl->free = dbg_bp_pool;
	bpctl->lst = dbg_bp_list;
	bpctl->cnt = 0;

	bp = bpctl->free;
	for (i = 1; i < DBG_BREAKPOINT_MAX; i++) {
		bp->next = &dbg_bp_pool[i];
		bp = &dbg_bp_pool[i];
	}
	bp->next = NULL;

}

static struct dbg_bp * dbg_bp_new(dbg_bp_ctrl_t * bpctl, 
								  uint32_t addr, uint32_t size)
{
	struct dbg_bp * bp;

	if ((bp = bpctl->free) != NULL)
		bpctl->free = bp->next;

	bp->addr = addr;
	bp->size = size;
	bp->active = 0;

	/* insert at the list's tail,
	   if the list is full drop the last entry */

	if (bpctl->cnt < DBG_BREAKPOINT_MAX)
		bpctl->cnt++;

	bpctl->lst[bpctl->cnt - 1] = bp;
	
	DCC_LOG1(LOG_TRACE, "cnt=%d", bpctl->cnt);

	return bp;
}

static void dbg_bp_move_to_head(dbg_bp_ctrl_t * bpctl, struct dbg_bp * bp)
{
	struct dbg_bp * tmp = NULL;
	struct dbg_bp * prev = bp;
	int i;

	DCC_LOG1(LOG_TRACE, "bpctl->cnt=%d", bpctl->cnt);

	/* moving the list (shifting) */
	for (i = 0; i < bpctl->cnt; i++) {
		DCC_LOG1(LOG_TRACE, "i=%d", i);
		tmp = bpctl->lst[i];
		bpctl->lst[i] = prev;
		if (tmp == bp) {
			break;
		}
		prev = tmp;
	}
}

#define HW_BP_MAX 4

static int dbg_bp_delete(dbg_bp_ctrl_t * bpctl, struct dbg_bp * bp)
{
	int i;
	int n;

	/* look up for this breakpoint in the list */
	for (i = 0; i < bpctl->cnt; i++) {
		if (bp == bpctl->lst[i])
			break;
	}

	n = i;

	if (n == bpctl->cnt) {
		DCC_LOG(LOG_EXCEPT, "not in the list!!!");
		abort();
	}

	/* one less item in the list */
	bpctl->cnt--;

	/* moving the remaining items in the list one position to the front */
	for (i = n; i < bpctl->cnt; i++) {
		bpctl->lst[i] = bpctl->lst[i + 1];
	}

	if (bp->active) {
		/* clear the current breakpoint */
		DCC_LOG1(LOG_TRACE, "clearing active BP: %d", bp->hw_id);
	//	ice_bp_clr(ice, bp->hw_id);

		/* activate the next if not disabled */
		if (bpctl->cnt >= HW_BP_MAX) {
			struct dbg_bp * nxt;
			uint32_t id;

			nxt = bpctl->lst[HW_BP_MAX - 1];
			if (nxt->enabled) {
				DCC_LOG(LOG_TRACE, "seting next BP on list");
//				if (ice_bp_set(ice, nxt->addr, nxt->size, &id) < 0) {
//					DCC_LOG(LOG_EXCEPT, "ice_bp_set() error!");
//				}
				id = 1;
				nxt->active = 1;
				nxt->hw_id = id;
			}
		}
	} 

	/* release memory */
	bp->next = bpctl->free;
	bpctl->free = bp;

	return 0;
}

#if 0
static int dbg_bp_enable_all(dbg_bp_ctrl_t * bpctl)
{
	struct dbg_bp * bp = NULL;
	uint32_t id;
	int ret = 0;
	int n;
	int i;

	/* activate the breakpoints */
	n = (bpctl->cnt > HW_BP_MAX) ? HW_BP_MAX: bpctl->cnt;
	for (i = 0; i < n; i++) {
		bp = bpctl->lst[i];
		if (!bp->active) {
			DCC_LOG(LOG_TRACE, "activating breakpoint...");
//			if ((ret = ice_bp_set(ice, bp->addr, bp->size, &id)) < 0) {
//				DCC_LOG(LOG_ERROR, "ice_bp_set() error!");
//				return ret;
//			}
			bp->active = 1;
			bp->hw_id = id;
		}
		bp->enabled = 1;
	}
	/* enable the remaining */
	for (i = n; i < bpctl->cnt; i++) {
		bp = bpctl->lst[i];
		bp->enabled = 1;
	}

	return ret;
}

static int dbg_bp_disable_all(dbg_bp_ctrl_t * bpctl)
{
	struct dbg_bp * bp = NULL;
	int ret = 0;
	int i;

	/* disable all the breakpoints */
	for (i = 0; i < bpctl->cnt; i++) {
		bp = bpctl->lst[i];
		if (bp->active) {
			/* deactivate the breakpoints */
			DCC_LOG1(LOG_TRACE, "deactivating breakpoint %d", i);
//			if ((ret = ice_bp_clr(ice, bp->hw_id)) < 0) {
//				DCC_LOG(LOG_ERROR, "ice_bp_clr() error!");
//				return ret;
//			}
			bp->active = 0;
			bp->hw_id = -1;
		}
		bp->enabled = 0;
	}

	return ret;
}
#endif

static int dbg_bp_activate(dbg_bp_ctrl_t * bpctl, struct dbg_bp * bp)
{
	uint32_t id;

	if (bp->active)
		return 0;

	if (bpctl->cnt > HW_BP_MAX) {
		struct dbg_bp * nxt;
		/* we are short of hardware breakpoints,
		   deactivate the least recently active */
		nxt = bpctl->lst[HW_BP_MAX];
		if (nxt->active) {
			id = nxt->hw_id;
			nxt->active = 0;
			DCC_LOG1(LOG_TRACE, "deactivating breakpoint %d", id);
//			ice_bp_clr(ice, id);
		}
	} 

	DCC_LOG2(LOG_TRACE, "activating breakpoint: 0x%08x(%d) ...", 
			bp->addr, bp->size);

	id = 1;
//	if (ice_bp_set(ice, bp->addr, bp->size, &id) < 0) {
//		DCC_LOG(LOG_ERROR, "ice_bp_set() error!");
//		bp->active = 0;
//		bp->hw_id = -1;
//		return -1;
//	} 

	DCC_LOG1(LOG_TRACE, "HW breakpoint id=%d", id);
	bp->active = 1;
	bp->hw_id = id;
	return 0;
}

#if 0
static int dbg_bp_deactivate(dbg_bp_ctrl_t * bpctl, struct dbg_bp * bp)
{
	uint32_t id;

	if (!bp->active)
		return 0;

	id = bp->hw_id;
	bp->active = 0;
	DCC_LOG1(LOG_TRACE, "deactivating breakpoint %d", id);
	(void)id;
//	ice_bp_clr(ice, id);

	return 0;
}

static int dbg_bp_activate_all(dbg_bp_ctrl_t * bpctl)
{
	struct dbg_bp * bp = NULL;
	uint32_t id;
	int ret = 0;
	int n;
	int i;

	n = (bpctl->cnt > HW_BP_MAX) ? HW_BP_MAX: bpctl->cnt;
	for (i = 0; i < n; i++) {
		bp = bpctl->lst[i];
		if (bp->enabled && !bp->active) {
			DCC_LOG(LOG_TRACE, "activating breakpoint...");
//			if ((ret = ice_bp_set(ice, bp->addr, bp->size, &id)) < 0) {
//				DCC_LOG(LOG_ERROR, "ice_bp_set() error!");
//				return ret;
//			}
			bp->active = 1;
			bp->hw_id = id;
		}
	}

	return ret;
}

static int dbg_bp_deactivate_all(dbg_bp_ctrl_t * bpctl)
{
	struct dbg_bp * bp = NULL;
	int ret = 0;
	int n;
	int i;

	n = (bpctl->cnt > HW_BP_MAX) ? HW_BP_MAX: bpctl->cnt;
	for (i = 0; i < n; i++) {
		bp = bpctl->lst[i];
		if (bp->active) {
			/* deactivate the breakpoints */
			DCC_LOG1(LOG_TRACE, "deactivating breakpoint %d", i);
//			if ((ret = ice_bp_clr(ice, bp->hw_id)) < 0) {
//				DCC_LOG(LOG_ERROR, "ice_bp_clr() error!");
//				return ret;
//			}
			bp->active = 0;
			bp->hw_id = -1;
		}
	}

	return ret;
}

static struct dbg_bp * dbg_bp_get_next(dbg_bp_ctrl_t * bpctl, 
									   struct dbg_bp * bp)
{
	int i;

	if (bpctl->cnt == 0)
		return NULL;

	/* look up for this breakpoint in the list */
	for (i = 0; i < bpctl->cnt; i++) {
		if (bp == bpctl->lst[i]) {
			/* the breakpoint is in the list, return the next item */
			i++;
			if (i < bpctl->cnt)
				return bpctl->lst[i];
			return NULL;
		}
	}

	return bpctl->lst[0];
}
#endif

static struct dbg_bp * dbg_bp_lookup(dbg_bp_ctrl_t * bpctl, 
									 uint32_t addr, uint32_t size)
{
	struct dbg_bp * bp;
	int pos;

	/* check if a breakpoint with the same address and size 
	 already exist */
	for (pos = 0; pos < bpctl->cnt; pos++) {
		bp = bpctl->lst[pos];
		if ((bp->addr == addr) && (bp->size == size)) {
			return bp;
		}
	}

	return NULL;
}

#define BP_DEFSZ 2

int target_breakpoint_set(struct gdb_rspd * gdb, 
						  uint32_t addr, uint32_t size)
{
	struct dbg_bp * bp = NULL;

	/* use default size if zero */
	size = (size == 0) ? BP_DEFSZ : size;

	DCC_LOG2(LOG_TRACE, "addr=0x%08x size=%d", addr, size);

	/* check if a breakpoint with the same address and size 
	 already exist */
	if ((bp = dbg_bp_lookup(&gdb->bp_ctrl, addr, size)) == NULL) {
		/* no breakpoint exist, create a new one */
		if ((bp = dbg_bp_new(&gdb->bp_ctrl, addr, size)) == NULL) {
			DCC_LOG(LOG_WARNING, "breakpoint allocation fail!");
			return -1;
		};
		DCC_LOG1(LOG_TRACE, "new breakpoint: %p", bp);
	}
	
	/* move the to the head */
	dbg_bp_move_to_head(&gdb->bp_ctrl, bp);

	/* enable the breakpoint */
	bp->enabled = 1;

	if (!bp->active) {
		/* activate the hardware break point */
		dbg_bp_activate(&gdb->bp_ctrl, bp);
	} 

	return 0;
}

int target_breakpoint_clear(struct gdb_rspd * gdb,
							uint32_t addr, uint32_t size)
{
	struct dbg_bp * bp;
	int ret;

	size = (size == 0) ? BP_DEFSZ : size;

	DCC_LOG2(LOG_TRACE, "addr=0x%08x size=%d", addr, size);

	/* check if a breakpoint with this address and size exists */
	if ((bp = dbg_bp_lookup(&gdb->bp_ctrl, addr, size)) == NULL) {
		DCC_LOG(LOG_WARNING, "breakpoint not found!");
		ret = -1;
	} else {
		ret = dbg_bp_delete(&gdb->bp_ctrl, bp);
	}

	return ret;
}

/*
 * Common response packets
 */

static inline int rsp_ack(FILE * f)
{
	return fwrite("+", 1, 1, f);
}

#if 0
static int rsp_nack(FILE * f)
{
	return fwrite("-", 1, 1, f);
}
#endif

static inline int rsp_ok(FILE * f)
{
	return fwrite("$OK#9a", 6, 1, f);
}

static int rsp_empty(FILE * f)
{
	return fwrite("$#00", 4, 1, f);
}

static int rsp_error(FILE * f, int err)
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

	return fwrite(pkt, 7, 1, f);
}

static int rsp_msg(FILE * f, char * pkt, const char * s)
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

	return fwrite(pkt, n, 1, f);
}

static int rsp_send_pkt(FILE * f, char * pkt_buf, 
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

	return fwrite(pkt_buf, n, 1, f);
}

static int rsp_signal(FILE * f, char * pkt, int sig)
{
	int sum;;

	pkt[0] = '$';
	pkt[1] = sum = 'S';
	sum += pkt[2] = hextab[((sig >> 4) & 0xf)];
	sum += pkt[3] = hextab[(sig & 0xf)];
	pkt[4] = '#';
	pkt[5] = hextab[((sum >> 4) & 0xf)];
	pkt[6] = hextab[sum & 0xf];

	return fwrite(pkt, 7, 1, f);
}

static int rsp_break_signal(FILE * f, char * pkt)
{
//	int state;

	DCC_LOG(LOG_TRACE, "break received, stopping...");

	if (target_halt(0) < 0) {
		return rsp_error(f, 1);
	}

	return 0;
#if 0
	if ((state = target_halt_wait(500)) == ERR_TIMEOUT) {
		DCC_LOG(LOG_TRACE, "timeout...");
	}

	if (state == DBG_ST_HALTED) {
		DCC_LOG(LOG_TRACE, "halted");
		return rsp_signal(f, pkt, SIGTRAP);
	}

	return rsp_msg(f, pkt, "YARD-ICE: target_halt failed!");
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

static int  rsp_offsets(FILE * f, unsigned int text,
							   unsigned int data, unsigned int bss)
{
	char s[128];
	int n;

	DCC_LOG3(LOG_TRACE, "text=%08x data=%08x bss=%08x", text, data, bss);

	/* FIXME: max id = 15 by the time */
	n = sprintf(s, "+$Text=%x;Data=%x;Bss=%x#", text, data, bss);
	rsp_fixup_sum(s + 2);
	return fwrite(s, n + 2, 1, f);
}
#endif

int rsp_thread_get_first(FILE * f)
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

	return fwrite(pkt, 6, 1, f);
}

int rsp_thread_get_next(FILE * f)
{
	char pkt[32];
	int sum;;

	DCC_LOG(LOG_TRACE, ".");

	pkt[0] = '$';
	pkt[1] = sum = 'l';
	pkt[2] = '#';
	pkt[3] = hextab[((sum >> 4) & 0xf)];
	pkt[4] = hextab[sum & 0xf];

	return fwrite(pkt, 5, 1, f);
}

static int rsp_last_signal(struct gdb_rspd * gdb, FILE * f, 
						   char * pkt, int len)
{
	int state;

	state = target_status();
	if (state < DBG_ST_CONNECTED) {
		DCC_LOG(LOG_WARNING, "target not connected!");
		return rsp_error(f, state);
	}

	if (state != DBG_ST_HALTED) {
		DCC_LOG(LOG_TRACE, "running");

		if ((state = target_halt(0)) < 0) {
			DCC_LOG(LOG_WARNING, "target_halt() failed!");
			rsp_msg(f, pkt, "YARD-ICE: halt fail\n");
			return rsp_error(f, 1);
		}

		if ((state = target_halt_wait(500)) == ERR_TIMEOUT) {
			DCC_LOG(LOG_TRACE, "timeout...");
			rsp_msg(f, pkt, "YARD-ICE: target_halt failed!");
			return rsp_error(f, 1);
		}
	 } 
	
	if (state == DBG_ST_HALTED) {
		DCC_LOG(LOG_TRACE, "halted");
		thinkos_flag_clr(gdb->run_flag);
		return rsp_signal(f, pkt, SIGTRAP);
	}

	switch (state) {
	case DBG_ST_ERROR:
		rsp_msg(f, pkt, "YARD-ICE: error state\n");
		break;
	case DBG_ST_OUTOFSYNC:
		DCC_LOG(LOG_TRACE, "out of sync");
		rsp_msg(f, pkt, "YARD-ICE: Out of sync\n");
		break;
	case DBG_ST_BUSY:
		DCC_LOG(LOG_TRACE, "busy...");
		rsp_msg(f, pkt, "YARD-ICE: busy ... \n");
		break;
	case DBG_ST_UNDEF:
		rsp_msg(f, pkt, "YARD-ICE: undefined state\n");
		break;
	case DBG_ST_UNCONNECTED:
		DCC_LOG(LOG_TRACE, "unconnected");
		rsp_msg(f, pkt, "YARD-ICE: unconnected ?\n");
		break;
	case DBG_ST_CONNECTED:
		DCC_LOG(LOG_TRACE, "connected");
		rsp_msg(f, pkt, "YARD-ICE: connected (busy)\n");
		break;
	case DBG_ST_RUNNING:
		DCC_LOG(LOG_TRACE, "running");
		rsp_msg(f, pkt, "YARD-ICE: running\n");
		thinkos_flag_set(gdb->run_flag);
		break;
	default:
		DCC_LOG1(LOG_WARNING, "unknown state: %d", state);
		rsp_msg(f, pkt, "YARD-ICE: unknown state, bailing out!\n");
		return -1;
	}

	return rsp_error(f, 1);
}

#if 0
static int rsp_thread_id(FILE * f, int id)
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

	return fwrite(pkt, n, 1, f);
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


int rsp_write(FILE * f, const void * buf, int len)
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

	return fwrite(pkt, n, 1, f);
}

int rsp_read(FILE * f, const void * buf, int len)
{
	DCC_LOG1(LOG_TRACE, "len=%d", len);

	return 0;
}

static const struct fileop rsp_fileop = {
	.write = (void *)rsp_write,
	.read = (void *)rsp_read,
	.close = (void *)null_close,
	.flush = (void *)null_flush,
};

static struct file * rsp_fopen(FILE * f)
{
	return file_alloc(f, &rsp_fileop);
}

static int rsp_fclose(struct file * f)
{
	fclose(f);
	return file_free(f);
}

int rsp_cmd(FILE * f, char * pkt, int len)
{
	char * cp = pkt + 6;
	char * s = pkt;
	FILE * fr;
	int ret;
	int c;
	int i;

	if ((fr = rsp_fopen(f)) == NULL) {
		DCC_LOG(LOG_ERROR, "rsp_fopen() failed!");
		return rsp_error(f, -1);
	}

	len -= 6;
	DCC_LOG1(LOG_TRACE, "len=%d", len);

	for (i = 0; i < (len / 2); i++) {
		c = tochar(cp);
		cp += 2;
		s[i] = c;
	}
	s[i] = '\0';

	if ((ret = cmd_exec(fr, NULL, s)) < 0) {
		DCC_LOG1(LOG_ERROR, "shell_exec(): %d", ret);
		rsp_fclose(fr);
		return rsp_error(f, -ret);
	}

	rsp_fclose(fr);
	return rsp_ok(f);
}

static int rsp_query(struct gdb_rspd * gdb, FILE * f,
					 char * pkt, int len)
{
	char s[128];
	int n;

	pkt[len] = '\0';

	if (strstr(pkt, "qRcmd,")) {
		DCC_LOG(LOG_TRACE, "qRcmd");
		return rsp_cmd(f, pkt, len);
	}

	if (strstr(pkt, "qCRC:")) {
		DCC_LOG(LOG_TRACE, "qCRC (not implemented!)");
		return rsp_empty(f);
	}

	if (strstr(pkt, "qC")) {
		DCC_LOG(LOG_TRACE, "qC (not implemented!)");
		return rsp_empty(f);
	}

	if (strstr(pkt, "qAttached")) {
		DCC_LOG(LOG_TRACE, "qAttached (not implemented!)");
		return rsp_empty(f);
	}

	if (strstr(pkt, "qOffsets")) {
		DCC_LOG(LOG_TRACE, "qOffsets (not implemented!)");
		return rsp_empty(f);
	}

	if (strstr(pkt, "qSymbol")) {
		DCC_LOG(LOG_TRACE, "qSymbol (not implemented!)");
		return rsp_empty(f);
	}

	if (strstr(pkt, "qSupported")) {
		DCC_LOG(LOG_TRACE, "qSupported");
		n = sprintf(s, "PacketSize=%x;"
					  "qXfer:features:read-;"
					  "QStartNoAckMode+",
					  RSP_BUFFER_LEN - 1);
		return rsp_send_pkt(f, pkt, s, n);
	}

	if (strstr(pkt, "qfThreadInfo")) {
		DCC_LOG(LOG_TRACE, "qfThreadInfo");
		/* First Thread Info */
		return rsp_thread_get_first(f);
	}

	if (strstr(pkt, "qsThreadInfo")) {
		DCC_LOG(LOG_TRACE, "qsThreadInfo");
		/* Sequence Thread Info */
		return rsp_thread_get_next(f);
	}

			/* Get thread info from RTOS */
			/* qThreadExtraInfo */
	if (strstr(pkt, "qXfer:memory-map:read::")) {
		DCC_LOG(LOG_TRACE, "qXfer:memory-map:read::");
		return rsp_empty(f);
	}

	if (strstr(pkt, "qXfer:features:read:")) {
		DCC_LOG(LOG_TRACE, "qXfer:features:read:");
		return rsp_empty(f);
	}

	if (strstr(pkt, "qTStatus")) {
		DCC_LOG(LOG_TRACE, "qTStatus");
		return rsp_empty(f);
	}


	if (strstr(pkt, "QStartNoAckMode")) {
		DCC_LOG(LOG_TRACE, "QStartNoAckMode");
		gdb->noack_mode = 1;
		return rsp_ok(f);
	}

	DCC_LOG(LOG_TRACE, "unsupported query");

	return rsp_empty(f);


}

static int rsp_all_registers_get(FILE * f, char * pkt, int len)
{
	unsigned int val = 0;
	int sum = 0;
	int n;
	int r;

	DCC_LOG(LOG_TRACE, ".");

	pkt[0] = '$';
	n = 1;

	/* all integer registers */
	for (r = 0; r < 16; r++) {
		target_register_get(r, &val);
	
		DCC_LOG2(LOG_TRACE, "R%d = 0x%08x", r, val);

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

	return fwrite(pkt, n, 1, f);
}

static int rsp_all_registers_set(FILE * f, char * pkt, int len)
{
	DCC_LOG(LOG_WARNING, "not implemented");

	return rsp_empty(f);
}

static int rsp_register_get(FILE * f, char * pkt, int len)
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
		return rsp_empty(f);
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

	return fwrite(pkt, n, 1, f);
}

static int rsp_register_set(FILE * f, char * pkt, int len)
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
		return rsp_empty(f);
	}

	/* cpsr */
	if (reg == 25) {
		reg = 16;
	}

	if (reg > 16) {
		DCC_LOG(LOG_TRACE, "CPSR");
		return rsp_error(f, 2);
	}

	DCC_LOG2(LOG_TRACE, "reg=%d val=0x%08x", reg, val);

	if (target_register_set(reg, val) < 0) {
		DCC_LOG(LOG_WARNING, "target_register_set() failed!");
		return rsp_error(f, 2);
	}

	return rsp_ok(f);
}

static int rsp_memory_read(FILE * f, char * pkt, int len)
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

	DCC_LOG2(LOG_TRACE, "addr=0x%08x size=%d", addr, size);

	max = (RSP_BUFFER_LEN - 5) >> 1;

	if (size > max)
		size = max;

	if ((ret = target_mem_read(addr, buf, size)) < 0) {
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

		DCC_LOG2(LOG_TRACE, "addr=%08x size=%d", addr, size);

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

	return fwrite(pkt, n, 1, f);
}

static int rsp_memory_write(FILE * f, char * pkt, int len)
{
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
	return rsp_ok(f);
}

static int rsp_breakpoint_insert(FILE * f, struct gdb_rspd * gdb,
								 char * pkt, int len)
{
	unsigned int addr;
	unsigned int size;
	char * cp;

	if (pkt[1] != '0')
		return rsp_ok(f);

	cp = &pkt[3];
	addr = strtoul(cp, &cp, 16);
	cp++;
	size = strtoul(cp, NULL, 16);

	DCC_LOG2(LOG_TRACE, "addr=0x%08x size=%d", addr, size);

	target_breakpoint_set(gdb, addr, size);

	DCC_LOG(LOG_TRACE, "target_breakpoint_set() done.");

	return rsp_ok(f);
}

static int rsp_breakpoint_remove(FILE * f, struct gdb_rspd * gdb,
								 char * pkt, int len)
{
	unsigned int addr;
	unsigned int size;
	char * cp;

	if (pkt[1] != '0')
		return rsp_ok(f);

	cp = &pkt[3];
	addr = strtoul(cp, &cp, 16);
	cp++;
	size = strtoul(cp, NULL, 16);

	DCC_LOG2(LOG_TRACE, "addr=%08x size=%d", addr, size);

	target_breakpoint_clear(gdb, addr, size);

	return rsp_ok(f);
}

static int rsp_step(FILE * f, char * pkt, int len)
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
		return rsp_error(f, 1);
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

	return fwrite(pkt, n, 1, f);
}

static int rsp_continue(struct gdb_rspd * gdb, FILE * f, 
						char * pkt, int len)
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
		return rsp_error(f, 1);
	} 

	/* signal that we are now running */
	thinkos_flag_set(gdb->run_flag);

	return fwrite("+", 1, 1, f);
}

static int rsp_h_packet(FILE * f, char * pkt, int len)
{
	int state;
	int ret;

	state = target_status();

	if (state != DBG_ST_HALTED) {
		DCC_LOG(LOG_TRACE, "running");
		if (target_halt(0) < 0) {
			DCC_LOG(LOG_WARNING, "target_halt() failed!");
			return rsp_error(f, 1);
		}

		if ((state = target_halt_wait(500)) == ERR_TIMEOUT) {
			DCC_LOG(LOG_TRACE, "timeout...");
			rsp_msg(f, pkt, "YARD-ICE: target_halt failed!");
			return rsp_error(f, 1);
		}
	}

	/* set thread for subsequent operations */
	switch (pkt[1]) {
	case 'c':
		DCC_LOG(LOG_TRACE, "Hc");
		ret = rsp_ok(f);
		break;
	case 'g':
		DCC_LOG1(LOG_TRACE, "Hg%c", pkt[2]);
		ret = rsp_ok(f);
		break;
	default:
		DCC_LOG(LOG_TRACE, "H.");
		/* we don't have threads, empty replay */
		ret = rsp_empty(f);
	}

	(void)ret;
	return 0;
}


static int rsp_v_packet(FILE * f, char * pkt, int len)
{
	return rsp_empty(f);
}

static int rsp_detach(FILE * f)
{
	DCC_LOG(LOG_TRACE, "[DETACH]");
	/* detach - just reply OK */
	return rsp_ok(f);
}

static int rsp_memory_write_bin(FILE * f, char * pkt, int len)
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
		return rsp_error(f, 1);
	}

	DCC_LOG2(LOG_TRACE, "addr=%08x size=%d", addr, size);
	return rsp_ok(f);
}

static int rsp_pkt_recv(FILE * f, char * pkt, int max)
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
		if ((n = fread(&pkt[pos], rem, 1, f)) < 0) {
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

int __attribute__((noreturn)) gdb_brk_task(struct gdb_rspd * gdb)
{
	FILE * f;
	char pkt[32];
	int sum;;
	int sig = 5;
	int state;

	for (;;) {
		/* wait for a connection */
		DCC_LOG(LOG_TRACE, "waiting connect...");
		thinkos_flag_wait(gdb->con_flag);

		/* wait for a 'target run' indication */
		DCC_LOG(LOG_TRACE, "waiting run...");

		/* wait for a target run */
		thinkos_flag_wait(gdb->run_flag);

		DCC_LOG(LOG_TRACE, "waiting halt...");

		while ((state = target_halt_wait(5000)) == ERR_TIMEOUT) {
			DCC_LOG(LOG_TRACE, "waiting...");
		}

		if (state == DBG_ST_HALTED) {
			DCC_LOG(LOG_TRACE, "halted");

			thinkos_flag_clr(gdb->run_flag);

			if ((f = gdb->f) != NULL) {
				pkt[0] = '$';
				pkt[1] = sum = 'S';
				sum += pkt[2] = hextab[((sig >> 4) & 0xf)];
				sum += pkt[3] = hextab[(sig & 0xf)];
				pkt[4] = '#';
				pkt[5] = hextab[((sum >> 4) & 0xf)];
				pkt[6] = hextab[sum & 0xf];
				fwrite(pkt, 8, 1, f);
			}
		}
	}
}

void gdb_task(FILE * f, struct gdb_rspd * gdb)
{
	char buf[RSP_BUFFER_LEN];
	char * pkt = buf;
	int len;
	int ret;
	int c;

	for (;;) {
		if ((len = fread(buf, 1, 1, f)) <= 0) {
			DCC_LOG1(LOG_WARNING, "tcp_recv(): %d", len);
			break;
		}

		c = buf[0];

		if (c == '+') {
			DCC_LOG(LOG_TRACE, "[ACK]");
			continue;
		}

		if (c == '-') {
			DCC_LOG(LOG_TRACE, "[NACK]");
			continue;

		}

		if (c == 0x03) {
			DCC_LOG(LOG_TRACE, "[BREAK]");
			if (rsp_break_signal(f, pkt) < 0) {
				DCC_LOG(LOG_WARNING, "rsp_break_signal() failed!");
				break;
			}
			continue;
		}

		if (c != '$') {
			DCC_LOG1(LOG_WARNING, "invalid: %02x", c);
			continue;
		}

		if ((len = rsp_pkt_recv(f, pkt, RSP_BUFFER_LEN)) <= 0) {
			DCC_LOG1(LOG_WARNING, "rsp_pkt_recv(): %d", len);
			break;
		}

		if (!gdb->noack_mode)
			rsp_ack(f);

#ifdef ENABLE_LOG_PKT
		log_pkt(pkt, len);
#endif

		switch (pkt[0]) {
		case 'H':
			ret = rsp_h_packet(f, pkt, len);
			break;
		case 'q':
		case 'Q':
			ret = rsp_query(gdb, f, pkt, len);
			break; 
		case 'g':
			ret = rsp_all_registers_get(f, pkt, len);
			break;
		case 'G':
			ret = rsp_all_registers_set(f, pkt, len);
			break;
		case 'p':
			ret = rsp_register_get(f, pkt, len);
			break;
		case 'P':
			ret = rsp_register_set(f, pkt, len);
			break;
		case 'm':
			ret = rsp_memory_read(f, pkt, len);
			break;
		case 'M':
			ret = rsp_memory_write(f, pkt, len);
			break;
		case 'z':
			/* remove breakpoint */
			ret = rsp_breakpoint_remove(f, gdb, pkt, len);
			break;
		case 'Z':
			/* insert breakpoint */
			ret = rsp_breakpoint_insert(f, gdb, pkt, len);
			break;
		case '?':
			ret = rsp_last_signal(gdb, f, pkt, len);
			break;
		case 'i':
		case 's':
			ret = rsp_step(f, pkt, len);
			break;
		case 'c':
			/* continue */
			ret = rsp_continue(gdb, f, pkt, len);
			break;
		case 'v':
			ret = rsp_v_packet(f, pkt, len);
			break;
		case 'D':
			ret = rsp_detach(f);
			break;
		case 'X':
			ret = rsp_memory_write_bin(f, pkt, len);
			break;
		case 'k':
			/*				if (extended_protocol != 0)
							break; */
			/* kill */
			ret = rsp_ok(f);
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
			ret = rsp_empty(f);
			break;
		}

		if (ret < 0)
			break;
	}

}

uint32_t gdb_brk_stack[(RSP_BUFFER_LEN / 3) + 128];
struct gdb_rspd gdb_rspd;

const struct thinkos_thread_inf gdb_brk_inf = {
	.stack_ptr = gdb_brk_stack, 
	.stack_size = sizeof(gdb_brk_stack),
	.priority = 32,
	.thread_id = 32,
	.paused = false,
	.tag = "GDB_BRK"
};

int gdb_rspd_start(FILE * f)
{  
	if (isfatty(f)) {
		f = ftty_lowlevel(f);
	}
	gdb_rspd.f = f;
	gdb_rspd.run_flag = thinkos_flag_alloc();
	gdb_rspd.con_flag = thinkos_flag_alloc();
	dbg_bp_init(&gdb_rspd.bp_ctrl);

	thinkos_thread_create_inf((void *)gdb_brk_task, (void *)&gdb_rspd, 
							  &gdb_brk_inf);


	gdb_task(f, &gdb_rspd);

	return 0;
}

