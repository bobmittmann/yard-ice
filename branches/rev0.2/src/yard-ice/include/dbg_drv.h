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
 * @file dbg_drv.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#ifndef __DBG_DRV_H__
#define __DBG_DRV_H__

#include <target.h>

/*
 * Memory operations funtion types
 */
typedef int (* dbg_mem_read_t)(target_addr_t, target_offs_t, 
							   void *, target_size_t);

typedef int (* dbg_mem_write_t)(target_addr_t, target_offs_t, 
								const void *, target_size_t);

typedef int (* dbg_mem_erase_t)(target_addr_t, target_offs_t, target_size_t);

/* Memory operations */
struct dbg_mem_oper {
	dbg_mem_read_t read;
	dbg_mem_write_t write;
	dbg_mem_erase_t erase;
};

typedef const struct dbg_mem_oper * dbg_mem_oper_t;

typedef enum {
	DBG_ICE_UNKNOWN = 0,
	DBG_ICE_HALTED = 1,
	DBG_ICE_RUNNING = 2,
} dbg_ice_state_t;

struct dbg_opt {
	const char * name;
	uint32_t val;
};

typedef struct dbg_opt dbg_opt_t;

typedef int (* dbg_init_t)(struct dbg_context *, dbg_opt_t opt[]);

typedef int (* dbg_status_t)(void);

typedef int (* dbg_poll_t)(void);

typedef int (* dbg_run_t)(dbg_context_t * ct);
typedef int (* dbg_step_t)(struct dbg_context *);

typedef int (* dbg_halt_t)(int method);
typedef int (* dbg_go_to_t)(dbg_context_t * ct, uint32_t addr);

typedef int (* dbg_int_enable_t)(void);
typedef int (* dbg_int_disable_t)(void);

typedef int (* dbg_connect_t)(int force);
typedef int (* dbg_release_t)(struct dbg_context *);

typedef int (* dbg_reset_t)(int soft);

typedef int (* dbg_bp_set_t)(int n, target_addr_t addr, 
							 target_addr_mask_t mask);

typedef int (* dbg_bp_clr_t)(int n);

typedef int (* dbg_wp_set_t)(int n, target_addr_t addr, 
							 target_addr_mask_t mask);

typedef int (* dbg_wp_clr_t)(int n);

typedef int (* dbg_context_get_t)(struct dbg_context *);
typedef int (* dbg_context_set_t)(const struct dbg_context *);

typedef int (* dbg_reg_get_t)(dbg_context_t * ct, int reg, uint32_t * val);

typedef int (* dbg_reg_set_t)(dbg_context_t * ct, int reg, uint32_t val);

typedef int (* dbg_pc_get_t)(dbg_context_t * ct, uint32_t * val);

typedef int (* dbg_pc_set_t)(dbg_context_t * ct, uint32_t val);

typedef int (* dbg_insn_get_t)(target_mem_map_t const * map, dbg_context_t *, 
							   target_addr_t, dbg_insn_t * insn);

typedef int (* dbg_insn_fmt_t)(char * buf, dbg_context_t * ct, 
							   dbg_insn_t * insn);

typedef int (* dbg_stack_read_t)(target_mem_map_t const * map, dbg_context_t * ct, 
             void * buf, int len, target_addr_t * addr);

typedef char * (* dbg_strerror_t)(void);

typedef int (* dbg_rd_t)(dbg_context_t * ct, uint32_t addr, uint32_t * val);

typedef int (* dbg_wr_t)(dbg_context_t * ct, uint32_t addr, uint32_t val);

#define ICE_VENDOR_MAX 14
#define ICE_NAME_MAX 10
#define ICE_VERSION_MAX 7

struct dbg_drv {
	char vendor[ICE_VENDOR_MAX + 1];
	char name[ICE_NAME_MAX + 1];
	char version[ICE_VERSION_MAX + 1];

	/* driver initialization */
	dbg_init_t init;
	
	dbg_strerror_t strerror;
	
	dbg_status_t status;

	/* target connect request */
	dbg_connect_t connect;

	/* target release */
	dbg_release_t release;	

	/* target halt request */
	dbg_halt_t halt;	

	/* start executing form specified address */
	dbg_go_to_t go_to;

	/* target run request */
	dbg_run_t run;
	/* target break polling */
	dbg_poll_t poll;

	/* target instruction step request */
	dbg_step_t step;

	/* target reset */
	dbg_reset_t reset;

	/* target set breakpoint */
	dbg_bp_set_t bp_set;
	/* target clear breakpoint */
	dbg_bp_clr_t bp_clr;

	/* target set watchpoint */
	dbg_wp_set_t wp_set;
	/* target clear watchpoint */
	dbg_wp_clr_t wp_clr;

	/* interrupt enabling/disabling */	
	dbg_int_enable_t int_enable;
	dbg_int_disable_t int_disable;

	/* register access */	
	dbg_reg_set_t reg_set;
	dbg_reg_get_t reg_get;

	/* io/special register access */	
	dbg_wr_t wr;
	dbg_rd_t rd;

	/*
     * XXX:
	 * execution pointer (program counter) access 
	 * The execution pointer may not reflect the actual program counter
	 *
	 */	
/*	dbg_pc_get_t pc_get;
	dbg_pc_set_t pc_set; */

	/* context saving/restoring */	
	dbg_context_get_t context_get;
	dbg_context_set_t context_set;

	/* instruction fetching / decoding */	
	dbg_insn_get_t insn_get;
	dbg_insn_fmt_t insn_fmt;

	/* stack access */	
	dbg_stack_read_t stack_read;

	/* memory access operations */
	const dbg_mem_oper_t mem_op[MEM_TYPE_MAX];
};

const struct dbg_mem_oper mem_op_null;

#ifdef __cplusplus
extern "C" {
#endif

int dbg_mem_read(target_mem_map_t const * map, dbg_mem_oper_t * mem_op, 
				 target_addr_t addr, void * ptr, target_size_t len);

int dbg_mem_write(target_mem_map_t const * map, dbg_mem_oper_t * mem_op, 
				  target_addr_t addr, const void * ptr, target_size_t len);

int dbg_mem_erase(target_mem_map_t const * map, dbg_mem_oper_t * mem_op, 
				  target_addr_t addr, target_size_t len);

#ifdef __cplusplus
	}
#endif

#endif /* __DBG_DRV_H__ */
