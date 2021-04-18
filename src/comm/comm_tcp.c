/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the YARD-ICE.
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
 * @file comm_tcp.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifdef CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <tcpip/tcp.h>
#include <netinet/in.h>
#include <time.h>
#include <trace.h>
#include <thinkos.h>

#include "jtag_arm.h"
#include "armice.h"
#include "arm_pn.h"
#include "dbglog.h"
#include "ice_comm.h"

struct comm_tcp {
	int con_flag;
	ice_comm_t * comm;
	struct tcp_pcb * volatile tp;
};

void __attribute__((noreturn)) comm_tcp_write_task(struct comm_tcp * parm)
{
	struct tcp_pcb * tp = parm->tp;
	ice_comm_t * comm = parm->comm;
	uint8_t net_buf[128];
	uint8_t * ptr;
	int len;
	int rem;
	int n;

	DCC_LOG1(LOG_TRACE, "thread=%d init...", thinkos_thread_self());

	for (;;) {
		/* wait for a connection */
		DCC_LOG(LOG_TRACE, "waiting connect...");
		thinkos_flag_take(parm->con_flag);

		while ((tp = parm->tp) != NULL) {

			DCC_LOG1(LOG_TRACE, "tp=0x%08x, connected.", (int)tp);

			/* receive data form network */
			if ((len = tcp_recv(tp, net_buf, 128)) <= 0) {
				DCC_LOG1(LOG_WARNING, "tcp_recv(): %d", len);
				parm->tp = NULL;
				break;
			}

			//		DCC_LOG3(LOG_TRACE, "%I:%d recv: %d", tp->t_faddr, 
			//				 ntohs(tp->t_fport), len);

#if 0
			ptr = net_buf;
			for (n = 0; n < len; n++) {
				DCC_LOG1(LOG_INFO, "rx: %c", 
						 (*ptr < ' ') || (*ptr > 127) ? '.' : *ptr);

				ptr++;
			}
#endif

			rem = len;
			ptr = net_buf;
			while (rem) {
				if ((n = ice_comm_write(comm, ptr, len, 500)) < 0) {
					DCC_LOG1(LOG_WARNING, "ice_comm_write(): %d", n); 
					break;
				}
				ptr += n;
				rem -= n;
			}
		}

		ice_comm_close(comm);
	} 
}

#define COMM_TCP_BUF_SIZE 512

void __attribute__((noreturn)) comm_tcp_read_task(struct comm_tcp * parm)
{
	uint32_t buf[COMM_TCP_BUF_SIZE / sizeof(uint32_t)];
	ice_comm_t * comm = parm->comm;
	struct tcp_pcb * tp;
	struct tcp_pcb * mux;
	int port = 1001;
	int n;

	DCC_LOG1(LOG_TRACE, "<%d>", thinkos_thread_self());

	for (;;) {

		mux = tcp_alloc();

		DCC_LOG1(LOG_TRACE, "mux=%p", mux);

		tcp_bind(mux, INADDR_ANY, htons(port));

		INF("COMM TCP: waiting for incomming connections... ");

		if (tcp_listen(mux, 1) != 0) {
			DCC_LOG(LOG_WARNING, "tcp_listen() failed!");
			break;
		}

		if ((tp = tcp_accept(mux)) == NULL) {
			DCC_LOG(LOG_ERROR, "tcp_accept().");
			break;
		}

		INF("COMM TCP: connection accepted. ");
		DCC_LOG(LOG_TRACE, "connection accepted...");
	
		DCC_LOG(LOG_TRACE, "closing TCP mux............");
		tcp_close(mux);

		ice_comm_open(comm);
		parm->tp = tp;

		thinkos_flag_give(parm->con_flag);

		for (;;) {
			if ((n = ice_comm_read(comm, buf, sizeof(buf), 250)) < 0) {
				DCC_LOG1(LOG_WARNING, "ice_comm_read(): %d", n); 
				break;
			}

			if (n == 0) {
				continue;
			}

			DCC_LOG1(LOG_INFO, "ice_comm_read(): %d", n); 

			if ((n = tcp_send(tp, buf, n, TCP_SEND_NOWAIT)) < 0) {
				DCC_LOG(LOG_WARNING, "tcp_send() failed!");
				break;
			}
		} 

		parm->tp = NULL;

		ice_comm_close(comm);

//		DCC_LOG2(LOG_TRACE, "%I:%d closed.", tp->t_faddr, ntohs(tp->t_fport));

		tcp_close(tp);
	}

	for (;;);
}

uint32_t comm_write_stack[512 + 128] __attribute__ ((aligned(64)));

#define COMM_RD_STACK_SIZE (512 * 4 + (COMM_TCP_BUF_SIZE)) 
uint32_t comm_read_stack[COMM_RD_STACK_SIZE / 4] __attribute__ ((aligned(64)));

struct comm_tcp comm_tcp;

const struct thinkos_thread_inf comm_read_inf = {
	.stack_ptr = comm_read_stack, 
	.stack_size = sizeof(comm_read_stack),
	.priority = 32,
	.thread_id = 32,
	.paused = false,
	.tag = "COMM_RD"
};

const struct thinkos_thread_inf comm_write_inf = {
	.stack_ptr = comm_write_stack, 
	.stack_size = sizeof(comm_write_stack),
	.priority = 32,
	.thread_id = 32,
	.paused = false,
	.tag = "COMM_WR"
};

int comm_tcp_start(ice_comm_t * comm)
{
	int th;

	comm_tcp.comm = comm;
	comm_tcp.tp = NULL;
	comm_tcp.con_flag = thinkos_flag_alloc();

	DCC_LOG1(LOG_TRACE, "flag=%d", comm_tcp.con_flag);

	th = thinkos_thread_create_inf((void *)comm_tcp_read_task, 
								   (void *)&comm_tcp, &comm_read_inf);

	INF("comm_tcp_start th=%d", th);
	DCC_LOG1(LOG_TRACE, "Comm read thread %d", th);

	th = thinkos_thread_create_inf((void *)comm_tcp_write_task, 
					   (void *)&comm_tcp, &comm_write_inf);
	(void)th;

	INF("comm_tcp_write_task th=%d", th);
	DCC_LOG1(LOG_TRACE, "Comm write thread=%d.", th);

	return 0;
}

