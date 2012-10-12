/* $Id: tcp.h,v 1.14 2008/06/04 00:03:14 bob Exp $ 
 *
 * File:	sys/tcp.h
 * Module:	
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:	
 * Comment:	TCP
 * Copyright(c) 2003-2007 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#ifndef __SYS_TCP_H__
#define __SYS_TCP_H__

#ifndef __USE_SYS_TCP__
#error "Never use <sys/tcp.h> directly; include <tcpip/tcp.h> instead."
#endif 

#ifdef CONFIG_H
#include "config.h"
#endif

#ifdef TCP_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif
#include <sys/dcclog.h>

#include <stdint.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include <tcpip/ifnet.h>
#include <tcpip/stat.h>

#define __USE_SYS_NET__
#include <sys/net.h>
#include <sys/pcb.h>

#include <thinkos.h>

/* tcp states */
#define TCPS_CLOSED         0
#define TCPS_LISTEN         1
#define TCPS_SYN_SENT       2
#define TCPS_SYN_RCVD       3
#define TCPS_ESTABLISHED    4
#define TCPS_CLOSE_WAIT     5
#define TCPS_FIN_WAIT_1     6
#define TCPS_CLOSING        7
#define TCPS_LAST_ACK       8
#define TCPS_FIN_WAIT_2     9
#define TCPS_TIME_WAIT      10

#define TCPS_HAVERCVDSYN(S) ((S) >= TCPS_SYN_RCVD)
#define TCPS_HAVERCVDFIN(S) ((S) >= TCPS_TIME_WAIT)

/* tcp pcb flags */
#define TF_ACKNOW       0x01
#define TF_DELACK       0x02
#define TF_SENTFIN      0x04
#define TF_IDLE         0x08
#define TF_RXMT         0x10
#define TF_NODELAY      0x20
//#define TF_ERROR        0x10
#define TF_NONBLOCK     0x80

/* useful macros */
#define SEQ_LT(A, B) ((int32_t)((A) - (B)) < 0)
#define SEQ_LEQ(A, B) ((int32_t)((A) - (B)) <= 0)
#define SEQ_GT(A, B) ((int32_t)((A) - (B)) > 0)
#define SEQ_GEQ(A, B) ((int32_t)((A) - (B)) >= 0)

#ifndef MIN
#define MIN(X, Y) (((int)(X) < (int)(Y)) ? (X) : (Y))
#endif

#ifndef MAX
#define MAX(X, Y) (((int)(X) > (int)(Y)) ? (X) : (Y))
#endif

#ifndef ENABLE_TCP_PROTO_STAT
#define ENABLE_TCP_PROTO_STAT 0
#endif

struct tcp_listen_pcb {
	/* foreign address */
	in_addr_t t_faddr;
	/* local address */
	in_addr_t t_laddr;
	union {
		struct {
			/* foreign port */
			uint16_t t_fport;
			/* local port */
			uint16_t t_lport;
		};
		uint32_t t_ports;
	};
	/* tcp flags + general flags */
	uint8_t t_flags;
	/* tcp state */
	uint8_t t_state;
	/* conditional variable */
	int8_t t_cond;
	/* event counter */
	uint8_t t_count;

	uint8_t t_max;
	uint8_t t_head;
	uint8_t t_tail;
	struct tcp_pcb * t_backlog[0]; 
};

struct tcp_pcb {
	/* foreign address */
	in_addr_t t_faddr;
	/* local address */
	in_addr_t t_laddr;
	union {
		struct {
			/* foreign port */
			uint16_t t_fport;
			/* local port */
			uint16_t t_lport;
		};
		uint32_t t_ports;
	};
	/* tcp flags + general flags */
	uint8_t t_flags;
	/* tcp state */
	volatile uint8_t t_state;
	/* conditional variable */
	int8_t t_cond;
	/* event counter:
	   when idle count idle intervals otherwise, 
	   count the retransmissions */
	uint8_t t_count;
	/* 16 */

	/* retransmission timer */
	uint8_t t_rxmt_tmr;
	/* connection timer: (keep alive/idle, time wait) */
	uint8_t t_conn_tmr;

	/* maximum segment size to send */
	uint16_t t_maxseg;

	/* 20 */
	/* route */
	struct route * t_route;

	/* sequence number at head of the send buffer */
	uint32_t snd_seq;
	/* offset in bytes from the head of the send buffer */
	uint16_t snd_off;
	/* maximum data already sent, including SYN, FIN; 
	 * used in retransmissions */
	uint16_t snd_max;

	/* 28 */
	/* actual send window size */
	uint16_t snd_wnd;
	/* advertised receive window size */
	uint16_t rcv_adv_wnd;

	/* 32 */

	/* next seqno expected */
	uint32_t rcv_nxt;

	/* 40 */
	struct mbuf_queue rcv_q;

	/* 52 */
	struct mbuf_queue snd_q;

	/* 64 */
};

#define TCP_BACKLOG_MAX ((sizeof(struct tcp_pcb) - \
						  sizeof(struct tcp_listen_pcb)) / sizeof(void *))

#define t_rxmt_cnt t_count
#define t_idle_cnt t_count

extern const uint8_t tcp_keepintvl[];
extern const uint8_t tcp_rxmtintvl[];

struct tcp_system {
	/* list of closed PCBs */
	struct pcb_list closed;
	/* list of listen state PCBs */
	struct pcb_list listen;
	/* list of active PCBs */
	struct pcb_list active;
	/* tcp initial segment sequence */
	uint16_t iss;
	/* used to generate ephemeral port numbers */
	uint16_t port_seq;
	volatile uint8_t need_output;
	int8_t output_cond;
	int8_t accept_cond;
#if ENABLE_TCP_PROTO_STAT
	struct proto_stat stat;
#endif
};

#if ENABLE_TCP_PROTO_STAT
#define TCP_PROTO_STAT_ADD(STAT, VAL) __tcp__.stat.STAT += (VAL)
#else
#define TCP_PROTO_STAT_ADD(STAT, VAL)
#endif

extern struct tcp_system __tcp__;

#define TCPDUMP_RX 0
#define TCPDUMP_TX 1

#ifdef __cplusplus
extern "C" {
#endif

/* Internals */
struct tcp_pcb * tcp_pcb_new(struct pcb_list * __list);

int tcp_input(struct ifnet * __if, struct iphdr * iph, 
			   struct tcphdr * tcp, int len);

int tcp_output(struct tcp_pcb * tp);

int tcp_pcb_free(struct tcp_pcb * tp);

int tcp_respond(struct iphdr * ip, struct tcphdr * th, 
	tcp_seq ack, tcp_seq seq, int flags);

int tcp_keepalive(struct tcp_pcb * __tp);

void tcp_abort(struct tcp_pcb * __tp);

int tcp_dump(struct iphdr * iph, struct tcphdr * th, int dir);

uint32_t tcp_timestamp(void);

uint32_t tcp_rel_timestamp(void);

void tcp_output_sched(struct tcp_pcb * __tp);

#ifdef __cplusplus
}
#endif

#endif /* __SYS_TCP_H__ */

