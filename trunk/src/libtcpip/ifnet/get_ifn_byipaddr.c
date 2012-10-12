/* $Id: ifnet.c,v 2.7 2008/06/04 00:03:15 bob Exp $ 
 *
 * File:	netif.c
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:	
 * Comment:
 * Copyright(c) 2004-2008 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#define __USE_SYS_IFNET__
#include <sys/ifnet.h>

#include <stdlib.h>

struct ifnet * get_ifn_byipaddr(in_addr_t __addr)
{
	struct ifnet * ifn = NULL;
	int i;

	tcpip_net_lock();

	for (i = 0; i < ifnet_max; i++) {
		/* naive lookup method to avoid division */
		if (__ifnet__[i].if_id != 0) {
			ifn = &__ifnet__[i];
			if (ifn->if_ipv4_addr == __addr)
				break;
		}
	}

	tcpip_net_unlock();

	return ifn;
}

