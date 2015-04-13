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
 * @file tcpip/dhcp.h
 * @brief 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

/*! \addtogroup tcpip
 * @{
 */
/*! \defgroup tcpip_dhcp tcpip/dhcp.h
 *	\brief DHCP client module header.
 *
 *	Usage: \n
 *	1) Create a struct dhcp_min filled with valid data; \n
 *	2) Start DHCP client; \n
 *	3) Keep ticking it until configuration callback is received; \n
 *	4) Start using the network after configuration. \n
 *
 *	Optionally, the programmer may choose to stop the DHCP client and stop 
 *	ticking it to free some resources. Note that this will result in a 
 *	misbehaviouring implementation (which may or may not cause harm, depending 
 *	on the case). \n
 *
 *	The program may save (by whatever means) the last IP acquired by DHCP so 
 *	it may be used across reboots.
 *
 *	\see RFC 2131 sections 1, 1.3, 1.5, 1.6, 2.2, 3.6 and 3.7 for an overview 
 *	of the protocol.
 * @{
 */
/*! \file
 *
 *	Revision tag: $Revision: 2.3 $ $Date: 2006/04/04 13:27:05 $
 *	@author Carlos A. V. e Vieira (carlos.vieira@boreste.com)
 */

#ifndef __SYS_DHCP_H__
#define __SYS_DHCP_H__

#ifndef __USE_SYS_DHCP__
#error "Never use <sys/dhcp.h> directly; include <tcpip/dhcp.h> instead."
#endif 

#ifdef CONFIG_H
#include "config.h"
#endif

#include <stdint.h>
#include <tcpip/ifnet.h>
#include <tcpip/dhcp.h>


/* minimum set of fields of any DHCP message */
/* According to RFC 2131: http://www.ietf.org/rfc/rfc2131.txt,
   sections 2 and 3. */
struct dhcp_msg {
	uint8_t op;			/* opcode, client always uses BOOTREQUEST */
	uint8_t htype;		/* hw address type, 1 = 10Mb ethernet */
	uint8_t hlen;		/* hw address lengh, 6 for 10Mb ethernet */
	uint8_t hops;		/* always zero, used by relay agents */
	uint32_t xid;		/* transaction id, random, used whithin negotiation */
	uint16_t secs;		/* seconds since negotiation started */
	uint16_t flags;		/* fags: broadcast-flag|all zeroes */
	uint32_t ciaddr;	/* client IP#, for BOUND, RENEW and REBINDING states */
	uint32_t yiaddr;	/* 'your' (client) IP# (set by server) */
	uint32_t siaddr;	/* server IP# (next server to bootstrap) */
	uint32_t giaddr;	/* relay agent IP#, when booting through it */
#define DHCP_CHADDR_LEN 16	/* client hw address */
	uint8_t chaddr[DHCP_CHADDR_LEN];
#define DHCP_SNAME_LEN 64	/* server name, optional, \0-terminated string */
	uint8_t sname[DHCP_SNAME_LEN];
#define DHCP_FILE_LEN 128	/* boot file, optional, null on DHCPDISCOVER */
	uint8_t file[DHCP_FILE_LEN];
	uint32_t cookie;	/* "Magic" cookie, the same as RFC 1497 and 1533 (both
						   obsoleted by 2132). */
//#define DHCP_OPTIONS_LEN 68	
#define DHCP_OPTIONS_LEN 144
	/* DHCP options, described in RFC 2132 */
	uint8_t options[DHCP_OPTIONS_LEN];
} __attribute__((__packed__));

#define DHCP_MSG_FIXED_LEN 	(240)	/* only options have variable lengh. */
#define DHCP_LOCAL_MSG_SIZE	(DHCP_MSG_FIXED_LEN + DHCP_OPTIONS_LEN)
/* From RFC2132: The code for this option is 57, and its length is 2. 
 * The minimum legal value is 576 octets. */
#if (DHCP_LOCAL_MSG_SIZE < 576)
#define DHCP_MAX_SUPPORTED_MSG_SIZE 576
#else
#define DHCP_MAX_SUPPORTED_MSG_SIZE (sizeof(struct dhcp_msg))
#endif

/* Common data among DHCP client module. */
struct dhcp_extra {
	/* current DHCP state machine state */
	uint8_t state;
	/* retries of current request */
	uint8_t retries;
	/* DHCP reply timeout. 
	 * uint8_t: 255ticks*4s = 1020s = 17min maximum. */
	uint8_t tmo_reply;
	/* Which was the timeout of the last received message? */
	uint8_t last_tmo;

