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
#include "http_hdr.h"

#ifndef HTTPD_POOL_SIZE
#define HTTPD_POOL_SIZE 1
#endif

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

/* Authentication */
#define HTTPAUTH_BASIC              0x77
#define HTTPAUTH_DIGEST             0x00

int http_process_request(struct httpctl * ctl, void * req)
{
	uint32_t * up = (uint32_t *)req;
	char * cp = (char *)req;
	char * uri;
	int c;
	int i;

#if (BYTE_ORDER != LITTLE_ENDIAN)
#error "This code works for little endian machines only."
#endif

	if (up[0] == 0x20544547) { /* 'GET '*/
		ctl->method = HTTP_GET;
		cp += 4;
	} else if ((up[0] == 0x54534f50) &&
			((up[1] & 0x000000ff) == 0x00000020)) { /* 'POST ' */
		ctl->method = HTTP_POST;
		cp += 5;
	} else {
		/* Invalid or unsupported method */
		return HTTP_NOT_IMPLEMENTED;
	}

	i = 0;
	uri = ctl->uri;
	while ((c = *cp++) != ' ') {
		if ((c == '\0') || (i >= HTTPD_URI_MAX_LEN)) {
			return HTTP_REQUEST_URI_TOO_LARGE;
		}
		uri[i++] = c;
	}
	uri[i] = '\0';

	if ((cp[0] = 'H') && (cp[1] = 'T') && (cp[2] = 'T') &&
		(cp[3] = 'P') && (cp[4] = '/') && (cp[5] = '1') && (cp[5] = '.')) {
		c = cp[7];
		if ((c >= '0') && (c <= '9')) {
			ctl->version = 10 + c - '0';
			return 0;
		}
	}

	/* Malformed request line, respond with: 400 Bad Request */
	return HTTP_BAD_REQUEST;
}

int http_parse_request(struct tcp_pcb * tp, struct httpctl * ctl)
{
	char * buf = (char *)ctl->rcvq.buf;
	int rem;
	int cnt;
	int c1;
	int n;

	rem = HTTP_RCVBUF_LEN; /* free space in the input buffer */
	cnt = 0; /* used space in the input buffer */
	c1 = '\0';

	/* receive and decode HTTP request */
	while ((n = tcp_recv(tp, &buf[cnt], rem)) > 0)  {
		int pos;
		int c2;

		rem -= n;
		pos = cnt;
		cnt += n;
		for (; pos < cnt; ++pos) {
			c2 = buf[pos];
			if (c1 == '\r' && c2 == '\n') {
				buf[pos - 1] = '\0';
				ctl->rcvq.head = cnt;
				ctl->rcvq.pos = pos + 1;
				ctl->rcvq.tail = pos + 1;
				return http_process_request(ctl, buf);
			}
			c1 = c2;
		}

		if (rem <= 0) {
			DCC_LOG(LOG_ERROR, "buffer ovreflow!");
			break;
		}
	}

	return -1;
}

int http_parse_multipart_form_data(struct httpctl * ctl, char * val);

