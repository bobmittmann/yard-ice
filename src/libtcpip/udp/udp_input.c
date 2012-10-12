/* $Id: udp.c,v 2.16 2008/06/04 00:03:15 bob Exp $ 
 *
 * File:	udp_input.c
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:	
 * Comment:
 * Copyright(c) 2004-2008 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#define __USE_SYS_UDP__
#include <sys/udp.h>

#include <string.h>
#include <stdlib.h>
#include <tcpip/icmp.h>

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
#if 0
	if (len > __len) {
		DCC_LOG1(LOG_WARNING, "invalid len=%d!", len);
		return 1;
	}
#endif
		
	up = (struct udp_pcb *)pcb_wildlookup(__ip->saddr, __udp->sport, 
										   __ip->daddr, __udp->dport, 
										   &__udp__.list);

	if (up == NULL) {
		DCC_LOG5(LOG_TRACE, "%I:%d > %I:%d (%d) port unreach", 
				 __ip->saddr, ntohs(__udp->sport),
				 __ip->daddr, ntohs(__udp->dport), len); 
		icmp_error(__ip, ICMP_DEST_UNREACH, ICMP_PORT_UNREACH, __if);
		UDP_PROTO_STAT_ADD(rx_drop, 1);
		return 1;
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
			return 1;
		}

	}
#endif

	tail = up->u_rcv_tail;
	cnt = (tail - (int)up->u_rcv_head) & 0xff;

	if (cnt >= UDP_RECV_MAX) {
		UDP_PROTO_STAT_ADD(rx_drop, 1);
		if ((unsigned int)cnt > (unsigned int)UDP_RECV_MAX) {
			DCC_LOG4(LOG_PANIC, "<%05x> queue error: head=%d tail=%d cnt=%d", 
					 (int)up, (int)up->u_rcv_head, tail, cnt);
		}
		DCC_LOG1(LOG_WARNING, "<%05x> queue full", (int)up);
	//	__os_cond_signal(up->u_rcv_cond);
	} else {
		dgram = &up->u_rcv_buf[tail % UDP_RECV_MAX];
		if ((dgram->q = mbuf_list_alloc(len)) != NULL) {

			n = mbuf_list_write(dgram->q, ptr, len);
			if (n != len) {
				DCC_LOG2(LOG_PANIC, "mbuf_list_write(%d)=%d", len, n);
			}

			dgram->len = n;
			dgram->port = __udp->sport;
			dgram->addr = __ip->saddr;
			up->u_rcv_tail = tail + 1;
			__os_cond_signal(up->u_rcv_cond);

			DCC_LOG5(LOG_INFO, "%I:%d > %I:%d (%d)", 
					 __ip->saddr, ntohs(__udp->sport), __ip->daddr, 
					 ntohs(__udp->dport), len); 
#if (LOG_LEVEL < LOG_INFO)
			DCC_LOG(LOG_TRACE, "queued.");
#endif

		} else {
			UDP_PROTO_STAT_ADD(rx_drop, 1);
			DCC_LOG1(LOG_WARNING, "<%05x> no more mbufs - discarded", (int)up);
		}
	}

	return 1;
} 

