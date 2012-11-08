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
 * @file cmd_netstat.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <tcpip/ethif.h>
#include <tcpip/etharp.h>
#include <tcpip/route.h>
#include <tcpip/pcb.h>
#include <tcpip/tcp.h>
#include <tcpip/udp.h>

#include <sys/shell.h>

static const char * const tcp_state[11] = {
	"CLOSED", "LISTEN", "SYN_SENT", "SYN_RCVD", "ESTABLISHED",
	"CLOSE_WAIT", "FIN_WAIT_1", "CLOSING", "LAST_ACK",
	"FIN_WAIT_2", "TIME_WAIT"
};

static char * sock2str(char * s, uint32_t addr, uint16_t port) 
{
	char ip[16];

	if ((port) && (addr))
		sprintf(s, "%s:%d", inet_ntop(AF_INET, (void *)&addr, ip, 16), 
				htons(port));
	else
		if (addr)
			sprintf(s, "%s:*", inet_ntop(AF_INET, (void *)&addr, ip, 16));
		else
			if (port)
				sprintf(s, "*:%d", htons(port));
			else
				strcpy(s, "*:*");
	return s;
}

static int show_tcp_pcb(struct tcp_pcb * tp, FILE * f)
{
	char buf1[24];	
	char buf2[24];	

	fprintf(f, "tcp    %-22s %-22s %s\n", 
				sock2str(buf1, tp->t_laddr, tp->t_lport),
				sock2str(buf2, tp->t_faddr, tp->t_fport),
				tcp_state[(int)tp->t_state]);

	return 0;
}

static int show_udp_pcb(struct pcb * pb, FILE * f)
{
	char buf1[24];	
	char buf2[24];	

	fprintf(f, "udp    %-22s %-22s\n", 
				sock2str(buf1, pb->inp_laddr, pb->inp_lport), 
				sock2str(buf2, pb->inp_faddr, pb->inp_fport));

	return 0;
}
				
int cmd_netstat(FILE *f, int argc, char ** argv)
{

	fprintf(f, "\nActive Internet connections (servers and established)\n");
	fprintf(f, "Proto  Local Address"
				"          Foreign Address        State\n");

	tcp_enum((void *)show_tcp_pcb, (void *)f);

	udp_enum((void *)show_udp_pcb, (void *)f);
	return 0;
}

