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

#if 0
/*
	"tag": "LED on", "seq": ["0xx"],
	"tag": "LED off", "seq": ["1x0"],
	"tag": "LED blink", "seq": ["1x1"],
	"tag": "Enable Type ID", "seq": ["x1x"],
	"tag": "Remote Test on", "seq": ["0x0 0x0"],
	"tag": "Remote Test off", "seq": ["1x1 1x1"],
						if ((ctl & 0x4) == 0) {
							dev->led = 1;
						} else if ((ctl & 0x5) == 4) {
							dev->led = 0;
						}
*/
#endif

static void simulate(struct ss_device * dev, struct db_dev_model * model)
{
	uint32_t ctl = dev->ctls;

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
		if ((model = db_dev_model_by_index(dev->model)) == NULL) {
			/* default to smoke detector */
			model = db_dev_model_photo();
		}

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

