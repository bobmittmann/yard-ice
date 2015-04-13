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
 * @file ifn_getinfo.c
 * @brief Get Network Interface Name
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __USE_SYS_IFNET__
#include <sys/ifnet.h>
#include <tcpip/ifnet.h>

#include <stdio.h>

int ifn_getinfo(struct ifnet * __if, struct ifnet_info * __info)
{
	if ((__if == NULL) || (__info == NULL))
		return -1;

	__info->type = __if->if_op->op_type;
	__info->flags = __if->if_flags;
	__info->mtu = __if->if_mtu;
	__info->lnk_speed = __if->if_link_speed;

	ifn_getname(__if, __info->name);
	ifn_getaddr(__if, __info->hw_addr);
	if (ifn_getdesc(__if, __info->desc, IFNET_INFO_DESC_MAX) == 0)
		__info->desc[0] = '\0';

	return 0;
}


