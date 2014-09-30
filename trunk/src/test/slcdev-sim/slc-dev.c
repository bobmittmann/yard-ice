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

uint32_t __attribute__((aligned(8))) io_event_stack[24];
uint32_t __attribute__((aligned(8))) sim_event_stack[80];

#if THINKOS_STDERR_FAULT_DUMP
const struct file stm32_uart_file = {
	.data = STM32_USART2, 
	.op = &stm32_usart_fops 
};
#endif

int main(int argc, char ** argv)
{
	struct serdrv * sdrv;
	FILE * f;

	DCC_LOG_INIT();
//	DCC_LOG_CONNECT();

	DCC_LOG(LOG_TRACE, "1. cm3_udelay_calibrate()");
	/* calibrate the delay loop fo udelay() and friends. */
	cm3_udelay_calibrate();

	DCC_LOG(LOG_TRACE, "2. thinkos_init()");
	/* initialize the operating system */
	thinkos_init(THINKOS_OPT_PRIORITY(2) | THINKOS_OPT_ID(2));

	DCC_LOG(LOG_TRACE, "3. ...");
#ifndef DEBUG
	/* wait for the device to charge a little bit before enabling
	   peripherals */
	thinkos_sleep(2000);
#endif

	DCC_LOG(LOG_TRACE, "4. io_init()");
	/* initialize IO pins */
	io_init();

	DCC_LOG(LOG_TRACE, "5. isink_init()");
	/* enable the current sink driver. This will start the negative 
	   voltage power supply */
	isink_init();

	/* perform a lamp test while the current sink 
	   negative voltage stabilizes */
	lamp_test();

	/* configure the current sink driver to default values */
	isink_mode_set(ISINK_CURRENT_NOM | ISINK_RATE_FAST);

	/* initilice the SLC device driver */
	slcdev_init();

	/* initialize constat string buffer */
	const_strbuf_init();

	/* initialize database */
	device_db_init();

	/* load configuration */
	config_load();

	/* create a thread to handle IO events like switches and
	   address selection */
	thinkos_thread_create((void *)io_event_task, (void *)NULL,
						  io_event_stack, sizeof(io_event_stack) |
						  THINKOS_OPT_PRIORITY(1) | THINKOS_OPT_ID(1), NULL);

	/* create a thread to handle simulation events. This is the 
	   actual simulation loop. The events are generated by the 
	   SLC device driver according to the configuration devices which 
	   are current enabled. */
	thinkos_thread_create((void *)sim_event_task, (void *)NULL,
						  sim_event_stack, sizeof(sim_event_stack) | 
						  THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0), NULL);

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

	/* start a shell on the serial TTY */
	shell(f, shell_prompt, shell_greeting, cmd_tab);

	DCC_LOG(LOG_WARNING, "bye bye !!!!!!!!!!");

	return 0;
}

