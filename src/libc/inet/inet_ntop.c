/* $Id: inet_ntop.c,v 2.0 2006/03/28 15:10:27 bob Exp $ 
 *
 * File:	inet_ntop.c
 * Module:
 * Project:	AT91X40DK	
 * Author:	Robinson Mittmann (bob@cnxtech.com)
 * Target:	at91x40
 * Comment:
 * Copyright(c) 2003 CNX Technologies. All Rights Reserved.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>

const char * inet_ntop(int af, const void * cp, char * buf, size_t len)
{
	char * ip;
	
	if (af != AF_INET)
		return NULL;

	ip = (char *)cp;

	snprintf(buf, len, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

	return buf;
}
