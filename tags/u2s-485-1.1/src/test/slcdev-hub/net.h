/* 
 * File:	 net.h
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2013 Bob Mittmann. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef __NET_H__
#define __NET_H__

struct netstats {
	struct {
		uint32_t pkt_cnt;
		uint32_t err_cnt;
		uint32_t seq_err_cnt;
		uint32_t octet_cnt;
	} rx;
	struct {
		uint32_t pkt_cnt;
		uint32_t err_cnt;
		uint32_t octet_cnt;
	} tx;
};

#ifdef __cplusplus
extern "C" {
#endif

#include "sndbuf.h"

/* ----------------------------------------------------------------------
 * NET subsystem
 * ----------------------------------------------------------------------
 */
int net_init(void);

int net_send(const void * buf, int len);

void net_pkt_mode(bool en);

int net_pkt_send(const void * buf, int len);

int net_probe(void);

void net_probe_enable(void);

void net_probe_disable(void);

void net_get_stats(struct netstats * stat, bool rst);

int audio_send(int stream, sndbuf_t * buf, uint32_t ts);

int audio_recv(int stream, sndbuf_t * buf, uint32_t * ts);

int g711_alaw_send(int stream, sndbuf_t * buf, uint32_t ts);

int g711_alaw_recv(int stream, sndbuf_t * buf, uint32_t * ts);


#ifdef __cplusplus
}
#endif	

#endif /* __NET_H__ */
