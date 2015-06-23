/* 
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
 * @file rtp.h
 * @brief 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __RTP_H__
#define __RTP_H__

#include <tcpip/udp.h>

struct rtp_client {
	in_addr_t addr;
	uint16_t port;
	uint32_t start_seq;
	uint32_t seq_no;
	struct udp_pcb * udp;
};

#ifdef __cplusplus
extern "C" {
#endif

int rtp_recv(struct rtp_client * __rtp, void * __buf, int __len);


#ifdef __cplusplus
}
#endif

#endif /* __RTP_H__ */

