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
#include <thinkos.h>

/***************************************************************************
  Device  Database
 ***************************************************************************/

#define SLCDEV_DESC_LEN_MAX 64

struct pw_entry {
	uint16_t min;
	uint16_t max;
	uint8_t desc;	/* The description string */
};

#define SLCDEV_PW_LIST_LEN_MAX 24

struct pw_list {
	uint32_t cnt;
	struct pw_entry pw[]; 
};

struct cmd_seq {
	uint16_t msk;
	uint16_t val;
} seq;

struct cmd_entry {
	struct cmd_seq seq;
	uint8_t tag; /* The tag string */
	uint8_t js[7]; /* The javascript lines */
};

#define SLCDEV_CMD_LIST_LEN_MAX 12

struct cmd_list {
	uint32_t cnt;
	struct cmd_entry cmd[SLCDEV_CMD_LIST_LEN_MAX]; 
};

struct db_obj {
	uint8_t len;
	uint8_t type;
	uint8_t id;
	uint8_t flags;
	struct db_obj * next;
};

struct db_info {
	struct {
		uint8_t len;
		uint8_t type;
		uint8_t id;
		uint8_t flags;
		struct db_obj * next;
	};
	uint16_t json_crc;
	uint16_t json_len;
	uint32_t obj_cnt;
	struct db_obj * obj[];
};

struct db_dev_model {
	struct {
		uint8_t len;
		uint8_t type;
		uint8_t id;
		uint8_t flags;
		struct db_obj * next;
	};
	uint16_t model;	
	uint16_t desc;	
	const struct pw_list * pw1; /* Reference Pulse Width */
	const struct pw_list * pw2; /* Remote Test Status */
	const struct pw_list * pw3; /* Manufacturer Code */
	const struct pw_list * pw4; /* Analog */
	const struct pw_list * pw5; /* Type Id */
	struct cmd_list * cmd;
};

struct obj_module {
	struct {
		uint8_t len;
		uint8_t type;
		uint8_t id;
		uint8_t flags;
		struct db_obj * next;
	};
	uint16_t model;	
	uint16_t desc;	
	struct pw_list * pw1;
	struct pw_list * pw2;
	struct pw_list * pw3;
	struct pw_list * pw4;
	struct pw_list * pw5;
	struct cmd_list * cmd;
};

struct obj_sensor {
	struct {
		uint8_t len;
		uint8_t type;
		uint8_t id;
		uint8_t flags;
		struct db_obj * next;
	};
	uint16_t model;	
	uint16_t desc;	
	struct pw_list * pw1;
	struct pw_list * pw2;
	struct pw_list * pw3;
	struct pw_list * pw4;
	struct pw_list * pw5;
	struct cmd_list * cmd;
};

enum {
	DB_OBJ_DB_INFO = 0,
	DB_OBJ_SENSOR,
	DB_OBJ_MODULE
};

/***************************************************************************
  Events
 ***************************************************************************/

#define SLC_EV_NONE 0
#define SLC_EV_SIM  (1 << 0)
#define SLC_EV_TRIG (1 << 1)

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
			uint32_t addr: 9; /* reverse lookup address */
			uint32_t model : 6; /* reference to a device model */
			uint32_t enabled : 1; /* enable device simulation */
			uint32_t ap : 1; /* advanced protocol */
			uint32_t led : 1; /* LED status */
			uint32_t pw5en : 1; /* PW5 (Type ID) enabled */
			uint32_t tst : 1; /* Remote test mode */
		}; 
		uint32_t opt;	
	};


    uint8_t tbias;  /* time accuracy multiplication factor */
	uint8_t icfg;   /* current sink configuration */
	uint8_t ipre;   /* current sink preenphasis time */
	uint8_t ilat;   /* Current sink latency (PW reponse time) */

	uint16_t pw1;   /* Reference Pulse Width */
	uint16_t pw2;   /* Remote Test Status */
	uint16_t pw3;   /* Manufacturer Code */

	uint16_t pw4;   /* Analog */
	uint16_t pw5;   /* Type Id */

	uint16_t ctln;   /* non consecutive polling control bit pattern */
	uint16_t ctls;   /* consecutive polling sequence control bit pattern */
};

