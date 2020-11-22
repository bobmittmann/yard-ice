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
 * @file ice_drv.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 



#include <string.h>

#include "target.h"
#include "ice_drv.h"

#include <sys/dcclog.h>

int default_mem_read(ice_ctrl_t * ctrl, const ice_mem_ref_t * addr, 
					 void * buf, ice_size_t len)
{
	ice_drv_t * ice = ctrl->ice;	
	uint8_t * ptr = (uint8_t *)buf;
	uint32_t pos = addr->base + addr->offs;
	int cnt = 0;

	DCC_LOG2(LOG_INFO, "addr=0x%08x len=%d", pos, len);

	if (len == 0)
		return 0;

	/* get the first byte, if the address is not half-word (16bits) aligned */
	if (pos & 0x01) {
		ice_rd8(ice, pos, ptr);
		ptr++;
		len--;
		pos++;
		cnt++;
	}

	/* get the next word, if the address is not word (32bits) aligned */
	if ((len >= 2) && (pos & 0x02)) {
		uint16_t data;
		ice_rd16(ice, pos, &data);
		ptr[0] = data;
		ptr[1] = data >> 8;
		ptr += 2;
		len -= 2;
		pos += 2;
		cnt += 2;
	}

	while (len >= 4) {
		uint32_t data;
		ice_rd32(ice, pos, &data);
		ptr[0] = data;
		ptr[1] = data >> 8;
		ptr[2] = data >> 16;
		ptr[3] = data >> 24;
		ptr += 4;
		len -= 4;
		pos += 4;
		cnt += 4;
	}

	/* get the last word */
	if (len >= 2) {
		uint16_t data;
		ice_rd16(ice, pos, &data);
		ptr[0] = data;
		ptr[1] = data >> 8;
		ptr += 2;
		len -= 2;
		pos += 2;
		cnt += 2;
	}

	/* get the last byte */
	if (len > 0) {
		ice_rd8(ice, pos, ptr);
		cnt++;
	}

	return cnt;
}

int default_mem_write(ice_ctrl_t * ctrl, const ice_mem_ref_t * addr, 
					  const void * buf, ice_size_t len)
{
	ice_drv_t * ice = ctrl->ice;	
	uint8_t * ptr = (uint8_t *)buf;
	uint32_t pos = addr->base + addr->offs;
	int cnt = 0;

	DCC_LOG2(LOG_INFO, "addr=0x%08x len=%d", pos, len);

	if (len == 0)
		return 0;

	/* write the first byte, if the address is not half-word (16bits) aligned */
	if (pos & 0x01) {
		ice_wr8(ice, pos, *ptr);
		ptr++;
		len--;
		pos++;
		cnt++;
	}

	/* write the next word, if the address is not word (32bits) aligned */
	if ((len >= 2) && (pos & 0x02)) {
		uint16_t data;
		data = ptr[0] | (ptr[1] << 8);
		ice_wr16(ice, pos, data);
		ptr += 2;
		len -= 2;
		pos += 2;
		cnt += 2;
	}

	while (len >= 4) {
		uint32_t data;
		data = ptr[0] | (ptr[1] << 8) | (ptr[2] << 16) | (ptr[3] << 24);
		ice_wr32(ice, pos, data);
		ptr += 4;
		len -= 4;
		pos += 4;
		cnt += 4;
	}

	/* write the last word, if there is at leas 2 bytes remaining */
	if (len >= 2) {
		uint16_t data;
		data = ptr[0] | (ptr[1] << 8);
		ice_wr16(ice, pos, data);
		ptr += 2;
		len -= 2;
		pos += 2;
		cnt += 2;
	}

	/* write the last byte, if any */
	if (len > 0) {
		ice_wr8(ice, pos, *ptr);
		cnt++;
	}

	return cnt;
}

