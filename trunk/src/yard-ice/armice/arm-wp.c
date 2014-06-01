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
 * @file arm-wp.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifdef CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <sys/dcclog.h>

#include "armice.h"
#include "jtag_arm.h"

int arm_wp_brk_req(jtag_tap_t * tap)
{
	DCC_LOG(LOG_TRACE, ".");

	jtag_arm_ice_rt_wr(tap, ARMICE_WP0_ADDR_MASK, 0xffffffff);
	jtag_arm_ice_rt_wr(tap, ARMICE_WP0_DATA_MASK, 0xffffffff);
	jtag_arm_ice_rt_wr(tap, ARMICE_WP0_CTRL_MASK, ~ARMICE_WP_NOPC & 0xff);
	return jtag_arm_ice_rt_wr(tap, ARMICE_WP0_CTRL_VALUE, ARMICE_WP_ENABLE);
}

int arm_wp_clr(jtag_tap_t * tap, int id)
{
	DCC_LOG1(LOG_TRACE, "id=%d", id);

	if (id == 0)
		return jtag_arm_ice_rt_wr(tap, ARMICE_WP0_CTRL_VALUE, 0);

	if (id == 1)
		return jtag_arm_ice_rt_wr(tap, ARMICE_WP1_CTRL_VALUE, 0);

	return -1;
}

int arm_wp_set(jtag_tap_t * tap, int id, jtag_arm_wp_t * wp)
{
	DCC_LOG5(LOG_TRACE, "id=%d addr=<0x%08x, 0x%08x> data=<0x%08x, 0x%08x>", 
			 id, wp->addr_value, wp->addr_mask, wp->data_value, wp->data_mask);

	if (id == 0) {
		/* configure the break point */
		jtag_arm_ice_rt_wr(tap, ARMICE_WP0_ADDR_MASK, wp->addr_mask);
		jtag_arm_ice_rt_wr(tap, ARMICE_WP0_ADDR_VALUE, wp->addr_value);
		jtag_arm_ice_rt_wr(tap, ARMICE_WP0_DATA_MASK, wp->data_mask);
		jtag_arm_ice_rt_wr(tap, ARMICE_WP0_DATA_VALUE, wp->data_value);
		jtag_arm_ice_rt_wr(tap, ARMICE_WP0_CTRL_MASK, wp->ctrl_mask);
		return jtag_arm_ice_rt_wr(tap, ARMICE_WP0_CTRL_VALUE, wp->ctrl_value);
	}

	if (id == 1) {
		/* configure the break point */
		jtag_arm_ice_rt_wr(tap, ARMICE_WP1_ADDR_MASK, wp->addr_mask);
		jtag_arm_ice_rt_wr(tap, ARMICE_WP1_ADDR_VALUE, wp->addr_value);
		jtag_arm_ice_rt_wr(tap, ARMICE_WP1_DATA_MASK, wp->data_mask);
		jtag_arm_ice_rt_wr(tap, ARMICE_WP1_DATA_VALUE, wp->data_value);
		jtag_arm_ice_rt_wr(tap, ARMICE_WP1_CTRL_MASK, wp->ctrl_mask);
		return jtag_arm_ice_rt_wr(tap, ARMICE_WP1_CTRL_VALUE, wp->ctrl_value);
	}

	return -1;
}

int arm_sstp_init(jtag_tap_t * tap)
{
	int ret;

	DCC_LOG(LOG_TRACE, ".");

	jtag_arm_ice_rt_wr(tap, ARMICE_WP0_ADDR_MASK, 0xffffffff);
	jtag_arm_ice_rt_wr(tap, ARMICE_WP1_ADDR_MASK, 0x00000000);
	jtag_arm_ice_rt_wr(tap, ARMICE_WP1_DATA_MASK, 0xffffffff);
	jtag_arm_ice_rt_wr(tap, ARMICE_WP1_CTRL_VALUE, 0x00000000);
	ret =  jtag_arm_ice_rt_wr(tap, ARMICE_WP1_CTRL_MASK, 
							  ~ARMICE_WP_NOPC & 0xff);

	return ret;
}

int arm_sstp_disable(jtag_tap_t * tap)
{
	DCC_LOG(LOG_TRACE, ".");

	return jtag_arm_ice_rt_wr(tap, ARMICE_WP0_CTRL_MASK, 
							  ~ARMICE_WP_NOPC & 0xff);
}


#if 0
int arm_sstp_enable(jtag_tap_t * tap)
{
	int ret;

	DCC_LOG(LOG_INFO, ".");

	ret = jtag_arm_ice_rt_wr(tap, ARMICE_WP0_CTRL_MASK, 
							  ~(ARMICE_WP_RANGE | ARMICE_WP_NOPC) & 0xff);

	return ret;
}
#endif

/* set breakpoint and enable single step */
int arm_sstp_bp_set(jtag_tap_t * tap, uint32_t addr)
{
	DCC_LOG1(LOG_TRACE, "addr=0x%08x", addr);

	jtag_arm_ice_rt_wr(tap, ARMICE_WP1_ADDR_VALUE, addr); 

	return jtag_arm_ice_rt_wr(tap, ARMICE_WP0_CTRL_MASK, 
							  ~(ARMICE_WP_RANGE | ARMICE_WP_NOPC) & 0xff);
}

