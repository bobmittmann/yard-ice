/* $Id: dhcp.h,v 2.3 2006/04/04 13:27:05 bob Exp $ 
 *
 * File:	tcpip/dhcp.h
 * Module:	
 * Project:	
 * Author:	Carlos A. V. e Vieira (carlos.vieira@boreste.com)
 * Target:	
 * Comment:	DHCP client module
 * Copyright(c) 2005 BORESTE. All Rights Reserved.
 *
 */

/*! \addtogroup comm
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

#ifndef __TCPIP_DHCP_H__
#define __TCPIP_DHCP_H__

#include <tcpip/ifnet.h>
#include <sys/msg.h>

#if	(defined(DHCP_TIME_GRANULARITY_POWER) && (DHCP_TIME_GRANULARITY_POWER < 2))
#undef DHCP_TIME_GRANULARITY_POWER
#warning "DHCP_TIME_GRANULARITY_POWER cannot be < 2."
#endif
#ifndef DHCP_TIME_GRANULARITY_POWER
#define DHCP_TIME_GRANULARITY_POWER 2
#endif

/*!	\brief Defines the clock tick time interval in seconds. */
#define DHCP_TIME_GRANULARITY 5

/*! DHCP client states */
enum dhcp_state {
	DHCP_INIT		= 1,
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

struct dhcp_extra;

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

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief DHCP client time ticker.
 *
 *	This function should be called every DHCP_TIME_GRANULARITY (default 4) 
 *	seconds to provide a clock for the DHCP client. \n
 */
void dhcp_tmr(void);

/*! \brief Default DHCP configuration OK callback.
 *	This is provided so it is possible to use it inside a custom callback.
 */
void dhcp_default_callback(struct dhcp * dhcp);

/*!	\brief Starts DHCP client.
 *
 *	Begins searching for a DHCP server to negotiate network parameters and 
 *	configure local network subsystem (IP address, routers, etc.).
 *
 *	\param net \n
 *	Pointer to the descriptor whose interface is to be configured.
 *	\return
 *	Returns <b>0</b> on success and less then 0 on failure.
 */
//int dhcp_start(struct dhcp * dhcp);
struct udp_pcb * dhcp_start(void);

/*!	\brief Stops DHCP client.
 *
 *	Stops listening on the allocated UDP socket (thus stopping all DHCP client 
 *	action).
 */
void dhcp_stop(void);

struct dhcp * dhcp_ifconfig(struct ifnet * __ifn, 
							void (* __callback)(struct dhcp *));

int dhcp_ifrelease(struct ifnet * __ifn);

int dhcp_ifdone(struct ifnet * __ifn);

/*! Returns current state of the DHCP client state machine. */
enum dhcp_state dhcp_get_state(struct dhcp * dhcp);


#ifdef __cplusplus
}
#endif

#include <sys/dhcp.h>

#endif /* __TCPIP_DHCP_H__ */

/*! @} */
/*! @} */
