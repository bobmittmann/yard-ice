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
 * @file jtag_chain_init.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#ifdef CONFIG_H
#include "config.h"
#endif

#include <stdint.h>
#include <stdio.h>
#include <jtag.h>

#include <bitvec.h>
#include <sys/dcclog.h>

#ifndef JTAG_VEC_BITS_MAX 
#error "JTAG_VEC_BITS_MAX undefined"
#endif

int jtag_chain_init(uint8_t * irlen, int cnt)
{
	uint32_t vec[JTAG_VEC_BITS_MAX / 32];
	jtag_tap_t * tap;
	uint32_t idcode;
	int len;
	int ret;
	int i;

	DCC_LOG1(LOG_TRACE, "devices on chain: %d", cnt);

	/* remove all taps from the chain */
	jtag_tap_purge();

	/* reset the TAPs to put the IDCODE in the DR scan */
	jtag_drv_tap_reset(4 + cnt);

	if (cnt > (JTAG_VEC_BITS_MAX / 32)) {
		DCC_LOG(LOG_WARNING, "too many devices on the chain!");
		cnt = (JTAG_VEC_BITS_MAX / 32);
	}

	/* create an all zeros vector */
	len = cnt * 32;
	vec_zeros(vec, len);

	/* shift the vector trough the data register to get
	 the idcodes */
	if ((ret = jtag_dr_scan(vec, vec, len, JTAG_TAP_DRUPDATE)) != JTAG_OK) {
		DCC_LOG(LOG_ERROR, "jtag_dr_scan() fail!");
		return ret;
	}

	/* add the taps into the chain */
	for (i = 0; i < cnt; i++) {
		idcode = vec[i];
		if ((ret = jtag_tap_add(&tap, irlen[i], idcode)) != JTAG_OK) {
			DCC_LOG(LOG_ERROR, "jtag_tap_add() fail!");
			return ret;
		}
		DCC_LOG3(LOG_TRACE, "pos=%d tap=0x%p idcode=0x%08x", 
				 i, tap, tap->idcode);
	}

	return JTAG_OK;
}

