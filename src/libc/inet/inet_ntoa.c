/* $Id: inet_ntoa.c,v 2.0 2006/03/28 15:10:27 bob Exp $ 
 *
 * File:	inet_ntoa.c
 * Module:
 * Project:	AT91X40DK	
 * Author:	Robinson Mittmann (bob@cnxtech.com)
 * Target:	at91x40
 * Comment:
 * Copyright(c) 2003 CNX Technologies. All Rights Reserved.
 *
 */

#ifdef __CONFIG__
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <tcpip/in.h>

char * inet_ntoa(struct in_addr addr)
{
	static char buf[15];
	
	sprintf(buf, "%d.%d.%d.%d", IP4_ADDR1(addr.s_addr),
		IP4_ADDR2(addr.s_addr), IP4_ADDR3(addr.s_addr), 
		IP4_ADDR4(addr.s_addr));	

	return buf;
}