static int http_process_field(struct httpctl * ctl,
		unsigned int hdr, char * val)
{
	char * cp;

	DCC_LOG1(LOG_INFO, "header field received: %s", http_hdr_name[hdr]);

	switch (hdr) {
	/* Request Header Fields */
	case HTTP_HDR_AUTHORIZATION:
		DCC_LOG1(LOG_INFO, "Authorization: %c ...", val);
		break;
	case HTTP_HDR_CONTENT_TYPE:
		ctl->content.type = http_parse_content_type(val, &cp);
		if (ctl->content.type == MULTIPART_FORM_DATA) {
			http_parse_multipart_form_data(ctl, cp);
		} else if (ctl->content.type == APPLICATION_X_WWW_FORM_URLENCODED) {
			DCC_LOGSTR(LOG_TRACE, "application/x-www-form-urlencoded; %s", cp);
		} else {
			DCC_LOG1(LOG_TRACE, "Content-Type: 0x%02x", ctl->content.type);
		}
		break;
	case HTTP_HDR_CONTENT_LENGTH:
		ctl->content.len = strtoul(val, NULL, 10);
		DCC_LOG1(LOG_INFO, "Content-Length: %d", ctl->content.len);
		break;
	case HTTP_HDR_CONTENT_DISPOSITION:
		DCC_LOG1(LOG_INFO, "Content-Disposition: %d", ctl->content.len);
		break;
	case HTTP_HDR_ACCEPT:
		DCC_LOG1(LOG_INFO, "Accept: %c ...", val[0]);
		break;
	case HTTP_HDR_ACCEPT_CHARSET:
		DCC_LOG1(LOG_INFO, "Accept-Charset: %c ...", val[0]);
		break;
	case HTTP_HDR_ACCEPT_ENCODING:
		DCC_LOG1(LOG_INFO, "Accept-Encoding: %c ...", val[0]);
		break;
	case HTTP_HDR_ACCEPT_LANGUAGE:
		DCC_LOG1(LOG_INFO, "Accept-Language: %c ...", val[0]);
		break;
	case HTTP_HDR_EXPECT:
		DCC_LOG1(LOG_INFO, "Expect: %c ...", val[0]);
		break;
	case HTTP_HDR_HOST:
		DCC_LOG1(LOG_INFO, "Host: %c ...", val[0]);
		break;
	case HTTP_HDR_IF_MATCH:
		DCC_LOG1(LOG_INFO, "If-Match: %c ...", val[0]);
		break;
	case HTTP_HDR_IF_MODIFIED_SINCE:
		DCC_LOG1(LOG_INFO, "If-Modified-Since: %c ...", val[0]);
		break;
	case HTTP_HDR_IF_NONE_MATCH:
		DCC_LOG1(LOG_INFO, "If-None-Match: %c ...", val[0]);
		break;
	case HTTP_HDR_IF_RANGE:
		DCC_LOG1(LOG_INFO, "If-Range: %c ...", val[0]);
		break;
	case HTTP_HDR_IF_UNMODIFIED_SINCE:
		DCC_LOG1(LOG_INFO, "If-Unmodified-Since: %c ...", val[0]);
		break;
	case HTTP_HDR_MAX_FORWARDS:
		DCC_LOG1(LOG_INFO, "Max-Forwards: %c ...", val[0]);
		break;
	case HTTP_HDR_PROXY_AUTHORIZATION:
		DCC_LOG1(LOG_INFO, "Proxy-Authorization: %c ...", val[0]);
		break;
	case HTTP_HDR_RANGE:
		DCC_LOG1(LOG_INFO, "Range: %c ...", val[0]);
		break;
	case HTTP_HDR_REFERER:
		DCC_LOG1(LOG_INFO, "Referer: %c ...", val[0]);
		break;
	case HTTP_HDR_TE:
		DCC_LOG1(LOG_INFO, "Te: %c ...", val[0]);
		break;
	case HTTP_HDR_USER_AGENT:
		DCC_LOG1(LOG_INFO, "User-Agent: %c ...", val[0]);
		break;
	/* Hop-by-Hop Headers */
	case HTTP_HDR_CONNECTION:
		DCC_LOG1(LOG_INFO, "Connection: %c ...", val[0]);
		break;
	case HTTP_HDR_TRAILER:
		DCC_LOG1(LOG_INFO, "Trailers: %c ...", val[0]);
		break;
	case HTTP_HDR_PROXY_AUTHENTICATE:
		DCC_LOG1(LOG_INFO, "Proxy-Authenticate: %c ...", val[0]);
		break;
	case HTTP_HDR_TRANSFER_ENCODING:
		DCC_LOG1(LOG_INFO, "Transfer-Encoding: %c ...", val[0]);
		break;
	case HTTP_HDR_UPGRADE:
		DCC_LOG1(LOG_INFO, "Upgrade: %c ...", val[0]);
		break;
	case HTTP_HDR_CACHE_CONTROL:
		DCC_LOG1(LOG_INFO, "Cache-Control: %c ...", val[0]);
		break;
	default:
		DCC_LOG2(LOG_INFO, "hdr=%02x: %c ...", hdr, val[0]);
	}

	return 0;
}

int http_parse_header(struct tcp_pcb * tp, struct httpctl * ctl)
{
	char * buf = (char *)ctl->rcvq.buf;
	int cnt;
	int pos;
	int lin;
	int c1;

	/* total bytes in the buffer */
	cnt = ctl->rcvq.head;
	/* processed bytes so far */
	pos = ctl->rcvq.pos;
	/* beginning of a line */
	lin = ctl->rcvq.tail;
	/* current character */
	c1 = (pos) ? buf[pos - 1] : '\0';

	/* receive and decode HTTP headers */
	for (;;) {
		int rem;
		int c2;
		int n;

		/* search for end of line */
		while (pos < cnt) {
			/* lookahead character */
			c2 = buf[pos++];
			/* search for CR+LF */
			if (c1 == '\r' && c2 == '\n') {
				char * val;
				unsigned int hdr;

				/* If the current position is 2 characters ahead of
				   the line start then this is an empty line. */
				if (pos == lin + 2) {
					/* end of HTTP Header */
					if (cnt == pos) {
						cnt = 0;
						pos = 0;
					} else {
						int i;
						int j;
						int n;
						/* move what was left in the receiving queue to the
						 * beginning of the buffer */
						n = cnt - pos;
						for (i = 0, j = pos; i < n; ++i, ++j)
							buf[i] = buf[j];
						cnt = n;
						pos = 0;
					}
					ctl->rcvq.head = cnt;
					ctl->rcvq.pos = pos;
					ctl->rcvq.tail = pos;
					buf[cnt] = '\0';
					return 0;
				}

				/* header field */
				buf[pos - 2] = '\0';
				DCC_LOGSTR(LOG_TRACE, "\"%s\"", &buf[lin]);

				if ((hdr = http_parse_field(&buf[lin], &val)) == 0) {
					DCC_LOGSTR(LOG_WARNING, "http_parse_field(\"%s\") failed!", 
							   &buf[lin]);
					return -1;
				}

				if (http_process_field(ctl, hdr, val) < 0) {
					DCC_LOG1(LOG_WARNING, "http_process_field(%d) failed!", 
							 &buf[lin]);
					return -1;
				}
				/* move to the next line */
				lin = pos;
			}
			c1 = c2;
		}

		/* if we reached the end of the buffer ... */
		if (HTTP_RCVBUF_LEN == cnt) {
			int i;
			int j;

			/* if the line marker is at position zero then we've reached the
			 * end of the buffer with no CR+LF detected in it... */
			if (lin == 0) {
				DCC_LOG(LOG_ERROR, "buffer ovreflow!");
				return -1;
			}

			/* move a partial line to the beginning of the buffer */
			n = cnt - lin;
			for (i = 0, j = lin; i < n; ++i, ++j)
				buf[i] = buf[j];

			/* update indexes */
			cnt = n;
			pos = n;
			lin = 0;
		}

		/* get free space at the end of input buffer */
		rem = HTTP_RCVBUF_LEN - cnt;

		/* read more data */
		if ((n = tcp_recv(tp, &buf[cnt], rem)) <= 0) {
			DCC_LOG(LOG_WARNING, "tcp_recv() failed!");
			return n;
		}

		/* update the data counter */
		cnt += n;
	}

	return -1;
}

