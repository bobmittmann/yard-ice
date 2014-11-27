#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/delay.h>
#include <sys/serial.h>
#include <sys/dcclog.h>
#include <sys/shell.h>

#include <thinkos.h>

#include "config.h"
#include "board.h"
#include "isink.h"
#include "slcdev.h"
#include "serdrv.h"
#include "xmodem.h"

#define VERSION_NUM "0.3"
#define VERSION_DATE "Nov, 2014"

extern const struct shell_cmd cmd_tab[];

const char * shell_greeting(void) 
{
	return "\n"
	"SLC Device Simulator " VERSION_NUM " - " VERSION_DATE "\n"
	"(c) Copyright 2014 - Mircom Group\n\n";
}

const char * shell_prompt(void)
{
	return "[DEV]$ ";
}

uint32_t __attribute__((aligned(8))) io_event_stack[24];
uint32_t __attribute__((aligned(8))) sim_event_stack[96];

#if THINKOS_STDERR_FAULT_DUMP
const struct file stm32_uart_file = {
	.data = STM32_USART2, 
	.op = &stm32_usart_fops 
};
#endif

int js(FILE * f, char * script, unsigned int len);

char hist_buf[5 + SHELL_HISTORY_MAX * SHELL_LINE_MAX];

int __attribute__((noreturn)) main(int argc, char ** argv)
{
	char line[SHELL_LINE_MAX];
	struct cmd_history * history;
	struct serdrv * sdrv;
	char * stat;
	char * cp;
	FILE * f;

	DCC_LOG_INIT();
//	DCC_LOG_CONNECT();

	DCC_LOG(LOG_INFO, "1. cm3_udelay_calibrate()");
	/* calibrate the delay loop fo udelay() and friends. */
	cm3_udelay_calibrate();

	DCC_LOG(LOG_INFO, "2. thinkos_init()");
	/* initialize the operating system */
	thinkos_init(THINKOS_OPT_PRIORITY(2) | THINKOS_OPT_ID(2));

#ifndef DEBUG
	/* wait for the device to charge a little bit before enabling
	   peripherals */
	thinkos_sleep(2000);
#endif

	DCC_LOG(LOG_INFO, "4. io_init()");
	/* initialize IO pins */
	io_init();

	DCC_LOG(LOG_INFO, "5. isink_init()");
	/* enable the current sink driver. This will start the negative 
	   voltage power supply */
	isink_init();

	/* create a thread to handle IO events like switches and
	   address selection. This thread also controls the LEDs flashing. */
	thinkos_thread_create((void *)io_event_task, (void *)NULL,
						  io_event_stack, sizeof(io_event_stack) |
						  THINKOS_OPT_PRIORITY(1) | THINKOS_OPT_ID(1));

	/* perform a lamp test while the current sink 
	   negative voltage stabilizes */
	lamp_test();

	/* configure the current sink driver to default values */
	isink_mode_set(ISINK_CURRENT_NOM | ISINK_RATE_FAST);

	/* initilice the SLC device driver */
	slcdev_init();

	/* initialize constat string buffer */
	const_strbuf_init();

	/* initializes database */
	device_db_init();

	/* load configuration */
	config_load();

	/* create a thread to handle simulation events. This is the 
	   actual simulation loop. The events are generated by the 
	   SLC device driver according to the configuration devices which 
	   are current enabled. */
	thinkos_thread_create((void *)sim_event_task, (void *)NULL,
						  sim_event_stack, sizeof(sim_event_stack) | 
						  THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	/* initialize serial driver */
	sdrv = serdrv_init(115200);

	/* initialize serial TTY driver */
	f = serdrv_tty_fopen(sdrv);

	/* initialize STDIO */
	stdout = f;
#if THINKOS_STDERR_FAULT_DUMP
	stderr = (struct file *)&stm32_uart_file;
#else
	stderr = f;
#endif

	/* start simulation */
	slcdev_event_raise(SLC_EV_SIM_START);

	/* main loop */
	for (;;) {
		history = history_init(hist_buf, sizeof(hist_buf), SHELL_LINE_MAX);
		fprintf(f, shell_greeting());

		/* start a shell on the serial TTY */
		for (;;) {
			fprintf(f, "%s", shell_prompt());

			if (history_readline(history, f, line, SHELL_LINE_MAX) == NULL)
				break;

			if ((cp = shell_stripline(line)) == NULL)
				continue;

			history_add(history, cp);

			/* try to get a command from the line */
			if (cmd_lookup(cmd_tab, cp) == NULL) {
				/* try to interpret a javascript code instead */
				js(f, cp, strlen(cp));
				continue;
			}
		
			DCC_LOG(LOG_TRACE, "shell command ...");

			while ((stat = cmd_get_next(&cp)) != NULL) {
				struct shell_cmd * cmd;
				int ret;

				if ((cmd = cmd_lookup(cmd_tab, stat)) == NULL) {
					fprintf(f, "Command not found!\n");
					break;
				}
				if ((ret = cmd_exec(f, cmd, stat)) < 0) {
					fprintf(f, "Error: %d\n", -ret);
					break;
				}

			}
		}
		DCC_LOG(LOG_WARNING, "bye bye !!!!!!!!!!");
		/* TODO: host protocol... */
		for (;;) {
		}
	}
}

