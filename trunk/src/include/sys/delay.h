/* 
 * File:    sys/delay.h
 * Author:  Robinson Mittmann (bobmittmann@gmail.com)
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

#ifndef __SYS_DELAY_H__
#define __SYS_DELAY_H__

#ifdef __cplusplus
extern "C" {
#endif

void udelay_calibrate(unsigned int ticks1ms, 
					  unsigned int (* get_ticks)(void));

void udelay(unsigned int usec);

void mdelay(unsigned int msec);

void delay(unsigned int sec);


#ifdef __cplusplus
}
#endif

#endif /* __SYS_DELAY_H__ */

