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
 * @file cmd_tcpstat.c
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
#include <sys/tcp.h>
#include <sys/mbuf.h>

#include <sys/shell.h>

static const char * const tcp_state[11] = {
	"CLOSED", "LISTEN", "SYN_SENT", "SYN_RCVD", "ESTABLISHED",
	"CLOSE_WAIT", "FIN_WAIT_1", "CLOSING", "LAST_ACK",
	"FIN_WAIT_2", "TIME_WAIT"
};

int list_check(FILE * f, struct mlink_list * __list)
{
	struct mlink * p;
	struct mlink * q;
	struct tcp_pcb * pcb;
	int n;

	q = (struct mlink *)&__list->first;

	n = 0;
	fprintf(f, " [%05x]->", (int)q);
	while ((p = q->next)) {
		pcb = (struct tcp_pcb *)(q + 1);
		fprintf(f, "%05x->", (int)p);
		q = p;
		n++;
	}
	fprintf(f, "NULL (%d)\n", n);

	if ((struct mlink *)__list->last != q) {
		fprintf(f, "\n#ERROR: last=%05x != q=%05x\n\n", 
			   (int)__list->last, (int)q);
		return -1;
	}

	return 0;
}

void tcp_check(FILE * f)
{
	fprintf(f, "MBUS: free:%d used:%d max:%d\n", 
			mbuf_avail(), mbuf_used(), mbuf_max());
	fprintf(f, "Closed list:\n");
	list_check(f, &__tcp__.closed);
	fprintf(f, "Listen list:\n");
	list_check(f, &__tcp__.listen);
	fprintf(f, "Active list:\n");
	list_check(f, &__tcp__.active);
}

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

int cmd_tcpstat(FILE *f, int argc, char ** argv)
{
	fprintf(f, "\nTCP stack check...\n");

	tcp_check(f);

	fprintf(f, "\nTCP connections...\n");
	fprintf(f, "Proto  Local Address"
				"          Foreign Address        State\n");

	tcp_enum((void *)show_tcp_pcb, (void *)f);


	return 0;
}

