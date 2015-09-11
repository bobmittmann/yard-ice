/* 
 * File:	 led.h
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(c) 2003-2006 BORESTE (www.boreste.com). All Rights Reserved.
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

#ifndef __ISINK_H__
#define __ISINK_H__

#define ISINK_CURRENT_LOW     0
#define ISINK_CURRENT_NOM     2
#define ISINK_CURRENT_HIGH    3
#define ISINK_CURRENT_DOUBLE  16

#define ISINK_RATE_VERY_SLOW (0 << 5)
#define ISINK_RATE_SLOW      (1 << 5)
#define ISINK_RATE_NORMAL    (2 << 5)
#define ISINK_RATE_FAST      (3 << 5)

#ifdef __cplusplus
extern "C" {
#endif

void isink_start(unsigned int mode, unsigned int pre, unsigned int pulse);

void isink_stop(void);

void isink_init(void);

void isink_sleep(void);

/* Adjust the current sink slew rate (mA/us) 
   MIN: 110 mA/us
   MAX: 1100 mA/us 
 */

#define SLEWRATE_MIN 100
#define SLEWRATE_MAX 2500

void isink_slewrate_set(unsigned int rate);

void isink_pulse(unsigned int pre, unsigned int pulse);

void isink_mode_set(unsigned int mode);

#ifdef __cplusplus
}
#endif

#endif /* __ISINK_H__ */


