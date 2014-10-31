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

#include "telnet_svc.h"
#include <sys/tty.h>
#include <sys/shell.h>

#include <sys/os.h>

#include <trace.h>
#include "command.h"

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

int telnet_shell(void * stack_buf, int stack_size)
{
	int th;

	th = __os_thread_create((void *)telnetd_task, NULL, 
						   stack_buf, stack_size, 0); 

	tracef("TELNET shell thread=%d", th);

	return th;
}

