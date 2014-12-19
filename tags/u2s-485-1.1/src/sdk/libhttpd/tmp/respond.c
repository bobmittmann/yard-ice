/* $Id: respond.c,v 2.2 2006/09/11 02:55:11 bob Exp $ 
 *
 * File:	respond.c
 * Module:  
 * Project:
 * Author:	Robinson Mittmann (bob@boreste.com)
 * Target:
 * Comment:
 * Copyright(c) 2004-2006 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include <tcpip/httpd.h>
#include "httpdefs.h"

#ifdef HTTPD_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif
#include <debug.h>

int httpd_respond(struct tcp_pcb * __tp, const char * __msg, int __len)
{
	int err;
	
	err = tcp_send(__tp, __msg, __len, TCP_SEND_NOCOPY);
	
	if (err <= 0) {
		DBG(DBG_WARNING, "tcp_send error!");
	}

	tcp_close(__tp);

	return err;
}

