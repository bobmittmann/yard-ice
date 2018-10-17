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
 * @file cmd_ymodem_rcv.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/clock.h>
#include <sys/tty.h>
#include <xmodem.h>

#include "config.h"
#include "target.h"
#include "debugger.h"
#include "eval.h"

#include <trace.h>
#include <thinkos.h>

#include <sys/dcclog.h>

#if ENABLE_YMODEM

#define __THINKOS_DBGMON__
#include <thinkos/dbgmon.h>

#define YMODEM_RCV_BUF_LEN 1024

void __attribute__((noreturn)) mon_task(const struct dbgmon_comm * comm,
											void * arg)
{
	bool connected;
	uint32_t sigmask = 0;
	uint8_t * ptr;
	int free;
	int cnt;
	int sig;
	int n;
	
	sigmask = ((1 << DBGMON_COMM_EOT) | (1 << DBGMON_COMM_RCV) |
			   (1 << DBGMON_TX_PIPE) | (1 << DBGMON_RX_PIPE));

	for(;;) {
		switch ((sig = dbgmon_select(sigmask))) {

		case DBGMON_STARTUP:
			dbgmon_clear(DBGMON_STARTUP);
			__thinkos_console_reset();
			/* Update the console connection flag which was cleared
			 by __console_reset(). */
			goto is_connected;

		case DBGMON_COMM_CTL:
			dbgmon_clear(DBGMON_COMM_CTL);
is_connected:
			connected = dbgmon_comm_isconnected(comm);
			__console_connect_set(connected);
			if (connected) {
				sigmask |= ((1 << DBGMON_COMM_EOT) | (1 << DBGMON_COMM_RCV) |
							(1 << DBGMON_TX_PIPE) | (1 << DBGMON_RX_PIPE));
			} else {
				sigmask &= ~((1 << DBGMON_COMM_EOT) | (1 << DBGMON_COMM_RCV) |
							 (1 << DBGMON_TX_PIPE) | (1 << DBGMON_RX_PIPE));
			}
			break;

		case DBGMON_COMM_RCV:
		case DBGMON_RX_PIPE:
			/* get a pointer to the head of the pipe.
			   __console_rx_pipe_ptr() will return the number of 
			   consecutive spaces in the buffer. */
			if ((free = __console_rx_pipe_ptr(&ptr)) > 0) {
				/* receive from the COMM driver */
				if ((n = dbgmon_comm_recv(comm, ptr, free)) > 0) {
					/* commit the fifo head */
					__console_rx_pipe_commit(n);
					if (n == free) {
						/* Wait for RX_PIPE */
						sigmask &= ~(1 << DBGMON_COMM_RCV);
						sigmask |= (1 << DBGMON_RX_PIPE);
					} else {
						/* Wait for RX_PIPE and/or COMM_RCV */
						sigmask |= (1 << DBGMON_RX_PIPE);
						sigmask |= (1 << DBGMON_COMM_RCV);
					}
				} else {
					/* Wait for COMM_RECV */
					sigmask |= (1 << DBGMON_COMM_RCV);
					sigmask &=  ~(1 << DBGMON_RX_PIPE);
				}
			} else {
				/* Wait for RX_PIPE */
				sigmask &= ~(1 << DBGMON_COMM_RCV);
				sigmask |= (1 << DBGMON_RX_PIPE);
			}
			break;

		case DBGMON_COMM_EOT:
		case DBGMON_TX_PIPE:
			if ((cnt = __console_tx_pipe_ptr(&ptr)) > 0) {
				if ((n = dbgmon_comm_send(comm, ptr, cnt)) > 0) {
					__console_tx_pipe_commit(n); 
					if (n == cnt) {
						/* Wait for TX_PIPE */
						sigmask |= (1 << DBGMON_TX_PIPE);
						sigmask &= ~(1 << DBGMON_COMM_EOT);
					} else {
						/* Wait for COMM_EOT */
						sigmask |= (1 << DBGMON_COMM_EOT);
						sigmask &= ~(1 << DBGMON_TX_PIPE);
					}
				} else {
					/* Wait for COMM_EOT */
					sigmask |= (1 << DBGMON_COMM_EOT);
					sigmask &=  ~(1 << DBGMON_TX_PIPE);
				}
			} else {
				/* Wait for TX_PIPE */
				sigmask |= (1 << DBGMON_TX_PIPE);
				sigmask &= ~(1 << DBGMON_COMM_EOT);
			}
			break;
		}
	}
}

int cmd_ymodem_recv(FILE * f, int argc, char ** argv)
{
	uint8_t buf[YMODEM_RCV_BUF_LEN];
	struct comm_dev comm;
	struct debugger * dbg = &debugger;
	struct xmodem_rcv rx;
	struct file * raw;
	value_t val;
	uint32_t addr;
	uint32_t size;
	uint32_t start;
	unsigned int cnt;
	unsigned int rem;
	unsigned int offs;
	int ret;

	int n;

	argc--;
	argv++;

	if (argc) {
		if ((n = eval_uint32(&val, argc, argv)) < 0) {
			return n;
		}
		addr = val.uint32;
		argc -= n;
		argv += n;
	} else {
		addr = (uint32_t)dbg->transf.base;
	}

	if (argc) {
		if ((n = eval_uint32(&val, argc, argv)) < 0) {
			return n;
		}
		size = val.uint32;
		argc -= n;
		argv += n;
	} else {
		size = dbg->transf.size;
	}

	if (argc) {
		fprintf(f, "Too many arguments...\n");
		return -1;
	}

	fprintf(f, " - Write address: 0x%08x\n", addr);
	fprintf(f, " - Maxmum size: %d\n", size);

	raw = ftty_lowlevel(f);

	comm.arg = raw->data;
	comm.op.send = (int (*)(void *, const void *, unsigned int))raw->op->write;
	comm.op.recv = (int (*)(void *, void *, 
						  unsigned int, unsigned int))raw->op->read;

	DBGS(".................................");

	xmodem_rcv_init(&rx, &comm, XMODEM_RCV_CRC);

	cnt = 0;
	offs = 0;
	rem = size;
	start = thinkos_clock();
	do {
		if ((ret = xmodem_rcv_loop(&rx, buf, YMODEM_RCV_BUF_LEN)) < 0) {
			ERR("ret=%d", ret);
			break;
		}

		n = ret;
		if ((n = target_mem_write(addr, buf, n)) <= 0) {
			fprintf(f, "!");
		} else {
			fprintf(f, ".");
			size += n;
			addr += n;
		}
		addr += n;
		cnt += n;
		offs += n;
		rem -= n;
	} while (rem > 0);

	if (size) {
		uint32_t ms = (int32_t)(thinkos_clock() - start);
		fprintf(f, "\n - EOT: size=%d tm=%d[ms] speed=%d[bytes/sec]\n", 
				size, ms, (size * 1000) / ms);
	}

	dbg->transf.size = size;

	return 0;
}

#endif /* ENABLE_TCP_RCV */

