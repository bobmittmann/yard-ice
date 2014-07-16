/* $Id: 500.c,v 2.1 2006/04/06 18:16:26 bob Exp $ 
 *
 * File:	500.c
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

static const char http_msg_500[] = HTTP_MSG(500, "Internal Server Error", "");

int httpd_500(struct tcp_pcb * __tp)
{
	return tcp_send(__tp, http_msg_500, sizeof(http_msg_500) - 1, TCP_SEND_NOCOPY);
}

