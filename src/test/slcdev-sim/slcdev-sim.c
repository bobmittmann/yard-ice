#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <sys/dcclog.h>
#include "slcdev.h"

#define __SLCDEV_LIB_DEF__
#include "slcdev-lib.h"

int32_t slcdev_vm_data[SLCDEV_VM_DATA_SZ / 4]; /* data area */
int32_t slcdev_vm_stack[SLCDEV_VM_STACK_SZ / 4]; /* data area */
struct slcdev_usr usr;

void dev_sim_enable(bool mod, unsigned int addr)
{
	if (addr > 160) 
		return;

	ss_dev_tab[addr + (mod ? 160 : 0)].enabled = 1;
}

void dev_sim_disable(bool mod, unsigned int addr)
{
	if (addr > 160) 
		return;

	ss_dev_tab[addr + (mod ? 160 : 0)].enabled = 0;
}

void dev_sim_multiple_disable(uint32_t s[], uint32_t m[])
{
	int n;

	for (n = 0; n < 160; ++n) {
		int j = n / 32; 
		int i = n % 32; 
		/* disable sensors */
		if ((s != NULL) && (s[j] & (1 << i))) {
			ss_dev_tab[n].enabled = 0;
		}
		/* disable modules */
		if ((m != NULL) && (m[j] & (1 << i)))
			ss_dev_tab[160 + n].enabled = 0;
	}
}

void dev_sim_multiple_enable(uint32_t s[], uint32_t m[])
{
	int n;

	for (n = 0; n < 160; ++n) {
		int j = n / 32; 
		int i = n % 32; 
		/* enable sensors */
		if ((s != NULL) && (s[j] & (1 << i)))
			ss_dev_tab[n].enabled = 1;
		/* enable modules */
		if ((m != NULL) && (m[j] & (1 << i)))
			ss_dev_tab[160 + n].enabled = 1;
	}
}

void dev_sim_multiple_alarm_set(uint32_t s[], uint32_t m[], unsigned int lvl)
{
	int n;

	for (n = 0; n < 160; ++n) {
		int j = n / 32; 
		int i = n % 32; 
		/* enable sensors */
		if ((s != NULL) && (s[j] & (1 << i)))
			ss_dev_tab[n].alm = lvl;
		/* enable modules */
		if ((m != NULL) && (m[j] & (1 << i)))
			ss_dev_tab[160 + n].alm = lvl;
	}
}

void dev_sim_multiple_trouble_set(uint32_t s[], uint32_t m[], unsigned int lvl)
{
	int n;

	for (n = 0; n < 160; ++n) {
		int j = n / 32; 
		int i = n % 32; 
		/* enable sensors */
		if ((s != NULL) && (s[j] & (1 << i)))
			ss_dev_tab[n].tbl = lvl;
		/* enable modules */
		if ((m != NULL) && (m[j] & (1 << i)))
			ss_dev_tab[160 + n].tbl = lvl;
	}
}

void sim_js_exec(void * ptr, struct ss_device * dev, 
				 struct db_dev_model * model, uint8_t code[])
{
	struct microjs_vm * vm = (struct microjs_vm *)ptr;

	microjs_exec(vm, code);
}

#define REMOTE_TEST_MSK 0x2d /* 101101 */
#define REMOTE_TEST_ON  0x00 
#define REMOTE_TEST_OFF 0x2d 

/* Default control bits processing for sensors */
void sensor_ctl_default(struct ss_device * dev, 
					   struct db_dev_model * model, uint32_t ctl)
{
	/* Remote test */
	switch (ctl & REMOTE_TEST_MSK) {
	case REMOTE_TEST_ON:
		DCC_LOG(LOG_INFO, "Remote test enabled");
		dev->tst = 1;
		dev->pw2 = device_db_pw_lookup(model->pw2, 1);
		dev->pw4 = device_db_pw_lookup(model->pw4, 3);
		break;
	case REMOTE_TEST_OFF:
		DCC_LOG(LOG_INFO, "Remote test disabled");
		dev->tst = 0;
//		dev->pw2 = device_db_pw_lookup(model->pw2, 0);
//		dev->pw4 = device_db_pw_lookup(model->pw4, 0);
		break;
	}
}

/* simulate a custom sensor */
void sensor_sim_custom(void * ptr, struct ss_device * dev, 
					   struct db_dev_model * model, uint32_t ctl)
{
	struct cmd_list * lst;

