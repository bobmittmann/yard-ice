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
 * @file raw_ioctl.c
 * @brief IP layer
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __USE_SYS_RAW__
#include <sys/raw.h>

#include <stdlib.h>
#include <sys/ioctl.h>

int raw_ioctl(struct raw_pcb * __raw, int __cmd, void * __data)
{
	int * flag;

	if ((__cmd == FIONBIO) && ((flag = (int *)__data) != NULL)) {
		if (*flag)
			__raw->r_flags |= RF_NONBLOCK;
		else
			__raw->r_flags &= ~RF_NONBLOCK;
		return 0;
	}

	return -1;
}

