/* 
 * File:	http_get.c
 * Module:  
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bobmittmann@gmail.com)
 * Target:	
 * Comment: 
 * Copyright(c) 2005-2009 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include "httpd-i.h"

const char http_index_obj[] = "index.html";

int http_get(struct httpctl * ctl)
{
	int (* cgi)(struct httpctl * ctl);
	struct httpd * httpd = ctl->httpd;
	char path[HTTPD_URI_MAX_LEN];
	const struct httpdobj * obj;
	const struct httpddir * dir;
	char * dst;
	char * src;
	char * sep;
	char * oid;
//	char * cp;
	int ret;
	int c;

	dst = path;
	sep = path;
	oid = ctl->uri;

	for (src = ctl->uri; (c = *src) != '\0'; ++src) {
		if (c == '/') {
			oid = src + 1;
			sep = dst;
		}
		*dst++ = c;
	}
	*sep = '\0';

	if (path[0] == '\0') {
		path[0] = '/';
		path[1] = '\0';
	}

#if 0
	DCC_LOG(LOG_TRACE, "PATH");
	for (cp = path; (c = *cp) != '\0'; ++cp) {
		DCC_LOG1(LOG_TRACE, "'%c'", c);
	}
#endif

	for (dir = httpd->dir; (dir->path != NULL); ++dir) {
		if (strcmp(dir->path, path) == 0) {
			break;
		};
	}
	
	if (dir->path == NULL) {
		DCC_LOG(LOG_WARNING, "404 File Not Found (invalid path)");
		httpd_404(ctl->tp);
		return -1;
	}

	if (*oid == '\0')
		oid = (char *)http_index_obj;

#if 0
	DCC_LOG(LOG_TRACE, "OID");
	for (cp = oid; (c = *cp) != '\0'; ++cp) {
		DCC_LOG1(LOG_TRACE, "'%c'", c);
	}
#endif

	for (obj = dir->objlst; (obj->oid != NULL); ++obj) {
		if (strcmp(obj->oid, oid) == 0) {
			break;
		};
	}

	if (obj->oid == NULL) {
		DCC_LOG(LOG_WARNING, "404 File Not Found (invalid OID)");
		httpd_404(ctl->tp);
		return -1;
	}

	ret = -1;

	switch (obj->typ) {
	case OBJ_STATIC_HTML:
		httpd_200_html(ctl->tp); 
		ret = tcp_send(ctl->tp, obj->ptr, obj->len, 0);
		break;

	case OBJ_STATIC_JS:
		break;
	case OBJ_STATIC_TEXT:
		break;
	case OBJ_STATIC_CSS:
		httpd_200_css(ctl->tp); 
		ret = tcp_send(ctl->tp, obj->ptr, obj->len, 0);
		break;
	case OBJ_STATIC_XML:
		break;
	case OBJ_STATIC_GIF:
		break;
	case OBJ_STATIC_JPEG:
		break;
	case OBJ_STATIC_PNG:
		break;
	case OBJ_STATIC_MPEG:
		break;
	case OBJ_CODE_CGI:
		cgi = (int (*)(struct httpctl *))obj->ptr;
		ret = cgi(ctl);
		break;
	case OBJ_CODE_JS:
		break;
	}

	return ret;
}