	sensor_ctl_default(dev, model, ctl);

	/* execute commands from the device model */
	if ((lst = model->cmd) != NULL) {
		int i;
		for (i = 0; i < lst->cnt; ++i) {
			struct cmd_entry * cmd = &lst->cmd[i];
			if ((ctl & cmd->seq.msk) == cmd->seq.val) {
				DCC_LOG1(LOG_INFO, "CMD[%d]", i);
				sim_js_exec(ptr, dev, model, cmd->code);
			}
		}
	}
}

/* simulate a photodetector smoke sensor */
void sensor_sim_photo(void * ptr, struct ss_device * dev, 
					  struct db_dev_model * model, uint32_t ctl)
{
	int tbl_max = 1;
	int alm_max = 3;
	int lvl;

	DCC_LOG3(LOG_INFO, "%s[%d].ctl=%04x", dev->module ? "module" : "sensor",
			 dev->addr, ctl);

	sensor_ctl_default(dev, model, ctl);

	if ((lvl = dev->alm) > 0) {
		if (lvl > alm_max)
			lvl = alm_max;
		DCC_LOG1(LOG_TRACE, "Alarm %d", dev->alm);
		dev->pw4 = device_db_pw_lookup(model->pw4, lvl + tbl_max);
	} else if ((lvl = dev->tbl) > 0) {
		if (lvl > tbl_max)
			lvl = tbl_max;
		DCC_LOG1(LOG_TRACE, "Trouble %d", dev->tbl);
		dev->pw4 = device_db_pw_lookup(model->pw4, lvl);
	} else {
		dev->pw4 = device_db_pw_lookup(model->pw4, 0);
	}
}

/* simulate a ion smoke detector */
void sensor_sim_ion(void * ptr, struct ss_device * dev, 
					struct db_dev_model * model, uint32_t ctl)
{
	int lvl;

	DCC_LOG3(LOG_INFO, "%s[%d].ctl=%04x", dev->module ? "module" : "sensor",
			 dev->addr, ctl);

	sensor_ctl_default(dev, model, ctl);

	if ((lvl = dev->alm) > 0) {
		if (lvl > 3)
			lvl = 3;
		DCC_LOG1(LOG_INFO, "Alarm %d", dev->alm);
		dev->pw4 = device_db_pw_lookup(model->pw4, lvl + 2);
	} else if ((lvl = dev->tbl) > 0) {
		if (lvl > 2)
			lvl = 2;
		DCC_LOG1(LOG_INFO, "Trouble %d", dev->alm);
		dev->pw4 = device_db_pw_lookup(model->pw4, lvl);
	} else {
		dev->pw4 = device_db_pw_lookup(model->pw4, 0);
	}
}

/* simulate a heat detector sensor */
void sensor_sim_heat(void * ptr, struct ss_device * dev, 
					 struct db_dev_model * model, uint32_t ctl)
{
	int lvl;

	DCC_LOG3(LOG_INFO, "%s[%d].ctl=%04x", dev->module ? "module" : "sensor",
			 dev->addr, ctl);

	sensor_ctl_default(dev, model, ctl);

	if ((lvl = dev->alm) > 0) {
		if (lvl > 2)
			lvl = 2;
		DCC_LOG1(LOG_INFO, "Alarm %d", dev->alm);
		dev->pw4 = device_db_pw_lookup(model->pw4, lvl);
	} else {
		dev->pw4 = device_db_pw_lookup(model->pw4, 0);
	}
}

/* simulate an Acclimate Photoelectric Smoke Sensor */
void sensor_sim_acclimate(void * ptr, struct ss_device * dev, 
						  struct db_dev_model * model, uint32_t ctl)
{
	DCC_LOG3(LOG_INFO, "%s[%d].ctl=%04x", dev->module ? "module" : "sensor",
			 dev->addr, ctl);

	sensor_ctl_default(dev, model, ctl);
}

/* simulate an Beam Smoke Sensor */
void sensor_sim_beam(void * ptr, struct ss_device * dev, 
						  struct db_dev_model * model, uint32_t ctl)
{
	DCC_LOG(LOG_INFO, "...");

	sensor_ctl_default(dev, model, ctl);
}

/* simulate a COPTIR Smoke Detector Sensor */
void sensor_sim_coptir(void * ptr, struct ss_device * dev, 
					   struct db_dev_model * model, uint32_t ctl)
{
	int tbl_max = 7;
	int alm_max = 7;
	int lvl;

