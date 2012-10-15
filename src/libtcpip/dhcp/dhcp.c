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
 * @file dhcp.c
 * @brief DHCP client
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 


/* TODO LIST:
	- Make link layer independent.
	- Make it suitable for a multi-homed system.
*/

#ifdef __CONFIG__
#include "config.h"
#endif

#include <tcpip/dhcp.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include <netinet/in.h>

#include <tcpip/udp.h>
#include <tcpip/ifnet.h>
#include <tcpip/route.h>

#include <sys/mbuf.h>

#ifdef DHCP_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL DBG_INFO
#endif
#endif
#include <debug.h>

#if DHCP_EXTRA_SIZE > MBUF_SIZE
#error "Can't work with such low MBUF_SIZE"
#endif

#ifndef DHCP_DISCOVER_MAXIMUM_RETRIES
/*!	\brief How many times to try to locate a DHCP server.
 *	2: ~28 seconds
 *	3: ~60 seconds
 *	4: ~124 seconds
 *	5: ~252 seconds
 */
#define DHCP_DISCOVER_MAXIMUM_RETRIES 5
#elif DHCP_DISCOVER_MAXIMUM_RETRIES < 3
#error DHCP_DISCOVER_MAXIMUM_RETRIES must be at least 3. 
#endif

#define DHCP_REQUEST_MAXIMUM_RETRIES (DHCP_DISCOVER_MAXIMUM_RETRIES - 3)
#define DHCP_OTHER_MAXIMUM_RETRIES (DHCP_DISCOVER_MAXIMUM_RETRIES - 1)

/* Ethernet link layer. */
#define DHCP_HTYPE_ETH 1
#define DHCP_HLEN_ETH 6

/** BootP options */
#define DHCP_OPTION_PAD 0
#define DHCP_OPTION_SUBNET_MASK 1	/* RFC 2132 3.3 */
#define DHCP_OPTION_ROUTER 3
#define DHCP_OPTION_HOSTNAME 12
#define DHCP_OPTION_DOMAINNAME 15
#define DHCP_OPTION_IP_TTL 23
#define DHCP_OPTION_MTU 26
#define DHCP_OPTION_BROADCAST 28
#define DHCP_OPTION_TCP_TTL 37
#define DHCP_OPTION_TCP_KEEPALIVE 38
#define DHCP_OPTION_END 255

/** DHCP options */
#define DHCP_OPTION_REQUESTED_IP 50	/* RFC 2132 9.1, requested IP address */
#define DHCP_OPTION_LEASE_TIME 51	/* RFC 2132 9.2, time in seconds, in 4 
									   bytes */
#define DHCP_OPTION_OVERLOAD 52		/* RFC2132 9.3, use file and/or sname field 
									   for options */
#define DHCP_OPTION_MESSAGE_TYPE 53	/* RFC 2132 9.6, important for DHCP */
#define DHCP_OPTION_MESSAGE_TYPE_LEN 1

#define DHCP_OPTION_SERVER_ID 54	/* RFC 2132 9.7, server IP address */
#define DHCP_OPTION_PARAMETER_REQUEST_LIST 55	/* RFC 2132 9.8, requested 
												   option types */
#define DHCP_OPTION_MAX_MSG_SIZE 57	/* RFC 2132 9.10, message size 
									   accepted >= 576 */
#define DHCP_OPTION_MAX_MSG_SIZE_LEN 2

#define DHCP_OPTION_T1 58	/* T1 renewal time */
#define DHCP_OPTION_T2 59	/* T2 rebinding time */
#define DHCP_OPTION_CLIENT_ID 61
#define DHCP_OPTION_TFTP_SERVERNAME 66
#define DHCP_OPTION_BOOTFILE 67

/** possible combinations of overloading the file and sname fields with options */
#define DHCP_OVERLOAD_NONE 0
#define DHCP_OVERLOAD_FILE 1
#define DHCP_OVERLOAD_SNAME 2
#define DHCP_OVERLOAD_SNAME_FILE 3

enum bootp_message_op {
	BOOTP_BOOTREQUEST	= 1,		/* Client message. */
	BOOTP_BOOTREPLY		= 2		/* Server message. */
};

enum dhcp_message_type {
	DHCP_DISCOVER	= 1,
	DHCP_OFFER		= 2,
	DHCP_REQUEST	= 3,
	DHCP_DECLINE	= 4,
	DHCP_ACK		= 5,
	DHCP_NAK		= 6,
	DHCP_RELEASE	= 7,
	DHCP_INFORM		= 8
};

/* From RFC 2131 (modified and/or stripped):
   ---------------------------------------------------------------------
   |DHCP_REQUEST  |INIT-REBOOT  |SELECTING    |RENEWING     |REBINDING |
   ---------------------------------------------------------------------
UDP|broad/unicast |broadcast    |broadcast    |unicast      |broadcast |
msg|ciaddr        |zero         |zero         |IP address   |IP address|
opt|server-ip     |MUST NOT     |MUST         |MUST NOT     |MUST NOT  |
opt|requested-ip  |MUST         |MUST         |MUST NOT     |MUST NOT  |
   ---------------------------------------------------------------------
 
Field      DHCPDISCOVER          DHCPREQUEST           DHCPDECLINE,
           DHCPINFORM                                  DHCPRELEASE
-----      ------------          -----------           -----------
'xid'      selected by client    'xid' from server     selected by
                                 DHCPOFFER message     client
'secs'     0 or seconds since    0 or seconds since    0
           DHCP process started  DHCP process started
'flags'    Set 'BROADCAST'       Set 'BROADCAST'       0
           flag if client        flag if client
           requires broadcast    requires broadcast
           reply                 reply
'ciaddr'   0 (DHCPDISCOVER)      0 or client's         0 (DHCPDECLINE)
           client's              network address       client's network
           network address       (BOUND/RENEW/REBIND)  address
           (DHCPINFORM)                                (DHCPRELEASE)
[ysg]iaddr] 0                     0                     0
'sname'    options, if...        options, if...        (unused)
'file'     options, if...        options, if...        (unused)

Option                     DHCPDISCOVER  DHCPREQUEST      DHCPDECLINE,
                           DHCPINFORM                     DHCPRELEASE
------                     ------------  -----------      -----------
Requested IP address       MAY           MUST (in         MUST
                           (DISCOVER)    SELECTING or     (DHCPDECLINE),
                           MUST NOT      INIT-REBOOT)     MUST NOT
                           (INFORM)      MUST NOT (in     (DHCPRELEASE)
                                         BOUND or
                                         RENEWING)
IP address lease time      MAY           MAY              MUST NOT
                           (DISCOVER)
                           MUST NOT
                           (INFORM)
Use 'file'/'sname' fields  MAY           MAY              MAY
DHCP message type          MUST          MUST             MUST
Client identifier          MAY           MAY              MAY
Vendor class identifier    MAY           MAY              MUST NOT
Server identifier          MUST NOT      MUST (in         MUST
                                         REQUESTING)
                                         MUST NOT (in
                                         REBOOTING, RENEWING
                                         or REBINDING)
Parameter request list     MAY           MAY              MUST NOT
Maximum message size       MAY           MAY              MUST NOT
Message                    SHOULD NOT    SHOULD NOT       SHOULD
Site-specific              MAY           MAY              MUST NOT
All others                 MAY           MAY              MUST NOT

*/

