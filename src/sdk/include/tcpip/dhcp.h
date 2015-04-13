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

#ifndef __TCPIP_DHCP_H__
#define __TCPIP_DHCP_H__

#include <tcpip/ifnet.h>

struct dhcp;


#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Default DHCP configuration OK callback.
 *	This is provided so it is possible to use it inside a custom callback.
 */
void dhcpc_default_callback(struct dhcp * dhcp);

/*!	\brief Starts DHCP client daemon.
 *
 *	\return
 *	Returns <b>0</b> on success and less then 0 on failure.
 */
int dhcpc_start(void);

/*!	\brief Stops DHCP client daemon.
 *
 *	Stops listening on the allocated UDP socket (thus stopping all DHCP client 
 *	action).
 */
void dhcpc_stop(void);

/*!	\brief Binds a DHCP client to a network interface.
 *
 *	Begins searching for a DHCP server to negotiate network parameters and 
 *	configure local network subsystem (IP address, routers, etc.).
 *
 *	\param __ifn
 *	Pointer to the descriptor whose interface is to be configured.
 *	\param __callback
 *	Function to be called in response to DHCP events.
 *	\return
 *	Returns <b>0</b> on success and less then 0 on failure.
 */
struct dhcp * dhcpc_ifconfig(struct ifnet * __ifn, 
							void (* __callback)(struct dhcp *));

int dhcp_ifrelease(struct ifnet * __ifn);

int dhcp_ifdone(struct ifnet * __ifn);

/*! Returns current state of the DHCP client state machine. */
enum dhcp_state dhcp_get_state(struct dhcp * dhcp);


#ifdef __cplusplus
}
#endif

#endif /* __TCPIP_DHCP_H__ */

/*! @} */
/*! @} */
