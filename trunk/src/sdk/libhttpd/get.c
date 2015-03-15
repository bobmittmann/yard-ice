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

int http_get(struct httpctl * ctl, const struct httpdobj * obj)
{
	int (* cgi)(struct httpctl * ctl);
	int ret = -1;

	switch (obj->typ) {
	case OBJ_STATIC_HTML:
	case OBJ_STATIC_JS:
	case OBJ_STATIC_TEXT:
	case OBJ_STATIC_CSS:
	case OBJ_STATIC_HTML_GZ:
	case OBJ_STATIC_JS_GZ:
	case OBJ_STATIC_TEXT_GZ:
	case OBJ_STATIC_CSS_GZ:
	case OBJ_STATIC_JPEG:
	case OBJ_STATIC_PNG:
		httpd_200(ctl->tp, obj->typ); 
		ret = tcp_send(ctl->tp, obj->ptr, obj->len, 0);
		break;
	case OBJ_CODE_CGI:
		cgi = (int (*)(struct httpctl *))obj->ptr;
		ret = cgi(ctl);
		break;
	}

	return ret;
}