struct dhcp_client {
	struct udp_pcb * pcb;
	/* double link list of clients */
	struct mlink * list;
};

struct dhcp_client __dhcpc__ = {
	NULL,
	NULL
};

/*
 * Insert a client in the list
 */
struct dhcp * dhcp_new(void)
{
	struct mlink * m;
	struct mlink * p;
	struct mlink * q;

	if ((m = mlink_new()) == NULL) {
		DBG(DBG_ERROR, "mlink_new()");
		return NULL;
	}

	if ((m->ptr = mbuf_new()) == NULL) {
		mlink_delete(m);
		return NULL;
	}

	if ((p = __dhcpc__.list) == NULL) {
		MLINK_DLINK(m, m);
	} else { 
		q = MLINK_NEXT(p);
		MLINK_DLINK(p, m);
		MLINK_DLINK(m, q);
	}

	__dhcpc__.list = m;

	DBG(DBG_NOTICE, "dhcp=0x%p", m);

	return m->ptr;
}

/*
 * Remove a client from the list
 */
int dhcp_remove(struct dhcp * __dhcp)
{
	struct mlink * p;
	struct mlink * q;
	struct mlink * m;

	DBG(DBG_NOTICE, "(%08x)", (int)__dhcp);

	if ((p = __dhcpc__.list) == NULL) {
		DBG(DBG_WARNING, "(%08x) dhcp list is empty (NULL)!", (int)__dhcp);
		return -1;
	}

	m = p;
	do {
		if (m->ptr == (void *)__dhcp) {
			q = MLINK_NEXT(m);
			if (m == q) {
				__dhcpc__.list = NULL;
			} else {
				p = MLINK_PREV(m);
				MLINK_DLINK(p, q);
				__dhcpc__.list = q;
			}
			MLINK_DLINK(m, m);
			/* release the mbuf */
			mbuf_free(m->ptr);
			/* release the mlink */
			mlink_delete(m);
			return 0;
		}
			
		m = MLINK_NEXT(m);
	} while (m != p); 

	DBG(DBG_WARNING, "(%08x) not found!", (int)__dhcp);
	return -1;
}

struct dhcp * dhcp_xid_lookup(int __xid)
{
	struct mlink * p;
	struct mlink * m;
	struct dhcp * dhcp;

	if ((p = __dhcpc__.list) == NULL)
		return NULL;

	m = p;
	do {
		dhcp = (struct dhcp *)m->ptr;
		if (dhcp->xid == __xid)
			return dhcp;
		m = MLINK_NEXT(m);
	} while (m != p); 

	return NULL;
}

struct dhcp * dhcp_ifn_lookup(struct ifnet * __ifn)
{
	struct mlink * p;
	struct mlink * m;
	struct dhcp * dhcp;

	if ((p = __dhcpc__.list) == NULL)
		return NULL;

	m = p;
	do {
		dhcp = (struct dhcp *)m->ptr;
		if (dhcp->ifn == __ifn)
			return dhcp;
		m = MLINK_NEXT(m);
	} while (m != p); 

	return NULL;
}

int dhcp_remove_all(void)
{
	/* this remove all links as well as all the mbufs */
	__dhcpc__.list = mlink_free(__dhcpc__.list);

	return 0;
}
	
static inline void dhcp_set_state(struct dhcp * dhcp, uint8_t new_state)
{
	if (new_state != dhcp->state) {
		dhcp->state = new_state;
		dhcp->retries = 0;
	}
}

static uint8_t *dhcp_find_option_ptr(uint8_t *field, uint8_t option_type,
									 int len, int *is_overloaded)
{
	int n;
	
	is_overloaded = DHCP_OVERLOAD_NONE;
	/* start with options field */
	/* at least 1 byte to read and end marker, 
	   then at least 3 bytes to read */
	for (n =0; (n < len) && (field[n] != DHCP_OPTION_END); ++n) {
		/* are the sname and/or file field overloaded with options? */
		if ((is_overloaded != NULL) && (field[n] == DHCP_OPTION_OVERLOAD)) {
			DBG(DBG_MSG, "overloaded message detected");
			/* skip option type and length */
			n += 2;
			*is_overloaded = field[n];
			continue;
		}

		/* requested option found? */
		if (field[n] == option_type) {
			DBG(DBG_MSG, "option found at offset %u in options", n);
			return &field[n];
		}
		/* skip option type */
		++n;
		/* skip option length, and then length bytes */
		n += field[n];
	}

	return NULL;
}

/*!	Finds the location of a specifig option, searching options and also file 
 * and sname fields, if overloaded.
 * \param msg
 *	Pointer to the begining of the DHCP message.
 * \param option_type 
 *	Option code you are looking for.
 * \param len
 *	Maximum lengh of options field.
 * \return
 *	Pointer to option code found or NULL on no option found.
 */
