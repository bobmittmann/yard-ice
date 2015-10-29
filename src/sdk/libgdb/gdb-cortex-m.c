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

#include <gdb.h>

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

	if (thread_id == THREAD_ID_ANY) {
		id = THINKOS_THREAD_IDLE;
		DCC_LOG1(LOG_MSG, "thread %d (IDLE)", id);
		return THREAD_ID_IDLE;
	} 
	
	if (thread_id == THREAD_ID_IDLE)
		id = __thinkos_thread_getnext(-1);
	else
		id = __thinkos_thread_getnext(thread_id - THREAD_ID_OFFS);

	if (id < 0) {
		DCC_LOG(LOG_MSG, "no more threads.");
		return -1;
	}

	DCC_LOG1(LOG_MSG, "thread %d", id);
	id += THREAD_ID_OFFS;

	return id;
}

int thread_active(void)
{
	if (thinkos_rt.active == THINKOS_THREAD_IDLE)
		DCC_LOG(LOG_TRACE, "IDLE");
	return thinkos_rt.active + THREAD_ID_OFFS;
}

int thread_step_id(void)
{
	if (thinkos_rt.step_id == -1) {
		DCC_LOG(LOG_WARNING, "invalid step thread!");
		return thinkos_rt.active + THREAD_ID_OFFS;
	}

	return thinkos_rt.step_id + THREAD_ID_OFFS;
}

int thread_break_id(void)
{
	if (thinkos_rt.break_id == -1) {
		DCC_LOG(LOG_WARNING, "invalid break thread!");
		if (thinkos_rt.xcpt_ipsr == 0) {
			DCC_LOG(LOG_WARNING, "No exception at IRQ!");
			return thinkos_rt.active + THREAD_ID_OFFS;
		}
		DCC_LOG1(LOG_WARNING, "Exception at IRQ:%d!", 
				 thinkos_rt.xcpt_ipsr - 16);
		return THINKOS_THREAD_VOID + THREAD_ID_OFFS;
	}

	DCC_LOG1(LOG_INFO, "break_id=%d", thinkos_rt.break_id);

	return thinkos_rt.break_id + THREAD_ID_OFFS;
}


