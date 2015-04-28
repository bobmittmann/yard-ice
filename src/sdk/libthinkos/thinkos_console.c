/* 
 * thikos_util.c
 *
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the ThinkOS library.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without flagen the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */

#define __THINKOS_SYS__
#include <thinkos_sys.h>
#define __THINKOS_DMON__
#include <thinkos_dmon.h>
#include <thinkos.h>

#include <sys/param.h>

#define CONSOLE_PIPE_LEN 64

struct console_pipe {
	uint32_t head;
	uint32_t tail;
	uint8_t buf[CONSOLE_PIPE_LEN];
};

struct {
	struct console_pipe tx_pipe;
	struct console_pipe rx_pipe;
} thinkos_console_rt;

static int pipe_get(struct console_pipe * pipe, 
					   uint8_t * buf, unsigned int len)
{
	uint32_t tail;
	int max;
	int cnt;
	int pos;

	/* pipe->tail is declared as volatile, 
	   for performance reasons we read it only once at 
	   the beginning and write it back at the end. */
	tail = pipe->tail;
	/* get the maximum number of chars we can read from the buffer */
	if ((max = pipe->head - tail) == 0)
		return 0;
	/* cnt is the number of chars we will read from the buffer,
	   it should be the the minimum of max and len */
	cnt = MIN(max, len);
	/* get the tail position in the buffer */
	pos = (tail % CONSOLE_PIPE_LEN);
	/* check whether to wrap arround or on not */
	if ((pos + cnt) > CONSOLE_PIPE_LEN) {
		/* we need to perform two reads */
		int n;
		int m;
		/* get the number of chars from tail pos until the end of buffer */
		n = CONSOLE_PIPE_LEN - pos;
		/* the remaining chars are at the beginning of the buffer */
		m = cnt - n;
		__thinkos_memcpy(buf, &pipe->buf[pos], n);
		__thinkos_memcpy(buf + n, &pipe->buf[0], m);
	} else {
		__thinkos_memcpy(buf, &pipe->buf[pos], cnt);
	}
	pipe->tail = tail += cnt;
	return cnt;
}

static int pipe_put(struct console_pipe * pipe, 
					   const uint8_t * buf, unsigned int len)
{
	uint8_t * cp = (uint8_t *)buf;
	uint32_t head;
	int max;
	int cnt;
	int pos;

	/* pipe->head is declared as volatile, 
	   for performance reasons we read it only once at 
	   the beginning and write it back at the end. */
	head = pipe->head;
	/* get the maximum number of chars we can write into buffer */
	if ((max = pipe->tail + CONSOLE_PIPE_LEN - head) == 0)
		return 0;
	/* cnt is the number of chars we will write to the buffer,
	   it should be the the minimum of max and len */
	cnt = MIN(max, len);
	/* get the tail position in the buffer */
	pos = (head % CONSOLE_PIPE_LEN);
	DCC_LOG4(LOG_INFO, "head=%d tail=%d cnt=%d pos=%d", 
			 head, pipe->tail, cnt, pos);
	/* check whether to wrap arround or on not */
	if ((pos + cnt) > CONSOLE_PIPE_LEN) {
		/* we need to perform two reads */
		int n;
		int m;
		/* get the number of chars from tail pos until the end of buffer */
		n = CONSOLE_PIPE_LEN - pos;
		/* the remaining chars are at the beginning of the buffer */
		m = cnt - n;
		__thinkos_memcpy(&pipe->buf[pos], cp, n);
		__thinkos_memcpy(&pipe->buf[0], cp + n, m);
	} else {
		__thinkos_memcpy(&pipe->buf[pos], cp, cnt);
	}

	pipe->head = head += cnt;

	return cnt;
}

int __console_rx_pipe_ptr(uint8_t ** ptr) 
{
	struct console_pipe * pipe = &thinkos_console_rt.rx_pipe;
	uint32_t head;
	int cnt;
	int pos;

	/* get the head position in the buffer */
	head = pipe->head;
	pos = (head % CONSOLE_PIPE_LEN);
	/* get the free space */
	cnt = pipe->tail + CONSOLE_PIPE_LEN - head;
	/* check whether to wrap arround or on not */
	if ((pos + cnt) > CONSOLE_PIPE_LEN) {
		/* get the number of chars from head pos until the end of buffer */
		cnt = CONSOLE_PIPE_LEN - pos;
	}
	*ptr = &pipe->buf[pos];

	return cnt;
}

