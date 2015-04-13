/* 
 * Copyright(c) 2004-2012 BORESTE (www.boreste.com). All Rights Reserved.
 *
 * This file is part of the libtcpip.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */

/** 
 * @file ifn_register.c
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __USE_SYS_IFNET__
#include <sys/ifnet.h>

#include <tcpip/ifnet.h>

#include <stdlib.h>

struct ifnet * ifn_register(void * __drv, const struct ifnet_operations * __op,
							void * __io, int __irq_no)
{
	struct ifnet * ifn = NULL;
	int type;
	int idx;
	int i;
	int n;

	if (__op == NULL) {
		DCC_LOG(LOG_WARNING, "null operations");
		return NULL;
	}

	tcpip_net_lock();

	for (i = 0; i < ifnet_max; ++i) {
		if (__ifnet__.ifn[i].if_id == 0) {
			ifn = &__ifnet__.ifn[i];
			idx = i;
			break;
		}
	}

	if (ifn == NULL) {
		DCC_LOG(LOG_WARNING, "the ifnet pool is full");
		tcpip_net_unlock();
		return ifn;
	}

	type = __op->op_type;
	n = 1;	
	for (i = 0; i < ifnet_max; ++i) {
		if (__ifnet__.ifn[i].if_id == ((type << 2) + n))
			n++;
	}

	/* alloc the interface  */
	ifn->if_idx = idx;
	ifn->if_id = (type << 2) + n;
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

