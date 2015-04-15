/* 
 * Copyright(c) 2004-2012 BORESTE (www.boreste.com). All Rights Reserved.
 *
 * This file is part of the YARD-ICE.
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
 * @file netinet/in.h
 * @brief YARD-ICE libc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef	__NETINET_IN_H__
#define	__NETINET_IN_H__

#include <stddef.h>
#include <stdint.h>
#include <endian.h>

#ifndef __BYTE_ORDER 
#error "Undefined __BYTE_ORDER"
#endif

#define IPPROTO_ICMP 1
#define IPPROTO_IGMP 2
#define IPPROTO_TCP 6
#define IPPROTO_UDP 17



/* Standard well-known ports.  */
#define IPPORT_ECHO 7 /* Echo service.  */
#define IPPORT_DISCARD 9 /* Discard transmissions service.  */
#define IPPORT_SYSTAT 11 /* System status service.  */
#define IPPORT_DAYTIME 13 /* Time of day service.  */
#define IPPORT_NETSTAT 15 /* Network status service.  */
#define IPPORT_FTP 21 /* File Transfer Protocol.  */
#define IPPORT_TELNET 23 /* Telnet protocol.  */
#define IPPORT_SMTP 25 /* Simple Mail Transfer Protocol.  */
#define IPPORT_TIMESERVER 37 /* Timeserver service.  */
#define IPPORT_NAMESERVER 42 /* Domain Name Service.  */
#define IPPORT_WHOIS 43 /* Internet Whois service.  */
#define IPPORT_BOOTPS 67 /* Bootstrap Protocol, server side */
#define IPPORT_BOOTPC 68 /* Bootstrap Protocol, client side */
#define IPPORT_TFTP 69 /* Trivial File Transfer Protocol.  */
#define IPPORT_FINGER 79 /* Finger service.  */
#define IPPORT_SUPDUP 95 /* SUPDUP protocol.  */
#define IPPORT_SNMP 161 /* SNMP protocol.  */
#define IPPORT_EXECSERVER 512 /* execd service.  */
#define IPPORT_LOGINSERVER 513 /* rlogind service.  */

	/* Ports less than this value are reserved for privileged processes.  */
#define IPPORT_RESERVED 1024
	/* Ports greater this value are reserved for (non-privileged) servers.  */
#define IPPORT_USERRESERVED 5000


/* Type to represent a port.  */
typedef uint16_t in_port_t;

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
#define INADDR_LOOPBACK	((in_addr_t) 0x0100007f)	/* Inet 127.0.0.1.  */

#define	IN_CLASSA(a)		((((in_addr_t)(a)) & 0x00000080) == 0)
#define	IN_CLASSB(a)		((((in_addr_t)(a)) & 0x000000c0) == 0x00000080)
#define	IN_CLASSC(a)		((((in_addr_t)(a)) & 0x000000e0) == 0x000000c0)
#define	IN_CLASSD(a)		((((in_addr_t)(a)) & 0x000000f0) == 0x000000e0)

#define	IN_CLASSA_NET		0x000000ff
#define	IN_CLASSB_NET		0x0000ffff
#define	IN_CLASSC_NET		0x00ffffff

#define	IN_MULTICAST(a)		IN_CLASSD(a)

#elif __BYTE_ORDER == __BIG_ENDIAN

#error "Unsupported __BYTE_ORDER"

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

