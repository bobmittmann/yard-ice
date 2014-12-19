/* 
 * File:	 io.h
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

#ifndef __IO_H__
#define __IO_H__

enum {
	LED_NET = 0,
	LED_I2S,
	LED_S2,
	LED_S1
};

enum {
	EVENT_NONE,
	EVENT_CLICK,
	EVENT_DBL_CLICK,
	EVENT_HOLD1,
	EVENT_CLICK_N_HOLD,
	EVENT_HOLD2
};

#ifdef __cplusplus
extern "C" {
#endif

/* ----------------------------------------------------------------------
 * IO subsystem
 * ----------------------------------------------------------------------
 */
void io_init(void);

/* ----------------------------------------------------------------------
 * Push button 
 * ----------------------------------------------------------------------
 */

int btn_event_wait(void);

/* ----------------------------------------------------------------------
 * LEDs 
 * ----------------------------------------------------------------------
 */

void leds_all_on(void);

void leds_all_off(void);

void led_off(int id);

void led_on(int id);

void led_flash(int id, int ms);

void leds_all_flash(int ms);

void leds_lock(void);

void leds_unlock(void);

#ifdef __cplusplus
}
#endif	

#endif /* __IO_H__ */

