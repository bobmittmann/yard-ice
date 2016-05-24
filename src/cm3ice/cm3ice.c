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
 * @file cm3ice.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */



#ifdef CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <sys/os.h>

#include "jtag_adi.h"
#include "cm3ice.h"
#include "arm_pn.h"
#include "arm-fpb.h"
#include "arm-dwt.h"
#include "dbglog.h"

#include "trace.h"

#ifndef ENABLE_CM3ICE_DP_STICKYERR
#define ENABLE_CM3ICE_DP_STICKYERR 0
#endif

static inline int dp_stickyerr_get(jtag_tap_t * tap)
{
	uint32_t stat;

	stat = jtag_dp_ctrl_stat_get(tap);

	return (stat & DP_STICKYERR);
}

static inline void dp_stickyerr_clr(jtag_tap_t * tap)
{
	/* clear stick error */
	jtag_dp_ctrl_set(tap, DP_STICKYERR);
}

/***********************************************************************
 ARM FlashPatch and Breakpoint Unit
 ***********************************************************************/

static int fpb_probe(jtag_tap_t * tap, armv7m_fpb_ctrl_t * fpb, 
					  uint32_t addr)
{
	uint32_t fp_ctrl;

	fpb->base = addr;

	DCC_LOG1(LOG_TRACE, "FPB @ 0x%08x", fpb->base); 

	if (jtag_mem_ap_rd32(tap, fpb->base + FP_CTRL_OFFS, 
						 &fp_ctrl) != JTAG_ADI_ACK_OK_FAULT) {
		DCC_LOG(LOG_WARNING, "jtag_mem_ap_rd32() failed!"); 
		return ICE_ERR_JTAG;
	}

	DCC_LOG2(LOG_TRACE, "FPB: code=%d lit=%d", FP_NUM_CODE(fp_ctrl),
			 FP_NUM_LIT(fp_ctrl));
	fpb->code_max = FP_NUM_CODE(fp_ctrl);
	fpb->lit_max = FP_NUM_LIT(fp_ctrl);
	fpb->code_base = fpb->base + FP_COMP0_OFFS;
	fpb->lit_base = fpb->code_base + 4 * fpb->code_max;
	fpb->code_bmp = 0;
	fpb->lit_bmp = 0;

	return ICE_OK;
}

static int fpb_clear(jtag_tap_t * tap, armv7m_fpb_ctrl_t * fpb)
{
	int id;

	fpb->code_bmp = 0;

	for (id = 0; id < fpb->code_max; id++) {
		if (jtag_mem_ap_wr32(tap, fpb->code_base + 4 * id, 
							 0) != JTAG_ADI_ACK_OK_FAULT) {
			DCC_LOG(LOG_WARNING, "jtag_mem_ap_wr32() failed!"); 
			return ICE_ERR_JTAG;
		}
	}

	fpb->lit_bmp = 0;

	for (id = 0; id < fpb->lit_max; id++) {
		if (jtag_mem_ap_wr32(tap, fpb->lit_base + 4 * id, 
							 0) != JTAG_ADI_ACK_OK_FAULT) {
			DCC_LOG(LOG_WARNING, "jtag_mem_ap_wr32() failed!"); 
			return ICE_ERR_JTAG;
		}
	}

	return 0;
}

static int fpb_code_comp_set(jtag_tap_t * tap, armv7m_fpb_ctrl_t * fpb, 
							 uint32_t comp)
{
	int id;

	id = ffs(~fpb->code_bmp) - 1;

	if ((id < 0) || (id >= fpb->code_max)) {
		DCC_LOG(LOG_WARNING, "all HW breakpoints are being used!"); 
		return ICE_ERR_UNDEF;
	}

	if (jtag_mem_ap_wr32(tap, fpb->code_base + 4 * id, 
						 comp) != JTAG_ADI_ACK_OK_FAULT) {
		DCC_LOG(LOG_WARNING, "jtag_mem_ap_wr32() failed!"); 
		return ICE_ERR_JTAG;
	}

	fpb->code_bmp |= (1 << id);

	return id;
}

static int fpb_code_comp_clr(jtag_tap_t * tap, armv7m_fpb_ctrl_t * fpb, 
							 int id)
{
	if (id >= fpb->code_max) {
		DCC_LOG1(LOG_WARNING, "invalid hardware BP: %d !", id);
		return ICE_ERR_ARG;
	}

	if ((fpb->code_bmp & (1 << id)) == 0)
		return ICE_OK;

	/* clear the allocation flag */
	fpb->code_bmp &= ~(1 << id);

	if (jtag_mem_ap_wr32(tap, fpb->code_base + 4 * id, 
						 0) != JTAG_ADI_ACK_OK_FAULT) {
		DCC_LOG(LOG_WARNING, "jtag_mem_ap_wr32() failed!"); 
		return ICE_ERR_JTAG;
	}

	return ICE_OK;
}

#if 0
static void fpb_code_comp_clr_all(jtag_tap_t * tap, armv7m_fpb_ctrl_t * fpb)
{
	int id;

	for (id = 0; id < fpb->code_max; id++)
		fpb_code_comp_clr(tap, fpb, id);
}
#endif

static int fpb_enable(jtag_tap_t * tap, armv7m_fpb_ctrl_t * fpb)
{
	/* enable the FPB unit */
	if (jtag_mem_ap_wr32(tap, fpb->base + FP_CTRL_OFFS, 
						 FP_KEY | FP_ENABLE) != JTAG_ADI_ACK_OK_FAULT) {
		DCC_LOG(LOG_WARNING, "jtag_mem_ap_wr32() failed!"); 
		return ICE_ERR_JTAG;
	}

	return ICE_OK;
}

static int fpb_disable(jtag_tap_t * tap, armv7m_fpb_ctrl_t * fpb)
{
	/* disable the FPB unit */
	if (jtag_mem_ap_wr32(tap, fpb->base + FP_CTRL_OFFS, 
						 FP_KEY | FP_DISABLE) != JTAG_ADI_ACK_OK_FAULT) {
		DCC_LOG(LOG_WARNING, "jtag_mem_ap_wr32() failed!"); 
		return ICE_ERR_JTAG;
	}

	return ICE_OK;
}

/****************************************************************************
  ARM DWT (Data Watchpoint and Trace Unit)
 ****************************************************************************/

static int dwt_probe(jtag_tap_t * tap, armv7m_dwt_ctrl_t * dwt, 
					  uint32_t addr)
{
	uint32_t ctrl;

	dwt->base = addr;

	DCC_LOG1(LOG_TRACE, "FPB @ 0x%08x", dwt->base); 

	if (jtag_mem_ap_rd32(tap, dwt->base + DWT_CTRL_OFFS, 
						 &ctrl) != JTAG_ADI_ACK_OK_FAULT) {
		DCC_LOG(LOG_WARNING, "jtag_mem_ap_rd32() failed!"); 
		return ICE_ERR_JTAG;
	}

	DCC_LOG2(LOG_TRACE, "FPB: code=%d lit=%d", FP_NUM_CODE(fp_ctrl),
			 FP_NUM_LIT(fp_ctrl));
	dwt->numcomp = DWT_NUMCOMP(ctrl);
	dwt->comp_base = dwt->base + DWT_COMP_OFFS;
	dwt->comp_bmp = 0;

	return ICE_OK;
}

static int dwt_clear(jtag_tap_t * tap, armv7m_dwt_ctrl_t * dwt)
{
	int id;

	dwt->comp_bmp = 0;

	for (id = 0; id < dwt->numcomp; id++) {
		if (jtag_mem_ap_wr32(tap, dwt->comp_base + 4 * id, 
							 0) != JTAG_ADI_ACK_OK_FAULT) {
			DCC_LOG(LOG_WARNING, "jtag_mem_ap_wr32() failed!"); 
			return ICE_ERR_JTAG;
		}
	}

	return 0;
}

static int dwt_comp_set(jtag_tap_t * tap, armv7m_dwt_ctrl_t * dwt, 
							 uint32_t comp)
{
	int id;

	id = ffs(~dwt->comp_bmp) - 1;

	if ((id < 0) || (id >= dwt->numcomp)) {
		DCC_LOG(LOG_WARNING, "all HW breakpoints are being used!"); 
		return ICE_ERR_UNDEF;
	}

	if (jtag_mem_ap_wr32(tap, dwt->comp_base + 4 * id, 
						 comp) != JTAG_ADI_ACK_OK_FAULT) {
		DCC_LOG(LOG_WARNING, "jtag_mem_ap_wr32() failed!"); 
		return ICE_ERR_JTAG;
	}

	dwt->comp_bmp |= (1 << id);

	return id;
}

