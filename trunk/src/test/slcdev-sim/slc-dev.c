#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/delay.h>
#include <sys/serial.h>
#include <sys/dcclog.h>
#include <sys/shell.h>

#include <thinkos.h>

#include "board.h"
#include "isink.h"
#include "slcdev.h"
#include "serdrv.h"
#include "xmodem.h"

#define VERSION_NUM "0.2"
#define VERSION_DATE "Jul, 2014"

extern const struct shell_cmd cmd_tab[];

const char * shell_greeting(void) 
{
	return "\n"
	"Clip Device Simulator " VERSION_NUM " - " VERSION_DATE "\n"
	"(c) Copyright 2014 - Mircom Group (bmittmann@mircomgroup.com)\n\n";
}

const char * shell_prompt(void)
{
	return "[DEV]$ ";
}


void __attribute__((noreturn)) io_event_task(void)
{
	uint32_t event;
	unsigned int sw;

	for (;;) {
	
		event = io_event_wait();

		sw = io_drv.sw;

		if (event & (1 << EV_SW1)) {
			io_event_clr(EV_SW1);  

			switch (sw & SW1_MSK) {
			case SW1_OFF:
				DCC_LOG(LOG_TRACE, "SW1 OFF");
				led_off(LED3);
				led_off(LED4);
				break;

			case SW1_A:
				DCC_LOG(LOG_TRACE, "SW1 A");
				led_on(LED3);
				break;

			case SW1_B:
				DCC_LOG(LOG_TRACE, "SW1 B");
				led_on(LED4);
			}
		}

		if (event & (1 << EV_SW2)) {
			DCC_LOG(LOG_TRACE, "SW2");
			io_event_clr(EV_SW2);  

			switch (sw & SW2_MSK) {

			case SW2_OFF:
				DCC_LOG(LOG_TRACE, "SW2 OFF");
				led_off(LED5);
				led_off(LED6);
				break;

			case SW2_A:
				DCC_LOG(LOG_TRACE, "SW2 A");
				led_on(LED5);
				break;

			case SW2_B:
				DCC_LOG(LOG_TRACE, "SW2 B");
				led_on(LED6);
			}
		}

		if (event & (1 << EV_ADDR)) {
			DCC_LOG1(LOG_TRACE, "ADDR=%d", io_drv.addr);
			io_event_clr(EV_ADDR);  
			trig_addr_set(io_drv.addr);
		}
	}
}

void __attribute__((noreturn)) sim_event_task(void)
{
	uint32_t event;

	for (;;) {
		event = slcdev_event_wait();
		DCC_LOG1(LOG_TRACE, "event=%d", event);
		switch (event) {
		case SLC_EV_TRIG:
			DCC_LOG(LOG_TRACE, "trigger");
			led_flash(0, 64);
			break;
		case SLC_EV_SIM:
			DCC_LOG(LOG_TRACE, "simulation");
			led_flash(1, 64);
			break;
		}
	}
}

uint32_t __attribute__((aligned(8))) io_event_stack[32];
uint32_t __attribute__((aligned(8))) sim_event_stack[32];

extern const uint8_t device_db_js[];
extern unsigned int sizeof_device_db_js;

const char str_loop_config_xfer[] = "\r\nLoop config Xfer...\r\n";
const char str_xmodem_err[] = "\r\nXmodem transfer failed.\r\n";
const char str_xmodem_done[] = "\r\nXmodem transfer done.\r\n";

int main(int argc, char ** argv)
{
	struct serdrv * sdrv;
	FILE * f;

	DCC_LOG_INIT();
//	DCC_LOG_CONNECT();

	DCC_LOG(LOG_TRACE, "1. cm3_udelay_calibrate()");
	cm3_udelay_calibrate();

	DCC_LOG(LOG_TRACE, "2. thinkos_init()");
	thinkos_init(THINKOS_OPT_PRIORITY(8) | THINKOS_OPT_ID(7));

	DCC_LOG(LOG_TRACE, "3. ...");
	thinkos_sleep(1000);

	DCC_LOG(LOG_TRACE, "4. io_init()");
	io_init();

	DCC_LOG(LOG_TRACE, "5. isink_init()");
	isink_init();

	/* perform a lamp test while the current sink 
	   negative voltage stabilizes */
	lamp_test();

	slcdev_init();
	isink_mode_set(ISINK_CURRENT_NOM | ISINK_RATE_FAST);
	isink_slewrate_set(1000);

//	device_db_init();

	thinkos_thread_create((void *)io_event_task, (void *)NULL,
						  io_event_stack, sizeof(io_event_stack),
						  THINKOS_OPT_PRIORITY(2) | THINKOS_OPT_ID(2));

	thinkos_thread_create((void *)sim_event_task, (void *)NULL,
						  sim_event_stack, sizeof(sim_event_stack),
						  THINKOS_OPT_PRIORITY(1) | THINKOS_OPT_ID(1));

	/* initialize serial driver */
	sdrv = serdrv_init(115200);

	/* initialize tty driver */
	f = serdrv_tty_fopen(sdrv);

	/* initialize stdio */
	stderr = f;
	stdin = f;
	stdout = f;

	shell(f, shell_prompt, shell_greeting, cmd_tab);

	DCC_LOG(LOG_WARNING, "bye bye !!!!!!!!!!");

	return 0;
}

