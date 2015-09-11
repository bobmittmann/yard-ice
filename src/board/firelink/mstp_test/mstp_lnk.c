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
 * @file mstp_lnk.c
 * @brief MS/TP Link Layer
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */

#include "mstp_lnk-i.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/param.h>

#undef DEBUG
#undef TRACE_LEVEL
#define TRACE_LEVEL TRACE_LVL_WARN
#include <trace.h>

/* -------------------------------------------------------------------------
 * MS/TP Link Layer
 * ------------------------------------------------------------------------- */

#define N_MAX_INFO_FRAMES 1
#define N_MAX_MASTER MSTP_LNK_MAX_MASTERS
#define N_POLL 50
#define N_RETRY_TOKEN 1
#define N_MIN_OCTETS 4

/* The minimum time without a DataAvailable or ReceiveError event within a 
   frame before a receiving node may discard the frame: 60 bit times. 
   (Implementations may use larger values for this timeout, not to exceed 
   100 milliseconds.) */
#define T_FRAME_ABORT_MS 3

/* The time without a DataAvailable or ReceiveError event before declaration 
   of loss of token: 500 millisecondsa */
#define T_NO_TOKEN      500
#define T_REPLY_DELAY   250
#define T_REPLY_TIMEOUT 255
#define T_SLOT          10
#define T_USAGE_DELAY   15
#define T_USAGE_TIMEOUT 20 

#define MSTP_TIMEOUT           1
#define MSTP_RCVD_VALID_FRAME  0
#define MSTP_HDR_SYNC_ERROR   -1
#define MSTP_HDR_CRC_ERROR    -2
#define MSTP_DATA_CRC_ERROR   -3

int mstp_frame_recv(struct mstp_lnk * lnk, unsigned int tmo)
{
	uint8_t * buf = lnk->rx.buf;
	unsigned int crc;
	unsigned int type;
	unsigned int daddr;
	int pdu_len;
	int cnt;

	if ((cnt = serial_recv(lnk->dev, buf, MSTP_LNK_MTU, tmo)) <= 0) {
		return MSTP_TIMEOUT;
	}

	if (buf[0] != 0x55) {
		WARN("frame error 1");
		lnk->rx.off = 0; 
		lnk->stat.rx_err++;
		return MSTP_HDR_SYNC_ERROR;
	}

	if (buf[1] != 0xff) {
		WARN("frame error 2");
		lnk->rx.off = 0; 
		lnk->stat.rx_err++;
		return MSTP_HDR_SYNC_ERROR;
	}

	crc = __mstp_crc8(0xff, buf[2]);
	crc = __mstp_crc8(crc, buf[3]);
	crc = __mstp_crc8(crc, buf[4]);
	crc = __mstp_crc8(crc, buf[5]);
	crc = (~__mstp_crc8(crc, buf[6]) & 0xff);

	pdu_len = (buf[5] << 8) + buf[6];

	if (buf[7] != crc) {
		WARN("CRC error: %02x != %02x, %d->%d %02x (%d)", buf[7], crc,
				buf[4], buf[3], buf[2], pdu_len);
		lnk->rx.off = 0; 
		lnk->stat.rx_err++;
		return MSTP_HDR_CRC_ERROR;
	}

//	INF("cnt=%d pdu_len=%d", cnt, pdu_len);

	if (pdu_len > 0) {
		unsigned int chk;
		chk = (buf[pdu_len + 9] << 8) + buf[pdu_len + 8];
		crc = (~mstp_crc16(0xffff, &buf[8], pdu_len)) & 0xffff;
		if (crc != chk) {
			WARN("Data CRC error %04x != %04x", crc, chk);
			lnk->rx.off = 8;
			lnk->stat.rx_err++;
			return MSTP_DATA_CRC_ERROR;
		}
		lnk->rx.off = pdu_len + 10;
	} else {
		lnk->rx.off = 8;
	}

	type = buf[2];
	daddr = buf[3];

	switch (type) {
	case FRM_TOKEN:
		lnk->stat.rx_token++;
		break;
	case FRM_POLL_FOR_MASTER:
	case FRM_REPLY_POLL_FOR_MASTER:
	case FRM_TEST_REQUEST:
	case FRM_TEST_RESPONSE:
	case FRM_REPLY_POSTPONED:
		lnk->stat.rx_mgmt++;
		break;
	default:
		if (daddr == MSTP_ADDR_BCAST)
			lnk->stat.rx_bcast++;
		else
			lnk->stat.rx_unicast++;
		break;
	}

	return MSTP_RCVD_VALID_FRAME; /* Frame Type */
}

#define ROUTE_FRM_TYPE(_ROUTE) ((_ROUTE) & 0xff)
#define ROUTE_DST_ADDR(_ROUTE) (((_ROUTE) >> 8) & 0xff)
#define ROUTE_SRC_ADDR(_ROUTE) (((_ROUTE) >> 16) & 0xff)

