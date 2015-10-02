#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/delay.h>
#include <sys/serial.h>
#include <sys/dcclog.h>
#include <sys/shell.h>
#include <sys/null.h>

#include "config.h"
#include "board.h"
#include "isink.h"
#include "slcdev.h"
#include "simrpc_clnt.h"
#include "io.h"

void __attribute__((noreturn)) sim_event_task(void);

void __attribute__((noreturn)) simlnk_recv_task(void);

void __attribute__((noreturn)) js_runtime_task(void);

//uint32_t __attribute__((aligned(8))) sim_event_stack[96];
uint32_t __attribute__((aligned(8))) simlnk_stack[512 + 256];

void isink_test(void);
void js_runtime_init(void);

int __attribute__((noreturn)) app_main(const char * mode)
{
	FILE * f;

	DCC_LOG1(LOG_WARNING, "mode=%p.", mode);

//	f = null_fopen(NULL);
	f = simrpc_stdout_fopen();

	/* initialize STDIO */
	stdout = f;
	stderr = f;
	stdin = f;

	io_init();

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
	slcdev_sim_init();

	/* initializes database */
	device_db_init();

	if (strcmp(mode, "safe") == 0) {
		DCC_LOG(LOG_WARNING, "safe mode!");
	} else {
		/* load configuration */
		config_load();
		/* start simulation */
		slcdev_sim_resume();
	}

#if 0
	/* create a thread to handle simulation events. This is the 
	   actual simulation loop. The events are generated by the 
	   SLC device driver according to the configuration devices which 
	   are current enabled. */
	thinkos_thread_create((void *)simlnk_event_task, (void *)NULL,
						  sim_event_stack, sizeof(sim_event_stack) | 
						  THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	/* stay forever in the SIMLNK task */
	simlnk_task();
#endif

	js_runtime_init();

	/* create a thread to handle simulation link incoming requests */
	thinkos_thread_create((void *)simlnk_recv_task, (void *)NULL,
						  simlnk_stack, sizeof(simlnk_stack) | 
						  THINKOS_OPT_PRIORITY(2) | THINKOS_OPT_ID(2));

	/* stay forever in the simulator task */
	sim_event_task();
}

int main(void *) __attribute__((weak, alias("app_main")));