static uint8_t *dhcp_get_option_ptr(struct dhcp_msg *msg,
									uint8_t option_type, int len)
{
	int overload;
	uint8_t *opt;
	uint8_t *got;

	/* options available? */
	if (len <= 0)
		return NULL;

	DBG(DBG_MSG, "len=%d", len);

	overload = DHCP_OVERLOAD_NONE;
	opt = msg->options;

	if ((got = dhcp_find_option_ptr(opt, option_type, len, &overload)) != NULL)
		return got;
	
	switch (overload) {
	case DHCP_OVERLOAD_NONE:
		return NULL;
	case DHCP_OVERLOAD_FILE:
		DBG(DBG_INFO, "overloaded file field");
		opt = msg->file;
		len = DHCP_FILE_LEN;
		return dhcp_find_option_ptr(opt, option_type, len, NULL);
	case DHCP_OVERLOAD_SNAME:
		DBG(DBG_INFO, "overloaded sname field");
		opt = msg->sname;
		len = DHCP_SNAME_LEN;
		return dhcp_find_option_ptr(opt, option_type, len, NULL);
	default:
	case DHCP_OVERLOAD_SNAME_FILE:
		DBG(DBG_INFO, "overloaded sname and file field");
		opt = msg->sname;
		len = DHCP_SNAME_LEN;
		if ((got = dhcp_find_option_ptr(opt, option_type, len, NULL)) != NULL)
			return got;
		opt = msg->file;
		len = DHCP_FILE_LEN;
		return dhcp_find_option_ptr(opt, option_type, len, NULL);
	}
}

/**
 * Options constructors.
 */
static inline int dhcp_option(uint8_t *opt, uint8_t type, uint8_t len)
{
	opt[0] = type;
	opt[1] = len;

	return 2;
}

static inline int dhcp_option_byte(uint8_t * opt, uint8_t type, uint8_t val)
{
	opt[0] = type;
	opt[1] = 1;
	opt[2] = val;

	return 3;
}

static inline int dhcp_option_short(uint8_t * opt, uint8_t type, uint16_t val)
{
	opt[0] = type;
	opt[1] = 2;
	opt[2] = val;
	opt[3] = val >> 8;

	return 4;
}

static inline int dhcp_option_long(uint8_t * opt, uint8_t type, uint32_t val)
{
	opt[0] = type;
	opt[1] = 4;
	opt[2] = val;
	opt[3] = val >> 8;
	opt[4] = val >> 16;
	opt[5] = val >> 24;

	return 6;
}

static inline int add_parameter_list(uint8_t * opt)
{
	dhcp_option(opt, DHCP_OPTION_PARAMETER_REQUEST_LIST, 5);
	opt[2] = DHCP_OPTION_SUBNET_MASK;
	opt[3] = DHCP_OPTION_ROUTER;
	opt[4] = DHCP_OPTION_HOSTNAME;
	opt[5] = DHCP_OPTION_DOMAINNAME;
	opt[6] = DHCP_OPTION_BROADCAST;
	//opt[7] = DHCP_OPTION_TCP_KEEPALIVE;

	return 7;
}

static inline uint32_t get_option_long(uint8_t * opt)
{
	return opt[2] + (opt[3] << 8) + (opt[4] << 16) + (opt[5] << 24);
}

#ifdef DEBUG
static void dhcp_option_dump(uint8_t * opt, int len)
{
#if DEBUG_LEVEL > DBG_INFO
	register int i, j;
	for (i = 0; i < len; ++i) {
		printf("%02x ", opt[i]);
		if ((opt[i] == DHCP_OPTION_END) || (opt[i] == DHCP_OPTION_PAD))
			break;
		else {
			++i;
			for (j = 0; j < opt[i] + 1; ++j)
				printf("%02x ", opt[i + j]);
			putchar('\n');
			i += j - 1;
		}
	}
	putchar('\n');
#endif
}
#endif

/**
 * Add a DHCP message trailer
 *
 * Adds the END option to the DHCP message and, if
 * necessary, up to three padding bytes.
 */
static int dhcp_option_trailer(uint8_t * opt)
{
	int n;
	int i;

	opt[0] = DHCP_OPTION_END;
	n = 3 - ((int) opt & 0x00000003);
	for (i = 1; i <= n; i++) {
		opt[i] = DHCP_OPTION_PAD;
	}

	return n + 1;
}

static inline void tmo_reply_set(struct dhcp * dhcp)
{
	/* Retry profile: wait 4s, then 8s, 16s, 32s, etc... */
	dhcp->tmo_reply = (1 << (dhcp->retries + 2 - 
									DHCP_TIME_GRANULARITY_POWER));
	dhcp->last_tmo = dhcp->tmo_reply;
}

static void dhcp_create_request(const struct dhcp * dhcp, struct dhcp_msg * msg)
{
	uint16_t i;

	msg->op = BOOTP_BOOTREQUEST;
	/* TODO: make link layer independent */
	msg->htype = DHCP_HTYPE_ETH;
	/* TODO: make link layer independent */
	msg->hlen = DHCP_HLEN_ETH;
	msg->hops = 0;
	/* give unique transaction identifier to this request */
	msg->xid = dhcp->xid;
	msg->secs = 0;

	switch (dhcp->state) {
	case DHCP_BOUND:
	case DHCP_RENEWING:
	case DHCP_REBINDING:
		msg->ciaddr = dhcp->ifn->if_addr;
		msg->flags = 0;
		break;
	default:
		msg->ciaddr = 0;
		/* set broadcast flag on */
		msg->flags = (1 << 7);
	}
	msg->yiaddr = 0;
	msg->siaddr = 0;
	msg->giaddr = 0;

	/* copy netif hardware address, pad with zeroes */
	for (i = 0; i < DHCP_CHADDR_LEN; i++)
		msg->chaddr[i] = 0;

	dhcp->ifn->if_getaddr(dhcp->ifn->if_parm, msg->chaddr);


	for (i = 0; i < DHCP_SNAME_LEN; i++)
		msg->sname[i] = 0;

	for (i = 0; i < DHCP_FILE_LEN; i++)
		msg->file[i] = 0;

	msg->cookie = htonl(0x63825363UL);

#ifdef DEBUG
	/* fill options field with an incrementing array (for debugging purposes) */
	for (i = 0; i < DHCP_OPTIONS_LEN; i++)
		msg->options[i] = i;
#endif
}

