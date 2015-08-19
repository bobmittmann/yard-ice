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

#undef DEBUG
#undef TRACE_LEVEL
#define TRACE_LEVEL TRACE_LVL_WARN
#include <trace.h>

#ifndef SIMLNK_POOL_SIZE
#define SIMLNK_POOL_SIZE 5
#endif

#define ROUTE_FRM_TYPE(_ROUTE) ((_ROUTE) & 0xff)
#define ROUTE_DST_ADDR(_ROUTE) (((_ROUTE) >> 8) & 0xff)
#define ROUTE_SRC_ADDR(_ROUTE) (((_ROUTE) >> 16) & 0xff)

#define MKROUTE(_TYP, _DST, _SRC) ((_TYP) + ((_DST) << 8) + ((_SRC) << 16))

#define SIMLNK_MTU 512
#define SIMLNK_PDU_MAX (SIMLNK_MTU - 11)

struct simlnk_stat {
	uint32_t rx_err;
	uint32_t rx_token;
	uint32_t rx_mgmt;
	uint32_t rx_unicast;
	uint32_t rx_bcast;
	uint32_t tx_token;
	uint32_t tx_mgmt;
	uint32_t tx_unicast;
	uint32_t tx_bcast;
	uint32_t token_lost;
};


struct simlnk {
	bool pool_alloc; /* indicate if the structure was 
						allocated from a resource pool */
	volatile uint8_t state; 
	bool sole_master;
	uint8_t addr; /* this station address */
	struct serial_dev * dev;
	struct {
		uint8_t buf[SIMLNK_MTU];
		volatile uint16_t pdu_len;
		uint8_t pdu[SIMLNK_PDU_MAX];
		unsigned int off;
		unsigned int cnt;
		int flag;
	} rx;
	struct {
		uint32_t buf[SIMLNK_MTU / 4];
		int flag;
	} tx;
	struct simlnk_stat stat;
};
/* -------------------------------------------------------------------------
 * Simulator Serial Link 
 * ------------------------------------------------------------------------- */

int simlnk_frame_recv(struct simlnk * lnk, unsigned int tmo)
{
	uint8_t * buf = lnk->rx.buf;
	int cnt;

	if ((cnt = serial_recv(lnk->dev, buf, SIMLNK_MTU, tmo)) <= 0) {
		return cnt;
	}

	return cnt;
}

int simlnk_frame_send(struct simlnk * lnk, unsigned int route, 
						   uint8_t * pdu, unsigned int len)
{
	uint8_t * buf = (uint8_t *)lnk->tx.buf;
	int cnt = len;

	return serial_send(lnk->dev, buf, cnt);
}

void __attribute__((noreturn)) simlnk_loop(struct simlnk * lnk)
{
	uint32_t clk = 0;
	int ret;

	clk = thinkos_clock();

	for (;;) {
		ret = simlnk_frame_recv(lnk, 100);
		(void)ret;
		clk = thinkos_clock();
		(void)clk;
	}

}

int simlnk_recv(struct simlnk * lnk, void * buf, unsigned int max)
{
#if 0
	int pdu_len;

	for(;;) {
		if (thinkos_flag_take(lnk->rx.flag) < 0) {
			ERR("thinkos_flag_take() failed!");
			abort();
		}

		/* check for a slot in the xmit queue ... */
		if (lnk->rx.pdu_len) 
			break;
	};

	pdu_len = MIN(lnk->rx.pdu_len, max);
	memcpy(buf, lnk->rx.pdu, pdu_len);

	lnk->rx.pdu_len = 0;

	return pdu_len;
#endif
	int cnt;

	if ((cnt = serial_recv(lnk->dev, lnk->rx.buf, SIMLNK_MTU, 1000)) <= 0) {
		return cnt;
	}

	return cnt;
}

int simlnk_send(struct simlnk * lnk, const void * buf, unsigned int cnt) 
{
	if (cnt > SIMLNK_PDU_MAX)
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

		DCC_LOG2(LOG_TRACE, "%08x %08x", p[0], p[1]);
	}

	return serial_send(lnk->dev, lnk->tx.buf, cnt);
//	return cnt;
}


/* -------------------------------------------------------------------------
 * Initialization
 * ------------------------------------------------------------------------- */

int simlnk_resume(struct simlnk * lnk)
{
	if (lnk == NULL)
		return -EINVAL;

	DBG("Starting BACnet MS/TP Data Link");

	lnk->state = 0;
	DBG("[SIMIDLE]");

	return 0;
}

int simlnk_stop(struct simlnk * lnk)
{
	if (lnk == NULL)
		return -EINVAL;

	DBG("Pausing BACnet MS/TP Data Link");

	lnk->state = 0;
	DBG("[SIMINITIALIZE]");

	return 0;
}

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

