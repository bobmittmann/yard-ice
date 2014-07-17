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
 * @file 401.c
 * @brief HTTPD library
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include "httpd-i.h"

static const char http_msg_401[] = HTTPD_MSG(401, "Unauthorized", "");

static const char http_msg_401_auth[] = "HTTP/1.1 401 Unauthorized\r\n"\
	"Server: " HTTPD_SERVER_NAME "\r\n"\
	"WWW-Authenticate: Basic realm=\"" HTTPD_SERVER_NAME "\"\r\n"\
	"Content-type: text/html\r\n\r\n"\
	"<html><head><title> 401</title></head>"\
	"<h2> 401 - Unauthorized</h2>" HTTPD_MSG_FOOTER;

int httpd_401(struct tcp_pcb * __tp) 
{
	return tcp_send(__tp, http_msg_401, 
					sizeof(http_msg_401) - 1, TCP_SEND_NOCOPY);
}

int httpd_401_auth(struct tcp_pcb * __tp) 
{
	return tcp_send(__tp, http_msg_401_auth, 
		sizeof(http_msg_401_auth) - 1, TCP_SEND_NOCOPY);
}