/* Start the DHCP process, discover a DHCP server. */
static int dhcp_discover(struct dhcp * dhcp)
{
	struct dhcp_msg msg;
	uint8_t *opt;
	struct sockaddr_in sin;
	int n;
	struct udp_pcb * pcb;

	pcb = __dhcpc__.pcb;

	/* create and initialize the DHCP message header */
	// dhcp->xid++; // Won't change XID so often.
	dhcp_create_request(dhcp, &msg);
	opt = msg.options;

	n = dhcp_option_byte(opt, DHCP_OPTION_MESSAGE_TYPE, DHCP_DISCOVER);
	n += dhcp_option_short(&opt[n], DHCP_OPTION_MAX_MSG_SIZE, 
						   htons(DHCP_MAX_SUPPORTED_MSG_SIZE));
	n += add_parameter_list(&opt[n]);
	n += dhcp_option_trailer(&opt[n]);

#ifdef DEBUG
	dhcp_option_dump(msg.options, n);
	DBG(DBG_INFO, "dhcp_msg: %dB", (int) sizeof(struct dhcp_msg));
#endif

	n += sizeof(struct dhcp_msg) - DHCP_OPTIONS_LEN;

	sin.sin_family = AF_INET;
	sin.sin_port = HTONS(IPPORT_BOOTPS);
	sin.sin_addr.s_addr = INADDR_BROADCAST;

	if (udp_sendto(pcb, &msg, n, &sin) != n) {
		DBG(DBG_ERROR, "could not send UDP packet.");
		return -1;
	}

	dhcp_set_state(dhcp, DHCP_SELECTING);
	DBG(DBG_INFO, "sending DHCP discover: %d bytes.", n);

	return 0;
}

static int dhcp_request(struct dhcp * dhcp)
{
	struct dhcp_msg request;
	uint8_t * opt;
	struct sockaddr_in sin;
	int n;
	struct udp_pcb * pcb;

	pcb = __dhcpc__.pcb;
	
	dhcp_create_request(dhcp, &request);
	
	opt = request.options;
	n = dhcp_option_byte(opt, DHCP_OPTION_MESSAGE_TYPE, DHCP_REQUEST);
	n += dhcp_option_short(&opt[n], DHCP_OPTION_MAX_MSG_SIZE, 
						   htons(DHCP_MAX_SUPPORTED_MSG_SIZE));
	switch (dhcp->state) {
	case DHCP_REQUESTING:	// MUST: server ID
	case DHCP_SELECTING:	// MUST: requested IP
		n+= dhcp_option_long(&opt[n], DHCP_OPTION_SERVER_ID, 
							 dhcp->server_ip);
		n += dhcp_option_long(&opt[n], DHCP_OPTION_REQUESTED_IP, 
							  dhcp->offered_ip);
		break;
	case DHCP_INIT_REBOOT:	// MUST: requested IP
		dhcp_set_state(dhcp, DHCP_REBOOTING);
	case DHCP_REBOOTING:	// MUST NOT: server ID
		n += dhcp_option_long(&opt[n], DHCP_OPTION_REQUESTED_IP, 
							  dhcp->ip);
		break;
	case DHCP_RENEWING:		// MUST NOT: server ID and requested IP
	case DHCP_REBINDING:	// MUST NOT: server ID
	case DHCP_BOUND:		// MUST NOT: requested IP
		break;
	}
	n += add_parameter_list(&opt[n]);
	n += dhcp_option_trailer(&opt[n]);

#ifdef DEBUG
	dhcp_option_dump(request.options, n);
	DBG(DBG_INFO, "dhcp_msg: %dB", (int) sizeof(struct dhcp_msg));
#endif

	n += sizeof(struct dhcp_msg) - DHCP_OPTIONS_LEN;

	sin.sin_family = AF_INET;
	sin.sin_port = HTONS(IPPORT_BOOTPS);
	
	/* XXX: if the transition to REBINDING state is made *after* the REQUEST, 
	   it will be sent unicast on <T2 expires> event. */
	if ((dhcp->state == DHCP_BOUND) || 
		(dhcp->state == DHCP_RENEWING)) {
		sin.sin_addr.s_addr = dhcp->server_ip;
	} else {
		sin.sin_addr.s_addr = INADDR_BROADCAST;
	}

	if (udp_sendto(pcb, &request, n, &sin) != n) {
		DBG(DBG_ERROR, "could not send UDP packet.");
		return -1;
	}
	DBG(DBG_INFO, "sent to %s.",
		sin.sin_addr.s_addr == INADDR_BROADCAST ? "broadcast" : "server");
	return 0;

	/* TODO: we really should bind to a specific local interface here
	   but we cannot specify an unconfigured netif as it is addressless */
#if 0
		udp_bind(dhcp->pcb, IP_ADDR_ANY, DHCP_CLIENT_PORT);
		/* send broadcast to any DHCP server */
		udp_connect(dhcp->pcb, IP_ADDR_BROADCAST, DHCP_SERVER_PORT);
		udp_send(dhcp->pcb, dhcp->p_out);
		/* reconnect to any (or to server here?!) */
		udp_connect(dhcp->pcb, IP_ADDR_ANY, DHCP_SERVER_PORT);
		dhcp_delete_request(ifn);
		DBG(DBG_INFO, "REQUESTING\n");
		dhcp_set_state(dhcp, DHCP_REQUESTING);
#endif
}

static int dhcp_init(struct dhcp * dhcp)
{
	dhcp->xid++;
	switch (dhcp->state) {
	case DHCP_INIT:
	case DHCP_SELECTING:
		return post_message(dhcp_discover, (void *) dhcp);
	case DHCP_INIT_REBOOT:
		return post_message(dhcp_request, (void *) dhcp);
	default:
		DBG(DBG_ERROR, "invalid state!");
		return -1;
	}
}

static void dhcp_decline(struct dhcp * dhcp)
{
	/* FIXME */
	DBG(DBG_ERROR, "not implemented");
}

static void dhcp_handle_nak(struct dhcp * dhcp, struct dhcp_msg * msg, int len)
{
	uint8_t * opt;
	in_addr_t server;
	
	switch (dhcp->state) {
	case DHCP_REBOOTING:
	case DHCP_REQUESTING:
	case DHCP_RENEWING:
	case DHCP_REBINDING:
		break;
	default:
		DBG(DBG_NOTICE, "discarded.");
	}
	if ((opt = dhcp_get_option_ptr(msg, DHCP_OPTION_SERVER_ID, len)) == NULL) {
		DBG(DBG_NOTICE, "no server ID, discarding.");
		return;
	}
	if ((server = get_option_long(opt)) != dhcp->next_server) {
		DBG(DBG_NOTICE, "NAK from unknown server %d.%d.%d.%d, dicarding.", 
			IP4_ADDR1(server), IP4_ADDR2(server), IP4_ADDR3(server), 
			IP4_ADDR4(server));
		return;
	}
	DBG(DBG_INFO, "NAK accepted, restarting DHCP...");
	dhcp_set_state(dhcp, DHCP_INIT);
	if (dhcp_init(dhcp) < 0) {
		DBG(DBG_ERROR, "could not reset client");
	}
}

