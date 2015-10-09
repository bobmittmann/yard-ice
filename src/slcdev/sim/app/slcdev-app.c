#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/delay.h>
#include <sys/serial.h>
#include <sys/dcclog.h>
#include <sys/shell.h>
#include <sys/null.h>

#include "config.h"
#include "profclk.h"
#include "board.h"
#include "isink.h"
#include "slcdev.h"
#include "simrpc_clnt.h"
#include "io.h"

void __attribute__((noreturn)) sim_event_task(void);

void isink_test(void);
void js_runtime_init(void);
void simrpc_test_signal(void);
void simlnk_usr_init(void);

void read_fault(void)
{
	volatile uint32_t * ptr = (uint32_t *)(0x0);
	uint32_t x;
	int i;

	for (i = 0; i < (16 << 4); ++i) {
		x = *ptr;
		(void)x;
		ptr += 0x10000000 / (2 << 4);
	}
}

int __attribute__((noreturn)) app_main(int mode)
{
	FILE * f;

	DCC_LOG1(LOG_TRACE, "mode=%d.", mode);

//	f = null_fopen(NULL);
	f = simrpc_stdout_fopen();

	/* initialize stdio */
	stdout = f;
	stderr = f;
	stdin = f;

	/* initialize IO */
	io_init();

	/* initialize profiling clock */
	profclk_init();

	/* enable the current sink driver. This will start the negative 
	   voltage power supply */
	isink_init();

	/* configure the current sink driver to default values */
	isink_mode_set(ISINK_CURRENT_NOM | ISINK_RATE_FAST);
	/* dummy pulse to reset the timer */
	isink_pulse(2, 0);

	/* perform a lamp test while the current sink 
	   negative voltage stabilizes */
	io_lamp_test();

	/* initilice the SLC device driver */
	slcdev_init();

	/* initialize constat string buffer */
	const_strbuf_init();

	/* initializing simulator */
	DCC_LOG(LOG_TRACE, "slcdev_sim_init().");
	slcdev_sim_init();

	/* initializes database */
	DCC_LOG(LOG_TRACE, "device_db_init().");
	device_db_init();

	DCC_LOG(LOG_TRACE, "js_runtime_init().");
	js_runtime_init();

	DCC_LOG(LOG_TRACE, "simlnk_usr_init().");
	simlnk_usr_init();

	if (mode == APP_MODE_NORMAL) {
		DCC_LOG(LOG_TRACE, "normal mode!");
		/* load configuration */
		DCC_LOG(LOG_TRACE, "config_load().");
		config_load();
		/* start simulation */
		DCC_LOG(LOG_TRACE, "slcdev_sim_resume().");
		slcdev_sim_resume();
	} else {
		DCC_LOG(LOG_TRACE, "safe mode!");
	}

	/* stay forever in the simulator task */
	DCC_LOG(LOG_TRACE, "sim_event_task().");
	sim_event_task();
}

int main(void *) __attribute__((weak, alias("app_main")));

