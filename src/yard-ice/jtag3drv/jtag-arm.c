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



#ifdef CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mclk.h>
#include <sys/clock.h>
#include <sys/delay.h>
#include <errno.h>

#include "jtag3drv.h"
#include "dbglog.h"

#include "armice.h"

#ifndef JTAG_VEC_BITS_MAX 
#error "JTAG_VEC_BITS_MAX undefined"
#endif

#define VEC_SCRATCH 0x0000
#define VEC_SCRATCH_LEN (JTAG_VEC_BITS_MAX + 7 / 8)
#define VEC_BYPASS (VEC_SCRATCH + VEC_SCRATCH_LEN)

/* Reserve the first 64 bytes (512 bits) 
   for scrath input */
#define VIN_SCRATCH 0x0000
#define VIN_SCRATCH_LEN (JTAG_VEC_BITS_MAX + 7 / 8)

/* return vector base */
#define RET_VECTOR_BASE 0x0000

/* Reserve the first 64 bytes (512 bits) 
   for scrath output */
#define VOUT_SCRATCH 0x0000

#define VOUT_SCRATCH_LEN (JTAG_VEC_BITS_MAX + 7 / 8)

#define VEC_ZEROS (VOUT_SCRATCH + VOUT_SCRATCH_LEN)
#define VEC_ZEROS_LEN 32


#define TAP_VEC_BASE VIN_SCRATCH_LEN
#define TAP_VEC_BITS_MAX 128
#define TAP_VEC_SIZE_MAX (TAP_VEC_BITS_MAX / 8)


#define IR_VECTORS_BASE (VEC_ZEROS + VEC_ZEROS_LEN)

#define VEC_IR_BYPASS   (IR_VECTORS_BASE + 0x00)
#define VEC_IR_INTEST   (IR_VECTORS_BASE + 0x04)
#define VEC_IR_SCAN_N   (IR_VECTORS_BASE + 0x08)
#define VEC_IR_RESTART  (IR_VECTORS_BASE + 0x0c)

#define DR_VECTORS_BASE (IR_VECTORS_BASE + 0x20)

#define VEC_DR_SCAN_N (DR_VECTORS_BASE + 0)
#define VEC_ICE_RT_WR (VEC_DR_SCAN_N + 2)
#define VEC_ICE_RT_RD (VEC_ICE_RT_WR + 6)
#define VEC_DBG_STATUS (VEC_ICE_RT_RD + 2)

#define DBG_CONTROL_RD (VEC_DBG_STATUS + 2)
#define DBG_CONTROL_WR (DBG_CONTROL_RD + 2)

#define VEC_DCC_CONTROL_REQ (DBG_CONTROL_WR + 6)
#define VEC_DCC_DATA_REQ (VEC_DCC_CONTROL_REQ + 2)
#define VEC_DCC_DATA_PUSH (VEC_DCC_DATA_REQ + 2) 

/*
   Descriptor tables
 */

#define DESC_IR_BYPASS (0)
#define DESC_IR_INTEST (DESC_IR_BYPASS + JTAG_TAP_MAX)
#define DESC_IR_SCAN_N (DESC_IR_INTEST + JTAG_TAP_MAX)
#define DESC_IR_RESTART (DESC_IR_SCAN_N + JTAG_TAP_MAX)

#define DESC_DR_SCAN_N (DESC_IR_RESTART + JTAG_TAP_MAX)
#define DESC_ICE_RT_WR (DESC_DR_SCAN_N + JTAG_TAP_MAX)
#define DESC_ICE_RT_RD_REQ (DESC_ICE_RT_WR + JTAG_TAP_MAX)
#define DESC_ICE_RT_RD_DATA (DESC_ICE_RT_RD_REQ + JTAG_TAP_MAX)

#define DESC_DBG_STATUS_REQ (DESC_ICE_RT_RD_DATA + JTAG_TAP_MAX)
#define DESC_DBG_STATUS_RET (DESC_DBG_STATUS_REQ + JTAG_TAP_MAX)
#define DESC_DCC_CONTROL_REQ (DESC_DBG_STATUS_RET + JTAG_TAP_MAX)
#define DESC_DCC_DATA_REQ (DESC_DCC_CONTROL_REQ + JTAG_TAP_MAX)
#define DESC_DCC_DATA_PUSH (DESC_DCC_DATA_REQ + JTAG_TAP_MAX)

#define DESC_ARM7_INSN (DESC_DCC_DATA_PUSH + JTAG_TAP_MAX)
#define DESC_IDCODE (DESC_ARM7_INSN + JTAG_TAP_MAX)

#define DESC_ARM9_INSN (DESC_IDCODE + JTAG_TAP_MAX)

#define DESC_ARM9_INSN_DATA (DESC_ARM9_INSN + JTAG_TAP_MAX)

#define DESC_CP15_REQ (DESC_ARM9_INSN_DATA + JTAG_TAP_MAX)
#define DESC_CP15_RD (DESC_CP15_REQ + JTAG_TAP_MAX)
#define DESC_CP15_WR (DESC_CP15_RD + JTAG_TAP_MAX)


/*
   Pointers to Descriptor tables
 */
#define PTR_SCARTCH 0
#define PTR_IR_BYPASS 1
#define PTR_IR_INTEST 2
#define PTR_IR_SCAN_N 3
#define PTR_IR_RESTART 4
#define PTR_DR_SCAN_N 5
#define PTR_ICE_RT_WR 6
#define PTR_ICE_RT_RD_REQ 7
#define PTR_ICE_RT_RD_DATA 8
#define PTR_DBG_STATUS_REQ 9
#define PTR_DBG_STATUS_RET 10
#define PTR_DCC_CONTROL_REQ 11
#define PTR_DCC_DATA_REQ 12
#define PTR_DCC_DATA_PUSH 13
#define PTR_ARM7_INSN 14
#define PTR_IDCODE 15
#define PTR_ARM9_INSN 16
#define PTR_ARM9_INSN_DATA 17
#define PTR_CP15_REQ 18
#define PTR_CP15_RD 19
#define PTR_CP15_WR 20

/***************************************************************************
 *
 * Common ARM7 & ARM9 JTAG operations
 *
 ***************************************************************************/

void jtag_arm_cache_clr(void)
{	
	jtag3drv.arm_scan_chain = -1;
}


