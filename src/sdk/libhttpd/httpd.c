/*
 * Copyright(c) 2004-2014 BORESTE (www.boreste.com). All Rights Reserved.
 *
 * This file is part of the libhttpd.
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
 * @file httpd.c
 * @brief HTTPD library
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include "httpd-i.h"

/* 
 * hash values for http tokens
 */

#define HTTPMETHOD_GET              0x55
#define HTTPMETHOD_POST             0xeb

/* Request Header Fields */
#define HTTPHDR_ACCEPT              0xb3
#define HTTPHDR_ACCEPT_CHARSET      0x46
#define HTTPHDR_ACCEPT_ENCODING     0x55
#define HTTPHDR_ACCEPT_LANGUAGE     0x9a
#define HTTPHDR_AUTHORIZATION       0x15
#define HTTPHDR_EXPECT              -1
#define HTTPHDR_HOST                0xec
#define HTTPHDR_IF_MATCH            -2
#define HTTPHDR_IF_MODIFIED_SINCE   -3
#define HTTPHDR_IF_NONE_MATCH       -4
#define HTTPHDR_IF_RANGE            -5
#define HTTPHDR_IF_UNMODIFIED_SINCE -6
#define HTTPHDR_MAX_FORWARDS        -7
#define HTTPHDR_PROXY_AUTHORIZATION -8
#define HTTPHDR_RANGE               -9
#define HTTPHDR_REFERER             0xea
#define HTTPHDR_TE                  -10
#define HTTPHDR_USER_AGENT          0xae
#define HTTPHDR_CONTENT_TYPE        0xeb
#define HTTPHDR_CONTENT_LENGTH      0x75

/* Hop-by-Hop Headers */
#define HTTPHDR_CONNECTION          0x30
#define HTTPHDR_KEEP_ALIVE          0x11
#define HTTPHDR_TRAILERS            -11
#define HTTPHDR_PROXY_AUTHENTICATE  -12
#define HTTPHDR_TRANSFER_ENCODING   -13
#define HTTPHDR_UPGRADE             -14

/* ?? */
#define HTTPHDR_CACHE_CONTROL       0xd9

/* Authentication */
#define HTTPAUTH_BASIC              0x77
#define HTTPAUTH_DIGEST             0x00


static int get_method(struct tcp_pcb * tp)
{
	char cp[1];
	int hash;
	int ret;
	int c;

	hash = 0;

	for (;;) {
		/* read data - character at time */
		/* FIXME: this is quite slow ... */
		if ((ret = tcp_recv(tp, cp, 1)) <= 0)
			return ret;

		if ((c = *cp) == ' ')
			break;

		DCC_LOG1(LOG_MSG, "'%c'", c);

		hash = CRC8(hash, c);
	}

	return hash;
}

static int get_uri(struct tcp_pcb * tp, char * uri, int max)
{
	char * cp;
	int ret;
	int n;
	int c;

	n = 0;
	cp = uri;

	for (n = 0; n < max; n++) {
		/* read data - character at time */
		/* FIXME: this is quite slow ... */
		if ((ret = tcp_recv(tp, cp, 1)) <= 0)
			return ret;

		if ((c = *cp) == ' ') {
			*cp = '\0';
			return n;
		}

		DCC_LOG(DCC_MSG, "'%c'", c);

		cp++;
	}

//	http->state = HTTPCTL_414;
	return -1;
}

static int get_version(struct tcp_pcb * tp)
{
	int version = 0;
	char cp[1];
	int ret;
	int c;

	for (;;) {
		if ((ret = tcp_recv(tp, cp, 1)) <= 0)
			return ret;

		if ((c = *cp) == '\r')
			break;

		DCC_LOG(DCC_MSG, "'%c'", c);

		if ((c >= '0') && (c <= '9'))
			version = (version * 10) + c - '0';
	}

	if ((ret = tcp_recv(tp, cp, 1)) <= 0)
		return ret;

	if ((c = *cp) == '\n')
		return version;

	/* Malformed request line, respond with: 400 Bad Request */
//	http->state = HTTPCTL_400;
	return -1;
}

