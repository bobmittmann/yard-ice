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
 * @file loopif.c
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifdef CONFIG_H
#include "config.h"
#endif

#ifdef LOOPIF_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif
#include <sys/dcclog.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <tcpip/ifnet.h>

#define __USE_SYS_NET__
#include <sys/net.h>
#define __USE_SYS_IFNET__
#include <sys/ifnet.h>
#define __USE_SYS_ROUTE__
#include <sys/route.h>
#include <tcpip/ifnet.h>
#include <sys/ip.h>

#define LOOPIF_STACK_SIZE 256

#ifndef LOOPIF_BUF_SIZE 
#define LOOPIF_BUF_SIZE 512
#endif

struct loopif_pkt {
	uint16_t proto;
	uint16_t len;
	uint8_t buf[LOOPIF_BUF_SIZE];
};

struct loopif_drv {
	uint8_t tx_sem;
	struct loopif_pkt pkt;
};

void * loopif_mmap(struct ifnet * __if, size_t __len)
{
	struct loopif_drv * drv = (struct loopif_drv *)__if->if_drv;
	struct loopif_pkt * pkt;

	DCC_LOG1(LOG_INFO, "(%d)", __len);

	if (__len > __if->if_mtu)
		return NULL;

	pkt = (struct loopif_pkt *)&drv->pkt;

	tcpip_net_unlock();
	thinkos_sem_wait(drv->tx_sem);
	tcpip_net_lock();

	return (void *)((uintptr_t)pkt + 4);
}

int loopif_send(struct ifnet * __if, const uint8_t * __dst, int __proto, 
					const void * __buf, int __len)
{
	struct loopif_pkt * pkt;

	pkt = (struct loopif_pkt *)((uintptr_t)__buf - 4);

	memcpy(pkt->buf, __buf, __len);
	pkt->len = __len;
	pkt->proto = __proto;

	DCC_LOG2(LOG_INFO, "proto=%d len=%d", __proto, __len);

	ifn_signal(__if->if_idx);

	return __len;
}

int loopif_pkt_recv(struct ifnet * __if, uint8_t ** __src, 
						  unsigned int * __proto, uint8_t ** __pkt)
{
	struct loopif_drv * drv = (struct loopif_drv *)__if->if_drv;
	struct loopif_pkt * pkt;
	
	pkt = (struct loopif_pkt *)&drv->pkt;
	if (pkt->len == 0)
		return 0;

	*__pkt = (void *)((uintptr_t)pkt + 4);
	*__proto = pkt->proto;

	DCC_LOG1(LOG_INFO, "(%d)", pkt->len);

	return pkt->len;
}

int loopif_ethif_munmap(struct ifnet * __if, void * __mem)
{
//	uint8_t * pktbuf = (uint8_t *)((uintptr_t)__mem - 14);
//	DCC_LOG1(LOG_INFO, "pktbuf=%p --", pktbuf);
//	pktbuf_free(pktbuf);
	struct loopif_drv * drv = (struct loopif_drv *)__if->if_drv;
	struct loopif_pkt * pkt;

	pkt = (struct loopif_pkt *)((uintptr_t)__mem - 4);
	pkt->len = 0;

	/* ok to send another package */
	thinkos_sem_post(drv->tx_sem);
	return 0;
}

int loopif_pkt_free(struct ifnet * __if, uint8_t * __pkt)
{
	struct loopif_drv * drv = (struct loopif_drv *)__if->if_drv;
	struct loopif_pkt * pkt;

	pkt = (struct loopif_pkt *)((uintptr_t)__pkt - 4);
	pkt->len = 0;

	/* ok to send another package */
	thinkos_sem_post(drv->tx_sem);
	return 0;
}

int loopif_getdesc(struct ifnet * __if, char * __s, int __len)
{
	return 0;
}

int loopif_startup(struct ifnet * __if)
{
	struct loopif_drv * drv = (struct loopif_drv *)__if->if_drv;

	__if->if_mtu = LOOPIF_BUF_SIZE;
	__if->if_flags |= IFF_LOOPBACK;

	/* alloc a semaphore to control packet transmission */
	drv->tx_sem = thinkos_sem_alloc(1);
	drv->pkt.len = 0;

	DCC_LOG1(LOG_INFO, "mtu=%d",  __if->if_mtu);

	return 0;
}

void * loopif_arplookup(struct ifnet * __if, in_addr_t __ipaddr)
{
	return &__if->if_id;
}

int loopif_getaddr(struct ifnet * __if, uint8_t * __buf)
{
	__buf[0] = 0;
	return 0;
}

const struct ifnet_operations loopif_op = {
	.op_type = IFT_LOOP,
	.op_addrlen = 0,
	.op_init = loopif_startup,
	.op_cleanup = NULL,
	.op_mmap = loopif_mmap,
	.op_send = loopif_send,
	.op_arplookup = loopif_arplookup,
	.op_getaddr = loopif_getaddr,
	.op_getdesc = loopif_getdesc,
	.op_sleep = NULL,
	.op_wakeup = NULL,
	.op_pkt_recv = loopif_pkt_recv,
	.op_pkt_free = loopif_pkt_free,
	.op_munmap = loopif_ethif_munmap
};

struct loopif_drv loopif_drv;

struct ifnet * loopif_init(void)
{
	struct ifnet * ifn;
	
	ifn = ifn_register(&loopif_drv, &loopif_op, NULL, 0);

	if (ifn != NULL) {
		ifn_ipv4_set(ifn, IPV4_ADDR(127, 0, 0, 1), IPV4_ADDR(255, 0, 0, 0));
	}

	return ifn;
}

