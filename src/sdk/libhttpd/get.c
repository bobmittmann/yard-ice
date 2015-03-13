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
		httpd_200_jpeg(ctl->tp); 
		ret = tcp_send(ctl->tp, obj->ptr, obj->len, 0);
		break;
	case OBJ_STATIC_PNG:
		httpd_200_png(ctl->tp); 
		ret = tcp_send(ctl->tp, obj->ptr, obj->len, 0);
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

