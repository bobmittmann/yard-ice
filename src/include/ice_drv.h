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
 * @file ice_drv..h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#ifndef __ICE_DRV_H__
#define __ICE_DRV_H__

#include <stdio.h>
#include <stdlib.h>

#include "jtag.h"
#include "ice_comm.h"
#include "context.h"
#include "memory.h"
#include "dis-asm.h"

#define ICE_MEM_NAME_MAX 10

extern const char * const mem_bus_str[];

/* Memory address type */
typedef uint32_t ice_mem_addr_t;
/* Memory address mask type */
typedef uint32_t ice_mem_mask_t;
/* Memory block size type */
typedef uint32_t ice_mem_size_t;

/* Generic address type */
typedef uint32_t ice_addr_t;
/* Generic address mask type */
typedef uint32_t ice_addr_mask_t;
/* Generic offset type */
typedef uint32_t ice_offs_t;
/* Generic size type */
typedef uint32_t ice_size_t;

struct ice_bp {
	ice_mem_addr_t addr;
	ice_mem_size_t size;
};

typedef struct ice_bp ice_bp_t;

struct ice_wp {
	ice_mem_addr_t addr;
	ice_mem_mask_t mask;
};

typedef struct ice_wp ice_wp_t;


/*
 * memory region descriptor
 */

typedef enum {
	ICE_UNDEF = 0,
	ICE_ERROR = 1,
	ICE_INIT = 2,
	ICE_UNCONNECTED = 3,
	ICE_CONNECTED = 4,
	ICE_OUTOFSYNC = 5,
	ICE_RUNNING = 6,
	ICE_HALTED = 7,
} ice_state_t;


/* ICE status flags */
#define ICE_ST_HALT  (1 << 0) 
#define ICE_ST_FAULT (1 << 7)

typedef enum {
	ICE_OK = 0,
	ICE_ERR_UNDEF = -101,
	ICE_ERR_ARG = -102,
	ICE_ERR_INVALID_STATE = -103,
	ICE_ERR_LOCKED = -104,
	ICE_ERR_NOT_LOCKED = -105,
	ICE_ERR_BIG_ENDIAN = -106,
	ICE_ERR_LITTLE_ENDIAN = -107,
	ICE_ERR_JTAG = -108,
	ICE_ERR_HALT_TMO = -109,
	ICE_ERR_HALT_FAIL = -110
} ice_erro_t;

typedef enum {
	ICE_SIG_POLL_STOP = 0,
	ICE_SIG_POLL_START,
	ICE_SIG_TARGET_RESET
} ice_sig_t;

typedef enum {
	LITTLE_ENDIAN = 0,
	BIG_ENDIAN = 1
} ice_endianness_t;


typedef int16_t ice_oid_t;

typedef uint32_t ice_val_t;

/* Dynamic configuration */
struct ice_opt {
	/* Maximum number of breakpoints */
	uint16_t bp_max;
	/* Default size for breakpoints */
	uint16_t bp_defsz;
	/* Maximum number of watchpoints */
	uint16_t wp_max;
	/* Default size for watchpoints */
	uint16_t wp_defsz;
};

typedef struct ice_opt ice_opt_t;

typedef struct ice_ctrl ice_ctrl_t;

typedef int (* ice_open_t)(ice_ctrl_t * ctrl);
typedef int (* ice_close_t)(ice_ctrl_t * ctrl);

typedef int (* ice_configure_t)(ice_ctrl_t * ctrl, jtag_tap_t * tap, 
								ice_opt_t * opt, void * cfg);

typedef int (* ice_status_t)(ice_ctrl_t * ctrl);
typedef int (* ice_poll_t)(ice_ctrl_t * ctrl, ice_comm_t * comm);
typedef void (* ice_signal_t)(ice_ctrl_t * ctrl, ice_sig_t sig);

typedef int (* ice_halt_wait_t)(ice_ctrl_t * ctrl, int mutex, int tmo);

typedef int (* ice_connect_t)(ice_ctrl_t * ctrl, uint32_t idmask, 
							  uint32_t idcomp);

typedef int (* ice_release_t)(ice_ctrl_t * ctrl);

typedef int (* ice_run_t)(ice_ctrl_t * ctrl);

typedef int (* ice_exec_t)(ice_ctrl_t * ctrl, uint32_t addr, 
						   uint32_t * regv, int regc);

typedef int (* ice_step_t)(ice_ctrl_t * ctrl);

