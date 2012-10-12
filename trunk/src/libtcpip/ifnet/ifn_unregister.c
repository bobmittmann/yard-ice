/* $Id: ifnet.c,v 2.7 2008/06/04 00:03:15 bob Exp $ 
 *
 * File:	ifn_unregister.c
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

int ifn_unregister(struct ifnet * __if)
{
	struct ifnet * ifn = NULL;
	int ret = -1;
	int i;

	tcpip_net_lock();

	for (i = 0; i < ifnet_max; i++) {
		/* naive lookup method to avoid division */
		if (ifn == (&__ifnet__[i])) {
			if (ifn->if_id != 0) {
				if (__if->if_op->op_cleanup != NULL) {
					ret = __if->if_op->op_cleanup(__if);
				} else
					ret = 0;
				ifn->if_id = 0;
			}
		}
	}

	tcpip_net_unlock();

	return ret;
}

