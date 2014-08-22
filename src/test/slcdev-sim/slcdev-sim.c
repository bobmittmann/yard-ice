#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <sys/dcclog.h>
#include "slcdev.h"

struct slcdev_obj {
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


struct ratio_u16 {
	int16_t num;
	uint16_t den;
};

const struct ratio_u16 sim_err_ltu[] = {
	{    1,     1 }, /*   0 % */
	{  1020, 1000 }, /*   2 % */
	{  1040, 1000 }, /*   4 % */
	{  1080, 1000 }, /*   8 % */
	{   840, 1000 }, /* -16 % */
	{   920, 1000 }, /*  -8 % */
	{   960, 1000 }, /*  -4 % */
	{   920, 1000 }  /*  -2 % */
};

int slcdev_sim_set(void)
{

	return 0;
}

void dev_sim_enable(unsigned int addr)
{
	if (addr > SS_DEVICES_MAX) 
		return;

	ss_dev_tab[addr].enabled = 1;
}

void dev_sim_disable(unsigned int addr)
{
	if (addr > SS_DEVICES_MAX) 
		return;

	ss_dev_tab[addr].enabled = 0;
}

struct ss_device * dev_sim_sensor_lookup(unsigned int addr)
{
	if (addr > 160) 
		return NULL;

	return &ss_dev_tab[addr];
}

struct ss_device * dev_sim_module_lookup(unsigned int addr)
{
	if (addr > 160) 
		return NULL;

	return &ss_dev_tab[addr + 160];
}


struct microjs_script {
	int x;
};

void sim_js_exec(struct ss_device * dev, struct db_dev_model * model, 
				 struct microjs_script * cmd)
{

}

/* simulate a custom sensor */
void sensor_sim_custom(struct ss_device * dev, 
					   struct db_dev_model * model, uint32_t ctl)
{
	struct cmd_list * lst;

	/* Remote test */
	if ((ctl & 0x2d) == 0x00) {
		DCC_LOG(LOG_TRACE, "Remote test enabled");
		dev->tst = 1;
		dev->pw2 = device_db_pw2_lookup(model, 1, dev->tbias);
		dev->pw4 = device_db_pw4_lookup(model, 3, dev->tbias);
	} else if ((ctl & 0x2d) == 0x00) {
		DCC_LOG(LOG_TRACE, "Remote test disabled");
		dev->tst = 0;
		dev->pw2 = device_db_pw2_lookup(model, 0, dev->tbias);
		dev->pw4 = device_db_pw4_lookup(model, 0, dev->tbias);
	}

	/* execute commands from the device model */
	if ((lst = model->cmd) != NULL) {
		int i;
		for (i = 0; i < lst->cnt; ++i) {
			struct cmd_entry * cmd = &lst->cmd[i];
			if ((ctl & cmd->seq.msk) == cmd->seq.val) {
				DCC_LOG1(LOG_TRACE, "CMD[%d]", i);
				sim_js_exec(dev, model, cmd->script);
			}
		}
	}
}


/* simulate a photodetector smoke sensor */
void sensor_sim_photo(struct ss_device * dev, 
					  struct db_dev_model * model, uint32_t ctl)
{
	DCC_LOG(LOG_TRACE, "...");

	/* Remote test */
	if ((ctl & 0x2d) == 0x00) {
		DCC_LOG(LOG_TRACE, "Remote test enabled");
		dev->tst = 1;
		dev->pw2 = device_db_pw2_lookup(model, 1, dev->tbias);
		dev->pw4 = device_db_pw4_lookup(model, 3, dev->tbias);
	} else if ((ctl & 0x2d) == 0x00) {
		DCC_LOG(LOG_TRACE, "Remote test disabled");
		dev->tst = 0;
		dev->pw2 = device_db_pw2_lookup(model, 0, dev->tbias);
		dev->pw4 = device_db_pw4_lookup(model, 0, dev->tbias);
	}
}

/* simulate a ion smoke detector */
void sensor_sim_ion(struct ss_device * dev, 
					struct db_dev_model * model, uint32_t ctl)
{
	DCC_LOG(LOG_TRACE, "...");