int thread_any(void)
{
	int thread_id;

	if ((unsigned int)thinkos_rt.active < THINKOS_THREADS_MAX) {
		DCC_LOG1(LOG_TRACE, "active=%d", thinkos_rt.active);
		return thinkos_rt.active + THREAD_ID_OFFS;
	}

	if (thinkos_rt.active == THINKOS_THREAD_IDLE)
		DCC_LOG(LOG_MSG, "IDLE thread!");
	else if (thinkos_rt.active == THINKOS_THREAD_VOID)
		DCC_LOG(LOG_MSG, "VOID thread!");
	else
		DCC_LOG1(LOG_MSG, "active=%d is invalid!", thinkos_rt.active);

	/* Active thread is IDLE or invalid, try to get the first 
	   initialized thread. */
	thread_id = __thinkos_thread_getnext(-1);
	
	if (thread_id < 0) {
		DCC_LOG(LOG_WARNING, "No threads!");
		thread_id = THINKOS_THREAD_IDLE;
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

	if (thread_id > THINKOS_THREAD_VOID) {
		DCC_LOG(LOG_ERROR, "Invalid thread!");
		return -1;
	}

	if (thread_id == THINKOS_THREAD_IDLE) {
		ctx = thinkos_rt.idle_ctx;
		DCC_LOG1(LOG_INFO, "ThinkOS Idle thread, context=%08x!", ctx);
	} else if (thread_id == THINKOS_THREAD_VOID) {
		ctx = thinkos_rt.void_ctx;
		DCC_LOG1(LOG_INFO, "ThinkOS Void thread, context=%08x!", ctx);
	} else if (__thinkos_thread_isfaulty(thread_id)) {
		if (thinkos_except_buf.thread_id != thread_id) {
			DCC_LOG(LOG_ERROR, "Invalid exception thread_id!");
			return -1;
		}
		ctx = &thinkos_except_buf.ctx;
	} else if (__thinkos_thread_ispaused(thread_id)) {
		ctx = thinkos_rt.ctx[thread_id];
		DCC_LOG2(LOG_INFO, "ThinkOS thread=%d context=%08x!", thread_id, ctx);
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
		x = (uint32_t)ctx + sizeof(struct thinkos_context);
		break;
	case 14:
		x = ctx->lr;
		break;
	case 15:
		x = ctx->pc;
		break;
	case 25:
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
	case 25:
		ctx->xpsr = (ctx->xpsr & ~CM_APSR_MASK) | (val & CM_APSR_MASK);
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

	if (thread_id >= THINKOS_THREADS_MAX)
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

	if (thread_id == THINKOS_THREAD_IDLE) {
		/* thread is the IDLE try to get the first initialized 
		   thread instead. */
		thread_id = __thinkos_thread_getnext(-1);
	}

	if (thread_id >= THINKOS_THREADS_MAX)
		return -1;

	if (thinkos_except_buf.thread_id == thread_id) {
		ctx = &thinkos_except_buf.ctx;
	} else {
		ctx = thinkos_rt.ctx[thread_id];
		if (((uint32_t)ctx < 0x10000000) || ((uint32_t)ctx >= 0x30000000)) {
			DCC_LOG1(LOG_ERROR, "Invalid context: %08x!", ctx);
			return -1;
		}
		DCC_LOG3(LOG_TRACE, "ThinkOS thread=%d context=%08x PC=%08x!", 
				 thread_id, ctx, ctx->pc);
	}

	return dmon_thread_step(thread_id, false);
}

int thread_continue(unsigned int gdb_thread_id)
{
	unsigned int thread_id = gdb_thread_id - THREAD_ID_OFFS;

	if (thread_id >= THINKOS_THREADS_MAX)
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

	if (thread_id > THINKOS_THREAD_VOID) {
		DCC_LOG(LOG_ERROR, "Invalid thread!");
		return -1;
	}

	if (thread_id == THINKOS_THREAD_IDLE) {
		DCC_LOG(LOG_INFO, "ThinkOS Idle thread");
	} else if (__thinkos_thread_isfaulty(thread_id)) {
		if (xcpt->thread_id != thread_id) {
			DCC_LOG(LOG_ERROR, "Invalid exception thread_id!");
			return -1;
		}
	} else if (__thinkos_thread_ispaused(thread_id)) {
		DCC_LOG1(LOG_INFO, "ThinkOS thread=%d!", thread_id);
	} else {
		DCC_LOG(LOG_ERROR, "Invalid thread state!");
		return -1;
	}

	if (thread_id == THINKOS_THREAD_IDLE) {
		cp += str2hex(cp, "IDLE");
	} else if (thread_id == THINKOS_THREAD_VOID) {
		int ipsr;
		ipsr = (thinkos_rt.void_ctx->xpsr & 0x1ff);
		cp += str2hex(cp, "IRQ");
		cp += int2str2hex(cp, ipsr - 16);
	} else {
#if THINKOS_ENABLE_THREAD_INFO
		if (thinkos_rt.th_inf[thread_id] != NULL)
			n = str2hex(cp, thinkos_rt.th_inf[thread_id]->tag);
		else
			n = int2str2hex(cp, thread_id);
#else
		n = int2str2hex(cp, thread_id);
#endif
		cp += n;
	}
	cp += char2hex(cp, ' ');

	if (thread_id == THINKOS_THREAD_IDLE) {
		oid = THINKOS_WQ_READY;
		tmw = false;
	} else if (thread_id == THINKOS_THREAD_VOID) {
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
#if THINKOS_IRQ_MAX > 0
			int irq;
			for (irq = 0; irq < THINKOS_IRQ_MAX; ++irq) {
				if (thinkos_rt.irq_th[irq] == thread_id) {
					cp += str2hex(cp, "wait on IRQ[");
					cp += int2str2hex(cp, irq);
					cp += char2hex(cp, ']');
					break;
				}
			}
			if (irq == THINKOS_IRQ_MAX)
#endif
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
		cp += char2hex(cp, '[');
		cp += int2str2hex(cp, oid);
		cp += char2hex(cp, ']');
	}
	
	return cp - buf;
}

/* -------------------------------------------------------------------------
 * Memory auxiliarly functions
 * ------------------------------------------------------------------------- */

struct mem_blk {
	uint32_t addr;
	uint32_t size: 31;
	uint32_t ro: 1;
};

static bool addr2block(const struct mem_desc * mem, 
					   uint32_t addr, struct mem_blk * blk) 
{
	uint32_t base;
	uint32_t size;
	int i;

	for (i = 0; mem->blk[i].cnt != 0; ++i) {
		size = mem->blk[i].cnt << mem->blk[i].siz;
		base = mem->blk[i].ref;
		if ((addr >= base) && (addr < (base + size))) {
			if (blk != NULL) {
				int pos;
				pos = (addr - base) >> mem->blk[i].siz;
				blk->addr = base + (pos << mem->blk[i].siz);
				blk->size = 1 << mem->blk[i].siz;
				blk->ro = (mem->blk[i].opt == BLK_RO) ? 1 : 0;
			}
			return true;
		}
	}

	return false;
}

#define FLASH_BASE ((uint32_t)STM32_FLASH_MEM)

int target_mem_write(uint32_t addr, 
					 const void * ptr, unsigned int len)
{
	uint8_t * src = (uint8_t *)ptr;
	struct mem_blk blk;
	unsigned int cnt;
	unsigned int rem;

	rem = len;

	while (rem) {
		if (addr2block(this_board.memory.ram, addr, &blk)) {
			if (blk.ro) {
				DCC_LOG2(LOG_ERROR, "read only block addr=0x%08x size=%d", 
						 blk.addr, blk.size);
				return -1;
			}

			DCC_LOG2(LOG_TRACE, "RAM block addr=0x%08x size=%d", 
					 blk.addr, blk.size);

			cnt = blk.size - (addr - blk.addr);
			if (cnt > rem)
				cnt = rem;

			__thinkos_memcpy((void *)addr, src, cnt);
		} else if (addr2block(this_board.memory.flash, addr, &blk)) {
			uint32_t offs;

			if (blk.ro) {
				DCC_LOG2(LOG_ERROR, "read only block addr=0x%08x size=%d", 
						 blk.addr, blk.size);
				return -1;
			}

			DCC_LOG2(LOG_TRACE, "FLASH block addr=0x%08x size=%d", 
					 blk.addr, blk.size);

			offs = addr - FLASH_BASE;
			if (blk.addr == addr) {
				DCC_LOG2(LOG_TRACE, "block erase addr=0x%08x size=%d", 
						 blk.addr, blk.size);
				stm32_flash_erase(offs, blk.size);
			};

			cnt = blk.size - (addr - blk.addr);
			if (cnt > rem)
				cnt = rem;

			if (stm32_flash_write(offs, src, cnt) < 0) {
				DCC_LOG1(LOG_ERROR, "stm32_flash_write(0x%08x) failed!", addr);
				return -1;
			}

		} else {
			DCC_LOG1(LOG_ERROR, "invalid address 0x%08x", addr);
			return -1;
		}

		addr += cnt;
		src += cnt;
		rem -= cnt;
	}

	return len - rem;
}

int target_mem_read(uint32_t addr, void * ptr, unsigned int len)
{
	uint8_t * dst = (uint8_t *)ptr;
	struct mem_blk blk;
	unsigned int rem;

	DCC_LOG2(LOG_MSG, "0x%08x .. 0x%08x", addr, addr + len - 1);

	rem = len;

	while (rem) {
		unsigned int cnt;

		if (addr2block(this_board.memory.ram, addr, &blk)) {
			/* not flash */
			DCC_LOG2(LOG_MSG, "RAM block addr=0x%08x size=%d", 
					 blk.addr, blk.size);
		} else if (addr2block(this_board.memory.flash, addr, &blk)) {
			/* flash */
			DCC_LOG2(LOG_MSG, "FLASH block addr=0x%08x size=%d", 
					 blk.addr, blk.size);
		} else {
			DCC_LOG1(LOG_MSG, "invalid mem location addr=0x%08x", addr);
			return -1;
		}

		cnt = blk.size - (addr - blk.addr);
		if (cnt > rem)
			cnt = rem;

		__thinkos_memcpy(dst, (void *)addr, cnt);

		addr += cnt;
		dst += cnt;
		rem -= cnt;
	}

	return len;
}

/* -------------------------------------------------------------------------
 * Core auxiliarly functions
 * ------------------------------------------------------------------------- */

void target_halt(void)
{
	dmon_app_suspend();
}

void target_continue(void)
{
	dmon_app_continue();
}

int target_goto(uint32_t addr, int opt)
{
	return 0;
}

/* -------------------------------------------------------------------------
 * Core files
 * ------------------------------------------------------------------------- */

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
"<reg name=\"xpsr\" bitsize=\"32\" regnum=\"25\"/>\n"
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


#ifndef GDB_ENABLE_MEM_MAP
#define GDB_ENABLE_MEM_MAP 0
#endif

#if (GDB_ENABLE_MEM_MAP) 
const char memory_map_xml[] = 
"<memory-map>"
"<memory type=\"flash\" start=\"0x8000000\" length=\"0x100000\"/>"
"<memory type=\"flash\" start=\"0x8100000\" length=\"0x100000\"/>"
"<memory type=\"ram\" start=\"0x20000000\" length=\"0x30000\"/>"
"<memory type=\"ram\" start=\"0x10000000\" length=\"0x10000\"/>"
"</memory-map>";
#endif

int target_file_read(const char * name, char * dst, 
					  unsigned int offs, unsigned int size)
{
	char * src;
	int len;
	int cnt;
	int i;

	if (prefix(name, "target.xml")) {
		src = (char *)target_xml;
		len = sizeof(target_xml) - 1;
#if (GDB_ENABLE_MEM_MAP) 
	} else if (prefix(name, "memmap.xml")) {
		src = (char *)memory_map_xml;
		len = sizeof(memory_map_xml) - 1;
#endif
	} else
		return -1;

	if (offs >= len)
		return 0;

	DCC_LOG3(LOG_INFO, "offs=%d len=%d size=%d", offs, len, size);

	cnt = len - offs;
	if (cnt > size)
		cnt = size;

	src += offs;

	for (i = 0; i < cnt; ++i)
		dst[i] = src[i];

	return cnt;
}

