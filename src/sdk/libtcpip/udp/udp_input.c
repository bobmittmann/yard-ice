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
 * @file udp_input.c
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __USE_SYS_UDP__
#include <sys/udp.h>

#include <string.h>
#include <stdlib.h>
#include <tcpip/icmp.h>

#include <trace.h>

#undef TRACE_LEVEL
#define TRACE_LEVEL TRACE_LVL_NONE

/*
  return value:
    -1 : error not processed.
     0 : ok processed, packet can be released.
     1 : ok processed, packet reused, don't release.
*/

int udp_input(struct ifnet * __if, struct iphdr * __ip, 
			   struct udphdr * __udp, int __len)
{
	struct udp_pcb * up;
	struct udp_dgram * dgram;
#if (ENABLE_NET_UDP_CHECKSUM)
	unsigned int sum;
#endif
	uint8_t * ptr;
	int tail;
	int cnt;
	int len;
	int n;

	len = ntohs(__udp->len);

#if 1
	if (len > __len) {
		DCC_LOG1(LOG_WARNING, "invalid len=%d!", len);
		WARN("UDP: invalid len=%d!", len);
		return -1;
	}
#endif

	up = (struct udp_pcb *)pcb_wildlookup(__ip->saddr, __udp->sport, 
										   __ip->daddr, __udp->dport, 
										   &__udp__.active);
	DBG("UDP: %I:%d > %I:%d (%d)", 
		__ip->saddr, ntohs(__udp->sport),
		__ip->daddr, ntohs(__udp->dport), len); 

	if (up == NULL) {
		DCC_LOG5(LOG_TRACE, "%I:%d > %I:%d (%d) port unreach", 
				 __ip->saddr, ntohs(__udp->sport),
				 __ip->daddr, ntohs(__udp->dport), len); 
		icmp_error(__ip, ICMP_DEST_UNREACH, ICMP_PORT_UNREACH, __if);
		UDP_PROTO_STAT_ADD(rx_drop, 1);
		return -1;
	}

	UDP_PROTO_STAT_ADD(rx_ok, 1);

	len -= sizeof(struct udphdr);
	ptr = (uint8_t *)__udp + sizeof(struct udphdr);

#if (ENABLE_NET_UDP_CHECKSUM)
	if ((sum = __udp->chksum) != 0) {
		sum += __udp->len << 1;
		sum += __udp->dport;
		sum += __udp->sport;
		sum += (IPPROTO_UDP << 8);
		sum += ((uint16_t *)(void *)&(__ip->saddr))[0];
		sum += ((uint16_t *)(void *)&(__ip->saddr))[1];
		sum += ((uint16_t *)(void *)&(__ip->daddr))[0];
		sum += ((uint16_t *)(void *)&(__ip->daddr))[1];

		if ((sum = in_chksum(sum, ptr, len)) != 0x0000ffff) {
			DCC_LOG6(LOG_WARNING, "%I:%d > %I:%d (%d) checksum error: 0x%08x", 
					 __ip->saddr, ntohs(__udp->sport),
					 __ip->daddr, ntohs(__udp->dport), len, sum); 
			UDP_PROTO_STAT_ADD(rx_drop, 1);
			UDP_PROTO_STAT_ADD(rx_err, 1);
			return -1;
		}

	}
#endif

	tail = up->u_rcv_tail;
	cnt = (tail - (int)up->u_rcv_head) & 0xff;

	if (cnt >= NET_UDP_RECV_QUEUE_LEN) {
		UDP_PROTO_STAT_ADD(rx_drop, 1);
		if ((unsigned int)cnt > (unsigned int)NET_UDP_RECV_QUEUE_LEN) {
			DCC_LOG4(LOG_PANIC, "<%05x> queue error: head=%d tail=%d cnt=%d", 
					 (int)up, (int)up->u_rcv_head, tail, cnt);
		}
		DCC_LOG1(LOG_WARNING, "<%05x> queue full", (int)up);
	//	thinkos_cond_signal(up->u_rcv_cond);
	} else {
		dgram = &up->u_rcv_buf[tail % NET_UDP_RECV_QUEUE_LEN];
		if ((dgram->q = mbuf_list_alloc(len)) != NULL) {

			n = mbuf_list_write(dgram->q, ptr, len);
			if (n != len) {
				DCC_LOG2(LOG_PANIC, "mbuf_list_write(%d)=%d", len, n);
			}

			dgram->len = n;
			dgram->port = __udp->sport;
			dgram->addr = __ip->saddr;
			up->u_rcv_tail = tail + 1;
			thinkos_cond_signal(up->u_rcv_cond);

			DCC_LOG5(LOG_TRACE, "%I:%d > %I:%d (%d) queued.", 
					 __ip->saddr, ntohs(__udp->sport), __ip->daddr, 
					 ntohs(__udp->dport), len); 

		} else {
			UDP_PROTO_STAT_ADD(rx_drop, 1);
			DCC_LOG1(LOG_WARNING, "<%05x> no more mbufs - discarded", (int)up);
		}
	}

	return 0;
} 

