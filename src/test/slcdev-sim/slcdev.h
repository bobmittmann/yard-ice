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
#include <stdio.h>

/***************************************************************************
  Device  Database
 ***************************************************************************/

#define SLCDEV_DESC_LEN_MAX 64

struct pw_entry {
	uint16_t min;
	uint16_t max;
	char desc[SLCDEV_DESC_LEN_MAX]; /* The description string */
};

struct ic_entry {
	uint8_t mode;
	uint8_t flags;
	char desc[SLCDEV_DESC_LEN_MAX]; /* The description string */
};

#define SLCDEV_PW_LIST_LEN_MAX 24

struct pw_list {
	uint16_t cnt;
	struct pw_entry * pw[SLCDEV_PW_LIST_LEN_MAX];
};

struct db_obj {
	uint8_t len;
	uint8_t type;
	uint8_t id;
	uint8_t flags;
	struct db_obj * next;
};

struct obj_module {
	uint8_t len;
	uint8_t type;
	uint8_t id;
	uint8_t flags;
	struct db_obj * next;
	const char * tag;	
	const char * desc;	
	struct pw_list * pw1;
	struct pw_list * pw2;
	struct pw_list * pw3;
	struct pw_list * pw4;
	struct pw_list * pw5;
	struct ic_entry * ic1;
	struct ic_entry * ic2;
	struct ic_entry * ic3;
};

struct obj_sensor {
	uint8_t len;
	uint8_t type;
	uint8_t id;
	uint8_t flags;
	struct db_obj * next;
	const char * tag;	
	const char * desc;	
	struct pw_list * pw1;
	struct pw_list * pw2;
	struct pw_list * pw3;
	struct pw_list * pw4;
	struct pw_list * pw5;
};

struct obj_db_info {
	uint8_t len;
	uint8_t type;
	uint8_t id;
	uint8_t flags;
	struct db_obj * next;
	uint16_t json_crc;
	uint16_t json_len;
	struct db_obj * obj[];
};


enum {
	DB_OBJ_DB_INFO = 0,
	DB_OBJ_SENSOR,
	DB_OBJ_MODULE
};

/***************************************************************************
  Events
 ***************************************************************************/

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

int device_db_init(void);
int device_db_erase(void);
int device_db_compile(void);
int device_db_dump(FILE * f);

#ifdef __cplusplus
}
#endif

#endif /* __SLCDEV_H__ */

