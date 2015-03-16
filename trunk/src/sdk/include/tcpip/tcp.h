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
 * @file tcpip/tcp.h
 * @brief 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __TCPIP_TCP_H__
#define __TCPIP_TCP_H__

#include <stdint.h>
#include <netinet/in.h>
#include <tcpip/stat.h>

/*
 * TCP protocol control block info
 */   
struct tcp_inf
{
	/* foreign address */
	in_addr_t faddr;
	/* local address */
	in_addr_t laddr;
	/* foreign port */
	uint16_t fport;
	/* local port */
	uint16_t lport;
	/* tcp state */
	uint8_t state;
};

/*
 * TCP protocol control block
 */   
struct tcp_pcb;

/*
 * TCP flush queue selection
 */

#define TCP_FLUSH_RECV 1

#define TCP_FLUSH_SEND 2

#define TCP_FLUSH_ALL 3

/*
 * TCP send flags
 */

/* The data is to be considered read-only and no copy will be made
	to memory buffers, a pointer to the data will be use instead. */
#define TCP_SEND_NOCOPY 0x0001

/* When the send function can't copy all the data to the memory
	buffers the operation will return 0, if the TSF_NOATOMIC flag
	is set, then the function will try to copy as much data as possible,
	and will return the number of bytes copied. */
#define TCP_SEND_NOATOMIC 0x0002

/* Disable the Nagle algorithm. This cause the data to be sent as soon as
   possible. */
#define TCP_SEND_NOWAIT 0x0004

/* the maximum number of retransmissions before droping the connection */
extern const uint16_t tcp_maxrxmt;

/* the default value for the maximum segment size */
extern const uint16_t tcp_defmss;

extern const uint16_t tcp_maxmss;

/* the default value for the roud trip time */
extern const uint16_t tcp_defrtt;

/* the maximum size of the receive queue per connection */
extern const uint16_t tcp_maxrcv;

/* the maximum size of the transmit queue per connection */
extern const uint16_t tcp_maxsnd;

/* the maximum size of the receive window */
extern const uint16_t tcp_maxwin;

/* the maximum length of the queue for incomplete sockets */
extern const uint16_t tcp_max_syn_backlog;

/* the value of miliseconds per fast timer ticks */
extern const uint16_t tcp_fast_ms_tick;

/* the value of miliseconds per slow timer ticks */
extern const uint16_t tcp_slow_ms_tick;

/*
 * |-------- tcp_keepidle --------------|----- tcp_maxidle -----|
 * |                                    |      |      |         | 
 * V                                    V      v      v         V
 *                                    [keep alive probes] ... [drop]
 */

/* the total of miliseconds per itdle timer ticks */
extern const uint32_t tcp_idle_ms_tick;

/* connection-establishment timer value */
extern const uint16_t tcp_conn_est_tmo;

/* idle detection timeout */
extern const uint16_t tcp_idle_det_tmo;

/* idle time for connection before first probe */
/* the keep idle timeout in TCP_IDLE_TICKS */
extern const uint16_t tcp_keep_idle;

/* the maximum time a connection may stay idle before dropping 
   given in TCP_IDLE_TICKS units */
extern const uint16_t tcp_max_idle;

/* the maximum segment lifetime in TCP_IDLE_TICKS units */
extern const uint16_t tcp_msl;

extern const char tcp_pcb_flags[64][8];

const char tcp_basic_flags[16][5];

extern const char tcp_all_flags[64][8];


#define TCP_LISTEN_TEMPLATE(_FADDR_, _LPORT_) \
	((struct tcp_pcb) { \
	.t_faddr = (in_addr_t)_FADDR_, \
	.t_laddr = INADDR_ANY, {{ \
		.t_fport = 0, \
		.t_lport = (uint16_t)HTONS(_LPORT_) }}, \
	.t_state = TCPS_LISTEN })

#ifdef __cplusplus
extern "C" {
#endif

/* API functions */

/* Alloc a new TCP PCB (Protocol Control Block). The newlly created
 TCP is in the CLOSED state. */

/* NAME
 *        tcp_alloc - allocate a new tcp pcb (protocol control
 *        block) structure.
 *
 * DESCRIPTION
 *
 * RETURN VALUE
 *        On success, the allocated tcp pcb 
 * 
 * NOTES
 *
 * ERRORS
 * 
 */
struct tcp_pcb * tcp_alloc(void);

int tcp_bind(struct tcp_pcb * __tp, in_addr_t __addr, uint16_t __port);

int tcp_connect(struct tcp_pcb * __tp, in_addr_t __addr, uint16_t __port);

int tcp_listen(struct tcp_pcb * __mux, int __backlog);

struct tcp_pcb * tcp_accept(const struct tcp_pcb * __mux);

struct tcp_pcb * tcp_accept_tmo(const struct tcp_pcb * __mux, int tmo);

int tcp_recv(struct tcp_pcb * __tp, void * __buf, int __len);

int tcp_send(struct tcp_pcb * __tp, const void * __buf, 
			 int __len, int __flags);

int tcp_close(struct tcp_pcb * __tp);

int tcp_shutdown(struct tcp_pcb * __tp, int __how);

/*
 * Flushes the TCP queues.
 *
 * the 'which' parameter can be one of TCP_FLUSH_RECV, TCP_FLUSH_SEND 
 * or TCP_FLUSH_ALL, to discards the receive, transmmit or both 
 * buffers.
 * The data in the transmmit queue that where already sent and
 * is awaiting acknowlegement is not discarded.
 *
 */

int tcp_flush(struct tcp_pcb * __tp, int __which);

int tcp_drop(struct tcp_pcb * __tp);

/* Debugging */
int tcp_pcb_stat(struct tcp_pcb * tp, char * buf);

/* get the network interface statistic counters, 
   optionally reseting the counters */
void tcp_proto_getstat(struct proto_stat * __st, int __rst);


int tcp_enum(int (* __callback)(struct tcp_inf *, void *), void * __arg);

#ifdef __cplusplus
}
#endif

#endif /* __TCPIP_TCP_H__ */

