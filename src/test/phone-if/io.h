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

#ifdef __cplusplus
extern "C" {
#endif

/* ----------------------------------------------------------------------
 * LEDs 
 * ----------------------------------------------------------------------
 */

void leds_init(void);
 
void leds_all_on(void);

void leds_all_off(void);

void led_off(int id);

void led_on(int id);

/* ----------------------------------------------------------------------
 * Relays 
 * ----------------------------------------------------------------------
 */
void relays_init(void);

void relay_on(int id);

void relay_off(int id);

/* ----------------------------------------------------------------------
 * CODEC I/O 
 * ----------------------------------------------------------------------
 */
void codec_io_init(void);

void codec_rst_lo(void);

void codec_rst_hi(void);

#ifdef __cplusplus
}
#endif	

#endif /* __IO_H__ */
