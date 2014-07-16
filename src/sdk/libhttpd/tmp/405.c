/*
 *
 * File:	405.c
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

static const char http_msg_405[] = HTTP_MSG(405, "Method Not Allowed", "");

int httpd_405(struct tcp_pcb * __tp) 
{
	return tcp_send(__tp, http_msg_405, 
					sizeof(http_msg_405) - 1, TCP_SEND_NOCOPY);
}

