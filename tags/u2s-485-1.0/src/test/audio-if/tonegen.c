/* 
 * File:	 i2s-slave.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2011 Bob Mittmann. All Rights Reserved.
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


#include <sys/stm32f.h>
#include <sys/delay.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <hexdump.h>

#include "wavetab.h"
#include "trace.h"
#include "tonegen.h"
#include "sndbuf.h"

#include <sys/dcclog.h>

#define AUDIO_FRAME_LEN SNDBUF_LEN

int tonegen_init(struct tonegen * gen, int32_t amp, int tone)
{
	int32_t y;
	int i;

	if (tone > wave_max)
		return -1;


	if (wave_lut[tone].len > 256) {
		return -1;
	}

	gen->locked = 1;

	gen->len = wave_lut[tone].len;
	gen->pos = 0;

	for (i = 0; i < gen->len; ++i) {
		y = wave_lut[tone].buf[i];
		/* scale */
		y = Q15_SAT(Q15_MUL(y, amp));
		gen->wave[i] = y;
	}

	gen->locked = 0;

	return wave_lut[tone].freq;
}

void tonegen_apply(struct tonegen * gen, sndbuf_t * buf)
{
	int pos;
	int len;
	int i;

	if (gen->locked)
		return;

	pos = gen->pos;
	len = gen->len;

	for (i = 0; i < AUDIO_FRAME_LEN; ++i) {
		buf->data[i] = gen->wave[pos];
		if (++pos == len)
			pos = 0;
	}

	gen->pos = pos;
}


void blank_apply(int16_t frm[])
{
	int i;

	for (i = 0; i < AUDIO_FRAME_LEN; ++i) {
		frm[i] = 0;
	}
}

void ramp_apply(int16_t frm[])
{
	int i;
//	int d = 65536 / AUDIO_FRAME_LEN;
//	int v = -32768;

	for (i = 0; i < AUDIO_FRAME_LEN; ++i) {
		frm[i] = i;
	}
}

void pulse_apply(int16_t frm[])
{
	int i;

	for (i = 0; i < AUDIO_FRAME_LEN; ++i) {
		frm[i] = (1 << (i % 16));
	}
}

void pattern_apply(int16_t frm[])
{
	int i;

	for (i = 0; i < AUDIO_FRAME_LEN; i += 8) {
		frm[i] = 0x4000;
		frm[i + 1] = 0x4000;
		frm[i + 2] = 0x4000;
		frm[i + 3] = 0x4000;

		frm[i + 4] = 0x0000;
		frm[i + 5] = 0x0000;
		frm[i + 6] = 0x0000;
		frm[i + 7] = 0x0000;
	}
}