static void dhcp_handle_ack(struct dhcp * dhcp, struct dhcp_msg * msg, int len)
{
	uint8_t * opt;
	int opt_len;
	in_addr_t aux;
	uint8_t is_configured;
	
	switch (dhcp->state) {
	case DHCP_REBOOTING:
	case DHCP_REQUESTING:
	case DHCP_RENEWING:
	case DHCP_REBINDING:
		break;
	default:
		DBG(DBG_NOTICE, "discarded.");
		return;
	}

	/* // Will never happen!
	if ((msg = dhcp->msg_in) == NULL) {
		DBG(DBG_ERROR, "msg NULL!");
		return;
	} */

	is_configured = (dhcp->ifn->if_addr != INADDR_NONE);
	
	if (msg->yiaddr == 0) {
		DBG(DBG_WARNING, "ACK received without IP assigned.");
		if(post_message(dhcp_decline, (void *) dhcp) < 0) {
			DBG(DBG_WARNING, "post_message(dhcp_decline) failed");
		}
		
		if (is_configured)	// Discard invalid ack
			return;
		if (post_message(dhcp_init, (void *) dhcp) < 0) {
			DBG(DBG_ERROR, "post_message(dhcp_init) failed");
		}
		return;
	}
	DBG(DBG_NOTICE, "marking %d.%d.%d.%d to bind to interface id %d.", 
		IP4_ADDR1(msg->yiaddr), IP4_ADDR2(msg->yiaddr), 
		IP4_ADDR3(msg->yiaddr), IP4_ADDR4(msg->yiaddr), dhcp->ifn->if_id);
	dhcp->ip = msg->yiaddr;

	/* obtain pointer to DHCP message type */
	opt_len = len - ((sizeof(struct dhcp_msg) - DHCP_OPTIONS_LEN));

#ifdef DEBUG
	DBG(DBG_INFO, "dumping received options:");
	dhcp_option_dump(msg->options, opt_len);
	dhcp_option_dump(msg->file, DHCP_FILE_LEN);
	dhcp_option_dump(msg->sname, DHCP_SNAME_LEN);
#endif

	/* TODO: lease must be specified if previous message was not INFORM.
	 *	Currently guessing it if the interface has already such yiaddr. */
	if ((opt = dhcp_get_option_ptr(msg, DHCP_OPTION_LEASE_TIME, opt_len)) 
		== NULL) {
		DBG(DBG_WARNING, "no lease time found.");
		dhcp->lease = 0;	/* No lease means: don't reset tick count (assumes 
							   this is in response to a INFORM request. */
		if (msg->yiaddr != dhcp->ifn->if_addr) {
			if (post_message(dhcp_decline, (void *) dhcp) < 0) {
				DBG(DBG_WARNING, "post_message(dhcp_decline) failed.");
			}
			return;
		}
	} else {
		dhcp->lease = ntohl(get_option_long(opt));
		DBG(DBG_NOTICE, "got lease: %ds.", dhcp->lease);
		dhcp->time = 0;
	}
	/* setting state here to ignore further ACKs. */
	dhcp_set_state(dhcp, DHCP_BOUND);
	
	if ((opt = dhcp_get_option_ptr(msg, DHCP_OPTION_SUBNET_MASK, opt_len)) 
		== NULL) {
		DBG(DBG_WARNING, "no subnet mask option, guessing.");
		if (IN_CLASSA(msg->yiaddr))
			dhcp->mask = IN_CLASSA_NET;
		else {
			if (IN_CLASSB(msg->yiaddr))
				dhcp->mask = IN_CLASSB_NET;
			else {
				if (IN_CLASSC(msg->yiaddr))
					dhcp->mask = IN_CLASSC_NET;
				else {
					dhcp->mask = INADDR_NONE;
					DBG(DBG_WARNING, "could not guess.");
				}
			}
		}
	} else {
		aux = get_option_long(opt);
		dhcp->mask = aux;
		DBG(DBG_INFO, "got subnet mask %d.%d.%d.%d.", IP4_ADDR1(aux), 
			IP4_ADDR2(aux), IP4_ADDR3(aux), IP4_ADDR4(aux));
	}
	
	if ((opt = dhcp_get_option_ptr(msg, DHCP_OPTION_ROUTER, opt_len)) == NULL) {
		DBG(DBG_WARNING, "no router option.");
		dhcp->gw = INADDR_NONE;
	} else {
		aux = get_option_long(opt);
		dhcp->gw = aux;
		DBG(DBG_INFO, "got gateway %d.%d.%d.%d.", IP4_ADDR1(aux), 
			IP4_ADDR2(aux), IP4_ADDR3(aux), IP4_ADDR4(aux));
	}
	
	if ((opt = dhcp_get_option_ptr(msg, DHCP_OPTION_BROADCAST, opt_len)) 
		== NULL) {
		DBG(DBG_WARNING, "no broadcast option.");
		dhcp->bcast = INADDR_NONE;
	} else {
		aux = get_option_long(opt);
		dhcp->bcast = aux;
		DBG(DBG_INFO, "got broadcast address %d.%d.%d.%d.", IP4_ADDR1(aux), 
			IP4_ADDR2(aux), IP4_ADDR3(aux), IP4_ADDR4(aux));
	}

#if 0
	if ((opt = dhcp_get_option_ptr(msg, DHCP_OPTION_HOSTNAME, opt_len)) 
		== NULL) {
		DBG(DBG_WARNING, "no hostname option.");
		if (dhcp->hn_len >= 1)
			dhcp->hostname[0] = '\0';
	} else {
		opt += 2;
		for (i = 0; (i < dhcp->hn_len) && (i < opt[-1]); ++i)
			dhcp->hostname[i] = opt[i];
		if (i < dhcp->hn_len)
			dhcp->hostname[i] = '\0';
		else
			dhcp->hostname[dhcp->hn_len - 1] = '\0';
		DBG(DBG_INFO, "got hostname: %s", dhcp->hostname);
	}
	
	if ((opt = dhcp_get_option_ptr(msg, DHCP_OPTION_DOMAINNAME, opt_len)) 
		== NULL) {
		DBG(DBG_WARNING, "no domain name option.");
		if (dhcp->dn_len > 0)
			dhcp->domain[0] = '\0';
	} else {
		opt += 2;
		for (i = 0; (i < dhcp->dn_len) && (i < opt[-1]); ++i)
			dhcp->domain[i] = opt[i];
		if (i < dhcp->dn_len)
			dhcp->domain[i] = '\0';
		else
			dhcp->domain[dhcp->hn_len - 1] = '\0';
		DBG(DBG_INFO, "got domain name: %s", dhcp->domain);
	}
#endif

	if (dhcp->callback != NULL) {
		if (post_message(dhcp->callback, (void *) dhcp) < 0) {
			DBG(DBG_WARNING, "post_message(dhcp_ok) failed");
			/* FIXME try again later instead */
			dhcp->callback((void *) dhcp);
		}
		return;
	} else {
		if (post_message(dhcp_default_callback, (void *) dhcp) < 0) {
			DBG(DBG_WARNING, "post_message(dhcp_default_callback) failed");
			dhcp_default_callback(dhcp);
		}
		return;
	}
}

