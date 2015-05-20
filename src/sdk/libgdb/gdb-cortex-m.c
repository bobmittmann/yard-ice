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
int uint2hex2hex(char * pkt, unsigned int val);

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
		BLK_RW + SZ_128K, 
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
#define THREAD_ID_IDLE (THINKOS_THREAD_IDLE + THREAD_ID_OFFS) 

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

bool thread_isalive(int gdb_thread_id)
{
	unsigned int thread_id = gdb_thread_id - THREAD_ID_OFFS;

	return __thinkos_thread_isalive(thread_id);
}

int thread_register_get(int gdb_thread_id, int reg, uint32_t * val)
{
	unsigned int thread_id = gdb_thread_id - THREAD_ID_OFFS;
	struct thinkos_context * ctx;
	uint32_t x;

	if (thread_id > THINKOS_THREADS_MAX) {
		DCC_LOG(LOG_ERROR, "Invalid thread!");
		return -1;
	}

	if (thread_id == THINKOS_THREAD_IDLE) {
		ctx = thinkos_rt.idle_ctx;
		DCC_LOG1(LOG_TRACE, "ThinkOS Idle thread, context=%08x!", ctx);
	} else if (__thinkos_thread_isfaulty(thread_id)) {
		if (thinkos_except_buf.thread_id != thread_id) {
			DCC_LOG(LOG_ERROR, "Invalid exception thread_id!");
			return -1;
		}
		ctx = &thinkos_except_buf.ctx;
	} else if (__thinkos_thread_ispaused(thread_id)) {
		ctx = thinkos_rt.ctx[thread_id];
		DCC_LOG2(LOG_TRACE, "ThinkOS thread=%d context=%08x!", thread_id, ctx);
		if (((uint32_t)ctx < 0x10000000) || ((uint32_t)ctx >= 0x30000000)) {
			DCC_LOG(LOG_ERROR, "Invalid context!");
			return -1;
		}
	} else {
		DCC_LOG(LOG_ERROR, "Invalid thread state!");
		return -1;
	}

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

int thread_register_set(unsigned int gdb_thread_id, int reg, uint32_t val)
{
	unsigned int thread_id = gdb_thread_id - THREAD_ID_OFFS;
	struct thinkos_context * ctx;

	if (thread_id > THINKOS_THREADS_MAX) {
		DCC_LOG(LOG_ERROR, "Invalid thread!");
		return -1;
	}

	if (thread_id == THINKOS_THREAD_IDLE) {
		ctx = thinkos_rt.idle_ctx;
		DCC_LOG1(LOG_TRACE, "ThinkOS Idle thread, context=%08x!", ctx);
		return 0;
	} 

	if (__thinkos_thread_isfaulty(thread_id)) {
		if (thinkos_except_buf.thread_id != thread_id) {
			DCC_LOG(LOG_ERROR, "Invalid exception thread_id!");
			return -1;
		}
		ctx = &thinkos_except_buf.ctx;
	} else if (__thinkos_thread_ispaused(thread_id)) {
		ctx = thinkos_rt.ctx[thread_id];
		DCC_LOG2(LOG_TRACE, "ThinkOS thread=%d context=%08x!", thread_id, ctx);
		if (((uint32_t)ctx < 0x10000000) || ((uint32_t)ctx >= 0x30000000)) {
			DCC_LOG(LOG_ERROR, "Invalid context!");
			return -1;
		}
	} else {
		DCC_LOG(LOG_ERROR, "Invalid thread state!");
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
		thinkos_rt.ctx[thread_id] = (struct thinkos_context *)val;
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

int thread_goto(unsigned int gdb_thread_id, uint32_t addr)
{
	unsigned int thread_id = gdb_thread_id - THREAD_ID_OFFS;
	struct thinkos_context * ctx;

	if (thread_id > THINKOS_THREADS_MAX)
		return -1;

	if (thinkos_except_buf.thread_id == thread_id) {
		ctx = &thinkos_except_buf.ctx;
	} else {
		ctx = thinkos_rt.ctx[thread_id];
		DCC_LOG2(LOG_TRACE, "ThinkOS thread=%d context=%08x!", thread_id, ctx);
		if (((uint32_t)ctx < 0x10000000) || ((uint32_t)ctx >= 0x30000000)) {
			DCC_LOG(LOG_ERROR, "Invalid context!");
			return -1;
		}
	}

	ctx->pc = addr;

	return 0;
}

int thread_step_req(unsigned int gdb_thread_id)
{
	unsigned int thread_id = gdb_thread_id - THREAD_ID_OFFS;
	struct thinkos_context * ctx;

	if (thread_id > THINKOS_THREADS_MAX)
		return -1;

	if (thinkos_except_buf.thread_id == thread_id) {
		ctx = &thinkos_except_buf.ctx;
	} else {
		ctx = thinkos_rt.ctx[thread_id];
		DCC_LOG2(LOG_TRACE, "ThinkOS thread=%d context=%08x!", thread_id, ctx);
		if (((uint32_t)ctx < 0x10000000) || ((uint32_t)ctx >= 0x30000000)) {
			DCC_LOG(LOG_ERROR, "Invalid context!");
			return -1;
		}
	}

	return dmon_thread_step(thread_id, false);
}

int thread_continue(unsigned int gdb_thread_id)
{
	unsigned int thread_id = gdb_thread_id - THREAD_ID_OFFS;

	if (thread_id > THINKOS_THREADS_MAX)
		return -1;

	return __thinkos_thread_resume(thread_id);
}

int thread_info(unsigned int gdb_thread_id, char * buf)
{
	unsigned int thread_id = gdb_thread_id - THREAD_ID_OFFS;
	struct thinkos_except * xcpt = &thinkos_except_buf;
	char * cp = buf;
	int oid;
	int type;
	bool tmw;
	int n;

	if (thread_id > THINKOS_THREADS_MAX) {
		DCC_LOG(LOG_ERROR, "Invalid thread!");
		return -1;
	}

	if (thread_id == THINKOS_THREAD_IDLE) {
		DCC_LOG(LOG_TRACE, "ThinkOS Idle thread");
	} else if (__thinkos_thread_isfaulty(thread_id)) {
		if (xcpt->thread_id != thread_id) {
			DCC_LOG(LOG_ERROR, "Invalid exception thread_id!");
			return -1;
		}
	} else if (__thinkos_thread_ispaused(thread_id)) {
		DCC_LOG1(LOG_TRACE, "ThinkOS thread=%d!", thread_id);
	} else {
		DCC_LOG(LOG_ERROR, "Invalid thread state!");
		return -1;
	}
	if (thread_id > THINKOS_THREADS_MAX)
		return 0;

	if (thread_id == THINKOS_THREAD_IDLE) {
		n = str2hex(cp, "IDLE");
	} else {
#if THINKOS_ENABLE_THREAD_INFO
		if (thinkos_rt.th_inf[thread_id] != NULL)
			n = str2hex(cp, thinkos_rt.th_inf[thread_id]->tag);
		else
			n = int2str2hex(cp, thread_id);
#else
		n = int2str2hex(cp, thread_id);
#endif
	}
	cp += n;
	cp += char2hex(cp, ' ');

	if (thread_id == THINKOS_THREAD_IDLE) {
		oid = THINKOS_WQ_READY;
		tmw = false;
	} else if (__thinkos_thread_isfaulty(thread_id)) {
		oid = THINKOS_WQ_FAULT;
		tmw = false;
	} else {
#if THINKOS_ENABLE_THREAD_STAT
		oid = thinkos_rt.th_stat[thread_id] >> 1;
		tmw = thinkos_rt.th_stat[thread_id] & 1;
#else
		oid = THINKOS_WQ_READY; /* FIXME */
		tmw = (thinkos_rt.wq_clock & (1 << thread_id)) ? true : false;
#endif
	}
	if (tmw) {
		if (oid == THINKOS_WQ_READY) {
			cp += str2hex(cp, "time wait");
		} else {
			cp += str2hex(cp, "timedwait on ");
		}
	} else {
		if (oid == THINKOS_WQ_FAULT) {
			uint32_t bfsr;
			uint32_t ufsr;
			uint32_t mmfsr;

			switch (xcpt->type) {
			case CM3_EXCEPT_HARD_FAULT:
				cp += str2hex(cp, "Hard Fault ");
				break;
			case CM3_EXCEPT_MEM_MANAGE:
				cp += str2hex(cp, "Memory Fault ");
				mmfsr = SCB_CFSR_MMFSR_GET(CM3_SCB->cfsr);
				cp += str2hex(cp, "MMFSR=[");
				if (mmfsr & MMFSR_MMARVALID)
					cp += str2hex(cp, " MMARVALID");
				if (mmfsr & MMFSR_MLSPERR)
					cp += str2hex(cp, " MLSPERR");
				if (mmfsr & MMFSR_MSTKERR)
					cp += str2hex(cp, " MSTKERR");
				if (mmfsr & MMFSR_MUNSTKERR)
					cp += str2hex(cp, " MUNSTKERR");
				if (mmfsr & MMFSR_DACCVIOL)  
					cp += str2hex(cp, " DACCVIOL");
				if (mmfsr & MMFSR_IACCVIOL)  
					cp += str2hex(cp, " IACCVIOL");
				cp += str2hex(cp, " ] ");
				if (mmfsr & MMFSR_MMARVALID) {
					cp += str2hex(cp, " MMFAR=");
					cp += uint2hex2hex(cp, (uint32_t)CM3_SCB->mmfar);
				}
				break;
			case CM3_EXCEPT_BUS_FAULT:
				cp += str2hex(cp, " Bus Fault ");
				bfsr = SCB_CFSR_BFSR_GET(CM3_SCB->cfsr);
				cp += str2hex(cp, " BFSR=[");
				if (bfsr & BFSR_BFARVALID)  
					cp += str2hex(cp, " BFARVALID");
				if (bfsr & BFSR_LSPERR)
					cp += str2hex(cp, " LSPERR");
				if (bfsr & BFSR_STKERR)  
					cp += str2hex(cp, " STKERR");
				if (bfsr & BFSR_UNSTKERR)  
					cp += str2hex(cp, " UNSTKERR");
				if (bfsr & BFSR_IMPRECISERR)  
					cp += str2hex(cp, " IMPRECISERR");
				if (bfsr & BFSR_PRECISERR)
					cp += str2hex(cp, " PRECISERR");
				if (bfsr & BFSR_IBUSERR)  
					cp += str2hex(cp, " IBUSERR");
				cp += str2hex(cp, " ]");
				if (bfsr & BFSR_BFARVALID) {
					cp += str2hex(cp, " BFAR=");
					cp += uint2hex2hex(cp, (uint32_t)CM3_SCB->bfar);
				}
				break;
			case CM3_EXCEPT_USAGE_FAULT: 
				cp += str2hex(cp, " Usage Fault ");
				ufsr = SCB_CFSR_UFSR_GET(CM3_SCB->cfsr);
				cp += str2hex(cp, " UFSR=[");
				if (ufsr & UFSR_DIVBYZERO)  
					cp += str2hex(cp, " DIVBYZERO");
				if (ufsr & UFSR_UNALIGNED)  
					cp += str2hex(cp, " UNALIGNED");
				if (ufsr & UFSR_NOCP)  
					cp += str2hex(cp, " NOCP");
				if (ufsr & UFSR_INVPC)  
					cp += str2hex(cp, " INVPC");
				if (ufsr & UFSR_INVSTATE)  
					cp += str2hex(cp, " INVSTATE");
				if (ufsr & UFSR_UNDEFINSTR)  
					cp += str2hex(cp, " UNDEFINSTR");
				cp += str2hex(cp, " ]");
				break;
			}
		} else if (oid == THINKOS_WQ_READY) {
			cp += str2hex(cp, "ready");
		} else {
			cp += str2hex(cp, "wait on ");
		}
	}
	if ((oid != THINKOS_WQ_READY) && (oid != THINKOS_WQ_FAULT)) {
		type = thinkos_obj_type_get(oid);
		if (type == THINKOS_OBJ_PAUSED) {
			DCC_LOG1(LOG_ERROR, "thread %d is paused!!!", thread_id);
		}
		cp += str2hex(cp, thinkos_type_name_lut[type]);
		cp += char2hex(cp, '(');
		cp += int2str2hex(cp, oid);
		cp += char2hex(cp, ')');
	}
	
	return cp - buf;
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


int target_mem_write(uint32_t addr, const void * ptr, unsigned int len)
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

int target_mem_read(uint32_t addr, void * ptr, unsigned int len)
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

void target_halt(void)
{
	dmon_app_suspend();
}

int target_goto(uint32_t addr, int opt)
{
	return 0;
}
