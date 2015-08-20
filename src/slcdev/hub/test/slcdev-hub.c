/* 
 * File:	 usb-test.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2011 Bob Mittmann. All Rights Reserved.
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

#include <sys/stm32f.h>
#include <sys/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <sys/dcclog.h>
#include <sys/usb-cdc.h>
#include <sys/tty.h>
#include <trace.h>

#include <thinkos.h>

#include "simlnk.h"
#include "simrpc.h"
#include "board.h"
#include "net.h"
#include "io.h"

#define VERSION_NUM "0.3"
#define VERSION_DATE "Nov, 2014"

const char * version_str = "SLC Device Hub " \
							VERSION_NUM " - " VERSION_DATE;
const char * copyright_str = "(c) Copyright 2014 - Mircom Group";


int stdio_shell(void);
void env_init(void);

/* -------------------------------------------------------------------------
 * System Supervision
 * ------------------------------------------------------------------------- */

const char * const trace_lvl_tab[] = {
		"   NONE",
		"  ERROR",
		"WARNING",
		"   INFO",
		"  DEBUG"
};

FILE * monitor_stream;
bool monitor_auto_flush;

void __attribute__((noreturn)) supervisor_task(void)
{
	struct trace_entry trace;
	uint32_t clk;

	INF("<%d> started...", thinkos_thread_self());

	/* fall back to stdout */
	monitor_stream = stdout;

	trace_tail(&trace);

	clk = thinkos_clock();
	for (;;) {
		struct timeval tv;
		char s[80];

		/* 8Hz periodic task */
		clk += 125;
		thinkos_alarm(clk);

		while (trace_getnext(&trace, s, sizeof(s)) >= 0) {
			trace_ts2timeval(&tv, trace.dt);
			fprintf(monitor_stream, "%s %2d.%06d: %s\n", 
					trace_lvl_tab[trace.ref->lvl],
					(int)tv.tv_sec, (int)tv.tv_usec, s);
		}

		if (monitor_auto_flush)
			trace_flush(&trace);
	}
}


uint32_t supervisor_stack[128];

const struct thinkos_thread_inf supervisor_inf = {
	.stack_ptr = supervisor_stack,
	.stack_size = sizeof(supervisor_stack),
	.priority = 32,
	.thread_id = 31,
	.paused = false,
	.tag = "SUPV"
};

void supervisor_init(void)
{
	trace_init();

	thinkos_thread_create_inf((void *)supervisor_task, (void *)NULL,
							  &supervisor_inf);
}

void __attribute__((noreturn)) stdio_shell_task(void * arg)
{
	DCC_LOG1(LOG_TRACE, "thread=%d", thinkos_thread_self());

	DBG("<%d> started...", thinkos_thread_self());
	
	for (;;) {
		printf("\n...\n");
		DCC_LOG(LOG_TRACE, "14. usb_shell()");
		stdio_shell();
	}
}

uint32_t __attribute__((aligned(8))) stdio_shell_stack[1024 + 256];

void stdio_init(void)
{
	usb_cdc_class_t * cdc;
	struct tty_dev * tty;
	FILE * f_tty;
	FILE * f_raw;

	DCC_LOG(LOG_TRACE, "13. usb_cdc_init()");
	usb_cdc_sn_set(*((uint64_t *)STM32F_UID));

	cdc = usb_cdc_init(&stm32f_otg_fs_dev, 
					   cdc_acm_def_str, 
					   cdc_acm_def_strcnt);

	f_raw = usb_cdc_fopen(cdc);
	tty = tty_attach(f_raw);
	f_tty = tty_fopen(tty);

	stderr = f_tty;
	stdout = f_tty;
	stdin = f_tty;

	thinkos_thread_create((void *)stdio_shell_task, (void *)cdc,
						  stdio_shell_stack, sizeof(stdio_shell_stack) | 
						  THINKOS_OPT_PRIORITY(2) | THINKOS_OPT_ID(2));
}

void board_init(void);

int main(int argc, char ** argv)
{
	int i;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	DCC_LOG(LOG_TRACE, "1. board_init()");
	board_init();

	DCC_LOG(LOG_TRACE, "2. cm3_udelay_calibrate()");
	cm3_udelay_calibrate();

	DCC_LOG(LOG_TRACE, "3. env_init().");
	stm32f_nvram_env_init();

	DCC_LOG(LOG_TRACE, "4. thinkos_init()");
	thinkos_init(THINKOS_OPT_PRIORITY(8) | THINKOS_OPT_ID(7));

	DCC_LOG(LOG_TRACE, "5. iodrv_init()");
	iodrv_init();

	DCC_LOG(LOG_TRACE, "5. stdio_init()");
	stdio_init();

	DCC_LOG(LOG_TRACE, "6. trace_init()");
	trace_init();

	DCC_LOG(LOG_TRACE, "9. supervisor_init()");
	supervisor_init();

	DCC_LOG(LOG_TRACE, "10. simrpc_init()");
	simrpc_init();

	led_set_rate(LED_X1, RATE_2BPS);

	for (i = 0; i < 10000; ++i) {
		char s[128];

//		DCC_LOG(LOG_TRACE, "11. simrpc_mem_lock()...");
		simrpc_mem_lock(1, 0x08010000, 8192);

		DCC_LOG(LOG_TRACE, "RPC memory test.");

		if (simrpc_mem_erase(1, 0, 2048) < 0) {
			DCC_LOG(LOG_WARNING, "simrpc_mem_erase() failed!");
		}

		if (simrpc_mem_write(1, "Hello world!", 14) < 0) {
			DCC_LOG(LOG_WARNING, "simrpc_mem_write() failed!");
		}

		if (simrpc_mem_seek(1, 0) < 0) {
			DCC_LOG(LOG_WARNING, "simrpc_mem_seek() failed!");
		}

		if (simrpc_mem_read(1, s, 14) < 0) {
			DCC_LOG(LOG_WARNING, "simrpc_mem_write() failed!");
		}

		DCC_LOGSTR(LOG_TRACE, "'%s'", s);

//		thinkos_sleep(100);
//		simrpc_mem_lock(2, 0x08000000, 1024);

//		thinkos_sleep(1000);

		simrpc_mem_unlock(1, 0x08010000, 8192);
//		thinkos_sleep(100);
//		simrpc_mem_unlock(2, 0x08000000, 1024);
		thinkos_sleep(1000);
	}

	for (i = 0; i < 100000000; ++i) {
//		DCC_LOG(LOG_TRACE, "11. simlink_send...");
//		simlnk_send(simlnk[0], "Hello world\n", 12); 
//		simlnk_send(simlnk[1], "Hello world\n", 12); 
//		simlnk_send(simlnk[2], "Hello world\n", 12); 
//		simlnk_send(simlnk[3], "Hello world\n", 12); 
//		simlnk_send(simlnk[4], "Hello world\n", 12); 
//		thinkos_sleep(1000);
		simrpc_mem_lock(0, 0x08000000, 1024);
	}

	DCC_LOG(LOG_TRACE, "10. net_init()");
	net_init();

	return 0;
}

