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

#define __USE_SYS_DHCP__
#include <sys/dhcp.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include <netinet/in.h>

#include <tcpip/udp.h>
#include <tcpip/ifnet.h>
#include <tcpip/route.h>
#include <tcpip/in.h>

#include <sys/dcclog.h>

#include <thinkos.h>

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

#ifndef DHCP_CLIENT_MAX 
#define DHCP_CLIENT_MAX 1
#endif

struct dhcp_client {
	struct udp_pcb * pcb;
	uint32_t xid_seed;
	/* list of clients */
	struct dhcp dhcp[DHCP_CLIENT_MAX];
};

struct dhcp_client __dhcpc__ = {
	.pcb = NULL,
	.xid_seed = 0x12345678,
	.dhcp = { [0 ... DHCP_CLIENT_MAX - 1] = 
		{
			.ifn = NULL,
		}
	}
};

/*
 * Insert a client in the list
 */
struct dhcp * dhcp_alloc(struct ifnet * __ifn)
{
	int i;

	for (i = 0; i < DHCP_CLIENT_MAX; ++i) {
		if (__dhcpc__.dhcp[i].ifn == NULL) {
			__dhcpc__.dhcp[i].ifn = __ifn;
			return &__dhcpc__.dhcp[i];
		}

	}

	return NULL;
}

static uint32_t dhcp_xid(void)
{
	__dhcpc__.xid_seed = (__dhcpc__.xid_seed * 1103515245) + 12345;
	return __dhcpc__.xid_seed & 0x3fffffff;
}

/*
 * Remove a client from the list
 */
int dhcp_free(struct dhcp * __dhcp)
{
	int i;

	for (i = 0; i < DHCP_CLIENT_MAX; ++i) {
		if (&__dhcpc__.dhcp[i] == __dhcp) {
			__dhcpc__.dhcp[i].ifn = NULL;
			__dhcpc__.dhcp[i].xid = 0;
			return 0;
		}

	}

	DCC_LOG1(LOG_WARNING, "(%08x) not found!", (int)__dhcp);

	return -1;
}

struct dhcp * dhcp_xid_lookup(int __xid)
{
	int i;

	for (i = 0; i < DHCP_CLIENT_MAX; ++i) {
		if (__dhcpc__.dhcp[i].xid == __xid) {
			return &__dhcpc__.dhcp[i];
		}

	}

	return NULL;
}

struct dhcp * dhcp_ifn_lookup(struct ifnet * __ifn)
{
	int i;

	for (i = 0; i < DHCP_CLIENT_MAX; ++i) {
		if (__dhcpc__.dhcp[i].ifn == __ifn) {
			return &__dhcpc__.dhcp[i];
		}

	}

	return NULL;
}

static inline void dhcp_set_state(struct dhcp * dhcp, int new_state)
{
	if (new_state != dhcp->state) {
		dhcp->state = new_state;
		dhcp->retries = 0;
	}
}