static int get_hdr_line(struct tcp_pcb * tp, char * val, int max)
{
	char buf[1];
	char * cp;
	int hash;
	int ret;
	int c;
	int n;

	DCC_LOG(DCC_INFO, "----------------");
	hash = 0;
	cp = buf;
	/*
	 * Parsing the Header Line
	 */
	for (;;) {
		/* read data - character at time */
		/* FIXME: this is quite slow ... */
		if ((ret = tcp_recv(tp, cp, 1)) <= 0)
			return ret;

		if ((c = *cp) == ':') {

			cp = val;
			n = 0;

			/* remove leading blanks */
			do {
				if ((ret = tcp_recv(tp, cp, 1)) <= 0)
					return ret;
			} while ((c = *cp) == ' ');

			/* get the value */
			while (c != '\r') {
				if (n < max) {
					cp++;
					n++;
				}

				if ((ret = tcp_recv(tp, cp, 1)) <= 0)
					return ret;

				c = *cp;
				DCC_LOG(DCC_INFO, "'%c'", c);
			}

			/* append zero */
			*cp = '\0';
			cp = buf;
			break;
		}

		if (c == '\r')
			break;

		DCC_LOG(DCC_INFO, "'%c'", c);

		hash = CRC8(hash, c);
	}

	if ((ret = tcp_recv(tp, cp, 1)) <= 0)
		return ret;

	if ((c = *cp) == '\n')
		return hash;

	/* Malformed request line, respond with: 400 Bad Request */
	return -1;
}

#define HTML_HDR_VAL_LEN_MAX 64

static int parse_hdr(struct tcp_pcb * tp, struct httpctl * ctl)
{
	char val[HTML_HDR_VAL_LEN_MAX];
	int opt;

	/*
	 * Parsing the Header Field Lines
	 */
	while ((opt = get_hdr_line(tp, val, HTML_HDR_VAL_LEN_MAX)) > 0) {
		switch (opt) {
		/* Request Header Fields */
		case HTTPHDR_AUTHORIZATION:
			DCC_LOG(DCC_TRACE, "Authorization: %s", val);
			break;
		case HTTPHDR_CONTENT_TYPE:
			/* hash the content type */
			ctl->ctype = crc8(0, val, strlen(val));
			DCC_LOG(DCC_TRACE, "Content-Type: %s", val);
			break;
		case HTTPHDR_CONTENT_LENGTH:
			DCC_LOG(DCC_TRACE, "Content-Length: %s", val);
			ctl->ctlen = atoi(val);
			break;			
		case HTTPHDR_ACCEPT:
			DCC_LOG(DCC_TRACE, "Accept: %s", val);
			break;
		case HTTPHDR_ACCEPT_CHARSET:
			DCC_LOG(DCC_TRACE, "Accept-Charset: %s", val);
			break;
		case HTTPHDR_ACCEPT_ENCODING:
			DCC_LOG(DCC_TRACE, "Accept-Encoding: %s", val);
			break;
		case HTTPHDR_ACCEPT_LANGUAGE:
			DCC_LOG(DCC_TRACE, "Accept-Language: %s", val);
			break;
		case HTTPHDR_EXPECT:
			DCC_LOG(DCC_TRACE, "Expect: %s", val);
			break;
		case HTTPHDR_HOST:
			DCC_LOG(DCC_TRACE, "Host: %s", val);
			break;
		case HTTPHDR_IF_MATCH:
			DCC_LOG(DCC_TRACE, "If-Match: %s", val);
			break;
		case HTTPHDR_IF_MODIFIED_SINCE:
			DCC_LOG(DCC_TRACE, "If-Modified-Since: %s", val);
			break;
		case HTTPHDR_IF_NONE_MATCH:
			DCC_LOG(DCC_TRACE, "If-None-Match: %s", val);
			break;
		case HTTPHDR_IF_RANGE:
			DCC_LOG(DCC_TRACE, "If-Range: %s", val);
			break;
		case HTTPHDR_IF_UNMODIFIED_SINCE:
			DCC_LOG(DCC_TRACE, "If-Unmodified-Since: %s", val);
			break;
		case HTTPHDR_MAX_FORWARDS:
			DCC_LOG(DCC_TRACE, "Max-Forwards: %s", val);
			break;
		case HTTPHDR_PROXY_AUTHORIZATION:
			DCC_LOG(DCC_TRACE, "Proxy-Authorization: %s", val);
			break;
		case HTTPHDR_RANGE:
			DCC_LOG(DCC_TRACE, "Range: %s", val);
			break;
		case HTTPHDR_REFERER:
			DCC_LOG(DCC_TRACE, "Referer: %s", val);
			break;
		case HTTPHDR_TE:
			DCC_LOG(DCC_TRACE, "Te: %s", val);
			break;
		case HTTPHDR_USER_AGENT:
			DCC_LOG(DCC_TRACE, "User-Agent: %s", val);
			break;
		/* Hop-by-Hop Headers */
		case HTTPHDR_CONNECTION:
			DCC_LOG(DCC_TRACE, "Connection: %s", val);
			break;
		case HTTPHDR_KEEP_ALIVE:
			DCC_LOG(DCC_TRACE, "keep-Alive: %s", val);
			break;
		case HTTPHDR_TRAILERS:
			DCC_LOG(DCC_TRACE, "Trailers: %s", val);
			break;
		case HTTPHDR_PROXY_AUTHENTICATE:
			DCC_LOG(DCC_TRACE, "Proxy-Authenticate: %s", val);
			break;
		case HTTPHDR_TRANSFER_ENCODING:
			DCC_LOG(DCC_TRACE, "Transfer-Encoding: %s", val);
			break;
		case HTTPHDR_UPGRADE:
			DCC_LOG(DCC_TRACE, "Upgrade: %s", val);
		case HTTPHDR_CACHE_CONTROL:
			DCC_LOG(DCC_TRACE, "Cache-Control: %s", val);
			break;
		default:
			DCC_LOG(DCC_TRACE, "opt=%02x: %s", opt, val);
		}
	}

	return opt;
}

