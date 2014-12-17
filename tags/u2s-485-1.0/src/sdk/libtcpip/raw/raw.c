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
 * @file raw.c
 * @brief IP layer
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 


#define __USE_SYS_RAW__
#include <sys/raw.h>

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/os.h>
#include <sys/mbuf.h>

#if (ENABLE_NET_RAW)

struct raw_system __raw__;

struct raw_pcb * raw_pcb_new(int __protocol)
{
	struct raw_pcb * raw;

	tcpip_net_lock();

	/* get a new memory descriptor */
	if ((raw = (struct raw_pcb *)pcb_alloc()) == NULL) {
		DCC_LOG(LOG_WARNING, "could not allocate a PCB");
		tcpip_net_unlock();
		return NULL;
	}

	/* ensure the mem is clean */
	memset(raw, 0, sizeof(struct raw_pcb));

	raw->r_cond = __os_cond_alloc();
	raw->r_protocol = __protocol;

	DCC_LOG2(LOG_TRACE, "<%x> protocol=%d", raw, __protocol); 

	pcb_insert((struct pcb *)raw, &__raw__.list);

	tcpip_net_unlock();

	return raw;
} 

int raw_pcb_free(struct raw_pcb * __raw)
{
	DCC_LOG1(LOG_TRACE, "<%x>...", __raw); 

	__os_cond_free(__raw->r_cond);

	pcb_release((struct pcb *)__raw, &__raw__.list);

	return 0;
} 

int raw_input(struct ifnet * __if, struct iphdr * __ip, int __len)
{
	struct pcb_link * q;
	struct raw_pcb * raw;

	q = (struct pcb_link *)&__raw__.list.first;

	DCC_LOG3(LOG_TRACE, "%I > %I (%d)", __ip->saddr, __ip->daddr, __len); 

	while ((q = q->next)) {
		raw = (struct raw_pcb *)&q->pcb;

		DCC_LOG2(LOG_TRACE, "<%0x> protocol=%d", raw, __ip->proto); 

		if (raw->r_protocol != __ip->proto)
			continue;

		DCC_LOG1(LOG_TRACE, "protocol %d", __ip->proto); 
		DCC_LOG3(LOG_TRACE, "%I > %I (%d)", __ip->saddr, __ip->daddr, __len); 

		raw->r_buf = __ip;
		raw->r_len = __len;
		raw->r_faddr = __ip->saddr;
		raw->r_laddr = __ip->daddr;

		__os_cond_signal(raw->r_cond);

		return 1;
	}

	return 0;
} 

void raw_init(void)
{
	DCC_LOG(LOG_TRACE, "initializing RAW subsystem."); 

	pcb_list_init(&__raw__.list);
}

#endif /* !ENABLE_NET_RAW */

