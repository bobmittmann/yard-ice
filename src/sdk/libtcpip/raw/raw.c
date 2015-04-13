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
#include <sys/param.h>

#include <thinkos.h>
#include <sys/mbuf.h>

const uint8_t raw_pcb_max = NET_RAW_PCB_MAX;

struct raw_system __raw__;

struct raw_pcb * raw_pcb_new(int __protocol)
{
	struct raw_pcb * raw;

	tcpip_net_lock();

	if ((raw = (struct raw_pcb *)pcb_remove_head(&__raw__.free)) == NULL) {
		DCC_LOG(LOG_WARNING, "could not allocate a PCB");
		return NULL;
	}

	pcb_insert((struct pcb *)raw, &__raw__.active);

	/* ensure the mem is clean */
	memset(raw, 0, sizeof(struct raw_pcb));

	raw->r_cond = thinkos_cond_alloc();
	raw->r_protocol = __protocol;

	DCC_LOG2(LOG_TRACE, "<%x> protocol=%d", raw, __protocol); 


	tcpip_net_unlock();

	return raw;
} 

int raw_pcb_free(struct raw_pcb * __raw)
{
	DCC_LOG1(LOG_TRACE, "<%x>...", __raw); 

	thinkos_cond_free(__raw->r_cond);

	return pcb_move((struct pcb *)__raw, &__raw__.active, &__raw__.free);
} 

int raw_input(struct ifnet * __if, struct iphdr * __ip, int __len)
{
	struct pcb_link * q;
	struct raw_pcb * raw;
	int n;

	q = (struct pcb_link *)&__raw__.active.first;

	DCC_LOG4(LOG_TRACE, "%I > %I prot=%d (%d) ", 
			 __ip->saddr, __ip->daddr, __ip->proto, __len); 

	while ((q = q->next)) {
		raw = (struct raw_pcb *)&q->pcb;

		if (raw->r_protocol != __ip->proto)
			continue;

		raw->r_faddr = __ip->saddr;
		raw->r_laddr = __ip->daddr;

		n = MIN(NET_RAW_RCV_BUF_LEN, __len);	
		memcpy(raw->r_buf, __ip, n);
		raw->r_len = n;

		DCC_LOG2(LOG_TRACE, "<%0x> signal ---> %d", raw, raw->r_cond); 

		thinkos_cond_signal(raw->r_cond);

		return 0;
	}

	return -1;
} 

void raw_init(void)
{
	int i;

	DCC_LOG(LOG_TRACE, "initializing RAW subsystem."); 

	pcb_list_init(&__raw__.free);

	for (i = 0; i < NET_RAW_PCB_MAX; ++i) {
		struct raw_pcb * p = &__raw__.pcb_pool[i].pcb;
		pcb_insert((struct pcb *)p, &__raw__.free);
	}

	pcb_list_init(&__raw__.active);
}