int http_accept(struct httpd * httpd, struct httpctl * ctl)
{
	struct tcp_pcb * tp;

	if ((tp = tcp_accept(httpd->tp)) == NULL) {
		DCC_LOG(DCC_ERROR, "tcp_accept().");
		return -1;
	}

	DCC_LOG(DCC_TRACE, "%d.%d.%d.%d:%d accepted.", 
		IP4_ADDR1(tp->t_faddr), IP4_ADDR2(tp->t_faddr), 
		IP4_ADDR3(tp->t_faddr), IP4_ADDR4(tp->t_faddr), ntohs(tp->t_fport));

	ctl->method = get_method(tp);
	DCC_LOG(DCC_TRACE, "method=0x%02x", ctl->method);

	get_uri(tp, ctl->uri, HTTPD_URI_MAX_LEN);
	DCC_LOG(DCC_TRACE, "uri='%s'", ctl->uri);

	ctl->version = get_version(tp);
	DCC_LOG(DCC_TRACE, "version=%d", ctl->version);

	if (parse_hdr(tp, ctl) < 0) {
		/* Malformed request line, respond with: 400 Bad Request */
		httpd_400(tp);
		tcp_close(tp);
		return -1;
	}

	ctl->tp = tp;
	return 0;
}

int http_process(struct httpd * httpd, struct httpctl * ctl)
{
	switch (ctl->method) {
	case HTTPMETHOD_GET:
		DCC_LOG(DCC_TRACE, "GET");
		http_get(httpd, ctl);
		break;
	case HTTPMETHOD_POST:
		DCC_LOG(DCC_TRACE, "POST");
		http_post(httpd, ctl);
		break;
	}

	tcp_close(ctl->tp);
	return 0;
}

struct tcp_pcb * httpd_start(struct httpd * httpd, 
							 int port, int backlog,
							 const char * path)
{
	struct tcp_pcb * tp;

	if (httpd == NULL) {
		DCC_LOG(DCC_ERROR, "Invalid parameter!\n");
		return NULL;
	}

	if ((tp = tcp_alloc()) == NULL) {
		DCC_LOG(DCC_ERROR, "Can't alloc TCP PCB!\n");
		return NULL;
	}

	tcp_bind(tp, INADDR_ANY, htons(port));

	if (tcp_listen(tp, backlog) != 0) {
		DCC_LOG(DCC_ERROR, "Can't register the TCP listner!\n");
		return NULL;
	}

	httpd->tp = tp;
	httpd->root = (char *)path;

	return tp;
}

