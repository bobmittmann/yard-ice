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
#include "flashfs.h"
#include <stdio.h>
#include <thinkos.h>
#include <microjs.h>

/* default current source (PW) latency */
#define ILAT_DEFAULT 20
#define IPRE_DEFAULT 35

/***************************************************************************
  Database
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

/* PW range represents a set of conntiguos entries in a PW list... */
struct pw_range {
	uint8_t idx;
	uint8_t cnt;
};

/* PW4 set is a set of ranges from the PW4 list:
   tbl: range of PW4 values representing trouble conditions. 
   alm: range of PW4 values representing alarm conditions. 
   tst: range of PW4 values representing remote test (usually one entry only). 
 */
struct pw4_set {
	struct pw_range tbl;
	struct pw_range alm;
	struct pw_range tst;
};

struct cmd_seq {
	uint16_t msk;
	uint16_t val;
};

struct cmd_entry {
	struct cmd_seq seq;
	uint8_t tag; /* The tag string */
	uint8_t * code; /* Compiled javascrit bytecodes */
};

#define SLCDEV_CMD_LIST_LEN_MAX 12

struct cmd_list {
	uint32_t cnt;
	struct cmd_entry cmd[SLCDEV_CMD_LIST_LEN_MAX]; 
};

/***************************************************************************
  Simulation models 
 ***************************************************************************/


struct db_dev_model {
	struct db_dev_model * next;
	union {
		uint32_t opt;
		struct {
			uint32_t ap: 1;
			uint32_t module: 1;
			uint8_t alm_lvl: 4;
			uint8_t tbl_lvl: 4;
		};
	};
	uint8_t model;	
	uint8_t desc;	
	uint8_t sim; /* Simulation algorithm */
	uint8_t res;
	const struct pw_list * pw1lst; /* Reference Pulse Width */
	const struct pw_list * pw2lst; /* Remote Test Status */
	const struct pw_list * pw3lst; /* Manufacturer Code */
	const struct pw_list * pw4lst; /* Analog */
	const struct pw_list * pw5lst; /* Type Id */
	struct pw4_set pw4lut;
	struct cmd_list * cmd;
};

#define DB_MODEL_MAX 64

struct db_info {
	uint32_t magic;
	uint8_t desc; /* Description string */
	uint8_t version[3]; /* version info */
	struct {
		uint16_t len;
		uint16_t crc;
		const char * txt;
	} json;

	uint32_t * symbuf; /* symbol buffer */
	uint16_t symbuf_sz; /* symbol buffer size */

	uint16_t obj_cnt;
	struct db_dev_model * obj[DB_MODEL_MAX];
};


/***************************************************************************
  Events
 ***************************************************************************/

#define SLC_EV_SIM_STOP   0
#define SLC_EV_SIM_START  1
#define SLC_EV_SIM_RESUME 2
#define SLC_EV_DEV_POLL   3
#define SLC_EV_TRIG       4
#define SLC_EV_SW1_OFF    5
#define SLC_EV_SW1_UP     6
#define SLC_EV_SW1_DOWN   7
#define SLC_EV_SW2_OFF    8
#define SLC_EV_SW2_UP     9
#define SLC_EV_SW2_DOWN   10

#define SLC_EV_TMR1       11
#define SLC_EV_TMR2       12
#define SLC_EV_TMR3       13
#define SLC_EV_TMR4       14

#define SLC_EV_USR1       16
#define SLC_EV_USR2       17
#define SLC_EV_USR3       18
#define SLC_EV_USR4       19
#define SLC_EV_USR5       20
#define SLC_EV_USR6       21
#define SLC_EV_USR7       22
#define SLC_EV_USR8       23

#define SLC_EV_AP_DIRECT_POLL 24
#define SLC_EV_AP_GROUP_POLL  25
#define SLC_EV_AP_RD_PRESENCE 26

/***************************************************************************
  Runtime
 ***************************************************************************/

struct usr_switch {
	uint8_t * off;
	uint8_t * up; 
	uint8_t * down;
};

struct slcdev_cfg_info {
	uint8_t tag; /* TAG string */
	uint8_t author; /* Author string */
	uint8_t desc; /* Description string */
	uint8_t version[3]; /* version info */
};

struct slcdev_usr {
	struct slcdev_cfg_info cfg_info;
	struct usr_switch sw[2];
	uint8_t * init; /* init script */
	uint8_t * trig; /* trigger script */
	uint8_t * tmr[4]; /* timers script */
	uint8_t * usr[8]; /* user events script */
	uint32_t verbose: 1;
};