int default_mem_erase(ice_ctrl_t * ctrl, const ice_mem_ref_t * addr, 
					  ice_size_t len)
{
	ice_drv_t * ice = ctrl->ice;	
	uint32_t pos = addr->base + addr->offs;
	int cnt = 0;

	DCC_LOG2(LOG_INFO, "addr=0x%08x len=%d", pos, len);

	if (len == 0)
		return 0;

	if (pos & 0x01) {
		ice_wr8(ice, pos, 0);
		len--;
		pos++;
		cnt++;
	}

	if ((len >= 2) && (pos & 0x02)) {
		ice_wr16(ice, pos, 0);
		len -= 2;
		pos += 2;
		cnt += 2;
	}

	while (len >= 4) {
		ice_wr32(ice, pos, 0);
		len -= 4;
		pos += 4;
		cnt += 4;
	}

	if (len >= 2) {
		ice_wr16(ice, pos, 0);
		len -= 2;
		pos += 2;
		cnt += 2;
	}

	if (len > 0) {
		ice_wr8(ice, pos, 0);
		cnt++;
	}

	return cnt;
}

const struct ice_mem_oper mem_op_null = {
	.read = default_mem_read,
	.write = default_mem_write, 
	.erase = default_mem_erase
};

int ice_mem_read(const ice_drv_t * ice, const ice_mem_entry_t * mem_map, 
				 ice_addr_t addr, void * dst, ice_size_t len)
{
	ice_mem_entry_t * e;
	ice_mem_ref_t ref;
	uint32_t size;
	uint32_t mem_start;
	uint32_t mem_end;
	uint8_t * ptr;
	uint32_t rem;
	uint32_t blk_start;
	uint32_t n;
	int ret;
	int blk;

	DCC_LOG2(LOG_INFO, "addr:%08x len:%d", addr, len);

	ptr = (uint8_t *)dst;
	rem = len;
	for (e = (ice_mem_entry_t *)mem_map; e->op != NULL; e++) {

		if ((size = e->blk.count * e->blk.size) == 0) {
			/* skip empty blocks */
			continue;
		}

		/* get the memory block start address */
		mem_start = e->addr.base + e->addr.offs;
		/* get the memory block end address */
		mem_end = mem_start + size - 1;

		if ((addr < mem_start) || (addr > mem_end)) {
			DCC_LOG4(LOG_INFO, "addr:%08x >< '%s': %08x...%08x", 
					 addr, e->name, mem_start, mem_end);
			continue;
		}

		DCC_LOG4(LOG_INFO, "addr:%08x -> '%s': %08x...%08x", 
				 addr, e->name, mem_start, mem_end);

//		if (e->flags & MEM_WO) {
			/* XXX: attempt to read a write only block ???? */
//			return -1;
//		}

		/* set the base */
		ref.base = e->addr.base;
		/* get the total memory offset */
		ref.offs = addr - ref.base;

		blk_start = mem_start;
		blk = (addr - mem_start) / e->blk.size;
		blk_start = mem_start + (blk * e->blk.size);

		n = e->blk.size - (addr - blk_start);

		DCC_LOG4(LOG_INFO, "blk.size:%08x addr:%08x blk_start:%08x n:%u", 
				 e->blk.size, addr, blk_start, n);

		for (; blk < e->blk.count; blk++) {
			if (n > rem)
				n = rem;

			DCC_LOG3(LOG_INFO, "read: %08x:%06x, %u", ref.base, ref.offs, n);

			ret = e->op->read(ice->ctrl, &ref, ptr, n);

			if (ret != n) {
				if (ret < 0) {
					DCC_LOG(LOG_WARNING, "read fail");
					return ret;
				}
				if (ret > n) {
					DCC_LOG2(LOG_ERROR, "long read: ret(%d) > n(%d)", 
							 ret, n);
				} else {
					DCC_LOG2(LOG_WARNING, "short read: ret(%d) < n(%d)", 
							 ret, n);
					if (ret == 0)
						break;
				}
			}

			if (ret >= rem) {
				return len + (ret - rem);
			}
			rem -= ret;
			addr += ret;
			ref.offs += ret;
			ptr += ret;
			n = e->blk.size;
		}
	}

	return len - rem;
}