/* Simply select the first offer received. */
static void dhcp_select(struct dhcp * dhcp)
{
	DBG(DBG_INFO, "sending REQUEST.");
	/* New transaction. */
	dhcp->xid++;
	if (post_message(dhcp_request, (void *) dhcp) < 0) {
		DBG(DBG_ERROR, "could not send REQUEST.");
		return;
	}
	dhcp_set_state(dhcp, DHCP_REQUESTING);
	tmo_reply_set(dhcp);
}

static inline void dhcp_handle_offer(struct dhcp* dhcp, in_addr_t server_ip,
									 struct dhcp_msg * msg)
{
	if (dhcp->state != DHCP_SELECTING) {
		DBG(DBG_NOTICE, "discarded, not SELECTING.");
		return;
	}

	dhcp->server_ip = server_ip;
	dhcp->offered_ip = msg->yiaddr;

	DBG(DBG_INFO, "server (0x%x) %d.%d.%d.%d offers %d.%d.%d.%d", server_ip, 
		IP4_ADDR1(server_ip), IP4_ADDR2(server_ip), 
		IP4_ADDR3(server_ip), IP4_ADDR4(server_ip), 
		IP4_ADDR1(dhcp->offered_ip), IP4_ADDR2(dhcp->offered_ip), 
		IP4_ADDR3(dhcp->offered_ip), IP4_ADDR4(dhcp->offered_ip)); 
	dhcp_select(dhcp);

	return;
}

/* TODO: make it identify which interface is being configured and associate 
 * with the correct dhcp* structure. */ 
static void dhcp_input(struct udp_pcb *pb, int len, struct sockaddr_in *sin)
{
	struct dhcp_msg * msg;
	uint8_t *opt;
	struct dhcp * dhcp;
	in_addr_t server_ip;
	int n;
	int msg_type;
	int opt_len;
	char buf[len];

	DBG(DBG_NOTICE, "from %d.%d.%d.%d:%d, %d bytes",
		IP4_ADDR1(sin->sin_addr.s_addr), IP4_ADDR2(sin->sin_addr.s_addr), 
		IP4_ADDR3(sin->sin_addr.s_addr), IP4_ADDR4(sin->sin_addr.s_addr), 
		ntohs(sin->sin_port), len);

	/* // Will never happen!
	if (curr == NULL) {
		DBG(DBG_ERROR, "received UDP, but DHCP client should not be running.");
		udp_release((struct pcbr *) pb);
		return;
	} */

	if (len > DHCP_MAX_SUPPORTED_MSG_SIZE) {
		DBG(DBG_WARNING, "UDP packet too large: %d > %d!", len, (int) 
			DHCP_MAX_SUPPORTED_MSG_SIZE);
		return;
	}
	
	/* read the data and make the checksum calculation */
	if ((n = udp_recv(pb, buf, len)) < 0) {
		DBG(DBG_ERROR, "udp_rcv error");
		return;
	}
#ifndef DHCP_DONT_BYPASS_UDP_CHKSUM_BUG
	else if (n == 0) {
		DBG(DBG_WARNING, "udp_rcv checksum error or no data read.");
	}
#else
	else if (n != len) {
		DBG(DBG_ERROR, "udp_rcv read error! (n=%dB)", n);
		return;
	}
#endif

	msg = (struct dhcp_msg *) buf;

	if (msg->op != BOOTP_BOOTREPLY) {
		DBG(DBG_WARNING, "DHCP operation != BOOTP_BOOTREPLY");
		return;
	}

	/* iterate through hardware address and match against DHCP message */
	/*for (i = 0; i < netif->hwaddr_len; i++) {
    	if (netif->hwaddr[i] != msg->chaddr[i]) {
			DBG(DBG_WARNING, "hw address mismatch.");
			return;
		}
	} */

	dhcp = dhcp_xid_lookup(msg->xid);
	/* match transaction ID against what we expected */
	if (dhcp == NULL) {
		DBG(DBG_WARNING, "no matching transaction id %d", msg->xid);
		return;
	}

	/* obtain pointer to DHCP message type */
	opt_len = len - ((sizeof(struct dhcp_msg) - DHCP_OPTIONS_LEN));
	/* obtain the server address */
	if ((opt = dhcp_get_option_ptr(msg, DHCP_OPTION_SERVER_ID, opt_len)) 
		== NULL) {
		DBG(DBG_WARNING, "no options?");
		return;
	}
	server_ip = get_option_long(opt);

	/* Discarding bogus messages to avoid some attacks. */
	if (server_ip != sin->sin_addr.s_addr) {
		DBG(DBG_WARNING, "UDP from %d.%d.%d.%d with options claiming being"
			" from %d.%d.%d.%d.", 
			IP4_ADDR1(sin->sin_addr.s_addr), IP4_ADDR2(sin->sin_addr.s_addr), 
			IP4_ADDR3(sin->sin_addr.s_addr), IP4_ADDR4(sin->sin_addr.s_addr), 
			IP4_ADDR1(server_ip), IP4_ADDR2(server_ip), 
			IP4_ADDR3(server_ip), IP4_ADDR4(server_ip)); 
		return;
	}
	
	if ((opt = dhcp_get_option_ptr(msg, DHCP_OPTION_MESSAGE_TYPE, opt_len))
		== NULL) {
		DBG(DBG_WARNING, "no options?");
		return;
	}

	dhcp->next_server = msg->siaddr;
	/* read DHCP message type */
	msg_type = *(opt + 2);

	switch (msg_type) {
	case DHCP_ACK:
		DBG(DBG_NOTICE, "DHCP_ACK received");
		dhcp_handle_ack(dhcp, msg, len);
		dhcp->tmo_reply = 0;
		break;

	case DHCP_NAK:
		DBG(DBG_NOTICE, "DHCP_NAK received");
		dhcp_handle_nak(dhcp, msg, len);
		dhcp->tmo_reply = 0;
		break;

	case DHCP_OFFER:
		DBG(DBG_NOTICE, "DHCP_OFFER received");
		dhcp->tmo_reply = 0;
		dhcp_handle_offer(dhcp, server_ip, msg);
		break;

	default:
		DBG(DBG_WARNING, "unhandled msg_type=%d", msg_type);
	}
}

