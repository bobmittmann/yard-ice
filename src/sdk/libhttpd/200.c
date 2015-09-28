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
#include <string.h>

/* Preformatted HTTP header */
const char http_hdr_200_html[] = "HTTP/1.1 200 OK\r\n"
	"Server: " HTTPD_SERVER_NAME "\r\n"
	"Content-type: text/html;charset=UTF-8\r\n\r\n";

const char http_hdr_200_js[] = "HTTP/1.1 200 OK\r\n"
	"Server: " HTTPD_SERVER_NAME "\r\n"
	"Cache-Control: private, max-age=21600\r\n"
	"Content-type: application/javascript\r\n\r\n";

const char http_hdr_200_text[] = "HTTP/1.1 200 OK\r\n"
	"Server: " HTTPD_SERVER_NAME "\r\n"
	"Content-type: text/plain\r\n\r\n";

const char http_hdr_200_css[] = "HTTP/1.1 200 OK\r\n"
	"Server: " HTTPD_SERVER_NAME "\r\n"
	"Cache-Control: private, max-age=21600\r\n"
	"Content-type: text/css\r\n\r\n";

const char http_hdr_200_html_gz[] = "HTTP/1.1 200 OK\r\n"
	"Server: " HTTPD_SERVER_NAME "\r\n"
	"Cache-Control: private, max-age=21600\r\n"
	"Content-type: text/html;charset=UTF-8\r\n"
	"Content-Encoding: gzip\r\n\r\n";

const char http_hdr_200_js_gz[] = "HTTP/1.1 200 OK\r\n"
	"Server: " HTTPD_SERVER_NAME "\r\n"
	"Cache-Control: public, max-age=21600\r\n"
	"Content-type: application/javascript\r\n"
	"Content-Encoding: gzip\r\n\r\n";

const char http_hdr_200_text_gz[] = "HTTP/1.1 200 OK\r\n"
	"Server: " HTTPD_SERVER_NAME "\r\n"
	"Cache-Control: public, max-age=21600\r\n"
	"Content-type: text/plain\r\n"
	"Content-Encoding: gzip\r\n\r\n";

const char http_hdr_200_css_gz[] = "HTTP/1.1 200 OK\r\n"
	"Server: " HTTPD_SERVER_NAME "\r\n"
	"Cache-Control: public, max-age=21600\r\n"
	"Content-type: text/css\r\n"
	"Content-Encoding: gzip\r\n\r\n";

const char http_hdr_200_png[] = "HTTP/1.1 200 OK\r\n"
	"Server: " HTTPD_SERVER_NAME "\r\n"
	"Cache-Control: public, max-age=21600\r\n"
	"Content-type: image/png\r\n\r\n";

const char http_hdr_200_jpeg[] = "HTTP/1.1 200 OK\r\n"
	"Server: " HTTPD_SERVER_NAME "\r\n"
	"Cache-Control: public, max-age=21600\r\n"
	"Content-type: image/jpeg\r\n\r\n";

const char http_hdr_200_gif[] = "HTTP/1.1 200 OK\r\n"
	"Server: " HTTPD_SERVER_NAME "\r\n"
	"Cache-Control: public, max-age=21600\r\n"
	"Content-type: image/gif\r\n\r\n";

const char http_hdr_200_json[] = "HTTP/1.1 200 OK\r\n"
	"Server: " HTTPD_SERVER_NAME "\r\n"
	"Cache-Control: no-cache\r\n"
	"Connection: keep-alive\r\n"
	"Content-Type: application/json\r\n"
	"\r\n";

const char http_hdr_200_xml[] = "HTTP/1.1 200 OK\r\n"
	"Server: " HTTPD_SERVER_NAME "\r\n"
	"Cache-Control: no-cache\r\n"
	"Connection: keep-alive\r\n"
	"Content-Type: application/xml\r\n"
	"\r\n";

/*
const char http_hdr_200_text_js[] = "HTTP/1.1 200 OK\r\n"
	"Server: " HTTPD_SERVER_NAME "\r\n"
	"Connection: keep-alive\r\n"
	"Content-type: text/javascript\r\n"
	"\r\n";
*/

const char * const http_200_hdr[] = {
	[OBJ_STATIC_HTML] = http_hdr_200_html,
	[OBJ_STATIC_TEXT] = http_hdr_200_text,
	[OBJ_STATIC_CSS] = http_hdr_200_css,
	[OBJ_STATIC_JS] = http_hdr_200_js,
	[OBJ_STATIC_JSON] = http_hdr_200_json,
	[OBJ_STATIC_XML] = http_hdr_200_xml,
	[OBJ_STATIC_PNG] = http_hdr_200_png,
	[OBJ_STATIC_JPEG] = http_hdr_200_jpeg,
	[OBJ_STATIC_GIF] = http_hdr_200_gif,
	[OBJ_STATIC_HTML_GZ] = http_hdr_200_html_gz,
	[OBJ_STATIC_TEXT_GZ] = http_hdr_200_text_gz,
	[OBJ_STATIC_CSS_GZ] = http_hdr_200_css_gz,
	[OBJ_STATIC_JS_GZ] = http_hdr_200_js_gz
};

const uint16_t http_200_len[] = {
	[OBJ_STATIC_HTML] = sizeof(http_hdr_200_html) - 1,
	[OBJ_STATIC_TEXT] = sizeof(http_hdr_200_text) - 1,
	[OBJ_STATIC_CSS] = sizeof(http_hdr_200_css) - 1,
	[OBJ_STATIC_JS] = sizeof(http_hdr_200_js) - 1,
	[OBJ_STATIC_JSON] = sizeof(http_hdr_200_json) - 1,
	[OBJ_STATIC_XML] = sizeof(http_hdr_200_xml) - 1,
	[OBJ_STATIC_PNG] = sizeof(http_hdr_200_png) - 1,
	[OBJ_STATIC_JPEG] = sizeof(http_hdr_200_jpeg) - 1,
	[OBJ_STATIC_GIF] = sizeof(http_hdr_200_gif) - 1,
	[OBJ_STATIC_HTML_GZ] = sizeof(http_hdr_200_html_gz) - 1,
	[OBJ_STATIC_TEXT_GZ] = sizeof(http_hdr_200_text_gz) - 1,
	[OBJ_STATIC_CSS_GZ] = sizeof(http_hdr_200_css_gz) - 1,
	[OBJ_STATIC_JS_GZ] = sizeof(http_hdr_200_js_gz) - 1
};

int httpd_200(struct tcp_pcb * __tp, int unsigned __type)
{
	char * hdr;
	int len;

	hdr = (char *)http_200_hdr[__type];
	len = http_200_len[__type];

	return tcp_send(__tp, hdr, len, TCP_SEND_NOCOPY);
}

