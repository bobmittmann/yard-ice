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
 * @file .h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#ifndef __JTAG_ARM_H__
#define __JTAG_ARM_H__

#include "jtag.h"
#include "ice_comm.h"

/*
 * ARM ICE Watchpoint struct
 */
typedef struct {
	uint32_t addr_value;
	uint32_t addr_mask;
	uint32_t data_value;
	uint32_t data_mask;
	uint16_t ctrl_value;
	uint16_t ctrl_mask;
} jtag_arm_wp_t;

typedef struct {
	volatile int status;
	volatile int enabled;
	uint32_t count;
} jtag_arm_poll_t;

#ifdef __cplusplus
extern "C" {
#endif


/***************************************************************************
 *
 * Common ARM7 & ARM9 JTAG operations
 *
 ***************************************************************************/

/**
 *  Configure the TAP to support extended ARM operations
 */
int jtag_arm_tap_setup(jtag_tap_t * tap, int family);

void jtag_arm_cache_clr(void);

int jtag_arm_dr_scan_select(jtag_tap_t * tap, int vec_addr);

int jtag_arm_embedded_ice_rt_wr(jtag_tap_t * tap, int reg, uint32_t data);

int jtag_arm_embedded_ice_rt_rd(jtag_tap_t * tap, int reg, uint32_t * data);

int jtag_arm_dbg_ctrl_wr(jtag_tap_t * tap, uint32_t ctrl, 
						 unsigned int final_state);

int jtag_arm_dbg_ctrl_rd(jtag_tap_t * tap, uint32_t * ctrl);

int jtag_arm_ice_rt_wr(jtag_tap_t * tap, int reg, uint32_t data);

int jtag_arm_ice_rt_rd(jtag_tap_t * tap, int reg, uint32_t * data);

int jtag_arm_restart(jtag_tap_t * tap);

/***************************************************************************
 *
 * ARM7 JTAG operations
 *
 ***************************************************************************/

int jtag_arm7_dbg_status(jtag_tap_t * tap);

int jtag_arm7_dbgack_wait(jtag_tap_t * tap, unsigned int tmo);

/* Execute instruction at debug speed */
int jtag_arm7_step(jtag_tap_t * tap, int dclk, uint32_t din, uint32_t * dout);

/* Execute instruction at system speed */
int jtag_arm7_step_sys(jtag_tap_t * tap, uint32_t insn1, uint32_t insn2);

int jtag_arm7_step_brk(jtag_tap_t * tap, int dclk, uint32_t din);

//int jtag_arm7_step_sync(jtag_tap_t * tap, int dclk, uint32_t din);

int jtag_arm7_sys_sync(jtag_tap_t * tap);

/***************************************************************************
 *
 * ARM9 JTAG operations
 *
 ***************************************************************************/

int jtag_arm9_dbg_status(jtag_tap_t * tap);

int jtag_arm9_dbgack_wait(jtag_tap_t * tap, unsigned int tmo);

/* Execute instruction at debug speed */
int jtag_arm9_step(jtag_tap_t * tap, int dclk, uint32_t insn);

/* Execute instruction at system speed */
int jtag_arm9_step_sys(jtag_tap_t * tap, uint32_t insn);

/* Write into data pipeline */
int jtag_arm9_step_wr(jtag_tap_t * tap, int dclk, uint32_t din);

/* Read from data pipeline */
int jtag_arm9_step_rd(jtag_tap_t * tap, int dclk, uint32_t * dout);

/* Write into data pipeline when in arm mode */
int jtag_arm9_step_thumb_wr(jtag_tap_t * tap, int dclk, uint32_t din);

/* Read from data pipeline when in thumb mode */
int jtag_arm9_step_thumb_rd(jtag_tap_t * tap, int dclk, uint32_t * dout);

int jtag_arm9_step_sync(jtag_tap_t * tap, uint32_t insn);

int jtag_arm9_sys_sync(jtag_tap_t * tap);

int jtag_arm926_cp15_rd(jtag_tap_t * tap, int addr, uint32_t * data);
int jtag_arm926_cp15_wr(jtag_tap_t * tap, int addr, uint32_t data);


/***************************************************************************
 *
 * ARM debug status and DCC polling
 *
 ***************************************************************************/

int jtag_arm_poll_stop(jtag_tap_t * tap);

int jtag_arm_poll_start(jtag_tap_t * tap);

int jtag_arm_halt_wait(jtag_tap_t * tap, int mutex, int tmo);

/***************************************************************************
 *
 * ARM DCC low level input output 
 *
 ***************************************************************************/

/* Put data directly into DCC channel  */
int jtag_arm_dcc_out(jtag_tap_t * tap, uint32_t data, unsigned int tmo);

/* Get data directly from DCC channel  */
int jtag_arm_dcc_in(jtag_tap_t * tap, uint32_t * data, unsigned int tmo);

/***************************************************************************
 *
 * ARM DCC buffered read and write
 *
 ***************************************************************************/

int jtag_arm_poll(jtag_tap_t * tap, jtag_arm_poll_t * poll, 
				  ice_comm_t * comm);

#ifdef __cplusplus
	}
#endif

#endif /* __JTAG_ARM_H__ */

