/* 
 * Copyright(C) 2015 Robinson Mittmann. All Rights Reserved.
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
 * @file simlnk.c
 * @brief MS/TP Link Layer
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */

//#include "simlnk-i.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/param.h>
#include <sys/serial.h>
#include <assert.h>

#include <thinkos.h>
#include "simlnk.h"
#include "simrpc.h"
#include "simrpc_svc.h"
#include "io.h"

#undef DEBUG
#undef TRACE_LEVEL
#define TRACE_LEVEL TRACE_LVL_DBG
#include <trace.h>

#ifndef SIMLNK_POOL_SIZE
#define SIMLNK_POOL_SIZE 5
#endif

#ifndef SIMLINK_STDOUT_BUF_SIZE
#define SIMLINK_STDOUT_BUF_SIZE 8192
#endif

#define PIPE_LEN SIMLINK_STDOUT_BUF_SIZE

struct pipe {
    uint32_t head;
    uint32_t tail;
    uint8_t buf[SIMLINK_STDOUT_BUF_SIZE];
};

#define SIMLINK_XFER_MAX (SIMLNK_MTU + 2)

struct simlnk {
	bool pool_alloc; /* indicate if the structure was 
						allocated from a resource pool */
	uint8_t addr; /* this station address */
	uint8_t mutex;
	struct serial_dev * dev;
	struct {
		uint32_t buf[(SIMLINK_XFER_MAX + 3) / 4];
		volatile unsigned int cnt;
		int flag;
	} rx;
	struct {
		uint32_t buf[(SIMLNK_MTU + 3) / 4];
		int flag;
		uint32_t seq;
	} tx;
	uint8_t stdout_seq;
	struct pipe stdout_pipe;
	struct simlnk_stat stat;
};

static int pipe_write(struct pipe * pipe, const uint8_t * buf, unsigned int len)
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
    if ((max = pipe->tail + PIPE_LEN - head) == 0)
        return 0;
    /* cnt is the number of chars we will write to the buffer,
       it should be the the minimum of max and len */
    cnt = MIN(max, len);
    /* get the tail position in the buffer */
    pos = (head % PIPE_LEN);
    /* check whether to wrap around or on not */
    if ((pos + cnt) > PIPE_LEN) {
        /* we need to perform two reads */
        int n;
        int m;
        /* get the number of chars from tail pos until the end of buffer */
        n = PIPE_LEN - pos;
        /* the remaining chars are at the beginning of the buffer */
        m = cnt - n;
        memcpy(&pipe->buf[pos], cp, n);
        memcpy(&pipe->buf[0], cp + n, m);
    } else {
        memcpy(&pipe->buf[pos], cp, cnt);
    }

    pipe->head = head + cnt;

    return cnt;
}

static int pipe_read(struct pipe * pipe, uint8_t * buf, unsigned int len)
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
    pos = (tail % PIPE_LEN);
    /* check whether to wrap around or on not */
    if ((pos + cnt) > PIPE_LEN) {
        /* we need to perform two reads */
        int n;
        int m;
        /* get the number of chars from tail pos until the end of buffer */
        n = PIPE_LEN - pos;
        /* the remaining chars are at the beginning of the buffer */
        m = cnt - n;
        memcpy(buf, &pipe->buf[pos], n);
        memcpy(buf + n, &pipe->buf[0], m);
    } else {
        memcpy(buf, &pipe->buf[pos], cnt);
    }

    pipe->tail = tail + cnt;

    return cnt;
}

/* -------------------------------------------------------------------------
 * Simulator Serial Link
 * ------------------------------------------------------------------------- */