	/* transaction identifier of last sent request */
	uint32_t xid;

	/* ticks elapsed since beginning of lease. */
	uint32_t ticks;

	/* Next server to bootstrap. */
	in_addr_t next_server;

	/* dhcp server address that offered this lease */
	in_addr_t server_ip;
	/* which ip address was offered? */
	in_addr_t offered_ip;
	/* under which subnet mask? */
	in_addr_t offered_sn;
	/* through which gateway? */
	in_addr_t offered_gw;
};

#define DHCP_EXTRA_SIZE 44

#ifndef DHCP_TIME_GRANULARITY_POWER
#define DHCP_TIME_GRANULARITY_POWER 2
#endif

#if	(DHCP_TIME_GRANULARITY_POWER < 2)
#undef DHCP_TIME_GRANULARITY_POWER
#error "DHCP_TIME_GRANULARITY_POWER cannot be < 2."
#endif

/*!	\brief Defines the clock tick time interval in seconds. */
#define DHCP_TIME_GRANULARITY 4

/*! DHCP client states */
enum dhcp_state {
	DHCP_IDLE       = 0,
	DHCP_INIT       = 1,
	DHCP_SELECTING	= 2,
	DHCP_REQUESTING	= 3,
	//DHCP_CHECKING	= 8,
	//DHCP_INFORMING	= 9,
	//DHCP_PERMANENT	= 10,
	DHCP_BOUND		= 4,
	DHCP_RENEWING	= 5,
	DHCP_REBINDING	= 6,
	/* not yet implemented
	DHCP_RELEASING	= 11, */
	DHCP_INIT_REBOOT = 7,
	DHCP_REBOOTING	= 8,
	//DHCP_BACKING_OFF	= 12,
	//DHCP_OFF		= 13
};

/*!	\brief Minimal data about the network infrastructure.
 * 
 *	Contains the basic information about the host. */
struct dhcp {
	/*!	Network interface descriptor (must be an initialized one).
	 *	\see tcpip/ifnet.h, tcpip/ethif.h */
	struct ifnet * ifn;	
	/*! DHCP configuration OK callback.
	 *	Issued when the configuration is ready to be applied.
	 *	Leave it NULL to use the default callback (which sets the IP stack up).
	 *
	 *	This is a regular message system callback, with the second parameter
	 *	being a pointer to the given struct dhcp_min filled with data 
	 *	gathered by the DHCP client. \n 
	 *	\see sys/msg.h for details. */
	void (* callback)(struct dhcp *);
	/*! Host IP address. */
	in_addr_t ip;
	/*! Local subnet mask. */
	in_addr_t mask;
	/*! Gateway (router) address. */
	in_addr_t gw;
	/*! Broadcast address. */
	in_addr_t bcast;

	/*! Network name of this interface. */
//	char * hostname;
	/*! Network domain name to which the interface is attached. */
//	char * domain;		//36 bytes, aligned
	
	/*! Hostname string lengh. */
//	uint8_t hn_len;
	/*! Domain name string lengh. */
//	uint8_t dn_len;		//4 bytes, not aligned
	
	/*!	Lease time in seconds. */
	uint32_t lease;
	/* current DHCP state machine state */
	uint8_t state;
	/* retries of current request */
	uint8_t retries;
	/* DHCP reply timeout. 
	 * uint8_t: 255ticks*4s = 1020s = 17min maximum. */
	uint8_t tmo_reply;
	/* Which was the timeout of the last received message? */
	uint8_t last_tmo;

	/* transaction identifier of last sent request */
	uint32_t xid;
	/* time elapsed since beginning of lease. */
	uint32_t time;

	/* Next server to bootstrap. */
	in_addr_t next_server;

	/* dhcp server address that offered this lease */
	in_addr_t server_ip;
	/* which ip address was offered? */
	in_addr_t offered_ip;
	/* under which subnet mask? */
//	in_addr_t offered_sn;
	/* through which gateway? */
//	in_addr_t offered_gw;
};

extern inline enum dhcp_state dhcp_get_state(struct dhcp * dhcp)
{
	return (enum dhcp_state) dhcp->state;
}


#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif /* __TCPIP_DHCP_H__ */

/*! @} */
/*! @} */
