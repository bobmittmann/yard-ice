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
 * @file ifn_enum.c
 * @brief 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __USE_SYS_IFNET__
#include <sys/ifnet.h>
#include <errno.h>

int ifn_enum(int (* __callback)(struct ifnet *, void *), void * __parm)
{
	struct ifnet * ifn;
	int i;
	int n;

	if (__callback == NULL) {
		DCC_LOG(LOG_WARNING, "null pointer");
		return -EINVAL;
	}

//	tcpip_net_lock();

	n = 0;
	for (i = 0; i < ifnet_max; ++i) {
		ifn = &__ifnet__.ifn[i];
		if (ifn->if_id != 0) {
			DCC_LOG1(LOG_INFO, "%s ...", ifn_name_lut[ifn->if_id]);
			if (__callback(ifn, __parm))
				break;
			n++;
		}
	}

//	tcpip_net_unlock();

	return n;
}