int jtag_arm_dr_select(jtag_tap_t * tap, int scan_n)
{
	if (jtag3drv.arm_scan_chain == scan_n) {
		DCC_LOG1(LOG_MSG, "keep chain: %d (from cache)", scan_n);
		return JTAG_OK;
	}

	DCC_LOG1(LOG_MSG, "new chain: %d", scan_n);

	jtag3drv.arm_scan_chain = -1;

	insn_irscan(PTR_IR_SCAN_N, JTAG_TAP_IRUPDATE);
	if ((vec_rd16(0) & tap->irmask) != 0x0001) {
		DCC_LOG(LOG_WARNING, "IR_SCAN_N fail!");
		return JTAG_ERR_IR_SCAN;
	}

 	vec_wr16(VEC_DR_SCAN_N, scan_n);
	insn_drscan(PTR_DR_SCAN_N, JTAG_TAP_DRUPDATE);
	if ((vec_rd16(0) & jtag3drv.arm_scan_n_mask) != jtag3drv.arm_scan_n_cmp) {
		DCC_LOG1(LOG_WARNING, "dr_scan(SCAN_N) fail:0x%x!", vec_rd16(0));
		return JTAG_ERR_ARM_DR_SCAN_N;
	}

	/*
	The INTEST instruction places the selected scan chain in test mode:
		- The INTEST instruction connects the selected scan chain 
			between TDI and TDO.
		- When the INTEST instruction is loaded into the instruction register, 
			all the scan cells are placed in their test mode of operation.
		- In the CAPTURE-DR state, the value of the data applied from the 
			core logic to the output scan cells and the value of the data 
			applied from the system logic to the input scan cells is captured.
		- In the SHIFT-DR state, the previously-captured test data is 
			shifted out of the scan chain through the TDO pin, while new 
			test data is shifted in through the TDI pin.
	Single-step operation of the core is possible using the INTEST instruction.
	*/
	insn_irscan(PTR_IR_INTEST, JTAG_TAP_IRUPDATE);

	if ((vec_rd16(0) & tap->irmask) != 0x0001) {
		DCC_LOG(LOG_WARNING, "IR_INTEST fail!");
		return JTAG_ERR_IR_SCAN;
	}
	
	jtag3drv.arm_scan_chain = scan_n;

	return JTAG_OK;
}


int jtag_arm_ice_rt_wr(jtag_tap_t * tap, int reg, uint32_t data)
{
	int ret;

	DCC_LOG2(LOG_MSG, "reg: %d <= 0x%08x", reg, data);

	if ((ret = jtag_arm_dr_select(tap, ARMICE_SCAN_CHAIN_2)) != JTAG_OK) {
		return  ret;
	}

	vec_wr32(VEC_ICE_RT_WR, data);
	vec_wr16(VEC_ICE_RT_WR + 4, reg | ARMICE_DBG_WRITE);
	/* XXX: ARM9 - requires going to RUN_TEST/IDLE ??? */
	insn_drscan(PTR_ICE_RT_WR, JTAG_TAP_RUN_TEST);

	return JTAG_OK;
}

int jtag_arm_ice_rt_rd(jtag_tap_t * tap, int reg, uint32_t * data)
{
	int ret;

	DCC_LOG1(LOG_MSG, "reg: %d", reg);

	if ((ret = jtag_arm_dr_select(tap, ARMICE_SCAN_CHAIN_2)) != JTAG_OK) {
		return  ret;
	}

	/* push the request */
	vec_wr16(VEC_ICE_RT_RD, reg);
	insn_drscan(PTR_ICE_RT_RD_REQ, JTAG_TAP_DRUPDATE);

	/* pull the result */
	insn_drscan(PTR_ICE_RT_RD_DATA, JTAG_TAP_DRUPDATE);
	/* read the result */
	*data = vec_rd32(0);

	return JTAG_OK;
}

int jtag_arm_dbg_ctrl_wr(jtag_tap_t * tap, uint32_t ctrl,
						 unsigned int final_state)
{
	int ret;

	DCC_LOG1(LOG_INFO, "0x%02x", ctrl);

	if ((ret = jtag_arm_dr_select(tap, ARMICE_SCAN_CHAIN_2)) != JTAG_OK)
		return ret;

	vec_wr32(VEC_ICE_RT_WR, ctrl);
	vec_wr16(VEC_ICE_RT_WR + 4, ARMICE_DBG_CONTROL + ARMICE_DBG_WRITE);

	insn_drscan(PTR_ICE_RT_WR, final_state);

	return JTAG_OK;
}

int jtag_arm_dbg_ctrl_rd(jtag_tap_t * tap, uint32_t * ctrl)
{
	int ret;

	DCC_LOG1(LOG_INFO, "ctrl=%02x", ctrl);

	if ((ret = jtag_arm_dr_select(tap, ARMICE_SCAN_CHAIN_2)) != JTAG_OK) {
		return  ret;
	}

	/* push the request */
	vec_wr16(VEC_ICE_RT_RD, ARMICE_DBG_CONTROL);
	insn_drscan(PTR_ICE_RT_RD_REQ, JTAG_TAP_DRUPDATE);

	/* pull the result */
	insn_drscan(PTR_ICE_RT_RD_REQ, JTAG_TAP_DRUPDATE);
	*ctrl = vec_rd16(0);

	DCC_LOG6(LOG_TRACE, "ICE:%d MON:%d SSTP:%d INTDIS:%d DBGRQ:%d DBGACK:%d", 
		(*ctrl & ARMICE_CTRL_ICE_DISABLE) ? 1 : 0,
		(*ctrl & ARMICE_CTRL_MONITOR_EN) ? 1 : 0,
		(*ctrl & ARMICE_CTRL_SINGLE_STEP) ? 1 : 0,
		(*ctrl & ARMICE_CTRL_INTDIS) ? 1 : 0,
		(*ctrl & ARMICE_CTRL_DBGRQ) ? 1 : 0,
		(*ctrl & ARMICE_CTRL_DBGACK) ? 1 : 0);

	return  ret;
}


int jtag_arm_restart(jtag_tap_t * tap)
{
	DCC_LOG(LOG_INFO, ".");

	/* invalidate scan chain selection */
	jtag3drv.arm_scan_chain = -1;

	/*
	The RESTART instruction restarts the processor on exit from debug state.
	The RESTART instruction connects the bypass register between TDI and TDO.
	The TAP controller behaves as if the BYPASS instruction had been loaded.
	The processor exits debug state when the RUN-TEST-IDLE state is entered.
	*/

	insn_irscan(PTR_IR_RESTART, JTAG_TAP_RUN_TEST);

	if ((vec_rd16(0) & tap->irmask) != 0x0001) {
		DCC_LOG(LOG_WARNING, "IR_INTEST fail!");
		return JTAG_ERR_IR_SCAN;
	}


	return JTAG_OK;
}

/***************************************************************************
 *
 * ARM7 JTAG operations
 *
 ***************************************************************************/

int jtag_arm7_dbg_status(jtag_tap_t * tap)
{
	int status;
	int ret;

	if ((ret = jtag_arm_dr_select(tap, ARMICE_SCAN_CHAIN_2)) != JTAG_OK)
		return  ret;

	/* push the request */
	insn_drscan(PTR_DBG_STATUS_REQ, JTAG_TAP_DRUPDATE);
	/* pull the result */
	insn_drscan(PTR_DBG_STATUS_RET, JTAG_TAP_DRUPDATE);
	status = vec_rd16(0);

	DCC_LOG5(LOG_MSG, " THUMB:%d nMREQ:%d IFEN:%d DBGRQ:%d DBGACK:%d", 
		(status & ARMICE_ST_TBIT) ? 1 : 0,
		(status & ARMICE_ST_NMREQ) ? 1 : 0,
		(status & ARMICE_ST_IFEN) ? 1 : 0,
		(status & ARMICE_ST_DBGRQ) ? 1 : 0,
		(status & ARMICE_ST_DBGACK) ? 1 : 0);

	return status;
}