void __console_rx_pipe_commit(unsigned int cnt) 
{
	int wq = THINKOS_WQ_CONSOLE_RD;
	unsigned int max;
	uint8_t * buf;
	int th;
	int n;

	thinkos_console_rt.rx_pipe.head += cnt;
//	dmon_signal(DMON_RX_PIPE);
//			DCC_LOG(LOG_INFO, "RX Pipe.");

	if ((th = __thinkos_wq_head(wq)) == THINKOS_THREAD_NULL) {
		return;
	}

	DCC_LOG1(LOG_INFO, "thread_id=%d", th);

	buf = (uint8_t *)thinkos_rt.ctx[th]->r1;
	max = thinkos_rt.ctx[th]->r2;
	/* read from the RX pipe into the thread's read buffer */
	if ((n = pipe_get(&thinkos_console_rt.rx_pipe, buf, max)) == 0) {
		DCC_LOG(LOG_INFO, "_pipe_get() == 0");
		return;
	}

	/* wakeup from the console wait queue */
	__thinkos_wakeup_return(wq, th, n);
	/* signal the scheduler ... */
	__thinkos_defer_sched();
}

int __console_tx_pipe_ptr(uint8_t ** ptr) 
{
	struct console_pipe * pipe = &thinkos_console_rt.tx_pipe;
	uint32_t tail;
	int cnt;
	int pos;

	/* get the tail position in the buffer */
	tail = pipe->tail;
	pos = (tail % CONSOLE_PIPE_LEN);
	/* get the used space */
	cnt = pipe->head - tail;
	DCC_LOG4(LOG_INFO, "head=%d tail=%d cnt=%d pos=%d", 
			 pipe->head, tail, cnt, pos);
	/* check whether to wrap arround or on not */
	if ((pos + cnt) > CONSOLE_PIPE_LEN) {
		/* get the number of chars from tail pos until the end of buffer */
		cnt = CONSOLE_PIPE_LEN - pos;
	}
	*ptr = &pipe->buf[pos];

	return cnt;
}


void __console_tx_pipe_commit(unsigned int cnt) 
{
	int wq = THINKOS_WQ_CONSOLE_WR;
	unsigned int len;
	uint8_t * buf;
	int th;
	int n;

	thinkos_console_rt.tx_pipe.tail += cnt;
	if ((th = __thinkos_wq_head(wq)) != THINKOS_THREAD_NULL) {
		buf = (uint8_t *)thinkos_rt.ctx[th]->r1;
		len = thinkos_rt.ctx[th]->r2;
		if ((n = pipe_put(&thinkos_console_rt.tx_pipe, 
									buf, len)) > 0) {
			DCC_LOG1(LOG_INFO, "TX Pipe wakeup %d !!!", th);
			/* wakeup from the console wait queue */
			__thinkos_wakeup_return(wq, th, n);
			/* signal the scheduler ... */
			__thinkos_defer_sched();
		}
	}
}


void thinkos_console_svc(int32_t * arg)
{
	int self = thinkos_rt.active;
	unsigned int req = arg[0];
	unsigned int len;
	uint8_t * buf;
	int n;
	
	switch (req) {
	case CONSOLE_OPEN:
		arg[0] = 0;
		break;

	case CONSOLE_TIMEDREAD:
		buf = (uint8_t *)arg[1];
		len = arg[2];
		DCC_LOG1(LOG_INFO, "Console timed read: len=%d", len);
		if ((n = pipe_get(&thinkos_console_rt.rx_pipe,
									buf, len)) > 0) {
			dmon_signal(DMON_RX_PIPE);
			arg[0] = n;
			break;
		}
		DCC_LOG(LOG_INFO, "Console timed read wait...");
		/* insert into the mutex wait queue */
		__thinkos_tmdwq_insert(THINKOS_WQ_CONSOLE_RD, self, arg[3]);
		/* Set the default return value to timeout. */
		arg[0] = THINKOS_ETIMEDOUT;
		/* wait for event */
		__thinkos_wait(self);
		break;

	case CONSOLE_READ:
		buf = (uint8_t *)arg[1];
		len = arg[2];
		DCC_LOG1(LOG_INFO, "Console read: len=%d", len);
		if ((n = pipe_get(&thinkos_console_rt.rx_pipe,
									buf, len)) > 0) {
			dmon_signal(DMON_RX_PIPE);
			arg[0] = n;
			break;
		}
		DCC_LOG(LOG_INFO, "Console read wait...");
		/* insert into the mutex wait queue */
		__thinkos_wq_insert(THINKOS_WQ_CONSOLE_RD, self);
		/* wait for event */
		__thinkos_wait(self);
		break;

	case CONSOLE_WRITE:
		buf = (uint8_t *)arg[1];
		len = arg[2];
		DCC_LOG1(LOG_INFO, "Console write: len=%d", len);
		if ((n = pipe_put(&thinkos_console_rt.tx_pipe, 
									buf, len)) > 0) {
			DCC_LOG1(LOG_INFO, "pipe_put: n=%d", n);
			dmon_signal(DMON_TX_PIPE);
			arg[0] = n;
		} else {
			DCC_LOG(LOG_INFO, "Console write wait...");
			/* insert into the mutex wait queue */
			__thinkos_wq_insert(THINKOS_WQ_CONSOLE_WR, self);
			/* wait for event */
			__thinkos_wait(self);
		}
		break;

	case CONSOLE_CLOSE:
		arg[0] = 0;
		break;

	case CONSOLE_IOCTL:
		arg[0] = 0;
		break;

	default:
		DCC_LOG1(LOG_ERROR, "invalid console request %d!", req);
		arg[0] = THINKOS_EINVAL;
		break;
	}
}

