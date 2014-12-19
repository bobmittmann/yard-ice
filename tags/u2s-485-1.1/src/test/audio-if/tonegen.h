/* 
 * File:	 tonegen.h
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2013 Bob Mittmann. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef __TONEGEN_H__
#define __TONEGEN_H__

#include <stdint.h>
#include "sndbuf.h"
#include "fixpt.h"

struct tonegen {
	volatile int locked;
	uint32_t len;
	uint32_t pos;
	int16_t wave[256];
};

extern const unsigned int wave_max;
extern const unsigned int dac_gain_max;

#ifdef __cplusplus
extern "C" {
#endif

int tonegen_init(struct tonegen * gen, int32_t amp, int tone);

void tonegen_apply(struct tonegen * gen, sndbuf_t * buf);

void blank_apply(int16_t frm[]);

void ramp_apply(int16_t frm[]);

void pulse_apply(int16_t frm[]);

void pattern_apply(int16_t frm[]);

#ifdef __cplusplus
}
#endif	

#endif /* __TONEGEN_H__ */

