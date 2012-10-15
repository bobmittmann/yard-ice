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
 * @file tcp_enum.c
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __USE_SYS_TCP__
#include <sys/tcp.h>

int tcp_enum(int (* __callback)(struct tcp_pcb *, void *), void * __parm)
{
	int n = 0;

	n = pcb_enum((int (*)(struct pcb *, void *))__callback,
				   __parm, &__tcp__.closed);

	n += pcb_enum((int (*)(struct pcb *, void *))__callback,
				   __parm, &__tcp__.listen);

	n += pcb_enum((int (*)(struct pcb *, void *))__callback,
					__parm, &__tcp__.active);

	return n;
}

