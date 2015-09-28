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

#ifndef __IO_H__
#define __IO_H__

#ifdef __cplusplus
extern "C" {
#endif

void io_init(void);
void __attribute__((noreturn)) io_event_task(void);
void io_lamp_test(void);

int32_t io_time(void);

void isink_start(unsigned int mode, unsigned int pre, unsigned int pulse);
void isink_stop(void);

void irate_set(unsigned int mv);

void led_on(unsigned int id);

void led_off(unsigned int id);

bool led_status(unsigned int id);

void led_flash(unsigned int id, unsigned int ms);

unsigned int timer_get(unsigned int id);

void timer_set(unsigned int id, unsigned int ms);

#ifdef __cplusplus
}
#endif

#endif /* __IO_H__ */