#define MKROUTE(_TYP, _DST, _SRC) ((_TYP) + ((_DST) << 8) + ((_SRC) << 16))

static int mstp_frame_send(struct mstp_lnk * lnk, unsigned int route, 
						   uint8_t * pdu, unsigned int len)
{
	uint8_t * buf = lnk->tx.buf;
	unsigned int crc;
	unsigned int type;
	unsigned int daddr;
	int cnt;
	int i;

	/* encode header */
	buf[0] = 0x55;
	buf[1] = 0xff;
	buf[2] = ROUTE_FRM_TYPE(route);
	buf[3] = ROUTE_DST_ADDR(route);
	buf[4] = ROUTE_SRC_ADDR(route);
	buf[5] = (len >> 8) & 0xff;
	buf[6] = len & 0xff;
	crc = __mstp_crc8(0xff, buf[2]);
	crc = __mstp_crc8(crc, buf[3]);
	crc = __mstp_crc8(crc, buf[4]);
	crc = __mstp_crc8(crc, buf[5]);
	buf[7] = ~__mstp_crc8(crc, buf[6]);

	if (len > 0) {
		/* encode PDU */
		uint8_t * cp = &buf[8];
		crc = ~mstp_crc16(0xffff, pdu, len);
		for (i = 0; i < len; ++i)
			cp[i] = pdu[i];

		cp[i++] = (crc & 0xff);
		cp[i++] = (crc >> 8) & 0xff;

		cnt = len + 10;
	} else {
		buf[8] = 0xff;
		buf[9] = 0xff;
		cnt = 8;
	}

	type = buf[2];
	daddr = buf[3];

	switch (type) {
	case FRM_TOKEN:
		lnk->stat.tx_token++;
		break;
	case FRM_POLL_FOR_MASTER:
	case FRM_REPLY_POLL_FOR_MASTER:
	case FRM_TEST_REQUEST:
	case FRM_TEST_RESPONSE:
	case FRM_REPLY_POSTPONED:
		lnk->stat.tx_mgmt++;
		break;
	default:
		if (daddr == MSTP_ADDR_BCAST)
			lnk->stat.tx_bcast++;
		else
			lnk->stat.tx_unicast++;
		break;
	}

	return serial_send(lnk->dev, buf, cnt);
}

static int mstp_fast_send(struct mstp_lnk * lnk, unsigned int type, unsigned int daddr)
{
	uint8_t * buf = lnk->tx.token;
	unsigned int crc;

	/* encode token */
	buf[2] = type;
	buf[3] = daddr;
	crc = __mstp_crc8(0xff, buf[2]);
	crc = __mstp_crc8(crc, buf[3]);
	crc = __mstp_crc8(crc, buf[4]);
	crc = __mstp_crc8(crc, 0);
	buf[7] = ~__mstp_crc8(crc, 0);

	switch (type) {
	case FRM_TOKEN:
		lnk->stat.tx_token++;
		break;
	case FRM_POLL_FOR_MASTER:
	case FRM_REPLY_POLL_FOR_MASTER:
	case FRM_TEST_REQUEST:
	case FRM_TEST_RESPONSE:
	case FRM_REPLY_POSTPONED:
		lnk->stat.tx_mgmt++;
		break;
	default:
		if (daddr == MSTP_ADDR_BCAST)
			lnk->stat.tx_bcast++;
		else
			lnk->stat.tx_unicast++;
		break;
	}

	return serial_send(lnk->dev, buf, 8);
}

#define SILENCE_TIMER() ((int32_t)(clk - silence_clk))
#define RESET_SILENCE_TIMER() silence_clk = clk

const char * state_nm[] = {
		[MSTP_INITIALIZE] = "INITIALIZE",
		[MSTP_IDLE] = "IDLE",
		[MSTP_ANSWER_DATA_REQUEST] = "ANSWER_DATA_REQUEST",
		[MSTP_NO_TOKEN] = "NO_TOKEN",
		[MSTP_POLL_FOR_MASTER] = "POLL_FOR_MASTER",
		[MSTP_PASS_TOKEN] = "PASS_TOKEN",
		[MSTP_USE_TOKEN] = "USE_TOKEN",
		[MSTP_DONE_WITH_TOKEN] = "DONE_WITH_TOKEN",
		[MSTP_WAIT_FOR_REPLY] = "WAIT_FOR_REPLY"
	};