void __attribute__((noreturn)) simlnk_loop(struct simlnk * lnk)
{
	uint32_t * pkt = lnk->rx.buf;
	uint32_t clk = 0;
	int cnt;

	INF("<%d> started...", thinkos_thread_self());
#if 0
	/* prepare for DMA transfer */
	assert(serial_dma_prepare(lnk->dev, lnk->rx.buf, SIMLINK_XFER_MAX) == 0);
#endif

	/* prepare for DMA transfer */
	if (serial_dma_prepare(lnk->dev, lnk->rx.buf, SIMLINK_XFER_MAX) < 0) {
		WARN("DMA transfer not supported!");
		/* set the trigger level above the MTU, this
		   will force the serial driver to notify the lower layer
		   when the channel is idle. */
		serial_rx_trig_set(lnk->dev, SIMLINK_XFER_MAX);
	} else {
		INF("Using DMA transfer!");
	}

	for (;;) {
		clk = thinkos_clock();
		uint32_t opc;
		unsigned int insn;
		unsigned int seq;

		(void)clk;
		/* Receive up to SIMLNK_MTU bytes plus the break condition */
		if ((cnt = serial_recv(lnk->dev, pkt, SIMLINK_XFER_MAX, 30000)) <= 0) {
			INF("SIMLNK %d idle.", lnk->addr);
			continue;
		}

		opc = pkt[0];
		insn = SIMRPC_INSN(opc);
		seq = SIMRPC_SEQ(opc);
		(void)seq;

		if (insn == SIMRPC_LINK) {
			WARN("SIMLNK %d signal: seq=%d cnt=%d.", lnk->addr, seq, cnt);
			lnk->stdout_seq = 0;
			continue;
		}

		if (insn == SIMRPC_FAULT) {
			WARN("SIMLNK %d fault: seq=%d cnt=%d!", lnk->addr, seq, cnt);
			lnk->stdout_seq = 0;
			continue;
		}

		if ((cnt >= 4) && (insn == SIMRPC_STDOUT_DATA)) {
			int n = cnt - 4;
/*
			DBG("RPC stdout: seq=%d cnt=%d.", seq, n);
*/
			if ((lnk->stdout_seq & 0xff) != seq) {
				WARN("RPC stdout: invalid seq %d, expected %d.", seq,
						lnk->stdout_seq & 0xff);
				/* Reset sequence */
				lnk->stdout_seq = seq;
			}
			lnk->stdout_seq++;
			pipe_write(&lnk->stdout_pipe, (uint8_t *)&pkt[1], n);
			continue;
		}
/*
		DBG("RPC recv: daddr=%02x saddr=%02x insn=%d seq=%d cnt=%d.",
				SIMRPC_DST(opc), SIMRPC_SRC(opc), insn, seq, cnt);
*/
		lnk->rx.cnt = cnt;
		thinkos_flag_give(lnk->rx.flag);
	}
}

int simlnk_recv(struct simlnk * lnk, void * buf, unsigned int max,
				unsigned int tmo)
{
	unsigned int cnt;

	if (thinkos_flag_timedtake(lnk->rx.flag, tmo) < 0) {
		ERR("thinkos_flag_take() failed!");
		return -1;
	}

	if (lnk->rx.cnt == 0) {
		ERR("thinkos_flag_take() sync error!");
		return -1;
	}

	cnt = MIN(lnk->rx.cnt, max);
	memcpy(buf, lnk->rx.buf, cnt);

	return cnt;
}

int simlnk_send(struct simlnk * lnk, const void * buf, unsigned int cnt) 
{
	if (cnt > SIMLNK_MTU)
		return -EINVAL;
#if 0
	for(;;) {
		if (thinkos_flag_take(lnk->tx.flag) < 0) {
			ERR("thinkos_flag_take() failed!");
			abort();
		}

		/* check for a slot in the xmit queue ... */
		if (lnk->tx.pdu_len == 0) 
			break;
	}

	/* insert frame in the transmission queue ...  */
	memcpy(lnk->tx.pdu, buf, cnt);
	lnk->tx.pdu_len = cnt;

	uint8_t * buf = lnk->tx.buf;
	int cnt = len;
#endif
	memcpy(lnk->tx.buf, buf, cnt);

	{ 
		uint32_t *p = (uint32_t *)lnk->tx.buf;
		(void)p;

		INF("%08x %08x", p[0], p[1]);
	}

	return serial_send(lnk->dev, lnk->tx.buf, cnt);
//	return cnt;
}

