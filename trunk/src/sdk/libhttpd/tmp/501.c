/* $Id: 501.c,v 2.1 2006/04/06 18:16:26 bob Exp $ 
 *
 * File:	501.c
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

static const char http_msg_501[] = HTTP_MSG(501, "Not Implemented", "");

int httpd_501(struct tcp_pcb * __tp)
{
	return tcp_send(__tp, http_msg_501, sizeof(http_msg_501) - 1, TCP_SEND_NOCOPY);
}