static void tmo_reply(struct dhcp * dhcp)
{
	switch (dhcp->state) {
	case DHCP_SELECTING:
		if ((dhcp->retries)++ == DHCP_DISCOVER_MAXIMUM_RETRIES) {
			DBG(DBG_NOTICE, "no DHCP server found after %d retries, aborting.", 
				DHCP_DISCOVER_MAXIMUM_RETRIES);
			dhcp->tmo_reply = 0;
			/* Issue callback. */
			if (dhcp->callback != NULL) {
				if (post_message(dhcp->callback, (void *) dhcp) < 0) {
					DBG(DBG_WARNING, "post_message(dhcp_halt) failed.");
					/* FIXME: try again later */
					dhcp->callback((void *) dhcp);
				}
			} else {
				/* FIXME: don't stop the server, but restart it, if
				   no callback suplied */
				if (post_message(dhcp_stop, (void *) dhcp) < 0) {
					DBG(DBG_WARNING, "post_message(dhcp_stop) failed.");
			//		dhcp_stop(dhcp);
				}
			}
			return;
		}
		tmo_reply_set(dhcp);
		/* Retry DISCOVER, go to INIT. */
		if (post_message(dhcp_init, (void *) dhcp) < 0) {
			DBG(DBG_WARNING, "post_message(dhcp_init) failed");
		}
		return;

	case DHCP_REQUESTING:
	case DHCP_REBOOTING:
		if ((dhcp->retries)++ == DHCP_REQUEST_MAXIMUM_RETRIES) {
			DBG(DBG_NOTICE, "DHCP ACK/NAK reply timeout.");
			dhcp_set_state(dhcp, DHCP_INIT);
			if (post_message(dhcp_init, (void *) dhcp) < 0) {
				DBG(DBG_WARNING, "post_message(dhcp_init) failed");
			}
			return;
		}
		break;	// Continues!

	case DHCP_RENEWING:
	case DHCP_REBINDING:
		(dhcp->retries)++;
		break;	// Continues!
	default:
		DBG(DBG_ERROR, "invalid state!");
		dhcp->tmo_reply = 0;
		return;
	}

	tmo_reply_set(dhcp);
	if (post_message(dhcp_request, (void *) dhcp) < 0) {
		DBG(DBG_WARNING, "post_message(dhcp_request) failed");
	}
}

static void tmo_t1(struct dhcp * dhcp)
{
	if (dhcp->state != DHCP_BOUND) {
		DBG(DBG_ERROR, "invalid state");
		return;
	}

	dhcp_set_state(dhcp, DHCP_RENEWING);
	
	/* MUST wait at least half the remaining time to T2 before retransmission.
	   (lease/2)/2 */
	dhcp->tmo_reply = dhcp->lease >> (2 + DHCP_TIME_GRANULARITY_POWER);
	dhcp->last_tmo = dhcp->tmo_reply;

	/* Changed state, changing XID. */
	dhcp->xid++;
	
	if (post_message(dhcp_request, (void *) dhcp) < 0) {
		DBG(DBG_WARNING, "post_message(dhcp_request) failed");
		dhcp_request(dhcp);
	}
}

static void tmo_t2(struct dhcp * dhcp)
{
	if (dhcp->state != DHCP_RENEWING) {
		DBG(DBG_ERROR, "invalid state");
		return;
	}

	dhcp_set_state(dhcp, DHCP_REBINDING);
	
	/* MUST wait at least half the remaining time to lease expiration before 
	   retransmission. (lease/8)/2 */
	dhcp->tmo_reply = dhcp->lease >> (4 + DHCP_TIME_GRANULARITY_POWER);
	dhcp->last_tmo = dhcp->tmo_reply;

	/* Changed state, changing XID. */
	dhcp->xid++;
	
	if (post_message(dhcp_request, (void *) dhcp) < 0) {
		DBG(DBG_WARNING, "post_message(dhcp_request) failed");
		dhcp_request(dhcp);
	}
}