int simlnk_rpc(struct simrpc_pcb * sp, uint32_t insn,
			   const void * req, unsigned int cnt,
			   void * rsp, unsigned int max)
{
	unsigned int daddr = sp->daddr;
	unsigned int saddr = sp->saddr;
	unsigned int tmo = sp->tmo;
	struct simlnk * lnk = sp->lnk;
	uint32_t opc;
	uint32_t seq;
	int ret;

	thinkos_mutex_lock(lnk->mutex);

	seq = sp->seq = lnk->tx.seq++;
	opc = simrpc_mkopc(daddr, saddr, seq, insn);
	lnk->tx.buf[0] = opc;
	memcpy(&lnk->tx.buf[1], req, cnt);

	INF("RPC <%d> daddr=%d insn=%d seq=%d tmo=%d. <<",
			thinkos_thread_self(), daddr, insn, seq, tmo);

#if 0
	{ 
		uint32_t *p = (uint32_t *)lnk->tx.buf;
		(void)p;

		INF("%08x %08x", p[0], p[1]);
	}
#endif

	if (serial_send(lnk->dev, lnk->tx.buf, 4 + cnt) < 0) {
		WARN("RPC <%d> serial_send() error!", thinkos_thread_self());
		thinkos_mutex_unlock(lnk->mutex);
		return SIMRPC_EDRIVER;
	}

again:
	if ((ret = thinkos_flag_timedtake(lnk->rx.flag, tmo)) < 0) {
		if (ret == THINKOS_ETIMEDOUT) {
			WARN("RPC <%d> thinkos_flag_take() timed out!", thinkos_thread_self());
			thinkos_mutex_unlock(lnk->mutex);
			return SIMRPC_ETIMEDOUT;
		} else {
			ERR("RPC <%d> thinkos_flag_take() failed!", thinkos_thread_self());
			thinkos_mutex_unlock(lnk->mutex);
			return SIMRPC_ESYSTEM;
		}
	}

	if (lnk->rx.cnt < 4) {
		ERR("RPC <%d> link error, rx.cnt=%d! >>", thinkos_thread_self(), lnk->rx.cnt);
		thinkos_mutex_unlock(lnk->mutex);
		return SIMRPC_ELINK;
	}

	cnt = lnk->rx.cnt - 4;
	opc = lnk->rx.buf[0];

//	DBG("RPC: seq=%d,%d", seq, SIMRPC_SEQ(opc));

	if (opc == simrpc_mkopc(saddr, daddr, seq, SIMRPC_OK)) {
		if (rsp != NULL) {
			cnt = MIN(cnt, max);
			memcpy(rsp, &lnk->rx.buf[1], cnt);
		}
		INF("RPC <%d> OK. >>", thinkos_thread_self());
		thinkos_mutex_unlock(lnk->mutex);
		return cnt;
	}

	if ((cnt == 4) && (opc == simrpc_mkopc(saddr, daddr, seq, SIMRPC_ERR))) {
		int ret = (int)lnk->rx.buf[1];
		WARN("RPC <%d> error %d! >>", thinkos_thread_self(), ret);
		thinkos_mutex_unlock(lnk->mutex);
		return ret;
	}

	if ((SIMRPC_DST(opc) == saddr) && (SIMRPC_SRC(opc) == daddr)) {
		WARN("RPC <%d> invalid seq %d, expect %d (insn=%d). >>",
				thinkos_thread_self(), SIMRPC_SEQ(opc), seq, SIMRPC_INSN(opc));
		goto again;
	}

	WARN("RPC <%d> invalid response.", thinkos_thread_self());

	thinkos_mutex_unlock(lnk->mutex);
	return SIMRPC_EPROTO;
}

int simrpc_stdout_get(struct simrpc_pcb * sp, void * buf, unsigned int max)
{
	struct simlnk * lnk = sp->lnk;
	int cnt;

	cnt = pipe_read(&lnk->stdout_pipe, buf, max);
	if (cnt) {
		//fwrite(buf, cnt, 1, stdout);
//		DBG("pipe_read, %d bytes", cnt);
	}

	return cnt;
}

