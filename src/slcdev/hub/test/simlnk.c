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
#include <sys/dcclog.h>

#include <thinkos.h>
#include "simlnk.h"
#include "simrpc.h"
#include "io.h"

#undef DEBUG
#undef TRACE_LEVEL
#define TRACE_LEVEL TRACE_LVL_WARN
#include <trace.h>

#ifndef SIMLNK_POOL_SIZE
#define SIMLNK_POOL_SIZE 5
#endif

struct simlnk {
	bool pool_alloc; /* indicate if the structure was 
						allocated from a resource pool */
	uint8_t addr; /* this station address */
	struct serial_dev * dev;
	struct {
		uint32_t buf[SIMLNK_MTU / 4];
		volatile unsigned int cnt;
		int flag;
	} rx;
	struct {
		uint32_t rpc_seq;
		uint32_t buf[SIMLNK_MTU / 4];
		int flag;
	} tx;
	struct simlnk_stat stat;
};
/* -------------------------------------------------------------------------
 * Simulator Serial Link 
 * ------------------------------------------------------------------------- */

void __attribute__((noreturn)) simlnk_loop(struct simlnk * lnk)
{
	uint32_t * pkt = lnk->rx.buf;
	uint32_t clk = 0;
	int cnt;

	INF("<%d> started...", thinkos_thread_self());

	for (;;) {
		clk = thinkos_clock();
		(void)clk;
		if ((cnt = serial_recv(lnk->dev, pkt, SIMLNK_MTU, 0x7fffffff)) <= 0) {
			continue;
		}

		if (SIMRPC_OPC(pkt[0]) == SIMRPC_SIGNAL) {
			continue;
		}

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

		DCC_LOG2(LOG_TRACE, "%08x %08x", p[0], p[1]);
	}

	return serial_send(lnk->dev, lnk->tx.buf, cnt);
//	return cnt;
}

static uint32_t mkopc(unsigned int daddr, unsigned int saddr,
					  unsigned int seq, unsigned int insn)
{
	return daddr | (saddr << 8) | ((seq & 0xff) << 16) | (insn << 24);
}

int simlnk_rpc(struct simlnk * lnk, 
			   unsigned int daddr, uint32_t insn,
			   const void * req, unsigned int cnt,
			   void * rsp, unsigned int max)
{
	unsigned int tmo = SIMRPC_DEF_TMO_MS;
	unsigned int saddr = io_addr_get();
	uint32_t opc;
	uint32_t seq;

	seq = lnk->tx.rpc_seq++; 
	opc = mkopc(daddr, saddr, seq, insn);

	lnk->tx.buf[0] = opc;
	memcpy(&lnk->tx.buf[1], req, cnt);

#if 0
	{ 
		uint32_t *p = (uint32_t *)lnk->tx.buf;
		(void)p;

		DCC_LOG2(LOG_TRACE, "%08x %08x", p[0], p[1]);
	}
#endif

	if (serial_send(lnk->dev, lnk->tx.buf, 4 + cnt) < 0) {
		return SIMRPC_EDRIVER;
	}

	if (thinkos_flag_timedtake(lnk->rx.flag, tmo) < 0) {
		ERR("thinkos_flag_take() failed!");
		return SIMRPC_ESYSTEM;
	}

	if (lnk->rx.cnt < 4) {
		ERR("thinkos_flag_take() link error!");
		return SIMRPC_ELINK;
	}

	cnt = lnk->rx.cnt - 4;
	opc = lnk->rx.buf[0];

	DCC_LOG2(LOG_MSG, "resp opc=%08x cnt=%d", opc, cnt);

	if (opc == mkopc(saddr, daddr, seq, SIMRPC_OK)) {
		if (rsp != NULL) {
			cnt = MIN(cnt, max);
			memcpy(rsp, &lnk->rx.buf[1], cnt);
		}
		return cnt;
	}

	if ((cnt == 4) && (opc == mkopc(saddr, daddr, seq, SIMRPC_ERR))) {
		DCC_LOG1(LOG_WARNING, "error %d.", (int)lnk->rx.buf[1]);
		return (int)lnk->rx.buf[1];
	}

	DCC_LOG(LOG_WARNING, "invalid response.");
	return SIMRPC_EPROTOCOL;
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

	DCC_LOG1(LOG_TRACE, "addr=%d", addr);

	lnk->dev = dev;
	lnk->addr = addr;

	lnk->rx.flag = thinkos_flag_alloc();
	lnk->tx.flag = thinkos_flag_alloc();

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

