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
 * @file tcp_tmr.c
 * @brief 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __USE_SYS_TCP__
#include <sys/tcp.h>

#define __USE_SYS_ARP__
#include <sys/arp.h>
#include <sys/etharp.h>

#include <stdlib.h>
#include <errno.h>

#ifndef TCP_FAST_TMR_MS
#define TCP_FAST_TMR_MS 100
#endif

#ifndef TCP_SLOW_TMR_MS
#define TCP_SLOW_TMR_MS 500
#endif

#if TCP_SLOW_TMR_MS < (2 * TCP_FAST_TMR_MS)
#undef TCP_SLOW_TMR_MS
#define TCP_SLOW_TMR_MS  (2 * TCP_FAST_TMR_MS)
#endif

/* miliseconds per fast timer ticks */
const uint16_t tcp_fast_tmr_ms = TCP_FAST_TMR_MS;

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

#if TCP_IDLE_TMR_MS < (2 * TCP_FAST_TMR_MS)
#undef TCP_IDLE_TMR_MS 
#define TCP_IDLE_TMR_MS (2 * TCP_FAST_TMR_MS)
#endif

/* we set the default MSL to 5 seconds becase we have no enougth resorces 
   to block PCBs for greater times */
#ifndef TCP_DEFAULT_MSL_SEC
#define TCP_DEFAULT_MSL_SEC 5
#endif

const uint16_t tcp_msl_sec = TCP_DEFAULT_MSL_SEC;

/* miliseconds per idle timer ticks */
const uint32_t tcp_idle_tmr_ms = TCP_IDLE_TMR_MS;

#ifndef TCP_CONN_EST_TMO_SEC
#define TCP_CONN_EST_TMO_SEC 10
#endif

const uint16_t tcp_conn_est_tmo_sec = TCP_CONN_EST_TMO_SEC;

#ifndef TCP_IDLE_DET_SEC
#define TCP_IDLE_DET_SEC (1 * 60)
#endif

const uint16_t tcp_idle_det_sec = TCP_IDLE_DET_SEC;

#ifndef TCP_KEEP_ALIVE_PROBE_SEC
#define TCP_KEEP_ALIVE_PROBE_SEC (5 * 60)
#endif

#if (TCP_KEEP_ALIVE_PROBE_SEC < TCP_IDLE_DET_SEC)
#undef TCP_KEEP_ALIVE_PROBE_SEC
#define TCP_KEEP_ALIVE_PROBE_SEC TCP_IDLE_DET_SEC
#endif

const uint16_t tcp_keep_alive_probe_sec = TCP_KEEP_ALIVE_PROBE_SEC;

#ifndef TCP_MAX_IDLE_SEC
#define TCP_MAX_IDLE_SEC (15 * 60)
#endif

#if (TCP_MAX_IDLE_SEC < TCP_KEEP_ALIVE_PROBE_SEC)
#undef TCP_MAX_IDLE_SEC
#define TCP_MAX_IDLE_SEC TCP_KEEP_ALIVE_PROBE_SEC
#endif

const uint16_t tcp_max_idle_sec = TCP_MAX_IDLE_SEC;


/* convert from seconds to idle timer time value */
#define SECOND_TO_IDLETV(S) ((((S) * 1000) + (TCP_IDLE_TMR_MS - 1)) \
							 / TCP_IDLE_TMR_MS)

#define TCPTV_MSL SECOND_TO_IDLETV(TCP_DEFAULT_MSL_SEC)

const uint16_t tcp_msl = TCPTV_MSL;

#define TCPTV_CONN_EST_TMO SECOND_TO_IDLETV(TCP_CONN_EST_TMO_SEC)

#define TCPTV_IDLE_DET_TMO SECOND_TO_IDLETV(TCP_IDLE_DET_SEC)
#define TCPTV_KEEP_IDLE SECOND_TO_IDLETV(TCP_KEEP_ALIVE_PROBE_SEC)
#define TCPTV_MAX_IDLE SECOND_TO_IDLETV(TCP_MAX_IDLE_SEC) 