void dhcp_default_callback(struct dhcp * dhcp)
{
#ifdef DEBUG
	/* sanity check */
	if (dhcp == NULL) {
		DBG(DBG_ERROR, "dhcp cannot be NULL");
		return;
	}
#endif

	if (dhcp_get_state(dhcp) != DHCP_BOUND) {
		DBG(DBG_ERROR, "dhcp error!");
		return;
	}
	
	if (dhcp->ip == dhcp->ifn->if_addr) {	/* Reconfiguring... */
		if (dhcp->mask == INADDR_NONE)
			dhcp->mask = dhcp->ifn->if_mask;
		if (dhcp->gw == INADDR_NONE) {
			struct route * route;
			route = route_lookup(INADDR_ANY);
			if (route->rt_dev == dhcp->ifn) {
				dhcp->gw = route->rt_gateway;
				DBG(DBG_NOTICE, "No GW, found previous GW: %d.%d.%d.%d.", 
					IP4_ADDR1(dhcp->gw), IP4_ADDR2(dhcp->gw), 
					IP4_ADDR3(dhcp->gw), IP4_ADDR4(dhcp->gw));
			}
		}
	}

	/* configure the ip address */
	if (ifn_ipconfig(dhcp->ifn, dhcp->ip, dhcp->mask) >= 0) {
		/* add the default route (gateway) to ifn */
		route_del(INADDR_ANY);
		route_add(INADDR_ANY, INADDR_ANY, dhcp->gw, dhcp->ifn);
		return;
	}
	
	if (dhcp->bcast != INADDR_NONE) {
		route_del(IN_ADDR(255, 255, 255, 255));
		route_add(IN_ADDR(255, 255, 255, 255), IN_ADDR(255, 255, 255, 255), 
				  dhcp->bcast, dhcp->ifn);
	}
	
#ifdef DEBUG
	printf("%s: DHCP configuration done.", __FUNCTION__);
#endif
}

void dhcp_tmr(void)
{
	uint32_t aux;
	struct dhcp * dhcp;
	struct mlink * p;
	struct mlink * m;

	if ((p = __dhcpc__.list) == NULL)
		return;

	m = p;
	do {
		dhcp = (struct dhcp *)m->ptr;

#ifdef DEBUG
		/* XXX */
		if ((dhcp == NULL) || (dhcp == NULL)) {
			DBG(DBG_ERROR, "invalid dhcp structure.");
			return;
		}
#endif

#if (defined(DEBUG) && (DEBUG_LEVEL <= DBG_INFO))
		putchar('"');
#endif

		dhcp->time += DHCP_TIME_GRANULARITY;

		/* Reply timer active? */
		if (dhcp->tmo_reply != 0) {
			if (--(dhcp->tmo_reply) == 0)
				tmo_reply(dhcp);
		}
		
		/* Are we under a lease? */
		if (dhcp->lease != 0) {
			/* Lease out? */
			if (dhcp->time >= dhcp->lease) {
				DBG(DBG_WARNING, "lease time off");
				if (dhcp->callback != NULL) {
					if (post_message(dhcp->callback, (void *) dhcp) < 0) {
						DBG(DBG_WARNING, "post_message(dhcp_halt) failed");
						dhcp->callback((void *) dhcp);
					}
				}
			} else {
				/* T2 (rebinding) out? */
				/* XXX: T2 is fixed as (7/8)*lease */
				aux = (dhcp->lease >> 1) + 
					(dhcp->lease >> 2) + (dhcp->lease >> 3);
				if (dhcp->time >= aux) {
					tmo_t2(dhcp);
				} else {
					/* T1 (renewing) out? */
					/* XXX: T1 is fixed as lease/2. */
					aux = dhcp->lease >> 1;
					if (dhcp->time >= aux) {
						tmo_t1(dhcp);
					}
				}
			}
		}
		m = MLINK_NEXT(m);
	} while (m != p); 
}

struct udp_pcb * dhcp_start(void)
{
	/* Sender pcb. */ 
	struct udp_pcb * pcb;

	if ((pcb = __dhcpc__.pcb) == NULL) {
		if ((pcb = (struct udp_pcb *) udp_pcb_new()) == NULL) {
			DBG(DBG_ERROR, "couldn't allocate UDP pcb.");
			return NULL;
		}
		__dhcpc__.pcb = pcb;
	}

	/* Set up sender PCB */
	pcb->u_faddr = INADDR_ANY;
	pcb->u_laddr = INADDR_ANY;
	pcb->u_lport = HTONS(IPPORT_BOOTPC);
	pcb->u_callback = dhcp_input;
	pcb->u_handle = NULL;
	pcb->u_mtu = sizeof(struct dhcp_msg) + 1;

	/* Start UDP listening on BOOTP client port. */
	if (udp_register((struct pcb *) pcb) < 0) {
		/* FIXME: multihomed ... */
		DBG(DBG_ERROR, "Can't register UDP listener");
		udp_release((struct pcb *)pcb);
		return NULL;
	}

	return pcb;
}

struct dhcp * dhcp_ifconfig(struct ifnet * __ifn, 
							void (* __callback)(struct dhcp *))
{
	/* XXX */
	struct dhcp * dhcp = NULL; 
	struct route * rt;

	if (__ifn == NULL) {
		DBG(DBG_ERROR, "invalid interface!");
		return NULL;
	}

	if ((dhcp = dhcp_ifn_lookup(__ifn)) == NULL) {
		/* allocate memory for dhcp */
		if ((dhcp = dhcp_new()) == NULL) {
			DBG(DBG_ERROR, "out of dynamic memory!");
			return NULL;
		}
	 
		dhcp->ifn = __ifn;	
		dhcp->callback = __callback;
		dhcp->ip = __ifn->if_addr;
		dhcp->mask = __ifn->if_mask;

		if ((rt = route_lookup(INADDR_ANY)) == NULL)
			dhcp->gw = INADDR_ANY;
		else
			dhcp->gw = rt->rt_gateway;

		if ((rt = route_lookup(__ifn->if_addr & __ifn->if_mask)) == NULL)
			dhcp->bcast = INADDR_ANY;
		else
			dhcp->bcast = rt->rt_gateway;

	//	dhcp->hostname = NULL;
	//	dhcp->domain = NULL;
	//	dhcp->hn_len = 0;
	//	dhcp->dn_len = 0;
		dhcp->retries = 0;
		/* FIXME: transaction identifier, must be unique for each DHCP request. */
		dhcp->xid = 0xedcb0000;
	} else {
		DBG(DBG_NOTICE, "DHCP client renew #%d", dhcp->ifn->if_id);
	}

	/* start the DHCP negotiation */
	if (dhcp->ip == INADDR_ANY) {	// Previous IP unknown
		dhcp->state = DHCP_INIT;
	} else {								// Previous IP known
		dhcp->state = DHCP_INIT_REBOOT;
	}

	tmo_reply_set(dhcp);

	dhcp_init(dhcp);

	return dhcp;
}

void dhcp_stop(void)
{
	struct udp_pcb * pcb;

	if ((pcb = __dhcpc__.pcb) == NULL) {
		return;
	}

	dhcp_remove_all();
	udp_release((struct pcb *)pcb);
}

/* EOF */
