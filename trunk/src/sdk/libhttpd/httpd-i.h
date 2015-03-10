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
 * @file httpd-i
 * @brief HTTPD library private header
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __HTTPD_I_H__
#define __HTTPD_I_H__

#ifdef CONFIG_H
#include "config.h"
#endif

#include <stdint.h>
#include <tcpip/tcp.h>
#include <tcpip/httpd.h>
#include <errno.h>

#include <sys/dcclog.h>


#ifndef HTTPD_URI_MAX_LEN
#define HTTPD_URI_MAX_LEN 64
#endif

#ifndef HTTPD_SERVER_NAME 
#define HTTPD_SERVER_NAME "ThinkOS Web Server"
#endif

#ifndef HTTPD_URI_MAX_LEN
#define HTTPD_URI_MAX_LEN 64
#endif

/*
 * Preformatted HTML macros 
 */
#ifndef HTTPD_MSG_FOOTER
#define HTTPD_MSG_FOOTER "<hr>&copy; Copyrigth 2013-2014, Bob Mittmann" \
	"<br><b>ThinkOS</b> - Cortex-M3 Operating System - "\
	"<i><a href=\"https://code.google.com/p/yard-ice\">YARD-ICE</a></i>"\
	"</body></html>\r\n"
#endif

/*
 * Template for HTML content of HTTP return code messages
 */
#define HTTPD_MSG(CODE, INFO, MSG) "HTTP/1.1 " #CODE " " INFO "\r\n"\
	"Server: " HTTPD_SERVER_NAME "\r\n"\
	"Content-type: text/html;charset=ISO-8859-1\r\n\r\n"\
	"<html><head><title>" #CODE "</title></head>"\
	"<h2>" #CODE " - " INFO "</h2><p>" MSG "</p>"\
	HTTPD_MSG_FOOTER

/* 
 * HTTP connection control structure
 */
struct httpctl {
	struct tcp_pcb * tp;
	uint8_t method;
	uint16_t version;
	uint8_t auth;
	uint8_t ctype;
	uint8_t ctbound;
	uint8_t ctlen;
	char * usr;
	char * pwd;
	char uri[HTTPD_URI_MAX_LEN];
};

#ifdef __cplusplus
extern "C" {
#endif

int http_get(struct httpd * httpd, struct httpctl * ctl);

int http_post(struct httpd * httpd, struct httpctl * ctl);

#ifdef __cplusplus
}
#endif

#endif /* __HTTPD_I_H__ */

