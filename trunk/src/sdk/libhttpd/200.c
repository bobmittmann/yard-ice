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

#include "mime_types.h"
#include "httpd-i.h"
#include "list.h"

#include <sys/dcclog.h>

const uint16_t mime_type_lst[] = {
	8,
	APPLICATION_JAVASCRIPT,
	APPLICATION_XML,
	IMAGE_GIF,
	IMAGE_JPEG,
	IMAGE_PNG,
	TEXT_CSS,
	TEXT_HTML,
	TEXT_PLAIN,
	VIDEO_MPEG,
};

/* name of various mime types we support */
const char * const mime_name_lut[] = {
	"",
	"application/javascript"
	"application/xml",
	"image/gif",
	"image/jpeg",
	"image/png",
	"text/css",
	"text/html;charset=ISO-8859-1",
	"text/plain",
	"video/mpeg",
	};

/* Preformatted http header */
const char http_hdr_200_html[] = "HTTP/1.1 200 OK\r\n"
	"Server: " HTTPD_SERVER_NAME "\r\n"
	"Content-type: text/html;charset=ISO-8859-1\r\n\r\n";

int httpd_200(struct tcp_pcb * __tp, int __type)
{
	char * hdr;
	int len;
	int idx;
	int ret;
	char buf[64];

	if ((idx = u16_list_indexof(mime_type_lst, __type)) < 0) {
		DCC_LOG1(LOG_TRACE, "Unsupported MIME type %d!", __type);
		return -EINVAL;
	}

	hdr = (char *)http_hdr_200_html;
	len = sizeof(http_hdr_200_html) - 
		sizeof("text/html;charset=ISO-8859-1\r\n\r\n");

	if ((ret = tcp_send(__tp, hdr, len, 0) < 0)) {
		DCC_LOG(LOG_ERROR, "tcp_send() failed!");
		return ret;
	}

	len = sprintf(buf, "%s\r\n\r\n", mime_name_lut[idx]);

	if ((ret = tcp_send(__tp, buf, len, 0) < 0)) {
		DCC_LOG(LOG_ERROR, "tcp_send() failed!");
	}

	return ret;
}

int httpd_200_html(struct tcp_pcb * __tp) 
{
	char * hdr;
	int len;
	int ret;

	hdr = (char *)http_hdr_200_html;
	len = sizeof(http_hdr_200_html) - 1;

	if ((ret = tcp_send(__tp, hdr, len, 0) < 0)) {
		DCC_LOG(LOG_ERROR, "tcp_send() failed!");
	}

	return ret;
}

