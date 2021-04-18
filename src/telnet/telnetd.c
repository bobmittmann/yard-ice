/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the YARD-ICE.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */

/** 
 * @file yard-ice.c
 * @brief YARD-ICE application main
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 


#include <sys/stm32f.h>
#include <sys/param.h>
#include <sys/file.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <thinkos.h>

#include "telnet_svc.h"
#include <sys/tty.h>
#include <sys/shell.h>
#include <sys/dcclog.h>

#include "command.h"

#define TRACE_LEVEL TRACE_LVL_DBG
#include <trace.h>

int telnetd_task(void * arg)
{
	struct telnet_svc * svc;
	struct tty_dev * tty;
	FILE * f_tty;
	FILE * f_raw;

	DCC_LOG(LOG_TRACE, "telnet_svc_init()");
	svc = telnet_svc_init(23);
	f_raw = telnet_svc_fopen(svc);

	tty = tty_attach(f_raw);
	f_tty = tty_fopen(tty);

	for (;;) {
		shell(f_tty, yard_ice_get_prompt, yard_ice_greeting, yard_ice_cmd_tab);
	}
}

//uint32_t __attribute__((section(".sram1"), aligned(5))) 
uint32_t telnet_shell_stack[1472] __attribute__ ((aligned(64)));

const struct thinkos_thread_inf telnet_shell_inf = {
	.stack_ptr = telnet_shell_stack, 
	.stack_size = sizeof(telnet_shell_stack),
	.priority = 12,
	.thread_id = 12,
	.paused = false,
	.tag = "TNET_SH"
};

int telnet_shell(void)
{
	int th;

	th = thinkos_thread_create_inf((void *)telnetd_task, NULL, 
								   &telnet_shell_inf);

	INF("TELNET shell thread=%d", th);

	return th;
}

