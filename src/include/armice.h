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
 * @file armice.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#ifndef __ARMICE_H__
#define __ARMICE_H__


/* ARM ICE Public Instructions */
#define ARMICE_IR_EXTEST        0x00
#define ARMICE_IR_SCAN_N        0x02
#define ARMICE_IR_SAMPLE        0x03
#define ARMICE_IR_PRELOAD       0x03
#define ARMICE_IR_RESTART       0x04
#define ARMICE_IR_CLAMP         0x05
#define ARMICE_IR_HIGHZ         0x07
#define ARMICE_IR_CLAMPZ        0x09
#define ARMICE_IR_INTEST        0x0c
#define ARMICE_IR_IDCODE        0x0e
#define ARMICE_IR_BYPASS        0x0f

/* ARM ICE Scan Chains */
#define ARMICE_SCAN_CHAIN_0     0 /* Reserved */
#define ARMICE_SCAN_CHAIN_1     1 /* Debug */
#define ARMICE_SCAN_CHAIN_2     2 /* EmbeddedICE-RT programming */
#define ARMICE_SCAN_CHAIN_3     3 /* External boundary scan */
#define ARMICE_SCAN_CHAIN_15   15 /* ARM926EJ-S CP15 */


/* ARM ICE EmbeddedICE-RT logic register map */
#define ARMICE_DBG_CONTROL      0x00
#define ARMICE_DBG_STATUS       0x01
#define ARMICE_VEC_CATCH_CTRL   0x02
#define ARMICE_DCC_CONTROL      0x04
#define ARMICE_DCC_DATA         0x05

#define ARMICE_WP0_ADDR_VALUE   0x08
#define ARMICE_WP0_ADDR_MASK    0x09
#define ARMICE_WP0_DATA_VALUE   0x0a
#define ARMICE_WP0_DATA_MASK    0x0b
#define ARMICE_WP0_CTRL_VALUE   0x0c
#define ARMICE_WP0_CTRL_MASK    0x0d

#define ARMICE_WP1_ADDR_VALUE   0x10
#define ARMICE_WP1_ADDR_MASK    0x11
#define ARMICE_WP1_DATA_VALUE   0x12
#define ARMICE_WP1_DATA_MASK    0x13
#define ARMICE_WP1_CTRL_VALUE   0x14
#define ARMICE_WP1_CTRL_MASK    0x15

#define ARMICE_DCC_READ_FLAG    0x01
#define ARMICE_DCC_WRITE_FLAG   0x02

#define ARMICE_DBG_WRITE        0x20
#define ARMICE_DBG_READ         0x00

/* Debug status register */
#define ARMICE_ST_DBGACK        (1 << 0)
#define ARMICE_ST_DBGRQ         (1 << 1)
#define ARMICE_ST_IFEN          (1 << 2)
#define ARMICE_ST_NMREQ         (1 << 3) /* ARM7 */
#define ARMICE_ST_SYSCOMP       (1 << 3) /* ARM9EJ-S */
#define ARMICE_ST_TBIT          (1 << 4)
#define ARMICE_ST_ITBIT         (1 << 4) /* ARM9EJ-S */
#define ARMICE_ST_IJBIT         (1 << 5) /* ARM9EJ-S */
#define ARMICE_ST_MOE(DBGST)    (((DBGST) >> 6) & (0xf)) /* ARM9EJ-S */

/* ARM9EJ-S debug method of entry (MODE bits) */
#define ARMICE_MOE_NODBG       (0x0) /* No debug entry */
#define ARMICE_MOE_BP0         (0x1) /* Breakpoint from EICE unit 0 */
#define ARMICE_MOE_BP1         (0x2) /* Breakpoint from EICE unit 1 */
#define ARMICE_MOE_BKPT        (0x3) /* Soft breakpoint */
#define ARMICE_MOE_VEC         (0x4) /* Vector catch breakpoint */
#define ARMICE_MOE_EXTBP       (0x5) /* External breakpoint */
#define ARMICE_MOE_WP0         (0x6) /* Watchpoint from EICE unit 0 */
#define ARMICE_MOE_WP1         (0x7) /* Watchpoint from EICE unit 1 */
#define ARMICE_MOE_EXTWP       (0x8) /* External watchpoint */
#define ARMICE_MOE_INTDBGRQ    (0x9) /* Internal debug request */
#define ARMICE_MOE_EXTDBGRQ    (0xa) /* External debug request */
#define ARMICE_MOE_DBGREENTRY  (0xb) /* Debug reentry from 
										 system speed access  */