void __attribute__((noreturn)) mstp_lnk_loop(struct mstp_lnk * lnk)
{
	uint32_t clk = 0;
	int32_t dt;
	int frm_type;
	int src_addr;
	int dst_addr;
	int frame_count;
	int retry_count;
	int event_count;
	int token_count; 
	int ts; /* This station */
	int ns; /* Next Station */
	int ps; /* Poll Station */
	uint32_t silence_clk;
	bool rcvd_valid_frm;
	bool rcvd_invalid_frm;
	int ret;
	uint16_t timer_ms[] = {
		[MSTP_INITIALIZE] = 100,
		[MSTP_IDLE] = T_NO_TOKEN,
		[MSTP_ANSWER_DATA_REQUEST] = T_REPLY_DELAY,
		[MSTP_NO_TOKEN] = (T_NO_TOKEN + T_SLOT * lnk->addr),
		[MSTP_POLL_FOR_MASTER] = T_USAGE_TIMEOUT,
		[MSTP_PASS_TOKEN] = T_USAGE_TIMEOUT,
		[MSTP_USE_TOKEN] = 5,
		[MSTP_DONE_WITH_TOKEN] = 0,
		[MSTP_WAIT_FOR_REPLY] = T_REPLY_TIMEOUT
	};

	lnk->tx.pdu_len = 0;
	thinkos_flag_give(lnk->rx.flag);

	lnk->rx.pdu_len = 0;
	thinkos_flag_give(lnk->tx.flag);

	/* set TS to the node's station address, set NS equal to TS 
	   (indicating that the next station is unknown), set PS equal to TS, 
	   set TokenCount to Npoll (thus causing a Poll For Master to be sent 
	   when this node first receives the token), set SoleMaster to FALSE, 
	   set ReceivedValidFrame and ReceivedInvalidFrame to FALSE, and enter 
	   the IDLE state.a */
	ts = lnk->addr;
	ns = ts;
	ps = ns;
	lnk->sole_master = false;
	rcvd_valid_frm = false;
	rcvd_invalid_frm = false;
	frame_count = 0;
	retry_count = 0;
	token_count = 0;
	event_count = 0;
	frm_type = 0;
	src_addr = 0;
	dst_addr = 0;

	/* prepare the token buffer for fast transfers */
	lnk->tx.token[0] = 0x55;
	lnk->tx.token[1] = 0xff;
	lnk->tx.token[2] = 0;
	lnk->tx.token[3] = 0;
	lnk->tx.token[4] = ts;
	lnk->tx.token[5] = 0;
	lnk->tx.token[6] = 0;

	while (lnk->state == MSTP_INITIALIZE) {
		/* In this state, the device waits for the network layer to 
		   initiate a MSTP data lnk connection or for the physical 
		   layer to indicate the occurrence of a physical 
		   layer connection. */
		thinkos_sleep(100);
	}

	clk = thinkos_clock();

	/* prepare for DMA transfer */
	if (serial_dma_prepare(lnk->dev, lnk->rx.buf, MSTP_LNK_MTU) < 0) {
		WARN("DMA transfer not supported!");
		/* set the trigger level above the MTU, this
		   will force the serial driver to notify the lower layer
		   when the channel is idle. */
		serial_rx_trig_set(lnk->dev, MSTP_LNK_MTU + 1);
	} else {
		WARN("Using DMA transfer!");
	}

	DBG("[INITIALIZE]  --> [IDLE]");
	RESET_SILENCE_TIMER();

again:
	/* XXX: find the clock about to expire sooner  */
	dt = silence_clk - clk + timer_ms[lnk->state];
/*
	INF("state=%s tmr=%d dt=%d", state_nm[lnk->state],
			 timer_ms[lnk->state], dt);
	if (dt <= 0) {
		INF("state=%s tmr=%d dt=%d", state_nm[lnk->state],
			 timer_ms[lnk->state], dt);
	}
*/
	ret = mstp_frame_recv(lnk, dt);
	event_count += lnk->rx.off;
	clk = thinkos_clock();

	switch (ret) {
	case MSTP_RCVD_VALID_FRAME:
		rcvd_valid_frm = true;
		frm_type = lnk->rx.buf[2];
		dst_addr = lnk->rx.buf[3];
		src_addr = lnk->rx.buf[4];
		RESET_SILENCE_TIMER();
		break;

	case MSTP_HDR_SYNC_ERROR:
	case MSTP_HDR_CRC_ERROR:
	case MSTP_DATA_CRC_ERROR:
		rcvd_invalid_frm = true;
		RESET_SILENCE_TIMER();
		break;

	case MSTP_TIMEOUT:
		break;
	}

transition_now:

	switch (lnk->state) {
	case MSTP_IDLE:
		if (SILENCE_TIMER() >= T_NO_TOKEN) {
			event_count = 0; /* Addendum 135-2004d-8 */
			lnk->state = MSTP_NO_TOKEN;
			DBG("[IDLE] LostToken --> [NO_TOKEN]");
			rcvd_invalid_frm = false;
			rcvd_valid_frm = false;
			lnk->stat.token_lost++;
			lnk->mgmt.callback(lnk, MSTP_EV_TOKEN_LOST);
			goto transition_now;
		} if (rcvd_invalid_frm) {
			rcvd_invalid_frm = false;
			DBG("[IDLE] ReceivedInvalidFrame --> [IDLE]");
		} if (rcvd_valid_frm) {
			if (dst_addr == ts || dst_addr == MSTP_ADDR_BCAST) {
				switch (frm_type) {
				case FRM_TOKEN:
				case FRM_REPLY_POLL_FOR_MASTER:
					if (dst_addr == MSTP_ADDR_BCAST) {
						ERR("FRM_TOKEN, dst_addr==MSTP_ADDR_BCAST");
						break;
					}
					rcvd_valid_frm = false;
					frame_count = 0;
					if (lnk->sole_master) {
						lnk->mgmt.callback(lnk, MSTP_EV_MULTIMASTER);
						lnk->sole_master = false;
					}
					lnk->state = MSTP_USE_TOKEN;
					DBG("[IDLE] ReceivedToken --> [USE_TOKEN]");
					goto transition_now;
				case FRM_POLL_FOR_MASTER:
					mstp_fast_send(lnk, FRM_REPLY_POLL_FOR_MASTER, src_addr);
					RESET_SILENCE_TIMER();
					DBG("[IDLE] ReceivedPFM --> [IDLE]");
					break;

				case FRM_BACNET_DATA_NO_REPLY:
				case FRM_TEST_RESPONSE:
				case FRM_DATA_NO_REPLY ... (FRM_DATA_NO_REPLY + 0x3f):
					lnk->rx.inf.type = frm_type;
					lnk->rx.inf.saddr = src_addr;
					lnk->rx.inf.daddr = dst_addr;
					lnk->rx.pdu_len = lnk->rx.off - 10;
					memcpy(lnk->rx.pdu, &lnk->rx.buf[8], lnk->rx.pdu_len);
					thinkos_flag_give(lnk->rx.flag);
//					bacnet_dl_pdu_recv_notify(lnk->addr.netif);
					INF("RCV saddr=%d pdu_len=%d", lnk->rx.inf.saddr, lnk->rx.pdu_len);
					DBG("[IDLE] ReceivedDataNoReply --> [IDLE]");
					break;
				case FRM_BACNET_DATA_XPCT_REPLY:
				case FRM_TEST_REQUEST:
				case FRM_DATA_XPCT_REPLY ... (FRM_DATA_XPCT_REPLY + 0x3f):
					lnk->rx.inf.type = frm_type;
					lnk->rx.inf.saddr = src_addr;
					lnk->rx.inf.daddr = dst_addr;
					lnk->rx.pdu_len = lnk->rx.off - 10;
					memcpy(lnk->rx.pdu, &lnk->rx.buf[8], lnk->rx.pdu_len);
					thinkos_flag_give(lnk->rx.flag);
					INF("RCV saddr=%d pdu_len=%d", lnk->rx.inf.saddr, lnk->rx.pdu_len);
//					bacnet_dl_pdu_recv_notify(lnk->addr.netif);
					if (dst_addr == MSTP_ADDR_BCAST) {
						lnk->state = MSTP_ANSWER_DATA_REQUEST;
						DBG("[IDLE] ReceivedDataNeedingReply"
								" --> [ANSWER_DATA_REQUEST]");
					}
					break;
				default:
					rcvd_valid_frm = false;
					DBG("[IDLE] ReceivedUnwantedFrame"
							" --> [IDLE]");
					break;
				}
			} else {
				DBG("[IDLE] NotForMe --> [IDLE]");
				if (frm_type == FRM_REPLY_POLL_FOR_MASTER) {
					int i = src_addr / 32;
					int j = src_addr % 32;
					/* keep track of other masters */
					lnk->mgmt.netmap[i] |= 1 << j;
				}
			}
			/* For DATA_EXPECTING_REPLY, we will keep the Rx Frame for
			   reference, and the flag will be cleared in the next state */
			if (lnk->state != MSTP_ANSWER_DATA_REQUEST)
				rcvd_valid_frm = false;
		}
		break;

	case MSTP_USE_TOKEN:
		if (lnk->tx.pdu_len == 0) {
			/* no data frame awaiting transmission */
			frame_count = N_MAX_INFO_FRAMES;
			lnk->state = MSTP_DONE_WITH_TOKEN;
			DBG("[USE_TOKEN] NothingToSend --> [DONE_WITH_TOKEN]");
			goto transition_now;
		} else {
			frm_type = lnk->tx.inf.type;
			dst_addr = lnk->tx.inf.daddr;
			mstp_frame_send(lnk, MKROUTE(frm_type, dst_addr, ts),
							lnk->tx.pdu, lnk->tx.pdu_len);
			INF("XMT daddr=%d pdu_len=%d", lnk->tx.inf.daddr, lnk->tx.pdu_len);
			RESET_SILENCE_TIMER();
			lnk->tx.pdu_len = 0;
			thinkos_flag_give(lnk->tx.flag);
			frame_count++;
			switch (frm_type) {
			case FRM_BACNET_DATA_XPCT_REPLY:
			case FRM_DATA_XPCT_REPLY ... (FRM_DATA_XPCT_REPLY + 0x3f):
				if (dst_addr == MSTP_ADDR_BCAST)  {
					lnk->state = MSTP_DONE_WITH_TOKEN;
					DBG("[USE_TOKEN] SendNoWait --> [DONE_WITH_TOKEN]");
					goto transition_now;
				} else {
					lnk->state = MSTP_WAIT_FOR_REPLY;
					DBG("[USE_TOKEN] SendAndWait --> [WAIT_FOR_REPLY]");
				}
				break;
			case FRM_TEST_REQUEST: 
				lnk->state = MSTP_WAIT_FOR_REPLY;
				DBG("[USE_TOKEN] SendAndWait --> [WAIT_FOR_REPLY]");
				break;
			case FRM_TEST_RESPONSE:
			case FRM_BACNET_DATA_NO_REPLY:
			case FRM_DATA_NO_REPLY ... (FRM_DATA_NO_REPLY + 0x3f):
			default:
				lnk->state = MSTP_DONE_WITH_TOKEN;
				DBG("[USE_TOKEN] SendNoWait --> [DONE_WITH_TOKEN]");
				goto transition_now;
				break;
			}
		}
		break;

	case MSTP_WAIT_FOR_REPLY:
		if (SILENCE_TIMER() >= T_REPLY_TIMEOUT) {
			frame_count = N_MAX_INFO_FRAMES;
			lnk->state = MSTP_DONE_WITH_TOKEN;
			DBG("[WAIT_FOR_REPLY] ReplyTimeout"
					" --> [DONE_WITH_TOKEN]");
			goto transition_now;
		} else {
			if (rcvd_invalid_frm == true) {
				rcvd_invalid_frm = false;
				lnk->state = MSTP_DONE_WITH_TOKEN;
				DBG("[WAIT_FOR_REPLY] InvalidFrame"
						" --> [DONE_WITH_TOKEN]");
				goto transition_now;
			} else if (rcvd_valid_frm == true) {
				if (dst_addr == ts) {
					switch (frm_type) {
					case FRM_REPLY_POSTPONED:
						lnk->state = MSTP_DONE_WITH_TOKEN;
						DBG("[WAIT_FOR_REPLY] ReceivedPostponed"
								" --> [DONE_WITH_TOKEN]");
						break;
					case FRM_TEST_RESPONSE:
					case FRM_BACNET_DATA_NO_REPLY:
					case FRM_DATA_NO_REPLY ... (FRM_DATA_NO_REPLY + 0x3f):
						lnk->rx.inf.type = frm_type;
						lnk->rx.inf.saddr = src_addr;
						lnk->rx.inf.daddr = dst_addr;
						lnk->rx.pdu_len = lnk->rx.off - 10;
						memcpy(lnk->rx.pdu, &lnk->rx.buf[8], lnk->rx.pdu_len);
						thinkos_flag_give(lnk->rx.flag);
//						bacnet_dl_pdu_recv_notify(lnk->addr.netif);
						lnk->state = MSTP_DONE_WITH_TOKEN;
						INF("RCV saddr=%d pdu_len=%d", lnk->rx.inf.saddr, lnk->rx.pdu_len);
						DBG("[WAIT_FOR_REPLY] ReceivedReply --> [DONE_WITH_TOKEN]");
						break;
					default:
						lnk->state = MSTP_IDLE;
						DBG("[WAIT_FOR_REPLY]"
								" ReceivedUnexpectedFrame --> [IDLE]");
						break;
					}
				} else {
					lnk->state = MSTP_IDLE;
					DBG("[WAIT_FOR_REPLY]"
							" ReceivedUnexpectedFrame --> [IDLE]");
				}
				rcvd_valid_frm = false;
				goto transition_now;
			}
		}
		break;

	case MSTP_DONE_WITH_TOKEN:
		/* The DONE_WITH_TOKEN state either sends another data frame,  */
		/* passes the token, or initiates a Poll For Master cycle. */
		/* SendAnotherFrame */
		if (frame_count < N_MAX_INFO_FRAMES) {
			lnk->state = MSTP_USE_TOKEN;
			DBG("[DONE_WITH_TOKEN] SendAnotherFrame --> [USE_TOKEN]");
			goto transition_now;
		} else if (lnk->sole_master == false && ns == ts) {
			/* NextStationUnknown - added in Addendum 135-2008v-1 */
			/*  then the next station to which the token
				should be sent is unknown - so PollForMaster */
			ps = (ts + 1) % (N_MAX_MASTER + 1);
			mstp_fast_send(lnk, FRM_POLL_FOR_MASTER, ps);
			RESET_SILENCE_TIMER();
			retry_count = 0;
			lnk->state = MSTP_POLL_FOR_MASTER;
			DBG("[DONE_WITH_TOKEN]"
					" NextStationUnknown --> [POLL_FOR_MASTER]", 
					thinkos_clock());
		} else if (token_count < (N_POLL - 1)) {
			if (lnk->sole_master == true &&
				ns != (ts + 1) % (N_MAX_MASTER + 1)) {
				frame_count = 0;
#if 0
				token_count++;
				/* FIXME: test only ... */
				RESET_SILENCE_TIMER();
#else
				token_count = N_POLL;
#endif
				lnk->state = MSTP_USE_TOKEN;
				DBG("[DONE_WITH_TOKEN] SoleMaster --> [USE_TOKEN]");
				goto transition_now;
			} else { 
				token_count++;
				mstp_fast_send(lnk, FRM_TOKEN, ns);
				RESET_SILENCE_TIMER();
				retry_count = 0;
				event_count = 0;
				lnk->state = MSTP_PASS_TOKEN;
				DBG("[DONE_WITH_TOKEN] SendToken --> [PASS_TOKEN]");
			}
		} else if ((ps + 1) % (N_MAX_MASTER + 1) == ns) {
			if (lnk->sole_master == true) {
				ps = (ns + 1) % (N_MAX_MASTER + 1);
				mstp_fast_send(lnk, FRM_POLL_FOR_MASTER, ps);
				RESET_SILENCE_TIMER();
				ns = ts;
				retry_count = 0;
				/* changed in Errata SSPC-135-2004 */
				token_count = 1;
				/* event_count = 0; removed in Addendum 135-2004d-8 */
				lnk->state = MSTP_POLL_FOR_MASTER;
				DBG("[DONE_WITH_TOKEN]"
						" SoleMasterRestartMaintenancePFM -->"
						" [POLL_FOR_MASTER]");
			} else {
				ps = ts;
				mstp_fast_send(lnk, FRM_POLL_FOR_MASTER, ns);
				RESET_SILENCE_TIMER();
				retry_count = 0;
                /* changed in Errata SSPC-135-2004 */
				token_count = 1;
				event_count = 0;
				lnk->state = MSTP_PASS_TOKEN;
				DBG("[DONE_WITH_TOKEN] ResetMaintenancePFM"
						" --> [PASS_TOKEN]");
			}
		} else {
			ps = (ps + 1) % (N_MAX_MASTER + 1);
			mstp_fast_send(lnk, FRM_POLL_FOR_MASTER, ps);
			RESET_SILENCE_TIMER();
			retry_count = 0;
			lnk->state = MSTP_POLL_FOR_MASTER;
			DBG("[DONE_WITH_TOKEN] SendMaintenancePFM"
					" --> [POLL_FOR_MASTER]");
		}

		break;

	case MSTP_PASS_TOKEN:
		if (SILENCE_TIMER() <= T_USAGE_TIMEOUT) {
			if (event_count > N_MIN_OCTETS) {
                /* Enter the IDLE state to process the frame. */
				lnk->state = MSTP_IDLE;
				DBG("[PASS_TOKEN] SawTokenUser --> [IDLE]");
				goto transition_now;
			}
		} else {
			if (retry_count < N_RETRY_TOKEN) {
				retry_count++;
				mstp_fast_send(lnk, FRM_TOKEN, ns);
				RESET_SILENCE_TIMER();
				event_count = 0;
				DBG("[PASS_TOKEN] RetrySendToken --> [PASS_TOKEN]");
			} else {
				ps = (ns + 1) % (N_MAX_MASTER + 1);
				mstp_fast_send(lnk, FRM_POLL_FOR_MASTER, ps);
				RESET_SILENCE_TIMER();
				ns = ts;
				retry_count = 0;
				token_count = 0;
				/* event_count = 0; removed in Addendum 135-2004d-8 */
				lnk->state = MSTP_POLL_FOR_MASTER;
				DBG("[PASS_TOKEN] FindNewSuccessor --> [POLL_FOR_MASTER]");
			}
		}
		break;

	case MSTP_NO_TOKEN:
		if (SILENCE_TIMER() < (T_NO_TOKEN + T_SLOT * ts)) {
			if (event_count > N_MIN_OCTETS) {
				lnk->state = MSTP_IDLE;
				DBG("[NO_TOKEN] SawFRame --> [IDLE]");
				goto transition_now;
			}
			DBG("(event_count(%d) <= N_MIN_OCTETS!!",
					 event_count);
		} else {
			int32_t ns_tmo = T_NO_TOKEN + T_SLOT * (ts + 1);
			int32_t mm_tmo = T_NO_TOKEN + T_SLOT * (N_MAX_MASTER  + 1);
			if (SILENCE_TIMER() < ns_tmo ||
				SILENCE_TIMER() > mm_tmo) {
				ps = (ts + 1) % (N_MAX_MASTER  + 1);
				mstp_fast_send(lnk, FRM_POLL_FOR_MASTER, ps);
				RESET_SILENCE_TIMER();
				ns = ts;
				token_count = 0;
				/* event_count = 0; removed Addendum 135-2004d-8 */
				retry_count = 0;
				lnk->state = MSTP_POLL_FOR_MASTER;
				DBG("[NO_TOKEN] GenerateToken "
						"--> [POLL_FOR_MASTER]");
			} else {
				if (event_count > N_MIN_OCTETS) {
					lnk->state = MSTP_IDLE;
					DBG("[NO_TOKEN] SawFRame --> [IDLE]");
					goto transition_now;
				}
			}
		}
		break;

	case MSTP_POLL_FOR_MASTER:
		/* In the POLL_FOR_MASTER state, the node listens for a reply to */
		/* a previously sent Poll For Master frame in order to find  */
		/* a successor node. */
		if (rcvd_valid_frm == true) {
			if (dst_addr == ts && frm_type == FRM_REPLY_POLL_FOR_MASTER) {
				ns = src_addr;
				event_count = 0;
				mstp_fast_send(lnk, FRM_TOKEN, ns);
				RESET_SILENCE_TIMER();
				ps = ts;
				token_count = 0;
				retry_count = 0;
				rcvd_valid_frm = false;
				if (lnk->sole_master) {
					lnk->mgmt.callback(lnk, MSTP_EV_MULTIMASTER);
					lnk->sole_master = false;
				}
				lnk->state = MSTP_PASS_TOKEN;
				DBG("[POLL_FOR_MASTER] ReceivedReplyToPFM --> [PASS_TOKEN]");
			} else {
				rcvd_valid_frm = false;
				lnk->state = MSTP_IDLE;
				DBG("[POLL_FOR_MASTER] "
						 "ReceivedUnexpectedFrame --> [IDLE]");
				goto transition_now;
			}
		} else if (SILENCE_TIMER() >= T_USAGE_TIMEOUT || 
				   rcvd_invalid_frm == true) { 
			if (lnk->sole_master == true) {
				frame_count = 0;
				lnk->state = MSTP_USE_TOKEN;
				DBG("[POLL_FOR_MASTER] SoleMaster --> [USE_TOKEN]");
				/* XXX: hack for testing pourposes .... */
				RESET_SILENCE_TIMER();
				/* XXX: end of hack. */
			} else {
				if (ns != ts) {
					event_count = 0;
					mstp_fast_send(lnk, FRM_TOKEN, ns);
					RESET_SILENCE_TIMER();
					retry_count = 0;
					lnk->state = MSTP_PASS_TOKEN;
					DBG("[POLL_FOR_MASTER] DoneWithPFM --> [PASS_TOKEN]");
				} else {
					if ((ps + 1) % (N_MAX_MASTER + 1) != ts) {
						ps = (ps + 1) % (N_MAX_MASTER + 1);
						mstp_fast_send(lnk, FRM_POLL_FOR_MASTER, ps);
						RESET_SILENCE_TIMER();
						retry_count = 0;
						DBG("[POLL_FOR_MASTER] SendNextPFM --> [POLL_FOR_MASTER]");
					} else {
						lnk->sole_master = true;
						frame_count = 0;
						rcvd_invalid_frm = false;
						lnk->state = MSTP_USE_TOKEN;
						lnk->mgmt.callback(lnk, MSTP_EV_SOLE_MASTER);
						DBG("[POLL_FOR_MASTER] DeclareSoleMaster --> [USE_TOKEN]");
						goto transition_now;
					}
				}
			}
			rcvd_invalid_frm = false;
		}
		break;


	case MSTP_ANSWER_DATA_REQUEST:
		if (SILENCE_TIMER() < T_REPLY_DELAY && lnk->tx.pdu_len > 0  &&
			(lnk->tx.inf.type == FRM_TEST_RESPONSE ||
			 lnk->tx.inf.type == FRM_BACNET_DATA_NO_REPLY ||
			 (lnk->tx.inf.type >= FRM_DATA_NO_REPLY  &&
			 lnk->tx.inf.type < (FRM_DATA_NO_REPLY + 0x3f)))) {
			mstp_frame_send(lnk, MKROUTE(lnk->tx.inf.type, lnk->tx.inf.daddr, ts),
							lnk->tx.pdu, lnk->tx.pdu_len);
			RESET_SILENCE_TIMER();
			lnk->state = MSTP_IDLE;
			DBG("[ANSWER_DATA_REQUEST] Reply"
					"--> IDLE[]");
			rcvd_valid_frm = false;
		} else if (SILENCE_TIMER() >= T_REPLY_DELAY) {
			mstp_fast_send(lnk, FRM_REPLY_POSTPONED, src_addr);
			RESET_SILENCE_TIMER();
			lnk->state = MSTP_IDLE;
			DBG("[ANSWER_DATA_REQUEST] DeferredReply"
					"--> [IDLE]");
			rcvd_valid_frm = false;
		}
		break;
	}

	goto again;
}

