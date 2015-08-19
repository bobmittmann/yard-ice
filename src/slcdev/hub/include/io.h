/* 
 * File:	 board.h
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

#ifndef __IO_H__
#define __IO_H__

#include <sys/stm32f.h>

enum rate {
	RATE_OFF = 0,
	RATE_STEADY,
	RATE_1BPS,
	RATE_2BPS,
	RATE_3BPS,
	RATE_4BPS
};

enum led {
	LED_X1 = 0,
	LED_X2
};

enum key {
	KEY_DOWN = 1,
	KEY_RIGHT = 2,
	KEY_ENTER = 3,
	KEY_UP = 4,
	KEY_LEFT = 5
};

#ifdef __cplusplus
extern "C" {
#endif

void iodrv_init(void);

void led_on(unsigned int led);

void led_off(unsigned int led);

void led_set_rate(unsigned int led, unsigned int rate);

unsigned int io_addr_get(void);

#ifdef __cplusplus
}
#endif

#endif /* __IO_H__ */

