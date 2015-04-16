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
 * @file tcp.c
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __USE_SYS_TCP__
#include <sys/tcp.h>

#include <string.h>

const uint8_t tcp_pcb_active_max = NET_TCP_PCB_ACTIVE_MAX;

/* FIXME: listen only PCBs 
const uint16_t tcp_pcb_listen_max = NET_TCP_PCB_LISTEN_MAX;
 */

#ifndef TCP_DEFAULT_RTT
#define TCP_DEFAULT_RTT     47
#endif

const uint8_t tcp_defrtt = TCP_DEFAULT_RTT;

#ifndef TCP_MAX_RCV_QUEUE
#define TCP_MAX_RCV_QUEUE   (1460 * 2)
#endif

const uint16_t tcp_maxrcv = TCP_MAX_RCV_QUEUE;

#ifndef TCP_MAX_SND_QUEUE 
#define TCP_MAX_SND_QUEUE   (1460 * 2)
#endif

const uint16_t tcp_maxsnd = TCP_MAX_SND_QUEUE;

#ifndef TCP_MAX_WIN
#define TCP_MAX_WIN         TCP_MAX_RCV_QUEUE
#endif

const uint16_t tcp_maxwin = TCP_MAX_WIN;

//#ifndef TCP_DEFAULT_WND
//#define TCP_DEFAULT_WND     TCP_MAX_RCV_QUEUE
//#endif

//const uint16_t tcp_defwnd = TCP_DEFAULT_WND;


#ifndef TCP_DEFAULT_MSS 
#if TCP_MAX_RCV_QUEUE > 1460
#define TCP_DEFAULT_MSS 1460
#else
#define TCP_DEFAULT_MSS (TCP_MAX_RCV_QUEUE / 2)
#endif
#endif

const uint16_t tcp_defmss = TCP_DEFAULT_MSS;

#ifndef TCP_MAX_MSS 
#define TCP_MAX_MSS TCP_MAX_SND_QUEUE
#endif

const uint16_t tcp_maxmss = TCP_MAX_MSS;

#ifndef TCP_MAX_SYN_BACKLOG
#define TCP_MAX_SYN_BACKLOG 16
#endif


const uint16_t tcp_max_syn_backlog = TCP_MAX_SYN_BACKLOG;

struct tcp_system __tcp__ = {
	.closed = { 0},
	.listen = { 0},
	.active = { 0},
	.iss = 0,
	.port_seq = 0
};

#if defined(DEBUG) || defined(ENABLE_LOG)
const char * const __tcp_state[11] = {
	/* CLOSED */
	"CLOSED",
	/* LISTEN */
	"LISTEN",
	/* SYN_SENT */
	"SYN_SENT",
	/* SYN_RCVD */
	"SYN_RCVD",
	/* ESTABLISHED */
	"ESTABLISHED",
	/* CLOSE_WAIT */
	"CLOSE_WAIT",
	/* FIN_WAIT_1 */
	"FIN_WAIT_1",
	/* CLOSING */
	"CLOSING",
	/* LAST_ACK */
	"LAST_ACK",
	/* FIN_WAIT_2 */
	"FIN_WAIT_2",
	/* TIME_WAIT */
	"TIME_WAIT"
};
#endif

struct tcp_pcb * tcp_pcb_new(struct pcb_list * __list)
{
	struct tcp_pcb * tp;

	/* get a new PCB */
	if ((tp = (struct tcp_pcb *)pcb_remove_head(&__tcp__.free)) == NULL) {
		DCC_LOG(LOG_WARNING, "could not allocate a PCB");
		return NULL;
	}

	pcb_insert((struct pcb *)tp, __list);

	/* ensure the mem is clean */
	memset(tp, 0, sizeof(struct tcp_pcb));
	tp->t_cond = -1;

	DCC_LOG1(LOG_INFO, "<%05x>", (int)tp);

	return tp;
}


