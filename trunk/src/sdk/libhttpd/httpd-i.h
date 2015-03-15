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

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <tcpip/tcp.h>
#include <tcpip/httpd.h>

#include <sys/dcclog.h>

/* http://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html */
#define HTTP_HEADER_ACCEPT "Accept"
#define HTTP_HEADER_ACCEPT_CHARSET "Accept-Charset"
#define HTTP_HEADER_ACCEPT_ENCODING "Accept-Encoding"
#define HTTP_HEADER_ACCEPT_LANGUAGE "Accept-Language"
#define HTTP_HEADER_ACCEPT_RANGES "Accept-Ranges"
#define HTTP_HEADER_AGE "Age"
#define HTTP_HEADER_ALLOW "Allow"
#define HTTP_HEADER_AUTHORIZATION "Authorization"
#define HTTP_HEADER_CACHE_CONTROL "Cache-Control"
#define HTTP_HEADER_CONNECTION "Connection"
#define HTTP_HEADER_CONTENT_ENCODING "Content-Encoding"
#define HTTP_HEADER_CONTENT_LANGUAGE "Content-Language"
#define HTTP_HEADER_CONTENT_LENGTH "Content-Length"
#define HTTP_HEADER_CONTENT_LOCATION "Content-Location"
#define HTTP_HEADER_CONTENT_MD5 "Content-MD5"
#define HTTP_HEADER_CONTENT_RANGE "Content-Range"
#define HTTP_HEADER_CONTENT_TYPE "Content-Type"
#define HTTP_HEADER_COOKIE "Cookie"
#define HTTP_HEADER_DATE "Date"
#define HTTP_HEADER_ETAG "ETag"
#define HTTP_HEADER_EXPECT "Expect"
#define HTTP_HEADER_EXPIRES "Expires"
#define HTTP_HEADER_FROM "From"
#define HTTP_HEADER_HOST "Host"
#define HTTP_HEADER_IF_MATCH "If-Match"
#define HTTP_HEADER_IF_MODIFIED_SINCE "If-Modified-Since"
#define HTTP_HEADER_IF_NONE_MATCH "If-None-Match"
#define HTTP_HEADER_IF_RANGE "If-Range"
#define HTTP_HEADER_IF_UNMODIFIED_SINCE "If-Unmodified-Since"
#define HTTP_HEADER_LAST_MODIFIED "Last-Modified"
#define HTTP_HEADER_LOCATION "Location"
#define HTTP_HEADER_MAX_FORWARDS "Max-Forwards"
#define HTTP_HEADER_PRAGMA "Pragma"
#define HTTP_HEADER_PROXY_AUTHENTICATE "Proxy-Authenticate"
#define HTTP_HEADER_PROXY_AUTHORIZATION "Proxy-Authorization"
#define HTTP_HEADER_RANGE "Range"
#define HTTP_HEADER_REFERER "Referer"
#define HTTP_HEADER_RETRY_AFTER "Retry-After"
#define HTTP_HEADER_SERVER "Server"
#define HTTP_HEADER_SET_COOKIE "Set-Cookie"
#define HTTP_HEADER_SET_COOKIE2 "Set-Cookie2"
#define HTTP_HEADER_TE "TE"
#define HTTP_HEADER_TRAILER "Trailer"
#define HTTP_HEADER_TRANSFER_ENCODING "Transfer-Encoding"
#define HTTP_HEADER_UPGRADE "Upgrade"
#define HTTP_HEADER_USER_AGENT "User-Agent"
#define HTTP_HEADER_VARY "Vary"
#define HTTP_HEADER_VIA "Via"
#define HTTP_HEADER_WARNING "Warning"
#define HTTP_HEADER_WWW_AUTHENTICATE "WWW-Authenticate"

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


#ifdef __cplusplus
extern "C" {
#endif

int httpd_200_css(struct tcp_pcb * __tp); 

int httpd_200_png(struct tcp_pcb * __tp); 

int httpd_200_jpeg(struct tcp_pcb * __tp);

int httpd_200_js(struct tcp_pcb * __tp);

int http_decode_uri_query(char * buf, int len, 
						  struct httpqry lst[], int max);

#ifdef __cplusplus
}
#endif

#endif /* __HTTPD_I_H__ */