static int dwt_comp_clr(jtag_tap_t * tap, armv7m_dwt_ctrl_t * dwt, 
							 int id)
{
	if (id >= dwt->numcomp) {
		DCC_LOG1(LOG_WARNING, "invalid hardware BP: %d !", id);
		return ICE_ERR_ARG;
	}

	if ((dwt->comp_bmp & (1 << id)) == 0)
		return ICE_OK;

	/* clear the allocation flag */
	dwt->comp_bmp &= ~(1 << id);

	if (jtag_mem_ap_wr32(tap, dwt->comp_base + 4 * id, 
						 0) != JTAG_ADI_ACK_OK_FAULT) {
		DCC_LOG(LOG_WARNING, "jtag_mem_ap_wr32() failed!"); 
		return ICE_ERR_JTAG;
	}

	return ICE_OK;
}

#if 0
static void dwt_code_comp_clr_all(jtag_tap_t * tap, armv7m_dwt_ctrl_t * dwt)
{
	int id;

	for (id = 0; id < dwt->code_max; id++)
		dwt_code_comp_clr(tap, dwt, id);
}
#endif

static int dwt_enable(jtag_tap_t * tap, armv7m_dwt_ctrl_t * dwt)
{
	/* enable the FPB unit */
	if (jtag_mem_ap_wr32(tap, dwt->base + DWT_CTRL_OFFS, 
						 FP_KEY | FP_ENABLE) != JTAG_ADI_ACK_OK_FAULT) {
		DCC_LOG(LOG_WARNING, "jtag_mem_ap_wr32() failed!"); 
		return ICE_ERR_JTAG;
	}

	return ICE_OK;
}

static int dwt_disable(jtag_tap_t * tap, armv7m_dwt_ctrl_t * dwt)
{
	/* disable the FPB unit */
	if (jtag_mem_ap_wr32(tap, dwt->base + DWT_CTRL_OFFS, 
						 FP_KEY | FP_DISABLE) != JTAG_ADI_ACK_OK_FAULT) {
		DCC_LOG(LOG_WARNING, "jtag_mem_ap_wr32() failed!"); 
		return ICE_ERR_JTAG;
	}

	return ICE_OK;
}

/***********************************************************************
 Core register access
 ***********************************************************************/

static const uint8_t cm3_dcrsr_lut[] = {
	DCRSR_REGSEL_R0,
	DCRSR_REGSEL_R1,
	DCRSR_REGSEL_R2,
	DCRSR_REGSEL_R3,
	DCRSR_REGSEL_R4,
	DCRSR_REGSEL_R5,
	DCRSR_REGSEL_R6,
	DCRSR_REGSEL_R7,
	DCRSR_REGSEL_R8,
	DCRSR_REGSEL_R9,
	DCRSR_REGSEL_R10,
	DCRSR_REGSEL_R11,
	DCRSR_REGSEL_R12,
	DCRSR_REGSEL_SP,
	DCRSR_REGSEL_LR,
	DCRSR_REGSEL_DBG_RET_ADDR,
	DCRSR_REGSEL_XPSR,
	DCRSR_REGSEL_MSP,
	DCRSR_REGSEL_PSP,
	DCRSR_REGSEL_CTRL,
	DCRSR_REGSEL_FPSCR };

static int core_reg_get(jtag_tap_t * tap, struct cm3_core_ctrl * core, 
						unsigned int id, uint32_t * ptr)
{
	uint32_t dhcsr;
	uint32_t reg;

	if ((core->cache_bmp & (1 << id)) == 0) {
/* To transfer a data word from an ARM core register, special-purpose 
   register, or Floating-point extension register, a debugger:
	1. Writes to the DCRSR, with the REGSEL value indicating the required 
	register, and the REGWnR bit as 0 to indicate a read access.
	This write clears the DHCSR.S_REGRDY bit to 0.
	2. Polls DHCSR until DHCSR.S_REGRDY reads-as-one. This shows that 
	the processor has transferred the value of the selected register to DCRDR.
	3. Reads the required value from DCRDR. 
*/
		reg = cm3_dcrsr_lut[id];
		
		if (jtag_mem_ap_wr32(tap, ARMV7M_DCRSR, reg) != JTAG_ADI_ACK_OK_FAULT) {
			DCC_LOG(LOG_WARNING, "jtag_mem_ap_wr32() failed!"); 
			return ICE_ERR_JTAG;
		}

		do {
			if (jtag_mem_ap_rd32(tap, ARMV7M_DHCSR, 
								 &dhcsr) != JTAG_ADI_ACK_OK_FAULT) {
				DCC_LOG(LOG_WARNING, "jtag_mem_ap_rd32() failed!"); 
				return ICE_ERR_JTAG;
			}
		} while ((dhcsr & DHCSR_S_REGRDY) == 0);

		if (jtag_mem_ap_rd32(tap, ARMV7M_DCRDR, 
							 &core->reg[id]) != JTAG_ADI_ACK_OK_FAULT) {
			DCC_LOG(LOG_WARNING, "jtag_mem_ap_rd32() failed!"); 
			return ICE_ERR_JTAG;
		}

		core->cache_bmp |= (1 << id);
	};

	if (ptr != NULL)
		*ptr = core->reg[id];

	return ICE_OK;
}

static int core_reg_set(jtag_tap_t * tap, struct cm3_core_ctrl * core, 
						unsigned int id, uint32_t data)
{
	uint32_t dhcsr;
	uint32_t reg;

	if ((core->cache_bmp & (1 << id)) &&
		(data == core->reg[id])) {
		return ICE_OK;
	}

/* To transfer a data word to an ARM core register, special-purpose 
   register, or Floating-point extension register, a debugger:

   1. Writes the required word to DCRDR.
   2. Writes to the DCRSR, with the REGSEL value indicating the required 
   register, and the REGWnR bit as 1 to indicate a write access. 
   This write clears the DHCSR S_REGRDY bit to 0.
   3. If required, polls DHCSR until DHCSR.S_REGRDY reads-as-one. This shows 
   that the processor has transferred the DCRDR value to the selected register.
*/
	if (jtag_mem_ap_wr32(tap, ARMV7M_DCRDR, 
						 data) != JTAG_ADI_ACK_OK_FAULT) {
		DCC_LOG(LOG_WARNING, "jtag_mem_ap_wr32() failed!"); 
		return ICE_ERR_JTAG;
	}

	reg = cm3_dcrsr_lut[id];

	if (jtag_mem_ap_wr32(tap, ARMV7M_DCRSR, 
						 reg | DCRSR_REGWNR) != JTAG_ADI_ACK_OK_FAULT) {
		DCC_LOG(LOG_WARNING, "jtag_mem_ap_wr32() failed!"); 
		return ICE_ERR_JTAG;
	}

	do {
		if (jtag_mem_ap_rd32(tap, ARMV7M_DHCSR, 
							 &dhcsr) != JTAG_ADI_ACK_OK_FAULT) {
			DCC_LOG(LOG_WARNING, "jtag_mem_ap_rd32() failed!"); 
			return ICE_ERR_JTAG;
		}
	} while ((dhcsr & DHCSR_S_REGRDY) == 0);

	core->reg[id] = data;
	core->cache_bmp |= (1 << id);

	return ICE_OK;
}

/***********************************************************************
 Events 
 ***********************************************************************/

/* stop the core */
static int core_break_req(cm3ice_ctrl_t * ctrl)
{
	jtag_tap_t * tap = ctrl->tap;

	if (jtag_mem_ap_wr32(tap, ARMV7M_DHCSR, DHCSR_DBGKEY | DHCSR_C_HALT | 
						 DHCSR_C_DEBUGEN) != JTAG_ADI_ACK_OK_FAULT) {
		DCC_LOG(LOG_WARNING, "jtag_mem_ap_wr32() failed!"); 

		return ICE_ERR_JTAG;
	}

	return ICE_OK;
}


/* restart the core */
static int core_resume(cm3ice_ctrl_t * ctrl)
{
	jtag_tap_t * tap = ctrl->tap;

	ctrl->core.cache_bmp = 0;

	if (jtag_mem_ap_wr32(tap, ARMV7M_DHCSR, DHCSR_DBGKEY | 
						 DHCSR_C_DEBUGEN) != JTAG_ADI_ACK_OK_FAULT) {
		DCC_LOG(LOG_WARNING, "jtag_mem_ap_wr32() failed!"); 

		return ICE_ERR_JTAG;
	}

	return ICE_OK;
}

