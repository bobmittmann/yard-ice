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
 * @file tcp_echo.c
 * @brief YARD-ICE application main
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/dcclog.h>
#include <tcpip/tcp.h>
#include <netinet/in.h>

#include <thinkos.h>

#include <netinet/in.h>

int tcp_echo_task(void * arg)
{
	struct tcp_pcb * svc;
	struct tcp_pcb * tp;
	uint8_t buf[128];
	int port = 23;
	int n;

	svc = tcp_alloc();

	tcp_bind(svc, INADDR_ANY, htons(port));

	if (tcp_listen(svc, 1) != 0) {
		DCC_LOG(LOG_ERROR, "Can't register the TCP listner!");
		return -1;
	}

	for (;;) {
		if ((tp = tcp_accept(svc)) == NULL) {
			DCC_LOG(LOG_ERROR, "tcp_accept() failed!");
			return -1;
		}

		DCC_LOG(LOG_TRACE, "Connection accepted.");

		while ((n = tcp_recv(tp, buf, 128)) > 0)  {
			if ((n = tcp_send(tp, buf, n, 0)) < 0) {
				break;
			}
		} 

		tcp_close(tp);
		DCC_LOG(LOG_TRACE, "Connection closed.");
	}

	return 0;
}

uint32_t tcp_echo_stack[256];

int tcp_echo_start(void)
{
	return thinkos_thread_create((void *)tcp_echo_task, (void *)NULL,
								 tcp_echo_stack, sizeof(tcp_echo_stack) |
								 THINKOS_OPT_PRIORITY(4) | THINKOS_OPT_ID(8));

}


