/* $Id: ifnet.c,v 2.7 2008/06/04 00:03:15 bob Exp $ 
 *
 * File:	ifn_get_next.c
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

struct ifnet * ifn_get_next(struct ifnet * __if)
{
	struct ifnet * ifn = NULL;
	int i;

	tcpip_net_lock();

	for (i = 0; i < ifnet_max; i++) {
		/* naive lookup method to avoid division */
		if (&__ifnet__[i] == __if) {
			break;
		}
	}

	for (; i < ifnet_max; i++) {
		if (__ifnet__[i].if_id != 0) {
			ifn = &__ifnet__[i];
		}
	}

	tcpip_net_unlock();

	return ifn;
}