	sensor_ctl_default(dev, model, ctl);

	if ((lvl = dev->alm) > 0) {
		if (lvl > alm_max)
			lvl = alm_max;
		DCC_LOG1(LOG_INFO, "Alarm %d", dev->alm);
		dev->pw4 = device_db_pw_lookup(model->pw4, lvl + tbl_max);
	} else if ((lvl = dev->tbl) > 0) {
		if (lvl > tbl_max)
			lvl = tbl_max;
		DCC_LOG1(LOG_INFO, "Trouble %d", dev->alm);
		dev->pw4 = device_db_pw_lookup(model->pw4, lvl);
	} else {
		dev->pw4 = device_db_pw_lookup(model->pw4, 0);
	}
}

/* simulate a custom module */
void module_sim_custom(void * ptr, struct ss_device * dev, 
					   struct db_dev_model * model, uint32_t ctl)
{
	struct cmd_list * lst;

	/* execute commands from the device model */
	if ((lst = model->cmd) != NULL) {
		int i;
		for (i = 0; i < lst->cnt; ++i) {
			struct cmd_entry * cmd = &lst->cmd[i];
			if ((ctl & cmd->seq.msk) == cmd->seq.val) {
				DCC_LOG1(LOG_INFO, "CMD[%d]", i);
				sim_js_exec(ptr, dev, model, cmd->code);
			}
		}
	}
}

#define CONTROL_OUT_MSK 0x2d /* 101101 */
#define CONTROL_OUT_ON  0x00 
#define CONTROL_OUT_OFF 0x2d 


void module_contorl_seq(struct ss_device * dev, 
						struct db_dev_model * model, uint32_t ctl)
{
	switch (ctl & 0x7) {
	case 0:/* 0 0 0 */
		/* Outputs:         1  2  3  5 */
		dev->out1 = 1; /* Pulse */
		dev->out2 = 1; 
		dev->out3 = 1;
		dev->out5 = 1;
		DCC_LOG(LOG_MSG, "Pu 1  1  1");
		break;
	case 4: /* 0 0 1 */
		dev->out1 = 1; /* Pulse */
		dev->out3 = 1;
		dev->out5 = 1;
		DCC_LOG(LOG_MSG, "Pu NC 1  1");
		break;
	case 2: /* 0 1 0 */
		dev->out1 = 0;
		dev->out2 = 1;
		dev->out3 = 1;
		dev->out5 = 1;
		DCC_LOG(LOG_MSG, "0  1  1  1");
		break;
	case 6: /* 0 1 1 */
		dev->out1 = 0;
		dev->out3 = 1;
		dev->out5 = 1;
		DCC_LOG(LOG_MSG, "0  NC 1  1");
		break;
	case 1: /* 1 0 0 */
		dev->out1 = 1; /* Pulse */
		dev->out3 = 0;
		dev->out5 = 0;
		DCC_LOG(LOG_MSG, "Pu NC 0  0");
		break;
	case 5: /* 1 0 1 */
		dev->out1 = 0; /* Pulse */
		dev->out2 = 0;
		dev->out3 = 1; /* Pulse */
		dev->out5 = 1; /* Pulse */
		DCC_LOG(LOG_MSG, "Pu 0  Pu Pu");
		break;
	case 3: /* 1 1 0 */
		dev->out1 = 0;
		dev->out2 = 0;
		dev->out5 = 0;
		DCC_LOG(LOG_MSG, "0  NC 0  0");
		break;
	case 7: /* 1 1 1 */
		dev->out1 = 0;
		dev->out2 = 0;
		dev->out3 = 1; /* Pulse */
		dev->out5 = 1; /* Pulse */
		DCC_LOG(LOG_MSG, "0  0  Pu Pu");
		break;
	}

	DCC_LOG4(LOG_INFO, "%d %d %d %d",
			 dev->out1, dev->out2, dev->out3, dev->out5);
}


/* simulate a relay module */
void module_sim_relay(void * ptr, struct ss_device * dev, 
					  struct db_dev_model * model, uint32_t ctl)
{
	DCC_LOG2(LOG_INFO, "addr=%d ctl=0x%04x", dev->addr, ctl);

	module_contorl_seq(dev, model, ctl);

