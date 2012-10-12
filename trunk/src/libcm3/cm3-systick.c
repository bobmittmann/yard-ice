/* 
 * File:	cm3-systick.c
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

#ifndef CM3_SYSTICK_CLK_HZ
#error "CM3_SYSTICK_CLK_HZ undefined"
#endif

uint32_t cm3_get_ticks(void)
{
	return 0xffffffff - (CM3_SYSTICK->val << 8);
}

void cm3_udelay_calibrate(void)
{
	struct cm3_systick * systick = CM3_SYSTICK;
	uint32_t ticks1ms;

	systick->load = 0xffffff;
	systick->ctrl = SYSTICK_CTRL_ENABLE;
	ticks1ms = ((CM3_SYSTICK_CLK_HZ / 1000) << 8) - 1;

	udelay_calibrate(ticks1ms, cm3_get_ticks);
}