void __attribute__((noreturn)) console_task(struct dmon_comm * comm)
{
	uint32_t sigmask;
	uint32_t sigset;
	uint8_t * buf;
	int cnt;
	int len;

	DCC_LOG(LOG_INFO, "Monitor start...");

	dmon_comm_rxflowctrl(comm, true);
	dmon_comm_connect(comm);

	DCC_LOG(LOG_INFO, "COMMM connected...");


	sigmask = (1 << DMON_THREAD_FAULT);
	sigmask |= (1 << DMON_COMM_RCV);
	sigmask |= (1 << DMON_COMM_CTL);
	sigmask |= (1 << DMON_TX_PIPE);
	sigmask |= (1 << DMON_RX_PIPE);
	for(;;) {
		
		sigset = dmon_select(sigmask);
		DCC_LOG1(LOG_INFO, "sigset=%08x.", sigset);

		if (sigset & (1 << DMON_THREAD_FAULT)) {
			DCC_LOG(LOG_INFO, "Thread fault.");
			dmon_clear(DMON_THREAD_FAULT);
		}

		if (sigset & (1 << DMON_COMM_CTL)) {
			DCC_LOG(LOG_INFO, "Comm Ctl.");
			dmon_clear(DMON_COMM_CTL);
			if (!dmon_comm_isconnected(comm))	
				dmon_reset();
		}

		if (sigset & (1 << DMON_COMM_RCV)) {
			if ((cnt = __console_rx_pipe_ptr(&buf)) > 0) {
				DCC_LOG1(LOG_INFO, "Comm recv. rx_pipe.free=%d", cnt);
				if ((len = dmon_comm_recv(comm, buf, cnt)) > 0)
					__console_rx_pipe_commit(len); 
			} else {
				DCC_LOG(LOG_INFO, "Comm recv. Masking DMON_COMM_RCV!");
				sigmask &= ~(1 << DMON_COMM_RCV);
			}
		}

		if (sigset & (1 << DMON_RX_PIPE)) {
			if ((cnt = __console_rx_pipe_ptr(&buf)) > 0) {
				DCC_LOG1(LOG_INFO, "RX Pipe. rx_pipe.free=%d. "
						 "Unmaksing DMON_COMM_RCV!", cnt);
				sigmask |= (1 << DMON_COMM_RCV);
			} else {
				DCC_LOG(LOG_INFO, "RX Pipe empty!!!");
			}
			dmon_clear(DMON_RX_PIPE);
		}

		if (sigset & (1 << DMON_TX_PIPE)) {
			DCC_LOG(LOG_INFO, "TX Pipe.");
			if ((cnt = __console_tx_pipe_ptr(&buf)) > 0) {
				len = dmon_comm_send(comm, buf, cnt);
				__console_tx_pipe_commit(len); 
			} else {
				DCC_LOG(LOG_INFO, "TX Pipe empty!!!");
				dmon_clear(DMON_TX_PIPE);
			}
		}
	}
}

void thinkos_console_init(void)
{
	thinkos_console_rt.tx_pipe.head = 0;
	thinkos_console_rt.tx_pipe.tail = 0;
	thinkos_console_rt.rx_pipe.head = 0;
	thinkos_console_rt.rx_pipe.tail = 0;
}


