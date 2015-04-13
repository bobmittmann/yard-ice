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
 * @file tcpip/udp.h
 * @brief 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

/*! \addtogroup tcpip
 * @{
 */
/*! \defgroup tcpip_udp tcpip/udp.h
 * @{
 */	 
/*! \file
 *	Revision tag: $Revision: 2.5 $ $Date: 2008/06/04 00:03:14 $
 *	@author Robinson Mittman(bob@boreste.com)
 */

#ifndef __TCPIP_UDP_H__
#define __TCPIP_UDP_H__

#include <tcpip/ifnet.h>
#include <tcpip/stat.h>

/*
 * UDP protocol control block info
 */   
struct udp_inf
{
	/* foreign address */
	in_addr_t faddr;
	/* local address */
	in_addr_t laddr;
	/* foreign port */
	uint16_t fport;
	/* local port */
	uint16_t lport;
};

/*
 * UDP protocol control block
 */   
struct udp_pcb;

extern const uint8_t udp_pcb_active_max;
extern const uint8_t udp_rcev_queue_len;
extern const uint8_t udp_def_ttl;
extern const uint8_t udp_def_tos;

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Allocates a new UDP control block.
 */
struct udp_pcb * udp_alloc(void);

int udp_bind(struct udp_pcb * __up, in_addr_t __addr, uint16_t __port);

int udp_connect(struct udp_pcb * __up, in_addr_t __addr, uint16_t __port);

int udp_send(struct udp_pcb * __up, void * __buf, int __len);

/*!	\brief Sends a UDP packet.
 *
 *	Long description unavailable.
 *	\param __pb \n
 *	\param __buf \n
 *	Pointer to where the data lie.
 *	\param __len \n
 *	Lengh of the data.
 *	\param __sin \n
 *	\return
 *	On success, \a __len is returned; otherwise, 0 or less is returned.
 */
int udp_sendto(struct udp_pcb * __pb, void * __buf, int __len, 
	const struct sockaddr_in * __sin);

/*!	\brief Receives a UDP packet.
 *
 *	Retrieves first \a __len bytes from the socket linked with \a __pb and 
 *	stores them into \a __buf.
 *
 *	\param __pb \n
 *	Pointer to a UDP protocol control block.
 *	\param __buf \n
 *	Pointer to where the data will be stored.
 *	\param __len \n
 *	Maximum lengh of the data to be retrieved. Do not pass a value greater 
 *	than the actual lengh of the buffer, or it will overflow and the results
 *	will be unpredictable.
 *	\return
 *	On success, the number of bytes written to \a __buf is returned.\n
 *	On checksum error, 0 is returned. A value less than zero is returned on 
 *	other errors.
 */
int udp_recv(struct udp_pcb * __pb, void * __buf, int __len, 
			 struct sockaddr_in * __sin);

int udp_recv_tmo(struct udp_pcb * __up, void * __buf, int __len, 
				 struct sockaddr_in * __sin, int msec);

int udp_enum(int (* __callback)(struct udp_inf *, void *), void * __arg);

/* get the network interface statistic counters, 
   optionally reseting the counters */
void udp_proto_getstat(struct proto_stat * __st, int __rst);

int udp_close(struct udp_pcb * __up);

int udp_bcast(struct udp_pcb * __up, void * __buf, int __len, 
			  struct ifnet * __ifn, unsigned int __dport);

#ifdef __cplusplus
}
#endif

#endif /* __TCPIP_UDP_H__ */

