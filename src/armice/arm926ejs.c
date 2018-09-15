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
 * @file arm926ejs.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 


#include <stdlib.h>
#include <string.h>

#include "jtag.h"
#include "jtag_arm.h"
#include "armice.h"
#include "arm_opc.h"

#include "dbglog.h"

void arm926_context_save(jtag_tap_t * tap, unsigned int flags, 
						 armice_context_t * ct)
{
	uint32_t data;
	uint32_t cp15_ctrl;

	DCC_LOG(LOG_TRACE, ".");
	arm9_context_save(tap, flags, ct);

	jtag_arm926_cp15_rd(tap, CP15_C1_CONTROL, &cp15_ctrl);

	if (cp15_ctrl & C1_MMU) {
		/* invalidate TLB */
		DCC_LOG(LOG_INFO, "Invalidate TLB...");
		jtag_arm926_cp15_wr(tap, CP15_ADDR(0, 0, 8, 7), 0x00000000);
		cp15_ctrl &= ~C1_MMU;
	}

	if (cp15_ctrl & C1_DCACHE) {
		uint32_t dbg_ovr;
		/* read-modify-write CP15 debug override register
		 * to enable "test and clean all" */
		jtag_arm926_cp15_rd(tap, CP15_C15_DBG_OVR, &dbg_ovr);
		jtag_arm926_cp15_wr(tap, CP15_C15_DBG_OVR, dbg_ovr | 0x80000);

		DCC_LOG(LOG_INFO, "Invalidate DCache...");
		jtag_arm926_cp15_wr(tap, CP15_ADDR(0, 0, 7, 5), 0x00000000);

		/* write CP15 debug override register
		 * to disable "test and clean all" */
		jtag_arm926_cp15_wr(tap, CP15_C15_DBG_OVR, dbg_ovr);

		cp15_ctrl &= ~C1_DCACHE;
	}

	if (cp15_ctrl & C1_ICACHE) {
		DCC_LOG(LOG_INFO, "Invalidate ICache...");
		jtag_arm926_cp15_wr(tap, CP15_ADDR(0, 0, 7, 5), 0x00000000);
		cp15_ctrl &= ~C1_ICACHE;
	}

	jtag_arm926_cp15_wr(tap, CP15_C1_CONTROL, cp15_ctrl);

	jtag_arm926_cp15_rd(tap, CP15_ADDR(7, 0, 15, 0), &data);
	data |= 0x7;
	jtag_arm926_cp15_wr(tap, CP15_ADDR(7, 0, 15, 0), data);
}

void arm926_context_restore(jtag_tap_t * tap, armice_context_t * ct)
{
//	uint32_t data;

	DCC_LOG(LOG_TRACE, ".");

#if 0
    /* restore i/d fault status and address register */
	arm926ejs->write_cp15(target, 0, 0, 5, 0, arm926ejs->d_fsr);
	arm926ejs->write_cp15(target, 0, 1, 5, 0, arm926ejs->i_fsr);
	arm926ejs->write_cp15(target, 0, 0, 6, 0, arm926ejs->d_far);

	uint32_t cache_dbg_ctrl;

	arm926ejs->read_cp15(target, 7, 0, 15, 0, &cache_dbg_ctrl);
	cache_dbg_ctrl &= ~0x7;
	arm926ejs->write_cp15(target, 7, 0, 15, 0, cache_dbg_ctrl);

#endif

#if 0
	/* read-modify-write CP15 cache debug control register
	 * to reenable I/D-cache linefills and disable WT */
	jtag_arm926_cp15_rd(tap, CP15_ADDR(7, 0, 15, 0), &data);
	data &= ~0x7;
	jtag_arm926_cp15_wr(tap, CP15_ADDR(7, 0, 15, 0), data);
#endif

	arm9_context_restore(tap, ct);
}

void arm926_thumb_context_save(jtag_tap_t * tap, unsigned int flags, 
						 armice_context_t * ct)
{
	DCC_LOG(LOG_TRACE, ".");
	arm9_thumb_context_save(tap, flags, ct);
}

void arm926_thumb_context_restore(jtag_tap_t * tap, armice_context_t * ct)
{
	DCC_LOG(LOG_TRACE, ".");
	arm9_thumb_context_restore(tap, ct);
}

void arm926_on_debug_dentry(jtag_tap_t * tap, armice_context_t * ct)
{
	uint32_t data;

	DCC_LOG(LOG_TRACE, ".");

	/* CP15 */
	jtag_arm926_cp15_rd(tap, CP15_C0_IDCODE, &data);
	DCC_LOG1(LOG_TRACE, "CP15 IDCODE: %08x", data);

	jtag_arm926_cp15_rd(tap, CP15_C0_CACHE_TYPE, &data);

	DCC_LOG2(LOG_TRACE, "ICahe: %d, DCache: %d", 
			 CACHE_SIZE(C0_ISIZE(data)), CACHE_SIZE(C0_DSIZE(data)));

	jtag_arm926_cp15_rd(tap, CP15_C0_TCM_TYPE, &data);
	DCC_LOG2(LOG_TRACE, "CP15 DTCM:%d ITCM:%d", 
			 C0_TCM_DTCM(data), C0_TCM_ITCM(data));

	jtag_arm926_cp15_rd(tap, CP15_ADDR(7, 0, 15, 0), &data);
	data |= 0x7;
	jtag_arm926_cp15_wr(tap, CP15_ADDR(7, 0, 15, 0), data);

#if 0
	/* TODO: save MMU and Cache control  */
	jtag_arm926_cp15_rd(tap, CP15_ADDR(0, 0, 5, 0), &d_fsr);
	jtag_arm926_cp15_rd(tap, CP15_ADDR(0, 1, 5, 0), &i_fsr);
	jtag_arm926_cp15_rd(tap, CP15_ADDR(0, 0, 6, 0), &d_far);
#endif

}
