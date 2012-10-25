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

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "jtag_arm.h"
#include "armice.h"
#include "arm_pn.h"
#include "dbglog.h"

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

int armice_info(armice_ctrl_t * ctrl, FILE * f, uint32_t which)
{
	int status;
	uint32_t dbg_ctrl;
	uint32_t data_mask;
	uint32_t data_value;
	uint32_t addr_mask;
	uint32_t addr_value;
	uint32_t ctrl_mask;
	uint32_t ctrl_value;

	jtag_tap_t * tap = ctrl->tap;

	DCC_LOG1(LOG_TRACE, "which=%d", which);

	fprintf(f, "== ARM ICE ==\n");

	fprintf(f, " - WP0: %s\n", (ctrl->flags & ARMICE_WP0_SET) ? 
			"enabled" : "disabled");
	jtag_arm_ice_rt_rd(tap, ARMICE_WP0_ADDR_MASK, &addr_mask);
	jtag_arm_ice_rt_rd(tap, ARMICE_WP0_ADDR_VALUE, &addr_value);
	jtag_arm_ice_rt_rd(tap, ARMICE_WP0_DATA_MASK, &data_mask);
	jtag_arm_ice_rt_rd(tap, ARMICE_WP0_DATA_VALUE, &data_value);
	jtag_arm_ice_rt_rd(tap, ARMICE_WP0_CTRL_MASK, &ctrl_mask);
	jtag_arm_ice_rt_rd(tap, ARMICE_WP0_CTRL_VALUE, &ctrl_value);

	fprintf(f, "   - addr=<0x%08x 0x%08x>\n", addr_value, addr_mask);
	fprintf(f, "   - data=<0x%08x 0x%08x>\n", data_value, data_mask);
	fprintf(f, "   - ctlr=<0x%04x 0x%04x>\n", ctrl_value, ctrl_mask); 


	fprintf(f, " - WP1: %s\n", (ctrl->flags & ARMICE_WP1_SET) ? 
			"enabled" : "disabled");
	jtag_arm_ice_rt_rd(tap, ARMICE_WP1_ADDR_MASK, &addr_mask);
	jtag_arm_ice_rt_rd(tap, ARMICE_WP1_ADDR_VALUE, &addr_value);
	jtag_arm_ice_rt_rd(tap, ARMICE_WP1_DATA_MASK, &data_mask);
	jtag_arm_ice_rt_rd(tap, ARMICE_WP1_DATA_VALUE, &data_value);
	jtag_arm_ice_rt_rd(tap, ARMICE_WP1_CTRL_MASK, &ctrl_mask);
	jtag_arm_ice_rt_rd(tap, ARMICE_WP1_CTRL_VALUE, &ctrl_value);

	fprintf(f, "   - addr=<0x%08x 0x%08x>\n", addr_value, addr_mask);
	fprintf(f, "   - data=<0x%08x 0x%08x>\n", data_value, data_mask);
	fprintf(f, "   - ctlr=<0x%04x 0x%04x>\n", ctrl_value, ctrl_mask); 


	if ((status = ctrl->jtag_arm_dbg_status(ctrl->tap)) < 0) {
		return status;
	}

	fprintf(f, " - DBG status: ");

	if (ctrl->pn.family == 9) {
		fprintf(f, "%s IJBIT:%d ITBIT:%d SYSCOMP:%d "
				"IFEN:%d DBGRQ:%d DBGACK:%d\n", 
				entry_mode[ARMICE_ST_MOE(status)],
				(status & ARMICE_ST_IJBIT) ? 1 : 0,
				(status & ARMICE_ST_ITBIT) ? 1 : 0,
				(status & ARMICE_ST_SYSCOMP) ? 1 : 0,
				(status & ARMICE_ST_IFEN) ? 1 : 0,
				(status & ARMICE_ST_DBGRQ) ? 1 : 0,
				(status & ARMICE_ST_DBGACK) ? 1 : 0);

	} else {
		fprintf(f, " THUMB:%d nMREQ:%d IFEN:%d DBGRQ:%d DBGACK:%d\n", 
				(status & ARMICE_ST_TBIT) ? 1 : 0,
				(status & ARMICE_ST_NMREQ) ? 1 : 0,
				(status & ARMICE_ST_IFEN) ? 1 : 0,
				(status & ARMICE_ST_DBGRQ) ? 1 : 0,
				(status & ARMICE_ST_DBGACK) ? 1 : 0);
	}

	fprintf(f, " - DBG ctrl: ");

	jtag_arm_dbg_ctrl_rd(ctrl->tap, &dbg_ctrl);
	if (ctrl->pn.family == 9) {
		fprintf(f, "ICE:%d MON:%d SSTP:%d INTDIS:%d DBGRQ:%d DBGACK:%d\n", 
				(dbg_ctrl & ARMICE_CTRL_ICE_DISABLE) ? 1 : 0,
				(dbg_ctrl & ARMICE_CTRL_MONITOR_EN) ? 1 : 0,
				(dbg_ctrl & ARMICE_CTRL_SINGLE_STEP) ? 1 : 0,
				(dbg_ctrl & ARMICE_CTRL_INTDIS) ? 1 : 0,
				(dbg_ctrl & ARMICE_CTRL_DBGRQ) ? 1 : 0,
				(dbg_ctrl & ARMICE_CTRL_DBGACK) ? 1 : 0);
	} else {
		fprintf(f, "INTDIS:%d DBGRQ:%d DBGACK:%d\n", 
				(dbg_ctrl & ARMICE_CTRL_INTDIS) ? 1 : 0,
				(dbg_ctrl & ARMICE_CTRL_DBGRQ) ? 1 : 0,
				(dbg_ctrl & ARMICE_CTRL_DBGACK) ? 1 : 0);
	}


	return 0;
}

