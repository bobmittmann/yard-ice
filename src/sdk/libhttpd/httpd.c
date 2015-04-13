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

/* CRC8 (polynomial x^8 + x^5 + x^4) */
const uint8_t crc8_lut[256] = {
	  0,  49,  98,  83, 196, 245, 166, 151,
	185, 136, 219, 234, 125,  76,  31,  46,
 	 67, 114,  33,  16, 135, 182, 229, 212,
	250, 203, 152, 169,  62,  15,  92, 109,
	134, 183, 228, 213,  66, 115,  32,  17,
	 63,  14,  93, 108, 251, 202, 153, 168,
	197, 244, 167, 150,   1,  48,  99,  82,
	124,  77,  30,  47, 184, 137, 218, 235,
	 61,  12,  95, 110, 249, 200, 155, 170,
	132, 181, 230, 215,  64, 113,  34,  19,
	126,  79,  28, 45,  186, 139, 216, 233,
	199, 246, 165, 148,   3,  50,  97,  80,
	187, 138, 217, 232, 127,  78,  29,  44,
	  2,  51,  96,  81, 198, 247, 164, 149,
	248, 201, 154, 171,  60,  13, 94,  111,
	 65, 112,  35,  18, 133, 180, 231, 214,
	122,  75,  24,  41, 190, 143, 220, 237,
	195, 242, 161, 144,   7,  54, 101,  84,
	 57,   8,  91, 106, 253, 204, 159, 174,
	128, 177, 226, 211,  68, 117,  38,  23,
	252, 205, 158, 175,  56,   9,  90, 107,
	 69, 116,  39,  22, 129, 176, 227, 210,
	191, 142, 221, 236, 123,  74,  25,  40,
	  6,  55, 100,  85, 194, 243, 160, 145,
	 71, 118,  37,  20, 131, 178, 225, 208,
	254, 207, 156, 173,  58,  11,  88, 105,
	  4,  53, 102,  87, 192, 241, 162, 147,
	189, 140, 223, 238, 121,  72,  27,  42,
	193, 240, 163, 146,   5,  52, 103,  86,
	120,  73,  26,  43, 188, 141, 222, 239,
	130, 179, 224, 209,  70, 119,  36,  21,
	 59,  10,  89, 104, 255, 206, 157, 172
};

static inline unsigned int __crc8(unsigned int crc, int c) {
	return crc8_lut[crc ^ c];
}

static inline unsigned int crc8(unsigned int crc, const void * buf, int len) {
	uint8_t * cp = (uint8_t *)buf;
	int i;

	for (i = 0; i < len; ++i) 
		crc = crc8_lut[crc ^ cp[i]];

	return crc;
}


/* 
 * hash values for http tokens
 */


#define HTTPMETHOD_GET_HASH         0x55
#define HTTPMETHOD_POST_HASH        0xeb

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

		hash = __crc8(hash, c);
	}

	switch (hash) {
	case HTTPMETHOD_GET_HASH:
		return HTTP_GET;
	case HTTPMETHOD_POST_HASH:
		return HTTP_POST;
	}

	return HTTP_UNKNOWN;
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

		DCC_LOG1(LOG_MSG, "'%c'", c);

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

		DCC_LOG1(LOG_MSG, "'%c'", c);

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

	DCC_LOG(LOG_INFO, "----------------");
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
				DCC_LOG1(LOG_INFO, "'%c'", c);
			}

			/* append zero */
			*cp = '\0';
			cp = buf;
			break;
		}

		if (c == '\r')
			break;

		DCC_LOG1(LOG_INFO, "'%c'", c);

		hash = __crc8(hash, c);
	}

	if ((ret = tcp_recv(tp, cp, 1)) <= 0)
		return ret;

	if ((c = *cp) == '\n')
		return hash;

	/* Malformed request line, respond with: 400 Bad Request */
	return -1;
}

#define HTML_HDR_VAL_LEN_MAX 80

