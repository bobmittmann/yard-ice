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
 * @file debugger.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#ifndef __DEBUGGER_H__
#define __DEBUGGER_H__

#include <thinkos.h>
#include <stdbool.h>

#include "nand.h"
#include "target.h"

/*
 * define a generic breakpoint controller type
 */
#include "bkptctrl.h"

typedef enum {
	OK = 0,
	ERR_UNDEF = -1,
	ERR_PARM = -2,
	ERR_BUSY = -3,
	ERR_STATE = -4,
	ERR_DRV_FAIL = -5,
	ERR_TIMEOUT = -6,
	ERR_NULL_TARGET = -7,
	ERR_OUTOFSYNC = -8,
	ERR_JTAG_TAP_INVALID = -9,
	ERR_JTAG_IR_LEN = -10,
	ERR_PROBE_INVALID = -11
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
	DBG_ST_HALTED = 4,
	DBG_ST_LOCKUP = 5
} dbg_state_t;

struct mem_range {
	uint32_t base;
	uint32_t size;
};

struct debugger;

extern struct debugger debugger;

#ifdef __cplusplus
extern "C" {
#endif

int target_status(void);

int target_ice_configure(FILE * f, const struct target_info * target, 
						 int force);

int target_config(FILE * f);

int target_mem_write(uint32_t addr, const void * ptr, int len);

int target_mem_read(uint32_t addr, void * ptr, int len);

int target_mem_erase(uint32_t addr, int len);

int target_connect(int force);

int target_halt(int force);

int target_run(void);

int target_step(void);

int target_release(void);

int target_goto(uint32_t addr, int opt);


int target_register_get(int reg, uint32_t * val);

int target_register_set(int reg, uint32_t val);

int target_sp_get(uint32_t * val);

int target_sp_set(uint32_t val);

int target_stack_refresh(void * buf, size_t len);

/* Instruction Fetch Address (IFA) get */
int target_ifa_get(uint32_t * val);

/* Instruction Fetch Address (IFA) set */
int target_ifa_set(uint32_t val);

int target_enable_debug(bool en);

int target_enable_irq(bool en);

int target_reset(FILE * f, int mode);

int target_init(FILE * f);



struct bkpt_ctrl * target_bkpt_next(struct bkpt_ctrl * p, unsigned int type);

struct bkpt_ctrl * target_bkpt_head(unsigned int type);

int target_bkpt_set(uint32_t addr, uint32_t size, unsigned int type);

int target_bkpt_clear(uint32_t addr, uint32_t size, unsigned int type);

int target_bkpt_enable(uint32_t addr, uint32_t size, unsigned int type);

int target_bkpt_disable(uint32_t addr, uint32_t size, unsigned int type);

int target_bkpt_disable_all(void);

int target_bkpt_enable_all(void);

int target_exp(char * sym, uint32_t offs, uint32_t * vp);

int target_send(int data);

char * target_strerror(int errno);

int fmt_line_word(char * s, uint32_t addr, uint32_t * ptr, int count);

void target_poll(void);

int target_tap_reset(void);

int target_ice_test(FILE * f, uint32_t req, uint32_t argc, uint32_t argv[]);

#define TARGET_IO_CLR 0
#define TARGET_IO_SET 1
#define TARGET_IO_PULSE 2

int target_tap_trst(unsigned int mode);

int target_nrst(unsigned int mode, unsigned int ms);

int target_power_ctl(bool on);

int target_power_stat(FILE * f);

int target_relay(bool on);

//int context_register_get(struct ice_context * ct, int reg, uint32_t * val);

/* 
 * resolve target names to address
 */
uint32_t target_strtoaddr(const char * s);

int target_int_enable(void);

int target_int_disable(void);

int target_context_show(FILE * f);

int target_stack_show(FILE * f);

int target_fpu_context_show(FILE * f);

/*
int target_insn_fetch(uint32_t addr, void * insn);

int target_insn_show(FILE * f, uint32_t addr, void * insn);
*/

int target_print_insn(FILE * f, uint32_t addr);

int target_test(FILE * f, uint32_t val);

int target_probe(FILE * f);

struct target_info * get_target_info(void);

int target_halt_wait(int tmo);

void target_halt_wait_break(void);

int target_ice_info(FILE * f, uint32_t which);

/* NAND operations */
int target_nand_chip_get(int dev_id, int chip_id, nand_chip_t ** chipp);

int target_nand_bb_check(uint32_t block);

int target_nand_block_erase(uint32_t block, bool force);

int target_enable_ice_poll(bool flag);

int target_enable_comm(bool flag);

ice_comm_t * target_comm(void);

void debugger_init(void);

void debugger_except(const char * msg);

void target_fault_clr(void);

struct mem_range * target_dasm_range(void);

struct mem_range * target_dump_range(void);

struct mem_range * target_stack_range(void);

struct mem_range * target_xfer_range(void);

ice_mem_entry_t * target_mem_tab(void);

ice_drv_t * target_ice_drv(void);

target_info_t * target_info(void);

ice_mem_entry_t * target_mem_by_name(const char * name);

#ifdef __cplusplus
	}
#endif

#endif /* __DEBUGGER_H__ */