#define ARMICE_ST_MASK (ARMICE_ST_DBGACK | ARMICE_ST_DBGRQ | \
						ARMICE_ST_IFEN | ARMICE_ST_NMREQ | ARMICE_ST_TBIT )

/* Debug control register */
#define ARMICE_CTRL_DBGACK      (1 << 0)
#define ARMICE_CTRL_DBGRQ       (1 << 1)
#define ARMICE_CTRL_INTDIS      (1 << 2)
#define ARMICE_CTRL_SINGLE_STEP (1 << 3) /* ARM9 only */
#define ARMICE_CTRL_MONITOR_EN  (1 << 4) /* Monitor mode enable - ARM9 only */
#define ARMICE_CTRL_ICE_DISABLE (1 << 5) /* Embedded-ICE disable - ARM9 only */

/* Vector catch register - ARM9 only */
#define ARMICE_VEC_RESET        (1 << 0)
#define ARMICE_VEC_UNDEF        (1 << 1)
#define ARMICE_VEC_SWI          (1 << 2)
#define ARMICE_VEC_P_ABORT      (1 << 3)
#define ARMICE_VEC_D_ABORT      (1 << 4)
#define ARMICE_VEC_RSVD         (1 << 5) 
#define ARMICE_VEC_IRQ          (1 << 6) 
#define ARMICE_VEC_FIQ          (1 << 7) 

/* Watchpoint control registers */
/* ARM9EJ-S: the format of the control registers depends on how bit
   3 is programmed. For bit 3='1' the comparators examine the data 
   address, data and control signals.  */
#define ARMICE_WP_NRW           (1 << 0)
#define ARMICE_WP_MAS0          (1 << 1)
#define ARMICE_WP_MAS1          (1 << 2)
#define ARMICE_WP_NOPC          (1 << 3)
#define ARMICE_WP_NTRANS        (1 << 4)
#define ARMICE_WP_DNTRANS       (1 << 4) /* ARM9EJ-S */
#define ARMICE_WP_EXTERN        (1 << 5)
#define ARMICE_WP_DBGEXT        (1 << 5) /* ARM9EJ-S */
#define ARMICE_WP_CHAIN         (1 << 6)
#define ARMICE_WP_RANGE         (1 << 7)
#define ARMICE_WP_ENABLE        (1 << 8)

/* ARM9EJ-S bit 3='0' the comparators examine the instruction address, 
   instruction data, and instruction control signals. */
#define ARMICE_WP_ITBIT         (1 << 1) /* Thumb state bit compare */
#define ARMICE_WP_IJBIT         (1 << 2) /* Jazelle state bit compare */
#define ARMICE_WP_INTRANS       (1 << 4)

/* ARM926EJ-S CP15 bits */
#define ARMICE_CP15_WR          (1 << 15)
#define ARMICE_CP15_REG_MASK    ((0x7fff) << 1)
#define ARMICE_CP15_OPC1(OPC1)  (((OPC1) & 0x07) << 12)
#define ARMICE_CP15_OPC2(OPC2)  (((OPC2) & 0x07) << 9)
#define ARMICE_CP15_CRN(CRN)    (((CRN) & 0x0f) << 5)
#define ARMICE_CP15_CRM(CRM)    (((CRM) & 0x0f) << 1)
#define ARMICE_CP15_ACCESS      (1 << 0)

/* CP15 - Primary register allocation */
#define CP15_C0 0 /* ID codes (read-only): Processor ID, Cache, 
					 Tightly-coupled Memory and TLB type */
#define CP15_C1 1 /* Control bits (read/write): System Configuration 
					 Bits Control */