typedef int (* ice_halt_t)(ice_ctrl_t * ctrl);
typedef int (* ice_go_to_t)(ice_ctrl_t * ctrl, uint32_t addr);

typedef int (* ice_int_enable_t)(ice_ctrl_t * ctrl);
typedef int (* ice_int_disable_t)(ice_ctrl_t * ctrl);

typedef int (* ice_bp_set_t)(ice_ctrl_t * ctrl, uint32_t addr, uint32_t len, 
							 uint32_t * id);

typedef int (* ice_bp_clr_t)(ice_ctrl_t * ctrl, uint32_t id);

typedef int (* ice_wp_set_t)(ice_ctrl_t * ctrl, uint32_t addr, uint32_t len, 
							 uint32_t * id);

typedef int (* ice_wp_clr_t)(ice_ctrl_t * ctrl, uint32_t id);

typedef int (* ice_context_get_t)(ice_ctrl_t * ctrl);
typedef int (* ice_context_set_t)(ice_ctrl_t * ctrl);

typedef int (* ice_reg_get_t)(ice_ctrl_t * ctrl, int reg, uint32_t * val);

typedef int (* ice_reg_set_t)(ice_ctrl_t * ctrl, int reg, uint32_t val);

typedef int (* ice_pc_get_t)(ice_ctrl_t * ctrl, uint32_t * val);

typedef int (* ice_pc_set_t)(ice_ctrl_t * ctrl, uint32_t val);

typedef int (* ice_sp_get_t)(ice_ctrl_t * ctrl, uint32_t * val);

typedef int (* ice_sp_set_t)(ice_ctrl_t * ctrl, uint32_t val);

typedef int (* ice_ifa_get_t)(ice_ctrl_t * ctrl, uint32_t * val);

typedef int (* ice_ifa_set_t)(ice_ctrl_t * ctrl, uint32_t val);

#if 0
typedef int (* ice_stack_read_t)(ice_mem_map_t const * map, 
								 void * buf, int len, ice_addr_t * addr);
#endif

typedef int (* ice_mem_lock_t)(ice_ctrl_t * ctrl);

typedef int (* ice_mem_unlock_t)(ice_ctrl_t * ctrl);

typedef int (* ice_rd32_t)(ice_ctrl_t * ctrl, uint32_t addr, uint32_t * val);
typedef int (* ice_wr32_t)(ice_ctrl_t * ctrl, uint32_t addr, uint32_t val);

typedef int (* ice_rd16_t)(ice_ctrl_t * ctrl, uint32_t addr, uint16_t * val);
typedef int (* ice_wr16_t)(ice_ctrl_t * ctrl, uint32_t addr, uint16_t val);

typedef int (* ice_rd8_t)(ice_ctrl_t * ctrl, uint32_t addr, uint8_t * val);
typedef int (* ice_wr8_t)(ice_ctrl_t * ctrl, uint32_t addr, uint8_t val);


typedef int (* ice_fifo_rd8_t)(ice_ctrl_t * ctrl, uint32_t addr, 
							   void * buf, int len);
typedef int (* ice_fifo_wr8_t)(ice_ctrl_t * ctrl, uint32_t addr, 
							   const void * buf, int len);

typedef int (* ice_context_show_t)(ice_ctrl_t * ctrl, FILE * f);

typedef int (* ice_print_insn_t)(ice_ctrl_t * ctrl, uint32_t addr, 
								 struct disassemble_info * info);

typedef int (* ice_test_t)(ice_ctrl_t * ctrl, FILE * f, 
						   uint32_t req, uint32_t argc, uint32_t argv[]);

typedef int (* ice_info_t)(ice_ctrl_t * ctrl, FILE * f, uint32_t which);

typedef int (* ice_core_reset_t)(ice_ctrl_t * ctrl);

typedef int (* ice_system_reset_t)(ice_ctrl_t * ctrl);

typedef int (* ice_on_break_t)(ice_ctrl_t * ctrl);

#if 0
typedef int (* ice_comm_read_t)(ice_ctrl_t * ctrl, void * buf, int len);

typedef int (* ice_comm_write_t)(ice_ctrl_t * ctrl, const void * buf, int len);

typedef int (* ice_comm_open_t)(ice_ctrl_t * ctrl);

typedef int (* ice_comm_close_t)(ice_ctrl_t * ctrl);
#endif

#define ICE_VENDOR_MAX 15
#define ICE_NAME_MAX 15
#define ICE_VERSION_MAX 7

