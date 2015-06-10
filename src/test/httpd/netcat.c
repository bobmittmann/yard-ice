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
#include <sys/shell.h>
#include <sys/dcclog.h>
#include <tcpip/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <thinkos.h>

const char cmd_netcat_usage[] = 
	"usage: nc HOST PORT\n";

#define BUF_LEN 128

int cmd_netcat(FILE * f, int argc, char ** argv)
{
	uint8_t buf[BUF_LEN];
	struct tcp_pcb * tp;
	in_addr_t host;
	int port;
	int n;

	if (argc < 3) {
		fprintf(f, "nc - open a TCP connection to HOST at PORT\n");
		fprintf(f, cmd_netcat_usage);
		return SHELL_ERR_ARG_MISSING;
	}

	if (argc > 3) {
		fprintf(f, cmd_netcat_usage);
		return SHELL_ERR_EXTRA_ARGS;
	}

	if (inet_aton(argv[1], (struct in_addr *)&host) == 0) {
		fprintf(f, "ip address invalid.\n");
		return SHELL_ERR_ARG_INVALID;
	}
	
	port = strtol(argv[2], NULL, 0);

	if ((tp = tcp_alloc()) == NULL) {
		fprintf(f, "can't allocate socket!\n");
		return SHELL_ERR_GENERAL;
	}

	if (tcp_connect(tp, host, htons(port)) < 0) {
		fprintf(f, "can't connect to host!\n");
		return SHELL_ERR_GENERAL;
	}

	while ((n = tcp_recv(tp, buf, BUF_LEN)) > 0)  {
		buf[n] = '\0';
		fprintf(f, (char *)buf);
	} 

	tcp_close(tp);

	return 0;
}
