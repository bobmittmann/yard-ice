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

int ifn_ipv4_set(struct ifnet * __if, in_addr_t __addr, in_addr_t __mask)
{
	if (__if == NULL)
		return -1;

	tcpip_net_lock();

	/* 
	 * TODO: remove all routes bounded to this interface 
	 */
	if (__if->if_ipv4_addr)
		route_del(__if->if_ipv4_addr & __if->if_ipv4_mask);

	__if->if_ipv4_addr = __addr;
	__if->if_ipv4_mask = __mask;

	if (__if->if_ipv4_addr) {
		route_add(__if->if_ipv4_addr & __if->if_ipv4_mask, 
		__if->if_ipv4_mask, 0, __if);
	}

	DCC_LOG4(LOG_TRACE, "%s%d - addr=%I mask=%I", 
		ifn_type_name[__if->if_id >> 4], __if->if_id & 0x0f,
		__if->if_ipv4_addr, __if->if_ipv4_mask);

	tcpip_net_unlock();

	return 0;
}

