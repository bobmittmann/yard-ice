/* 
 *
 * File:	http_send.c
 * Module:  
 * Project:
 * Author:	Robinson Mittmann (bob@boreste.com, bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(c) 2004-2006 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include <stdlib.h>
#include <string.h>

#include <tcpip/httpd.h>
#include "httpdefs.h"

#ifdef HTTPD_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif
#include <debug.h>

/* Preformatted http header */
static const char http_hdr_200[] = "HTTP/1.1 200 OK\r\n"
	"Server: " HTTPD_SERVER "\r\n";

int httpd_send(struct tcp_pcb * __tp, 
	mimetype_t __type, char * __content, int __len)
{
	char * ct;
	int err;

	DBG(DBG_TRACE, "data=0x%p size=%d type=%d", 
		__content, __len, __type);

	if ((ct = (char *)http_content_type[__type]) == NULL) {
		DBG(DBG_ERROR, "415 Unsupported Media Type: %d", __type);
		httpd_415(__tp);
		return -1;
	}

	err = tcp_send(__tp, http_hdr_200, sizeof(http_hdr_200) - 1, 
				   TCP_SEND_NOCOPY);
	if (err > 0)
		err = tcp_send(__tp, ct, strlen(ct), TCP_SEND_NOCOPY);

	if (err > 0)
		err = tcp_send(__tp, __content, __len, TCP_SEND_NOCOPY);

	if (err < 0) {
		DBG(DBG_WARNING, "tcp_send error");
	}

	tcp_close(__tp);
	
	return err;
}