int ice_mem_write(const ice_drv_t * ice, const ice_mem_entry_t * mem_map, 
				  ice_addr_t addr, const void * src, ice_size_t len)
{
	ice_mem_entry_t * e;
	ice_mem_ref_t ref;
	uint32_t size;
	uint32_t mem_start;
	uint32_t mem_end;
	uint8_t * ptr;
	uint32_t rem;
	uint32_t blk_start;
	uint32_t n;
	int ret;
	int blk;

	DCC_LOG2(LOG_INFO, "addr:%08x len:%d", addr, len);

	ptr = (uint8_t *)src;
	rem = len;

	for (e = (ice_mem_entry_t *)mem_map; e->op != NULL; e++) {

		if ((size = e->blk.count * e->blk.size) == 0) {
			/* skip empty blocks */
			continue;
		}

		/* get the memory block start address */
		mem_start = e->addr.base + e->addr.offs;
		/* get the memory block end address */
		mem_end = mem_start + size - 1;
		if ((addr < mem_start) || (addr > mem_end))
			continue;

		if (e->flags & MEM_RO) {
			DCC_LOG1(LOG_WARNING, "%s: read only!", e->name);
			/* XXX: attempt to write into a readonly block ???? */
			return -1;
		}

		/* set the base */
		ref.base = e->addr.base;
		/* get the total memory offset */
		ref.offs = addr - ref.base;

		/* get block number to write on */
		blk = (addr - mem_start) / e->blk.size;
		blk_start = mem_start + (blk * e->blk.size);
		n = e->blk.size - (addr - blk_start);

		DCC_LOG4(LOG_INFO, "blk.size:%08x addr:%08x blk_start:%08x n:%u", 
				 e->blk.size, addr, blk_start, n);


		for (; blk < e->blk.count; blk++) {
			if (n > rem)
				n = rem;

			DCC_LOG3(LOG_INFO, "write: %08x:%06x, %d", ref.base, ref.offs, n);

			ret = e->op->write(ice->ctrl, &ref, ptr, n);

			if (ret != n) {
				if (ret < 0) {
					DCC_LOG(LOG_WARNING, "write fail");
					return ret;
				}
				if (ret > n) {
					DCC_LOG2(LOG_WARNING, "long write: ret(%d) > n(%d)", 
							 ret, n);
				} else {
					DCC_LOG2(LOG_WARNING, "short write: ret(%d) < n(%d)", 
							 ret, n);
					if (ret == 0)
						break;
				}
			}

			if (ret >= rem) {
				return len + (ret - rem);
			}
			rem -= ret;
			addr += ret;
			ref.offs += ret;
			ptr += ret;
			n = e->blk.size;
		}
	}

	return len - rem;
}

int ice_mem_erase(const ice_drv_t * ice, const ice_mem_entry_t * mem_map, 
				  ice_addr_t addr, ice_size_t len)
{
	ice_mem_entry_t * e;
	ice_mem_ref_t ref;
	uint32_t size;
	uint32_t mem_start;
	uint32_t mem_end;
	uint32_t rem;
	uint32_t blk_start;
	uint32_t n;
	int ret;
	int blk;

	DCC_LOG2(LOG_TRACE, "addr:%08x len:%d", addr, len);

	rem = len;

	for (e = (ice_mem_entry_t *)mem_map; e->op != NULL; e++) {

		if ((size = e->blk.count * e->blk.size) == 0) {
			/* skip empty blocks */
			continue;
		}
		/* get the memory block start address */
		mem_start = e->addr.base + e->addr.offs;
		/* get the memory block end address */
		mem_end = mem_start + size - 1;
		if ((addr < mem_start) || (addr > mem_end)) {
			/* addr must be between the start and end addresses */
			DCC_LOG2(LOG_INFO, "skip block, start:%08x end:%08x", 
					 mem_start, mem_end);
			continue;
		}

		DCC_LOG5(LOG_INFO, "'%s': %08x:%06x, %d blocks of %d.", 
				 e->name, e->addr.base, e->addr.offs, 
				 e->blk.count, e->blk.size);

		if (e->flags & MEM_RO) {
			/* XXX: attempt to erase a readonly block ???? */
			DCC_LOG2(LOG_INFO, "read only, start:%08x end:%08x", 
					 mem_start, mem_end);
			return -1;
		}

		/* set the base */
		ref.base = e->addr.base;
		/* get the offset form the memory base address */
		ref.offs = addr - ref.base;

		/* compute the block number and the number of bytes
		   from addr to the end of the block  */
		blk_start = mem_start;
		blk = (addr - mem_start) / e->blk.size;
		blk_start = mem_start + (blk * e->blk.size);

		n = e->blk.size - (addr - blk_start);

		DCC_LOG4(LOG_INFO, "blk.size:%08x addr:%08x blk_start:%08x n:%u", 
				 e->blk.size, addr, blk_start, n);

		for (; blk < e->blk.count; blk++) {

			DCC_LOG3(LOG_INFO, "blk:%d n:%d rem:%d", blk, n, rem);

			/* adjust n to fit in the remaining bytes */
			if (n > rem)
				n = rem;

			/* try to erase 'n' bytes */
			ret = e->op->erase(ice->ctrl, &ref, n);

			if (ret != n) {
				if (ret < 0) {
					DCC_LOG(LOG_WARNING, "erase fail");
					return ret;
				}
				if (ret > n) {
					DCC_LOG2(LOG_WARNING, "long erase: ret(%d) > n(%d)", 
							 ret, n);
				} else {
					DCC_LOG2(LOG_WARNING, "short erase: ret(%d) < n(%d)", 
							 ret, n);
					if (ret == 0)
						break;
				}
			}

			if (ret >= rem) {
				DCC_LOG2(LOG_INFO, "rem:%d, ret:%d", rem, ret);
				return len + (ret - rem);
			}

			n = e->blk.size;
			while (ret > n) {
				blk++;
				rem -= n;
				addr += n;
				ref.offs += n;
				ret -= n;
			}
			rem -= ret;
			addr += ret;
			ref.offs += ret;
		}

		/* TODO: check code generation erro with GCC-4.4.4 !! */

		DCC_LOG3(LOG_INFO, "rem:%d, n:%d, blk:%d", rem, n, blk);
	}

	return len - rem;
}

