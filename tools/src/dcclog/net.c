/* $Id: dcclog.c,v 1.10 2006/09/28 19:31:45 bob Exp $ 
 *
 * File:	dcclog.c
 * Module:
 * Project:	ARM-DCC logger expander
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:
 * Comment:
 * Copyright(C) 2005 Robinson Mittmann. All Rights Reserved.
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
 * 
 */

#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#if defined(WIN32)
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #include <io.h>
  #ifndef in_addr_t
    #define in_addr_t uint32_t
  #endif
#else
  #include <sys/socket.h>
#endif

#include "dcc.h"

#ifdef DEBUG
#undef DEBUG
#endif

#include "debug.h"

struct tcp_drv {
#ifdef _WIN32
	SOCKET sock;
#else
	int sock
#endif
};

extern const struct dcc_lnk_op net_op;
extern char * prog;

int tcp_read_addr(struct tcp_drv * tcp, uint32_t * addr)
{
	DBG(DBG_TRACE, "tcp=%p", tcp); 

	if (recv(tcp->sock, (char *)addr, 4, 0) != 4) {
		fprintf(stderr, "%s: recv(): %s\n", prog, strerror(errno));
		return -1;
	}

	return 4;
}

uint32_t tcp_read_u32(struct tcp_drv * tcp)
{
	uint32_t val;

	if (recv(tcp->sock, (char *)&val, 4, 0) != 4)
		return 0;

	return val;
}

float tcp_read_float(struct tcp_drv * tcp)
{
	float val;

	if (recv(tcp->sock, (char *)&val, 4, 0) != 4)
		return 0;

	return val;
}


void * tcp_read_ptr(struct tcp_drv * tcp)
{
	uint32_t addr;

	if (recv(tcp->sock, (char *)&addr, 4, 0) != 4)
		return NULL;

	return image_ptr(addr);
}

char * tcp_read_str(struct tcp_drv * tcp)
{
	static char s[8192 + 1];
	uint32_t val;
	int i;
	int c;

	i = 0;
	do {
		if (recv(tcp->sock, (char *)&val, 4, 0) != 4)
			return NULL;
		c = val & 0xff;
		if (c != '\0') {
			s[i++] = c;
			c = (val >> 8) & 0xff;
			if (c != '\0') {
				s[i++] = c;
				c = (val >> 16) & 0xff;
				if (c != '\0') {
					s[i++] = c;
					c = (val >> 24)  & 0xff;
				}
			}
		}
		s[i++] = c;
	} while ((c != '\0') && (i < 8192)) ;

	return s;
}

uint8_t * tcp_read_bin(struct tcp_drv * lnk, unsigned int len)
{
#ifdef _WIN32
	SOCKET sock = lnk->sock;
#else
	int sock = lnk->sock;
#endif
	static uint8_t s[8192 + 1];
	uint32_t val;
	int i;
	int c;

	for (i = 0; i < len; ++i) {
		if ((i % 4) == 0) {
			if (recv(sock, (char *)&val, 4, 0) != 4)
				return NULL;
		}
		c = val & 0xff;
		s[i] = c;
		val >>= 8;
	} 

	return s;
}

void tcp_close(struct tcp_drv * lnk)
{
#ifdef _WIN32
	closesocket(lnk->sock);
#else
	close(lnk->sock);
#endif

	free(lnk);
}

const struct dcc_lnk_op net_lnk_op = {
	.read_addr = (void *)tcp_read_addr,
	.read_u32 = (void *)tcp_read_u32,
	.read_ptr = (void *)tcp_read_ptr,
	.read_str = (void *)tcp_read_str,
	.read_float = (void *)tcp_read_float,
	.read_bin = (void *)tcp_read_bin,
	.close = (void *)tcp_close
};

int net_connect(struct dcc_lnk * lnk, char * host, int port)
{
	struct tcp_drv * drv;
	struct hostent * hp;
	struct sockaddr_in sa;
	in_addr_t addr;
#if defined(WIN32)
	SOCKET sock;
#else
	int sock;
#endif
	int opt;

#if defined(WIN32)
	WSADATA wsaData;

	/* initialize windows sockets */
	if (WSAStartup(MAKEWORD(2,1), &wsaData) != 0) {
		MessageBox(NULL, 
			"Fatal ERROR!\nUnable to initialize windows socket!",
			"DCC Client", MB_ICONERROR);
		return -1;
	}
#endif

	/* Configuracao do Socket como TCP */
	sock = socket(AF_INET, SOCK_STREAM, 0); 
	if (sock < 0) {
		fprintf(stderr, "%s: socket(): %s\n", prog, strerror(errno));
		return -1;
	}

	memset(&sa, 0, sizeof(struct sockaddr_in));

	if ((hp = gethostbyname(host)) != NULL) {
		memcpy(&(sa.sin_addr), hp->h_addr, hp->h_length);
		sa.sin_family = hp->h_addrtype;
	} else {
		if ((addr = inet_addr(host)) == INADDR_NONE) {
			fprintf(stderr, "%s: Unknown host %s.\n", prog, host);
			return -1;
		}
		sa.sin_addr.s_addr = addr;
		sa.sin_family = AF_INET;
	}

	sa.sin_port = htons(port);

	if (connect(sock, (struct sockaddr *)&sa, sizeof(sa))) { 
		fprintf(stderr, "%s: connect(): %s\n", prog, strerror(errno));
		return -1;
	}

	opt = 1;
	setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void *)&opt, sizeof(int));
#if 0
	setsockopt(sock, IPPROTO_TCP, TCP_QUICKACK, (void *)&opt, sizeof(int));
	send(, "\r", 1, 0);
#endif
#if defined(WIN32)
//	if ((fd = _open_osfhandle(sock, O_RDWR | O_BINARY)) < 0) {
//		fprintf(stderr, "%s: _open_osfhandle(): %s\n", prog, strerror(errno));
//		return -1;
//	}
#endif
	drv = malloc(sizeof(struct tcp_drv));
	if (drv == NULL) {
		fprintf(stderr, "%s: malloc(): %s\n", prog, strerror(errno));
		return -1;
	}
	drv->sock = sock;

	lnk->op = &net_lnk_op;
	lnk->drv = drv;

	return sock;
}