/* Control bits simulation trigger:
 
   The control bit pattern triggers works by ...

   Each bit control sequence (3 bits) is shifted in to the "ctln" and "ctls"
   shift registers.
   The difference between them is that "ctln" will shift in all incoming 
   control bits regardless of the polling sequence, whereas the "ctls" will
   shift only consecutive polling for the same device. The "ctls" will be 
   cleared whenever a different device is addressed.
 
 */

#define SS_DEVICES_MAX 320

extern struct ss_device ss_dev_tab[SS_DEVICES_MAX];

/* -------------------------------------------------------------------------
 * SLC Device Driver 
 * ------------------------------------------------------------------------- */

enum {
	DEV_IDLE = 0,
	DEV_MSG,

	DEV_PW1_START_WAIT,
	DEV_PW1_RESPONSE_TIME,
	DEV_PW1_PULSE,
	DEV_PW1_END_WAIT,

	DEV_PW2_START_WAIT,
	DEV_PW2_RESPONSE_TIME,
	DEV_PW2_PULSE,
	DEV_PW2_END_WAIT,

	DEV_PW3_START_WAIT,
	DEV_PW3_RESPONSE_TIME,
	DEV_PW3_PULSE,
	DEV_PW3_END_WAIT,

	DEV_PW4_START_WAIT,
	DEV_PW4_RESPONSE_TIME,
	DEV_PW4_PULSE,
	DEV_PW4_END_WAIT,

	DEV_PW5_START_WAIT,
	DEV_PW5_RESPONSE_TIME,
	DEV_PW5_PULSE,
	DEV_PW5_END_WAIT,
	DEV_RECOVER_TIME,

	DEV_INACTIVE_START_WAIT,
	DEV_INACTIVE_STOP_WAIT,

	DEV_PARITY_ERROR,
	DEV_PW_ABORT

};

struct slcdev_drv {
	int8_t ev_flag;
	uint8_t ev_bmp;
	uint8_t trig_en;
	uint8_t bit_cnt;
	uint16_t msg;
	uint16_t trig_addr;
	unsigned int state;
	unsigned int addr;
	struct ss_device * volatile dev;
} ;

extern struct slcdev_drv slcdev_drv;

#ifdef __cplusplus
extern "C" {
#endif

static inline void trig_out_clr(void) {
	stm32_gpio_clr(TRIG_OUT);
}

static inline void trig_out_set(void) {
	stm32_gpio_set(TRIG_OUT);
}

static inline uint32_t slcdev_event_wait(void) {
	uint32_t events;
	thinkos_flag_wait(slcdev_drv.ev_flag);
	events = slcdev_drv.ev_bmp;
	slcdev_drv.ev_bmp= 0;
	thinkos_flag_clr(slcdev_drv.ev_flag);
	return events;
}

void trig_addr_set(unsigned int addr);

void trig_mode_set(unsigned int mode);

unsigned int trig_addr_get(void);

void slcdev_init(void);

void dev_sim_enable(unsigned int addr);

void dev_sim_disable(unsigned int addr);

int device_db_init(void);
int device_db_erase(void);
int device_db_compile(void);
int device_db_dump(FILE * f);

int config_dump(FILE * f);
int config_erase(void);
int config_compile(void);

struct db_dev_model * device_db_lookup(unsigned int id);

int slcdev_const_str_purge(void);

int db_dev_model_index_by_name(unsigned int str_id);

struct db_dev_model * db_dev_model_by_index(unsigned int idx);

/* default photodetector sensor */ 
struct db_dev_model * db_dev_model_photo(void);

int device_db_pw1_lookup(struct db_dev_model * obj, unsigned int sel,
						 unsigned int bias);

int device_db_pw2_lookup(struct db_dev_model * obj, unsigned int sel,
						 unsigned int bias);

int device_db_pw3_lookup(struct db_dev_model * obj, unsigned int sel,
						 unsigned int bias);

int device_db_pw4_lookup(struct db_dev_model * obj, unsigned int sel,
						 unsigned int bias);

int device_db_pw5_lookup(struct db_dev_model * obj, unsigned int sel,
						 unsigned int bias);

void __attribute__((noreturn)) sim_event_task(void);

#ifdef __cplusplus
}
#endif

#endif /* __SLCDEV_H__ */

