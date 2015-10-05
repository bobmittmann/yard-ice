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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <sys/delay.h>
#include <sys/console.h>
#include <sys/tty.h>
#include <sys/null.h>
#include <trace.h>

#include <thinkos.h>

#include <tcpip/ethif.h>
#include <tcpip/route.h>
#include <tcpip/loopif.h>
#include <tcpip/udp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>

#include "simlnk.h"
#include "simrpc.h"
#include "board.h"
#include "io.h"


void __attribute__((noreturn)) app_default(void * arg)
{
	thinkos_udelay_factor(&udelay_factor);

	iodrv_init();

	trace_init();

	simrpc_init();

	for (;;) {
		thinkos_sleep(60000);
	}
}

