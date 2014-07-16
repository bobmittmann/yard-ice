/*
 *
 * File:	415.c
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

static const char http_msg_415[] = HTTP_MSG(415, "Unsupported Media Type", "");

int httpd_415(struct tcp_pcb * __tp)
{
	return tcp_send(__tp, http_msg_415, 
					sizeof(http_msg_415) - 1, TCP_SEND_NOCOPY);
}

