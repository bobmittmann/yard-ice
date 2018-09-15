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
 * @file armice_cfi16_2.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 


#include <stdlib.h>
#include <stdio.h>
#include "armice.h"
#include "jtag_arm.h"
#include "ice_drv.h"

#include "dbglog.h"

extern struct armice_codelet codelet_cfi16_write;
extern struct armice_codelet codelet_cfi16_erase;

int arm_cfi16_2_write(armice_ctrl_t * ctrl, const ice_mem_ref_t * addr, 
					  const void * buf, ice_size_t len)
{
	uint32_t exec_addr = ctrl->work_addr;
	uint32_t data;
	uint8_t * cp;
	uint32_t reg[2];
	uint32_t offs;
	int ret;
	int n;
	int m;

	DCC_LOG3(LOG_INFO, "%08x:%06x len=%d", addr->base, addr->offs, len);

	cp = (uint8_t *)buf;

	/* FIXME: offset not aligned */
	offs = addr->offs & ~0x000003;

	if ((ret = armice_code_load(ctrl, exec_addr, 
								codelet_cfi16_write.code, 
								codelet_cfi16_write.size)) < 0) { 

		DCC_LOG(LOG_ERROR, "arm7ice_code_load()");
		return ret;
	}

	reg[0] = addr->base;
	reg[1] = offs;

	DCC_LOG(LOG_INFO, "code exec...");

	if ((ret = armice_code_exec(ctrl, exec_addr, reg, 2)) < 0) { 
		DCC_LOG(LOG_ERROR, "arm7ice_code_load()");
		//		ctrl->ice_state = ICE_OUTOFSYNC;
		return ret;
	}

	m = (len + (sizeof(uint32_t) - 1)) & ~(sizeof(uint32_t) - 1);

	DCC_LOG1(LOG_INFO, "m=%d", m);

	for (n = 0; n < m; n += 4) {
		data = cp[0] + (cp[1] << 8) + (cp[2] << 16) + (cp[3] << 24);
		DCC_LOG1(LOG_INFO, "%08x", data);
		/* TODO: adjust the timeout parameter according to the
		   processor, memory & jtag speeds... */
		if ((ret = jtag_arm_dcc_out(ctrl->tap, data, 100)) != JTAG_OK) {
			DCC_LOG2(LOG_WARNING, "arm7ice_dcc_out(): %d out of %d bytes", 
					 n, m);
			break;
		}
		cp += 4;
	}

	armice_code_stop(ctrl);

	DCC_LOG2(LOG_INFO, "n=%d len=%d", n, len);

	return n;
}

int arm_cfi16_2_erase(armice_ctrl_t * ctrl, const ice_mem_ref_t * addr, 
					  ice_size_t len)
{
	uint32_t exec_addr = ctrl->work_addr;
	unsigned int tmo;
	uint32_t reg[3];
	uint32_t data;
	int ret;

//	printf("erasing %d bytes...\n", len);

	DCC_LOG3(LOG_INFO, "%08x:%06x len=%d", addr->base, addr->offs, len);

	/* flushes the dcc channel */
	jtag_arm_dcc_in(ctrl->tap, &data, 1);

//	printf("code size=%d\n", codelet_cfi16_erase.size);

	if ((ret = armice_code_load(ctrl, exec_addr, 
								codelet_cfi16_erase.code, 
								codelet_cfi16_erase.size)) < 0) { 
		DCC_LOG(LOG_ERROR, "arm7ice_code_load()");
		return 0;
	}


	reg[0] = addr->base;
	reg[1] = addr->offs;
	reg[2] = len;

	if ((ret = armice_code_exec(ctrl, exec_addr, reg, 3)) < 0) { 
		DCC_LOG(LOG_ERROR, "arm7ice_code_load()");
		return 0;
	}

	tmo = len < 8192 ? 10000 : len * 8;

//	printf("tmo=%d\n", tmo);

	/* wait for operation termination response in dcc channel */
	if ((ret = jtag_arm_dcc_in(ctrl->tap, &data, tmo)) != JTAG_OK) {
		DCC_LOG1(LOG_WARNING, "jtag_arm_dcc_in(): %d", ret);
		return -1;
	} else {
		DCC_LOG1(LOG_INFO, "data=%08x", data);
	}

	armice_code_stop(ctrl);

	return len;
}

