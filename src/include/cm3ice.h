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
 * @file cm3ice.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */



#ifndef __CM3ICE_H__
#define __CM3ICE_H__

#include "arm-v7m.h" 

/* ARM ICE errors */
#define CM3ICE_OK                  0
#define CM3ICE_ERR_NONE            0
#define CM3ICE_ERR_IR_INTEST       1
#define CM3ICE_ERR_IR_RESTART      2
#define CM3ICE_ERR_IR_IDCODE       3
#define CM3ICE_ERR_IR_BYPASS       4
#define CM3ICE_ERR_IR_SCAN_N       5
#define CM3ICE_ERR_DR_SCAN_CHAIN   6
#define CM3ICE_ERR_DR_IDCODE       7
#define CM3ICE_ERR_DCC_WR          8
#define CM3ICE_ERR_DCC_RD          9
#define CM3ICE_ERR_DCC_TIMEOUT     10
#define CM3ICE_ERR_BYPASS_TEST     11
#define CM3ICE_ERR_IDCODE_INVALID  12
#define CM3ICE_ERR_BREAK_TIMEOUT   13
#define CM3ICE_ERR_SYSTEM_FAILURE  14
#define CM3ICE_ERR_INVALID_CPU     15
#define CM3ICE_ERR_INVALID_IDCODE  16
#define CM3ICE_ERR_SYNC_TIMEOUT    17
#define CM3ICE_ERR_CONNECT_TIMEOUT 18
#define CM3ICE_ERR_INVALID_WP      19
#define CM3ICE_ERR_HARDWARE        20

#define CM3_R0    0
#define CM3_R1    1
#define CM3_R2    2
#define CM3_R3    3
#define CM3_R4    4
#define CM3_R5    5
#define CM3_R6    6
#define CM3_R7    7
#define CM3_R8    8
#define CM3_R9    9
#define CM3_R10   10
#define CM3_R11   11
#define CM3_R12   12
#define CM3_SP    13
#define CM3_LR    14
#define CM3_PC    15
#define CM3_XPSR  16 
#define CM3_MSP   17
#define CM3_PSP   18
#define CM3_CTRL  19
#define CM3_FPSCR 20
#define CM4_S0    21
#define CM4_S1    22
#define CM4_S2    23
#define CM4_S3    24
#define CM4_S4    25
#define CM4_S5    26
#define CM4_S6    27
#define CM4_S7    28
#define CM4_S8    29
#define CM4_S9    30
#define CM4_S10   31
#define CM4_S11   32
#define CM4_S12   33
#define CM4_S13   34
#define CM4_S14   35
#define CM4_S15   36
#define CM4_S16   37
#define CM4_S17   38
#define CM4_S18   39
#define CM4_S19   41
#define CM4_S20   42
#define CM4_S21   43
#define CM4_S22   44
#define CM4_S23   45
#define CM4_S24   46
#define CM4_S25   47
#define CM4_S26   48
#define CM4_S27   49
#define CM4_S28   50
#define CM4_S29   51
#define CM4_S30   52
#define CM4_S31   53

#define CM3_CONTROL   54
#define CM3_FAULTMASK 56
#define CM3_BASEPRI   57
#define CM3_PRIMASK   58

#ifdef __ASSEMBLER__


#else /* __ASSEMBLER__ */

#include <stdint.h>
#include <stdbool.h>
#include "jtag_adi.h" 
#include "ice_drv.h" 
#include "arm_opc.h"
#include "arm_pn.h"
#include "module.h"

/* flags */
#define CM3ICE_CTX_SAVED      (1 << 0)
#define CM3ICE_MEM_LOCK       (1 << 1)
#define CM3ICE_RESUME         (1 << 2)
#define CM3ICE_OPENED         (1 << 3)
#define CM3ICE_CONNECTED      (1 << 4)

struct cm3ice_codelet {
	uint32_t size;
	uint32_t code[];
};

typedef struct cm3ice_codelet cm3ice_codelet_t;

struct cm3ice_context {
	uint32_t r[13];
	uint32_t sp;
	uint32_t lr;
	uint32_t pc;
	uint32_t xpsr;
	uint32_t msp;
	uint32_t psp;
	uint32_t ctrl;
};

struct cm3ice_insn {
	uint8_t len;
	uint8_t thumb:1;
	uint32_t data;
};

typedef struct cm3ice_context cm3ice_context_t;

struct cm3ice_cfg {
	uint8_t endianness:1;
};

typedef struct cm3ice_cfg cm3ice_cfg_t;