static int parse_hdr(struct tcp_pcb * tp, struct httpctl * ctl)
{
	char val[HTML_HDR_VAL_LEN_MAX + 1];
	int opt;

	/*
	 * Parsing the Header Field Lines
	 */
	while ((opt = get_hdr_line(tp, val, HTML_HDR_VAL_LEN_MAX)) > 0) {
		switch (opt) {
		/* Request Header Fields */
		case HTTPHDR_AUTHORIZATION:
			DCC_LOG1(LOG_INFO, "Authorization: %c ...", val);
			break;
		case HTTPHDR_CONTENT_TYPE:
			/* hash the content type */
			ctl->ctype = crc8(0, val, strlen(val));
			DCC_LOG1(LOG_INFO, "Content-Type: 0x%02x", ctl->ctype);
			break;
		case HTTPHDR_CONTENT_LENGTH:
			ctl->ctlen = atoi(val);
			DCC_LOG1(LOG_INFO, "Content-Length: %d", ctl->ctlen);
			break;			
		case HTTPHDR_ACCEPT:
			DCC_LOG1(LOG_INFO, "Accept: %c ...", val[0]);
			break;
		case HTTPHDR_ACCEPT_CHARSET:
			DCC_LOG1(LOG_INFO, "Accept-Charset: %c ...", val[0]);
			break;
		case HTTPHDR_ACCEPT_ENCODING:
			DCC_LOG1(LOG_INFO, "Accept-Encoding: %c ...", val[0]);
			break;
		case HTTPHDR_ACCEPT_LANGUAGE:
			DCC_LOG1(LOG_INFO, "Accept-Language: %c ...", val[0]);
			break;
		case HTTPHDR_EXPECT:
			DCC_LOG1(LOG_INFO, "Expect: %c ...", val[0]);
			break;
		case HTTPHDR_HOST:
			DCC_LOG1(LOG_INFO, "Host: %c ...", val[0]);
			break;
		case HTTPHDR_IF_MATCH:
			DCC_LOG1(LOG_INFO, "If-Match: %c ...", val[0]);
			break;
		case HTTPHDR_IF_MODIFIED_SINCE:
			DCC_LOG1(LOG_INFO, "If-Modified-Since: %c ...", val[0]);
			break;
		case HTTPHDR_IF_NONE_MATCH:
			DCC_LOG1(LOG_INFO, "If-None-Match: %c ...", val[0]);
			break;
		case HTTPHDR_IF_RANGE:
			DCC_LOG1(LOG_INFO, "If-Range: %c ...", val[0]);
			break;
		case HTTPHDR_IF_UNMODIFIED_SINCE:
			DCC_LOG1(LOG_INFO, "If-Unmodified-Since: %c ...", val[0]);
			break;
		case HTTPHDR_MAX_FORWARDS:
			DCC_LOG1(LOG_INFO, "Max-Forwards: %c ...", val[0]);
			break;
		case HTTPHDR_PROXY_AUTHORIZATION:
			DCC_LOG1(LOG_INFO, "Proxy-Authorization: %c ...", val[0]);
			break;
		case HTTPHDR_RANGE:
			DCC_LOG1(LOG_INFO, "Range: %c ...", val[0]);
			break;
		case HTTPHDR_REFERER:
			DCC_LOG1(LOG_INFO, "Referer: %c ...", val[0]);
			break;
		case HTTPHDR_TE:
			DCC_LOG1(LOG_INFO, "Te: %c ...", val[0]);
			break;
		case HTTPHDR_USER_AGENT:
			DCC_LOG1(LOG_INFO, "User-Agent: %c ...", val[0]);
			break;
		/* Hop-by-Hop Headers */
		case HTTPHDR_CONNECTION:
			DCC_LOG1(LOG_INFO, "Connection: %c ...", val[0]);
			break;
		case HTTPHDR_KEEP_ALIVE:
			DCC_LOG1(LOG_INFO, "keep-Alive: %c ...", val[0]);
			break;
		case HTTPHDR_TRAILERS:
			DCC_LOG1(LOG_INFO, "Trailers: %c ...", val[0]);
			break;
		case HTTPHDR_PROXY_AUTHENTICATE:
			DCC_LOG1(LOG_INFO, "Proxy-Authenticate: %c ...", val[0]);
			break;
		case HTTPHDR_TRANSFER_ENCODING:
			DCC_LOG1(LOG_INFO, "Transfer-Encoding: %c ...", val[0]);
			break;
		case HTTPHDR_UPGRADE:
			DCC_LOG1(LOG_INFO, "Upgrade: %c ...", val[0]);
			break;
		case HTTPHDR_CACHE_CONTROL:
			DCC_LOG1(LOG_INFO, "Cache-Control: %c ...", val[0]);
			break;
		default:
			DCC_LOG2(LOG_INFO, "opt=%02x: %c ...", opt, val[0]);
		}
	}

	return opt;
}

int http_accept(struct httpd * httpd, struct httpctl * ctl)
{
	struct tcp_pcb * tp;

	DCC_LOG1(LOG_INFO, "httpd=%p.", httpd);

	if ((tp = tcp_accept(httpd->tp)) == NULL) {
		DCC_LOG(LOG_ERROR, "tcp_accept().");
		return -1;
	}

	DCC_LOG1(LOG_INFO, "ctl=%p accepted.", ctl);
	memset(ctl, 0, sizeof(struct httpctl));

	ctl->method = get_method(tp);
	DCC_LOG1(LOG_INFO, "method=%d", ctl->method);

	get_uri(tp, ctl->uri, HTTPD_URI_MAX_LEN);
	DCC_LOG1(LOG_INFO, "uri='%c ...'", ctl->uri[0]);

	ctl->version = get_version(tp);
	DCC_LOG1(LOG_INFO, "version=%d", ctl->version);

	if (parse_hdr(tp, ctl) < 0) {
		/* Malformed request line, respond with: 400 Bad Request */
		httpd_400(tp);
		tcp_close(tp);
		return -1;
	}

	ctl->tp = tp;
	ctl->httpd = httpd;
	
	DCC_LOG1(LOG_INFO, "version=%d", ctl->version);

	return 0;
}

