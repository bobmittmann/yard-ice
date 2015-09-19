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

_Pragma ("GCC optimize (\"Ofast\")")

#define __THINKOS_SYS__
#include <thinkos_sys.h>
#define __THINKOS_DMON__
#include <thinkos_dmon.h>
#include <thinkos.h>

#include <sys/param.h>

#if THINKOS_ENABLE_CONSOLE

#if (!THINKOS_ENABLE_MONITOR)
#error "Need THINKOS_ENABLE_MONITOR!"
#endif

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

void __comm_on_recv(unsigned int cnt) 
{
	int wq = THINKOS_WQ_CONSOLE_RD;
	unsigned int max;
	uint8_t * buf;
	int th;
	int n;

	thinkos_console_rt.rx_pipe.head += cnt;

	DCC_LOG2(LOG_INFO, "wq=%d -> 0x%08x", wq, thinkos_rt.wq_lst[wq]);

	if ((th = __thinkos_wq_head(wq)) == THINKOS_THREAD_NULL) {
		DCC_LOG(LOG_INFO, "no thread waiting.");
		return;
	}

	DCC_LOG1(LOG_INFO, "thread_id=%d", th);

	buf = (uint8_t *)thinkos_rt.ctx[th]->r1;
	max = thinkos_rt.ctx[th]->r2;
	/* read from the RX pipe into the thread's read buffer */
	if ((n = pipe_read(&thinkos_console_rt.rx_pipe, buf, max)) == 0) {
		DCC_LOG(LOG_INFO, "_pipe_read() == 0");
		return;
	}

	/* wakeup from the console wait queue */
	__thinkos_wakeup_return(wq, th, n);
	/* signal the scheduler ... */
	__thinkos_defer_sched();
}

void __console_tx_pipe_commit(unsigned int cnt) 
{
	int wq = THINKOS_WQ_CONSOLE_WR;
	int th;

	thinkos_console_rt.tx_pipe.tail += cnt;
	if ((th = __thinkos_wq_head(wq)) != THINKOS_THREAD_NULL) {
		/* XXX: To avoid a race condition when writing to the 
		   pipe from the service call and this function (invoked
		   from an interrupt handler), let the thread to
		   wakes up and retry. */
		/* wakeup from the console wait queue */
		__thinkos_wakeup(wq, th);
		/* signal the scheduler ... */
		__thinkos_defer_sched();
	}
}


void thinkos_comm_svc(int32_t * arg)
{
	int self = thinkos_rt.active;
	unsigned int req = arg[0];
	unsigned int wq;
	unsigned int len;
	uint32_t queue;
	uint8_t * buf;
	int n;
	
	switch (req) {
	case COMM_RECV:
		buf = (uint8_t *)arg[1];
		len = arg[2];
		DCC_LOG1(LOG_INFO, "Console read: len=%d", len);
		if ((n = pipe_read(&thinkos_console_rt.rx_pipe, buf, len)) > 0) {
			dmon_signal(DMON_RX_PIPE);
			arg[0] = n;
			break;
		}
		DCC_LOG(LOG_INFO, "Console read wait...");
		/* wait for event */
		__thinkos_suspend(self);
		/* insert into the mutex wait queue */
		__thinkos_wq_insert(THINKOS_WQ_CONSOLE_RD, self);
		/* signal the scheduler ... */
		__thinkos_defer_sched(); 
		break;

	case COMM_SEND:
		buf = (uint8_t *)arg[1];
		len = arg[2];
		wq = THINKOS_WQ_CONSOLE_WR;
		DCC_LOG1(LOG_INFO, "Console write: len=%d", len);
wr_again:
		if ((n = pipe_write(&thinkos_console_rt.tx_pipe, buf, len)) > 0) {
			DCC_LOG1(LOG_INFO, "pipe_write: n=%d", n);
			dmon_signal(DMON_TX_PIPE);
			arg[0] = n;
		} else {
			/* Set the return value to ZERO. The calling thread 
			   shuould retry sending data. */
			arg[0] = 0;
			/* (1) suspend the thread by removing it from the
			   ready wait queue. The __thinkos_suspend() call cannot be nested
			   inside a LDREX/STREX pair as it may use the exclusive access 
			   itself, in case we have anabled the time sharing option. */
			__thinkos_suspend(self);
			/* update the thread status in preparation for event wait */
#if THINKOS_ENABLE_THREAD_STAT
			thinkos_rt.th_stat[self] = wq << 1;
#endif
			/* (2) Save the context pointer. In case an interrupt wakes up
			   this thread before the scheduler is called, this will allow
			   the interrupt handler to locate the return value (r0) address. */
			/* FIXME: the hard coded (-8) value is dependent on the 
			   servicehandler call stack in this case of Cortex-M.
			   This may be complettly wrong if the float point unit is enabled.
			   A macro shuld be definde to deal with this. 
			 */ 
			thinkos_rt.ctx[self] = (struct thinkos_context *)&arg[-8];
			/* Insert into the event wait queue */
			queue = __ldrex(&thinkos_rt.wq_lst[wq]);
			queue |= (1 << self);
			/* (3) Try to save the queu state back.
			   The queue may have changed by an interrup handler.
			   If this is the case roll back and restart. */
			if (__strex(&thinkos_rt.wq_lst[wq], queue)) {
				/* roll back */
#if THINKOS_ENABLE_THREAD_STAT
				thinkos_rt.th_stat[self] = 0;
#endif
				/* insert into the ready wait queue */
				__bit_mem_wr(&thinkos_rt.wq_ready, self, 1);  
				DCC_LOG2(LOG_WARNING, "<%d> rollback 2 %d...", self, wq);
				goto wr_again;
			}

			/* -- wait for event ---------------------------------------- */
			DCC_LOG2(LOG_INFO, "<%d> waiting on console write %d...", self, wq);
			/* signal the scheduler ... */
			__thinkos_defer_sched(); 
		}
		break;

	default:
		DCC_LOG1(LOG_ERROR, "invalid console request %d!", req);
		arg[0] = THINKOS_EINVAL;
		break;
	}
}

void __comm_reset(void)
{
}

void thinkos_comm_init(void)
{
	__comm_reset();
}

#endif /* THINKOS_ENABLE_COMM */

