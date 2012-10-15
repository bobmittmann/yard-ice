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
#include "armice.h"
#include "jtag_arm.h"
#include "ice_drv.h"

#include "dbglog.h"

extern struct armice_codelet codelet_fifo_write;
extern struct armice_codelet codelet_fifo_read;

int armice_fifo_get(armice_ctrl_t * ctrl, uint32_t addr, void * buf, int len)
{
	uint32_t exec_addr = ctrl->work_addr;
	uint32_t data;
	uint8_t * cp;
	uint32_t reg[2];
	int ret;
	int n;
	int m;

	context_update(ctrl, ct);

	if ((ret = armice_code_load(ctrl, exec_addr, 
								codelet_fifo_write.code, 
								codelet_fifo_write.size)) < 0) { 
		ctrl->ice_state = ICE_OUTOFSYNC;
		DCC_LOG(LOG_ERROR, "armice_code_load()");
		return ret;
	}

	reg[0] = addr;
	reg[1] = addr;

	DCC_LOG(LOG_INFO, "code exec...");

	if ((ret = armice_code_exec(ctrl, exec_addr, reg, 1)) < 0) { 
		DCC_LOG(LOG_ERROR, "arm7ice_code_load()");
		ctrl->ice_state = ICE_OUTOFSYNC;
		return ret;
	}

	cp = (uint8_t *)buf;

	m = (len + (sizeof(uint32_t) - 1)) & ~(sizeof(uint32_t) - 1);

	DCC_LOG1(LOG_INFO, "m=%d", m);

	for (n = 0; n < m; n += 4) {
		data = cp[0] + (cp[1] << 8) + (cp[2] << 16) + (cp[3] << 24);
		DCC_LOG1(LOG_INFO, "%08x", data);
		/* TODO: adjust the timeout parameter according to the
		   processor, memory & jtag speeds... */
		if ((ret = jtag_arm_dcc_out(ctrl->tap, data, 100)) != JTAG_OK) {
			DCC_LOG2(LOG_WARNING, "jtag_arm_dcc_out(): %d out of %d bytes", 
					 n, m);
			break;
		}
		cp += 4;
	}

	armice_code_stop(ctrl);

	DCC_LOG2(LOG_INFO, "n=%d len=%d", n, len);

	return ICE_OK;
}


int armice_fifo_put(armice_ctrl_t * ctrl, uint32_t addr, 
					const void * buf, int len)
{
	uint32_t exec_addr = ctrl->work_addr;
	uint32_t data;
	uint8_t * cp;
	uint32_t reg[2];
	int ret;
	int n;
	int m;

	context_update(ctrl, ct);

	if ((ret = armice_code_load(ctrl, exec_addr, 
								codelet_fifo_write.code, 
								codelet_fifo_write.size)) < 0) { 
		ctrl->ice_state = ICE_OUTOFSYNC;
		DCC_LOG(LOG_ERROR, "armice_code_load()");
		return ret;
	}

	reg[0] = addr;
	reg[1] = addr;

	DCC_LOG(LOG_INFO, "code exec...");

	if ((ret = armice_code_exec(ctrl, exec_addr, reg, 1)) < 0) { 
		DCC_LOG(LOG_ERROR, "arm7ice_code_load()");
		ctrl->ice_state = ICE_OUTOFSYNC;
		return ret;
	}

	cp = (uint8_t *)buf;
	m = (len + (sizeof(uint32_t) - 1)) & ~(sizeof(uint32_t) - 1);

	DCC_LOG1(LOG_INFO, "m=%d", m);

	for (n = 0; n < m; n += 4) {
		if ((ret = jtag_arm_dcc_in(ctrl->tap, &data, 100)) != JTAG_OK) {
			DCC_LOG1(LOG_WARNING, "jtag_arm_dcc_in(): %d", ret);
			return -1;
		}
		*cp++ = data;
		*cp++ = data >> 8;
		*cp++ = data >> 16;
		*cp++ = data >> 24;
	}

	armice_code_stop(ctrl);

	DCC_LOG2(LOG_INFO, "n=%d len=%d", n, len);

	return ICE_OK;
}

