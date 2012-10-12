/* $Id: tcp_tmr.c,v 2.14 2008/06/04 00:03:14 bob Exp $ 
 *
 * File:	tcp_tmr.c
 * Module:
 * Project:
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:	
 * Comment:
 * Copyright(c) 2004-2008 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#ifdef TCP_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif

#ifdef CONFIG_H
#include "config.h"
#endif

#include <stdint.h>
#include <sys/mbuf.h>
#include <sys/net.h>

#include <tcpip/tcp.h>
#include <tcpip/arp.h>

#include <sys/dcclog.h>
#include <errno.h>

#ifndef TCP_FAST_TMR_MS
#define TCP_FAST_TMR_MS 100
#endif

/* miliseconds per fast timer ticks */
const uint16_t tcp_fast_tmr_ms = TCP_FAST_TMR_MS;

/*
 * The slow timer constants
 */

#ifndef TCP_SLOW_TMR_MS
#define TCP_SLOW_TMR_MS 500
#endif

/* miliseconds per slow timer ticks */
const uint16_t tcp_slow_tmr_ms = TCP_SLOW_TMR_MS;

#ifndef TCP_DEFAULT_MAXRXMTS
#define TCP_DEFAULT_MAXRXMTS   6
#endif

#define TCP_MAXRXMTS TCP_DEFAULT_MAXRXMTS

const uint16_t tcp_maxrxmts = TCP_MAXRXMTS;

/* the values here are in (miliseconds / TCP_SLOW_TMR_MS) */
const uint8_t tcp_rxmtintvl[] = {
	1600 / TCP_SLOW_TMR_MS,
	3200 / TCP_SLOW_TMR_MS, 
	6400 / TCP_SLOW_TMR_MS, 
	12000 / TCP_SLOW_TMR_MS, 
	24000 / TCP_SLOW_TMR_MS, 
	48000 / TCP_SLOW_TMR_MS, 
	64000 / TCP_SLOW_TMR_MS, 
	64000 / TCP_SLOW_TMR_MS,
	64000 / TCP_SLOW_TMR_MS,
	64000 / TCP_SLOW_TMR_MS,
	64000 / TCP_SLOW_TMR_MS,
	64000 / TCP_SLOW_TMR_MS };

/*
 * The idle timer constants
 */
#ifndef TCP_IDLE_TMR_MS
#define TCP_IDLE_TMR_MS 1000
#endif

/* we set the default MSL to 2 seconds becase we have no enougth resorces 
   to block PCBs for greater times */
#ifndef TCP_DEFAULT_MSL
#define TCP_DEFAULT_MSL 2
#endif

const uint16_t tcp_msl_sec = TCP_DEFAULT_MSL;

/* miliseconds per idle timer ticks */
const uint32_t tcp_idle_tmr_ms = TCP_IDLE_TMR_MS;

#ifndef TCP_CONN_EST_TMO_SEC
#define TCP_CONN_EST_TMO_SEC 10
#endif

const uint16_t tcp_conn_est_tmo_sec = TCP_CONN_EST_TMO_SEC;

#ifndef TCP_IDLE_DET_SEC
#define TCP_IDLE_DET_SEC 60
#endif

const uint16_t tcp_idle_det_sec = TCP_IDLE_DET_SEC;

#ifndef TCP_KEEP_ALIVE_PROBE_MN
#define TCP_KEEP_ALIVE_PROBE_MN 10
#endif

const uint16_t tcp_keep_alive_probe_mn = TCP_KEEP_ALIVE_PROBE_MN;

#ifndef TCP_MAX_IDLE_MN
#define TCP_MAX_IDLE_MN 15
#endif

const uint16_t tcp_max_idle_mn = TCP_MAX_IDLE_MN;


/* convert from seconds to idle timer time value */
#define SECOND_TO_IDLETV(S) ((((S) * 1000) + (TCP_IDLE_TMR_MS - 1)) \
							 / TCP_IDLE_TMR_MS)

/* convert from minutes to idle timer time value */
#define MINUTE_TO_IDLETV(M) ((((M) * 60000) + (TCP_IDLE_TMR_MS - 1)) \
							 / TCP_IDLE_TMR_MS)

#define TCPTV_MSL SECOND_TO_IDLETV(TCP_DEFAULT_MSL)

const uint16_t tcp_msl = TCPTV_MSL;

#define TCPTV_CONN_EST_TMO SECOND_TO_IDLETV(TCP_CONN_EST_TMO_SEC)

#ifdef DEBUG_KEEPALIVE
#define TCPTV_IDLE_DET_TMO SECOND_TO_IDLETV(5)
#define TCPTV_KEEP_IDLE SECOND_TO_IDLETV(10)
#define TCPTV_MAX_IDLE SECOND_TO_IDLETV(30)
#else
#define TCPTV_IDLE_DET_TMO SECOND_TO_IDLETV(TCP_IDLE_DET_SEC)
#define TCPTV_KEEP_IDLE MINUTE_TO_IDLETV(TCP_KEEP_ALIVE_PROBE_MN)
#define TCPTV_MAX_IDLE MINUTE_TO_IDLETV(TCP_MAX_IDLE_MN) 
#endif

