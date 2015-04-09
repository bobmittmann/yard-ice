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

int ifn_enum(int (* __callback)(struct ifnet *, void *), void * __parm)
{
	int i;
	int n;

	n = 0;
	for (i = 0; i < ifnet_max; i++) {
		if (__ifnet__.ifn[i].if_id != 0) {
			if (__callback(&__ifnet__.ifn[i], __parm))
				break;
		}
	}

	return n;
}

