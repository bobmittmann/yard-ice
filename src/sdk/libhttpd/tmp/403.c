/* 
 *
 * File:	403.c
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

static const char http_msg_403[] = HTTP_MSG(403, "Forbidden", "");

int httpd_403(struct tcp_pcb * __tp) 
{
	return tcp_send(__tp, http_msg_403, sizeof(http_msg_403) - 1, TCP_SEND_NOCOPY);
}