const uint16_t tcp_conn_est_tmo = TCPTV_CONN_EST_TMO;

const uint16_t tcp_idle_det_tmo = TCPTV_IDLE_DET_TMO;

const uint8_t tcp_keepintvl[13] = {
	TCPTV_KEEP_IDLE / 4,
	TCPTV_KEEP_IDLE / 4,
	TCPTV_KEEP_IDLE / 4,
	TCPTV_KEEP_IDLE / 4,

	(TCPTV_MAX_IDLE - TCPTV_KEEP_IDLE) / 8,
	(TCPTV_MAX_IDLE - TCPTV_KEEP_IDLE) / 8,
	(TCPTV_MAX_IDLE - TCPTV_KEEP_IDLE) / 8,
	(TCPTV_MAX_IDLE - TCPTV_KEEP_IDLE) / 8,

	(TCPTV_MAX_IDLE - TCPTV_KEEP_IDLE) / 8,
	(TCPTV_MAX_IDLE - TCPTV_KEEP_IDLE) / 8,
	(TCPTV_MAX_IDLE - TCPTV_KEEP_IDLE) / 8,
	(TCPTV_MAX_IDLE - TCPTV_KEEP_IDLE) / 8,

	(TCPTV_MAX_IDLE - TCPTV_KEEP_IDLE) / 8
};

/* Fast TCP timer: 
 * This timer is used to delayed acks, periodic notifications ... 
 *
 * The system must call this function periodically. At a
 * rate of about 200ms.
 */

/* Slow TCP timer: 
 * This timer is used to retransmissions... 
 *
 * The system must call this function periodically at a
 * rate of about 500ms.
 */

/* TCP Connection timer: 
 * This timer is used to connection timeout,
 * keep alive, FIN_WAIT_2 and TIME_WAIT temporizations, as
 * well as to cleanup... 
 *
 * The system must call this function periodically at a
 * rate of about 2.5 secs.
 */

int tcp_fast_tmr(void)
{
	struct tcp_pcb * tp;
	struct mbuf * q;
	int n = 0;

//	DCC_LOG2(LOG_TRACE, "timestamp=%d", tcp_rel_timestamp());

	q = (struct mbuf *)&__tcp__.active.first;

	if (q->next) {
		DCC_LOG1(LOG_MSG, "%4u", tcp_rel_timestamp());
	}

	while ((q = q->next)) {
		tp = (struct tcp_pcb *)&q->data;

		if (tp->t_flags & TF_DELACK) {	
			DCC_LOG(LOG_TRACE, "delayed ack");
			DCC_LOG1(LOG_INFO, "%4u delayed ack", tcp_rel_timestamp());
			tp->t_flags &= ~TF_DELACK;
			tp->t_flags |= TF_ACKNOW;
			n++;
		}
	}

	return n;
}

int tcp_rxmt_tmr(void)
{
	struct tcp_pcb * tp;
	struct mbuf * q;
	int n = 0;

	/* Update the Initial Segment Sequence */
//	__tcp__.iss++;

	q = (struct mbuf *)&__tcp__.active.first;

	while ((q = q->next)) {
		tp = (struct tcp_pcb *)&q->data;

		if ((tp->t_rxmt_tmr) && (--tp->t_rxmt_tmr == 0)) {
			if (++tp->t_rxmt_cnt > TCP_MAXRXMTS) {
				/* TODO: statistics 
				   tcpstat.tcps_timeoutdrop++; */
				DCC_LOG1(LOG_TRACE, "<%05x> too many retransmissions", 
					(int)tp);
				/* TODO: close connection... */
			} else {
				uint32_t snd_una;
				uint32_t snd_nxt;
				uint32_t snd_max;

				snd_una = tp->snd_seq;
				snd_nxt = tp->snd_seq + tp->snd_off;
				snd_max = tp->snd_seq + tp->snd_max;

				DCC_LOG3(LOG_TRACE, "snd_nxt=%u snd_una=%u snd_len=%d", 
						 snd_nxt, snd_una, tp->snd_q.len);

				tp->t_rxmt_tmr = tcp_rxmtintvl[tp->t_rxmt_cnt];
				tp->snd_off = 0;
				tp->t_flags &= ~TF_SENTFIN;
				n++;
			}
		}
	}

	return n;
}

