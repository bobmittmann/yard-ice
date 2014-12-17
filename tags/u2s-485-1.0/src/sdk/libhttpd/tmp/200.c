/*
 *
 * File:	200.c
 * Module:  
 * Project:
 * Author:	Robinson Mittmann (bob@boreste.com, bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(c) 2004-2006 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include <tcpip/httpd.h>
#include "httpdefs.h"
#include <debug.h>

/* Preformatted http headers */
const char http_hdr_200[] = "HTTP/1.1 200 OK\r\n"
	"Server: " HTTPD_SERVER "\r\n";

const char http_hdr_200_html[] = "HTTP/1.1 200 OK\r\n"
	"Server: " HTTPD_SERVER "\r\n"
	"Content-type: text/html;charset=ISO-8859-1\r\n\r\n";

const char http_hdr_200_gif[] = "HTTP/1.1 200 OK\r\n"
	"Server: " HTTPD_SERVER "\r\n"
	"Content-type: image/gif\r\n\r\n";
	
const char http_hdr_200_png[] = "HTTP/1.1 200 OK\r\n"
	"Server: " HTTPD_SERVER "\r\n"
	"Content-type: image/png\r\n\r\n";

const char http_hdr_200_jpeg[] = "HTTP/1.1 200 OK\r\n"
	"Server: " HTTPD_SERVER "\r\n"
	"Content-type: image/jpeg\r\n\r\n";

const char http_hdr_200_text[] = "HTTP/1.1 200 OK\r\n"
	"Server: " HTTPD_SERVER "\r\n"
	"Content-type: text/plain\r\n\r\n";

const char http_hdr_200_xml[] = "HTTP/1.1 200 OK\r\n"
	"Server: " HTTPD_SERVER "\r\n"
	"Content-type: text/xml\r\n\r\n";

const char http_hdr_200_mpeg[] = "HTTP/1.1 200 OK\r\n"
	"Server: " HTTPD_SERVER "\r\n"
	"Content-type: video/mpeg\r\n\r\n";

const char http_hdr_200_css[] = "HTTP/1.1 200 OK\r\n"
	"Server: " HTTPD_SERVER "\r\n"
	"Content-type: text/css\r\n\r\n";

const char http_hdr_200_javascript[] = "HTTP/1.1 200 OK\r\n"
	"Server: " HTTPD_SERVER "\r\n"
	"Content-type: application/x-javascript\r\n\r\n";

int httpd_200(struct tcp_pcb * __tp, mimetype_t __type)
{
	char * hdr;
	int len;

	DBG(DBG_TRACE, "type=%d", __type);

	switch (__type) {
	case text_html:
		hdr = (char *)http_hdr_200_html;
		len = sizeof(http_hdr_200_html) - 1;
		break;
	case image_gif:
		hdr = (char *)http_hdr_200_gif;
		len = sizeof(http_hdr_200_gif) - 1;
		break;
	case image_png:
		hdr = (char *)http_hdr_200_png;
		len = sizeof(http_hdr_200_png) - 1;
		break;
	case image_jpeg:
		hdr = (char *)http_hdr_200_jpeg;
		len = sizeof(http_hdr_200_jpeg) - 1;
		break;
	case text_plain:
		hdr = (char *)http_hdr_200_text;
		len = sizeof(http_hdr_200_text) - 1;
		break;
	case text_xml:
		hdr = (char *)http_hdr_200_xml;
		len = sizeof(http_hdr_200_xml) - 1;
		break;
	case text_css:
		hdr = (char *)http_hdr_200_css;
		len = sizeof(http_hdr_200_css) - 1;
		break;
	case video_mpeg:
		hdr = (char *)http_hdr_200_mpeg;
		len = sizeof(http_hdr_200_mpeg) - 1;
		break;
	case application_x_javascript:
		hdr = (char *)http_hdr_200_javascript;
		len = sizeof(http_hdr_200_javascript) - 1;
		break;
	default:
		DBG(DBG_ERROR, "415 Unsupported Media Type: %d", __type);
		httpd_415(__tp);
		return -1;
	}

	return tcp_send(__tp, hdr, len, 0);
}

int httpd_200_html(struct tcp_pcb * __tp) 
{
	return tcp_send(__tp, http_hdr_200_html, 
					sizeof(http_hdr_200_html) - 1, 0);
}

