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
 * @file raw_getstat.c
 * @brief IP layer
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __USE_SYS_RAW__
#include <sys/raw.h>
#include <tcpip/stat.h>

#include <string.h>

void raw_getstat(struct proto_stat * __st, int __rst)
{
#if ENABLE_RAW_STAT
	proto_stat_copy(__st, &raw_stat, __rst);
#else
	memset(__st, 0, sizeof(struct proto_stat));
#endif
}

