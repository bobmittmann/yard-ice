/* $Id: 413.c,v 2.1 2006/04/06 18:16:26 bob Exp $ 
 *
 * File:	413.c
 * Module:  
 * Project:
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:
 * Comment:
 * Copyright(c) 2004-2006 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include <tcpip/httpd.h>
#include "httpdefs.h"

static const char http_msg_413[] = HTTP_MSG(413, 
	"Request Entity Too Large", "");

int httpd_413(struct tcp_pcb * __tp) 
{
	return tcp_send(__tp, http_msg_413, sizeof(http_msg_413) - 1, TCP_SEND_NOCOPY);
}