static uint8_t *dhcp_find_option_ptr(uint8_t *field, int option_type,
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
			DCC_LOG(LOG_MSG, "overloaded message detected");
			/* skip option type and length */
			n += 2;
			*is_overloaded = field[n];
			continue;
		}

		/* requested option found? */
		if (field[n] == option_type) {
			DCC_LOG1(LOG_MSG, "option found at offset %u in options", n);
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

	DCC_LOG1(LOG_MSG, "len=%d", len);

	overload = DHCP_OVERLOAD_NONE;
	opt = msg->options;

	if ((got = dhcp_find_option_ptr(opt, option_type, len, &overload)) != NULL)
		return got;

	switch (overload) {
	case DHCP_OVERLOAD_NONE:
		return NULL;
	case DHCP_OVERLOAD_FILE:
		DCC_LOG(LOG_TRACE, "overloaded file field");
		opt = msg->file;
		len = DHCP_FILE_LEN;
		return dhcp_find_option_ptr(opt, option_type, len, NULL);
	case DHCP_OVERLOAD_SNAME:
		DCC_LOG(LOG_TRACE, "overloaded sname field");
		opt = msg->sname;
		len = DHCP_SNAME_LEN;
		return dhcp_find_option_ptr(opt, option_type, len, NULL);
	default:
	case DHCP_OVERLOAD_SNAME_FILE:
		DCC_LOG(LOG_TRACE, "overloaded sname and file field");
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
static inline int dhcp_option(uint8_t *opt, int type, int len)
{
	opt[0] = type;
	opt[1] = len;

	return 2;
}

static inline int dhcp_option_byte(uint8_t * opt, int type, uint8_t val)
{
	opt[0] = type;
	opt[1] = 1;
	opt[2] = val;

	return 3;
}

static inline int dhcp_option_short(uint8_t * opt, int type, uint16_t val)
{
	opt[0] = type;
	opt[1] = 2;
	opt[2] = val;
	opt[3] = val >> 8;

	return 4;
}

static inline int dhcp_option_long(uint8_t * opt, int type, uint32_t val)
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
		ifn_ipv4_get(dhcp->ifn, &msg->ciaddr, NULL);
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

	ifn_getaddr(dhcp->ifn, msg->chaddr);

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
	struct udp_pcb * pcb;
	uint8_t *opt;
	int n;

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

#ifdef DHCP_DEBUG
	dhcp_option_dump(msg.options, n);
	DCC_LOG1(LOG_TRACE, "dhcp_msg: %dB", sizeof(struct dhcp_msg));
#endif
	n += sizeof(struct dhcp_msg) - DHCP_OPTIONS_LEN;

	DCC_LOG2(LOG_TRACE, "%s: udp_bcast(%d).", 
			 ifn_name(dhcp->ifn), IPPORT_BOOTPS);
	if (udp_bcast(pcb, &msg, n, dhcp->ifn, HTONS(IPPORT_BOOTPS)) != n) {
		DCC_LOG(LOG_ERROR, "could not send UDP packet.");
		return -1;
	}

	dhcp_set_state(dhcp, DHCP_SELECTING);
	DCC_LOG1(LOG_TRACE, "sending DHCP discover: %d bytes.", n);


	return 0;
}

static int dhcp_request(struct dhcp * dhcp)
{
	struct dhcp_msg request;
	struct udp_pcb * pcb;
	uint8_t * opt;
	int n;

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
		DCC_LOG1(LOG_TRACE, "%s: [DHCP_REBOOTING]", ifn_name(dhcp->ifn));
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

#ifdef DHCP_DEBUG
	dhcp_option_dump(request.options, n);
	DCC_LOG1(LOG_TRACE, "dhcp_msg: %dB", (int) sizeof(struct dhcp_msg));
#endif

	n += sizeof(struct dhcp_msg) - DHCP_OPTIONS_LEN;

	/* XXX: if the transition to REBINDING state is made *after* the REQUEST, 
	   it will be sent unicast on <T2 expires> event. */
	if ((dhcp->state == DHCP_BOUND) || 
		(dhcp->state == DHCP_RENEWING)) {
		struct sockaddr_in sin;
		sin.sin_family = AF_INET;
		sin.sin_port = HTONS(IPPORT_BOOTPS);
		sin.sin_addr.s_addr = dhcp->server_ip;
		DCC_LOG3(LOG_TRACE, "%s: udp_sendto(%I %d).", 
				 ifn_name(dhcp->ifn), sin.sin_addr.s_addr, IPPORT_BOOTPS);
		if (udp_sendto(pcb, &request, n, &sin) != n) {
			DCC_LOG(LOG_ERROR, "could not send UDP packet.");
			return -1;
		}
	} else {
		DCC_LOG2(LOG_TRACE, "%s: udp_bcast(%d).", 
				 ifn_name(dhcp->ifn), IPPORT_BOOTPS);
		if (udp_bcast(pcb, &request, n, dhcp->ifn, HTONS(IPPORT_BOOTPS)) != n) {
			DCC_LOG(LOG_ERROR, "could not send UDP packet.");
			return -1;
		}
	}

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
	DCC_LOG(LOG_TRACE, "REQUESTING\n");
	dhcp_set_state(dhcp, DHCP_REQUESTING);
#endif
}

static int dhcp_init(struct dhcp * dhcp)
{
	dhcp->xid++;
	switch (dhcp->state) {
	case DHCP_INIT:
	case DHCP_SELECTING:
		return dhcp_discover(dhcp);
	case DHCP_INIT_REBOOT:
		return dhcp_request(dhcp);
	default:
		DCC_LOG(LOG_ERROR, "invalid state!");
		return -1;
	}
}

static void dhcp_decline(struct dhcp * dhcp)
{
	/* FIXME */
	DCC_LOG(LOG_ERROR, "not implemented");
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
		DCC_LOG(LOG_TRACE, "discarded.");
	}
	if ((opt = dhcp_get_option_ptr(msg, DHCP_OPTION_SERVER_ID, len)) == NULL) {
		DCC_LOG(LOG_TRACE, "no server ID, discarding.");
		return;
	}
	if ((server = get_option_long(opt)) != dhcp->next_server) {
		DCC_LOG1(LOG_TRACE, "NAK from unknown server %I, dicarding.", 
				 server);
		return;
	}
	DCC_LOG(LOG_TRACE, "NAK accepted, restarting DHCP...");
	dhcp_set_state(dhcp, DHCP_INIT);
	if (dhcp_init(dhcp) < 0) {
		DCC_LOG(LOG_ERROR, "could not reset client");
	}
}