#if 0
static void cm3ice_context_update(cm3ice_ctrl_t * ctrl, cm3ice_context_t * ct)
{
	jtag_tap_t * tap = ctrl->tap;
	unsigned int id;

	if (ctrl->flags & CM3ICE_CTX_SAVED)
		return;

	DCC_LOG(LOG_TRACE, "saving context <<< <<< ");

	for (id = CM3_R0; id <= CM3_CTRL; id++)
		core_reg_get(tap, &ctrl->core, id, &ct->r[id]);

	ctrl->flags |= CM3ICE_CTX_SAVED;
}

static void cm3ice_context_restore(cm3ice_ctrl_t * ctrl, cm3ice_context_t * ct)
{
	jtag_tap_t * tap = ctrl->tap;
	unsigned int id;

	if ((ctrl->flags & CM3ICE_CTX_SAVED) == 0)
		return;

	DCC_LOG(LOG_TRACE, "restoring context >>> >>>");

	for (id = CM3_R0; id <= CM3_CTRL; id++)
		core_reg_set(tap, &ctrl->core, id, ct->r[id]);

	ctrl->flags &= ~CM3ICE_CTX_SAVED;
}
#endif

/***********************************************************************
 ICE Driver Methods
 ***********************************************************************/
static int cm3ice_comm_sync(cm3ice_ctrl_t * ctrl, ice_comm_t * comm)
{
	jtag_tap_t * tap = ctrl->tap;

	/* Reload the COMM address */
	if (jtag_mem_ap_rd32(tap, 8 * 4, 
						 &ctrl->comm_addr) != JTAG_ADI_ACK_OK_FAULT) {
		DCC_LOG(LOG_WARNING, "jtag_mem_ap_rd32() failed!"); 
		return ICE_ST_FAULT;
	}

	if ((ctrl->comm_addr == 0x00000000) || (ctrl->comm_addr == 0xffffffff)) {
		DCC_LOG1(LOG_INFO, "comm block not found! comm_addr=0x%08x", 
				 ctrl->comm_addr);
		return 0;
	}

	DCC_LOG1(LOG_TRACE, "COMM=0x%08x", ctrl->comm_addr);

	/* get the state of the device's COMM buffer */
	jtag_mem_ap_read(tap, ctrl->comm_addr, &ctrl->cc, 8);

	DCC_LOG4(LOG_TRACE, "COMM: dbg=%d dev=%d tx_tail=%d tx_head=%d", 
			 ctrl->cc.rw.dbg, ctrl->cc.ro.dev, 
			 ctrl->cc.rw.tx_tail, ctrl->cc.ro.tx_head);

	if (ctrl->cc.ro.dev == DEV_CONNECTED) {
		if (ctrl->cc.rw.dbg == DBG_CONNECTED) {
			DCC_LOG(LOG_TRACE, "COMM already connected...");
			return 0;
		}
		if (ctrl->cc.rw.dbg != DBG_SYNC) {
			DCC_LOG1(LOG_WARNING, 
					 "DEV=CONNECTED, DBG!=(SYNC|CONNECTED) %02x??", 
					 ctrl->cc.rw.dbg);
			return 0;
		}
		DCC_LOG(LOG_TRACE, "COMM: [CONNECTED]"); 

		ctrl->cc.rw.dbg = DBG_CONNECTED;
		/* update the state of the buffer */
		if (jtag_mem_ap_wr32(tap, ctrl->comm_addr + 4, 
							 ctrl->cc.rw.u32) != JTAG_ADI_ACK_OK_FAULT) {
			DCC_LOG(LOG_WARNING, "jtag_mem_ap_wr32() failed!"); 
			return ICE_ST_FAULT;
		}
		return 0;
	}

	if (ctrl->cc.ro.dev == DEV_SYNC) {
		if (ctrl->cc.rw.dbg == DBG_SYNC) {
			DCC_LOG(LOG_TRACE, "DEV=SYNC, DBG=SYNC, wating ....");
			return 0;
		}	
		if (ctrl->cc.rw.dbg == DBG_CONNECTED) {
			DCC_LOG(LOG_WARNING, "DEV=SYNC, DBG=CONNECTED ???");
		}
		DCC_LOG(LOG_TRACE, "COMM: [SYNC]"); 
		ctrl->cc.rw.dbg = DBG_SYNC;
		/* ensure the buffer is flushed */
		ctrl->cc.rw.tx_tail = 0;
		ctrl->cc.rw.rx_head = 0;
		/* update the state of the buffer */
		if (jtag_mem_ap_wr32(tap, ctrl->comm_addr + 4, 
							 ctrl->cc.rw.u32) != JTAG_ADI_ACK_OK_FAULT) {
			DCC_LOG(LOG_WARNING, "jtag_mem_ap_wr32() failed!"); 
			return ICE_ST_FAULT;
		}

		return 0;
	}

	DCC_LOG(LOG_INFO, "COMM: [IDLE]");

	return 0;
}

static int cm3ice_comm_poll(cm3ice_ctrl_t * ctrl, ice_comm_t * comm)
{
	jtag_tap_t * tap = ctrl->tap;
	uint32_t data;
	uint32_t head;
	uint32_t addr;
	int i;
	int rem;
	int cnt;

	if (ctrl->comm_addr == 0x00000000)
		return 0;

	while (ctrl->poll_enabled) {

		/* get the remote COMM channel data */
		if (jtag_mem_ap_rd32(tap, ctrl->comm_addr, 
							 &ctrl->cc.ro.u32) != JTAG_ADI_ACK_OK_FAULT) {
			DCC_LOG(LOG_WARNING, "jtag_mem_ap_rd32() failed!"); 
			return ICE_ST_FAULT;
		}

		if (dp_stickyerr_get(tap)) {
			/* The comm block is invalid and resulted in
			   a MEM-AP access error */
			DCC_LOG(LOG_WARNING, "JTAD DP STICKYERR flag set!");
			dp_stickyerr_clr(tap);
			/* Set the COMM address as invalid */
			ctrl->comm_addr = 0x00000000;
			return 0;
		}

		if ((ctrl->cc.ro.dev != DEV_CONNECTED) ||
			(ctrl->cc.rw.dbg != DBG_CONNECTED)) {
			return cm3ice_comm_sync(ctrl, comm);
		}

		/* get the ICE-COMM buffer info */
		head = comm->rx.head;
		rem = ICE_COMM_RX_BUF_LEN - (head - comm->rx.tail);
		if (rem == 0) {
			DCC_LOG(LOG_MSG, "DCC buffer full!");
			break;
		}

		/* get the number of words in the buffer */
		cnt = (ctrl->cc.ro.tx_head - ctrl->cc.rw.tx_tail) & 0xffff;

		if (cnt == 0)
			break;

		if (cnt > 16) {
			DCC_LOG3(LOG_WARNING, "head=%d tail=%d cnt=%d > 16!!!",
					 ctrl->cc.ro.tx_head, ctrl->cc.rw.tx_tail, cnt);

			/* flushes the buffer */
			ctrl->cc.rw.tx_tail = ctrl->cc.ro.tx_head;
			/* resynchronize */
			ctrl->cc.rw.dbg = DBG_SYNC;
			DCC_LOG(LOG_TRACE, "COMM: [SYNC]"); 
			/* update the state of the buffer */
			if (jtag_mem_ap_wr32(tap, ctrl->comm_addr + 4, 
								 ctrl->cc.rw.u32) != JTAG_ADI_ACK_OK_FAULT) {
				DCC_LOG(LOG_WARNING, "jtag_mem_ap_wr32() failed!"); 
				return ICE_ST_FAULT;
			}
			break;
		}

		DCC_LOG3(LOG_TRACE, "head=%d tail=%d cnt=%d",
				 ctrl->cc.ro.tx_head, ctrl->cc.rw.tx_tail, cnt);

		for (i = 0; i < cnt; i++) {
			if (rem == 0) {
				DCC_LOG(LOG_INFO, "COMM buffer full!");
				break;
			}
			
			addr = ctrl->comm_addr + 8 + 4 * (ctrl->cc.rw.tx_tail & 0x0f);

			if (jtag_mem_ap_rd32(tap, addr, 
								 &data) != JTAG_ADI_ACK_OK_FAULT) {
				DCC_LOG(LOG_WARNING, "jtag_mem_ap_rd32() failed!"); 
				return ICE_ST_FAULT;
			}
		
			ctrl->cc.rw.tx_tail++;

			DCC_LOG1(LOG_INFO, "data=%08x", data);

			comm->rx.buf[head & (ICE_COMM_RX_BUF_LEN - 1)] = data;
			head++;
			comm->rx.head = head;
			rem--;
			if (rem == (ICE_COMM_RX_BUF_LEN / 2)) {
				DCC_LOG(LOG_MSG, "DCC notify!");
				__os_sem_post(comm->rx_sem);
			}
		}

		/* update the state of the buffer */
		if (jtag_mem_ap_wr32(tap, ctrl->comm_addr + 4, 
							 ctrl->cc.rw.u32) != JTAG_ADI_ACK_OK_FAULT) {
			DCC_LOG(LOG_WARNING, "jtag_mem_ap_wr32() failed!"); 
			return ICE_ST_FAULT;
		}
	}

	return 0;
}