int jtag_arm7_dbgack_wait(jtag_tap_t * tap, unsigned int tmo)
{
	int status = 0;
	int retry;
	int ret;

	DCC_LOG(LOG_MSG, ".");

	if ((ret = jtag_arm_dr_select(tap, ARMICE_SCAN_CHAIN_2)) != JTAG_OK)
		return  ret;

	DCC_LOG2(LOG_MSG, "tmo = %d, jtag3drv.tck_freq = %d", 
			 tmo, jtag3drv.tck_freq );
	tmo *= ((jtag3drv.tck_freq >> 12) + 1);
	DCC_LOG1(LOG_MSG, "%d tries...", tmo);
	for (retry = tmo; (retry); retry--) {

		/* push the request */
		insn_drscan(PTR_DBG_STATUS_REQ, JTAG_TAP_DRUPDATE);
		/* pull the result */
		insn_drscan(PTR_DBG_STATUS_RET, JTAG_TAP_DRUPDATE);
		status = vec_rd16(0);

		DCC_LOG4(LOG_MSG, "DBGACK:%d THUMB:%d nMREQ:%d DBGRQ:%d", 
				 (status & ARMICE_ST_DBGACK) ? 1 : 0,
				 (status & ARMICE_ST_TBIT) ? 1 : 0,
				 (status & ARMICE_ST_NMREQ) ? 1 : 0,
				 (status & ARMICE_ST_DBGRQ) ? 1 : 0);

		if (((status & (ARMICE_ST_DBGACK | ARMICE_ST_NMREQ)) ^ 
			 (ARMICE_ST_DBGACK | ARMICE_ST_NMREQ)) == 0) {
			return status;
		}
	}

	DCC_LOG(LOG_WARNING, "too many retries!"); 

	return JTAG_ERR_ARM_SYNC_TIMEOUT;
}

int jtag_arm7_step(jtag_tap_t * tap, int dclk, uint32_t din, uint32_t * dout)
{
	int ret;

	if ((ret = jtag_arm_dr_select(tap, ARMICE_SCAN_CHAIN_1)) != JTAG_OK)
		return  ret;

	/* write vector */
	vec_wr32(0x000, din);
	vec_wr16(0x004, 0);

	if (dclk == 1)
		insn_drscan(PTR_ARM7_INSN, JTAG_TAP_RUN_TEST);
	else {
		insn_drscan(PTR_ARM7_INSN, JTAG_TAP_DRUPDATE);
		if (dclk != 0)
			insn_run_test(dclk, JTAG_TAP_RUN_TEST);
	}

	DCC_LOG3(LOG_MSG, "dclk=%d din=0x%08x dout=0x%08x", dclk, 
			 din, vec_rd32(0x00));

	if (dout != NULL)
		*dout = vec_rd32(0x00);

	return JTAG_OK;
}

int jtag_arm7_step_brk(jtag_tap_t * tap, int dclk, uint32_t din)
{
	int ret;

	if ((ret = jtag_arm_dr_select(tap, ARMICE_SCAN_CHAIN_1)) != JTAG_OK)
		return  ret;

	/* write vector */
	vec_wr32(0x000, din);
	vec_wr16(0x004, 1);

	DCC_LOG2(LOG_MSG, "dclk=%d din=0x%08x", dclk, din);

	if (dclk == 1)
		insn_drscan(PTR_ARM7_INSN, JTAG_TAP_RUN_TEST);
	else {
		insn_drscan(PTR_ARM7_INSN, JTAG_TAP_DRUPDATE);
		if (dclk != 0)
			insn_run_test(dclk, JTAG_TAP_RUN_TEST);
	}

	return JTAG_OK;
}

#if 0
int jtag_arm7_step_sync(jtag_tap_t * tap, int dclk, 
						uint32_t din)
{
	int ret;

	DCC_LOG2(LOG_INFO, "dclk=%d din=0x%08x", dclk, din);

	if ((ret = jtag_arm_dr_select(tap, ARMICE_SCAN_CHAIN_1)) != JTAG_OK) {
		return  ret;
	}

	/* write vector */
	vec_wr32(0x000, din);
	vec_wr16(0x004, 0);

	/* push the request */
	if (dclk == 1)
		insn_drscan(PTR_ARM7_INSN, JTAG_TAP_RUN_TEST);
	else {
		insn_drscan(PTR_ARM7_INSN, JTAG_TAP_DRUPDATE);
		if (dclk != 0)
			insn_run_test(dclk, JTAG_TAP_RUN_TEST);
	}

	jtag3drv.arm_scan_chain = -1;
	insn_irscan(PTR_IR_RESTART, JTAG_TAP_RUN_TEST);

	return jtag_arm7_dbgack_wait(tap, 100);
}
#endif

int jtag_arm7_step_sys(jtag_tap_t * tap, uint32_t insn1, uint32_t insn2)
{
	int ret;

	if ((ret = jtag_arm_dr_select(tap, ARMICE_SCAN_CHAIN_1)) != JTAG_OK)
		return  ret;

	DCC_LOG2(LOG_MSG, "insn1=0x%08x insn1=0x%08x", insn1, insn2);

	vec_wr32(0x000, insn1);
	vec_wr16(0x004, 1);
	insn_drscan(PTR_ARM7_INSN, JTAG_TAP_RUN_TEST);

	vec_wr32(0x000, insn2);
	vec_wr16(0x004, 0);
	insn_drscan(PTR_ARM7_INSN, JTAG_TAP_RUN_TEST);

	jtag3drv.arm_scan_chain = -1;
	insn_irscan(PTR_IR_RESTART, JTAG_TAP_RUN_TEST);

	return jtag_arm7_dbgack_wait(tap, 100);
}

int jtag_arm7_sys_sync(jtag_tap_t * tap)
{
	DCC_LOG(LOG_MSG, ".");

	jtag3drv.arm_scan_chain = -1;
	insn_irscan(PTR_IR_RESTART, JTAG_TAP_RUN_TEST);

	return jtag_arm7_dbgack_wait(tap, 100);
}


/***************************************************************************
 *
 * ARM9 JTAG operations
 *
 ***************************************************************************/
static const char entry_mode[][12] = {
	"NODBG",
	"BP0",
	"BP1",
	"BKPT",
	"VEC",
	"EXTBP",
	"WP0",
	"WP1",
	"EXTWP",
	"INTDBGRQ",
	"EXTDBGRQ",
	"DBGREENTRY"
};

int jtag_arm9_dbg_status(jtag_tap_t * tap)
{
	int status;
	int ret;

	if ((ret = jtag_arm_dr_select(tap, ARMICE_SCAN_CHAIN_2)) != JTAG_OK)
		return  ret;

	/* push the request */
	insn_drscan(PTR_DBG_STATUS_REQ, JTAG_TAP_DRUPDATE);
	/* pull the result */
	insn_drscan(PTR_DBG_STATUS_RET, JTAG_TAP_DRUPDATE);
	status = vec_rd16(0);

	DCC_LOG7(LOG_INFO, "%s IJBIT:%d ITBIT:%d SYSCOMP:%d "
			 "IFEN:%d DBGRQ:%d DBGACK:%d", 
			 entry_mode[ARMICE_ST_MOE(status)],
			 (status & ARMICE_ST_IJBIT) ? 1 : 0,
			 (status & ARMICE_ST_ITBIT) ? 1 : 0,
			 (status & ARMICE_ST_SYSCOMP) ? 1 : 0,
			 (status & ARMICE_ST_IFEN) ? 1 : 0,
			 (status & ARMICE_ST_DBGRQ) ? 1 : 0,
			 (status & ARMICE_ST_DBGACK) ? 1 : 0);

	return status;
}

