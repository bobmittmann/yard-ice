/* 
 * File:	 lcd20x4.h
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

#ifndef __LCD20X4_H__
#define __LCD20X4_H__

#include <stdbool.h>

struct lcd_dev;

#ifdef __cplusplus
extern "C" {
#endif

struct lcd_dev * lcd20x4_init(void);

void lcd_puts(struct lcd_dev * dev, const char * s);

void lcd_putc(struct lcd_dev * dev, int c);

void lcd_at_puts(struct lcd_dev * dev, unsigned int lin,
		unsigned int col, const char * s);

void lcd_clear(struct lcd_dev * dev);

void lcd_on(struct lcd_dev * dev);

void lcd_off(struct lcd_dev * dev);

void lcd_set_pos(struct lcd_dev * dev, unsigned int lin, unsigned int col);

void lcd_cursor_on(struct lcd_dev * dev, bool blink);

void lcd_cursor_off(struct lcd_dev * dev);

#ifdef __cplusplus
}
#endif

#endif /* __LCD20X4_H__ */