int cm3ice_poll(cm3ice_ctrl_t * ctrl, ice_comm_t * comm)
{
	jtag_tap_t * tap = ctrl->tap;
	uint32_t dhcsr;
	int ret;
//	uint32_t dfsr;
	
#if 0	
		if (jtag_mem_ap_rd32(tap, ARMV7M_DFSR, &dfsr) != JTAG_ADI_ACK_OK_FAULT) {
			DCC_LOG(LOG_WARNING, "jtag_mem_ap_rd32() failed!"); 
			ctrl->polling = false;
			return ICE_ST_FAULT;
		}

		DCC_LOG5(LOG_TRACE, " EXTERNAL=%d VCATCH=%d DWTTRAP=%d BKPT=%d HALTED=%d",
				 (dfsr & DFSR_EXTERNAL) ? 1: 0, (dfsr & DFSR_VCATCH) ? 1: 0,
				 (dfsr & DFSR_DWTTRAP) ? 1: 0, (dfsr & DFSR_BKPT) ? 1: 0,
				 (dfsr & DFSR_HALTED) ? 1: 0);
#endif

	DCC_LOG(LOG_TRACE, "cm3ice_comm_sync()..."); 
	if ((ret = cm3ice_comm_sync(ctrl, comm)) != 0) {
		DCC_LOG(LOG_WARNING, "cm3ice_comm_sync() failed!"); 
		ctrl->polling = false;
		return ret;
	}

	while (ctrl->poll_enabled) {

		if ((ret = cm3ice_comm_poll(ctrl, comm)) != 0) {
			DCC_LOG(LOG_WARNING, "cm3ice_comm_poll() failed!"); 
			ctrl->polling = false;
			return ret;
		}

		if (jtag_mem_ap_rd32(tap, ARMV7M_DHCSR, 
							 &dhcsr) != JTAG_ADI_ACK_OK_FAULT) {
			DCC_LOG(LOG_WARNING, "jtag_mem_ap_rd32() failed!"); 
			ctrl->polling = false;
			return ICE_ST_FAULT;
		}

		ctrl->dhcsr = dhcsr;
		if (dhcsr & DHCSR_S_HALT)
			break;
	}

	ctrl->polling = false;

	return (dhcsr & DHCSR_S_HALT) ? ICE_ST_HALT : 0;
}

int cm3ice_signal(cm3ice_ctrl_t * ctrl, ice_sig_t sig)
{
	switch (sig) {
	case ICE_SIG_POLL_STOP:
		ctrl->poll_enabled = false;
		break;
	case ICE_SIG_POLL_START:
		ctrl->poll_enabled = true;
		ctrl->polling = true;
		break;
	case ICE_SIG_TARGET_RESET:
		break;
	} 
	return 0;
}

int cm3ice_status(cm3ice_ctrl_t * ctrl)
{
	jtag_tap_t * tap = ctrl->tap;
	uint32_t dhcsr;

	DCC_LOG(LOG_MSG, "1.");

	if (ctrl->polling) {
		DCC_LOG(LOG_MSG, "2.");
		dhcsr = ctrl->dhcsr;
	} else {
		DCC_LOG(LOG_MSG, "3.");
//		ctrl->jtag_lock = true;
		if (jtag_mem_ap_rd32(tap, ARMV7M_DHCSR, 
							 &dhcsr) != JTAG_ADI_ACK_OK_FAULT) {
			DCC_LOG(LOG_WARNING, "jtag_mem_ap_rd32() failed!"); 
			ctrl->jtag_lock = false;
			return ICE_ST_FAULT;
		}
//		ctrl->jtag_lock = false;
	}

	DCC_LOG5(LOG_INFO, "S_RESET_ST=%d S_RETIRE_ST=%d S_LOCKUP=%d "\
			 "S_SLEEP=%d S_HALT=%d", (dhcsr & DHCSR_S_RESET_ST) ? 1 : 0,
			 (dhcsr & DHCSR_S_RETIRE_ST) ? 1 : 0,
			 (dhcsr & DHCSR_S_LOCKUP) ? 1 : 0,
			 (dhcsr & DHCSR_S_SLEEP) ? 1 : 0,
			 (dhcsr & DHCSR_S_HALT) ? 1 : 0);

	return (dhcsr & DHCSR_S_HALT) ? ICE_ST_HALT : 0;
}

int cm3ice_connect(cm3ice_ctrl_t * ctrl, uint32_t idmask, uint32_t idcomp)
{
	jtag_tap_t * tap = ctrl->tap;
	uint32_t dhcsr;
//	uint32_t dfsr;
	int ret;

	if (dp_stickyerr_get(tap)) {
		DCC_LOG(LOG_WARNING, "JTAD DP STICKYERR flag set!");
		dp_stickyerr_clr(tap);
	}

	DCC_LOG2(LOG_TRACE, "idmask=%08x idcomp=%08x", idmask, idcomp);

	if (jtag_mem_ap_rd32(tap, ARMV7M_DHCSR, &dhcsr) != JTAG_ADI_ACK_OK_FAULT) {
		DCC_LOG(LOG_WARNING, "jtag_mem_ap_rd32() failed!"); 
		return ICE_ERR_JTAG;
	}

	DCC_LOG1(LOG_TRACE, "DHCSR: 0x%08x", dhcsr); 

	if (!(dhcsr & DHCSR_C_DEBUGEN)) { 
		/* enable debug */
		if (jtag_mem_ap_wr32(tap, ARMV7M_DHCSR, DHCSR_DBGKEY | 
							 DHCSR_C_DEBUGEN) != JTAG_ADI_ACK_OK_FAULT) {
			DCC_LOG(LOG_WARNING, "jtag_mem_ap_wr32() failed!"); 
			return ICE_ERR_JTAG;
		}
	}

	/* enable the FPB unit */
	if ((ret = fpb_enable(tap, &ctrl->fpb)) != ICE_OK) {
		DCC_LOG(LOG_WARNING, "fpb_enable() failed!"); 
		return ret;
	}

	/* enable the DWT unit */
	if ((ret = dwt_enable(tap, &ctrl->dwt)) != ICE_OK) {
		DCC_LOG(LOG_WARNING, "dwt_enable() failed!"); 
		return ret;
	}

	if (dhcsr & DHCSR_S_HALT) {
	} else {
		ctrl->core.cache_bmp = 0;
	}

/*
	if (jtag_mem_ap_rd32(tap, ARMV7M_DFSR, &dfsr) != JTAG_ADI_ACK_OK_FAULT) {
		DCC_LOG(LOG_WARNING, "jtag_mem_ap_rd32() failed!"); 
		return ICE_ERR_JTAG;
	}
*/

	/* clear DFSR */
	if (jtag_mem_ap_wr32(tap, ARMV7M_DFSR, 0x1f) != JTAG_ADI_ACK_OK_FAULT) {
		DCC_LOG(LOG_WARNING, "jtag_mem_ap_wr32() failed!"); 
		return ICE_ERR_JTAG;
	}

	DCC_LOG5(LOG_TRACE, "S_RESET_ST=%d S_RETIRE_ST=%d S_LOCKUP=%d "\
			 "S_SLEEP=%d S_HALT=%d", (dhcsr & DHCSR_S_RESET_ST) ? 1 : 0,
			 (dhcsr & DHCSR_S_RETIRE_ST) ? 1 : 0,
			 (dhcsr & DHCSR_S_LOCKUP) ? 1 : 0,
			 (dhcsr & DHCSR_S_SLEEP) ? 1 : 0,
			 (dhcsr & DHCSR_S_HALT) ? 1 : 0);

	DCC_LOG5(LOG_TRACE, "S_REGRDY=%d C_MASKINTS=%d C_STEP=%d "\
			 "C_HALT=%d C_DEBUGEN=%d", (dhcsr & DHCSR_S_REGRDY) ? 1 : 0,
			 (dhcsr & DHCSR_C_MASKINTS) ? 1 : 0,
			 (dhcsr & DHCSR_C_STEP) ? 1 : 0,
			 (dhcsr & DHCSR_C_HALT) ? 1 : 0,
			 (dhcsr & DHCSR_C_DEBUGEN) ? 1 : 0);

	if (dp_stickyerr_get(tap)) {
		DCC_LOG(LOG_ERROR, "JTAD DP STICKYERR flag is set!");
		return -1;
	}

	return 0;
}