#define CP15_C2 2 /* Memory protection and control : Page Table Control */
#define CP15_C3 3 /* Memory protection and control : Domain Access Control */
#define CP15_C4 4 /* Memory protection and control : Reserved */
#define CP15_C5 5 /* Memory protection and control : Fault status  */
#define CP15_C6 6 /* Memory protection and control : Fault address  */
#define CP15_C7 7 /* Cache and write buffer: Cache/write buffer control */
#define CP15_C8 8 /* Memory protection and control TLB control */
#define CP15_C9 9 /* Cache and write buffer Cache lockdown */
#define CP15_C10 10 /* Memory protection and control TLB lockdown */
#define CP15_C11 11 /* Tightly-coupled Memory Control : DMA Control  */
#define CP15_C12 12 /* Reserved  */
#define CP15_C13 13 /* Process ID */
#define CP15_C14 14 /* Reserved */
#define CP15_C15 15 /* ARM926EJ-S: Test and Debug Registers */ 

#define OPC1(OPC)  (((OPC) & 0x07) << 12)
#define OPC2(OPC)  (((OPC) & 0x07) << 9)
#define CRN(REG)    (((REG) & 0x0f) << 5)
#define CRM(REG)    (((REG) & 0x0f) << 1)

/* Register 0: ID codes */
#define CP15_C0_IDCODE      (OPC1(0) | OPC2(0) | CRN(0) | CRM(0))
#define CP15_C0_CACHE_TYPE  (OPC1(0) | OPC2(1) | CRN(0) | CRM(0))
#define CP15_C0_TCM_TYPE    (OPC1(0) | OPC2(2) | CRN(0) | CRM(0))
#define CP15_C0_TLB_TYPE    (OPC1(0) | OPC2(3) | CRN(0) | CRM(0))
#define CP15_C0_MPU_TYPE    (OPC1(0) | OPC2(4) | CRN(0) | CRM(0))

/* Register 1: Control registers */
#define CP15_C1_CONTROL     (OPC1(0) | OPC2(0) | CRN(1) | CRM(0))
#define CP15_C1_AUX         (OPC1(0) | OPC2(1) | CRN(1) | CRM(0))
#define CP15_C1_CP_ACCESS   (OPC1(0) | OPC2(2) | CRN(1) | CRM(0))

/* CP15 - C15: Test and Debug registers */
/* Debug Override Register */
#define CP15_C15_DBG_OVR    (OPC1(0) | CRN(15) | CRM(0) | OPC2(0))
/* Debug and Test Address Register */
#define CP15_C15_DBG_ADDR   (OPC1(0) | CRN(15) | CRM(1) | OPC2(0))
/* Trace Control Register */
#define CP15_C15_TRC_CTRL   (OPC1(1) | CRN(15) | CRM(1) | OPC2(0))
/* Cache Debug Control Register */
#define CP15_C15_CACHE_DBG_CTRL  (OPC1(7) | CRN(15) | CRM(0) | OPC2(0))
/*
Cache Debug Control Register bit assignments:

	[2] DWB - Disable write-back (force WT) 
		0 = Enable write-back behavior
		1 = Force write-through behavior
	[1] DIL Disable ICache linefill
		0 = Enable ICache linefills
		1 = Disable ICache linefills
	[0] DDL - Disable DCache linefill
		0 = Enable DCache linefills
		1 = Disable DCache linefills
*/
#define CACHE_DBG_CTRL_DWB (1 << 2)
#define CACHE_DBG_CTRL_DIL (1 << 1)
#define CACHE_DBG_CTRL_DDL (1 << 0)


#define CP15_ADDR(O1, O2, RN, RM) (OPC1(O1) | OPC2(O2) | CRN(RN) | CRM(RM))


/* ARM ICE errors */
#define ARMICE_OK                  0
#define ARMICE_ERR_NONE            0
#define ARMICE_ERR_IR_INTEST       1
#define ARMICE_ERR_IR_RESTART      2
#define ARMICE_ERR_IR_IDCODE       3
#define ARMICE_ERR_IR_BYPASS       4
#define ARMICE_ERR_IR_SCAN_N       5
#define ARMICE_ERR_DR_SCAN_CHAIN   6
#define ARMICE_ERR_DR_IDCODE       7
#define ARMICE_ERR_DCC_WR          8
#define ARMICE_ERR_DCC_RD          9
#define ARMICE_ERR_DCC_TIMEOUT     10
#define ARMICE_ERR_BYPASS_TEST     11
#define ARMICE_ERR_IDCODE_INVALID  12
#define ARMICE_ERR_BREAK_TIMEOUT   13
#define ARMICE_ERR_SYSTEM_FAILURE  14
#define ARMICE_ERR_INVALID_CPU     15
#define ARMICE_ERR_INVALID_IDCODE  16
#define ARMICE_ERR_SYNC_TIMEOUT    17
#define ARMICE_ERR_CONNECT_TIMEOUT 18
#define ARMICE_ERR_INVALID_WP      19
#define ARMICE_ERR_HARDWARE        20

