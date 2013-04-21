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
 * @file jtag_tap_api.c
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

#include <sys/dcclog.h>

#ifndef JTAG_VEC_BITS_MAX 
#error "JTAG_VEC_BITS_MAX undefined"
#endif

#ifndef JTAG_TAP_MAX 
#error "JTAG_TAP_MAX undefined"
#endif

/***************************************************************************
 *
 * JTAG - TAP management and operations
 *
 ***************************************************************************/

static struct {
	unsigned int tap_cnt;
	int tap_sel;
	struct jtag_tap tap[JTAG_TAP_MAX];
} jtag;

int jtag_tap_purge(void)
{	
	jtag_tap_t * tap;
	int i;

	DCC_LOG1(LOG_TRACE, "purging %d TAPs", jtag.tap_cnt);
	for (i = 0; i < jtag.tap_cnt; i++) {
		tap = &jtag.tap[i];
		(void)tap;
	}

	jtag.tap_cnt = 0;
	jtag.tap_sel = 0;

	return JTAG_OK;
}

int jtag_tap_insert(jtag_tap_t ** tap_ptr, unsigned int chain_pos, 
					unsigned int ir_len, uint32_t id_code)
{
	jtag_tap_t * tap;
	int preirlen;
	int posirlen;
	int i;

	if (jtag.tap_cnt == JTAG_TAP_MAX) {
		DCC_LOG(LOG_WARNING, "too many TAPs!");
		return JTAG_ERR_TOO_MANY_TAPS;
	}

	if (chain_pos > jtag.tap_cnt) {
		DCC_LOG(LOG_WARNING, "invalid TAP position!");
		return JTAG_ERR_INVALID_TAP;
	}

	if (chain_pos < jtag.tap_cnt) {
		/* move the existing taps to the end */
		for (i = jtag.tap_cnt; i > chain_pos; i--) {
			jtag.tap[i] = jtag.tap[i - 1];
		}
	}

	/* update the TAP counter */
	jtag.tap_cnt++;

	/* get a pointer to the TAP sctructure */
	tap = &jtag.tap[chain_pos];
	if (tap_ptr != NULL)
		*tap_ptr = tap;

	/* fill in the TAP fields */
	tap->pos = chain_pos;

	tap->irlen = ir_len;
	tap->irmask = ~(0xffffffff << ir_len);

	tap->idcode = id_code;
	tap->idmask = 0;
	tap->idcomp = 0;

	/* update all pre ir lengths */
	/* update all pre counts (taps before)... */
	preirlen = 0;

	for (i = 0; i < jtag.tap_cnt; i++) {
		jtag.tap[i].precnt = i;
		jtag.tap[i].preirlen = preirlen;
		preirlen += jtag.tap[i].irlen;
	}

	/* how many taps after... */
	/* update all pos ir lengths */
	tap->precnt = tap->pos;
	tap->poscnt = jtag.tap_cnt - (tap->pos + 1);

	posirlen = 0;
	for (i = (jtag.tap_cnt - 1); i >= 0; i--) {
		jtag.tap[i].poscnt = (jtag.tap_cnt - 1) - i;
		jtag.tap[i].posirlen = posirlen;
		posirlen += jtag.tap[i].irlen;
	}

	DCC_LOG5(LOG_TRACE, "TAP=%p, pos=%d irlen=%d irmask=0x%08x idcode=0x%08x", 
			 tap, tap->pos, tap->irlen, tap->irmask, tap->idcode);

	return JTAG_OK;
}

int jtag_tap_add(jtag_tap_t ** tap_ptr, unsigned int ir_len, uint32_t id_code)
{
	return jtag_tap_insert(tap_ptr, jtag.tap_cnt, ir_len, id_code);
}

int jtag_tap_get(jtag_tap_t ** tap_ptr, unsigned int chain_pos)
{
	if (chain_pos >= jtag.tap_cnt) {
		DCC_LOG2(LOG_INFO, "chain_pos(%d) >= jtag.tap_cnt(%d)", 
				 chain_pos, jtag.tap_cnt);
		return JTAG_ERR_INVALID_TAP;
	}

	if (tap_ptr != NULL) {
		*tap_ptr = (jtag_tap_t *)&jtag.tap[chain_pos];
		DCC_LOG1(LOG_INFO, "TAP: %p", *tap_ptr);
	}

	return JTAG_OK;
}

int jtag_tap_tell(void)
{
	return jtag.tap_cnt;
}

