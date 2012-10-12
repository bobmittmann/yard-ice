/* $Id: in.h,v 2.0 2006/03/28 15:10:05 bob Exp $ 
 *
 * File:	netinet/in.h
 * Module:	
 * Project:
 * Author:	Robinson Mittmann (bob@boreste.com)
 * Target:
 * Comment:
 * Copyright(c) 2003-2005 CNX Technologies. All Rights Reserved.
 *
 */

/*! \addtogroup comm
 * @{
 */
/*! \defgroup netinet_in netinet/in.h
 * @{
 */
/*! \file
 *	Revision tag: $Revision: 2.0 $ $Date: 2006/03/28 15:10:05 $
 *	@author Robinson Mittman(bob@boreste.com)
 */

#ifndef	__NETINET_IN_H__
#define	__NETINET_IN_H__

#include <stdint.h>

/* Standard well-defined IP protocols.  */
enum {
	IPPROTO_IP = 0,		/* Dummy protocol for TCP.  */
#define IPPROTO_IP		IPPROTO_IP
	IPPROTO_HOPOPTS = 0,	/* IPv6 Hop-by-Hop options.  */
#define IPPROTO_HOPOPTS		IPPROTO_HOPOPTS
	IPPROTO_ICMP = 1,	/* Internet Control Message Protocol.  */
#define IPPROTO_ICMP		IPPROTO_ICMP
	IPPROTO_IGMP = 2,	/* Internet Group Management Protocol. */
#define IPPROTO_IGMP		IPPROTO_IGMP
	IPPROTO_IPIP = 4,	/* IPIP tunnels (older KA9Q tunnels use 94).  */
#define IPPROTO_IPIP		IPPROTO_IPIP
	IPPROTO_TCP = 6,	/* Transmission Control Protocol.  */
#define IPPROTO_TCP		IPPROTO_TCP
	IPPROTO_EGP = 8,	/* Exterior Gateway Protocol.  */
#define IPPROTO_EGP		IPPROTO_EGP
	IPPROTO_PUP = 12,	/* PUP protocol.  */
#define IPPROTO_PUP		IPPROTO_PUP
	IPPROTO_UDP = 17,	/* User Datagram Protocol.  */
#define IPPROTO_UDP		IPPROTO_UDP
	IPPROTO_IDP = 22,	/* XNS IDP protocol.  */
#define IPPROTO_IDP		IPPROTO_IDP
	IPPROTO_TP = 29,	/* SO Transport Protocol Class 4.  */
#define IPPROTO_TP		IPPROTO_TP
	IPPROTO_IPV6 = 41,	/* IPv6 header.  */
#define IPPROTO_IPV6		IPPROTO_IPV6
	IPPROTO_ROUTING = 43,	/* IPv6 routing header.  */
#define IPPROTO_ROUTING		IPPROTO_ROUTING
	IPPROTO_FRAGMENT = 44,	/* IPv6 fragmentation header.  */
#define IPPROTO_FRAGMENT	IPPROTO_FRAGMENT
	IPPROTO_RSVP = 46,	/* Reservation Protocol.  */
#define IPPROTO_RSVP		IPPROTO_RSVP
	IPPROTO_GRE = 47,	/* General Routing Encapsulation.  */
#define IPPROTO_GRE		IPPROTO_GRE
	IPPROTO_ESP = 50,	/* encapsulating security payload.  */
#define IPPROTO_ESP		IPPROTO_ESP
	IPPROTO_AH = 51,	/* authentication header.  */
#define IPPROTO_AH		IPPROTO_AH
	IPPROTO_ICMPV6 = 58,	/* ICMPv6.  */
#define IPPROTO_ICMPV6		IPPROTO_ICMPV6
	IPPROTO_NONE = 59,	/* IPv6 no next header.  */
#define IPPROTO_NONE		IPPROTO_NONE
	IPPROTO_DSTOPTS = 60,	/* IPv6 destination options.  */
#define IPPROTO_DSTOPTS		IPPROTO_DSTOPTS
	IPPROTO_MTP = 92,	/* Multicast Transport Protocol.  */
#define IPPROTO_MTP		IPPROTO_MTP
	IPPROTO_ENCAP = 98,	/* Encapsulation Header.  */
#define IPPROTO_ENCAP		IPPROTO_ENCAP
	IPPROTO_PIM = 103,	/* Protocol Independent Multicast.  */
#define IPPROTO_PIM		IPPROTO_PIM
	IPPROTO_COMP = 108,	/* Compression Header Protocol.  */
#define IPPROTO_COMP		IPPROTO_COMP
	IPPROTO_RAW = 255,	/* Raw IP packets.  */
#define IPPROTO_RAW		IPPROTO_RAW
	IPPROTO_MAX
};

/* Type to represent a port.  */
typedef uint16_t in_port_t;

