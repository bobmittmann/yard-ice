/* 
 * File:	tcpip/in.h
 * Module:	
 * Project:
 * Author:	Robinson Mittmann (bob@boreste.com)
 * Target:
 * Comment:
 * Copyright(c) 2003-2005 CNX Technologies. All Rights Reserved.
 *
 */

#ifndef	__TCPIP_IN_H__
#define	__TCPIP_IN_H__

#include <stdint.h>
#include <netinet/in.h> /* in_addr_t */

#if __BYTE_ORDER == __LITTLE_ENDIAN

#define IP4_ADDR1(a)    (((in_addr_t) (a)) & 0xff)
#define IP4_ADDR2(a)    ((((in_addr_t) (a)) >> 8) & 0xff)
#define IP4_ADDR3(a)    ((((in_addr_t) (a)) >> 16) & 0xff)
#define IP4_ADDR4(a)    ((((in_addr_t) (a)) >> 24) & 0xff)

#define IP4_ADDR(a, b, c, d) ((((a) & 0xff) << 0) | \
		(((b) & 0xff) << 8) | (((c) & 0xff) << 16) | (((d) & 0xff) << 24))

#define IN_ADDR(a, b, c, d) (in_addr_t)((((a) & 0xff) << 0) | \
		(((b) & 0xff) << 8) | (((c) & 0xff) << 16) | (((d) & 0xff) << 24))


#define NTOHL(x) \
	((uint32_t)((((uint32_t)(x) & 0x000000ffU) << 24) | \
	(((uint32_t)(x) & 0x0000ff00U) <<  8) | \
	(((uint32_t)(x) & 0x00ff0000U) >>  8) | \
	(((uint32_t)(x) & 0xff000000U) >> 24)))

#define HTONL NTOHL

#define NTOHS(x) \
	((uint16_t)((((uint16_t)(x) & 0x00ff) << 8) | \
	(((uint16_t)(x) & 0xff00) >> 8)))

#define HTONS NTOHS

#elif __BYTE_ORDER == __BIG_ENDIAN

#else
#error "Undefined __BYTE_ORDER"
#endif

#ifdef __cplusplus
extern "C" {
#endif

uint16_t in_chksum(uint32_t, const void * __data, int __len);

#ifdef __cplusplus
}
#endif

#endif	/* __TCPIP_IN_H__ */