int http_accept(struct httpd * httpd, struct httpctl * ctl)
{
	struct tcp_pcb * tp;
	int code;

	DCC_LOG1(LOG_INFO, "httpd=%p.", httpd);

	if ((tp = tcp_accept(httpd->tp)) == NULL) {
		DCC_LOG(LOG_ERROR, "tcp_accept().");
		return -1;
	}

	memset(ctl, 0, sizeof(struct httpctl));

	DCC_LOG1(LOG_INFO, "ctl=%p accepted.", ctl);

	if ((code = http_parse_request(tp, ctl)) != 0) {
		DCC_LOG1(LOG_WARNING, "http_parse_request() failed with code %d.", 
				 code);
		switch (code) {
		case HTTP_BAD_REQUEST:
			httpd_400(tp);
			break;
		case HTTP_REQUEST_URI_TOO_LARGE:
			httpd_414(tp);
			break;
		case HTTP_NOT_IMPLEMENTED:
			httpd_501(tp);
			break;
		}
		tcp_close(tp);
		return -1;
	}

	if (http_parse_header(tp, ctl) < 0) {
		DCC_LOG(LOG_WARNING, "http_parse_header() failed!");
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



int http_close(struct httpctl * ctl)
{
	DCC_LOG(LOG_INFO, "...");
	return tcp_close(ctl->tp);
}

const char http_index_obj[] = "index.html";


const char * http_uri_get(struct httpctl * http)
{
	return http->uri;
}

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

	DCC_LOGSTR(LOG_INFO, "path=\"%s\"", path);

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

	DCC_LOGSTR(LOG_INFO, "obj=\"%s\"", oid);

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

/* ---------------------------------------------------------------------------
 * HTTP server
 * ---------------------------------------------------------------------------
 */

const struct httpdauth httpd_def_authlst[] = {
	{ .uid = 0, .lvl = 0, .name = "", .passwd = "" }
};

int http_parser_test(void);

int httpd_init(struct httpd * httpd,
							 int port, int backlog,
							 const struct httpddir dirlst[],
							 const struct httpdauth authlst[])
{
	struct tcp_pcb * tp;

//	http_parser_test();

	if (httpd == NULL) {
		DCC_LOG(LOG_ERROR, "Invalid parameter!");
		return -1;
	}

	if ((tp = tcp_alloc()) == NULL) {
		DCC_LOG(LOG_ERROR, "Can't alloc TCP PCB!");
		return -2;
	}

	tcp_bind(tp, INADDR_ANY, htons(port));

	if (tcp_listen(tp, backlog) != 0) {
		DCC_LOG(LOG_ERROR, "Can't register the TCP listener!");
		return -3;
	}

	httpd->tp = tp;
	httpd->dir = dirlst;

	if (authlst == NULL)
		httpd->auth = httpd_def_authlst;
	else
		httpd->auth = authlst;

	return 0;
}

/* -------------------------------------------------------------------------
 * Pool of resources
 * ------------------------------------------------------------------------- */

static struct httpd httpd_pool[HTTPD_POOL_SIZE];

struct httpd * httpd_alloc(void)
{
	struct httpd * httpd;
	int i;

	for (i = 0; i < HTTPD_POOL_SIZE; ++i) {
		httpd = &httpd_pool[i];
		if (!httpd->auth) {
			httpd->auth = httpd_def_authlst;
			return httpd;
		}
	}

	return httpd;
}
