/*
 * File:	ifn_ipconfig.c
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:	
 * Comment:
 * Copyright(c) 2004-2009 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#define __USE_SYS_IFNET__
#include <sys/ifnet.h>

#include <stdlib.h>

int ifn_ipv4_get(struct ifnet * __if, in_addr_t * __addr, in_addr_t * __mask)
{
	if (__if == NULL)
		return -1;

	tcpip_net_lock();

	if (__addr != NULL)
		*__addr = __if->if_ipv4_addr;

	if (__mask != NULL)
		*__mask = __if->if_ipv4_mask;

	tcpip_net_unlock();

	return 0;
}