int tcp_pcb_free(struct tcp_pcb * tp)
{
#if 0
	if (tp == NULL) {
		DCC_LOG1(LOG_PANIC, "NULL pointer");
		return -1;
	}
#endif
	if (tp->t_flags & TF_NEEDOUTPUT) {
		DCC_LOG(LOG_WARNING, "TF_NEEDOUTPUT set?????");
		tp->t_flags &= ~TF_NEEDOUTPUT;
		tcp_output_dequeue(tp);
	}

	DCC_LOG2(LOG_INFO, "<%05x> state=%s", (int)tp, __tcp_state[tp->t_state]);

	if (tp->t_state == TCPS_LISTEN) {
		struct tcp_listen_pcb * mux = (struct tcp_listen_pcb *)tp;

		while (mux->t_head != mux->t_tail) {
			tp = (struct tcp_pcb *)mux->t_backlog[mux->t_head++];
			tcp_pcb_free(tp);
			/* wrap */
			if (mux->t_head == mux->t_max)
				mux->t_head = 0;
		}

		thinkos_sem_free(mux->t_sem);

		/* listening sockets do not have receiving or trasmmit queues,
		so we don't release this structures */
		DCC_LOG1(LOG_INFO, "<%05x> release LISTEN", (int)tp);
		return pcb_move((struct pcb *)tp, &__tcp__.listen, &__tcp__.free);
	} else  if (tp->t_state == TCPS_CLOSED) {
		DCC_LOG1(LOG_INFO, "<%05x> release CLOSED", (int)tp);
		/* connections in the close state had their buffers
		and conditional variables released already,
		just release the control block. */
		return pcb_move((struct pcb *)tp, &__tcp__.closed, &__tcp__.free);
	} else {
		DCC_LOG1(LOG_INFO, "<%05x> [CLOSED]", (int)tp);
		/* release all the control structures */

		mbuf_queue_free(&tp->rcv_q);
		mbuf_queue_free(&tp->snd_q);
		thinkos_cond_free(tp->t_cond);

		return pcb_move((struct pcb *)tp, &__tcp__.active, &__tcp__.free);
	}

	return 0;
} 

struct tcp_pcb * tcp_alloc(void)
{
	struct tcp_pcb * tp;
	
	tcpip_net_lock();

	if ((tp = tcp_pcb_new(&__tcp__.closed)) != NULL)
		tp->t_state = TCPS_CLOSED;

	DCC_LOG1(LOG_INFO, "<%05x>", (int)tp);

	tcpip_net_unlock();

	return tp;
}

extern int  tcp_tmr_task(void * p);


#if (ENABLE_TCP_PROFILING)

#include <sys/prof.h>

uint32_t tcp_last_timestamp;

uint32_t tcp_timestamp(void)
{
	return prof_clock();
}

uint32_t tcp_rel_timestamp(void)
{
	uint32_t ts = prof_clock();
	uint32_t dt = (ts - tcp_last_timestamp) & 0xffff;

	tcp_last_timestamp = ts;

	return dt; 
}
#else
uint32_t tcp_rel_timestamp(void)
{
	return 0;
}
#endif

#if defined(DEBUG) || defined(ENABLE_TCPDUMP) || defined(IP_DEBUG) || \
	defined(ETHARP_DEBUG)
uint32_t tcp_tmr_stack[128];
#else
uint32_t tcp_tmr_stack[96];
#endif

const struct thinkos_thread_inf tcp_tmr_inf = {
	.stack_ptr = tcp_tmr_stack, 
	.stack_size = sizeof(tcp_tmr_stack), 
	.priority = 32,
	.thread_id = 32, 
	.paused = 0,
	.tag = "TCP_TMR"
};

void tcp_init(void)
{
	int i;
	DCC_LOG(LOG_TRACE, "initializing TCP subsystem."); 

	pcb_list_init(&__tcp__.free);
	for (i = 0; i < NET_TCP_PCB_ACTIVE_MAX; ++i) {
		struct tcp_pcb * tp = &__tcp__.pcb_pool[i].pcb;
		pcb_insert((struct pcb *)tp, &__tcp__.free);
	}
	pcb_list_init(&__tcp__.closed);
	pcb_list_init(&__tcp__.listen);
	pcb_list_init(&__tcp__.active);

	__tcp__.out.head = 0;
	__tcp__.out.tail = 0;
	__tcp__.out.cond = thinkos_cond_alloc();
	DCC_LOG1(LOG_TRACE, "tcp output_cond=%d", __tcp__.out.cond);

	DCC_LOG1(LOG_TRACE, "   max active TCP PCBs : %d ", tcp_pcb_active_max);
	DCC_LOG1(LOG_TRACE, " max receive queue len : %d ", tcp_maxrcv);
	DCC_LOG1(LOG_TRACE, "max transmit queue len : %d ", tcp_maxsnd);
	DCC_LOG1(LOG_TRACE, "    max receive window : %d ", tcp_maxwin);
	DCC_LOG1(LOG_TRACE, "           default mss : %d ", tcp_defmss);
	DCC_LOG1(LOG_TRACE, "               max mss : %d ", tcp_maxmss);

#if 0
	DCC_LOG1(LOG_TRACE, "max listen TCP PCBs : %d ", tcp_pcb_listen_max);
#endif

	thinkos_thread_create_inf((void *)tcp_tmr_task, NULL, &tcp_tmr_inf);

#if (ENABLE_TCP_PROFILING)
	prof_clock_init();
	prof_clock_start();
#endif
}

