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

#include <sys/os.h>
#include <stdbool.h>

#include "target.h"
#include "ice_drv.h"
#include "nand.h"

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

/*
 * Hardware breakpoint type
 */
 
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

struct dbg_wp {
	union {
		struct dbg_wp * next;
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

struct dbg_wp_ctrl {
	uint16_t cnt;
	struct dbg_wp ** lst;
	struct dbg_wp * free;
};

#define DBG_STACK_MAX 64
#define DBG_CACHE_MAX 64
#define DBG_TRACE_MAX 2

struct debugger {
	/* Target ice driver */
	ice_drv_t ice;

	/* configuration */
	struct {
		bool enable_ice_polling;
	} cfg;

	dbg_state_t state;

	/* target memory map cache */
	const ice_mem_entry_t * mem;

	/* Target info */
	const target_info_t * target;

	/* XXX: memory module id */
	int16_t mem_mod_id;

	uint16_t tcp_port;

	uint8_t ext_pwr;

	int busy;

	struct mem_range dasm;
	struct mem_range dump;
	struct mem_range stack;
	struct mem_range transf;

	struct dbg_bp_ctrl bp_ctrl;
	struct dbg_wp_ctrl wp_ctrl;

	/* poll control */
	int halt_cond;
	int poll_cond;
	int poll_thread;
	int poll_mutex;

	volatile bool poll_enabled;
	volatile bool poll_stop_req;
	volatile bool poll_start_req;

	/* debug communication control */
	ice_comm_t comm;
};

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

int target_pc_get(uint32_t * val);

int target_pc_set(uint32_t val);

int target_sp_get(uint32_t * val);

int target_sp_set(uint32_t val);

/* Instruction Fetch Address (IFA) get */
int target_ifa_get(uint32_t * val);

/* Instruction Fetch Address (IFA) set */
int target_ifa_set(uint32_t val);


int target_reset(FILE * f, int mode);

int target_init(FILE * f);

int target_breakpoint_get(struct dbg_bp * bp, struct dbg_bp ** next);

int target_breakpoint_set(uint32_t addr, uint32_t size);

int target_breakpoint_clear(uint32_t addr, uint32_t size);

int target_breakpoint_enable(uint32_t addr, uint32_t size);

int target_breakpoint_disable(uint32_t addr, uint32_t size);

int target_breakpoint_delete(struct dbg_bp * bp);

int target_breakpoint_all_disable(void);

int target_breakpoint_all_enable(void);



int target_watchpoint_get(struct dbg_wp * wp, struct dbg_wp ** next);

int target_watchpoint_set(uint32_t addr, uint32_t mask);

int target_watchpoint_clear(uint32_t addr, uint32_t mask);

int target_watchpoint_enable(uint32_t addr, uint32_t size);

int target_watchpoint_disable(uint32_t addr, uint32_t size);

int target_watchpoint_delete(struct dbg_wp * wp);

int target_watchpoint_all_disable(void);

int target_watchpoint_all_enable(void);



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

/*
int target_insn_fetch(uint32_t addr, void * insn);

int target_insn_show(FILE * f, uint32_t addr, void * insn);
*/

int target_print_insn(FILE * f, uint32_t addr);

int target_test(FILE * f, uint32_t val);

int target_eval(ice_val_t * val, int argc, char * argv[]);

int target_probe(FILE * f);

struct target_info * get_target_info(void);

int target_halt_wait(int tmo);

int target_ice_info(FILE * f, uint32_t which);

/* NAND operations */

int target_nand_bb_check(uint32_t block);

int target_nand_block_erase(uint32_t block, bool force);

int target_nand_dev_get(int dev_id, nand_dev_t ** nandp);

int target_nand_chip_get(int dev_id, int chip_id, nand_chip_t ** chipp);

int target_enable_ice_poll(bool flag);

int target_enable_comm(bool flag);

void debugger_init(void);

void debugger_except(const char * msg);

#ifdef __cplusplus
	}
#endif

#endif /* __DEBUGGER_H__ */