int jtag_arm9_dbgack_wait(jtag_tap_t * tap, unsigned int tmo)
{
	int status = 0;
	int retry;
	int ret;

	DCC_LOG(LOG_MSG, ".");

	if ((ret = jtag_arm_dr_select(tap, ARMICE_SCAN_CHAIN_2)) != JTAG_OK)
		return  ret;

	DCC_LOG2(LOG_MSG, "tmo = %d, jtag3drv.tck_freq = %d", 
			 tmo, jtag3drv.tck_freq );
	tmo *= ((jtag3drv.tck_freq >> 12) + 1);
	DCC_LOG1(LOG_MSG, "%d tries...", tmo);
	for (retry = tmo; (retry); retry--) {

		/* push the request */
		insn_drscan(PTR_DBG_STATUS_REQ, JTAG_TAP_DRUPDATE);
		/* pull the result */
		insn_drscan(PTR_DBG_STATUS_RET, JTAG_TAP_DRUPDATE);
		status = vec_rd16(0);

		DCC_LOG7(LOG_MSG, "%s IJBIT:%d ITBIT:%d SYSCOMP:%d "
				 "IFEN:%d DBGRQ:%d DBGACK:%d", 
				 entry_mode[ARMICE_ST_MOE(status)],
				 (status & ARMICE_ST_IJBIT) ? 1 : 0,
				 (status & ARMICE_ST_ITBIT) ? 1 : 0,
				 (status & ARMICE_ST_SYSCOMP) ? 1 : 0,
				 (status & ARMICE_ST_IFEN) ? 1 : 0,
				 (status & ARMICE_ST_DBGRQ) ? 1 : 0,
				 (status & ARMICE_ST_DBGACK) ? 1 : 0);

		if (((status & (ARMICE_ST_DBGACK | ARMICE_ST_NMREQ)) ^ 
			 (ARMICE_ST_DBGACK | ARMICE_ST_NMREQ)) == 0) {
			return status;
		}
	}

	DCC_LOG(LOG_WARNING, "too many retries!"); 

	return JTAG_ERR_ARM_SYNC_TIMEOUT;
}

/* The Scan Chain 1 on ARM9 has 67 bits:
   [0..31] : DATA[0..31] (LSB first)
   [32] : Unused
   [33] : WPTANDBKPT
   [34] : SYSSPEED
   [35..66] : INSTR[31..0] (MSB first) 
 */

/* Insert an instruction into pipeline */
int jtag_arm9_step(jtag_tap_t * tap, int dclk, uint32_t insn)
{
	int ret;

	if ((ret = jtag_arm_dr_select(tap, ARMICE_SCAN_CHAIN_1)) != JTAG_OK)
		return  ret;

	/* write vector */
	vec_wr32(0x000, insn);
	vec_wr16(0x004, 0);

	if (dclk == 1)
		insn_drscan(PTR_ARM9_INSN, JTAG_TAP_RUN_TEST);
	else {
		insn_drscan(PTR_ARM9_INSN, JTAG_TAP_DRUPDATE);
		if (dclk != 0)
			insn_run_test(dclk, JTAG_TAP_RUN_TEST);
	}

	DCC_LOG2(LOG_MSG, "dclk=%d insn=0x%08x", dclk, insn);

	return JTAG_OK;
}

/* Insert an NOP into instruction bus and get the data bus content */
int jtag_arm9_step_rd(jtag_tap_t * tap, int dclk, uint32_t * dout)
{
	int ret;

	if ((ret = jtag_arm_dr_select(tap, ARMICE_SCAN_CHAIN_1)) != JTAG_OK)
		return  ret;

	/* ARM NOP */
	vec_wr32(0x000, 0xe1a00000);
	/* SYSSPEED = 0 */
	vec_wr16(0x004, 0);

	if (dclk == 1)
		insn_drscan(PTR_ARM9_INSN, JTAG_TAP_RUN_TEST);
	else {
		insn_drscan(PTR_ARM9_INSN, JTAG_TAP_DRUPDATE);
		if (dclk != 0)
			insn_run_test(dclk, JTAG_TAP_RUN_TEST);
	}

	DCC_LOG2(LOG_MSG, "dclk=%d dout=0x%08x", dclk, vec_rd32(0x00));

	if (dout != NULL)
		*dout = vec_rd32(0x00);

	return JTAG_OK;
}

int jtag_arm9_step_wr(jtag_tap_t * tap, int dclk, uint32_t din)
{
	int ret;

	if ((ret = jtag_arm_dr_select(tap, ARMICE_SCAN_CHAIN_1)) != JTAG_OK)
		return  ret;

	/* We shift a NOP into the instruction pipeline along
	   with the data */
    /*
	   ARM_NOP:
        0xe1a00000 : 11100001 10100000 00000000 00000000
         Inverting : 00000000 00000000 00000101 10000111
	   Shift 3 : 000 00000000 00000000 00101100 00111000 : 0x0 0x00002c38
	 */

	vec_wr32(0x000, din);
	vec_wr32(0x004, 0x00002c38); 
	vec_wr16(0x008, 0x0000);

	if (dclk == 1)
		insn_drscan(PTR_ARM9_INSN_DATA, JTAG_TAP_RUN_TEST);
	else {
		insn_drscan(PTR_ARM9_INSN_DATA, JTAG_TAP_DRUPDATE);
		if (dclk != 0)
			insn_run_test(dclk, JTAG_TAP_RUN_TEST);
	}

	DCC_LOG2(LOG_MSG, "dclk=%d din=0x%08x", dclk, din);

	return JTAG_OK;
}

int jtag_arm9_step_thumb_rd(jtag_tap_t * tap, int dclk, uint32_t * dout)
{
	int ret;

	if ((ret = jtag_arm_dr_select(tap, ARMICE_SCAN_CHAIN_1)) != JTAG_OK)
		return  ret;

	/* THUMB NOP */
	vec_wr32(0x000, 0x46c046c0);
	/* SYSSPEED = 0 */
	vec_wr16(0x004, 0);

	if (dclk == 1)
		insn_drscan(PTR_ARM9_INSN, JTAG_TAP_RUN_TEST);
	else {
		insn_drscan(PTR_ARM9_INSN, JTAG_TAP_DRUPDATE);
		if (dclk != 0)
			insn_run_test(dclk, JTAG_TAP_RUN_TEST);
	}

	DCC_LOG2(LOG_INFO, "dclk=%d dout=0x%08x", dclk, vec_rd32(0x00));

	if (dout != NULL)
		*dout = vec_rd32(0x00);

	return JTAG_OK;
}

