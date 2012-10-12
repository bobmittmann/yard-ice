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

#ifndef IFNET_INTERFACES_MAX
#define IFNET_INTERFACES_MAX 1
#endif

const uint8_t ifnet_max = IFNET_INTERFACES_MAX;

struct ifnet __ifnet__[IFNET_INTERFACES_MAX];

struct ifnet * ifn_register(void * __drv, const struct ifnet_operations * __op,
							void * __io, int __irq_no)
{
	struct ifnet * ifn = NULL;
	int i;
	int type;
	int n;

	if (__op == NULL) {
		DCC_LOG(LOG_WARNING, "null operations");
		return NULL;
	}

	tcpip_net_lock();

	for (i = 0; i < ifnet_max; i++) {
		if (__ifnet__[i].if_id == 0) {
			ifn = &__ifnet__[i];
			break;
		}
	}

	if (ifn == NULL) {
		DCC_LOG(LOG_WARNING, "the ifnet pool is full");
		tcpip_net_unlock();
		return ifn;
	}

	type = __op->op_type;
	n = 0;	
	for (i = 0; i < ifnet_max; i++) {
		if ((__ifnet__[i].if_id & IFT_MASK) == type) {
			n++;
		}
	}

	/* alloc the interface  */
	ifn->if_id = type + n;
	ifn->if_ipv4_addr = INADDR_ANY;
	ifn->if_ipv4_mask = INADDR_ANY;
	ifn->if_flags = 0;
	ifn->if_link_speed = 0;
	ifn->if_mtu = 0;
	ifn->if_drv = __drv;
	ifn->if_op = __op;
	ifn->if_io = __io;
	ifn->if_irq_no = __irq_no;

	if (ifn->if_op->op_init != NULL) {
		if (ifn->if_op->op_init(ifn) < 0) {
			DCC_LOG(LOG_WARNING, "ifnet->init() fail!");
		} else {
			ifn->if_flags |= IFF_UP;
		}
	}

	tcpip_net_unlock();

	return ifn;
}

