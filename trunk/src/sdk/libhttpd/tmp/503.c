/* $Id: 503.c,v 2.1 2006/04/06 18:16:26 bob Exp $ 
 *
 * File:	503.c
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

static const char http_msg_503[] = HTTP_MSG(503, "Service Unavailable", "");

int httpd_503(struct tcp_pcb * __tp)
{
	return tcp_send(__tp, http_msg_503, sizeof(http_msg_503) - 1, TCP_SEND_NOCOPY);
}

