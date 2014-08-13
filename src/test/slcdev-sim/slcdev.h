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
	uint16_t desc;	/* The description string */
};

struct ic_entry {
	uint8_t mode;
	uint8_t flags;
	uint16_t desc; /* The description string */
};

#define SLCDEV_PW_LIST_LEN_MAX 24

struct pw_list {
	uint32_t cnt;
	struct pw_entry pw[SLCDEV_PW_LIST_LEN_MAX]; 
};

struct db_obj {
	uint8_t len;
	uint8_t type;
	uint8_t id;
	uint8_t flags;
	struct db_obj * next;
};

struct obj_device {
	uint8_t len;
	uint8_t type;
	uint8_t id;
	uint8_t flags;
	struct db_obj * next;
	uint16_t tag;	
	uint16_t desc;	
	struct pw_list * pw1; /* Reference Pulse Width */
	struct pw_list * pw2; /* Remote Test Status */
	struct pw_list * pw3; /* Manufacturer Code */
	struct pw_list * pw4; /* Analog */
	struct pw_list * pw5; /* Type Id */
};

struct obj_module {
	uint8_t len;
	uint8_t type;
	uint8_t id;
	uint8_t flags;
	struct db_obj * next;
	uint16_t tag;	
	uint16_t desc;	
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
	uint16_t tag;	
	uint16_t desc;	
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
	uint32_t obj_cnt;
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

/***************************************************************************
  Configuration
 ***************************************************************************/

struct cfg_slcdev {
	uint32_t device_type : 7;
	uint32_t advanced_protocol : 1;

	uint32_t enabled : 1;
	uint32_t poll_flash : 1;
	uint32_t isink_pulse_level : 4;
	uint32_t isink_slewrate : 2;

	uint32_t isink_width_err : 3;
	uint32_t isink_latency : 5;  /* tm = (x + 1) * 5 ( 5us .. 160us) */

	uint32_t isink_pulse_pre : 5; /* tm = (x + 1) * 5 ( 5us .. 160us) */
};

enum {
	SIM_NOP,
	SIM_ENABLE,
	SIM_WR_PW2,
	SIM_SEL_PW2,
	SIM_WR_PW3,
	SIM_SEL_PW3
};

struct sim_insn {
	uint16_t opc: 7;
	uint16_t addr: 9;
	uint16_t val;
};

#define CFG_SW_INSN_MAX 16

struct cfg_sw {
	struct sim_insn up[CFG_SW_INSN_MAX];
	struct sim_insn down[CFG_SW_INSN_MAX];
	struct sim_insn off[CFG_SW_INSN_MAX];
};

struct cfg_info {
	uint16_t json_crc;
	uint16_t json_len;
};

struct devsim_cfg {
	struct cfg_info info;

	struct cfg_slcdev sensor[160];

	struct cfg_slcdev module[160];

	struct cfg_sw sw1;

	struct cfg_sw sw2;

	struct {
		uint32_t opt;
		struct sim_insn insn[32];
	} zone[16];
};


/***************************************************************************
  Runtime
 ***************************************************************************/

/* -------------------------------------------------------------------------
 * Sysem Sensor device
 * ------------------------------------------------------------------------- */

struct ss_device {
	union {
		struct {
			uint32_t enabled: 1;
			uint32_t addr: 9; /* reverse lookup address */
			uint32_t ap: 1;
			uint32_t type: 6;   /* reference to an object type */
			uint32_t poll_flash : 1;
		}; 
		uint32_t opt;	
	};

	uint8_t dev;   /* reference to a device type */

    uint8_t tbias;  /* time accuracy multiplication factor */
	uint8_t icfg;   /* current sink configuration */
	uint8_t ipre;   /* current sink preenphasis time */
	uint8_t ilat;   /* Current sink latency (PW reponse time) */

	uint16_t pw1;

	uint16_t pw2;
	uint16_t pw3;

	uint16_t pw4;
	uint16_t pw5;

	uint16_t usr1;
	uint16_t usr2;
	uint16_t usr3;
	uint16_t usr4;
};

#define SS_DEVICES_MAX 320

extern struct ss_device ss_dev_tab[SS_DEVICES_MAX];

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

int config_dump(FILE * f);
int config_erase(void);
int config_compile(void);

struct obj_device * device_db_lookup(unsigned int id);

int slcdev_const_str_purge(void);
int slcdev_const_str_write(const char * s, unsigned int len);

#ifdef __cplusplus
}
#endif

#endif /* __SLCDEV_H__ */

