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
 * @file 200.c
 * @brief HTTPD library
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 


#include "httpd-i.h"
#include "list.h"
#include <string.h>

/* Preformatted HTTP header */
const char http_hdr_200_html[] = "HTTP/1.1 200 OK\r\n"
	"Server: " HTTPD_SERVER_NAME "\r\n"
	"Content-type: text/html;charset=ISO-8859-1\r\n\r\n";

/* name of various mime types we support */
const char * const mime_name_lut[] = {
	"application/javascript\r\n\r\n"
	"application/xml\r\n\r\n",
	"image/gif\r\n\r\n",
	"image/jpeg\r\n\r\n",
	"image/png\r\n\r\n",
	"text/css\r\n\r\n",
	"text/html;charset=ISO-8859-1\r\n\r\n",
	"text/plain\r\n\r\n",
	"video/mpeg\r\n\r\n",
	};

int httpd_200(struct tcp_pcb * __tp, int unsigned __type)
{
	char * hdr;
	char * str;
	int len;
	int idx;
	int ret;

	if (__type >= MIME_END_MARK) {
		DCC_LOG1(LOG_TRACE, "Unsupported MIME type %d!", __type);
		return -EINVAL;
	}
	idx = __type;

	hdr = (char *)http_hdr_200_html;
	len = sizeof(http_hdr_200_html) - 
		sizeof("text/html;charset=ISO-8859-1\r\n\r\n");

	if ((ret = tcp_send(__tp, hdr, len, TCP_SEND_NOCOPY) < 0)) {
		DCC_LOG(LOG_ERROR, "tcp_send() failed!");
		return ret;
	}

	str = (char *)mime_name_lut[idx];
	len = strlen(str);

	if ((ret = tcp_send(__tp, str, len, 0) < 0)) {
		DCC_LOG(LOG_ERROR, "tcp_send() failed!");
	}

	return ret;
}

int httpd_200_html(struct tcp_pcb * __tp) 
{
	char * hdr;
	int len;
	int ret;

	hdr = (char *)http_hdr_200_html;
	len = sizeof(http_hdr_200_html) - 1;

	if ((ret = tcp_send(__tp, hdr, len, TCP_SEND_NOCOPY) < 0)) {
		DCC_LOG(LOG_ERROR, "tcp_send() failed!");
	}

	return ret;
}

