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
 * @file .c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 



#include <string.h>
#include <stdlib.h>

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

	e = (ice_mem_entry_t *)mem_map;

	DCC_LOG2(LOG_INFO, "addr:%08x len:%d", addr, len);

	ptr = (uint8_t *)dst;
	rem = len;
	for (; (size = e->blk.count * e->blk.size) > 0; e++) {
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

	e = (ice_mem_entry_t *)mem_map;

	DCC_LOG2(LOG_INFO, "addr:%08x len:%d", addr, len);

	ptr = (uint8_t *)src;
	rem = len;
	for (; (size = e->blk.count * e->blk.size) > 0; e++) {
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

	e = (ice_mem_entry_t *)mem_map;

	DCC_LOG2(LOG_INFO, "addr:%08x len:%d", addr, len);

	rem = len;
	for (; (size = e->blk.count * e->blk.size) > 0; e++) {
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

			rem -= ret;
			addr += ret;
			ref.offs += ret;
			n = e->blk.size;
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

	for (i = 0; mem[i].blk.count > 0; i++) {
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

const char * ice_errtab[] = {
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
	.info = (ice_test_t)null_ice_success,

	.core_reset = (ice_core_reset_t)null_ice_success,

	/* context display */	
	.context_show = (ice_context_show_t)null_ice_fail,
	/* instruction decoding */	
	.print_insn = (ice_print_insn_t )null_ice_fail
};

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

