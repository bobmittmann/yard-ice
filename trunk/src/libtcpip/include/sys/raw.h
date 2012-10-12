/* $Id: raw.h,v 2.1 2008/06/04 00:03:14 bob Exp $ 
 *
 * File:	tcpip/raw.h
 * Module:	
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:	
 * Comment:	TCPIP
 * Copyright(c) 2008 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

/*! \addtogroup comm
 * @{
 */
/*! \defgroup tcpip_udp tcpip/raw.h
 * @{
 */	 
/*! \file
 *	Revision tag: $Revision: 2.1 $ $Date: 2008/06/04 00:03:14 $
 *	@author Robinson Mittman(bob@boreste.com)
 */

#ifndef __SYS_RAW_H__
#define __SYS_RAW_H__

#ifndef __USE_SYS_RAW__
#error "Never use <sys/raw.h> directly; include <tcpip/raw.h> instead."
#endif 

#ifdef CONFIG_H
#include "config.h"
#endif

#ifdef RAW_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif
#include <sys/dcclog.h>

#define __USE_SYS_NET__
#include <sys/net.h>
#include <sys/pcb.h>

#include <tcpip/in.h>
#include <tcpip/ip.h>
#include <tcpip/route.h>

/* raw pcb flags */
#define RF_NONBLOCK     0x80

struct raw_stat {
	uint32_t rx_pkt;
	uint32_t rx_drop;
	uint32_t rx_err;
	uint32_t tx_pkt;
	uint32_t tx_drop;
	uint32_t tx_err;
};

struct raw_pcb {
	/*! foreign address */
	in_addr_t r_faddr;
	/*! local address */
	in_addr_t r_laddr;

	union {
		struct {
			/* foreign port */
			uint16_t r_fport;
			/* local port */
			uint16_t r_lport;
		};
		uint32_t r_ports;
	};

	uint8_t r_flags;
	uint8_t r_state;

	int8_t r_cond;

	/*! ip type of service  */
	uint8_t r_tos;

	uint16_t r_protocol;

	/*! mtu */
	uint16_t r_mtu;

	uint16_t r_len;

	void * r_buf;
	struct iphdr * r_ip;
};

extern struct raw_pcb * ip_raw;

#ifdef __cplusplus
extern "C" {
#endif

int raw_pcb_free(struct raw_pcb * __raw);

int raw_input(struct ifnet * __if, struct iphdr * __ip, int __len);

#ifdef __cplusplus
}
#endif

#endif /* __SYS_RAW_H__ */