struct armv7m_fpb_ctrl {
	uint32_t base; /* FBP unit base address */
	uint8_t code_max; /* number of code comparators */
	uint8_t lit_max; /* number of literal comparators */
	uint32_t code_bmp; /* code comparators allocation bitmap */
	uint32_t lit_bmp; /* literal comparators allocation bitmap */ 
	uint32_t code_base; /* base address of code comparators */
	uint32_t lit_base; /* base address of literal comparators */
};

typedef struct armv7m_fpb_ctrl armv7m_fpb_ctrl_t;

struct armv7m_dwt_ctrl {
	uint32_t base; /* DWT unit base address */
	uint8_t numcomp; /* number of comparators */
	uint32_t comp_bmp; /* allocation bitmap */
};

typedef struct armv7m_dwt_ctrl armv7m_dwt_ctrl_t;


struct cm3_core_ctrl {
	uint32_t reg[CM4_S31 + 1]; 
	uint64_t cache_bmp; /* cache bitmap */
	uint64_t wback_bmp; /* write back bitmap */
};

typedef struct cm3_core_ctrl cm3_core_ctrl_t;

#define DEV_SYNC 0x43
#define DEV_CONNECTED 0x65

#define DBG_CONNECTED 1
#define DBG_SYNC 2

struct cm3_comm_ctrl {
	union {
		struct {
			uint16_t tx_head;	
			uint8_t rx_tail;	
			uint8_t dev;	
		};
		uint32_t u32;
	} ro;
	union {
		struct {
			uint16_t tx_tail;	
			uint8_t rx_head;	
			uint8_t dbg;	
		};
		uint32_t u32;
	} rw;
};


/*
 * cm3ice control structure
 */
struct cm3ice_ctrl {
	/* this field must be on the head of the structure */
	ice_drv_t * ice;

	uint32_t flags;

	jtag_tap_t * tap;

	volatile bool jtag_lock;
	volatile bool polling;
	volatile bool poll_enabled;
	uint32_t dhcsr;
	uint32_t comm_addr;

	struct cm3_comm_ctrl cc; /* cached data */

	int mod_id;

	cm3_core_ctrl_t core;
	armv7m_fpb_ctrl_t fpb;
	armv7m_dwt_ctrl_t dwt;

	struct armv7m_map dbg_map;
};

typedef struct cm3ice_ctrl cm3ice_ctrl_t;

extern const struct ice_drv_info cm3ice_drv;

extern const struct module_def cm3ice_module;

#ifdef __cplusplus
extern "C" {
#endif

int cm3_show_regs(FILE * f, uint32_t * reg);

int cm3_show_fpu_regs(FILE * f, uint32_t * reg);

int cm3ice_mem_read(cm3ice_ctrl_t * ctrl, ice_mem_ref_t * addr, 
					 void * p, ice_size_t len);
int cm3ice_mem_write(cm3ice_ctrl_t * ctrl, ice_mem_ref_t * addr, 
					 const void * p, ice_size_t len);
int cm3ice_mem_zero(cm3ice_ctrl_t * ctrl, 
					ice_mem_ref_t * addr, ice_size_t len);

int cm3ice_code_load(cm3ice_ctrl_t * ctrl, uint32_t addr, 
					 uint32_t * ptr, int size);

int cm3ice_code_exec(cm3ice_ctrl_t * ctrl, uint32_t addr, 
					 uint32_t * regv, int regc);

int cm3ice_code_stop(cm3ice_ctrl_t * ctrl);

int cm3ice_code_resume(cm3ice_ctrl_t * ctrl);

int cm3ice_wr8(cm3ice_ctrl_t * ctrl, uint32_t addr, uint8_t data);

int cm3ice_rd8(cm3ice_ctrl_t * ctrl, uint32_t addr, uint8_t * data);

int cm3ice_wr16(cm3ice_ctrl_t * ctrl, uint32_t addr, uint16_t data);

int cm3ice_rd16(cm3ice_ctrl_t * ctrl, uint32_t addr, uint16_t * data);

int cm3ice_wr32(cm3ice_ctrl_t * ctrl, uint32_t addr, uint32_t data);

int cm3ice_rd32(cm3ice_ctrl_t * ctrl, uint32_t addr, uint32_t * data);

int cm3ice_thumb_mode(cm3ice_ctrl_t * ctrl, int thumb);

int cm3ice_reg_get(cm3ice_ctrl_t * ctrl, int reg, uint32_t * val);

int cm3ice_reg_set(cm3ice_ctrl_t * ctrl, int reg, uint32_t val);



#ifdef __cplusplus
}
#endif

#endif /* __ASSEMBLER__ */

#endif /* __CM3ICE_H__ */