	if ((ctl & 0x81) == 0) {
		/* 1.	Bit 10 = 0,  sent two consecutive times, 
		   will reset PW3 latches */
		DCC_LOG(LOG_INFO, "Reset PW3 latches.");
	}

	switch (ctl & CONTROL_OUT_MSK) {
	case CONTROL_OUT_ON:
		DCC_LOG(LOG_INFO, "Set");
		dev->pw2 = device_db_pw_lookup(model->pw2, 1);
		break;

	case CONTROL_OUT_OFF:
		DCC_LOG(LOG_INFO, "Reset");
		dev->pw2 = device_db_pw_lookup(model->pw2, 0);
		break;
	}
}

/* simulate a supervised cntrol module */
void module_sim_control(void * ptr, struct ss_device * dev, 
						struct db_dev_model * model, uint32_t ctl)
{
	DCC_LOG2(LOG_INFO, "addr=%d ctl=0x%04x", dev->addr, ctl);

	module_contorl_seq(dev, model, ctl);

	switch (ctl & CONTROL_OUT_MSK) {
	case CONTROL_OUT_ON:
		DCC_LOG(LOG_INFO, "Control ON");
		dev->pw2 = device_db_pw_lookup(model->pw2, 1);
		break;
	case CONTROL_OUT_OFF:
		DCC_LOG(LOG_INFO, "Control Off");
		dev->pw2 = device_db_pw_lookup(model->pw2, 0);
		break;
	}
}

/* simulate a firefighter telephone module */
void module_sim_phone(void * ptr, struct ss_device * dev, 
					  struct db_dev_model * model, uint32_t ctl)
{
	DCC_LOG2(LOG_INFO, "addr=%d ctl=0x%04x", dev->addr, ctl);

	module_contorl_seq(dev, model, ctl);

	switch (ctl & CONTROL_OUT_MSK) {
	case CONTROL_OUT_ON:
		DCC_LOG(LOG_INFO, "Set");
		dev->pw2 = device_db_pw_lookup(model->pw2, 1);
		break;

	case CONTROL_OUT_OFF:
		DCC_LOG(LOG_INFO, "Reset");
		dev->pw2 = device_db_pw_lookup(model->pw2, 0);
		break;
	}
}

#define CLASS_A_MSK      0x2d /* 101101 */
#define CLASS_A_SWITCHED 0x00 
#define CLASS_A_NORMAL   0x2d 

/* simulate a monitor module */
void module_sim_monitor(void * ptr, struct ss_device * dev, 
						struct db_dev_model * model, uint32_t ctl)
{
	DCC_LOG2(LOG_INFO, "addr=%d ctl=0x%04x", dev->addr, ctl);

	switch (ctl & CLASS_A_MSK) {
	case CLASS_A_SWITCHED:
		DCC_LOG(LOG_INFO, "Class A switched");
		dev->pw2 = device_db_pw_lookup(model->pw2, 1);
		break;
	case CLASS_A_NORMAL:
		DCC_LOG(LOG_INFO, "Class A normal");
		dev->pw2 = device_db_pw_lookup(model->pw2, 0);
		break;
	}
}

/* simulate a mini-module */
void module_sim_mini(void * ptr, struct ss_device * dev, 
					 struct db_dev_model * model, uint32_t ctl)
{
	DCC_LOG2(LOG_INFO, "addr=%d ctl=0x%04x", dev->addr, ctl);
}

/* simulate a 2 wire module */
void module_sim_czif(void * ptr, struct ss_device * dev, 
					  struct db_dev_model * model, uint32_t ctl)
{
	DCC_LOG2(LOG_INFO, "addr=%d ctl=0x%04x", dev->addr, ctl);
}

/* simulate a 4-20ma input device */
void module_sim_4_20ma(void * ptr, struct ss_device * dev, 
					   struct db_dev_model * model, uint32_t ctl)
{
	DCC_LOG2(LOG_INFO, "addr=%d ctl=0x%04x", dev->addr, ctl);
}

#define SIM_MODEL_NAME_MAX 12

struct sim_model {
	const char name[SIM_MODEL_NAME_MAX]; 
	void (* run)(void * ptr, struct ss_device * dev, 
				 struct db_dev_model * model, 
				 uint32_t ctl);
};