int jtag_arm9_step_thumb_wr(jtag_tap_t * tap, int dclk, uint32_t din)
{
	int ret;

	if ((ret = jtag_arm_dr_select(tap, ARMICE_SCAN_CHAIN_1)) != JTAG_OK)
		return  ret;

	/* We shift a NOP into the instruction pipeline along
	   with the data */

    /*
	  THUMB_NOP:
       0x46c046c0 : 01000110 11000000 01000110 11000000
        Inverting : 00000011 01100010 00000011 01100010
	  Shift 3 : 000 00011011 00010000 00011011 00010000 : 0x0 0x1b101b10 
	 */

	vec_wr32(0x000, din);
	vec_wr32(0x004, 0x1b101b10); 
	vec_wr16(0x008, 0x0000);

	if (dclk == 1)
		insn_drscan(PTR_ARM9_INSN_DATA, JTAG_TAP_RUN_TEST);
	else {
		insn_drscan(PTR_ARM9_INSN_DATA, JTAG_TAP_DRUPDATE);
		if (dclk != 0)
			insn_run_test(dclk, JTAG_TAP_RUN_TEST);
	}

	DCC_LOG2(LOG_INFO, "dclk=%d din=0x%08x", dclk, din);

	return JTAG_OK;
}

int jtag_arm9_step_sys(jtag_tap_t * tap, uint32_t insn)
{
	int ret;

	if ((ret = jtag_arm_dr_select(tap, ARMICE_SCAN_CHAIN_1)) != JTAG_OK)
		return  ret;

	DCC_LOG1(LOG_MSG, "insn=0x%08x", insn);

	/* write vector */
	vec_wr32(0x000, insn);
	vec_wr16(0x004, 0);
	insn_drscan(PTR_ARM9_INSN, JTAG_TAP_RUN_TEST);

	/* ARM NOP */
	vec_wr32(0x000, 0xe1a00000);
	vec_wr16(0x004, 1);
	insn_drscan(PTR_ARM9_INSN, JTAG_TAP_RUN_TEST);

	jtag3drv.arm_scan_chain = -1;
	insn_irscan(PTR_IR_RESTART, JTAG_TAP_RUN_TEST);

	return jtag_arm9_dbgack_wait(tap, 100);
}

int jtag_arm9_step_sync(jtag_tap_t * tap, uint32_t insn)
{
	int ret;

	if ((ret = jtag_arm_dr_select(tap, ARMICE_SCAN_CHAIN_1)) != JTAG_OK)
		return  ret;

	DCC_LOG1(LOG_MSG, "insn=0x%08x", insn);

	/* write vector */
	vec_wr32(0x000, insn);
	vec_wr16(0x004, 1);
	insn_drscan(PTR_ARM9_INSN, JTAG_TAP_RUN_TEST);

	return JTAG_OK;
}

int jtag_arm9_sys_sync(jtag_tap_t * tap)
{
	DCC_LOG(LOG_MSG, ".");

	jtag3drv.arm_scan_chain = -1;
	insn_irscan(PTR_IR_RESTART, JTAG_TAP_RUN_TEST);

	return jtag_arm9_dbgack_wait(tap, 100);
}

/***************************************************************************
 *
 * ARM926EJ-S JTAG operations
 *
 ***************************************************************************/

int jtag_arm926_cp15_rd(jtag_tap_t * tap, int reg, uint32_t * data)
{
	int tmo = 100;
	int retry;
	int ret;

	DCC_LOG4(LOG_INFO, "Opc_1=%d CRn=%d CRm=%d Opc_2=%d", 
			 (((reg) >> 12) & 0x07), (((reg) >>5) & 0x0f), 
			 (((reg) >> 1) & 0x0f), (((reg) >> 9) & 0x07));

	if ((ret = jtag_arm_dr_select(tap, ARMICE_SCAN_CHAIN_15)) != JTAG_OK)
		return  ret;

#if 1
	vec_wr16(0x000, (ARMICE_CP15_REG_MASK & reg) | ARMICE_CP15_ACCESS);
	/* push the request */
	insn_drscan(PTR_CP15_REQ, JTAG_TAP_DRUPDATE);
#else
	vec_wr32(0x000, 0);
	vec_wr16(0x004, (ARMICE_CP15_REG_MASK & reg) | ARMICE_CP15_ACCESS );
	/* push the request */
	insn_drscan(PTR_CP15_WR, JTAG_TAP_DRUPDATE);
#endif

	tmo *= ((jtag3drv.tck_freq >> 12) + 1);
	DCC_LOG1(LOG_MSG, "trying %d times...", tmo);
	for (retry = tmo; (retry); retry--) {
		/* pull out the CP15 data value plus the access bit */
		insn_drscan(PTR_CP15_RD, JTAG_TAP_DRUPDATE);
		if (vec_rd16(0x004) & 0x0001) {
			*data = vec_rd32(0x000);
			DCC_LOG1(LOG_INFO, "%08x", *data);

			/* FIXME: check why the TAP has to be reset after 
			   the CP15 access!!! */
			insn_irscan(PTR_IR_BYPASS, JTAG_TAP_IRUPDATE);
			jtag3drv.arm_scan_chain = -1;

			return JTAG_OK;
		}
	}

	DCC_LOG(LOG_WARNING, "too many retries!"); 
	insn_tap_reset(5, JTAG_TAP_RESET);
	jtag3drv.arm_scan_chain = -1;

	return JTAG_ERR_ARM_SYNC_TIMEOUT;
}

int jtag_arm926_cp15_wr(jtag_tap_t * tap, int reg, uint32_t data)
{
	int retry;
	int ret;
	int tmo = 100;

	DCC_LOG5(LOG_INFO, "Opc_1=%d CRn=%d CRm=%d Opc_2=%d data=0x%08x", 
			 (((reg) >> 12) & 0x07), (((reg) >>5) & 0x0f), 
			 (((reg) >> 1) & 0x0f), (((reg) >> 9) & 0x07), data);

	if ((ret = jtag_arm_dr_select(tap, ARMICE_SCAN_CHAIN_15)) != JTAG_OK)
		return  ret;

	vec_wr32(0x000, data);
	vec_wr16(0x004, (ARMICE_CP15_REG_MASK & reg) | 
			 ARMICE_CP15_ACCESS | ARMICE_CP15_WR);
	/* push the request */
	insn_drscan(PTR_CP15_WR, JTAG_TAP_DRUPDATE);

	tmo *= ((jtag3drv.tck_freq >> 12) + 1);
	DCC_LOG1(LOG_MSG, "trying %d times...", tmo);
	for (retry = tmo; (retry); retry--) {
		/* pull out the CP15 data value plus the access bit */
		insn_drscan(PTR_CP15_RD, JTAG_TAP_DRUPDATE);

		if (vec_rd16(0x004) & 0x0001) {
			DCC_LOG1(LOG_MSG, "%d tries...", tmo - retry);

			/* FIXME: check why the TAP has to be reset after 
			   the CP15 access!!! */
//			insn_run_test(1, JTAG_TAP_RESET);
			insn_irscan(PTR_IR_BYPASS, JTAG_TAP_IRUPDATE);
			jtag3drv.arm_scan_chain = -1;

			return JTAG_OK;
		}
	}

	DCC_LOG(LOG_WARNING, "too many retries!"); 

	insn_tap_reset(5, JTAG_TAP_RESET);
	jtag3drv.arm_scan_chain = -1;

	return JTAG_ERR_ARM_SYNC_TIMEOUT;
}

/***************************************************************************
 *
 * ARM debug status and DCC polling
 *
 ***************************************************************************/

