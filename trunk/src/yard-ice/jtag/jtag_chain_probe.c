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
 * @file jtag_chain_probe.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#ifdef CONFIG_H
#include "config.h"
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <jtag.h>

#include <bitvec.h>
#include <sys/dcclog.h>

#ifndef JTAG_VEC_BITS_MAX 
#error "JTAG_VEC_BITS_MAX undefined"
#endif

int jtag_chain_probe(uint8_t * irlen, int max, unsigned int * cnt_ptr)
{
	uint32_t vec[JTAG_VEC_BITS_MAX / 32];
	int len = JTAG_VEC_BITS_MAX;
	int ones;
	int zeros;
	int cnt;
	int ret;
	int i;
	int j;


	DCC_LOG1(LOG_TRACE, "IR probe %d bits...", JTAG_VEC_BITS_MAX);

	/* create an all ones vector */
	vec_ones(vec, JTAG_VEC_BITS_MAX);

	/* shift the vector trough the instruction register */
	if ((ret = jtag_ir_scan(vec, vec, len, JTAG_TAP_IRUPDATE)) != JTAG_OK) {
		DCC_LOG(LOG_ERROR, "jtag_ir_scan() fail!");
		return ret;
	}

	ones = 0;
	zeros = 0;
	cnt = 0;
	for (j = 0; j < (len / 32); j++) {
		for (i = 0; i < 32; i++) {
			if ((vec[j] >> i) & 1) {
				ones++;
				if (cnt == max) {
					DCC_LOG(LOG_ERROR, "too many devices!");
					return JTAG_ERR_TOO_MANY_TAPS;
				}
				if (zeros > 0) {
					irlen[cnt++] = zeros + ones;
				}
				zeros = 0;
			} else {
				zeros++;
				if (ones > 1) {
					/* 2 ones ?? */
					break;
				}
				ones = 0;
			}
		}
	}

	irlen[cnt] = 0;

	if (cnt_ptr != NULL)
		*cnt_ptr = cnt;

	return JTAG_OK;
}