struct ice_oper {
	/* driver initialization */
	ice_open_t open;
	/* driver unloading */
	ice_close_t close;

	/* driver configuration */
	ice_configure_t configure;
	/* target status */
	ice_status_t status;
	/* target status poll */
	ice_poll_t poll;
	/* send assynchronous signal to the ICE driver */
	ice_signal_t signal;

	/* target connect request */
	ice_connect_t connect;
	/* target release */
	ice_release_t release;	

	/* target halt request */
	ice_halt_t halt;	
	ice_halt_wait_t halt_wait;
	/* on_break */	
	ice_on_break_t on_break;

	/* start executing form specified address */
	ice_go_to_t go_to;
	/* target run request */
	ice_run_t run;
	/* target instruction step request */
	ice_step_t step;
	/* execute a piece of code */
	ice_exec_t exec;

	/* target set breakpoint */
	ice_bp_set_t bp_set;
	/* target clear breakpoint */
	ice_bp_clr_t bp_clr;

	/* target set watchpoint */
	ice_wp_set_t wp_set;
	/* target clear watchpoint */
	ice_wp_clr_t wp_clr;

	/* register access */	
	ice_reg_get_t reg_get;
	ice_reg_set_t reg_set;

	/* instruction fetch address */	
	ice_ifa_get_t ifa_get;
	ice_ifa_set_t ifa_set;

	ice_mem_lock_t mem_lock;
	ice_mem_unlock_t mem_unlock;

	/* io access */	
	ice_wr8_t wr8;
	ice_rd8_t rd8;

	ice_wr16_t wr16;
	ice_rd16_t rd16;

	ice_wr32_t wr32;
	ice_rd32_t rd32;

	ice_fifo_wr8_t fifo_wr8;
	ice_fifo_rd8_t fifo_rd8;

	/* ice test */	
	ice_test_t test;
	/* ice info */	
	ice_info_t info;

	/* core reset */	
	ice_core_reset_t core_reset;
	/* system reset */	
	ice_system_reset_t system_reset;

	/* stack access */	
	ice_context_show_t context_show;
	/* instruction fetching / decoding */	
	ice_print_insn_t print_insn;

};

typedef struct ice_oper ice_operations_t;

/* Static information */
struct ice_drv_info {
	char name[ICE_NAME_MAX + 1];
	char version[ICE_VERSION_MAX + 1];
	char vendor[ICE_VENDOR_MAX + 1];
	const ice_operations_t * oper;
};

typedef struct ice_drv_info ice_drv_info_t;

struct ice_drv {
	ice_ctrl_t * ctrl;
	const struct ice_drv_info * info;
	struct ice_opt opt;
	ice_operations_t op;
};

typedef struct ice_drv ice_drv_t;

struct ice_ctrl {
	ice_drv_t * ice;
	uint32_t priv[];
};

struct ice_mem_ref {
	ice_addr_t base;
	ice_offs_t offs;
};

typedef struct ice_mem_ref ice_mem_ref_t; 

/*
 * Memory operations funtion types
 */
typedef int (* ice_mem_read_t)(ice_ctrl_t *, const ice_mem_ref_t * ,
							   void *, ice_size_t);

typedef int (* ice_mem_write_t)(ice_ctrl_t *, const ice_mem_ref_t *,
								const void *, ice_size_t);

typedef int (* ice_mem_erase_t)(ice_ctrl_t *, const ice_mem_ref_t *, 
								ice_size_t);

/* Memory operations */
struct ice_mem_oper {
	ice_mem_read_t read;
	ice_mem_write_t write;
	ice_mem_erase_t erase;
};

typedef struct ice_mem_oper ice_mem_oper_t;

struct ice_mem_entry {
	char name[ICE_MEM_NAME_MAX + 1];
	uint8_t flags;
	ice_mem_ref_t addr;
	struct {
		uint32_t count;
		uint32_t size;
	} blk;
	const ice_mem_oper_t * op;
};

typedef struct ice_mem_entry ice_mem_entry_t;

extern const struct ice_mem_oper mem_op_null;

const struct ice_drv_info ice_drv_null;

extern inline int ice_close(const ice_drv_t * ice) {
	return ice->op.close(ice->ctrl);
}

extern inline int ice_configure(const ice_drv_t * ice, jtag_tap_t * tap, 
								ice_opt_t * opt, void * cfg) {
	return ice->op.configure(ice->ctrl, tap, opt, cfg);
}