void tcp_idle_tmr(void)
{
	struct tcp_pcb * tp;
	struct mbuf * q;

	q = (struct mbuf *)&__tcp__.active.first;

	while ((q = q->next)) {
		tp = (struct tcp_pcb *)&q->data;

		if ((tp->t_conn_tmr) && (--tp->t_conn_tmr == 0)) {
			switch (tp->t_state) {
			case TCPS_SYN_SENT:
			case TCPS_SYN_RCVD:
				/* connection establishment timer */
				/* discard the data */
				mbuf_queue_free(&tp->snd_q);
				mbuf_queue_free(&tp->rcv_q);
				/* */
				tp->t_state = TCPS_CLOSED;
				pcb_move((struct pcb *)tp, &__tcp__.active, &__tcp__.closed);
				DCC_LOG1(LOG_TRACE, "<%05x> establishment timeout [CLOSED]", 
						 (int)tp);

				/* notify the upper layer */
				uthread_cond_signal(tp->t_cond);
				/* TODO: statistics */
				break;

			case TCPS_ESTABLISHED:
				/* keep alive timer */
				if ((tp->t_flags & TF_IDLE) == 0) {
					/* enter in the idle state and start counting */
					tp->t_flags |= TF_IDLE;
					tp->t_idle_cnt = 0;
					/* restarts the timer */
					tp->t_conn_tmr = tcp_keepintvl[0];
					DCC_LOG1(LOG_TRACE, "<%05x> idle", (int)tp);
					break;
				}
				tp->t_idle_cnt++;
				if (tp->t_idle_cnt < 12) {
					/* reload the keepalive timer */
					tp->t_conn_tmr = tcp_keepintvl[tp->t_idle_cnt];

					if (tp->t_idle_cnt >= 4) {
						/* send a keep alive probe */
						DCC_LOG1(LOG_TRACE, "<%05x>  keep alive probe", 
								 (int)tp);
						tcp_keepalive(tp);
					}
					break;
				}
				/* Keep alive timeout */

				break;
			case TCPS_FIN_WAIT_2:
				/* FIN_WAIT_2 timer */
				tp->t_rxmt_tmr = 0;
				tp->t_state = TCPS_TIME_WAIT;
				DCC_LOG1(LOG_TRACE, "<%05x> [TIME_WAIT]", (int)tp);
				tp->t_conn_tmr = TCPTV_MSL << 1;
				break;

			case TCPS_TIME_WAIT:
				DCC_LOG1(LOG_INFO, "<%05x> [CLOSED]", (int)tp);
				tcp_pcb_free(tp);
				break;
			}
		}
	}
}

void tcp_output_sched(struct tcp_pcb * __tp)
{
	/* schedule output */
	__tcp__.need_output = 1;
	uthread_cond_signal(__tcp__.output_cond);
}

int __attribute__((noreturn, naked)) tcp_tmr_task(void * p, uthread_id_t id)
{
	struct tcp_pcb * tp;
	struct mbuf * q;
	int rxmt;
	int idle;
	int ret;

	DCC_LOG1(LOG_TRACE, "      fast timer period : %4d ms", tcp_fast_tmr_ms);
	DCC_LOG1(LOG_TRACE, "      slow timer period : %4d ms", tcp_slow_tmr_ms);
	DCC_LOG1(LOG_TRACE, "      idle timer period : %4d ms", tcp_idle_tmr_ms);
	DCC_LOG1(LOG_TRACE, "    retransmissions max : %4d", tcp_maxrxmts);
	DCC_LOG1(LOG_TRACE, "conn. establishment tmo : %4d seconds", 
			 tcp_conn_est_tmo_sec);
	DCC_LOG1(LOG_TRACE, "     idle detection tmo : %4d seconds", 
			 tcp_idle_det_sec);
	DCC_LOG1(LOG_TRACE, "   keep alive probe tmo : %4d minutes", 
			 tcp_keep_alive_probe_mn);
	DCC_LOG1(LOG_TRACE, "    idle connection max : %4d minutes", 
			 tcp_max_idle_mn);
	DCC_LOG1(LOG_TRACE, "                    msl : %4d seconds", 
			 tcp_msl_sec);

	tcpip_net_lock();

	rxmt = TCP_SLOW_TMR_MS / TCP_FAST_TMR_MS;
	idle = TCP_IDLE_TMR_MS / TCP_FAST_TMR_MS;
	for (;;) {
		if (__tcp__.need_output) {
			__tcp__.need_output = 0;
			q = (struct mbuf *)&__tcp__.active.first;
			while ((q = q->next)) {
				tp = (struct tcp_pcb *)&q->data;
				if ((ret = tcp_output(tp)) == -EAGAIN) {
					DCC_LOG(LOG_TRACE, "tcp_output(tp)) == -EAGAIN");
					/* if the reason to fail was an arp failure
					   try query an address pending for resolution ... */
					arp_query_pending();
				}
			}
			continue;
		}
		ret = uthread_cond_timedwait(__tcp__.output_cond, net_mutex, 
									 TCP_FAST_TMR_MS);
		if (ret < 0) {

			/* timeout */
			if (tcp_fast_tmr() != 0) {
				__tcp__.need_output = 1;
			}

			if (--rxmt == 0) { 
				rxmt = TCP_SLOW_TMR_MS / TCP_FAST_TMR_MS;
				if (tcp_rxmt_tmr() != 0) {
					__tcp__.need_output = 1;
				}
			}

			if (--idle == 0) { 
				idle = TCP_IDLE_TMR_MS / TCP_FAST_TMR_MS;
				tcp_idle_tmr();
			}

		} else {
			DCC_LOG(LOG_INFO, "signal.");
		}
	}
}

