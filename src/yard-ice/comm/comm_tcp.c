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
#include <sys/os.h>
#include <trace.h>

#include "jtag_arm.h"
#include "armice.h"
#include "arm_pn.h"
#include "dbglog.h"
#include "ice_comm.h"

struct comm_tcp_parm {
	struct tcp_pcb * tp;
	ice_comm_t * comm;
};

int comm_tcp_write_task(struct comm_tcp_parm * parm, int id)
{
	struct tcp_pcb * tp = parm->tp;
	ice_comm_t * comm = parm->comm;
	uint8_t net_buf[128];
	uint8_t * ptr;
	int len;
	int rem;
	int n;

	for (;;) {
		/* receive data form network */
		if ((len = tcp_recv(tp, net_buf, 128)) <= 0) {
			DCC_LOG1(LOG_WARNING, "tcp_recv(): %d", len);
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


	DCC_LOG(LOG_TRACE, "done..."); 


	return 0;
}

uint32_t comm_write_stack[512 + 128];

#define COMM_TCP_BUF_SIZE 512

int comm_tcp_read_task(ice_comm_t * comm)
{
	struct comm_tcp_parm tcp_parm;
	uint32_t buf[COMM_TCP_BUF_SIZE / sizeof(uint32_t)];
	struct tcp_pcb * tp;
	struct tcp_pcb * mux;
	int th;
	int n;
	int port = 1001;

	__os_sleep(100);
	DCC_LOG1(LOG_TRACE, "<%d>", __os_thread_self());
	__os_sleep(100);
	DCC_LOG1(LOG_TRACE, "<%d>", __os_thread_self());

	for (;;) {

		mux = tcp_alloc();

		DCC_LOG1(LOG_TRACE, "mux=%p", mux);

		tcp_bind(mux, INADDR_ANY, htons(port));

		if (tcp_listen(mux, 1) != 0) {
			DCC_LOG(LOG_WARNING, "tcp_listen() failed!");
			break;
		}

		if ((tp = tcp_accept(mux)) == NULL) {
			DCC_LOG(LOG_ERROR, "tcp_accept().");
			break;
		}

		tcp_close(mux);

//		DCC_LOG2(LOG_TRACE, "%I:%d accepted.", 
//				 tp->t_faddr, ntohs(tp->t_fport));
	
		ice_comm_open(comm);

		tcp_parm.tp = tp;
		tcp_parm.comm = comm;

		th = __os_thread_create((void *)comm_tcp_write_task, (void *)&tcp_parm, 
								comm_write_stack, sizeof(comm_write_stack), 
								__OS_PRIORITY_LOWEST);

		if (th < 0) {
			DCC_LOG(LOG_ERROR, "__os_thread_create() fail!"); 
			ice_comm_close(comm);
			tcp_close(tp);
//			DCC_LOG2(LOG_TRACE, "%I:%d closed.", 
//					 tp->t_faddr, ntohs(tp->t_fport));
			break;
		}

		for (;;) {
			if ((n = ice_comm_read(comm, buf, sizeof(buf), 500)) < 0) {
				DCC_LOG1(LOG_WARNING, "ice_comm_read(): %d", n); 
				break;
			}

			DCC_LOG1(LOG_MSG, "ice_comm_read(): %d", n); 

			if (n == 0) {
				continue;
			}

			if ((n = tcp_send(tp, buf, n, 0)) < 0) {
//				DCC_LOG2(LOG_TRACE, "%I:%d error.", 
//						 tp->t_faddr, ntohs(tp->t_fport));
				break;
			}
		} 

		ice_comm_close(comm);

//		DCC_LOG2(LOG_TRACE, "%I:%d closed.", tp->t_faddr, ntohs(tp->t_fport));

		tcp_close(tp);

		__os_thread_join(th);
	}

	for(;;);
}

uint32_t comm_read_stack[512 + (COMM_TCP_BUF_SIZE / 4)];

int comm_tcp_start(ice_comm_t * comm)
{
	int th;

	th = __os_thread_create((void *)comm_tcp_read_task, (void *)comm, 
							comm_read_stack, sizeof(comm_read_stack), 
							__OS_PRIORITY_LOWEST);

	tracef("comm_tcp_start th=%d", th);

	return 0;
}

