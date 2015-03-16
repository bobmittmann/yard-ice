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
 * @file pcb_enum.c
 * @brief Internet Protocol Control Block
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifdef CONFIG_H
#include "config.h"
#endif

#ifdef PCB_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif
#include <sys/dcclog.h>

#include <sys/pcb.h>
#include <sys/mbuf.h>

#include <errno.h>
#include <stdlib.h>

int pcb_enum(int (* __callback)(struct pcb *, void *), 
			   void * __parm, struct pcb_list * __list)
{
	struct pcb_link * q;
	struct pcb * pcb;

	q = (struct pcb_link *)&__list->first;

	while ((q = q->next)) {
		pcb = &q->pcb;

		if (__callback != NULL) {
			if (__callback(pcb, __parm))
				break;
		}
	}

	return 0;
}