const struct httpdauth httpd_def_authlst[] = {
	{ .uid = 0, .lvl = 0, .name = "", .passwd = "" }
};

struct tcp_pcb * httpd_start(struct httpd * httpd, 
							 int port, int backlog,
							 const struct httpddir dirlst[],
							 const struct httpdauth authlst[])
{
	struct tcp_pcb * tp;

	if (httpd == NULL) {
		DCC_LOG(LOG_ERROR, "Invalid parameter!\n");
		return NULL;
	}

	if ((tp = tcp_alloc()) == NULL) {
		DCC_LOG(LOG_ERROR, "Can't alloc TCP PCB!\n");
		return NULL;
	}

	tcp_bind(tp, INADDR_ANY, htons(port));

	if (tcp_listen(tp, backlog) != 0) {
		DCC_LOG(LOG_ERROR, "Can't register the TCP listner!\n");
		return NULL;
	}

	httpd->tp = tp;
	httpd->dir = dirlst;

	if (authlst == NULL)
		httpd->auth = httpd_def_authlst;
	else
		httpd->auth = authlst;

	return tp;
}

int http_close(struct httpctl * ctl)
{
	return tcp_close(ctl->tp);
}

const char http_index_obj[] = "index.html";

const struct httpdobj * http_obj_lookup(struct httpctl * ctl)
{
	struct httpd * httpd = ctl->httpd;
	char path[HTTPD_URI_MAX_LEN];
	const struct httpdobj * obj;
	const struct httpddir * dir;
	char * dst;
	char * src;
	char * sep;
	char * qry;
	char * oid;
	int c;

	dst = path;
	sep = path;
	oid = ctl->uri;


//	for (opt = buf; (*opt); opt++) {
 //		if (*opt == '?') {
//			*opt++ = '\0';
//			break;
//		}
//	}

	qry = NULL;
	for (src = ctl->uri; (c = *src) != '\0'; ++src) {
		if (c == '/') {
			oid = src + 1;
			sep = dst;
		}
		if (c == '?') {
			*src = '\0';
			qry = src + 1;
			break;
		}
		*dst++ = c;
	}
	*sep = '\0';

	if (path[0] == '\0') {
		path[0] = '/';
		path[1] = '\0';
	}

	DCC_LOGSTR(LOG_TRACE, "path=\"%s\"", path);

	for (dir = httpd->dir; (dir->path != NULL); ++dir) {
		if (strcmp(dir->path, path) == 0) {
			break;
		};
	}
	
	if (dir->path == NULL) {
		DCC_LOG(LOG_WARNING, "404 File Not Found (invalid path)");
		httpd_404(ctl->tp);
		return NULL;
	}

	if (*oid == '\0')
		oid = (char *)http_index_obj;

	DCC_LOGSTR(LOG_TRACE, "obj=\"%s\"", oid);

	for (obj = dir->objlst; (obj->oid != NULL); ++obj) {
		if (strcmp(obj->oid, oid) == 0) {
			break;
		};
	}

	if (obj->oid == NULL) {
		DCC_LOG(LOG_WARNING, "404 File Not Found (invalid OID)");
		httpd_404(ctl->tp);
		return NULL;
	}

	if (qry != NULL) {
		int n;

		n = http_decode_uri_query(qry, 10000, ctl->qrylst, 
								  HTTPD_QUERY_LST_MAX);
		if (n < 0) {
			DCC_LOG(LOG_WARNING, "400 Invalid Request");
			httpd_400(ctl->tp);
			return NULL;
		}
		ctl->qrycnt = n;
	}

	return obj;
}

#if 0
int http_process(struct httpd * httpd, struct httpctl * ctl)
{
	switch (ctl->method) {
	case HTTPMETHOD_GET:
		DCC_LOG(LOG_INFO, "GET");
		http_get(httpd, ctl);
		break;
	case HTTPMETHOD_POST:
		DCC_LOG(LOG_INFO, "POST");
		http_post(httpd, ctl);
		break;
	}

	return 0;
}


#define SIZEOF_HTTPCTL (sizeof(struct httpctl) + HTTPD_URI_MAX_LEN)
#define SIZEOF_HTTPD (sizeof(struct httpd)

/*
 * HTTP server control structure
 */
struct httpd {
	struct tcp_pcb * tp;
	/* server root directory */
	const struct httpddir * dir;
	/* authentication data */
	const struct httpdauth * auth;
	uint8_t max_con;
	struct {
		struct httpctl ctl;
		uint8_t uri[HTTPD_URI_MAX_LEN];
	} con[];
};

struct httpd * httpd_init(uint32_t * srv_buf, unsigned int srv_size,
						  const struct httpddir dirlst[], 
						  const struct httpdauth authlst[])
{
	struct httpdsrv * srv = (struct httpdsrv *)srv_buf;

	if (srv_size < (SIZEOF_HTTPD + SIZEOF_HTTPCTL)) {
		DCC_LOG(LOG_ERROR, "invalid buffer size!");
		return NULL;
	}

	return httpd;
}

#endif