int jtag_arm_poll(jtag_tap_t * tap, jtag_arm_poll_t * poll, 
				  ice_comm_t * comm)
{
	int dcc_status;
	int dbg_status = 0;
	uint32_t data;
	uint32_t tx_tail;
	uint32_t tx_head;

	if (jtag_arm_dr_select(tap, ARMICE_SCAN_CHAIN_2) != JTAG_OK) {
		DCC_LOG(LOG_WARNING, "jtag_arm_dr_select() fail!");
		return  -1;
	}

	/* request the debug status register */
	insn_drscan(PTR_DBG_STATUS_REQ, JTAG_TAP_DRUPDATE);

	tx_tail = comm->tx.tail;
	
	for (;;) {
		/* request the DCC control register */
		/* push the DCC control request and pull out the dbg status */
		insn_drscan(PTR_DCC_CONTROL_REQ, JTAG_TAP_DRUPDATE);
		/* get the result */
		dbg_status = vec_rd16(0);

		/* update statistics */
		poll->count++;
		poll->status = dbg_status;

		if (dbg_status & ARMICE_ST_DBGACK) {
			DCC_LOG(LOG_TRACE, "BREAK!!!");
			return dbg_status;
		}

		if (!poll->enabled)
			return dbg_status;

#if 0
		/* scan the IR register and read the result (must be 0x0001)
		   just to make sure the target is connected */
		insn_irscan(PTR_IR_INTEST, JTAG_TAP_IRUPDATE);
		if ((vec_rd16(0) & tap->irmask) != 0x0001) {
			DCC_LOG(LOG_WARNING, "IR_SCAN fail!");
			return -1;
		}
#endif

/* get debug status */
		/* request the debug status register */
		/* push the debug status request and pull out the DCC control */
		insn_drscan(PTR_DBG_STATUS_REQ, JTAG_TAP_DRUPDATE);
		/* read the resulting vector */
		dcc_status = vec_rd16(0);

		while ((dcc_status & ARMICE_DCC_WRITE_FLAG)) {
			uint32_t head;
			int rem;

			/* get the ICE-COMM buffer info */
			head = comm->rx.head;
			rem = ICE_COMM_RX_BUF_LEN - (head - comm->rx.tail);

			if (rem == 0) {
				DCC_LOG(LOG_MSG, "DCC buffer full!");
				break;
			}

			/* push the DCC data request and pull out the dbg status */
			insn_drscan(PTR_DCC_DATA_REQ, JTAG_TAP_DRUPDATE);

			/* read the resulting vector */
			dbg_status = vec_rd16(0);
			/* update statistics */
			poll->count++;
			poll->status = dbg_status;

			/* pull out the DCC data, push the DCC data request  */
			insn_drscan(PTR_DCC_DATA_PUSH, JTAG_TAP_DRUPDATE);

			comm->rx.buf[head & (ICE_COMM_RX_BUF_LEN - 1)] = vec_rd32(0);
			head++;
			comm->rx.head = head;

			DCC_LOG1(LOG_MSG, "DCC: head=%d", head);

			if (rem == (ICE_COMM_RX_BUF_LEN / 2)) {
				DCC_LOG(LOG_MSG, "DCC notify!");
				uthread_sem_post(comm->rx_sem);
			}
dcc_poll:
			if (dbg_status & ARMICE_ST_DBGACK) {
				DCC_LOG(LOG_TRACE, "BREAK!!!");
				return dbg_status;
			}
			if (!poll->enabled)
				return dbg_status;

			/* push the dbg status request and pull out the DCC control */
			insn_drscan(PTR_DBG_STATUS_REQ, JTAG_TAP_DRUPDATE);
			/* read the resulting vector */
			dcc_status = vec_rd16(0);
		}

		if (((dcc_status & ARMICE_DCC_READ_FLAG) == 0) && 
			((tx_head = comm->tx.head) != tx_tail)) {
			
			data = comm->tx.buf[tx_tail & (ICE_COMM_TX_BUF_LEN - 1)];
			comm->tx.tail = ++tx_tail;

			DCC_LOG1(LOG_MSG, "DCC out: %08x", data);

			/* write the request vector into the vector memory */
			vec_wr32(VEC_ICE_RT_WR, data);
			vec_wr16(VEC_ICE_RT_WR + 4, ARMICE_DCC_DATA + ARMICE_DBG_WRITE);
			/* push the DCC data request and pull out the dbg status */
			insn_drscan(PTR_ICE_RT_WR, JTAG_TAP_DRUPDATE);

			/* read the resulting vector */
			dbg_status = vec_rd16(0);
			/* update statistics */
			poll->count++;
			poll->status = dbg_status;

			DCC_LOG(LOG_MSG, "DCC notify!");
			uthread_sem_post(comm->tx_sem);

			goto dcc_poll;
		}
	};
}

/* Put data directly into DCC channel  */
int jtag_arm_dcc_out(jtag_tap_t * tap, uint32_t data, unsigned int tmo)
{
	unsigned int retry;
	int status;
	int ret;

	DCC_LOG2(LOG_INFO, "%08x, tmo:%d", data, tmo);

	if ((ret = jtag_arm_dr_select(tap, ARMICE_SCAN_CHAIN_2)) != JTAG_OK) {
		DCC_LOG(LOG_WARNING, "select_arm_scan_chain() fail!");
		return  ret;
	}

	/* request the DCC control register */
	insn_drscan(PTR_DCC_CONTROL_REQ, JTAG_TAP_DRUPDATE);

	DCC_LOG2(LOG_INFO, "tmo = %d, jtag3drv.tck_freq = %d", 
			 tmo, jtag3drv.tck_freq );
	tmo *= ((jtag3drv.tck_freq >> 14) + 1);
	DCC_LOG1(LOG_INFO, "%d tries...", tmo);
	for (retry = tmo; (retry); retry--) {

		/* pull out the DCC control */
		insn_drscan(PTR_DCC_CONTROL_REQ, JTAG_TAP_DRUPDATE);
		/* read the resulting vector */
		status = vec_rd16(0);

		if ((status & ARMICE_DCC_READ_FLAG) == 0) {
			/* write the request vector into the vector memory */
			vec_wr32(VEC_ICE_RT_WR, data);
			vec_wr16(VEC_ICE_RT_WR + 4, ARMICE_DCC_DATA + ARMICE_DBG_WRITE);
			/* push the DCC data */
			insn_drscan(PTR_ICE_RT_WR, JTAG_TAP_DRUPDATE);

			DCC_LOG1(LOG_INFO, "%d tries.", tmo - retry);
			return JTAG_OK;
		}

		DCC_LOG(LOG_INFO, "again.");
	} 

	DCC_LOG1(LOG_WARNING, "timeout after %d tries.", tmo);
	return JTAG_ERR_ARM_DCC_TIMEOUT;
}