	/* Remote test */
	if ((ctl & 0x2d) == 0x00) {
		DCC_LOG(LOG_TRACE, "Remote test enabled");
		dev->tst = 1;
		dev->pw2 = device_db_pw2_lookup(model, 1, dev->tbias);
		dev->pw4 = device_db_pw4_lookup(model, 3, dev->tbias);
	} else if ((ctl & 0x2d) == 0x00) {
		DCC_LOG(LOG_TRACE, "Remote test disabled");
		dev->tst = 0;
		dev->pw2 = device_db_pw2_lookup(model, 0, dev->tbias);
		dev->pw4 = device_db_pw4_lookup(model, 0, dev->tbias);
	}
}

/* simulate a heat detector sensor */
void sensor_sim_heat(struct ss_device * dev, 
					 struct db_dev_model * model, uint32_t ctl)
{
	DCC_LOG(LOG_TRACE, "...");

	/* Remote test */
	if ((ctl & 0x2d) == 0x00) {
		DCC_LOG(LOG_TRACE, "Remote test enabled");
		dev->tst = 1;
		dev->pw2 = device_db_pw2_lookup(model, 1, dev->tbias);
		dev->pw4 = device_db_pw4_lookup(model, 3, dev->tbias);
	} else if ((ctl & 0x2d) == 0x00) {
		DCC_LOG(LOG_TRACE, "Remote test disabled");
		dev->tst = 0;
		dev->pw2 = device_db_pw2_lookup(model, 0, dev->tbias);
		dev->pw4 = device_db_pw4_lookup(model, 0, dev->tbias);
	}
}

/* simulate an Acclimate Photoelectric Smoke Sensor */
void sensor_sim_acclimate(struct ss_device * dev, 
						  struct db_dev_model * model, uint32_t ctl)
{
	DCC_LOG(LOG_TRACE, "...");

	/* Remote test */
	if ((ctl & 0x2d) == 0x00) {
		DCC_LOG(LOG_TRACE, "Remote test enabled");
		dev->tst = 1;
		dev->pw2 = device_db_pw2_lookup(model, 1, dev->tbias);
		dev->pw4 = device_db_pw4_lookup(model, 3, dev->tbias);
	} else if ((ctl & 0x2d) == 0x00) {
		DCC_LOG(LOG_TRACE, "Remote test disabled");
		dev->tst = 0;
		dev->pw2 = device_db_pw2_lookup(model, 0, dev->tbias);
		dev->pw4 = device_db_pw4_lookup(model, 0, dev->tbias);
	}
}

/* simulate an Beam Smoke Sensor */
void sensor_sim_beam(struct ss_device * dev, 
						  struct db_dev_model * model, uint32_t ctl)
{
	DCC_LOG(LOG_TRACE, "...");

	/* Remote test */
	if ((ctl & 0x2d) == 0x00) {
		DCC_LOG(LOG_TRACE, "Remote test enabled");
		dev->tst = 1;
		dev->pw2 = device_db_pw2_lookup(model, 1, dev->tbias);
		dev->pw4 = device_db_pw4_lookup(model, 3, dev->tbias);
	} else if ((ctl & 0x2d) == 0x00) {
		DCC_LOG(LOG_TRACE, "Remote test disabled");
		dev->tst = 0;
		dev->pw2 = device_db_pw2_lookup(model, 0, dev->tbias);
		dev->pw4 = device_db_pw4_lookup(model, 0, dev->tbias);
	} else if ((ctl & 0x5b6d) == 0x0844) {
		/* Reset */
		DCC_LOG(LOG_TRACE, "Reset");
	} else if ((ctl & 0x5b6d) == 0x0244) {
		DCC_LOG(LOG_TRACE, "Obscuration test enabled");
	}
}

/* simulate a COPTIR Smoke Detector Sensor */
void sensor_sim_coptir(struct ss_device * dev, 
					   struct db_dev_model * model, uint32_t ctl)
{
	DCC_LOG(LOG_TRACE, "...");

