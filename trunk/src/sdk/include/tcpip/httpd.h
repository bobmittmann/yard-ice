/* 
 * Copyright(c) 2004-2012 BORESTE (www.boreste.com). All Rights Reserved.
 *
 * This file is part of the libtcpip.
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
 * @file tcpip/httpd.h
 * @brief 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __HTTPD_H__
#define __HTTPD_H__

#ifdef CONFIG_H
#include "config.h"
#endif

#include <stdint.h>
#include <tcpip/tcp.h>

#ifndef HTTPD_SERVER_NAME 
#define HTTPD_SERVER_NAME "ThinkOS Web Server"
#endif

#ifndef HTTPD_URI_MAX_LEN
#define HTTPD_URI_MAX_LEN 127
#endif


/* Mime types */
enum {
	APPLICATION_JAVASCRIPT = 0,
	APPLICATION_XML,
	IMAGE_GIF,
	IMAGE_JPEG,
	IMAGE_PNG,
	TEXT_CSS,
	TEXT_HTML,
	TEXT_PLAIN,
	VIDEO_MPEG,
	MIME_END_MARK
};

enum {
	OBJ_STATIC_HTML = 0,
	OBJ_STATIC_JS,
	OBJ_STATIC_TEXT,
	OBJ_STATIC_CSS,
	OBJ_STATIC_XML,
	OBJ_STATIC_GIF,
	OBJ_STATIC_JPEG,
	OBJ_STATIC_PNG,
	OBJ_STATIC_MPEG,
	OBJ_CODE_CGI,
	OBJ_CODE_JS,
};

enum {
	HTTP_UNKNOWN = 0,
	HTTP_GET = 1,
	HTTP_POST = 2
};

struct httpdobj {
	const char * oid;
	uint8_t typ;
	uint8_t lvl;
	uint16_t len;
	const void * ptr;
};

struct httpddir {
	const char * path;
	const struct httpdobj * objlst;
};

#define HTTPDAUTH_NAME_MAX 12
#define HTTPDAUTH_PASSWD_MAX 12

struct httpdauth {
	uint8_t uid;
	uint8_t lvl;
	char name[HTTPDAUTH_NAME_MAX + 1];
	char passwd[HTTPDAUTH_PASSWD_MAX + 1];
};

/*
 * HTTP server control structure
 */

struct httpd {
	struct tcp_pcb * tp;
	/* server root directory */
	const struct httpddir * dir;
	/* authentication data */
	const struct httpdauth * auth;
};

/* 
 * HTTP connection control structure
 */
struct httpctl {
	struct httpd * httpd;
	struct tcp_pcb * tp;
	uint8_t method;
	uint16_t version;
	uint8_t auth;
	uint8_t ctype;
	uint8_t ctbound;
	uint8_t ctlen;
	char * usr;
	char * pwd;
	char uri[HTTPD_URI_MAX_LEN + 1];
};

typedef int (* httpd_cgi_t)(struct httpctl ctl);

typedef int (* __httpd_cgi_t)(struct tcp_pcb * tp, char * opt, 
							int content_type, int content_len); 
/*
 */
typedef int (* cgi_callback_t)(struct tcp_pcb * tp, int event, void * context);

/* 
	Ex:
	int _form_cgi_(struct tcp_pcb * tp, char * nm[], char * val[], int n);
*/	
typedef int (* httpd_form_cgi_t)(struct tcp_pcb * tp, char ** name, 
								 char ** value, int count);


/* 
	Ex:
	int _upload_cgi_(struct tcp_pcb * tp, char * buf, int len, int offs);
 */
typedef int (* httpd_file_cgi_t)(struct tcp_pcb * tp, char * buf, 
								 int len, int offs);

#ifdef __cplusplus
extern "C" {
#endif

struct tcp_pcb * httpd_start(struct httpd * httpd, 
							 int port, int backlog,
							 const struct httpddir dirlst[],
							 const struct httpdauth authlst[]);

int httpd_stop(struct httpd * httpd);

int http_accept(struct httpd * httpd, struct httpctl * ctl);

int http_close(struct httpctl * ctl);

int http_get(struct httpctl * ctl);

int http_post(struct httpctl * ctl);

int http_process(struct httpd * httpd, struct httpctl * ctl);

int httpd_dirlist(struct httpd * httpd, struct httpctl * ctl);

/*
 Connect a cgi event handler to a CGI tcp connection. 
 Returns a pointer to an internall allocated memory, used for the
 CGI to keep its context... This same pointer is passed to the
 event handler. The event handler is called whenever a TCP event is raised.
 */
void * httpd_cgi_attach(struct tcp_pcb * __tp, cgi_callback_t __callback); 

int httpd_contenttype(struct tcp_pcb * __tp, unsigned int __type);

/* 200 OK */
int httpd_200(struct tcp_pcb * __tp, unsigned int __type);
int httpd_200_html(struct tcp_pcb * __tp);

/* 400 Bad Request */
int httpd_400(struct tcp_pcb * __tp);

/* 401 Unauthorized */
int httpd_401(struct tcp_pcb * __tp);
int httpd_401_auth(struct tcp_pcb * __tp);

/* 402 Payment Required */

/* 403 Forbidden */
int httpd_403(struct tcp_pcb * __tp);

/* 404 Not Found */
int httpd_404(struct tcp_pcb * __tp);

/* 405 Method Not Allowed */
int httpd_405(struct tcp_pcb * __tp);

/* 406 Not Acceptable */

/* 407 Proxy Authentication Required */

/* 408 Request Timeout */
int httpd_408(struct tcp_pcb * __tp);

/* 409 Conflict */
int httpd_409(struct tcp_pcb * __tp);

/* 410 Gone */
int httpd_410(struct tcp_pcb * __tp);

/* 411 Length Required */
int httpd_411(struct tcp_pcb * __tp);

/* 412 Precondition Failed */

/* 413 Request Entity Too Large */
int httpd_413(struct tcp_pcb * __tp);

/* 414 Request-URI Too Long */
int httpd_414(struct tcp_pcb * __tp);

/* 415 Unsupported Media Type */
int httpd_415(struct tcp_pcb * __tp);

/* 416 Requested Range Not Satisfiable */

/* 417 Expectation Failed */

/* 500 Internal Server Error */
int httpd_500(struct tcp_pcb * __tp);

/* 501 Not Implemented */
int httpd_501(struct tcp_pcb * __tp);

/* 502 Bad Gateway */

/* 503 Service Unavailable */
int httpd_503(struct tcp_pcb * __tp);

/* 504 Gateway Timeout */

/* 505 HTTP Version Not Supported */
int httpd_505(struct tcp_pcb * __tp);

/* Auxiliar Name Value List parser */
int httpd_nvparse(const char * s, char * name[], char * value[], int count);

/* */
void httpd_listen_callback(struct tcp_pcb * tp, int event, 
						   struct httpd * httpd);
#ifdef __cplusplus
}
#endif

#endif /* __HTTPD_H__ */

