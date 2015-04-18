/* 
 * File:	 usb-cdc.c
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
#include <arch/cortex-m3.h>
#include <sys/param.h>
#include <sys/serial.h>
#include <string.h>
#include <stdbool.h>
#include <sys/usb-cdc.h>

#include <sys/dcclog.h>

#include "monitor.h"


const char monitor_banner[] = "\r\n\r\n"
"-------------------------------------------------------------------------\r\n"
"- ThinkOS Monitor\r\n"
"-------------------------------------------------------------------------\r\n"
"\r\n";

void __attribute__((noreturn)) monitor_task(struct thinkos_dmon * mon) 
{
	void * comm = mon->comm;
	char buf[64];
	int n;
	int c;

	DCC_LOG(LOG_TRACE, "Monitor start...");
	dmon_comm_connect(comm);

	dmon_comm_send(comm, monitor_banner, 
					  sizeof(monitor_banner) - 1);

	for(;;) {
		n = dmon_comm_recv(comm, buf, 64);
		DCC_LOG1(LOG_TRACE, "n=%d...", n);
		(void)n;
		if (n == 0)
			continue;

		dmon_comm_send(comm, buf, n);
		c = buf[0];
		if (c == 'i') {
			dmon_signal(DMON_START);
		}
	}
}


void monitor_init(void)
{
	void * comm;
	
	comm = usb_mon_init(&stm32f_otg_fs_dev, 
						cdc_acm_def_str, 
						cdc_acm_def_strcnt);

	thinkos_dmon_init(comm, monitor_task);
}

