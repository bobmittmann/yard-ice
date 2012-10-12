/* $Id: inet_addr.c,v 2.0 2006/03/28 15:10:27 bob Exp $ 
 *
 * File:	inet_addr.c
 * Module:	ulibc
 * Project:	
 * Author:	Carlos Augusto Vieira e Vieira <carlos.vieira@boreste.com>
 * Target:
 * Comment:
 * Copyright(c) 2004 CNX Technologies. All Rights Reserved.
 *
 */

#include <arpa/inet.h>

in_addr_t inet_addr(const char * cp)
{
	struct in_addr dummy;
	if (inet_aton(cp, &dummy) == 0)
		return INADDR_NONE;
	return dummy.s_addr;
}

