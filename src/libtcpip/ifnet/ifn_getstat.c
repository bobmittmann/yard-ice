/* 
 * File:	ifn_getstat.c
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bobmittmann@gmail.com)
 * Target:	
 * Comment:
 * Copyright(c) 2009 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#define __USE_SYS_IFNET__
#include <sys/ifnet.h>

#include <tcpip/ifnet.h>

#include <string.h>

void ifn_getstat(struct ifnet * __if, struct ifnet_stat * __st, int __rst)
{
#if ENABLE_NETIF_STAT
	if (__if == NULL) {
		DCC_LOG(LOG_WARNING, "null pointer");
		return;
	}

	tcpip_net_lock();
	if (__st != NULL) {
		memcpy(__st, &__if->stat, sizeof(struct ifnet_stat)); 
		if (__rst) {
			memset(__st, 0, sizeof(struct ifnet_stat)); 
		}
	}
	tcpip_net_unlock();
#endif
}


