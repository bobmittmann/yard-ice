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

#include <tcpip/ifnet.h>

#include <tcpip/ip.h>

#define LOOPIF_STACK_SIZE 256

#ifndef LOOPIF_BUF_SIZE 
#define LOOPIF_BUF_SIZE 512
#endif

struct loopif_drv {
	int8_t rx_sem;
	int8_t tx_sem;
	int16_t len;
	uint32_t buf[LOOPIF_BUF_SIZE / sizeof(uint32_t)];
	uint32_t stack[LOOPIF_STACK_SIZE / sizeof(uint32_t)];
};

int __attribute__((noreturn)) loopif_input_task(struct ifnet * ifn)
{
	struct loopif_drv * drv = (struct loopif_drv *)ifn->if_drv;
	int len;


	tcpip_net_lock();

	DCC_LOG(LOG_TRACE, "...");

	for (;;) {
		drv->len = 0;
		/* ok to send another package */
		__os_sem_post(drv->tx_sem);

		/* unlock the network layer */
		tcpip_net_unlock();

		__os_sem_wait(drv->rx_sem);

		tcpip_net_lock();

		len = drv->len;

		DCC_LOG1(LOG_TRACE, "packet received: %d bytes", len);

		ip_input(ifn, (struct iphdr *)drv->buf, len);
	}
}

int loopif_send(struct ifnet * __if, const uint8_t * dst, int proto, 
					const void * buf, int len)
{
	struct loopif_drv * drv = (struct loopif_drv *)__if->if_drv;

	if (len > __if->if_mtu)
		return -1;

	memcpy(drv->buf, buf, len);
	drv->len = len;

	DCC_LOG1(LOG_TRACE, "(%d)", len);

	__os_sem_post(drv->rx_sem);

	return len;
}

void * loopif_mmap(struct ifnet * __if, size_t __length)
{
	struct loopif_drv * drv = (struct loopif_drv *)__if->if_drv;

	__os_sem_wait(drv->tx_sem);

	return drv->buf;
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

	/* alloc a semaphore to control packet reception */
	drv->rx_sem = __os_sem_alloc(0);
	/* alloc a semaphore to control packet transmission */
	drv->tx_sem = __os_sem_alloc(0);

	DCC_LOG1(LOG_TRACE, "mtu=%d",  __if->if_mtu);

	__os_thread_create((void *)loopif_input_task, (void *)__if, 
					   drv->stack, LOOPIF_STACK_SIZE, __OS_PRIORITY_LOWEST);

	DCC_LOG(LOG_TRACE, "...");

	return 0;
}

void * loopif_arplookup(struct ifnet * __if, in_addr_t __ipaddr)
{
	return &__if->if_id;
}

	/* set the broadcast flag */
const struct ifnet_operations loopif_op = {
	.op_type = IFT_LOOP,
	.op_addrlen = 0,
	.op_init = loopif_startup,
	.op_cleanup = NULL,
	.op_mmap = loopif_mmap,
	.op_send = loopif_send,
	.op_arplookup = loopif_arplookup,
	.op_getaddr = NULL,
	.op_getdesc = loopif_getdesc,
	.op_sleep = NULL,
	.op_wakeup = NULL
};

struct loopif_drv loopif_drv;

struct ifnet * loopif_init(in_addr_t ip_addr, in_addr_t netmask)
{
	struct ifnet * ifn;
	
	ifn = ifn_register(&loopif_drv, &loopif_op, NULL, 0);

	if (ifn != NULL) {
		ifn_ipv4_set(ifn, ip_addr, netmask);
	}

	return ifn;
}

