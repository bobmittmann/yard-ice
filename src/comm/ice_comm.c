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
 * @file ice_comm.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifdef CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <thinkos.h>

#include "ice_comm.h"
#include "dbglog.h"

int ice_comm_open(ice_comm_t * comm)
{
	DCC_LOG(LOG_TRACE, ".");

	if (comm->open == 0) {
		/* Reset the queue */
//		comm->tx_head = comm->tx_tail;
//		comm->rx_head = comm->rx_tail;
		thinkos_sem_init(comm->rx_sem, 0);
		DCC_LOG(LOG_TRACE, "semaphore initialized...");
		comm->open = 1;
	}

	return 0;
}

int ice_comm_close(ice_comm_t * comm)
{
	DCC_LOG(LOG_TRACE, ".");

	if (comm->open) {
		thinkos_sem_post(comm->rx_sem);
//		comm->tx_head = 0;
//		comm->tx_tail = 0;
//		comm->rx_head = 0;
//		comm->rx_tail = 0;
		comm->open = 0;
	}

	return 0;
}

/*
 * The DCC read function waits on the timer (TC2) interrupt that
 * is either a 500ms timeout or a block of data ready in the
 * dcc buffer (dcc_buf).
 * The poll routine in the FIQ polls the target, read data from
 * the DCC and store in the dcc buffer.
 */
int ice_comm_read(ice_comm_t * comm, void * buf, int len, int tmo)
{
	int tail;
	int head;
	int max;
	int ret;
	int n;
	int m;

	if (!comm->open)
		return -1;

	DCC_LOG1(LOG_MSG, "len=%d", len);

	tail = comm->rx.tail;

	if ((head = comm->rx.head) == tail) {
		DCC_LOG(LOG_MSG, "block");

		/* wait for data to became available in the buffer */
		ret = thinkos_sem_timedwait(comm->rx_sem, tmo);

		if (ret < 0) {
			DCC_LOG(LOG_TRACE, "timeout!!!");
		}

		if ((head = comm->rx.head) == tail) {
			return 0;
		}
	}
	
	DCC_LOG2(LOG_TRACE, "tail=%d head=%d", tail, head);

	/* round wrap the pointers */
	head &= ICE_COMM_RX_BUF_LEN - 1;
	tail &= ICE_COMM_RX_BUF_LEN - 1;

	/* we are copying 4 bytes at time, so the we can copy up to 
	   floor(length / 4) words */
	max = len / 4;

	if (head > tail) {
		n = head - tail;
		m = 0;
	} else {
		n = (ICE_COMM_RX_BUF_LEN - tail);
		m = head;
	}
	if (n > max) {
		n = max;
		m = 0;
	} else {
		if ((n + m) > max) {
			m = (max - n);
		}
	}

	DCC_LOG4(LOG_MSG, "tail=%d head=%d n=%d m=%d", tail, head, n, m);

	memcpy(buf, &comm->rx.buf[tail], n * 4);
	memcpy(((uint8_t *)buf) + n * 4, comm->rx.buf, m * 4);

	comm->rx.tail += n + m;

	return (n + m) * 4;
}

int ice_comm_write(ice_comm_t * comm, const void * buf, 
					   int len, int tmo)
{
	uint8_t * src = (uint8_t *)buf;
	uint32_t val;
	int head;
	int n;

	DCC_LOG1(LOG_MSG, "len=%d", len);

	if (!comm->open)
		return -1;

	head = comm->tx.head;

//	for (i = 0; i < (len / 4); i++) {
	for (n = 0; n < len; n++) {
		if ((head - comm->tx.tail) == ICE_COMM_TX_BUF_LEN) {
			DCC_LOG(LOG_MSG, "block");
			/* wait the end of operation */
			thinkos_sem_timedwait(comm->tx_sem, tmo);
		}

		if ((head - comm->tx.tail) == ICE_COMM_TX_BUF_LEN) {
			DCC_LOG(LOG_MSG, "timeout");
			return n;
		}

		/* TODO: endianess */
		/* TODO: byte write vs. word write */
//		val = src[i] | (src[i + 1] << 4) | 
//			(src[i + 2] << 8) | (src[i + 3] << 12);

		val = src[n];

		comm->tx.buf[head & (ICE_COMM_TX_BUF_LEN - 1)] = val;
		comm->tx.head = ++head;
	}

	return n;
}

int ice_comm_flush(ice_comm_t * comm)
{
	DCC_LOG(LOG_TRACE, ".");

	if (!comm->open)
		return -1;

	/* Reset the queues */
	comm->tx.head = comm->tx.tail;
	comm->rx.head = comm->rx.tail;

	return 0;
}

int ice_comm_init(ice_comm_t * comm)
{
	comm->open = 0;

	comm->rx.head = 0;
	comm->rx.tail = 0;
	comm->rx_sem = thinkos_sem_alloc(0);

	comm->tx.head = 0;
	comm->tx.tail = 0;
	comm->tx_sem = thinkos_sem_alloc(ICE_COMM_TX_BUF_LEN);

	DCC_LOG2(LOG_TRACE, "[INIT] rx_sem=%d tx_sem=%d",
			 comm->rx_sem, comm->tx_sem);

	return 0;
}

int ice_comm_done(ice_comm_t * comm)
{
	thinkos_sem_free(comm->rx_sem);
	comm->rx_sem = -1;

	thinkos_sem_free(comm->tx_sem);
	comm->tx_sem = -1;

	DCC_LOG(LOG_TRACE, "[DONE]");

	return 0;
}