int mstp_lnk_recv(struct mstp_lnk * lnk, void * buf, unsigned int max,
				  struct mstp_frame_inf * inf)
{
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
	*inf = lnk->rx.inf;

	lnk->rx.pdu_len = 0;

	return pdu_len;
}

int mstp_lnk_send(struct mstp_lnk * lnk, const void * buf, unsigned int cnt, 
				  const struct mstp_frame_inf * inf)
{
	if (cnt > MSTP_LNK_PDU_MAX)
		return -EINVAL;

	if ((inf->type != FRM_BACNET_DATA_XPCT_REPLY) &&
		(inf->type < FRM_DATA_XPCT_REPLY)) {
		return -EINVAL;
	}

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
	lnk->tx.inf = *inf;
	lnk->tx.pdu_len = cnt;

	return cnt;
}

int mstp_lnk_getaddr(struct mstp_lnk * lnk)
{
	return lnk->addr;
}

int mstp_lnk_getbcast(struct mstp_lnk * lnk)
{
	return MSTP_ADDR_BCAST;
}

/* -------------------------------------------------------------------------
 * Initialization
 * ------------------------------------------------------------------------- */

static void mstp_lnk_default_callback(struct mstp_lnk * lnk, unsigned int ev)
{
}

int mstp_lnk_init(struct mstp_lnk * lnk, const char * name, 
				  unsigned int addr, struct serial_dev * dev)
{
	if (lnk == NULL)
		return -EINVAL;

	if (addr > N_MAX_MASTER)
		return -EINVAL;

	lnk->dev = dev;
	lnk->state = MSTP_INITIALIZE;
	lnk->addr = addr;

	DBG("[MSTP_INITIALIZE]");
	lnk->rx.flag = thinkos_flag_alloc();
	lnk->tx.flag = thinkos_flag_alloc();


	lnk->mgmt.callback = mstp_lnk_default_callback;

	DBG("tx.flag=%d rx.flag=%d", lnk->rx.flag, lnk->tx.flag);

	return 0;
}