const struct sim_model sim_model_lut[] = {
	[0] = { .name = "custom", .run = sensor_sim_custom },
	[1] = { .name = "photo", .run = sensor_sim_photo },
	[2] = { .name = "ion", .run = sensor_sim_ion },
	[3] = { .name = "heat", .run = sensor_sim_heat },
	[4] = { .name = "acclimate", .run = sensor_sim_acclimate },
	[5] = { .name = "beam", .run = sensor_sim_beam },
	[6] = { .name = "coptir", .run = sensor_sim_coptir },

	[7] = { .name = "custom", .run = module_sim_custom },
	[8] = { .name = "relay", .run = module_sim_relay },
	[9] = { .name = "control", .run = module_sim_control },
	[10] = { .name = "monitor", .run = module_sim_monitor },
	[11] = { .name = "mini", .run = module_sim_mini },
	[12] = { .name = "czif", .run = module_sim_czif },
	[13] = { .name = "phone", .run = module_sim_phone },
	[14] = { .name = "4-20mA", .run = module_sim_4_20ma }
};

#define SENSOR_SIM_CUSTOM 0
/* XXX: changes in the table bellow must be refelected in 
   this two constants */
#define MODULE_SIM_CUSTOM 7
#define MODEL_SIM_LAST 13

int sensor_sim_lookup(const char * name, unsigned int len)
{
	int i;

	for (i = SENSOR_SIM_CUSTOM; i < MODULE_SIM_CUSTOM; ++i) {
		if ((strncmp(name, sim_model_lut[i].name, len) == 0) && 
			(strlen(sim_model_lut[i].name) == len))
			return i;
	}

	return SENSOR_SIM_CUSTOM;
}

int sensor_sim_default(void)
{
	return SENSOR_SIM_CUSTOM;
}

int module_sim_lookup(const char * name, unsigned int len)
{
	int i;

	for (i = MODULE_SIM_CUSTOM; i <= MODEL_SIM_LAST; ++i) {
		if ((strncmp(name, sim_model_lut[i].name, len) == 0) && 
			(strlen(sim_model_lut[i].name) == len))
			return i;
	}

	return MODULE_SIM_CUSTOM;
}

int module_sim_default(void)
{
	return MODULE_SIM_CUSTOM;
}

const char * model_sim_name(unsigned int idx)
{
	if (idx > MODEL_SIM_LAST)
		return "";

	return sim_model_lut[idx].name;
}

