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

void cmd_exec(struct ss_device * dev, struct db_dev_model * model, 
			  struct microjs_script * cmd)
{

}

static void simulate(struct ss_device * dev, struct db_dev_model * model)
{
	uint32_t ctl = dev->ctls;
	struct cmd_list * lst;

	if (model == NULL) {
		DCC_LOG(LOG_TRACE, "Invalid model");
		return;
	}

	/* LED state */
	if ((ctl & 0x4) == 0) {
		dev->led = 1;
		led_on(1);
	} else if ((ctl & 0x5) == 4) {
		dev->led = 0;
		led_off(1);
	} else if ((ctl & 0x5) == 5) {
		led_flash(1, 64);
	}

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
				cmd_exec(dev, model, cmd->script);
			}
		}
	}
}

void __attribute__((noreturn)) sim_event_task(void)
{
	uint32_t event;
	struct ss_device * dev;
	struct db_dev_model * model;

	for (;;) {
		event = slcdev_event_wait();
		dev = slcdev_drv.dev;
		/* get the model for this device */
		model = db_dev_model_by_index(dev->model);

		if (event & SLC_EV_TRIG) {
			DCC_LOG(LOG_TRACE, "trigger");
			led_flash(0, 64);
		}

		if (event & SLC_EV_SIM) {
			DCC_LOG1(LOG_TRACE, "simulation %d", dev->addr);
			simulate(dev, model);
		}
	}
}