/* Get data directly from DCC channel  */
int jtag_arm_dcc_in(jtag_tap_t * tap, uint32_t * data, unsigned int tmo)
{
	unsigned int retry;
	int status;
	int ret;

	DCC_LOG1(LOG_MSG, "tmo:%d", tmo);

	if ((ret = jtag_arm_dr_select(tap, ARMICE_SCAN_CHAIN_2)) != JTAG_OK) {
		DCC_LOG(LOG_WARNING, "select_arm_scan_chain() fail!");
		return  ret;
	}

	/* request the DCC control register */
	insn_drscan(PTR_DCC_CONTROL_REQ, JTAG_TAP_DRUPDATE);

	DCC_LOG2(LOG_MSG, "tmo = %d, jtag3drv.tck_freq = %d", 
			 tmo, jtag3drv.tck_freq );
	tmo *= ((jtag3drv.tck_freq >> 14) + 1);
	DCC_LOG1(LOG_MSG, "max %d tries ..", tmo);
	for (retry = tmo; (retry); retry--) {
	
		/* pull out the DCC control */
		insn_drscan(PTR_DCC_CONTROL_REQ, JTAG_TAP_DRUPDATE);
		/* read the resulting vector */
		status = vec_rd16(0);

		if ((status & ARMICE_DCC_WRITE_FLAG)) {

			insn_drscan(PTR_DCC_DATA_REQ, JTAG_TAP_DRUPDATE);

			/* pull out the DCC data, push the DCC control request  */
			insn_drscan(PTR_DCC_DATA_PUSH, JTAG_TAP_DRUPDATE);
			
			/* read the resulting vector */
			*data = vec_rd32(0);

			DCC_LOG2(LOG_INFO, "%08x, after %d tries", *data,  tmo - retry);

			return JTAG_OK;
		}

		DCC_LOG(LOG_MSG, "again.");
	} 

	DCC_LOG(LOG_WARNING, "timeout.");
	return JTAG_ERR_ARM_DCC_TIMEOUT;
}


/***************************************************************************
 *
 * Initialization and configuration
 *
 ***************************************************************************/

const uint16_t ir_vectors[] = {
	ARMICE_IR_BYPASS, 0x0000,
	ARMICE_IR_INTEST, 0x0000,
	ARMICE_IR_SCAN_N, 0x0000,
	ARMICE_IR_RESTART, 0x0000
};

const uint16_t dr_vectors[] = {
	/* VEC_DR_SCAN_N */
	0x0000,

	/* VEC_ICE_RT_WR */
	0x0000, 0x0000, ARMICE_DBG_WRITE,

	/* VEC_ICE_RT_RD */
	0x0000, 

	/* VEC_DBG_STATUS */
	ARMICE_DBG_STATUS,

	/* VEC_DBG_CONTROL */
	ARMICE_DBG_CONTROL,

	/* DBG_CONTROL_WR */
	0x0000, 0x0000, ARMICE_DBG_CONTROL + ARMICE_DBG_WRITE,

	/* VEC_DCC_CONTROL_REQ */
	ARMICE_DCC_CONTROL,

	/* VEC_DCC_DATA_REQ */
	ARMICE_DCC_DATA,

	/* VEC_DCC_DATA_PUSH (request a DCC_CONTROL read) */
	0x0000, ARMICE_DCC_CONTROL << 10
};