	/* Remote test */
	if ((ctl & 0x2d) == 0x00) {
		DCC_LOG(LOG_TRACE, "Remote test enabled");
		dev->tst = 1;
		dev->pw2 = device_db_pw2_lookup(model, 1, dev->tbias);
		dev->pw4 = device_db_pw4_lookup(model, 3, dev->tbias);
	} else if ((ctl & 0x2d) == 0x00) {
		DCC_LOG(LOG_TRACE, "Remote test disabled");
		dev->tst = 0;
		dev->pw2 = device_db_pw2_lookup(model, 0, dev->tbias);
		dev->pw4 = device_db_pw4_lookup(model, 0, dev->tbias);
	}
}

/* simulate a custom module */
void module_sim_custom(struct ss_device * dev, 
					   struct db_dev_model * model, uint32_t ctl)
{
	struct cmd_list * lst;

	/* execute commands from the device model */
	if ((lst = model->cmd) != NULL) {
		int i;
		for (i = 0; i < lst->cnt; ++i) {
			struct cmd_entry * cmd = &lst->cmd[i];
			if ((ctl & cmd->seq.msk) == cmd->seq.val) {
				DCC_LOG1(LOG_TRACE, "CMD[%d]", i);
				sim_js_exec(dev, model, cmd->script);
			}
		}
	}
}

/* simulate a relay module */
void module_sim_relay(struct ss_device * dev, 
					  struct db_dev_model * model, uint32_t ctl)
{
	DCC_LOG(LOG_TRACE, "...");
}

/* simulate a supervised cntrol module */
void module_sim_control(struct ss_device * dev, 
						struct db_dev_model * model, uint32_t ctl)
{
	DCC_LOG(LOG_TRACE, "...");
}

/* simulate a monitor module */
void module_sim_monitor(struct ss_device * dev, 
						struct db_dev_model * model, uint32_t ctl)
{
	DCC_LOG(LOG_TRACE, "...");
}

/* simulate a mini-module */
void module_sim_mini(struct ss_device * dev, 
					 struct db_dev_model * model, uint32_t ctl)
{
	DCC_LOG(LOG_TRACE, "...");
}

/* simulate a 2 wire module */
void module_sim_2wire(struct ss_device * dev, 
					  struct db_dev_model * model, uint32_t ctl)
{
	DCC_LOG(LOG_TRACE, "...");
}

/* simulate a firefighter telephone module */
void module_sim_phone(struct ss_device * dev, 
					  struct db_dev_model * model, uint32_t ctl)
{
	DCC_LOG(LOG_TRACE, "...");
}

#define SIM_MODEL_NAME_MAX 12

struct sim_model {
	const char name[SIM_MODEL_NAME_MAX]; 
	void (* run)(struct ss_device * dev, 
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
	[12] = { .name = "2wire", .run = module_sim_2wire },
	[13] = { .name = "phone", .run = module_sim_phone }
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
	uint32_t event;
	struct ss_device * dev;
	struct db_dev_model * model;
	uint32_t ctl;

	for (;;) {
		event = slcdev_event_wait();
		dev = slcdev_drv.dev;
		/* get the model for this device */
		model = db_dev_model_by_index(dev->model);
		ctl = dev->ctls;

		if (event & SLC_EV_TRIG) {
			DCC_LOG1(LOG_TRACE, "trigger %d", dev->addr);
			led_flash(0, 64);
		}

		if ((event & SLC_EV_SIM) && (model != NULL)) {
			const struct sim_model * sim;
			sim = &sim_model_lut[model->sim];

			DCC_LOG1(LOG_TRACE, "simulation %d", dev->addr);

			/* Poll LED state */
			if ((ctl & 0x4) == 0) {
				dev->led = 1;
				led_on(1);
			} else if ((ctl & 0x5) == 4) {
				dev->led = 0;
				led_off(1);
			} else if ((ctl & 0x5) == 5) {
				led_flash(1, 64);
			}

			sim->run(dev, model, ctl);
		}
	}
}