#ifdef __ASSEMBLER__


#else /* __ASSEMBLER__ */



#include <stdint.h>
#include <stdbool.h>
#include "jtag_arm.h" 
#include "ice_drv.h" 
#include "arm_opc.h"
#include "arm_pn.h"
#include "module.h"

/* flags */
#define ARMICE_CTX_SAVED      (1 << 0)
#define ARMICE_RESUME         (1 << 2)
#define ARMICE_DBGRQ          (1 << 3)
#define ARMICE_THUMB          (1 << 4)
#define ARMICE_SSTEP_INIT     (1 << 6)
#define ARMICE_SSTEP_EN       (1 << 7)
#define ARMICE_WP0_EN         (1 << 8)
#define ARMICE_WP1_EN         (1 << 9)
#define ARMICE_BRK_EN         (1 << 10)
#define ARMICE_WP0_SET        (1 << 11)
#define ARMICE_WP1_SET        (1 << 12)

#define ARMICE_OPENED         (1 << 16)
#define ARMICE_CONNECTED      (1 << 17)

struct armice_codelet {
	uint32_t size;
	uint32_t code[];
};

typedef struct armice_codelet armice_codelet_t;

struct armice_context {
	uint32_t r[13];
	uint32_t sp;
	uint32_t lr;
	uint32_t pc;
	uint32_t cpsr;
};

struct armice_insn {
	uint8_t len;
	uint8_t thumb:1;
	uint32_t data;
};

typedef struct armice_context armice_context_t;

struct armice_cfg {
	uint8_t endianness:1;
	uint32_t work_addr;
	uint32_t work_size;
};

typedef struct armice_cfg armice_cfg_t;

/*
 * armice control structure
 */
struct armice_ctrl {
	/* this field must be on the head of the structure */
	ice_drv_t * ice;
	/* module identifier */
	int8_t mod_id;
	/* memory lock counter */
	uint8_t mem_lock;
	/*  */
	uint32_t flags;

	jtag_tap_t * tap;

	/* watchpoint cache */
	jtag_arm_wp_t wp[2];

	/* poll control */
	jtag_arm_poll_t poll;

	/* flag that indicates whether we are polling or not */
	volatile bool polling;

	arm_pn_t pn;

	uint32_t dbg_status;


	/* working memory */
	uint32_t work_addr;
	uint32_t work_size;

	struct {
		uint32_t addr;
		uint32_t * ptr;
	} code_cache;

	struct {
		uint32_t base;
		uint32_t offs;
	} mem_cache;

	/* context cache */
	armice_context_t context;

	int (* jtag_arm_dbg_status)(jtag_tap_t * tap);
	int (* jtag_arm_dbgack_wait)(jtag_tap_t * tap, unsigned int tmo);

	int (* arm_exec)(jtag_tap_t * tap, int thumb);
	void (* arm_context_save)(jtag_tap_t * tap, unsigned int flags, 
							  armice_context_t * ct);
	void (* thumb_context_save)(jtag_tap_t * tap, unsigned int flags, 
								armice_context_t * ct);
	int (* arm_insn_step)(jtag_tap_t * tap, armice_context_t * ct);
	int (* thumb_insn_step)(jtag_tap_t * tap, armice_context_t * ct);
	void (* thumb_context_restore)(jtag_tap_t * tap, armice_context_t * ct);
	void (* arm_context_restore)(jtag_tap_t * tap, armice_context_t * ct);
	int (* arm_wr8)(jtag_tap_t * tap, uint32_t addr, uint8_t data);
	int (* arm_rd8)(jtag_tap_t * tap, uint32_t addr, uint8_t * data);
	int (* arm_wr16)(jtag_tap_t * tap, uint32_t addr, uint16_t data);
	int (* arm_rd16)(jtag_tap_t * tap, uint32_t addr, uint16_t * data);
	int (* arm_wr32)(jtag_tap_t * tap, uint32_t addr, uint32_t data);
	int (* arm_rd32)(jtag_tap_t * tap, uint32_t addr, uint32_t * data);
	int (* mem_set)(jtag_tap_t * tap, uint32_t base, 
				   uint32_t offs, int val, int len);
	int (* mem_write)(jtag_tap_t * tap, uint32_t base, 
					 uint32_t offs, const void * p, int len);
	int (* mem_read)(jtag_tap_t * tap, uint32_t base, 
				   uint32_t offs, void * p, int len);