static void dhcp_handle_ack(struct dhcp * dhcp, struct dhcp_msg * msg, int len)
{
	uint8_t * opt;
	int opt_len;
	in_addr_t aux;
	uint8_t is_configured;
	in_addr_t ip_addr;

	switch (dhcp->state) {
	case DHCP_REBOOTING:
	case DHCP_REQUESTING:
	case DHCP_RENEWING:
	case DHCP_REBINDING:
		break;
	default:
		DCC_LOG(LOG_TRACE, "discarded.");
		return;
	}

	/* // Will never happen!
	   if ((msg = dhcp->msg_in) == NULL) {
	   DCC_LOG(LOG_ERROR, "msg NULL!");
	   return;
	   } */


	ifn_ipv4_get(dhcp->ifn, &ip_addr, NULL);

	is_configured = (ip_addr != INADDR_NONE);

	if (msg->yiaddr == 0) {
		DCC_LOG(LOG_WARNING, "ACK received without IP assigned.");
		dhcp_decline(dhcp);

		if (is_configured)	// Discard invalid ack
			return;

		dhcp_init(dhcp);
		
		return;
	}
	DCC_LOG2(LOG_TRACE, "marking %I to bind to interface %s.", 
			msg->yiaddr, ifn_name(dhcp->ifn));
	dhcp->ip = msg->yiaddr;

	/* obtain pointer to DHCP message type */
	opt_len = len - ((sizeof(struct dhcp_msg) - DHCP_OPTIONS_LEN));

#ifdef DEBUG
	DCC_LOG(LOG_TRACE, "dumping received options:");
	dhcp_option_dump(msg->options, opt_len);
	dhcp_option_dump(msg->file, DHCP_FILE_LEN);
	dhcp_option_dump(msg->sname, DHCP_SNAME_LEN);
#endif

	/* TODO: lease must be specified if previous message was not INFORM.
	 *	Currently guessing it if the interface has already such yiaddr. */
	if ((opt = dhcp_get_option_ptr(msg, DHCP_OPTION_LEASE_TIME, opt_len)) 
		== NULL) {
		DCC_LOG(LOG_WARNING, "no lease time found.");
		dhcp->lease = 0;	/* No lease means: don't reset tick count (assumes 
							   this is in response to a INFORM request. */
		if (msg->yiaddr != ip_addr) {
			dhcp_decline(dhcp);
			return;
		}
	} else {
		dhcp->lease = ntohl(get_option_long(opt));
		DCC_LOG1(LOG_TRACE, "got lease: %ds.", dhcp->lease);
		dhcp->time = 0;
	}
	/* setting state here to ignore further ACKs. */
	dhcp_set_state(dhcp, DHCP_BOUND);

	if ((opt = dhcp_get_option_ptr(msg, DHCP_OPTION_SUBNET_MASK, opt_len)) 
		== NULL) {
		DCC_LOG(LOG_WARNING, "no subnet mask option, guessing.");
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
					DCC_LOG(LOG_WARNING, "could not guess.");
				}
			}
		}
	} else {
		aux = get_option_long(opt);
		dhcp->mask = aux;
		DCC_LOG1(LOG_TRACE, "got subnet mask %I.", aux);
	}

	if ((opt = dhcp_get_option_ptr(msg, DHCP_OPTION_ROUTER, opt_len)) == NULL) {
		DCC_LOG(LOG_WARNING, "no router option.");
		dhcp->gw = INADDR_NONE;
	} else {
		aux = get_option_long(opt);
		dhcp->gw = aux;
		DCC_LOG1(LOG_TRACE, "got gateway %I.", aux);
	}

	if ((opt = dhcp_get_option_ptr(msg, DHCP_OPTION_BROADCAST, opt_len)) 
		== NULL) {
		DCC_LOG(LOG_WARNING, "no broadcast option.");
		dhcp->bcast = INADDR_NONE;
	} else {
		aux = get_option_long(opt);
		dhcp->bcast = aux;
		DCC_LOG1(LOG_TRACE, "got broadcast address %I.", aux);
	}

