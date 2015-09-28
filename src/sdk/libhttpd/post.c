/* 
 * File:	post.c
 * Module:  
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bobmittmann@gmail.com)
 * Target:	
 * Comment: 
 * Copyright(c) 2005-2009 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include "httpd-i.h"
#include "http_hdr.h"

#define HEX2INT(C) (((C) <= '9') ? (C) - '0' : (C) - ('A' - 10))

#ifndef HTTP_QUERY_KEY_MAX
#define HTTP_QUERY_KEY_MAX 16
#endif

int http_decode_uri_query(char * buf, int len, 
						  struct httpqry lst[], int max)  
{
	int n;
	int i;
	int j;
	int c;

	n = 0;
	lst[n].key = buf;
	for (i = 0, j = 0; i < len; i++) {
		c = buf[i];
		/* translate */
		switch (c) {
		case '+':
			buf[j++] = ' ';
			break;
		case '%':
			c = (HEX2INT(buf[i + 1]) << 4) + HEX2INT(buf[i + 2]);
			i += 2;
			buf[j++] = c;
			break;
		case '&':
		case ';':
			buf[j++] = '\0';
			if (n < HTTP_QUERY_KEY_MAX) {
				lst[n].key = &buf[j];
				lst[n].val = "\0";
			}
			break;
		case '=':
			buf[j++] = '\0';
			lst[n++].val = &buf[j];
			break;
		case '\0':
			goto done;
		case '#':
			/* FIXME: implement fragment optional part */
			break;
		default:
			buf[j++] = c;
		}
	}

done:
	buf[j] = '\0';
	return n;
}

char * http_query_lookup(struct httpctl * ctl, char * key)
{
	struct httpqry * lst = ctl->qrylst;
	int cnt = ctl->qrycnt;
	int i;

	for (i = 0; i < cnt; ++i) {
		if (strcmp(lst[i].key, key) == 0)
			return lst[i].val;
	}

	return "\0";
}

int http_post(struct httpctl * ctl, const struct httpdobj * obj)
{
	int (* cgi)(struct httpctl * ctl);
	int content_type;
	int content_len;
	int len;
	int ret;

	if (obj->typ != OBJ_CODE_CGI) {
		DCC_LOG(LOG_ERROR, "invalid object type!");
		return -1;
	}

	content_type = ctl->content.type;
	content_len = ctl->content.len;

	if (content_type == APPLICATION_X_WWW_FORM_URLENCODED) {
		char *  buf;
		int n;

		if (content_len == 0) {
			DCC_LOG(LOG_ERROR, "invalid content length!");
			return -1;
		}

		DCC_LOG(LOG_TRACE, "application/x-www-form-urlencoded");

		if ((len = http_content_recv(ctl)) <= 0) {
			return len;
		}
		buf = (char *)ctl->rcvq.buf;

		n = http_decode_uri_query(buf, len, ctl->qrylst, HTTPD_QUERY_LST_MAX);
		if (n < 0) {
			/* FIXME: return error code */
			DCC_LOG(LOG_ERROR, "http_decode_uri_query() failed!");
			return -1;
		}
		ctl->qrycnt = n;

	} else if (content_type == MULTIPART_FORM_DATA) {
		DCC_LOG(LOG_TRACE, "multipart/form-data");

		http_recv_queue_shift(ctl);

		if (http_multipart_boundary_lookup(ctl) < 0) {
			DCC_LOG(LOG_WARNING, "http_parse_header() failed!");
			/* Malformed request line, respond with: 400 Bad Request */
			httpd_400(ctl->tp);
			tcp_close(ctl->tp);
			return -1;
		}

		http_recv_queue_shift(ctl);

		if (http_parse_header(ctl->tp, ctl) < 0) {
			DCC_LOG(LOG_WARNING, "http_parse_header() failed!");
			/* Malformed request line, respond with: 400 Bad Request */
			httpd_400(ctl->tp);
			tcp_close(ctl->tp);
			return -1;
		}

	} else {
		/* dynamic cgi */
		DCC_LOG1(LOG_TRACE, "content_type=%02x", content_type);
	}

	cgi = (int (*)(struct httpctl *))obj->ptr;
	ret = cgi(ctl);

	return ret;
}