const uint16_t tcp_conn_est_tmo = TCPTV_CONN_EST_TMO;

const uint16_t tcp_idle_det_tmo = TCPTV_IDLE_DET_TMO;

static const uint16_t tcp_keepintvl[9] = {
	((TCPTV_KEEP_IDLE) - (TCPTV_IDLE_DET_TMO)),
	((TCPTV_MAX_IDLE) - (TCPTV_IDLE_DET_TMO) + 7) / 8,
	((TCPTV_MAX_IDLE) - (TCPTV_IDLE_DET_TMO) + 6) / 8,
	((TCPTV_MAX_IDLE) - (TCPTV_IDLE_DET_TMO) + 5) / 8,
	((TCPTV_MAX_IDLE) - (TCPTV_IDLE_DET_TMO) + 4) / 8,
	((TCPTV_MAX_IDLE) - (TCPTV_IDLE_DET_TMO) + 3) / 8,
	((TCPTV_MAX_IDLE) - (TCPTV_IDLE_DET_TMO) + 2) / 8,
	((TCPTV_MAX_IDLE) - (TCPTV_IDLE_DET_TMO) + 1) / 8,
	((TCPTV_MAX_IDLE) - (TCPTV_IDLE_DET_TMO) + 0) / 8
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

/* enqueue the PCB for output processing */
static void tcp_output_enqueue(struct tcp_pcb * __tp)
{
	if (!(__tp->t_flags & TF_NEEDOUTPUT)) {
		DCC_LOG1(LOG_MSG, "<%05x>", (int)__tp);
		__tcp__.out.tp[__tcp__.out.head++ % NET_TCP_PCB_ACTIVE_MAX] = __tp;
		__tp->t_flags |= TF_NEEDOUTPUT;
	}
}

/* enqueue and schedule the PCB for output processing */
void tcp_output_sched(struct tcp_pcb * __tp)
{
	if ((__tp->t_flags & TF_NEEDOUTPUT) == 0) {
		DCC_LOG2(LOG_MSG, "<%05x> head=%d", (int)__tp, __tcp__.out.head);
		__tcp__.out.tp[__tcp__.out.head++ % NET_TCP_PCB_ACTIVE_MAX] = __tp;
		__tp->t_flags |= TF_NEEDOUTPUT;
		thinkos_cond_signal(__tcp__.out.cond);
	}
}

void tcp_output_dequeue(struct tcp_pcb * __tp)
{
	/* remove PCB from the output processing fifo */
	uint16_t tail = __tcp__.out.tail;
	uint16_t head = __tcp__.out.head;
	uint16_t mark;

	DCC_LOG1(LOG_WARNING, "<%05x> ...", (int)__tp);

#if 0
	for (mark = tail; mark != head; ++mark) { 
		if (__tp == __tcp__.out.tp[mark % NET_TCP_PCB_ACTIVE_MAX]) {
			__tcp__.out.tail = tail + 1;
			while (tail != mark) {
				int i = tail++ % NET_TCP_PCB_ACTIVE_MAX;
				int j = tail % NET_TCP_PCB_ACTIVE_MAX;
				__tcp__.out.tp[j] = __tcp__.out.tp[i];
			}
			return;
		}
	}
#endif
	for (mark = tail; mark != head; ++mark) { 
		if (__tp == __tcp__.out.tp[mark % NET_TCP_PCB_ACTIVE_MAX]) {
			__tcp__.out.tp[mark] = NULL;
			return;
		}
	}
}

static void tcp_fast_tmr(void)
{
	struct tcp_pcb * tp;
	struct pcb_link * q;

//	DCC_LOG2(LOG_TRACE, "timestamp=%d", tcp_rel_timestamp());

	q = (struct pcb_link *)&__tcp__.active.first;

	if (q->next) {
		DCC_LOG1(LOG_MSG, "%4u", tcp_rel_timestamp());
	}

	while ((q = q->next)) {
		tp = (struct tcp_pcb *)&q->pcb;

		if (tp->t_flags & TF_DELACK) {	
			DCC_LOG(LOG_TRACE, "delayed ack");
			DCC_LOG1(LOG_INFO, "%4u delayed ack", tcp_rel_timestamp());
			tp->t_flags &= ~TF_DELACK;
			tp->t_flags |= TF_ACKNOW;
			tcp_output_enqueue(tp);
		}
	}
}

static void tcp_rxmt_tmr(void)
{
	struct tcp_pcb * tp;
	struct pcb_link * q;

	/* Update the Initial Segment Sequence */
//	__tcp__.iss++;

	q = (struct pcb_link *)&__tcp__.active.first;

	while ((q = q->next)) {
		tp = (struct tcp_pcb *)&q->pcb;

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
				tcp_output_enqueue(tp);

				(void)snd_una;
				(void)snd_nxt;
				(void)snd_max;
			}
		}
	}
}