int ice_mem_lookup(const ice_drv_t * ice, 
					const ice_mem_entry_t * mem, 
					const char * name)
{
	int i;

	for (i = 0; mem[i].op != NULL; i++) {
		
		DCC_LOG1(LOG_INFO, "'%s'", mem[i].name);
		if (strcmp(mem[i].name, name) == 0)
			return i;
	}

	return -1;
}

struct ice_mem_entry * ice_mem_by_name(const ice_drv_t * ice, 
									   const ice_mem_entry_t * mem, 
									   const char * name)
{
	int idx;

	if ((idx  = ice_mem_lookup(ice, mem, name)) < 0)
		return NULL;

	return (struct ice_mem_entry *)&mem[idx];
}

const char * const ice_errtab[] = {
	"ICE OK",
	"ICE undefined error",
	"ICE invalid argument",
	"ICE invalid state",
	"ICE locked",
	"ICE not locked",
	"ICE BIG endian unsupported",
	"ICE LITTLE endian unsupported",
	"ICE JTAG fail",
	"ICE halt timeout",
	"ICE halt fail"
};

char * ice_strerror(const ice_drv_t * ice, int errno)
{
	errno += 100;

	if (errno < -10)
		errno = -1;

	return (char *)ice_errtab[-errno];
}


int	null_ice_configure(ice_ctrl_t * ctrl, uint32_t jtag_freq, int endianess)
{
	DCC_LOG1(LOG_INFO, "ctrl=%p", ctrl);

	return 0;
}

int null_ice_status(ice_ctrl_t * ctrl)
{
	DCC_LOG1(LOG_INFO, "ctrl=%p", ctrl);

	return ICE_ST_FAULT;
}

int null_ice_fail(ice_ctrl_t * ctrl)
{
	DCC_LOG1(LOG_INFO, "ctrl=%p", ctrl);

	return -1;
}

int null_ice_success(ice_ctrl_t * ctrl)
{
	DCC_LOG1(LOG_INFO, "ctrl=%p", ctrl);

	return 0;
}

int default_ice_rd16(ice_ctrl_t * ctrl, uint32_t addr, uint16_t * data)
{
	ice_drv_t * ice = ctrl->ice;	
	uint8_t * ptr = (uint8_t *)data;
	int ret;

	DCC_LOG1(LOG_INFO, "addr=0x%08x", addr);

	if ((ret = ice_rd8(ice, addr, &ptr[0])) < 0)
		return ret;

	return ice_rd8(ice, addr, &ptr[1]);
}