extern inline int ice_status(const ice_drv_t * ice) {
	return ice->op.status(ice->ctrl);
}

extern inline int ice_poll(const ice_drv_t * ice, ice_comm_t * comm) {
	return ice->op.poll(ice->ctrl, comm);
}

extern inline void ice_signal(const ice_drv_t * ice, ice_sig_t sig) {
	ice->op.signal(ice->ctrl, sig);
}

extern inline int ice_halt_wait(const ice_drv_t * ice, int mutex, int tmo) {
	return ice->op.halt_wait(ice->ctrl, mutex, tmo);
}

extern inline int ice_connect(const ice_drv_t * ice, uint32_t idmask, 
							  uint32_t idcomp) {
	return ice->op.connect(ice->ctrl, idmask, idcomp);
}

extern inline int ice_release(const ice_drv_t * ice) {
	return ice->op.release(ice->ctrl);
}

extern inline int ice_rd32(const ice_drv_t * ice, 
						   uint32_t addr, uint32_t * val) {
	return ice->op.rd32(ice->ctrl, addr, val);
}

extern inline int ice_wr32(const ice_drv_t * ice, 
						   uint32_t addr, uint32_t val) {
	return ice->op.wr32(ice->ctrl, addr, val);
}

extern inline int ice_rd16(const ice_drv_t * ice, 
						   uint32_t addr, uint16_t * val) {
	return ice->op.rd16(ice->ctrl, addr, val);
}

extern inline int ice_wr16(const ice_drv_t * ice, 
						   uint32_t addr, uint16_t val) {
	return ice->op.wr16(ice->ctrl, addr, val);
}

extern inline int ice_rd8(const ice_drv_t * ice, 
						  uint32_t addr, uint8_t * val) {
	return ice->op.rd8(ice->ctrl, addr, val);
}

extern inline int ice_wr8(const ice_drv_t * ice, 
						  uint32_t addr, uint8_t val) {
	return ice->op.wr8(ice->ctrl, addr, val);
}

extern inline int ice_fifo_rd8(const ice_drv_t * ice,
							   uint32_t addr, void * buf, int len) {
	return ice->op.fifo_rd8(ice->ctrl, addr, buf, len);
}

extern inline int ice_fifo_wr8(const ice_drv_t * ice,
							   uint32_t addr, const void * buf, int len) {
	return ice->op.fifo_wr8(ice->ctrl, addr, buf, len);
}

extern inline int ice_mem_lock(const ice_drv_t * ice) {
	return ice->op.mem_lock(ice->ctrl);
}

extern inline int ice_mem_unlock(const ice_drv_t * ice) {
	return ice->op.mem_unlock(ice->ctrl);
}

extern inline int ice_halt_req(const ice_drv_t * ice) {
	return ice->op.halt(ice->ctrl);
}

extern inline int ice_run(const ice_drv_t * ice) {
	return ice->op.run(ice->ctrl);
}

extern inline int ice_step(const ice_drv_t * ice) {
	return ice->op.step(ice->ctrl);
}

extern inline int ice_exec(const ice_drv_t * ice, uint32_t addr, 
						   uint32_t * regv, int regc) {
	return ice->op.exec(ice->ctrl, addr, regv, regc);
}

extern inline int ice_context_show(const ice_drv_t * ice, FILE * f) {
	return ice->op.context_show(ice->ctrl, f);
}

extern inline int ice_reg_get(const ice_drv_t * ice, int reg, uint32_t * val) {
	return ice->op.reg_get(ice->ctrl, reg, val);
}

extern inline int ice_reg_set(const ice_drv_t * ice, int reg, uint32_t val) {
	return ice->op.reg_set(ice->ctrl, reg, val);
}

/*
extern inline int ice_pc_get(const ice_drv_t * ice, uint32_t * val) {
	return ice->op.pc_get(ice->ctrl, val);
}

extern inline int ice_pc_set(const ice_drv_t * ice, uint32_t val) {
	return ice->op.pc_set(ice->ctrl, val);
}

extern inline int ice_sp_get(const ice_drv_t * ice, uint32_t * val) {
	return ice->op.sp_get(ice->ctrl, val);
}

extern inline int ice_sp_set(const ice_drv_t * ice, uint32_t val) {
	return ice->op.sp_set(ice->ctrl, val);
}
*/

extern inline int ice_ifa_get(const ice_drv_t * ice, uint32_t * val) {
	return ice->op.ifa_get(ice->ctrl, val);
}

