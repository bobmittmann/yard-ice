/* 
 * File:	 udelay.c
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

#include <stdint.h>
#include <stdio.h>

int32_t udelay_factor;

#if UDELAY_ERROR_ENABLE
int32_t udelay_error;
#endif

void udelay(uint32_t usec)
{
	uint32_t count;

	count = (usec * udelay_factor) >> 7;

	while (count) {
		asm("nop\n"
			"nop\n"
			"nop\n"
			"nop\n"
			"nop\n");
		count--;
	}
}

void udelay_calibrate(int32_t ticks1ms, uint32_t (* get_ticks)(void))
{
	uint32_t t0;
	uint32_t t1;
	int32_t x;
	int32_t dt;
#if UDELAY_ERROR_ENABLE
	int32_t err = 0;
#endif

	udelay_factor = 1;
	for (x = 0; x < 20; x++) {
		t0 = get_ticks();
		udelay(1000);
		t1 = get_ticks();
		dt = t1 - t0;
		if (dt > ticks1ms) {
#if UDELAY_ERROR_ENABLE
			err = ticks1ms - dt;
#endif
			x--;
			break;
		}
		udelay_factor = 1 << x;
	}

	for (; x >= 0; x--) {
		udelay_factor |= 1 << x;
		t0 = get_ticks();
		udelay(1000);
		t1 = get_ticks();
		dt = t1 - t0;
		if (dt > ticks1ms) {
			udelay_factor &= ~(1 << x);
		} else {
#if UDELAY_ERROR_ENABLE
			err = ticks1ms - dt;
#endif
		}
	}

#if UDELAY_ERROR_ENABLE
	udelay_error = (err * 1000) / ticks1ms;
#endif
}

