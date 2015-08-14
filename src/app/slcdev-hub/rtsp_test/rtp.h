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
#include "sndbuf.h"

struct ntp_time {
    uint32_t sec;
    uint32_t frac;
};

#if 0
static inline void get_ntp_time(struct ntp_time * tm) {
    tm->sec = (uint32_t)sec + 2208988800u;
    tm->frac = (((uint64_t)msec) << 32) / 1000u;
}
#endif

struct rtp_session {
	volatile uint32_t ssrc;
	in_addr_t faddr;
	uint16_t lport[2];
	uint16_t fport[2];
	uint32_t start_seq;
	uint32_t seq_no;
	uint32_t pkt_count;
	uint32_t octet_count;
	uint32_t ts;
	struct udp_pcb * udp[2];

	uint8_t buf[SNDBUF_LEN];
	unsigned int rem;
};

#ifdef __cplusplus
extern "C" {
#endif

void rtp_g711_start(struct rtp_session * rtp);

int rtp_g711_recv(struct rtp_session * __rtp, struct sockaddr_in * __sin);

void get_ntp_time(struct ntp_time * tm);

int rtcp_send_sr_sdes(struct rtp_session * __rtp, uint32_t __rtptime,
		struct ntp_time * __ntp, struct sockaddr_in * __sin);

void rtp_close_session(struct rtp_session * rtp);

void rtp_open_session(struct rtp_session * rtp, uint32_t ssrc);

#ifdef __cplusplus
}
#endif

#endif /* __RTP_H__ */