int default_ice_wr16(ice_ctrl_t * ctrl, uint32_t addr, uint16_t data)
{
	ice_drv_t * ice = ctrl->ice;	
	int ret;

	DCC_LOG1(LOG_INFO, "addr=0x%08x", addr);

	if ((ret = ice_wr8(ice, addr, data)) < 0)
		return ret;

	return ice_wr8(ice, addr, data >> 8);
}

int default_ice_rd32(ice_ctrl_t * ctrl, uint32_t addr, uint32_t * data)
{
	ice_drv_t * ice = ctrl->ice;	
	uint16_t * ptr = (uint16_t *)data;
	int ret;

	DCC_LOG1(LOG_INFO, "addr=0x%08x", addr);

	if ((ret = ice_rd16(ice, addr, &ptr[0])) < 0)
		return ret;

	return ice_rd16(ice, addr, &ptr[1]);
}

int default_ice_wr32(ice_ctrl_t * ctrl, uint32_t addr, uint32_t data)
{
	ice_drv_t * ice = ctrl->ice;	
	int ret;

	DCC_LOG1(LOG_INFO, "addr=0x%08x", addr);

	if ((ret = ice_wr16(ice, addr, data)) < 0)
		return ret;

	return ice_wr16(ice, addr, data >> 16);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
const struct ice_oper null_ice_oper = {
	/* driver initialization */
	.open = (ice_open_t)null_ice_success,
	/* driver unloading */
	.close = (ice_close_t)null_ice_success,

	/* driver configuration */
	.configure = (ice_configure_t)null_ice_configure,
	/* status */
	.status = (ice_status_t)null_ice_status,
	.poll = (ice_poll_t)null_ice_status,
	.signal = (ice_signal_t)null_ice_success,

	/* target connect request */
	.connect = (ice_connect_t)null_ice_success,	
	/* target release */
	.release = (ice_release_t)null_ice_success,	

	/* target halt request */
	.halt = (ice_halt_t)null_ice_fail,
	.halt_wait = (ice_halt_wait_t)null_ice_success,
	.on_break = (ice_on_break_t)null_ice_success,

	/* start executing form specified address */
	.go_to = (ice_go_to_t)null_ice_fail,
	/* target run request */
	.run = (ice_run_t)null_ice_fail,
	/* target instruction step request */
	.step = (ice_step_t)null_ice_fail,	
	.exec = (ice_exec_t)null_ice_fail,

	/* breakpoints */
	.bp_set = (ice_bp_set_t)null_ice_fail,
	.bp_clr = (ice_bp_clr_t)null_ice_fail,

	/* watchpoints */
	.wp_set = (ice_wp_set_t)null_ice_fail,
	.wp_clr = (ice_wp_clr_t)null_ice_fail,

	/* register access */	
	.reg_get = (ice_reg_get_t)null_ice_fail,
	.reg_set = (ice_reg_set_t)null_ice_fail,

	/* instruction fetch address */	
	.ifa_get = (ice_ifa_get_t)null_ice_fail,
	.ifa_set = (ice_ifa_set_t)null_ice_fail,

	/* memory lock/unlock */	
	.mem_lock = (ice_mem_lock_t)null_ice_success,
	.mem_unlock = (ice_mem_unlock_t)null_ice_success,

	/* io access */	
	.rd8 = (ice_rd8_t)null_ice_fail,
	.wr8 = (ice_wr8_t)null_ice_fail,

	.rd16 = default_ice_rd16,
	.wr16 = default_ice_wr16,

	.rd32 = default_ice_rd32,
	.wr32 = default_ice_wr32,

	.fifo_wr8 = (ice_fifo_wr8_t)null_ice_fail,
	.fifo_rd8 = (ice_fifo_rd8_t)null_ice_fail,

	.test = (ice_test_t)null_ice_success,
	.info = (ice_info_t)null_ice_success,

	.core_reset = (ice_core_reset_t)null_ice_success,

	/* context display */	
	.context_show = (ice_context_show_t)null_ice_fail,
	/* instruction decoding */	
	.print_insn = (ice_print_insn_t )null_ice_fail
};
#pragma GCC diagnostic pop

const struct ice_drv_info ice_drv_null = {
	.name = "null",
	.version = "0.2",
	.vendor = "BORESTE",
	.oper = &null_ice_oper
};

typedef int (* ice_call_t)(ice_ctrl_t * ctrl);

int ice_open(ice_drv_t * ice, const ice_drv_info_t * info, ice_ctrl_t * ctrl)
{
	ice_call_t * null_op = (ice_call_t *)&null_ice_oper;
	ice_call_t * src_op = (ice_call_t *)info->oper;
	ice_call_t * dst_op = (ice_call_t *)&ice->op;
	int n = sizeof(ice_operations_t) / sizeof(ice_call_t);
	int i;

	ice->info = info;
	ice->ctrl = ctrl;
	ctrl->ice = ice;

	DCC_LOG2(LOG_INFO, "'%s' %s", info->name, info->version);

	for (i = 0; i < n; i++) {
		dst_op[i] = (src_op[i] != NULL) ? src_op[i] : null_op[i];
	}

	return ice->op.open(ice->ctrl);
}


#if 0
int target_ice_select(const struct target_info * target, const char * name)
{
	struct debugger * dbg = &debugger;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	const ice_drv_info_t * info;
	uint8_t irlen[32];
	jtag_tap_t * tap;
	ice_val_t jtag_clk;
	jtag_idcode_t idcode;
	int tap_pos;
	uint8_t * irpath;
	unsigned int cnt;
	char s[16];

	int ret;
	int i;

	if (dbg->state == DBG_ST_FAULT ) {
		DCC_LOG(LOG_ERROR, "Invalid state!");
		return ERR_STATE;
	}

	if ((info = target->ice_drv) == NULL) {
		DCC_LOG(LOG_ERROR, "NULL ice driver info!");
		return ERR_PARM;
	}

	if (!dbg->ext_pwr) { 
		ext_pwr_on();
		/* FIXME: configurable power on time */
		__os_sleep(200);
		dbg->ext_pwr = 1;
	}

	__os_mutex_lock(dbg->busy);

	if ((target == dbg->target) && (!force)) {
		DCC_LOG1(LOG_TRACE, "Keeping target: '%s'", target->name);
		__os_mutex_unlock(dbg->busy);
		return 0;
	}

	DCC_LOG1(LOG_TRACE, "Changing target: '%s'", target->name);

	if (dbg->mem_mod_id != -1) {
		DCC_LOG(LOG_TRACE, "Memory module unregister...");
		module_unregister(dbg->mem_mod_id); 
		dbg->mem_mod_id = -1;
	}

	/* release the target */
	poll_stop(dbg);
	ice_release(ice);

	dbg->target = &target_null;
	dbg->mem = target_null.mem;

	if (ice->info != info) {
		DCC_LOG(LOG_TRACE, "ICE driver change...");

		if (ice_close(ice) < 0) {
			/* closing the ICE controller driver */
			DCC_LOG(LOG_WARNING, "ICE controller close fail!");
		}

		dbg->state = DBG_ST_UNDEF;
		DCC_LOG(LOG_TRACE, "[DBG_ST_UNDEF]");

		memset(&dbg_ice_ctrl_buf, 0, sizeof(dbg_ice_ctrl_buf));

		/* load the ICE controller driver */
		if ((ret = ice_open(ice, info, &dbg_ice_ctrl_buf.ctrl)) < 0) {
			DCC_LOG(LOG_ERROR, "ICE controller open fail!");
			__os_mutex_unlock(dbg->busy);
			return ret;
		}

		fprintf(f, " - ICE driver: %s - %s - %s\n",
				ice->info->name, ice->info->version, ice->info->vendor);
		tracef("- ICE driver: %s - %s - %s",
			   ice->info->name, ice->info->version, ice->info->vendor);
	} 

	/* cache of frequently accessed structures ... */
	dbg->target = target;
	dbg->mem = target->mem;
	dbg->tcp_port = 9;

	/* TODO: modular link layer  to support other
	   access ports than JTAG... */

	if (target->clk_slow_on_connect)
		jtag_clk = target->jtag_clk_slow;
	else
		jtag_clk = target->jtag_clk_def;

	fprintf(f, " - Slow clock: %s.\n", fmt_freq(s, target->jtag_clk_slow));
	fprintf(f, " - Fast clock: %s.\n", fmt_freq(s, target->jtag_clk_def));

	jtag_rtck_freq_set(target->jtag_clk_slow);

	if (target->has_rtck) {
		jtag_tck_freq_set(target->jtag_clk_max);
		/* The preferred clock method is adaptive (RTCK) */
		if (target->prefer_rtck) {
			fprintf(f, " - RTCK enabled.\n");
			DCC_LOG(LOG_TRACE, "enabling RTCK.");
			jtag_rtck_enable();
		} else {
			jtag_rtck_disable();
		}
	} else {
		/* adjust the JTAG TCK frequency */
		if ((ret = jtag_tck_freq_set(jtag_clk)) != JTAG_OK) {
			DCC_LOG(LOG_ERROR, "jtag_clk_set()!");
			__os_mutex_unlock(dbg->busy);
			return ret;
		}

		jtag_rtck_disable();
	}

	if (target->reset_on_config) {

#if 0
		switch (target->reset_mode) {
		case RST_HARD:
			fprintf(f, " - Hardware reset on config...\n");
			DCC_LOG(LOG_TRACE, "hardware reset...");
			if ((ret = hw_reset(ice, target)) < 0) {
				DCC_LOG(LOG_WARNING, "hardware reset failed!");
			}
			break;
		case RST_CORE:
			fprintf(f, " - Core reset on config...\n");
			DCC_LOG(LOG_TRACE, "core reset...");
			if ((ret = core_reset(ice, target)) < 0) {
				DCC_LOG(LOG_WARNING, "core reset failed!");
			}
			break;
		case RST_SOFT:
		case RST_DBG:
			fprintf(f, " - Debug reset on config...\n");
			DCC_LOG(LOG_TRACE, "debug reset...");
			if ((ret = dbg_reset(ice, target)) < 0) {
				DCC_LOG(LOG_WARNING, "debug reset failed!");
			}
			break;
		}
#endif
		fprintf(f, " - Hardware reset on config...\n");
		DCC_LOG(LOG_TRACE, "hardware reset...");
		if ((ret = hw_reset(ice, target)) < 0) {
			DCC_LOG(LOG_WARNING, "hardware reset failed!");
		}
	}

	/* configure the scan path */
	if (target->jtag_probe) {

		fprintf(f, " - JTAG probe...\n");

#if 0
		DCC_LOG(LOG_MSG, "TAP reset...");
		/* assert the JTAG TRST signal (low) */
		jtag_trst(true);
		jtag_run_test(1, JTAG_TAP_IDLE);
		/* deasser the JTAG TRST signal (high) */
		jtag_trst(false);

		jtag_tap_reset();
#endif
		DCC_LOG(LOG_TRACE, "Dynamic JTAC config ...");

		/* dynamic configuration */
		if (jtag_chain_probe(irlen, 32, &cnt) != JTAG_OK) {
			if (target->arch->cpu->irlength == 0) {
				DCC_LOG(LOG_ERROR, "IR length !");
				__os_mutex_unlock(dbg->busy);
				return -1;
			} 
			irlen[0] = target->arch->cpu->irlength;
			cnt = 1;
		}
		irpath = irlen;
		(void)irpath;
		tap_pos = -1;
	} else {
		/* TODO: preconfigured scan chain */
		fprintf(f, " #NOTICE: target->jtag_probe flag not set!!!.\n");
		cnt = 0;
	}

	DCC_LOG1(LOG_TRACE, "TAPS: %d", cnt);

	if (cnt == 0) {
		DCC_LOG(LOG_WARNING, "No TAPs defined!");
		__os_mutex_unlock(dbg->busy);
		return 0;
	}

	/* reset the TAPs to put the IDCODE in the DR scan */
	jtag_tap_reset();

	/* initializing the jtag chain */
	if ((ret = jtag_chain_init(irlen, cnt)) != JTAG_OK) {
		DCC_LOG(LOG_ERROR, "JTAG chain fail!");
		__os_mutex_unlock(dbg->busy);
		return ret;
	}

	if (target->pre_config) {
		DCC_LOG(LOG_TRACE, "Target pre config callback...");
		if ((ret = target->pre_config(f, ice, target)) < 0) {
			DCC_LOG(LOG_ERROR, "target->pre_config() fail!");
			__os_mutex_unlock(dbg->busy);
			return ret;
		}
		cnt = jtag_tap_tell();
	} else {
		DCC_LOG(LOG_TRACE, "target->pre_config callback undefined!");
	}

	if (tap_pos < 0) {
		DCC_LOG(LOG_TRACE, "Detecting the TAP position...");

		for (i = 0; i < cnt; i++) {
			if ((ret = jtag_tap_get(&tap, i)) != JTAG_OK) {
				DCC_LOG(LOG_ERROR, "jtag_tap_get()!");
				__os_mutex_unlock(dbg->busy);
				return ret;
			}

			if ((ret = jtag_tap_idcode(tap, &idcode)) != JTAG_OK) {
				DCC_LOG(LOG_ERROR, "jtag_tap_idcode()!");
				__os_mutex_unlock(dbg->busy);
				return ret;
			}

			if ((idcode & target->arch->cpu->idmask) == 
				target->arch->cpu->idcomp) {
				DCC_LOG1(LOG_TRACE, "match, idcode:%08x", idcode);

				fprintf(f, " - JTAG IDCODE: 0x%08x\n", idcode); 
				tap_pos = i;
				break;
			}
		}

		if (tap_pos < 0) {
			DCC_LOG(LOG_WARNING, "no suitable CPU found()!");
			__os_mutex_unlock(dbg->busy);
			return -1;
		}
	} else {
		if (tap_pos > cnt) {
			DCC_LOG1(LOG_ERROR, "TAP position (%d) is out of bounds!", tap_pos);
			__os_mutex_unlock(dbg->busy);
			/* XXX: this is a JTAG error and shuld not be used in
			   a high level function... */
			return JTAG_ERR_INVALID_TAP;
		}

		if ((ret = jtag_tap_get(&tap, tap_pos)) != JTAG_OK) {
			DCC_LOG(LOG_ERROR, "jtag_tap_get()!");
			__os_mutex_unlock(dbg->busy);
			return ret;
		}
		if ((ret = jtag_tap_idcode(tap, &idcode)) != JTAG_OK) {
			DCC_LOG(LOG_ERROR, "jtag_tap_idcode()!");
			__os_mutex_unlock(dbg->busy);
			return ret;
		}

		DCC_LOG1(LOG_TRACE, "IDCODE:%08x", idcode);
		fprintf(f, " - JTAG IDCODE: 0x%08x\n", idcode); 

		if ((idcode & target->arch->cpu->idmask) != target->arch->cpu->idcomp) {
			DCC_LOG(LOG_TRACE, "invalid IDCODE");
			__os_mutex_unlock(dbg->busy);
			return -1;
		}

		DCC_LOG1(LOG_TRACE, "match, idcode:%08x", idcode);
	}

	tap->idmask = target->arch->cpu->idmask;
	tap->idcomp = target->arch->cpu->idcomp;

	memset(&ice->opt, 0, sizeof(ice_opt_t));

	if (ice_configure(ice, tap, &ice->opt, target->ice_cfg) < 0) {
		DCC_LOG(LOG_ERROR, "ICE controller configurarion fail!");
		__os_mutex_unlock(dbg->busy);
		return -1;
	}

	DCC_LOG(LOG_TRACE, "Memory module register...");
	dbg->mem_mod_id = mod_mem_register(target->mem);

#if 0
	for (i = 0; i < DBG_BREAKPOINT_MAX; i++) {
		dbg->bp[i].addr = 0;
		dbg->bp[i].size = 0;
	}
	for (i = 0; i < DBG_WATCHPOINT_MAX; i++) {
		dbg->wp[i].addr = 0;
		dbg->wp[i].mask = 0xffffffff;
	}
	dbg->bp_cnt = 0;
#endif
	dbg->dasm.base = dbg->target->start_addr;
	dbg->transf.base = dbg->target->start_addr;

	dbg->state = DBG_ST_UNCONNECTED;
	DCC_LOG(LOG_TRACE, "[DBG_ST_UNCONNECTED]");

	__os_mutex_unlock(dbg->busy);

	return 0;
}

#endif



