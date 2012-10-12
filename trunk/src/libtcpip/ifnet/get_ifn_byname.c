/* $Id: ifnet.c,v 2.7 2008/06/04 00:03:15 bob Exp $ 
 *
 * File:	get_ifn_byname.c
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

#include <tcpip/ifnet.h>

#include <string.h>
#include <stdlib.h>

struct ifnet * get_ifn_byname(const char * __s)
{
	struct ifnet * ifn = NULL;
	char nm[8];
	int i;

	tcpip_net_lock();

	for (i = 0; i < ifnet_max; i++) {
		/* naive lookup method to avoid division */
		if (__ifnet__[i].if_id != 0) {
			ifn = &__ifnet__[i];
			ifn_getname(ifn, nm);
			if (strcmp(nm, __s) == 0)
				break;
		}
	}

	tcpip_net_unlock();

	return ifn;
}

