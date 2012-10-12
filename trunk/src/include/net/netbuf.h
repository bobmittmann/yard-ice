/* $Id: netbuf.h,v 2.0 2006/03/28 15:10:05 bob Exp $ 
 *
 * File:	netbuf.h
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@cnxtech.com)
 * Target:
 * Comment:
 * Copyright(c) 2004 CNX Technologies. All Rights Reserved.
 *
 */

#ifndef _NETBUF_H_
#define _NETBUF_H_

#include <sys/types.h>
#include <sys/memory.h>

#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

struct sock_buf {
	void * sock;
	struct net_if * dev;
    /* Network layer header */	
	union {
        struct ip_hdr    *iph;
        struct arp_hdr   *arph;
        unsigned char   *raw;
    } nh;
    /* Transport layer header */ 
    union {
        struct tcp_hdr   *tcph;
        struct udp_hdr   *udph;
        struct icmp_hdr  *icmph;
/*        struct igmp_hdr  *igmph; */
        struct ip_hdr    *iph;
        unsigned char   *raw;
    } th;
    /* Memory handlers */ 
	struct mem_info mem[6];
}

/*
 * fragment buffer
 */
 

#define FRAGMENT_MEM
#define FRAGMENT_FBUF

typedef struct net_buf netbuf_t;

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif /* __TTY_H__ */