extern struct slcdev_usr usr;

#define SLCDEV_VERBOSE() (usr.verbose) 

/* -------------------------------------------------------------------------
 * Sysem Sensor device
 * ------------------------------------------------------------------------- */

struct ss_device {
	union {
		struct {
			uint32_t addr: 8;   /* reverse lookup address */

			uint32_t model: 6;  /* reference to a device model */
			uint32_t apen : 1;  /* advanced protocol */
			uint32_t module : 1; /* 1 = module, 0 = sensor */

			uint32_t enabled : 1; /* enable device simulation */
			uint32_t cfg : 1; /* Device configured */
			uint32_t led : 1; /* LED status */
			uint32_t tst : 1; /* Remote test mode */

			uint32_t out1: 1;  
			uint32_t out2: 1;  
			uint32_t out3: 1;  
			uint32_t out5: 1;  

			uint32_t alm : 4; /* Alarm level */
			uint32_t tbl : 4; /* Trouble level */
		}; 
		uint32_t opt;	
	};
	
	uint8_t tbias;     /* time accuracy multiplication factor */
	uint8_t ilat;      /* Current sink latency (PW reponse time) */
	uint8_t icfg;      /* current sink configuration */
	uint8_t ipre;      /* current sink preenphasis time */

	uint8_t grp[4];    /* Group membership */
	uint8_t lvl[4];    /* Internal variable levels */

	union {
		struct {
			uint16_t pw1;   /* Reference Pulse Width */
			uint16_t pw2;   /* Remote Test Status */
			uint16_t pw3;   /* Manufacturer Code */
			uint16_t pw4;   /* Analog */
			uint16_t pw5;   /* Type Id */
		};
		struct {
			uint16_t pw;   /* AP bit response pulse width */
		} ap;
	};

	uint8_t event:5;   /* Simulation event */
	uint8_t ledno:3;
	uint8_t res2[1];

	uint32_t pcnt;     /* poll count */
};

#define SS_MODULES_IDX 160
#define SS_DEVICES_MAX 320

extern struct ss_device ss_dev_tab[SS_DEVICES_MAX];

/***************************************************************************
  Configuration
 ***************************************************************************/

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


/* -------------------------------------------------------------------------
 * SLC Device Driver 
 * ------------------------------------------------------------------------- */

struct slcdev_trig {
	uint16_t msk;    /* bitmask */
	uint16_t cmp;    /* compare value */
	uint16_t ap_msk; /* AP mode bitmask */
	uint16_t ap_cmp; /* AP mode compare value */
};

/* device simulator context */
struct slcdev_sim {
	volatile uint32_t halt : 1;
	uint32_t ctls;      /* consecutive polling sequence control bit pattern */
	struct {
		uint8_t insn;   /* AP instruction */
		uint8_t irq;    /* Interrupt request */
		uint8_t zone;   
		uint8_t cmd;   
		uint8_t parm;   /* command parameter */
		uint8_t subaddr;   /* subaddress */
	} ap; /* AP specific options */
};

struct slcdev_drv {
	int state;          /* decoder state */
	int clip_state;     /* decoder state */
	uint16_t idx;       /* current polled device index */
	uint8_t bit_cnt;    /* message bit count */
	uint8_t pw5en;      /* PW5 (Type ID) requested */
	uint32_t msg;       /* message data from the panel */
	struct slcdev_trig trig; /* trigger module  */
	struct slcdev_sim sim;
	struct {
		uint8_t icfg;   /* pulse preenphasis width (microsseconds) */
		uint8_t ipre;   /* pulse preenphasis width (microsseconds) */
		uint8_t ilat;   /* pulse latency (microsseconds) */
		uint16_t ipw;   /* pulse width (microsseconds) */
		struct {
			uint32_t act;   /* active bitmap, one bit per ten digit, 
							   one bit per ones digit */
			uint8_t tens[16]; /* number of devices per ten digit */
			uint8_t ones[10]; /* number of devices per one digit */
		} s; /* sensors */
		struct {
			uint32_t act;   /* active bitmap, one bit per ten digit, 
							   one bit per ones digit */
			uint8_t tens[16]; /* number of devices per ten digit */
			uint8_t ones[10]; /* number of devices per one digit */
		} m; /* modules */
	} ap; /* AP specific options */