/* Standard well-known ports.  */
enum {
	IPPORT_ECHO = 7,	/* Echo service.  */
	IPPORT_DISCARD = 9,	/* Discard transmissions service.  */
	IPPORT_SYSTAT = 11,	/* System status service.  */
	IPPORT_DAYTIME = 13,	/* Time of day service.  */
	IPPORT_NETSTAT = 15,	/* Network status service.  */
	IPPORT_FTP = 21,	/* File Transfer Protocol.  */
	IPPORT_TELNET = 23,	/* Telnet protocol.  */
	IPPORT_SMTP = 25,	/* Simple Mail Transfer Protocol.  */
	IPPORT_TIMESERVER = 37,	/* Timeserver service.  */
	IPPORT_NAMESERVER = 42,	/* Domain Name Service.  */
	IPPORT_WHOIS = 43,	/* Internet Whois service.  */
	IPPORT_MTP = 57,
	IPPORT_BOOTPS = 67,	/* Bootstrap Protocol, server side */
	IPPORT_BOOTPC = 68,	/* Bootstrap Protocol, client side */

	IPPORT_TFTP = 69,	/* Trivial File Transfer Protocol.  */
	IPPORT_RJE = 77,
	IPPORT_FINGER = 79,	/* Finger service.  */
	IPPORT_TTYLINK = 87,
	IPPORT_SUPDUP = 95,	/* SUPDUP protocol.  */

	IPPORT_SNMP = 161,	/* SNMP protocol.  */

	IPPORT_EXECSERVER = 512,	/* execd service.  */
	IPPORT_LOGINSERVER = 513,	/* rlogind service.  */
	IPPORT_CMDSERVER = 514,
	IPPORT_EFSSERVER = 520,

	/* UDP ports.  */
	IPPORT_BIFFUDP = 512,
	IPPORT_WHOSERVER = 513,
	IPPORT_ROUTESERVER = 520,

	/* Ports less than this value are reserved for privileged processes.  */
	IPPORT_RESERVED = 1024,

	/* Ports greater this value are reserved for (non-privileged) servers.  */
	IPPORT_USERRESERVED = 5000
};

/*! Internet address.  */
typedef uint32_t in_addr_t;

/*! \brief Internet address, alternate structure. */
struct in_addr {
	/*! Internet address. */
	in_addr_t s_addr;
};


/* Protocol families.  */
#define	PF_INET		2	/* IP protocol family.  */
/* Address families.  */
#define	AF_INET		PF_INET

/*! \brief Socket infomation. */
struct sockaddr_in {
	/*! Address family (usually AF_INET) */
	uint16_t sin_family;
	/*!	Port number (use HTONS(<portnumber>) to write to it if portnumber is 
	 *	constant). */
	uint16_t sin_port;
	/*! IP address */
	struct in_addr sin_addr;
};

#if __BYTE_ORDER == __LITTLE_ENDIAN

/* Address to accept any incoming messages.  */
#define	INADDR_ANY		((in_addr_t) 0x00000000)
/* Address to send to all hosts.  */
#define	INADDR_BROADCAST	((in_addr_t) 0xffffffff)
/* Address indicating an error return.  */
#define	INADDR_NONE		((in_addr_t) 0xffffffff)

/* Network number for local host loopback.  */
#define	IN_LOOPBACKNET		127

/* Address to loopback in software to local host.  */
#define INADDR_LOOPBACK	((in_addr_t) 0x7f000001)	/* Inet 127.0.0.1.  */

#define	IN_CLASSA(a)		((((in_addr_t)(a)) & 0x00000080) == 0)
#define	IN_CLASSB(a)		((((in_addr_t)(a)) & 0x000000c0) == 0x00000080)
#define	IN_CLASSC(a)		((((in_addr_t)(a)) & 0x000000e0) == 0x000000c0)
#define	IN_CLASSD(a)		((((in_addr_t)(a)) & 0x000000f0) == 0x000000e0)

#define	IN_CLASSA_NET		0x000000ff
#define	IN_CLASSB_NET		0x0000ffff
#define	IN_CLASSC_NET		0x00ffffff

#define	IN_MULTICAST(a)		IN_CLASSD(a)

#elif __BYTE_ORDER == __BIG_ENDIAN

#else
#error "Undefined __BYTE_ORDER"
#endif


#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

static inline uint32_t ntohl(uint32_t __netlong) {
#if __BYTE_ORDER == __BIG_ENDIAN
	return __netlong;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
	return ((__netlong & 0xff) << 24) | ((__netlong & 0xff00) << 8) |
		((__netlong & 0xff0000) >> 8) | ((__netlong & 0xff000000) >> 24);
#endif
}

static inline uint16_t ntohs(uint16_t __netshort) {
#if __BYTE_ORDER == __BIG_ENDIAN
	return __netshort;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
	return ((__netshort & 0xff) << 8) | ((__netshort & 0xff00) >> 8);
#endif
}

static inline uint32_t htonl(uint32_t __hostlong) {
#if __BYTE_ORDER == __BIG_ENDIAN
	return __hostlong;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
	return ((__hostlong & 0xff) << 24) | ((__hostlong & 0xff00) << 8) |
		((__hostlong & 0xff0000) >> 8) | ((__hostlong & 0xff000000) >> 24);
#endif
}

static inline uint16_t htons(uint16_t __hostshort) {
#if __BYTE_ORDER == __BIG_ENDIAN
	return __hostshort;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
	return ((__hostshort & 0xff) << 8) | ((__hostshort & 0xff00) >> 8);
#endif
}

#endif /* __NETINET_IN_H__ */