int jtag_arm_tap_setup(jtag_tap_t * tap, int family)
{
	int irlen;
	int chain_pos;
	int scan_n_len = 0;
	int vaddr;
	int i;

	chain_pos = tap->pos;

	jtag3drv.arm_scan_n_cmp = 0;
	jtag3drv.arm_scan_n_mask = 0;
	jtag3drv.arm_scan_chain = -1;

	/* initialize scratch and bypass vectors */
	for (vaddr = 0; vaddr < (VEC_ZEROS + VEC_ZEROS_LEN); vaddr += 4) {
		vec_wr32(vaddr, 0);
	}

	/* initialize the IR vector poll */
	jtag3ctrl_vec_wr(IR_VECTORS_BASE, ir_vectors, sizeof(ir_vectors) * 8);
	DCC_LOG1(LOG_TRACE, "IR vectors table: %d bytes", sizeof(ir_vectors));

	/* initialize the DR vector poll */
	jtag3ctrl_vec_wr(DR_VECTORS_BASE, dr_vectors, sizeof(dr_vectors) * 8);

	DCC_LOG1(LOG_TRACE, "DR vectors table: %d bytes", sizeof(dr_vectors));
	DCC_LOG2(LOG_TRACE, "DR vectors 0x%04x..0x%04x ", DR_VECTORS_BASE,
			 DR_VECTORS_BASE + sizeof(dr_vectors));

	if (family == 7) {
		DCC_LOG(LOG_TRACE, "ARM7 family.");
		/* configure the TAP */
		scan_n_len = 4;
		jtag3drv.arm_scan_n_cmp = 0x08;
		jtag3drv.arm_scan_n_mask = 0x0f;
	} else {
		if (family == 9) {
			DCC_LOG(LOG_TRACE, "ARM9 family.");
			/* configure the TAP */
			scan_n_len = 5;
			jtag3drv.arm_scan_n_cmp = 0x010;
			jtag3drv.arm_scan_n_mask = 0x01f;
		} else {
			DCC_LOG(LOG_WARNING, "ARM family not unsupported.");
		}
	}

	DCC_LOG1(LOG_INFO, "chain_pos:%d", chain_pos);

	for (i = 0; jtag_tap_get(&tap, i) == JTAG_OK; i++) {

		irlen = tap->irlen;
		DCC_LOG2(LOG_INFO, "tap:%08x irlen:%d", tap, irlen);

		if (i == chain_pos) {
			DCC_LOG1(LOG_MSG, "scan_n_len: %d", scan_n_len);
			DCC_LOG1(LOG_MSG, "DR_SCAN_N desc: %d", DESC_DR_SCAN_N);
			DCC_LOG1(LOG_MSG, "DR_SCAN_N addr: %03x", VEC_DR_SCAN_N);

			desc_wr(DESC_IR_BYPASS + i, 
					JTAG_DESC(0, 0, VEC_IR_BYPASS, 0, irlen));

			desc_wr(DESC_IR_INTEST + i, 
					JTAG_DESC(0, 0, VEC_IR_INTEST, 0, irlen));

			desc_wr(DESC_IR_SCAN_N + i, 
					JTAG_DESC(0, 0, VEC_IR_SCAN_N, 0, irlen));

			desc_wr(DESC_IR_RESTART + i, 
					JTAG_DESC(0, 0, VEC_IR_RESTART, 0, irlen));

			desc_wr(DESC_DR_SCAN_N + i, 
					JTAG_DESC(0, 0, VEC_DR_SCAN_N, 0, scan_n_len));

			desc_wr(DESC_ICE_RT_WR + i, JTAG_DESC(0, 0, VEC_ICE_RT_WR, 0, 38));

			desc_wr(DESC_ICE_RT_RD_REQ + i, 
					JTAG_DESC(0, 0, VEC_ICE_RT_RD, 0, 6));

			desc_wr(DESC_ICE_RT_RD_DATA + i, 
					JTAG_DESC(0, 0, VEC_ZEROS, 0, 32));

			desc_wr(DESC_DBG_STATUS_REQ + i, 
					JTAG_DESC(0, 0, VEC_DBG_STATUS, 0, 6));

			if (family == 9) {
				desc_wr(DESC_DBG_STATUS_RET + i, 
						JTAG_DESC(0, 0, VEC_DBG_STATUS, 0, 10));
			} else {
				desc_wr(DESC_DBG_STATUS_RET + i, 
						JTAG_DESC(0, 0, VEC_DBG_STATUS, 0, 5));
			}

			desc_wr(DESC_DCC_CONTROL_REQ + i, 
					JTAG_DESC(0, 0, VEC_DCC_CONTROL_REQ, 0, 6));

			desc_wr(DESC_DCC_DATA_REQ + i, 
					JTAG_DESC(0, 0, VEC_DCC_DATA_REQ, 0, 6));

			desc_wr(DESC_DCC_DATA_PUSH + i, 
					JTAG_DESC(0, 0, VEC_DCC_DATA_PUSH, 0, 32));

			desc_wr(DESC_ARM7_INSN + i, 
					JTAG_DESC(0, 1, 0, 1, 33));

			desc_wr(DESC_IDCODE + i, 
					JTAG_DESC(0, 0, VEC_ZEROS, 0, 32));

			/* ARM 9 - send MSB first (instruction) and
			   receive LSB first (data) */
			desc_wr(DESC_ARM9_INSN + i, 
					JTAG_DESC(0, 0, 0, 1, 33));

			/* ARM 9 - Shift in and out out LSB first as we are only 
			   interested in the data */
			desc_wr(DESC_ARM9_INSN_DATA + i, 
					JTAG_DESC(0, 0, 0, 0, 67));

			/* ARM 9 - CP15 reqest */
			desc_wr(DESC_CP15_REQ + i, 
					JTAG_DESC(0, 0, 0, 0, 16));

			/* ARM 9 - CP15 data read */
			desc_wr(DESC_CP15_RD + i, 
					JTAG_DESC(0, 0, VEC_ZEROS, 0, 33));

			/* ARM 9 - CP15 data write */
			desc_wr(DESC_CP15_WR + i, 
					JTAG_DESC(0, 0, 0, 0, 48));
		} else {
			desc_wr(DESC_IR_BYPASS + i, 
					JTAG_DESC(VEC_BYPASS, 0, VEC_IR_BYPASS, 0, irlen));

			desc_wr(DESC_IR_INTEST + i, 
					JTAG_DESC(VEC_BYPASS, 0, VEC_IR_BYPASS, 0, irlen));

			desc_wr(DESC_IR_SCAN_N + i, 
					JTAG_DESC(VEC_BYPASS, 0, VEC_IR_BYPASS, 0, irlen));

			desc_wr(DESC_IR_RESTART + i, 
					JTAG_DESC(VEC_BYPASS, 0, VEC_IR_BYPASS, 0, irlen));

			desc_wr(DESC_DR_SCAN_N + i, 
					JTAG_DESC(VEC_BYPASS, 0, VEC_ZEROS, 0, 1));

			desc_wr(DESC_ICE_RT_WR + i, 
					JTAG_DESC(VEC_BYPASS, 0, VEC_ZEROS, 0, 1));

			desc_wr(DESC_ICE_RT_RD_REQ + i,
					JTAG_DESC(VEC_BYPASS, 0, VEC_ZEROS, 0, 1));

			desc_wr(DESC_ICE_RT_RD_DATA + i, 
					JTAG_DESC(VEC_BYPASS, 0, VEC_ZEROS, 0, 1));

			desc_wr(DESC_DBG_STATUS_REQ + i, 
					JTAG_DESC(VEC_BYPASS, 0, VEC_ZEROS, 0, 1));

			desc_wr(DESC_DBG_STATUS_RET + i, 
					JTAG_DESC(VEC_BYPASS, 0, VEC_ZEROS, 0, 1));

			desc_wr(DESC_DCC_CONTROL_REQ + i, 
					JTAG_DESC(VEC_BYPASS, 0, VEC_ZEROS, 0, 1));

			desc_wr(DESC_DCC_DATA_REQ + i, 
					JTAG_DESC(VEC_BYPASS, 0, VEC_ZEROS, 0, 1));

			desc_wr(DESC_DCC_DATA_PUSH + i, 
					JTAG_DESC(VEC_BYPASS, 0, VEC_ZEROS, 0, 1));

			desc_wr(DESC_ARM7_INSN + i, 
					JTAG_DESC(VEC_BYPASS, 0, VEC_ZEROS, 0, 1));

			desc_wr(DESC_IDCODE + i, 
					JTAG_DESC(VEC_BYPASS, 0, VEC_ZEROS, 0, 1));

			desc_wr(DESC_ARM9_INSN + i, 
					JTAG_DESC(VEC_BYPASS, 0, VEC_ZEROS, 0, 1));

			desc_wr(DESC_ARM9_INSN_DATA + i, 
					JTAG_DESC(VEC_BYPASS, 0, VEC_ZEROS, 0, 1));

			desc_wr(DESC_CP15_REQ + i, 
					JTAG_DESC(VEC_BYPASS, 0, VEC_ZEROS, 0, 1));

			desc_wr(DESC_CP15_RD + i, 
					JTAG_DESC(VEC_BYPASS, 0, VEC_ZEROS, 0, 1));

			desc_wr(DESC_CP15_WR + i, 
					JTAG_DESC(VEC_BYPASS, 0, VEC_ZEROS, 0, 1));
		}
	}


	/* create pointers to the descriptor tables */
	ptr_wr(PTR_IR_BYPASS, JTAG_PTR(DESC_IR_BYPASS, i));
	ptr_wr(PTR_IR_INTEST, JTAG_PTR(DESC_IR_INTEST, i));
	ptr_wr(PTR_IR_SCAN_N, JTAG_PTR(DESC_IR_SCAN_N, i));
	ptr_wr(PTR_IR_RESTART, JTAG_PTR(DESC_IR_RESTART, i));
	ptr_wr(PTR_DR_SCAN_N, JTAG_PTR(DESC_DR_SCAN_N, i));
	ptr_wr(PTR_ICE_RT_WR, JTAG_PTR(DESC_ICE_RT_WR, i));
	ptr_wr(PTR_ICE_RT_RD_REQ, JTAG_PTR(DESC_ICE_RT_RD_REQ, i));
	ptr_wr(PTR_ICE_RT_RD_DATA, JTAG_PTR(DESC_ICE_RT_RD_DATA, i));
	ptr_wr(PTR_DBG_STATUS_REQ, JTAG_PTR(DESC_DBG_STATUS_REQ, i));
	ptr_wr(PTR_DBG_STATUS_RET, JTAG_PTR(DESC_DBG_STATUS_RET, i));
	ptr_wr(PTR_DCC_CONTROL_REQ, JTAG_PTR(DESC_DCC_CONTROL_REQ, i));
	ptr_wr(PTR_DCC_DATA_REQ, JTAG_PTR(DESC_DCC_DATA_REQ, i));
	ptr_wr(PTR_DCC_DATA_PUSH, JTAG_PTR(DESC_DCC_DATA_PUSH, i));
	ptr_wr(PTR_ARM7_INSN, JTAG_PTR(DESC_ARM7_INSN, i));
	ptr_wr(PTR_IDCODE, JTAG_PTR(DESC_IDCODE, i));
	ptr_wr(PTR_ARM9_INSN, JTAG_PTR(DESC_ARM9_INSN, i));
	ptr_wr(PTR_ARM9_INSN_DATA, JTAG_PTR(DESC_ARM9_INSN_DATA, i));
	ptr_wr(PTR_CP15_REQ, JTAG_PTR(DESC_CP15_REQ, i));
	ptr_wr(PTR_CP15_RD, JTAG_PTR(DESC_CP15_RD, i));
	ptr_wr(PTR_CP15_WR, JTAG_PTR(DESC_CP15_WR, i));


	return JTAG_OK;
}