	struct {
		unsigned int state; /* decoder state */
		uint8_t pre; /* pulse preenphasis width (microsseconds) */
		uint8_t lat; /* pulse latency (microsseconds) */
		uint16_t pw; /* pulse width (microsseconds) */
	} isink;
	struct ss_device * volatile dev;
};

/* Control bits simulation trigger:
 
   The control bit pattern triggers works by ...

   Each bit control sequence (3 bits) is shifted in to the "ctls"
   shift registers.
   "ctls" will shift only consecutive polling for the same device. 
   The "ctls" will be cleared whenever a different device is addressed.
 
 */

#define SLCDEV_VM_STACK_SZ 64
#define SLCDEV_VM_DATA_SZ 64

extern struct slcdev_drv slcdev_drv;
extern int32_t slcdev_vm_data[SLCDEV_VM_DATA_SZ / 4]; /* data area */
extern int32_t slcdev_vm_stack[SLCDEV_VM_STACK_SZ / 4]; /* data area */
extern uint32_t slcdev_symbuf[64]; /* symbol table buffer */

#ifdef __cplusplus
extern "C" {
#endif

static inline void trig_out_clr(void) {
	stm32_gpio_clr(TRIG_OUT);
}

static inline void trig_out_set(void) {
	stm32_gpio_set(TRIG_OUT);
}
void slcdev_init(void);
void slcdev_stop(void);
void slcdev_resume(void);
void slcdev_sleep(void);

bool trig_addr_set(unsigned int addr);
unsigned int trig_addr_get(void);
void trig_module_set(bool en);
void trig_sensor_set(bool en);

void device_db_init(void);
bool device_db_compile(struct fs_file * json);
int device_db_info(FILE * f);
int device_db_dump(FILE * f);

int config_dump(FILE * f);

int config_erase(void);
int config_load(void);
int config_save(struct fs_file * json);
int config_compile(struct fs_file * json);
bool config_need_update(struct fs_file * json);
bool config_is_sane(void);
int config_show_info(FILE * f);

struct db_dev_model * device_db_lookup(unsigned int id);
bool device_db_need_update(struct fs_file * json);
bool device_db_is_sane(void);

uint8_t * db_js_lookup(const char * model, const char * jstag);

int const_strbuf_init(void);

struct db_info * db_info_get(void);

int db_dev_model_index_by_name(struct db_info * inf,
							   unsigned int str_id);

struct db_dev_model * db_dev_model_by_index(struct db_info * inf,
											unsigned int idx);

/* default photodetector sensor */ 
struct db_dev_model * db_dev_model_photo(void);

int device_db_pw_lookup(const struct pw_list * lst, unsigned int sel);

void __attribute__((noreturn)) sim_event_task(void);

int sensor_sim_lookup(const char * name, unsigned int len);

int module_sim_lookup(const char * name, unsigned int len);

int sensor_sim_default(void);

int module_sim_default(void);

const char * model_sim_name(unsigned int idx);

struct ss_device * dev_sim_lookup(bool mod, unsigned int addr); 

static inline struct ss_device * module(unsigned int addr) {
	return &ss_dev_tab[addr + 160];
}

static inline struct ss_device * sensor(unsigned int addr) {
	return &ss_dev_tab[addr];
}

void dev_sim_uncofigure_all(void);

void dev_sim_enable(bool mod, unsigned int addr);

void dev_sim_disable(bool mod, unsigned int addr);

void dev_sim_multiple_disable(uint32_t s[], uint32_t m[]);

void dev_sim_multiple_enable(uint32_t s[], uint32_t m[]);

void dev_sim_multiple_alarm_set(uint32_t s[], uint32_t m[], 
								unsigned int lvl);

void dev_sim_multiple_trouble_set(uint32_t s[], uint32_t m[], 
								  unsigned int lvl);

int device_dump(FILE * f, bool mod, unsigned int addr);

int device_attr_set(bool mod, unsigned int addr, 
					const char * name, const char * val);

int device_attr_print(FILE * f, bool mod, 
					  unsigned int addr, const char * name);

void sim_reset(void);

void slcdev_sim_stop(void);


void slcdev_ssdev_enable(struct ss_device * dev);

void slcdev_ssdev_disable(struct ss_device * dev);

struct ss_device * slcdev_ssdev_getinstance(bool module, unsigned int addr);

void slcdev_update_ap(void); 

#ifdef __cplusplus
}
#endif

#endif /* __SLCDEV_H__ */