extern inline int ice_ifa_set(const ice_drv_t * ice, uint32_t val) {
	return ice->op.ifa_set(ice->ctrl, val);
}

extern inline int ice_goto(const ice_drv_t * ice, uint32_t addr) {
	return ice->op.go_to(ice->ctrl, addr);
}

#if 0
extern inline int ice_int_enable(const ice_drv_t * ice) {
	return ice->op.int_enable(ice->ctrl);
}

extern inline int ice_int_disable(const ice_drv_t * ice) {
	return ice->op.int_disable(ice->ctrl);
}
#endif

extern inline int ice_core_reset(const ice_drv_t * ice) {
	return ice->op.core_reset(ice->ctrl);
}

extern inline int ice_system_reset(const ice_drv_t * ice) {
	return ice->op.system_reset(ice->ctrl);
}

extern inline int ice_bp_set(const ice_drv_t * ice, uint32_t addr, 
							 uint32_t len, uint32_t * id)
{
	return ice->op.bp_set(ice->ctrl, addr, len, id);
}

extern inline int ice_bp_clr(const ice_drv_t * ice, uint32_t id)
{
	return ice->op.bp_clr(ice->ctrl, id);
}

extern inline int ice_wp_set(const ice_drv_t * ice, uint32_t addr, 
							 uint32_t len, uint32_t * id)
{
	return ice->op.wp_set(ice->ctrl, addr, len, id);
}

extern inline int ice_wp_clr(const ice_drv_t * ice, uint32_t id)
{
	return ice->op.wp_clr(ice->ctrl, id);
}


extern inline int ice_info(const ice_drv_t * ice, FILE * f, uint32_t which)
{
	return ice->op.info(ice->ctrl, f, which);
}

extern inline int ice_test(const ice_drv_t * ice, FILE * f, 
						   uint32_t req, uint32_t argc, uint32_t argv[])
{
	return ice->op.test(ice->ctrl, f, req, argc, argv);
}

extern inline int ice_print_insn(const ice_drv_t * ice, uint32_t addr, 
								 struct disassemble_info * info)
{
	return ice->op.print_insn(ice->ctrl, addr, info);
}

#if 0
extern inline int ice_comm_open(const ice_drv_t * ice)
{
	return ice->comm_open(ice->ctrl);
}
	
extern inline int ice_comm_close(const ice_drv_t * ice)
{
	return ice->comm_close(ice->ctrl);
}

extern inline int ice_comm_read(const ice_drv_t * ice, void * buf, int len) 
{
	return ice->comm_read(ice->ctrl, buf, len);
}

extern inline int ice_comm_write(const ice_drv_t * ice, 
								 const void * buf, int len) 
{
	return ice->comm_write(ice->ctrl, buf, len);
}
#endif

#ifdef __cplusplus
extern "C" {
#endif

int ice_open(ice_drv_t * ice, const ice_drv_info_t * info, ice_ctrl_t * ctrl);

int ice_mem_read(const ice_drv_t * ice, const ice_mem_entry_t * mem, 
				 ice_addr_t addr, void * ptr, ice_size_t len);

int ice_mem_write(const ice_drv_t * ice, const ice_mem_entry_t * mem, 
				  ice_addr_t addr, const void * ptr, ice_size_t len);

int ice_mem_erase(const ice_drv_t * ice, const ice_mem_entry_t * mem, 
				  ice_addr_t addr, ice_size_t len);

struct ice_mem_entry * ice_mem_by_name(const ice_drv_t * ice, 
									   const ice_mem_entry_t * mem_map, 
									   const char * name);

int ice_mem_lookup(const ice_drv_t * ice, 
					const ice_mem_entry_t * mem, 
					const char * name);

int ice_rd32(const ice_drv_t * ice, uint32_t addr, uint32_t * val);
int ice_wr32(const ice_drv_t * ice, uint32_t addr, uint32_t val);

int ice_rd16(const ice_drv_t * ice, uint32_t addr, uint16_t * val);
int ice_wr16(const ice_drv_t * ice, uint32_t addr, uint16_t val);

int ice_rd8(const ice_drv_t * ice, uint32_t addr, uint8_t * val);
int ice_wr8(const ice_drv_t * ice, uint32_t addr, uint8_t val);

char * ice_strerror(const ice_drv_t * ice, int errno);

#ifdef __cplusplus
	}
#endif

#endif /* __ICE_DRV_H__ */