#if 0
	if ((opt = dhcp_get_option_ptr(msg, DHCP_OPTION_HOSTNAME, opt_len)) 
		== NULL) {
		DCC_LOG(LOG_WARNING, "no hostname option.");
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
		DCC_LOG(LOG_TRACE, "got hostname: %s", dhcp->hostname);
	}

	if ((opt = dhcp_get_option_ptr(msg, DHCP_OPTION_DOMAINNAME, opt_len)) 
		== NULL) {
		DCC_LOG(LOG_WARNING, "no domain name option.");
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
		DCC_LOG(LOG_TRACE, "got domain name: %s", dhcp->domain);
	}
#endif

	dhcp->callback(dhcp);
}

/* Simply select the first offer received. */
static void dhcp_select(struct dhcp * dhcp)
{
	DCC_LOG(LOG_TRACE, "sending REQUEST.");
	/* New transaction. */
	dhcp->xid++;
	dhcp_request(dhcp);
	dhcp_set_state(dhcp, DHCP_REQUESTING);
	tmo_reply_set(dhcp);
}

static inline void dhcp_handle_offer(struct dhcp* dhcp, in_addr_t server_ip,
									 struct dhcp_msg * msg)
{
	if (dhcp->state != DHCP_SELECTING) {
		DCC_LOG(LOG_TRACE, "discarded, not SELECTING.");
		return;
	}

	dhcp->server_ip = server_ip;
	dhcp->offered_ip = msg->yiaddr;

	DCC_LOG2(LOG_TRACE, "server %I offers %I", server_ip, dhcp->offered_ip);
	dhcp_select(dhcp);

	return;
}

static void tmo_reply(struct dhcp * dhcp)
{
	switch (dhcp->state) {
	case DHCP_SELECTING:
		if ((dhcp->retries)++ == DHCP_DISCOVER_MAXIMUM_RETRIES) {
			DCC_LOG1(LOG_TRACE, "no DHCP server found after %d retries,"
					 " aborting.", DHCP_DISCOVER_MAXIMUM_RETRIES);
			dhcp->tmo_reply = 0;
			/* Issue callback. */
			dhcp->callback(dhcp);
			return;
		}
		tmo_reply_set(dhcp);
		/* Retry DISCOVER, go to INIT. */
		dhcp_init(dhcp);
		return;

	case DHCP_REQUESTING:
	case DHCP_REBOOTING:
		if ((dhcp->retries)++ == DHCP_REQUEST_MAXIMUM_RETRIES) {
			DCC_LOG(LOG_TRACE, "DHCP ACK/NAK reply timeout.");
			dhcp_set_state(dhcp, DHCP_INIT);
			dhcp_init(dhcp);
			return;
		}
		break;	// Continues!

	case DHCP_RENEWING:
	case DHCP_REBINDING:
		(dhcp->retries)++;
		break;	// Continues!
	default:
		DCC_LOG(LOG_ERROR, "invalid state!");
		dhcp->tmo_reply = 0;
		return;
	}

	tmo_reply_set(dhcp);
	dhcp_request(dhcp);
}