int cm3ice_release(cm3ice_ctrl_t * ctrl)
{
	jtag_tap_t * tap = ctrl->tap;

	/* disable all comparators int the FPB unit */
	if (fpb_clear(tap, &ctrl->fpb) != ICE_OK) {
		DCC_LOG(LOG_WARNING, "fpb_clear() failed!"); 
	}

	/* disable the FPB unit */
	if (fpb_disable(tap, &ctrl->fpb) != ICE_OK) {
		DCC_LOG(LOG_WARNING, "fpb_disable() failed!"); 
	}

	/* disable all comparators int the DWT unit */
	if (dwt_clear(tap, &ctrl->dwt) != ICE_OK) {
		DCC_LOG(LOG_WARNING, "dwt_clear() failed!"); 
	}

	/* disable the DWT unit */
	if (dwt_disable(tap, &ctrl->dwt) != ICE_OK) {
		DCC_LOG(LOG_WARNING, "dwt_disable() failed!"); 
	}

	/* disable the halt Debug function */
	if (jtag_mem_ap_wr32(tap, ARMV7M_DHCSR, DHCSR_DBGKEY) != 
		JTAG_ADI_ACK_OK_FAULT) {
		DCC_LOG(LOG_WARNING, "jtag_mem_ap_wr32() failed!"); 
	}

	/* clear the DFSR */
	if (jtag_mem_ap_wr32(tap, ARMV7M_DFSR, 0x1f) != JTAG_ADI_ACK_OK_FAULT) {
		DCC_LOG(LOG_WARNING, "jtag_mem_ap_wr32() failed!"); 
		return ICE_ERR_JTAG;
	}

	if (ctrl->comm_addr != 0x00000000) {
		/* return the DBG to sync state  */
		ctrl->cc.rw.dbg = DBG_SYNC;
		DCC_LOG(LOG_TRACE, "COMM: [SYNC]"); 
		if (jtag_mem_ap_wr32(tap, ctrl->comm_addr + 4, 
							 ctrl->cc.rw.u32) != JTAG_ADI_ACK_OK_FAULT) {
			DCC_LOG(LOG_WARNING, "jtag_mem_ap_wr32() failed!"); 
			return ICE_ST_FAULT;
		}
	}

	ctrl->flags &= ~CM3ICE_CONNECTED;
	ctrl->core.cache_bmp = 0;
	
	return 0;
}

int cm3ice_halt(cm3ice_ctrl_t * ctrl)
{
	int ret;

	/* break the core */
	if ((ret = core_break_req(ctrl)) != ICE_OK) {
		DCC_LOG(LOG_ERROR, "core_break_req()"); 
	}

	return ret;
}

int cm3ice_run(cm3ice_ctrl_t * ctrl)
{
	int ret;

	DCC_LOG(LOG_INFO, "."); 

	if ((ret = core_resume(ctrl)) != ICE_OK) {
		DCC_LOG(LOG_ERROR, "core_resume()");
	}

	return ret;
}

int cm3ice_goto(cm3ice_ctrl_t * ctrl, unsigned int addr)
{
	int ret;

	if ((ret = core_reg_set(ctrl->tap, &ctrl->core, CM3_PC, addr)) != ICE_OK) {
		DCC_LOG(LOG_WARNING, "jtag_mem_ap_wr8() failed!"); 
	}

	return ret;
}

int cm3ice_step(cm3ice_ctrl_t * ctrl)
{
	jtag_tap_t * tap = ctrl->tap;
	int ret;

	if ((ret = jtag_mem_ap_wr32(tap, ARMV7M_DHCSR, DHCSR_DBGKEY | 
								DHCSR_C_STEP | 
								DHCSR_C_DEBUGEN)) < 0) {
		DCC_LOG(LOG_WARNING, "jtag_mem_ap_wr32() failed!"); 
		return ret;
	}

	ctrl->core.cache_bmp = 0; /* clear cache */

	return 0;
}

/***********************************************************************
 Initialization, finalization and configuration
 ***********************************************************************/

/* configure the ice driver */
int cm3ice_configure(cm3ice_ctrl_t * ctrl, jtag_tap_t * tap, 
					 ice_opt_t * opt, cm3ice_cfg_t * cfg)
{
	jtag_idcode_t idcode;
	int ret;

	if (cfg == NULL) {
		DCC_LOG(LOG_WARNING, "cfg == NULL"); 
		return -1;
	}

	if (cfg->endianness) {
		DCC_LOG(LOG_ERROR, "BIG endian unsupported.");
		return ICE_ERR_BIG_ENDIAN;
	}

	if (jtag_tap_idcode((jtag_tap_t *)tap, &idcode) != ICE_OK) {
		DCC_LOG(LOG_WARNING, "jtag_tap_idcode() failed!"); 
		return -1;
	}

	DCC_LOG1(LOG_TRACE, "idcode:%08x", idcode); 

	/* select the working TAP */
	jtag_tap_select(tap);

	/* configure the JTAG driver for ADIv5 operations */
	jtag_adi_tap_setup(tap);

	/* initialize the JTAG-DP */
	jtag_dp_init(tap);

	/* power up the debug domain */
	jtag_dp_dbg_pwr_up(tap);

	/* initialize the MEM-AP */
	if ((ret = jtag_mem_ap_init(tap)) < 0) {
		DCC_LOG(LOG_WARNING, "jtag_mem_ap_init() fail."); 
//		return ret;

		/* XXX: the sequence bellow is a guess and may not make any sense... */

		/* power down the system domain */
		jtag_dp_sys_pwr_down(tap);

		jtag_adi_abort(tap);

		/* power down the debug domain */
		jtag_dp_dbg_pwr_down(tap);

		/* power up the debug domain */
		jtag_dp_dbg_pwr_up(tap);

		jtag_dp_dbg_reset(tap);

		if ((ret = jtag_mem_ap_init(tap)) < 0) {
			DCC_LOG(LOG_WARNING, "jtag_mem_ap_init() fail."); 
			return ret;
		}
	}

	/* power up the system domain */
	jtag_dp_sys_pwr_up(tap);

	if ((ret = armv7m_probe(tap, &ctrl->dbg_map)) < 0) {
		DCC_LOG(LOG_WARNING, "armv7m_probe() fail."); 
		/* reset the debug domain */
		jtag_dp_dbg_reset(tap);
		if ((ret = armv7m_probe(tap, &ctrl->dbg_map)) < 0) {
			DCC_LOG(LOG_WARNING, "armv7m_probe() fail."); 
			return ret;
		}
	}

	/* initialize the ICE control structure */
	ctrl->tap = tap;
	DCC_LOG(LOG_TRACE, "[UNCONNECTED]");

	/* Configure ICE options */

	/* check for Flash Patch and Breakpoint Unit */
	memset(&ctrl->fpb, 0, sizeof(armv7m_fpb_ctrl_t));
	if (ctrl->dbg_map.fpb != 0x00000000) {
		fpb_probe(tap, &ctrl->fpb, ctrl->dbg_map.fpb);
		/* maximum number of hardware breakpoints */
		opt->bp_max = ctrl->fpb.code_max;
		/* default breakpoint's size */
		opt->bp_defsz = 2;
	}

	/* check for Flash Patch and Breakpoint Unit */
	memset(&ctrl->dwt, 0, sizeof(armv7m_dwt_ctrl_t));
	if (ctrl->dbg_map.dwt != 0x00000000) {
		dwt_probe(tap, &ctrl->dwt, ctrl->dbg_map.dwt);
		/* maximum number of hardware breakpoints */
		opt->wp_max = ctrl->dwt.numcomp;
		/* default breakpoint's size */
		opt->wp_defsz = 2;
	}

	return ICE_OK;
}

