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
 * @file target.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */



#ifndef __TARGET_H__
#define __TARGET_H__

#include <stdint.h>
#include <stdlib.h>
#include <ice_drv.h>

#define LITTLE_ENDIAN 0
#define BIG_ENDIAN 1

#define TARGET_SYM_NAME_MAX 17

typedef unsigned char target_sym_size_t;
typedef uint32_t target_sym_val_t;

typedef enum {
	_XX_ = 0,
	_RO_ = 1,
	_WO_ = 2,
	_RW_ = 3
} target_sym_access_t;

struct target_sym {
	ice_mem_addr_t addr;
	char name[TARGET_SYM_NAME_MAX + 1]; 
	target_sym_size_t size;
	target_sym_access_t access;
};

typedef const struct target_sym * target_sym_map_t;

/* struct target_attr { */
struct target_conf {
	ice_mem_addr_t addr;
	target_sym_val_t val;
};

typedef struct target_conf target_conf_t;
typedef const struct target_conf * target_conf_set_t;

#define TARGET_ARCH_VENDOR_MAX 11
#define TARGET_ARCH_NAME_MAX 15
#define TARGET_ARCH_MODEL_MAX 11

#define TARGET_CPU_VENDOR_MAX 11
#define TARGET_CPU_FAMILY_MAX 11
#define TARGET_CPU_MODEL_MAX 15

typedef struct target_info target_info_t;

typedef int (* target_script_t)(FILE * f, const ice_drv_t * drv, 
								ice_mem_entry_t const * mem_map);

typedef int (* target_config_t)(FILE * f, const ice_drv_t * drv,
								const target_info_t * target); 

typedef int (* target_test_t)(FILE * f, const ice_drv_t * drv, 
								ice_mem_entry_t const * mem_map, uint32_t val);

/******************************************************************************
 * CPU
 *****************************************************************************/

struct target_cpu {
	char family[TARGET_CPU_FAMILY_MAX + 1];
	char model[TARGET_CPU_MODEL_MAX + 1];
	char vendor[TARGET_CPU_VENDOR_MAX + 1];
	uint8_t irlength;
	uint32_t idmask;
	uint32_t idcomp;
	target_sym_map_t sym;
};

typedef struct target_cpu target_cpu_t;

/******************************************************************************
 * Arch
 *****************************************************************************/

struct target_arch {
	char name[TARGET_ARCH_NAME_MAX + 1];
	char model[TARGET_ARCH_MODEL_MAX + 1];
	char vendor[TARGET_ARCH_VENDOR_MAX + 1];
	target_cpu_t const * cpu;
	target_sym_map_t sym;
};

typedef struct target_arch target_arch_t;

#if 0
#define TARGET_MEM_NAME_MAX 13

/* Target memory description */
struct target_mem {
	char name[TARGET_MEM_NAME_MAX + 1]; 
	short count;
	/* Memory map */
	ice_mem_map_t map[];
};

typedef const struct target_mem target_mem_t;
#endif

#define TARGET_NAME_MAX 15
#define TARGET_DESC_MAX 63

#define YES 1
#define NO 0

typedef enum {
	RST_AUTO = 0, /* automatically select the reset mode 
					 based on target's configuration */
	RST_HARD = 1,
	RST_SOFT = 2,
	RST_CORE = 3, /* core only */
	RST_DBG = 4, /* debugger logic only */
	RST_SYS = 5 /* system */
} reset_mode_t;

struct target_info {
	char name[TARGET_NAME_MAX + 1];
	char desc[TARGET_DESC_MAX + 1];

	target_arch_t const * arch;
//	target_cpu_t const * cpu;
	ice_mem_entry_t const * mem;

	ice_drv_info_t const * ice_drv;
	void * ice_cfg;

	/* JTAG clock frequency (Hz)*/
	uint32_t jtag_clk_slow;
	uint32_t jtag_clk_def;
	uint32_t jtag_clk_max;

	/* The target has a TRST connection */
	uint32_t has_trst:1;
	/* The target has a nRST connection */
	uint32_t has_nrst:1;
	/* Target supports adaptive clock */
	uint32_t has_rtck:1;
	/* The preferred clock method is adaptive (RTCK) */
	uint32_t prefer_rtck:1;
	/* Start with slow clock */
	uint32_t clk_slow_on_connect:1;
	/* Set default clock after init  */
	uint32_t clk_def_on_init:1;
	/* probe the JTAG scan chain */
	uint32_t jtag_probe:1;
	/*  */
	uint32_t reset_on_config:1;

	/* default reset mode */
	uint8_t reset_mode;

	ice_mem_addr_t start_addr;

	target_sym_map_t sym;

	/* this callbacak allows to configure the JTACG chain
	   for a specific target. This is called once 
	   before to the ICE configuration */
	target_config_t pre_config;

	/* this callbacak allows dinamic configuration of the
	   target prior to connection. This is called once
	   after to the ICE configuration */
	target_config_t pos_config;

	target_script_t on_init;
	target_script_t on_halt;
	target_script_t on_run;
	target_script_t reset_script;
	target_script_t probe;
	target_test_t test;
};

extern const struct target_cpu * const cpu_db[];
extern const struct target_arch * const arch_db[];
extern const struct target_mem * const mem_db[];

extern const struct target_sym void_sym[];

extern const struct ice_mem_entry target_null_mem[];
extern const struct target_arch target_null_arch;
extern const struct target_cpu target_null_cpu;
extern const struct target_info target_null;


extern const struct target_info * const target_db[];

#ifdef __cplusplus
extern "C" {
#endif

struct target_info * target_lookup(const char * s);

struct target_info * target_first(void);
struct target_info * target_next(struct target_info * tp);

int target_sym_lookup(const struct target_info * target, const char * name, 
					  struct target_sym ** sp);

int dummy_probe(FILE * f, const struct ice_drv * ice, 
				ice_mem_entry_t * mem);

int mem_hexdump(FILE * f, uint32_t addr, int size);

#ifdef __cplusplus
	}
#endif

#endif /* __TARGET_H__ */