static void tmo_t1(struct dhcp * dhcp)
{
	if (dhcp->state != DHCP_BOUND) {
		DCC_LOG(LOG_ERROR, "invalid state");
		return;
	}

	dhcp_set_state(dhcp, DHCP_RENEWING);

	/* MUST wait at least half the remaining time to T2 before retransmission.
	   (lease/2)/2 */
	dhcp->tmo_reply = dhcp->lease >> (2 + DHCP_TIME_GRANULARITY_POWER);
	dhcp->last_tmo = dhcp->tmo_reply;

	/* Changed state, changing XID. */
	dhcp->xid++;

	dhcp_request(dhcp);
}

static void tmo_t2(struct dhcp * dhcp)
{
	if (dhcp->state != DHCP_RENEWING) {
		DCC_LOG(LOG_ERROR, "invalid state");
		return;
	}

	dhcp_set_state(dhcp, DHCP_REBINDING);

	/* MUST wait at least half the remaining time to lease expiration before 
	   retransmission. (lease/8)/2 */
	dhcp->tmo_reply = dhcp->lease >> (4 + DHCP_TIME_GRANULARITY_POWER);
	dhcp->last_tmo = dhcp->tmo_reply;

	/* Changed state, changing XID. */
	dhcp->xid++;

	dhcp_request(dhcp);
}


/*! \brief DHCP client time ticker.
 *
 *	This function should be called every DHCP_TIME_GRANULARITY (default 4) 
 *	seconds to provide a clock for the DHCP client. \n
 */

