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
 * @file script.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */



#ifndef __SCRIPT_H__
#define __SCRIPT_H__

#include "ice_drv.h"

#define POLL() drv->poll(NULL, NULL)

#define RUN() drv->run((struct dbg_context *)(ct), NULL, NULL)

#define STEP() drv->step((struct dbg_context *)(ct))

#define CONTEXT_GET() drv->context_get((struct dbg_context *)(ct))

#define CONTEXT_SET() drv->context_set((const struct dbg_context *)(ct))

#define HALT() drv->halt()

#define STATUS() drv->status()

#define INT_ENABLE() drv->int_enable()

#define INT_DISABLE() drv->int_disable()

#define CONNECT() drv->connect(1)

#define RELEASE() drv->release((struct dbg_context *)(ct))

#define RESET(SOFT) drv->reset((int)SOFT)

#define BP_SET(N, ADDR) drv->bp_set((int)(N), (target_addr_t)(ADDR), \
									(target_addr_mask_t)(0xfffffffe))

#define BP_CLR(N) drv->bp_clr((int)(N));

#define REG_GET(REG, VAL) drv->reg_get((dbg_context_t *)ct, \
									   (int)(REG), (unsigned int *)(VAL))

#define REG_SET(REG, VAL) drv->reg_set((dbg_context_t *)ct, \
									   (int)(REG), (unsigned int)(VAL))

#define PC_GET(VAL) drv->pc_get((dbg_context_t *)ct, (unsigned int *)(VAL))

#define PC_SET(VAL) drv->pc_get((dbg_context_t *)ct, (unsigned int)(VAL))

#define INSN_GET(ADDR, INSN) drv->insn_get((target_mem_map_t *)map, \
	                                        (dbg_context_t *)ct, \
	                                        (target_addr_t)(ADDR), \
	                                        (dbg_insn_t *)(INSN))

#define INSN_FMT(BUF, INSN) drv->insn_fmt((char *)(BUF) \
										  (dbg_context_t *)ct, \
										  (dbg_insn_t *)(INSN))

#define STRERROR() drv->strerror()

#define STACK_READ(BUF, LEN) drv->stack_read((target_mem_map_t *)map, \
									         (dbg_context_t *)ct, \
									         (void *)(BUF), \
									         (int)(LEN), NULL)

#define RD(ADDR, DATA) dev->rd((dbg_context_t *)ct, \
							   (unsigned int *)(DATA))

#define WR(ADDR, DATA) dev->wr((dbg_context_t *)ct, \
							   (unsigned int )(DATA))

#define IN(ADDR, DATA) dev->rd((dbg_context_t *)ct, \
							   (unsigned int *)(DATA))

#define OUT(ADDR, DATA) dev->wr((dbg_context_t *)ct, \
								(unsigned int )(DATA))

#define MEM_READ(ADDR, BUF, LEN) mem_read((target_mem_map_t *)map, \
										  dev->mem_op, (void *)(BUF), \
										  (target_size_t*)(LEN))

#define MEM_WRITE(ADDR, BUF, LEN) mem_read((target_mem_map_t *)map, \
										  dev->mem_op, (const void *)(BUF), \
										  (target_size_t*)(LEN))

#define MEM_ERASE(ADDR, LEN) mem_erase((target_mem_map_t *)map, \
									   dev->mem_op, (target_size_t*)(LEN))

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
	}
#endif

#endif /* __DBG_DRV_H__ */