void tcp_idle_tmr(void)
{
	struct tcp_pcb * tp;
	struct pcb_link * q;
	struct pcb_link * p;

	q = (struct pcb_link *)&__tcp__.active.first;
	p = q->next;
	while ((q = p) != NULL) {
		tp = (struct tcp_pcb *)&q->pcb;
		/* save the next in the list, as the current item can be removed 
		   from the list */
		p = q->next;

		DCC_LOG2(LOG_MSG, "<%05x> tmr=%d", (int)tp, tp->t_conn_tmr);

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
				thinkos_cond_signal(tp->t_cond);
				/* TODO: statistics */

				/* send reset */
				tcp_output_enqueue(tp);
				break;

			case TCPS_ESTABLISHED:
				/* keep alive timer */
				if ((tp->t_flags & TF_IDLE) == 0) {
					/* enter in the idle state and start counting */
					tp->t_flags |= TF_IDLE;
					DCC_LOG1(LOG_TRACE, "<%05x> idle", (int)tp);
					/* reset the idle counter */
					tp->t_idle_cnt = 0;
					/* restarts the timer */
					if ((tp->t_conn_tmr = tcp_keepintvl[tp->t_idle_cnt]) != 0) {
						DCC_LOG2(LOG_INFO, "<%05x> 1. tmr=%d", (int)tp, 
							 tp->t_conn_tmr);
						break;
					}
					/* if the timer is 0 then fall through and send 
					   a keep alive probe now! */
				}
				tp->t_idle_cnt++;
				if (tp->t_idle_cnt < 9) {
					/* reload the keepalive timer */
					if ((tp->t_conn_tmr = tcp_keepintvl[tp->t_idle_cnt]) != 0) {
						/* send a keep alive probe */
						DCC_LOG1(LOG_TRACE, "<%05x>  keep alive probe", 
								 (int)tp);
						tcp_keepalive(tp);
						break;
					}
					/* if the timer is 0 then fall through and close 
					   the connection */

					DCC_LOG2(LOG_INFO, "<%05x> 2. tmr=%d", (int)tp, 
							 tp->t_conn_tmr);
				}
				/* Keep alive timeout */

				/* discard the data */
				mbuf_queue_free(&tp->snd_q);
				mbuf_queue_free(&tp->rcv_q);
				/* */
				tp->t_state = TCPS_CLOSED;
				pcb_move((struct pcb *)tp, &__tcp__.active, &__tcp__.closed);
				DCC_LOG1(LOG_TRACE, "<%05x> keep alive timeout [CLOSED]", 
						 (int)tp);

				/* notify the upper layer */
				thinkos_cond_signal(tp->t_cond);
				/* TODO: statistics */
				tcp_output_enqueue(tp);
				break;
			case TCPS_FIN_WAIT_2:
				/* FIN_WAIT_2 timer */
				tp->t_rxmt_tmr = 0;
				tp->t_state = TCPS_TIME_WAIT;
				DCC_LOG1(LOG_TRACE, "<%05x> [TIME_WAIT]", (int)tp);
				tp->t_conn_tmr = TCPTV_MSL * 2;
				break;

			case TCPS_TIME_WAIT:
				tcp_pcb_free(tp);
				break;
			}
		}
	}
}

