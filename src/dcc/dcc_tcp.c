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
 * @file dcc_tcp.c
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
#include <uthreads.h>

#include "jtag_arm.h"
#include "armice.h"
#include "arm_pn.h"
#include "dbglog.h"

#define DCC_TCP_BUF_SIZE 512

struct dcc_tcp_parm {
	struct tcp_pcb * tp;
	jtag_arm_dcc_t * arm_dcc;
};

int dcc_tcp_write_task(struct dcc_tcp_parm * parm, int id)
{
	struct tcp_pcb * tp = parm->tp;
	jtag_arm_dcc_t * arm_dcc = parm->arm_dcc;
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

		DCC_LOG3(LOG_TRACE, "%I:%d recv: %d", tp->t_faddr, 
				 ntohs(tp->t_fport), len);

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
		while (len) {
			if ((n = jtag_arm_dcc_write(arm_dcc, ptr, len, 500)) < 0) {
				DCC_LOG1(LOG_WARNING, "jtag_arm_dcc_write(): %d", n); 
				break;
			}
			ptr += n;
			len -= n;
		}
	} 

	jtag_arm_dcc_close(arm_dcc);

	return 0;
}

uint32_t dcc_write_stack[512 + 128];

int __attribute__((noreturn)) dcc_tcp_read_task(armice_ctrl_t * ctrl, int id)
{
	struct dcc_tcp_parm tcp_parm;
	jtag_arm_dcc_t * arm_dcc = &ctrl->arm_dcc;
	uint32_t buf[DCC_TCP_BUF_SIZE / sizeof(uint32_t)];
	struct tcp_pcb * tp;
	struct tcp_pcb * mux;
	int th;
	int n;
	int port = 1001;

	for (;;) {
		mux = tcp_alloc();

		DCC_LOG1(LOG_TRACE, "mux=%p", mux);

		tcp_bind(mux, INADDR_ANY, htons(port));

		if (tcp_listen(mux, 1) != 0) {
			DCC_LOG1(LOG_WARNING, "<%d> tcp_listen()", id);
			break;
		}

		if ((tp = tcp_accept(mux)) == NULL) {
			DCC_LOG(LOG_ERROR, "tcp_accept().");
			break;
		}

		tcp_close(mux);

		DCC_LOG2(LOG_TRACE, "%I:%d accepted.", 
				 tp->t_faddr, ntohs(tp->t_fport));
	
		jtag_arm_dcc_open(arm_dcc);

		tcp_parm.tp = tp;
		tcp_parm.arm_dcc = arm_dcc;

		th = uthread_create(dcc_write_stack, sizeof(dcc_write_stack), 
							(uthread_task_t)dcc_tcp_write_task, 
							(void *)&tcp_parm, 0, NULL); 
		if (th < 0) {
			DCC_LOG(LOG_ERROR, "uthread_create() fail!"); 
			jtag_arm_dcc_close(arm_dcc);
			tcp_close(tp);
			DCC_LOG2(LOG_TRACE, "%I:%d closed.", 
					 tp->t_faddr, ntohs(tp->t_fport));
			break;
		}

		for (;;) {
			if ((n = jtag_arm_dcc_read(arm_dcc, buf, sizeof(buf), 500)) < 0) {
				DCC_LOG1(LOG_WARNING, "jtag_arm_dcc_read(): %d", n); 
				break;
			}

			DCC_LOG1(LOG_MSG, "jtag_arm_dcc_read(): %d", n); 

			if (n == 0) {
				continue;
			}

			if ((n = tcp_send(tp, buf, n, 0)) < 0) {
				DCC_LOG2(LOG_TRACE, "%I:%d error.", 
						 tp->t_faddr, ntohs(tp->t_fport));
				break;
			}
		} 

		jtag_arm_dcc_close(arm_dcc);

		DCC_LOG2(LOG_TRACE, "%I:%d closed.", tp->t_faddr, ntohs(tp->t_fport));

		tcp_close(tp);

		uthread_join(th);
	}

	for(;;);
}

uint32_t dcc_read_stack[1024];

int dcc_tcp_start(armice_ctrl_t * ctrl)
{
	int th;

	th = uthread_create(dcc_read_stack, sizeof(dcc_read_stack), 
						(uthread_task_t)dcc_tcp_read_task, 
						(void *)ctrl, 0, NULL); 

	printf("<%d> ", th);

	return 0;
}