	void (* code_exec)(jtag_tap_t * tap, uint32_t addr, 
					   uint32_t * regv, int regc);

	int (*insn_fetch)(jtag_tap_t * tap, uint32_t addr, 
					   int thumb, uint32_t * insn);
};

typedef struct armice_ctrl armice_ctrl_t;

extern const struct ice_drv_info armice_drv;

extern const struct module_def armice_module;

#ifdef __cplusplus
extern "C" {
#endif

int arm_wp_brk_req(jtag_tap_t * tap);

int arm_wp_clr(jtag_tap_t * tap, int id);

int arm_wp_set(jtag_tap_t * tap, int id, jtag_arm_wp_t * wp);

int arm_sstp_init(jtag_tap_t * tap);

int arm_sstp_disable(jtag_tap_t * tap);

int arm_sstp_enable(jtag_tap_t * tap);

/* set breakpoint and enable single step */
int arm_sstp_bp_set(jtag_tap_t * tap, uint32_t addr);

/*
 * ARM7
 */

int arm7_insn_step(jtag_tap_t * tap, armice_context_t * ct);

int arm7_thumb_insn_step(jtag_tap_t * tap, armice_context_t * ct);

void arm7_context_save(jtag_tap_t * tap, unsigned int flags, 
						   armice_context_t * ct);

void arm7_thumb_context_save(jtag_tap_t * tap, unsigned int flags, 
							 armice_context_t * ct);

int arm7_insn_fetch(jtag_tap_t * tap, uint32_t addr, 
					   int thumb, uint32_t * insn);

void arm7_thumb_context_restore(jtag_tap_t * tap, armice_context_t * ct);

void arm7_context_restore(jtag_tap_t * tap, armice_context_t * ct);

int  arm7_exec(jtag_tap_t * tap, int thumb);

int arm7_wr8(jtag_tap_t * tap, uint32_t addr, uint8_t data);

int arm7_rd8(jtag_tap_t * tap, uint32_t addr, uint8_t * data);

int arm7_wr16(jtag_tap_t * tap, uint32_t addr, uint16_t data);

int arm7_rd16(jtag_tap_t * tap, uint32_t addr, uint16_t * data);

int arm7_wr32(jtag_tap_t * tap, uint32_t addr, uint32_t data);

int arm7_rd32(jtag_tap_t * tap, uint32_t addr, uint32_t * data);

int arm7_mem_read(jtag_tap_t * tap, uint32_t base, 
				  uint32_t offs, void * p, int len);

int arm7_mem_write(jtag_tap_t * tap, uint32_t base, 
				   uint32_t offs, const void * p, int len);

int arm7_mem_set(jtag_tap_t * tap, uint32_t base, 
				 uint32_t offs, int c, int len);

int arm7_cfi16_erase(uint32_t base, uint32_t offs, int len);

int arm7_cfi16_write(uint32_t base, uint32_t offs, const void * ptr, int len);

void arm7_code_exec(jtag_tap_t * tap, uint32_t addr, 
					uint32_t * regv, int regc);

void arm7_safe_state(jtag_tap_t * tap);

/*
 * ARM9
 */

int arm9_insn_step(jtag_tap_t * tap, armice_context_t * ct);

int arm9_thumb_insn_step(jtag_tap_t * tap, armice_context_t * ct);

void arm9_context_save(jtag_tap_t * tap, unsigned int flags, 
						   armice_context_t * ct);

void arm9_thumb_context_save(jtag_tap_t * tap, unsigned int flags, 
							 armice_context_t * ct);

int arm9_insn_fetch(jtag_tap_t * tap, uint32_t addr, 
					   int thumb, uint32_t * insn);

void arm9_thumb_context_restore(jtag_tap_t * tap, armice_context_t * ct);

void arm9_context_restore(jtag_tap_t * tap, armice_context_t * ct);

int arm9_exec(jtag_tap_t * tap, int thumb);

int arm9_wr8(jtag_tap_t * tap, uint32_t addr, uint8_t data);

int arm9_rd8(jtag_tap_t * tap, uint32_t addr, uint8_t * data);

int arm9_wr16(jtag_tap_t * tap, uint32_t addr, uint16_t data);

int arm9_rd16(jtag_tap_t * tap, uint32_t addr, uint16_t * data);

int arm9_wr32(jtag_tap_t * tap, uint32_t addr, uint32_t data);

int arm9_rd32(jtag_tap_t * tap, uint32_t addr, uint32_t * data);

int arm9_mem_read(jtag_tap_t * tap, uint32_t base, 
				  uint32_t offs, void * p, int len);

int arm9_mem_write(jtag_tap_t * tap, uint32_t base, 
				   uint32_t offs, const void * p, int len);

int arm9_mem_set(jtag_tap_t * tap, uint32_t base, 
				 uint32_t offs, int c, int len);

void arm9_code_exec(jtag_tap_t * tap, uint32_t addr, 
					uint32_t * regv, int regc);


/*
 * ARM926EJ-S
 */

void arm926_context_save(jtag_tap_t * tap, unsigned int flags, 
						 armice_context_t * ct);

void arm926_context_restore(jtag_tap_t * tap, armice_context_t * ct);

void arm926_thumb_context_save(jtag_tap_t * tap, unsigned int flags, 
						 armice_context_t * ct);

void arm926_thumb_context_restore(jtag_tap_t * tap, armice_context_t * ct);
/*
 * Common
 */

int arm_cfi16_rase(armice_ctrl_t * ctrl, const ice_mem_ref_t * addr, 
					  ice_size_t len);

int arm_cfi16_write(armice_ctrl_t * ctrl, const ice_mem_ref_t * addr, 
					  const void * buf, ice_size_t len);

int arm_cfi16_2_erase(armice_ctrl_t * ctrl, const ice_mem_ref_t * addr, 
					  ice_size_t len);

int arm_cfi16_2_write(armice_ctrl_t * ctrl, const ice_mem_ref_t * addr, 
					  const void * buf, ice_size_t len);

int armice_mem_read(armice_ctrl_t * ctrl, ice_mem_ref_t * addr, 
					void * p, ice_size_t len);
 
int armice_mem_write(armice_ctrl_t * ctrl, ice_mem_ref_t * addr, 
					 const void * p, ice_size_t len);

int armice_mem_zero(armice_ctrl_t * ctrl, ice_mem_ref_t * addr,
					ice_size_t len);

int armice_code_load(armice_ctrl_t * ctrl, uint32_t addr, 
					 uint32_t * ptr, int size);

int armice_code_exec(armice_ctrl_t * ctrl, uint32_t addr, 
					 uint32_t * regv, int regc);

int armice_code_stop(armice_ctrl_t * ctrl);

int armice_code_resume(armice_ctrl_t * ctrl);

int armice_wr8(armice_ctrl_t * ctrl, uint32_t addr, uint8_t data);

int armice_rd8(armice_ctrl_t * ctrl, uint32_t addr, uint8_t * data);

int armice_wr16(armice_ctrl_t * ctrl, uint32_t addr, uint16_t data);

int armice_rd16(armice_ctrl_t * ctrl, uint32_t addr, uint16_t * data);

int armice_wr32(armice_ctrl_t * ctrl, uint32_t addr, uint32_t data);

int armice_rd32(armice_ctrl_t * ctrl, uint32_t addr, uint32_t * data);

int armice_thumb_mode(armice_ctrl_t * ctrl, int thumb);

int armice_reg_get(armice_ctrl_t * ctrl, int reg, uint32_t * val);

int armice_reg_set(armice_ctrl_t * ctrl, int reg, uint32_t val);



#ifdef __cplusplus
}
#endif

#endif /* __ASSEMBLER__ */

#endif /* __ARMICE_H__ */

