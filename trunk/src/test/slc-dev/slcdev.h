/* 
 * File:	 slcdev.h
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

#ifndef __SLCDEV_H__
#define __SLCDEV_H__

#include "board.h"

enum {
	TRIG_MODE_VSLC = 0,
	TRIG_MODE_BIT,
	TRIG_MODE_PW,
	TRIG_MODE_MATCH
};

enum {
	SLC_EV_NONE = 0,
	SLC_EV_TRIG,
	SLC_EV_SIM,
};

#ifdef __cplusplus
extern "C" {
#endif

static inline void trig_out_clr(void) {
	stm32_gpio_clr(TRIG_OUT);
}

static inline void trig_out_set(void) {
	stm32_gpio_set(TRIG_OUT);
}

void trig_addr_set(unsigned int addr);

void trig_mode_set(unsigned int mode);

unsigned int trig_addr_get(void);

void slcdev_init(void);

uint32_t slcdev_event_wait(void);

void dev_sim_enable(unsigned int addr);

void dev_sim_disable(unsigned int addr);

#ifdef __cplusplus
}
#endif

#endif /* __SLCDEV_H__ */