void __attribute__((noreturn)) tcp_tmr_task(void * p)
{
	int mutex = net_mutex;
	int cond = __tcp__.out.cond;
	uint32_t fast_clk;
	uint32_t rxmt_clk;
	uint32_t idle_clk;
	uint32_t clk;
	int ret;

	tcpip_net_lock();

	DCC_LOG1(LOG_TRACE, "      fast timer period : %4d ms", tcp_fast_tmr_ms);
	DCC_LOG1(LOG_TRACE, "      slow timer period : %4d ms", tcp_slow_tmr_ms);
	DCC_LOG1(LOG_TRACE, "      idle timer period : %4d ms", tcp_idle_tmr_ms);
	DCC_LOG1(LOG_TRACE, "    retransmissions max : %4d", tcp_maxrxmts);
	DCC_LOG1(LOG_TRACE, "conn. establishment tmo : %4d seconds", 
			 tcp_conn_est_tmo_sec);
	DCC_LOG1(LOG_TRACE, "     idle detection tmo : %4d seconds", 
			 tcp_idle_det_sec);
	DCC_LOG1(LOG_TRACE, "   keep alive probe tmo : %4d seconds", 
			 tcp_keep_alive_probe_sec);
	DCC_LOG1(LOG_TRACE, "    idle connection max : %4d seconds", 
			 tcp_max_idle_sec);
	DCC_LOG2(LOG_TRACE, "        msl (%4d itv) : %4d seconds", 
			 tcp_msl, tcp_msl_sec);

	clk = thinkos_clock();
	fast_clk = clk + TCP_FAST_TMR_MS;
	rxmt_clk = clk + TCP_SLOW_TMR_MS ;
	idle_clk = clk + TCP_IDLE_TMR_MS + TCP_FAST_TMR_MS;
	for (;;) {
		/* process PCBs pending for output */
		struct tcp_pcb * tp;
		uint16_t head;
		uint16_t tail;

		head = __tcp__.out.head;
	//	DCC_LOG1(LOG_TRACE, "head=%d", head);
		for (tail = __tcp__.out.tail; tail != head; ++tail) {
			int i = tail % NET_TCP_PCB_ACTIVE_MAX;
			
			tp = __tcp__.out.tp[i];

			DCC_LOG2(LOG_MSG, "<%05x> tail=%d", (int)tp, tail);

			if (tp == NULL) {
				DCC_LOG2(LOG_WARNING, "Null pointer, tail=%d i=%d!!!", tail, i);
				continue;
			}
			
			if (!(tp->t_flags & TF_NEEDOUTPUT)) {
				DCC_LOG(LOG_PANIC, "TF_NEEDOUTPUT not set!!!");
				continue;
			}

			if ((ret = tcp_output(tp)) == -EAGAIN) {
				DCC_LOG(LOG_TRACE, "tcp_output(tp)) == -EAGAIN");
				/* if the reason to fail was an arp failure
				   try query an address pending for resolution ... */
				etharp_query_pending();
				break;
			}

			__tcp__.out.tp[i] = NULL;

			tp->t_flags &= ~TF_NEEDOUTPUT;
#if (!TCP_ENABLE_TIMEWAIT)
			/* force closing TIME_WAIT sockets to save resources */
			if (tp->t_state == TCPS_TIME_WAIT) {
				DCC_LOG1(LOG_INFO, "<%05x> closing TIME_WAIT...", (int)tp);
				tcp_pcb_free(tp);
			}
#endif
		}
		__tcp__.out.tail = tail;

		ret = thinkos_cond_timedwait(cond, mutex, fast_clk - clk);

		clk = thinkos_clock();

		if ((int32_t)(clk - fast_clk) >= 0) {
			fast_clk += TCP_FAST_TMR_MS;
			/* timeout */
			tcp_fast_tmr();
		}

		if ((int32_t)(clk - rxmt_clk) >= 0) {
			rxmt_clk += TCP_SLOW_TMR_MS;
			tcp_rxmt_tmr();
		}

		if ((int32_t)(clk - idle_clk) >= 0) {
			idle_clk += TCP_IDLE_TMR_MS;
			tcp_idle_tmr();
		}
	}
}