int cm3_show_xpsr(FILE * f, unsigned int psr);
int cm3_show_ctrl(FILE * f, unsigned int ctrl);
int cm3_show_shcsr(FILE * f, uint32_t shcsr);
int cm3_show_icsr(FILE * f, uint32_t icsr);

int cm3ice_context_show(cm3ice_ctrl_t * ctrl, FILE * f)
{
	uint32_t reg[CM3_CTRL + 1];
	jtag_tap_t * tap = ctrl->tap;
	unsigned int id;
	uint32_t dhcsr;
	int ipsr;

	jtag_mem_ap_rd32(tap, ARMV7M_DHCSR, &dhcsr);
	if (dhcsr & DHCSR_S_LOCKUP) {
		fprintf(f, " Core Locked up!!\n");
	}

	for (id = CM3_R0; id <= CM3_CTRL; id++)
		core_reg_get(tap, &ctrl->core, id, &reg[id]);

	fprintf(f, " xpsr= ");
	cm3_show_xpsr(f, reg[CM3_XPSR]);
	fprintf(f, "\n ctrl= ");
	cm3_show_ctrl(f, reg[CM3_CTRL]);

	ipsr = reg[CM3_XPSR] & 0x1ff;
	if (ipsr) {
		uint32_t icsr;
		uint32_t shcsr;
		fprintf(f, "\n icsr= ");
		jtag_mem_ap_rd32(tap, ARMV7M_ICSR, &icsr);
		cm3_show_icsr(f, icsr);
		fprintf(f, "\nshcsr= ");
		jtag_mem_ap_rd32(tap, ARMV7M_SHCSR, &shcsr);
		cm3_show_shcsr(f, shcsr);
	}

	fprintf(f, "\n\n");

	cm3_show_regs(f, reg);

	return ICE_OK;
}

int cm3ice_wr8(cm3ice_ctrl_t * ctrl, uint32_t addr, uint8_t data)
{
	if (jtag_mem_ap_wr8(ctrl->tap, addr, data) != JTAG_ADI_ACK_OK_FAULT) {
		DCC_LOG(LOG_WARNING, "jtag_mem_ap_wr8() failed!"); 
		return ICE_ERR_JTAG;
	}

	return ICE_OK;
}

int cm3ice_rd8(cm3ice_ctrl_t * ctrl, uint32_t addr, uint8_t * data)
{
	if (jtag_mem_ap_rd8(ctrl->tap, addr, data) != JTAG_ADI_ACK_OK_FAULT) {
		DCC_LOG(LOG_WARNING, "jtag_mem_ap_rd8() failed!"); 
		return ICE_ERR_JTAG;
	}

	return ICE_OK;
}

int cm3ice_wr16(cm3ice_ctrl_t * ctrl, uint32_t addr, uint16_t data)
{
	if (jtag_mem_ap_wr16(ctrl->tap, addr, data) != JTAG_ADI_ACK_OK_FAULT) {
		DCC_LOG(LOG_WARNING, "jtag_mem_ap_wr16() failed!"); 
		return ICE_ERR_JTAG;
	}

	return ICE_OK;
}

int cm3ice_rd16(cm3ice_ctrl_t * ctrl, uint32_t addr, uint16_t * data)
{
	if (jtag_mem_ap_rd16(ctrl->tap, addr, data) != JTAG_ADI_ACK_OK_FAULT) {
		DCC_LOG(LOG_WARNING, "jtag_mem_ap_rd16() failed!"); 
		return ICE_ERR_JTAG;
	}

	return ICE_OK;
}

int cm3ice_wr32(cm3ice_ctrl_t * ctrl, uint32_t addr, uint32_t data)
{
	if (jtag_mem_ap_wr32(ctrl->tap, addr, data) != JTAG_ADI_ACK_OK_FAULT) {
		DCC_LOG(LOG_WARNING, "jtag_mem_ap_wr32() failed!"); 
		return ICE_ERR_JTAG;
	}

#if ENABLE_CM3ICE_DP_STICKYERR
	if (dp_stickyerr_get(ctrl->tap)) {
		DCC_LOG1(LOG_WARNING, "addr=0x%08x, STICKYERR flag set!", addr);
		return ICE_ERR_JTAG;
	}
#endif

	return ICE_OK;
}

int cm3ice_rd32(cm3ice_ctrl_t * ctrl, uint32_t addr, uint32_t * data)
{
	if (jtag_mem_ap_rd32(ctrl->tap, addr, data) != JTAG_ADI_ACK_OK_FAULT) {
		DCC_LOG(LOG_WARNING, "jtag_mem_ap_rd32() failed!"); 
		return ICE_ERR_JTAG;
	}

#if ENABLE_CM3ICE_DP_STICKYERR
	if (dp_stickyerr_get(ctrl->tap)) {
		DCC_LOG1(LOG_WARNING, "addr=0x%08x, STICKYERR flag set!", addr);
		return ICE_ERR_JTAG;
	}
#endif

	return ICE_OK;
}

int cm3ice_reg_get(cm3ice_ctrl_t * ctrl, int reg, uint32_t * val)
{
	int ret;

	if (reg > CM3_CTRL)
		return ICE_ERR_ARG;

	if ((ret = core_reg_get(ctrl->tap, &ctrl->core, reg, val)) != ICE_OK) {
		DCC_LOG(LOG_WARNING, "core_reg_get() failed!"); 
	}

	return ret;
}

int cm3ice_reg_set(cm3ice_ctrl_t * ctrl, int reg, uint32_t val)
{
	int ret;

	if (reg > CM3_CTRL)
		return ICE_ERR_ARG;

	if ((ret = core_reg_set(ctrl->tap, &ctrl->core, reg, val)) != ICE_OK) {
		DCC_LOG(LOG_WARNING, "core_reg_set() failed!"); 
	}

	return ret;
}

#if 0
int cm3ice_sp_get(cm3ice_ctrl_t * ctrl, uint32_t * val)
{
	return cm3ice_reg_get(ctrl, 13, val);
}

int cm3ice_sp_set(cm3ice_ctrl_t * ctrl, uint32_t val)
{
	return cm3ice_reg_set(ctrl, 13, val);
}

int cm3ice_pc_get(cm3ice_ctrl_t * ctrl, uint32_t * val)
{
	return cm3ice_reg_get(ctrl, 15, val);
}

int cm3ice_pc_set(cm3ice_ctrl_t * ctrl, uint32_t val)
{
	return cm3ice_reg_set(ctrl, 15, val);
}
#endif

/* Instruction fetch address get */
int cm3ice_ifa_get(cm3ice_ctrl_t * ctrl, uint32_t * val)
{
	return core_reg_get(ctrl->tap, &ctrl->core, CM3_PC, val);
}

/* Instruction fetch address set */
int cm3ice_ifa_set(cm3ice_ctrl_t * ctrl, uint32_t val)
{
	return core_reg_set(ctrl->tap, &ctrl->core, CM3_PC, val);
}

static int core_read_memory(bfd_vma addr, uint8_t * buf, 
						 unsigned int len, struct disassemble_info *dinfo)
{
	cm3ice_ctrl_t * ctrl = (cm3ice_ctrl_t *)dinfo->application_data;
	uint16_t data[len / sizeof(uint16_t)];
	unsigned int pos;

	DCC_LOG2(LOG_INFO, "addr=0x%08x len=%d", addr, len);

	for (pos = 0; pos < len / 2; pos++) {
		if (cm3ice_rd16(ctrl, addr, &data[pos]) != ICE_OK) {
			DCC_LOG(LOG_WARNING, "ice_rd16(), fail");
			return -1;
		}
	}

	if (dp_stickyerr_get(ctrl->tap)) {
		DCC_LOG1(LOG_WARNING, "addr=0x%08x, STICKYERR flag set!", addr);
		dp_stickyerr_clr(ctrl->tap);
		return -1;
	}

	memcpy(buf, data, len);

	return 0;
}

int cm3_print_insn(cm3ice_ctrl_t * ctrl, uint32_t addr, 
				   struct disassemble_info * info)
{
	info->endian_code = BFD_ENDIAN_LITTLE;
	info->mach = bfd_mach_arm_unknown;
	info->flags = USER_SPECIFIED_MACHINE_TYPE;
	info->disassembler_options = "force-thumb";

