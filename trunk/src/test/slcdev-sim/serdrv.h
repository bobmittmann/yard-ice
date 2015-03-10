/* 
 * File:	 serdev.h
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

#ifndef __SERDRV_H__
#define __SERDRV_H__

struct serdrv;

#ifdef __cplusplus
extern "C" {
#endif

struct serdrv * serdrv_init(unsigned int speed);

int serdrv_send(struct serdrv * drv, const void * buf, int len);

int serdrv_recv(struct serdrv * drv, void * buf, int len, unsigned int tmo);

void serdrv_flush(struct serdrv * dev);

FILE * serdrv_tty_fopen(struct serdrv * drv);

#ifdef __cplusplus
}
#endif

#endif /* __SERDRV_H__ */