void __attribute__((noreturn)) sim_event_task(void)
{
	struct microjs_rt rt;
	struct db_dev_model * model;
	struct ss_device * dev;
	struct db_info * db;
	struct microjs_vm vm; 
	uint32_t ev_bmp;
	uint32_t ctl;

	/* initialize virtual machine */
	rt.data_sz = sizeof(slcdev_vm_data);
	rt.stack_sz = sizeof(slcdev_vm_stack);
	microjs_vm_init(&vm, &rt, NULL, slcdev_vm_data, slcdev_vm_stack);

	thinkos_sleep(2000);

	db = db_info_get();
	DCC_LOG1(LOG_INFO, "db=%08x", db);

	for (;;) {
		int ev;

		DCC_LOG(LOG_INFO, ".1");

		thinkos_flag_wait(SLCDEV_DRV_EV_FLAG);
		ev_bmp = slcdev_drv.ev_bmp;
		/* FIXME: race condition, exclusive access is needed!!! */
		slcdev_drv.ev_bmp = 0;
		thinkos_flag_clr(SLCDEV_DRV_EV_FLAG);

		dev = slcdev_drv.dev;
		ctl = slcdev_drv.ctls;
		DCC_LOG(LOG_INFO, ".2");
		/* get the model for this device */
		model = db_dev_model_by_index(db, dev->model);
		DCC_LOG(LOG_INFO, ".3");

		/* get an event form bitmap */
		while ((ev = __clz(__rbit(ev_bmp))) != 32) {

			/* clear event from bitmap */
			ev_bmp &= ~(1 << ev);
			switch (ev) {

			case SLC_EV_DEV_POLL:
				if (model != NULL) {
					const struct sim_model * sim;
					sim = &sim_model_lut[model->sim];

					DCC_LOG2(LOG_INFO, "dev=%d ctl=0x%x", dev->addr, ctl);

					if (dev->ledno) {
						/* Poll LED state */
						if ((ctl & 0x4) == 0) {
							led_on(dev->ledno - 1);
						} else if ((ctl & 0x5) == 4) {
							led_off(dev->ledno - 1);
						} else if ((ctl & 0x5) == 5) {
							led_flash(dev->ledno - 1, 64);
						}
					}

					if ((ctl & 0x4) == 0)
						dev->led = 1;
					else if ((ctl & 0x5) == 4)
						dev->led = 0;
					sim->run(&vm, dev, model, ctl);
				}
				break;

			case SLC_EV_TRIG:
				DCC_LOG1(LOG_INFO, "trigger %d", dev->addr);
				microjs_exec(&vm, usr.trig);
				break;

			case SLC_EV_SW1_OFF:
				DCC_LOG(LOG_TRACE, "SW1_OFF");
				microjs_exec(&vm, usr.sw[0].off);
				break;

			case SLC_EV_SW1_UP:
				DCC_LOG1(LOG_TRACE, "SW1_UP %p", usr.sw[0].up);
				microjs_exec(&vm, usr.sw[0].up);
				break;

			case SLC_EV_SW1_DOWN:
				DCC_LOG(LOG_TRACE, "SW1_DOWN");
				microjs_exec(&vm, usr.sw[0].down);
				break;

			case SLC_EV_SW2_OFF:
				DCC_LOG(LOG_TRACE, "SW2_OFF");
				microjs_exec(&vm, usr.sw[1].off);
				break;

			case SLC_EV_SW2_UP:
				DCC_LOG(LOG_TRACE, "SW2_UP");
				microjs_exec(&vm, usr.sw[1].up);
				break;

			case SLC_EV_SW2_DOWN:
				DCC_LOG(LOG_TRACE, "SW2_DOWN");
				microjs_exec(&vm, usr.sw[1].down);
				break;

			case SLC_EV_TMR1:
				DCC_LOG(LOG_TRACE, "TMR1");
				microjs_exec(&vm, usr.tmr[0]);
				break;

			case SLC_EV_TMR2:
				DCC_LOG(LOG_TRACE, "TMR2");
				microjs_exec(&vm, usr.tmr[1]);
				break;

			case SLC_EV_TMR3:
				DCC_LOG(LOG_TRACE, "TMR3");
				microjs_exec(&vm, usr.tmr[2]);
				break;

			case SLC_EV_TMR4:
				DCC_LOG(LOG_TRACE, "TMR4");
				microjs_exec(&vm, usr.tmr[3]);
				break;

			case SLC_EV_USR1:
				DCC_LOG(LOG_TRACE, "USR1");
				microjs_exec(&vm, usr.usr[0]);
				break;

			case SLC_EV_USR2:
				DCC_LOG(LOG_TRACE, "USR2");
				microjs_exec(&vm, usr.usr[1]);
				break;

			case SLC_EV_USR3:
				DCC_LOG(LOG_TRACE, "USR3");
				microjs_exec(&vm, usr.usr[2]);
				break;

			case SLC_EV_USR4:
				DCC_LOG(LOG_TRACE, "USR4");
				microjs_exec(&vm, usr.usr[3]);
				break;

			case SLC_EV_USR5:
				DCC_LOG(LOG_TRACE, "USR5");
				microjs_exec(&vm, usr.usr[4]);
				break;

			case SLC_EV_USR6:
				DCC_LOG(LOG_TRACE, "USR6");
				microjs_exec(&vm, usr.usr[5]);
				break;

			case SLC_EV_USR7:
				DCC_LOG(LOG_TRACE, "USR7");
				microjs_exec(&vm, usr.usr[6]);
				break;

			case SLC_EV_USR8:
				DCC_LOG(LOG_TRACE, "USR8");
				microjs_exec(&vm, usr.usr[7]);
				break;

			case SLC_EV_SIM_START:
				DCC_LOG(LOG_TRACE, "SIM_START");
				db = db_info_get();
				/* clear virtual machine data area */
				microjs_vm_clr_data(&vm, &rt);
				/* run script */
				microjs_exec(&vm, usr.init);
				/* resume simulation */
				slcdev_event_raise(SLC_EV_SIM_RESUME);
				break;

			case SLC_EV_SIM_STOP:
				DCC_LOG(LOG_TRACE, "SIM_STOP");
				slcdev_stop();
				break;

			case SLC_EV_SIM_RESUME:
				DCC_LOG(LOG_TRACE, "SIM_RESUME");
				slcdev_resume();
				break;
			}
		}
	}
}