	info->read_memory_func = core_read_memory;
	info->application_data = ctrl;

	return print_insn_little_arm(addr, info);
};


/*****************************************************************************
 * Breakpoints and Watchpoins
 *****************************************************************************/

int cm3ice_bp_set(cm3ice_ctrl_t * ctrl, uint32_t addr, 
				  uint32_t size, uint32_t * id_ptr)
{
	jtag_tap_t * tap = ctrl->tap;
	uint32_t comp;
	int id;

	comp = (addr & 0x3ffffffc) | COMP_ENABLE;

	if (size == 2) {
		if (addr & 0x02) {
			comp |= COMP_BP_HIGH;
		} else {
			comp |= COMP_BP_LOW;
		}
	} else if (size == 4) {
		comp |= COMP_BP_WORD;
	} else {
		DCC_LOG1(LOG_WARNING, "invalid BP size: %d!", size); 
		return -1;
	}

	if ((id = fpb_code_comp_set(tap, &ctrl->fpb, comp)) < 0) {
		DCC_LOG(LOG_WARNING, "fpb_code_comp_set() failed!");
		return id;
	}

	*id_ptr = id;

	return 0;
}

int cm3ice_bp_clr(cm3ice_ctrl_t * ctrl, uint32_t id)
{
	jtag_tap_t * tap = ctrl->tap;

	DCC_LOG1(LOG_TRACE, "clearing bp %d...", id); 

	return fpb_code_comp_clr(tap, &ctrl->fpb, id);
}

int cm3ice_wp_set(cm3ice_ctrl_t * ctrl, uint32_t addr, 
				  uint32_t size, uint32_t * id_ptr)
{
	jtag_tap_t * tap = ctrl->tap;
	uint32_t comp;
	int id;

	comp = (addr & 0x3ffffffc) | COMP_ENABLE;

/*	if (size == 2) {
		if (addr & 0x02) {
			comp |= COMP_BP_HIGH;
		} else {
			comp |= COMP_BP_LOW;
		}
	} else if (size == 4) {
		comp |= COMP_BP_WORD;
	} else {
		DCC_LOG1(LOG_WARNING, "invalid BP size: %d!", size); 
		return -1;
	}
*/

	if ((id = dwt_comp_set(tap, &ctrl->dwt, comp)) < 0) {
		DCC_LOG(LOG_WARNING, "dwt_comp_set() failed!");
		return id;
	}

	*id_ptr = id;

	return 0;
}

int cm3ice_wp_clr(cm3ice_ctrl_t * ctrl, uint32_t id)
{
	jtag_tap_t * tap = ctrl->tap;

	DCC_LOG1(LOG_TRACE, "clearing bp %d...", id); 

	return dwt_comp_clr(tap, &ctrl->dwt, id);
}

/*****************************************************************************
 * Reset
 *****************************************************************************/

int cm3ice_system_reset(cm3ice_ctrl_t * ctrl)
{
	jtag_tap_t * tap = ctrl->tap;
	uint32_t dhcsr;
	bool halt = false;

	DCC_LOG(LOG_TRACE, ".");

	if (jtag_mem_ap_rd32(tap, ARMV7M_DHCSR, &dhcsr) != JTAG_ADI_ACK_OK_FAULT) {
		DCC_LOG(LOG_WARNING, "jtag_mem_ap_rd32() failed!"); 
		return ICE_ERR_JTAG;
	}

	if (dhcsr & DHCSR_S_HALT) { 
		DCC_LOG(LOG_TRACE, "core halted");
		halt = true;
	}

	if (jtag_mem_ap_wr32(tap, ARMV7M_AIRCR, AIRCR_VECTKEY | 
						 AIRCR_SYSRESETREQ ) != JTAG_ADI_ACK_OK_FAULT) {
		DCC_LOG(LOG_WARNING, "jtag_mem_ap_wr32() failed!"); 
		return ICE_ERR_JTAG;
	}

	if (!halt)
		return ICE_OK;

	do {
		DCC_LOG(LOG_TRACE, "halt 2. ...");

		/* halt the core */
		if (jtag_mem_ap_wr32(tap, ARMV7M_DHCSR, DHCSR_DBGKEY | DHCSR_C_HALT | 
							 DHCSR_C_DEBUGEN) != JTAG_ADI_ACK_OK_FAULT) {
			DCC_LOG(LOG_WARNING, "jtag_mem_ap_wr32() failed!"); 
			return ICE_ERR_JTAG;
		}


		if (jtag_mem_ap_rd32(tap, ARMV7M_DHCSR, 
							 &dhcsr) != JTAG_ADI_ACK_OK_FAULT) {
			DCC_LOG(LOG_WARNING, "jtag_mem_ap_rd32() failed!"); 
			return ICE_ERR_JTAG;
		}
		DCC_LOG5(LOG_TRACE, "S_RESET_ST=%d S_RETIRE_ST=%d S_LOCKUP=%d "\
				 "S_SLEEP=%d S_HALT=%d", (dhcsr & DHCSR_S_RESET_ST) ? 1 : 0,
				 (dhcsr & DHCSR_S_RETIRE_ST) ? 1 : 0,
				 (dhcsr & DHCSR_S_LOCKUP) ? 1 : 0,
				 (dhcsr & DHCSR_S_SLEEP) ? 1 : 0,
				 (dhcsr & DHCSR_S_HALT) ? 1 : 0);
	} while ((dhcsr & DHCSR_S_HALT) == 0);

	return ICE_OK;
}

