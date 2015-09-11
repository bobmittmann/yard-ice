/* 
 * File:	 profclk.h
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

#ifndef __PROFCLK_H__
#define __PROFCLK_H__

#include <arch/cortex-m3.h>

static inline uint32_t profclk_get(void) {
	return CM3_DWT->cyccnt;
}

#ifdef __cplusplus
extern "C" {
#endif

void profclk_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __PROFCLK_H__ */

