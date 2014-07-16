/* 
 * File:	404.c
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

static const char http_msg_404[] = HTTP_MSG(404, "Not Found", 
	"Sorry, the page you are requesting was not found on this server.");

int httpd_404(struct tcp_pcb * __tp) 
{
	return tcp_send(__tp, http_msg_404, 
					sizeof(http_msg_404) - 1, TCP_SEND_NOCOPY);
}