int cm3ice_core_reset(cm3ice_ctrl_t * ctrl)
{
	jtag_tap_t * tap = ctrl->tap;
	uint32_t dhcsr;
	uint32_t demcr;
	bool halt = false;

	DCC_LOG(LOG_TRACE, ".");

	if (jtag_mem_ap_rd32(tap, ARMV7M_DHCSR, &dhcsr) != JTAG_ADI_ACK_OK_FAULT) {
		DCC_LOG(LOG_WARNING, "jtag_mem_ap_rd32() failed!"); 
		return ICE_ERR_JTAG;
	}

	if (jtag_mem_ap_rd32(tap, ARMV7M_DEMCR, &demcr) != JTAG_ADI_ACK_OK_FAULT) {
		DCC_LOG(LOG_WARNING, "jtag_mem_ap_rd32() failed!"); 
		return ICE_ERR_JTAG;
	}

	if (dhcsr & DHCSR_S_HALT) { 
		DCC_LOG(LOG_TRACE, "core halted");
		halt = true;

		if ((demcr & DEMCR_VC_CORERESET) == 0) { 
			if (jtag_mem_ap_wr32(tap, ARMV7M_DEMCR, demcr |
								 DEMCR_VC_CORERESET) != JTAG_ADI_ACK_OK_FAULT) {
				DCC_LOG(LOG_WARNING, "jtag_mem_ap_wr32() failed!"); 
				return ICE_ERR_JTAG;
			}
		}

		if ((dhcsr & DHCSR_C_DEBUGEN) == 0) { 
			if (jtag_mem_ap_wr32(tap, ARMV7M_DHCSR, DHCSR_DBGKEY | dhcsr | 
								 DHCSR_C_DEBUGEN) != JTAG_ADI_ACK_OK_FAULT) {
				DCC_LOG(LOG_WARNING, "jtag_mem_ap_wr32() failed!"); 
				return ICE_ERR_JTAG;
			}
		}

	} else {
		DCC_LOG(LOG_TRACE, "halting...");
		/* halt the core but leave the C_DEBUGEN set
		   To force the processor to enter Debug state as soon as it comes 
		   out of reset, a debugger set DHCSR.C_DEBUGEN to 1, to enable 
		   halting debut, and sets DEMCR.VC_CORERESET to 1 to enable vector 
		   catch on the Reset exception. When the processor comes out 
		   of reset it sets DHCSR.C_HALT to 1,
		   and enters Debug state.
		 */
		if (jtag_mem_ap_wr32(tap, ARMV7M_DHCSR, DHCSR_DBGKEY | DHCSR_C_HALT | 
							 DHCSR_C_DEBUGEN) != JTAG_ADI_ACK_OK_FAULT) {
			DCC_LOG(LOG_WARNING, "jtag_mem_ap_wr32() failed!"); 
			return ICE_ERR_JTAG;
		}

		if (demcr & DEMCR_VC_CORERESET) { 
			if (jtag_mem_ap_wr32(tap, ARMV7M_DEMCR, 
					 demcr & ~DEMCR_VC_CORERESET) != JTAG_ADI_ACK_OK_FAULT) {
				DCC_LOG(LOG_WARNING, "jtag_mem_ap_wr32() failed!"); 
				return ICE_ERR_JTAG;
			}
		}

		halt = false;
	}

	/* Local reset */
	if (jtag_mem_ap_wr32(tap, ARMV7M_AIRCR, AIRCR_VECTKEY | AIRCR_VECTRESET |
						 AIRCR_VECTCLRACTIVE) != JTAG_ADI_ACK_OK_FAULT) {
		DCC_LOG(LOG_WARNING, "jtag_mem_ap_wr32() failed!"); 
		return ICE_ERR_JTAG;
	}

	return ICE_OK;

	do {
		if (jtag_mem_ap_rd32(tap, ARMV7M_DHCSR, 
							 &dhcsr) != JTAG_ADI_ACK_OK_FAULT) {
			DCC_LOG(LOG_WARNING, "jtag_mem_ap_rd32() failed!"); 
			return ICE_ERR_JTAG;
		}
		DCC_LOG5(LOG_TRACE, "S_RESET_ST=%d S_RETIRE_ST=%d S_LOCKUP=%d "\
				 "S_SLEEP=%d S_HALT=%d", (dhcsr & DHCSR_S_RESET_ST) ? 1 : 0,
				 (dhcsr & DHCSR_S_RETIRE_ST) ? 1 : 0,
				 (dhcsr & DHCSR_S_LOCKUP) ? 1 : 0,
				 (dhcsr & DHCSR_S_SLEEP) ? 1 : 0,
				 (dhcsr & DHCSR_S_HALT) ? 1 : 0);
	} while ((dhcsr & DHCSR_S_RESET_ST) == 0);

	if (!halt)
		return ICE_OK;

	
	do {
		DCC_LOG(LOG_TRACE, "halt 2. ...");

		/* halt the core */
		if (jtag_mem_ap_wr32(tap, ARMV7M_DHCSR, DHCSR_DBGKEY | DHCSR_C_HALT | 
							 DHCSR_C_DEBUGEN) != JTAG_ADI_ACK_OK_FAULT) {
			DCC_LOG(LOG_WARNING, "jtag_mem_ap_wr32() failed!"); 
			return ICE_ERR_JTAG;
		}


		if (jtag_mem_ap_rd32(tap, ARMV7M_DHCSR, 
							 &dhcsr) != JTAG_ADI_ACK_OK_FAULT) {
			DCC_LOG(LOG_WARNING, "jtag_mem_ap_rd32() failed!"); 
			return ICE_ERR_JTAG;
		}
		DCC_LOG5(LOG_TRACE, "S_RESET_ST=%d S_RETIRE_ST=%d S_LOCKUP=%d "\
				 "S_SLEEP=%d S_HALT=%d", (dhcsr & DHCSR_S_RESET_ST) ? 1 : 0,
				 (dhcsr & DHCSR_S_RETIRE_ST) ? 1 : 0,
				 (dhcsr & DHCSR_S_LOCKUP) ? 1 : 0,
				 (dhcsr & DHCSR_S_SLEEP) ? 1 : 0,
				 (dhcsr & DHCSR_S_HALT) ? 1 : 0);
	} while ((dhcsr & DHCSR_S_HALT) == 0);

	return ICE_OK;

}



/*****************************************************************************
 * Default memory access functions
 *****************************************************************************/

int cm3ice_mem_read(cm3ice_ctrl_t * ctrl, ice_mem_ref_t * addr, 
					 void * p, ice_size_t len)
{
	return jtag_mem_ap_read(ctrl->tap, addr->base + addr->offs, p, len);
}

int cm3ice_mem_write(cm3ice_ctrl_t * ctrl, ice_mem_ref_t * addr, 
					 const void * p, ice_size_t len)
{
	return jtag_mem_ap_write(ctrl->tap, addr->base + addr->offs, p, len);
}

int cm3ice_mem_zero(cm3ice_ctrl_t * ctrl, 
					ice_mem_ref_t * addr, ice_size_t len)
{
	return jtag_mem_ap_set(ctrl->tap, addr->base + addr->offs, 0, len);
}

/* initialize the ice driver */
int cm3ice_open(cm3ice_ctrl_t * ctrl)
{
	if (ctrl->flags & CM3ICE_OPENED) {
		DCC_LOG(LOG_WARNING, "already open!");
		return ICE_ERROR;
	}

	/* initialize the control structure */
	ctrl->tap = NULL;
	ctrl->flags = CM3ICE_OPENED;

	/* core state cache */
	ctrl->core.cache_bmp = 0; /* cache bitmap */
	ctrl->core.wback_bmp = 0; /* write back bitmap */

	ctrl->polling = false;
	ctrl->comm_addr = 0;

	ctrl->mod_id = module_register(&cm3ice_module, ctrl);

	DCC_LOG(LOG_TRACE, "[OPEN]");

	return ICE_OK;
}

int cm3ice_close(cm3ice_ctrl_t * ctrl)
{
	if (!(ctrl->flags & CM3ICE_OPENED)) {
		DCC_LOG(LOG_WARNING, "not open!");
		return ICE_ERROR;
	}

	DCC_LOG(LOG_TRACE, "[CLOSE]");
	module_unregister(ctrl->mod_id); 
	ctrl->mod_id = -1;
	ctrl->flags = 0;

	return ICE_OK;
}

int cm3ice_info(cm3ice_ctrl_t * ctrl, FILE * f, uint32_t which);
int cm3ice_test(cm3ice_ctrl_t * ctrl, FILE * f, uint32_t req, 
				uint32_t argc, uint32_t argv[]);

const struct ice_oper cm3ice_oper = {

	.open = (ice_open_t)cm3ice_open,
	.close = (ice_close_t)cm3ice_close,

	.configure = (ice_configure_t)cm3ice_configure,
	.status = (ice_status_t)cm3ice_status,
	.poll = (ice_poll_t)cm3ice_poll,
	.signal = (ice_signal_t)cm3ice_signal,

	.connect = (ice_connect_t)cm3ice_connect,	
	.release = (ice_release_t)cm3ice_release,	

	.halt = (ice_halt_t)cm3ice_halt,
	.halt_wait = (ice_halt_wait_t)NULL,
    .on_break = (ice_on_break_t)NULL,

	.go_to = (ice_go_to_t)cm3ice_goto,
	.run = (ice_run_t)cm3ice_run,
	.step = (ice_step_t)cm3ice_step,
	.exec = (ice_exec_t)NULL,

	.bp_set = (ice_bp_set_t)cm3ice_bp_set,
	.bp_clr = (ice_bp_clr_t)cm3ice_bp_clr,

	.wp_set = (ice_wp_set_t)cm3ice_wp_set,
	.wp_clr = (ice_wp_clr_t)cm3ice_wp_clr,

	.reg_get = (ice_reg_get_t)cm3ice_reg_get,
	.reg_set = (ice_reg_set_t)cm3ice_reg_set,

	.ifa_get = (ice_ifa_get_t)cm3ice_ifa_get,
	.ifa_set = (ice_ifa_set_t)cm3ice_ifa_set,

	.mem_lock = (ice_mem_lock_t)NULL,
	.mem_unlock = (ice_mem_unlock_t)NULL,

	.rd8 = (ice_rd8_t)cm3ice_rd8,
	.wr8 = (ice_wr8_t)cm3ice_wr8,

	.rd16 = (ice_rd16_t)cm3ice_rd16,
	.wr16 = (ice_wr16_t)cm3ice_wr16,

	.rd32 = (ice_rd32_t)cm3ice_rd32,
	.wr32 = (ice_wr32_t)cm3ice_wr32,

	.fifo_rd8 = (ice_fifo_rd8_t)NULL,
	.fifo_wr8 = (ice_fifo_wr8_t)NULL,

	.test = (ice_test_t)cm3ice_test,
	.info = (ice_info_t)cm3ice_info,

	.core_reset = (ice_core_reset_t)cm3ice_core_reset,

	.system_reset = (ice_system_reset_t)cm3ice_system_reset,

	.context_show = (ice_context_show_t)cm3ice_context_show,
	.print_insn = (ice_print_insn_t)cm3_print_insn
};


const struct ice_drv_info cm3ice_drv = {
	.name = "cm3ice",
	.version = "0.3",
	.vendor = "YARD-ICE",
	.oper = (void *)&cm3ice_oper
};

