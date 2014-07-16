/*
 *
 * File:	401.c
 * Module:  
 * Project:
 * Author:	Robinson Mittmann (bob@boreste.com, bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(c) 2004-2009 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include <tcpip/httpd.h>
#include "httpdefs.h"

static const char http_msg_401[] = HTTP_MSG(401, "Unauthorized", "");

static const char http_msg_401_auth[] = "HTTP/1.1 401 Unauthorized\r\n"\
	"Server: " HTTPD_SERVER "\r\n"\
	"WWW-Authenticate: Basic realm=\"" HTTPD_SERVER "\"\r\n"\
	"Content-type: text/html\r\n\r\n"\
	"<html><head><title> 401</title></head>"\
	"<h2> 401 - Unauthorized</h2>" HTML_MSG_FOOTER;

int httpd_401(struct tcp_pcb * __tp) 
{
	return tcp_send(__tp, http_msg_401, 
					sizeof(http_msg_401) - 1, TCP_SEND_NOCOPY);
}

int httpd_401_auth(struct tcp_pcb * __tp) 
{
	return tcp_send(__tp, http_msg_401_auth, 
		sizeof(http_msg_401_auth) - 1, TCP_SEND_NOCOPY);
}

