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

#ifndef __MONITOR_H__
#define __MONITOR_H__

#define __THINKOS_DMON__
#include <thinkos_dmon.h>

#ifdef __cplusplus
extern "C" {
#endif

struct usb_cdc_class * usb_mon_init(const usb_dev_t * usb, 
									const uint8_t * const str[], 
									unsigned int strcnt);

void monitor_init(void);


#ifdef __cplusplus
}
#endif

#endif /* __MONITOR_H__ */