int simlnk_rpc_async(struct simrpc_pcb * sp, uint32_t insn,
			   const void * buf, unsigned int cnt)
{
	struct simlnk * lnk = sp->lnk;
	unsigned int daddr = sp->daddr;
	unsigned int saddr = sp->saddr;
	uint32_t opc;
	uint32_t seq;

	if (thinkos_mutex_trylock(lnk->mutex) < 0) {
		WARN("SIMLNK busy!");
		return -1;
	}

	seq = sp->seq = lnk->tx.seq++;
	opc = simrpc_mkopc(daddr, saddr, seq, insn);

	INF("RPC Async <%d> daddr=%d insn=%d seq=%d --",
			thinkos_thread_self(), daddr, insn, seq);

	lnk->tx.buf[0] = opc;
	memcpy(&lnk->tx.buf[1], buf, cnt);

	if (serial_send(lnk->dev, lnk->tx.buf, 4 + cnt) < 0) {
		thinkos_mutex_unlock(lnk->mutex);
		return SIMRPC_EDRIVER;
	}

	thinkos_mutex_unlock(lnk->mutex);

	return cnt;
}

/* -------------------------------------------------------------------------
 * Initialization
 * ------------------------------------------------------------------------- */

int simlnk_getstat(struct simlnk * lnk, struct simlnk_stat * stat, bool reset)
{
	if (lnk == NULL)
		return -EINVAL;

	if (stat == NULL)
		return -EINVAL;

	stat->rx_err = lnk->stat.rx_err;
	stat->rx_err = lnk->stat.rx_token;
	stat->rx_err = lnk->stat.rx_mgmt;
	stat->rx_err = lnk->stat.rx_unicast;
	stat->rx_err = lnk->stat.rx_bcast;
	stat->rx_err = lnk->stat.tx_token;
	stat->rx_err = lnk->stat.tx_mgmt;
	stat->rx_err = lnk->stat.tx_unicast;
	stat->rx_err = lnk->stat.tx_bcast;
	stat->token_lost = lnk->stat.token_lost;

	if (reset) {
		lnk->stat.rx_err = 0;
		lnk->stat.rx_token = 0;
		lnk->stat.rx_mgmt = 0;
		lnk->stat.rx_unicast = 0;
		lnk->stat.rx_bcast = 0;
		lnk->stat.tx_token = 0;
		lnk->stat.tx_mgmt = 0;
		lnk->stat.tx_unicast = 0;
		lnk->stat.tx_bcast = 0;
		lnk->stat.token_lost = 0;
	}

	return 0;
}

/* -------------------------------------------------------------------------
 * Initialization
 * ------------------------------------------------------------------------- */
int simlnk_init(struct simlnk * lnk, const char * name, 
				unsigned int addr, struct serial_dev * dev)
{
	if (lnk == NULL)
		return -EINVAL;

	INF("addr=%d", addr);

	lnk->dev = dev;
	lnk->addr = addr;

	lnk->rx.flag = thinkos_flag_alloc();
	lnk->tx.flag = thinkos_flag_alloc();
	lnk->tx.seq = 0;
	lnk->mutex = thinkos_mutex_alloc();

	DBG("tx.flag=%d rx.flag=%d", lnk->rx.flag, lnk->tx.flag);

	return 0;
}


/* -------------------------------------------------------------------------
 * Pool of resources
 * ------------------------------------------------------------------------- */

static struct simlnk simlnk_pool[SIMLNK_POOL_SIZE]; 

struct simlnk * simlnk_alloc(void)
{
	struct simlnk * lnk;
	int i;

	for (i = 0; i < SIMLNK_POOL_SIZE; ++i) { 
		lnk = &simlnk_pool[i];
		if (!lnk->pool_alloc) {
			memset(lnk, 0, sizeof(struct simlnk));
			lnk->pool_alloc = true;
			return lnk;
		}
	}

	return NULL;
}

