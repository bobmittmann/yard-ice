/* $Id: ip_icmp.h,v 2.1 2008/06/04 00:03:14 bob Exp $ 
 *
 * File:	netinet/udp.h
 * Module:	
 * Project:	AT91X40DK	
 * Author:	Robinson Mittmann (bob@cnxtech.com)
 * Target:	arm7tdmi
 * Comment:	generic ipv4 routing scheme	
 * Copyright(c) 2003,2004 CNX Technologies. All Rights Reserved.
 *
 */

#ifndef __NETINET_IP_ICMP_H__
#define __NETINET_IP_ICMP_H__

#include <stdint.h>
#include <netinet/ip.h>

struct icmphdr {
	uint8_t  type;
	uint8_t  code;
	uint16_t chksum;
	union {
		struct {
			uint16_t id;
			uint16_t sequence;
    	} echo;			/* echo datagram */
		struct {
			uint16_t unused;
			uint16_t nextmtu;
		} frag;			/* path mtu discovery */
		uint32_t gateway;	/* gateway address */
	} un;
};

struct icmp {
	uint8_t  icmp_type;
	uint8_t  icmp_code;
	uint16_t icmp_chksum;
	union {
		uint8_t icmp_pptr;
		uint32_t icmp_gwaddr;	/* gateway address */
		struct {
			uint16_t icmp_id;
			uint16_t icmp_seq;
    	};			/* echo datagram */
		uint32_t icmp_void;	/* gateway address */
		struct {
			uint16_t icmp_pmvoid;
			uint16_t icmp_nextmtu;
		};			/* path mtu discovery */
	};
	union {
		struct {
			int32_t icmp_otime;
			int32_t icmp_rtime;
			int32_t icmp_ttime;
		};
		struct iphdr icmp_ip;
		uint32_t icmp_mask;
		uint8_t icmp_data[1];
	};
};


#define ICMP_MINLEN 8
#define ICMP_TSLEN 20
#define ICMP_MASKLEN 12
#define ICMP_ADVLENMIN 36


#define ICMP_HEADER_LEN 4

#define ICMP_ECHO_HEADER_LEN 8

#define ICMP_ECHOREPLY		0	/* Echo Reply			*/
#define ICMP_DEST_UNREACH	3	/* Destination Unreachable	*/
#define ICMP_SOURCE_QUENCH	4	/* Source Quench		*/
#define ICMP_REDIRECT		5	/* Redirect (change route)	*/
#define ICMP_ECHO		8	/* Echo Request			*/
#define ICMP_TIME_EXCEEDED	11	/* Time Exceeded		*/
#define ICMP_PARAMETERPROB	12	/* Parameter Problem		*/
#define ICMP_TIMESTAMP		13	/* Timestamp Request		*/
#define ICMP_TIMESTAMPREPLY	14	/* Timestamp Reply		*/
#define ICMP_INFO_REQUEST	15	/* Information Request		*/
#define ICMP_INFO_REPLY		16	/* Information Reply		*/
#define ICMP_ADDRESS		17	/* Address Mask Request		*/
#define ICMP_ADDRESSREPLY	18	/* Address Mask Reply		*/
#define NR_ICMP_TYPES		18


/* Codes for UNREACH. */
#define ICMP_NET_UNREACH	0	/* Network Unreachable		*/
#define ICMP_HOST_UNREACH	1	/* Host Unreachable		*/
#define ICMP_PROT_UNREACH	2	/* Protocol Unreachable		*/
#define ICMP_PORT_UNREACH	3	/* Port Unreachable		*/
#define ICMP_FRAG_NEEDED	4	/* Fragmentation Needed/DF set	*/
#define ICMP_SR_FAILED		5	/* Source Route failed		*/
#define ICMP_NET_UNKNOWN	6
#define ICMP_HOST_UNKNOWN	7
#define ICMP_HOST_ISOLATED	8
#define ICMP_NET_ANO		9
#define ICMP_HOST_ANO		10
#define ICMP_NET_UNR_TOS	11
#define ICMP_HOST_UNR_TOS	12
#define ICMP_PKT_FILTERED	13	/* Packet filtered */
#define ICMP_PREC_VIOLATION	14	/* Precedence violation */
#define ICMP_PREC_CUTOFF	15	/* Precedence cut off */
#define NR_ICMP_UNREACH		15	/* instead of hardcoding immediate value */

/* Codes for REDIRECT. */
#define ICMP_REDIR_NET		0	/* Redirect Net			*/
#define ICMP_REDIR_HOST		1	/* Redirect Host		*/
#define ICMP_REDIR_NETTOS	2	/* Redirect Net for TOS		*/
#define ICMP_REDIR_HOSTTOS	3	/* Redirect Host for TOS	*/

/* Codes for TIME_EXCEEDED. */
#define ICMP_EXC_TTL		0	/* TTL count exceeded		*/
#define ICMP_EXC_FRAGTIME	1	/* Fragment Reass time exceeded	*/

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __NETINET_IP_ICMP_H__ */