int mstp_lnk_resume(struct mstp_lnk * lnk)
{
	if (lnk == NULL)
		return -EINVAL;

	DBG("Starting BACnet MS/TP Data Link");

	lnk->state = MSTP_IDLE;
	DBG("[MSTP_IDLE]");

	return 0;
}

int mstp_lnk_stop(struct mstp_lnk * lnk)
{
	if (lnk == NULL)
		return -EINVAL;

	DBG("Pausing BACnet MS/TP Data Link");

	lnk->state = MSTP_INITIALIZE;
	DBG("[MSTP_INITIALIZE]");

	return 0;
}

int mstp_lnk_mgmt(struct mstp_lnk * lnk,
		void (*cbk)(struct mstp_lnk *, unsigned int))
{
	if (lnk == NULL)
		return -EINVAL;

	lnk->mgmt.callback = cbk;
	return 0;
}

int mstp_lnk_getstat(struct mstp_lnk * lnk, struct mstp_lnk_stat * stat, bool reset)
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

int mstp_lnk_getnetmap(struct mstp_lnk * lnk, uint8_t map[], unsigned int max)
{
	int addr;
	int cnt;

	if (max > MSTP_LNK_MAX_MASTERS)
		max = MSTP_LNK_MAX_MASTERS;

	for (addr = 0; addr < MSTP_LNK_MAX_MASTERS; ++addr) {
		int i = addr / 32;
		int j = addr % 32;
		if (lnk->mgmt.netmap[i] & (j << i)) {
			if (cnt < max)
				map[cnt++] = addr;
		}
	}

	return cnt;
}

/* -------------------------------------------------------------------------
 * Pool of resources
 * ------------------------------------------------------------------------- */

static struct mstp_lnk mstp_lnk_pool[MSTP_LNK_POOL_SIZE]; 

struct mstp_lnk * mstp_lnk_alloc(void)
{
	struct mstp_lnk * lnk;
	int i;

	for (i = 0; i < MSTP_LNK_POOL_SIZE; ++i) { 
		lnk = &mstp_lnk_pool[i];
		if (!lnk->pool_alloc) {
			memset(lnk, 0, sizeof(struct mstp_lnk));
			lnk->pool_alloc = true;
			return lnk;
		}
	}

	return NULL;
}