void dhcp_tmr(unsigned int dt)
{
	struct dhcp * dhcp;
	uint32_t aux;
	int i;

	for (i = 0; i < DHCP_CLIENT_MAX; ++i) {
		dhcp = &__dhcpc__.dhcp[i];

		if (dhcp->ifn == NULL)
			continue;

		if (dhcp->state == DHCP_IDLE) {
			/* start the DHCP negotiation */
			if (dhcp->ip == INADDR_ANY) {	// Previous IP unknown
				dhcp->state = DHCP_INIT;
				DCC_LOG1(LOG_TRACE, "%s: [DHCP_INIT]", 
						 ifn_name(dhcp->ifn));
			} else {								// Previous IP known
				dhcp->state = DHCP_INIT_REBOOT;
				DCC_LOG1(LOG_TRACE, "%s: [DHCP_INIT_REBOOT]", 
						 ifn_name(dhcp->ifn));
			}

			tmo_reply_set(dhcp);

			dhcp_init(dhcp);

			continue;
		}

		DCC_LOG1(LOG_INFO, "%s: ....", ifn_name(dhcp->ifn));

		dhcp->time += dt;

		/* Reply timer active? */
		if (dhcp->tmo_reply != 0) {
			if (--(dhcp->tmo_reply) == 0)
				tmo_reply(dhcp);
		}

		/* Are we under a lease? */
		if (dhcp->lease != 0) {
			/* Lease out? */
			if (dhcp->time >= dhcp->lease) {
				DCC_LOG(LOG_WARNING, "lease time off");
				dhcp->callback(dhcp);
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
	} 
}

void __attribute__((noreturn)) dhcpc_task(struct udp_pcb * udp)
{
	uint8_t buf[DHCP_MAX_SUPPORTED_MSG_SIZE];
	struct sockaddr_in sin;
	struct dhcp_msg * msg;
	uint8_t *opt;
	struct dhcp * dhcp;
	in_addr_t server_ip;
	int msg_type;
	int opt_len;
	uint32_t clk;
	uint32_t tmo;
	int len;

	/* // Will never happen!
	   if (curr == NULL) {
	   DCC_LOG(LOG_ERROR, "received UDP, but DHCP client should not be running.");
	   udp_release((struct pcbr *) pb);
	   return;
	   } */

	DCC_LOG1(LOG_TRACE, "sizeof(buf)=%d", (int)sizeof(buf));

	clk = thinkos_clock();
	tmo = clk + 500;

	for (;;) {
		clk = thinkos_clock();
		if ((int32_t)(clk - tmo) >= 0) {
			dhcp_tmr(DHCP_TIME_GRANULARITY);
			tmo = clk + (DHCP_TIME_GRANULARITY * 1000);
		}

		len = udp_recv_tmo(udp, buf, DHCP_MAX_SUPPORTED_MSG_SIZE, &sin, 
						   DHCP_TIME_GRANULARITY * 500);

		if (len == -ECONNREFUSED) {
			DCC_LOG(LOG_WARNING, "udp_rcv ICMP error: ECONNREFUSED");
			continue;
		}

		if (len == -EFAULT) {
			DCC_LOG(LOG_WARNING, "udp_rcv error: EFAULT");
			continue;
		}
			
		if (len == -ENOTCONN) {
			DCC_LOG(LOG_WARNING, "udp_rcv error: ENOTCONN");
			continue;
		}

		if (len == -ETIMEDOUT) {
			DCC_LOG(LOG_INFO, "udp_rcv_tmo(): ETIMEDOUT");
			continue;
		}

		LOG3(LOG_TRACE, "from %I.%d %d bytes", 
			 sin.sin_addr.s_addr, ntohs(sin.sin_port), len);

		/* TODO: make it identify which interface is being 
		   configured and associate with the correct dhcp* structure. */ 
		msg = (struct dhcp_msg *)buf;
		if (msg->op != BOOTP_BOOTREPLY) {
			DCC_LOG(LOG_WARNING, "DHCP operation != BOOTP_BOOTREPLY");
			continue;
		}

		/* iterate through hardware address and match against DHCP message */
		/*for (i = 0; i < netif->hwaddr_len; i++) {
		  if (netif->hwaddr[i] != msg->chaddr[i]) {
		  DCC_LOG(LOG_WARNING, "hw address mismatch.");
		  return;
		  }
		  } */

		dhcp = dhcp_xid_lookup(msg->xid);
		/* match transaction ID against what we expected */
		if (dhcp == NULL) {
			DCC_LOG1(LOG_WARNING, "no matching transaction id %d", msg->xid);
			continue;
		}

		/* obtain pointer to DHCP message type */
		opt_len = len - ((sizeof(struct dhcp_msg) - DHCP_OPTIONS_LEN));
		/* obtain the server address */
		if ((opt = dhcp_get_option_ptr(msg, DHCP_OPTION_SERVER_ID, 
									   opt_len)) == NULL) {
			DCC_LOG(LOG_WARNING, "no options?");
			continue;
		}
		server_ip = get_option_long(opt);

		/* Discarding bogus messages to avoid some attacks. */
		if (server_ip != sin.sin_addr.s_addr) {
			DCC_LOG2(LOG_WARNING, "UDP from %I with options claiming being"
					" from %I.", sin.sin_addr.s_addr, server_ip);
			continue;
		}

		if ((opt = dhcp_get_option_ptr(msg, DHCP_OPTION_MESSAGE_TYPE, 
									   opt_len)) == NULL) {
			DCC_LOG(LOG_WARNING, "no options?");
			continue;
		}

		dhcp->next_server = msg->siaddr;
		/* read DHCP message type */
		msg_type = *(opt + 2);

		switch (msg_type) {
		case DHCP_ACK:
			DCC_LOG(LOG_TRACE, "DHCP_ACK received");
			dhcp_handle_ack(dhcp, msg, len);
			dhcp->tmo_reply = 0;
			break;

		case DHCP_NAK:
			DCC_LOG(LOG_TRACE, "DHCP_NAK received");
			dhcp_handle_nak(dhcp, msg, len);
			dhcp->tmo_reply = 0;
			break;

		case DHCP_OFFER:
			DCC_LOG(LOG_TRACE, "DHCP_OFFER received");
			dhcp->tmo_reply = 0;
			dhcp_handle_offer(dhcp, server_ip, msg);
			break;

		default:
			DCC_LOG1(LOG_WARNING, "unhandled msg_type=%d", msg_type);
		}
	}
}


uint32_t dhcpc_stack[512];

const struct thinkos_thread_inf dhcpc_inf = {
	.stack_ptr = dhcpc_stack, 
	.stack_size = sizeof(dhcpc_stack), 
	.priority = 32,
	.thread_id = 32, 
	.paused = 0,
	.tag = "DHCPC"
};

int dhcpc_start(void)
{
	struct udp_pcb * udp;

	DCC_LOG1(LOG_TRACE, "thread: %d", thinkos_thread_self());

	if ((udp = udp_alloc()) == NULL) {
		DCC_LOG(LOG_WARNING, "udp_alloc() fail!");
		return -1;
	}

	if (udp_bind(udp, INADDR_ANY, htons(IPPORT_BOOTPC)) < 0) {
		DCC_LOG(LOG_WARNING, "udp_bind() fail!");
		return -1;
	}

	__dhcpc__.pcb = udp;

	return thinkos_thread_create_inf((void *)dhcpc_task, 
									 (void *)udp, &dhcpc_inf);
}

void dhcpc_stop(void)
{
	struct udp_pcb * pcb;

	if ((pcb = __dhcpc__.pcb) == NULL) {
		return;
	}

	udp_close(pcb);
}

void dhcpc_default_callback(struct dhcp * dhcp)
{
	in_addr_t ip_addr;
	in_addr_t netmask;

#ifdef DEBUG
	/* sanity check */
	if (dhcp == NULL) {
		DCC_LOG(LOG_ERROR, "dhcp cannot be NULL");
		return;
	}
#endif

	if (dhcp_get_state(dhcp) != DHCP_BOUND) {
		DCC_LOG(LOG_ERROR, "dhcp error!");
		return;
	}

	ifn_ipv4_get(dhcp->ifn, &ip_addr, &netmask);

	if (dhcp->ip == ip_addr) {	/* Reconfiguring... */
		if (dhcp->mask == INADDR_NONE)
			dhcp->mask = netmask;
		if (dhcp->gw == INADDR_NONE) {
			struct route * route;
			route = ipv4_route_lookup(INADDR_ANY);
			if (route->rt_ifn == dhcp->ifn) {
				dhcp->gw = route->rt_gateway;
				DCC_LOG1(LOG_TRACE, "No GW, found previous GW: %I.", dhcp->gw);
			}
		}
	}

	/* configure the ip address */
	if (ifn_ipv4_set(dhcp->ifn, dhcp->ip, dhcp->mask) >= 0) {
		/* add the default route (gateway) to ifn */
		ipv4_route_del(INADDR_ANY);
		ipv4_route_add(INADDR_ANY, INADDR_ANY, dhcp->gw, dhcp->ifn);
		return;
	}

	if (dhcp->bcast != INADDR_NONE) {
		ipv4_route_del(INADDR_NONE);
		ipv4_route_add(INADDR_NONE, INADDR_NONE, dhcp->bcast, dhcp->ifn);
	}
}


struct dhcp * dhcpc_ifconfig(struct ifnet * __ifn, 
							void (* __callback)(struct dhcp *))
{
	/* XXX */
	struct dhcp * dhcp = NULL; 
	struct route * rt;
	in_addr_t ip_addr;
	in_addr_t netmask;

	if (__ifn == NULL) {
		DCC_LOG(LOG_ERROR, "invalid interface!");
		return NULL;
	}

	DCC_LOG1(LOG_TRACE, "%s: ....", ifn_name(__ifn));

	if ((dhcp = dhcp_ifn_lookup(__ifn)) == NULL) {
		/* allocate memory for dhcp */
		if ((dhcp = dhcp_alloc(__ifn)) == NULL) {
			DCC_LOG(LOG_ERROR, "dhcp_alloc() failed!");
			return NULL;
		}

		ifn_ipv4_get(__ifn, &ip_addr, &netmask);

		if (__callback == NULL) 
			dhcp->callback = dhcpc_default_callback;
		else
			dhcp->callback = __callback;
		dhcp->ip = ip_addr;
		dhcp->mask = netmask;

		if ((rt = ipv4_route_lookup(INADDR_ANY)) == NULL)
			dhcp->gw = INADDR_ANY;
		else
			dhcp->gw = rt->rt_gateway;

		if ((rt = ipv4_route_lookup(ip_addr & netmask)) == NULL)
			dhcp->bcast = INADDR_ANY;
		else
			dhcp->bcast = rt->rt_gateway;

		//	dhcp->hostname = NULL;
		//	dhcp->domain = NULL;
		//	dhcp->hn_len = 0;
		//	dhcp->dn_len = 0;
		dhcp->time = 0;
		dhcp->retries = 0;
		/* transaction identifier, must be unique for each DHCP request. */
		dhcp->xid = dhcp_xid();
	} else {
		DCC_LOG(LOG_TRACE, "DHCP client renew ");
	}

	dhcp->state = DHCP_IDLE;

	return dhcp;
}

/* EOF */
