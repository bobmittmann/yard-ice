/* 
 * File:	 usb-test.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2011 Bob Mittmann. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <sys/param.h>
#include <thinkos.h>
#include <errno.h>

#include <sys/dcclog.h>

#include <bacnet/bacnet-dl.h>
#include <bacnet/bacnet-mstp.h>

#include "bacnet-i.h"

/* -------------------------------------------------------------------------
 * 10 DATA LINK/PHYSICAL LAYERS: POINT-TO-POINT (MSTP)
 * ------------------------------------------------------------------------- */

/* MSTP Frame  types */
enum {
	FRM_TOKEN                  = 0x00,
	FRM_POLL_FOR_MASTER        = 0x01,
	FRM_REPLY_POLL_FOR_MASTER  = 0x02, 
	FRM_TEST_REQUEST           = 0x03,
	FRM_TEST_RESPONSE          = 0x04,
	FRM_BACNET_DATA_XPCT_REPLY = 0x05,
	FRM_BACNET_DATA_NO_REPLY   = 0x06,
	FRM_REPLY_POSTPONED        = 0x07
};


struct bacnet_mstp_frm {
	struct bacnet_mstp_hdr hdr;
	uint8_t data[];
};

/* MSTP Connection State Machine states */
enum {
	MSTP_INITIALIZE = 0,
	MSTP_IDLE,
	MSTP_ANSWER_DATA_REQUEST,
	MSTP_NO_TOKEN,
	MSTP_POLL_FOR_MASTER,
	MSTP_PASS_TOKEN,
	MSTP_USE_TOKEN,
	MSTP_DONE_WITH_TOKEN,
	MSTP_WAIT_FOR_REPLY
};

#define N_MAX_INFO_FRAMES 1
//#define N_MAX_MASTER 127
#define N_MAX_MASTER 31
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

#define MSTP_BCAST 255

int bacnet_mstp_frame_recv(struct bacnet_mstp_lnk * lnk, unsigned int tmo)
{
	int off = lnk->rx.off;
	int cnt = lnk->rx.cnt;
	uint8_t * buf = lnk->rx.buf;
	uint8_t * cp;
	int len;
	uint8_t crc;

	/* move remaining octets to the beginning of the buffer */
	if (off) {
		DCC_LOG2(LOG_INFO, "shift off=%d cnt=%d", off, cnt);
		cnt = cnt - off;
		if (cnt > 0)
			memcpy(buf, &buf[off], cnt);
	} else
		cnt = 0;

	off = 0;
	cp = &buf[cnt];

	/* receive a packet header */
	while (cnt < 8) {
		int i;
		int j;
		int n;
		int rem = BACNET_MSTP_MTU - cnt;

		if ((n = serial_recv(lnk->dev, cp, rem, tmo)) <= 0) {
			DCC_LOG1(LOG_INFO, "serial_recv=%d", n);
			lnk->rx.off = 0;
			lnk->rx.cnt = 0;
			return MSTP_TIMEOUT;
		}

		cnt += n;
		cp += n;
	}

	if (buf[0] != 0x55) {
		DCC_LOG(LOG_WARNING, "frame error 1");
		lnk->rx.off = 1;
		lnk->rx.cnt = cnt;
		return MSTP_HDR_SYNC_ERROR;
	}

	if (buf[1] != 0xff) {
		DCC_LOG(LOG_WARNING, "frame error 2");
		lnk->rx.off = 2;
		lnk->rx.cnt = cnt;
		return MSTP_HDR_SYNC_ERROR;
	}

	crc = ~bacnet_crc8(0xff, &buf[2], 5);

	if (buf[7] != crc) {
		DCC_LOG2(LOG_WARNING, "CRC error: %02x != %02x", buf[7], crc);
		lnk->rx.off = 2;
		lnk->rx.cnt = cnt;
		return MSTP_HDR_CRC_ERROR;
	}

	len = (buf[5] << 8) + buf[6];
	DCC_LOG2(LOG_INFO, "cnt=%d len=%d", cnt, len);

	if (len > 0) {
		uint16_t crc;
		uint16_t chk;
		int i;

		/* receive the frame data */
		while (cnt < len + 10) {
			int rem = BACNET_MSTP_MTU - cnt;
			int n;

			n = serial_recv(lnk->dev, cp, rem, tmo);
			if (n <= 0) {
				DCC_LOG1(LOG_WARNING, "serdrv_recv=%d", n);
				lnk->rx.off = 0;
				lnk->rx.cnt = 0;
				return MSTP_TIMEOUT;
			}

			cnt += n;
			cp += n;
		}

		chk = (buf[len + 9] << 8) + buf[len + 8];
		crc = ~bacnet_crc16(0xffff, &buf[8], len);
		if (crc != chk) {
			DCC_LOG2(LOG_WARNING, "Data CRC error %04x != %04x", crc, chk);
			lnk->rx.off = 0;
			lnk->rx.cnt = 0;
			return MSTP_DATA_CRC_ERROR;
		}
		lnk->rx.pdu_len = len;
		off = len + 10;
	} else {
		lnk->rx.pdu_len = 0;
		off = 8;
	}

	lnk->rx.off = off;
	lnk->rx.cnt = cnt;

	switch (buf[2]) {
	case FRM_TOKEN:
		DCC_LOG(LOG_INFO, "Token");
		break;
	case FRM_POLL_FOR_MASTER:
		DCC_LOG2(LOG_TRACE, "Poll For Master (%d, %d)", buf[3], buf[4]);
		break;
	case FRM_REPLY_POLL_FOR_MASTER:
		DCC_LOG2(LOG_TRACE, "Reply Poll For Master (%d, %d)", buf[3], buf[4]);
		break;
	case FRM_TEST_REQUEST:
		DCC_LOG(LOG_TRACE, "Test Request");
		break;
	case FRM_TEST_RESPONSE:
		DCC_LOG(LOG_TRACE, "Test Response");
		break;
	case FRM_BACNET_DATA_XPCT_REPLY:
		DCC_LOG(LOG_TRACE, "BACnet Data Expecting Reply");
		break;
	case FRM_BACNET_DATA_NO_REPLY:
		DCC_LOG(LOG_TRACE, "BACnet Data Not Expecting Reply");
		break;
	case FRM_REPLY_POSTPONED:
		DCC_LOG(LOG_TRACE, "Reply Postponed");
		break;
	}

	return MSTP_RCVD_VALID_FRAME; /* Frame Type */
}

#define ROUTE_FRM_TYPE(_ROUTE) ((_ROUTE) & 0xff)
#define ROUTE_DST_ADDR(_ROUTE) (((_ROUTE) >> 8) & 0xff)
#define ROUTE_SRC_ADDR(_ROUTE) (((_ROUTE) >> 16) & 0xff)

#define FRAME(_TYP, _DST, _SRC) ((_TYP) + ((_DST) << 8) + ((_SRC) << 16))

#define BACNET_DATA_XPCT_REPLY(_DST, _SRC) \
	FRAME(FRM_BACNET_DATA_XPCT_REPLY, _DST, _SRC)

#define BACNET_DATA_NO_REPLY(_DST, _SRC) \
	FRAME(FRM_BACNET_DATA_NO_REPLY, _DST, _SRC)

#define REPLY_POLL_FOR_MASTER(_DST, _SRC) \
	FRAME(FRM_REPLY_POLL_FOR_MASTER, _DST, _SRC)

#define POLL_FOR_MASTER(_DST, _SRC) FRAME(FRM_POLL_FOR_MASTER, _DST, _SRC)

#define TOKEN(_DST, _SRC) FRAME(FRM_TOKEN, _DST, _SRC)

#define REPLY_POSTPONED(_DST, _SRC) FRAME(FRM_REPLY_POSTPONED, _DST, _SRC)

int bacnet_mstp_frame_send(struct bacnet_mstp_lnk * lnk, unsigned int route, 
						   uint8_t * pdu, unsigned int len)
{
	uint8_t buf[10 + len];
	int cnt;
	int i;
	int j;
	int c;

	/* reset TX idle timer */
//	lnk->tx.idle_clk = lnk->clk + TX_IDLE_HBEAT_TMO_MS; 

	/* encode header */
	buf[0] = 0x55;
	buf[1] = 0xff;
	buf[2] = ROUTE_FRM_TYPE(route);
	buf[3] = ROUTE_DST_ADDR(route);
	buf[4] = ROUTE_SRC_ADDR(route);
	buf[5] = (len >> 8) & 0xff;
	buf[6] = len & 0xff;
	buf[7] = ~bacnet_crc8(0xff, &buf[2], 5);

	cnt = 8;

	if (len > 0) {
		uint16_t crc;
		uint8_t * cp = &buf[8];
		crc = ~bacnet_crc16(0xffff, pdu, len);
		for (i = 0; i < len; ++i)
			cp[i] = pdu[i];

		cp[i++] = (crc & 0xff);
		cp[i++] = (crc >> 8) & 0xff;

		cnt += len;
	} 

	thinkos_sleep(1);

	switch (buf[2]) {
	case FRM_TOKEN:
		DCC_LOG(LOG_INFO, "Token");
		break;
	case FRM_POLL_FOR_MASTER:
		DCC_LOG2(LOG_INFO, "Poll For Master (%d, %d)", buf[3], buf[4]);
		break;
	case FRM_REPLY_POLL_FOR_MASTER:
		DCC_LOG2(LOG_TRACE, "Reply Poll For Master (%d, %d)", buf[3], buf[4]);
		break;
	case FRM_TEST_REQUEST:
		DCC_LOG(LOG_TRACE, "Test Request");
		break;
	case FRM_TEST_RESPONSE:
		DCC_LOG(LOG_TRACE, "Test Response");
		break;
	case FRM_BACNET_DATA_XPCT_REPLY:
		DCC_LOG(LOG_TRACE, "BACnet Data Expecting Reply");
		break;
	case FRM_BACNET_DATA_NO_REPLY:
		DCC_LOG(LOG_TRACE, "BACnet Data Not Expecting Reply");
		break;
	case FRM_REPLY_POSTPONED:
		DCC_LOG(LOG_TRACE, "Reply Postponed");
		break;
	}

	return serial_send(lnk->dev, buf, cnt);
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

int bacnet_mstp_loop(struct bacnet_mstp_lnk * lnk)
{
	uint32_t clk = 0;
	int32_t dt;
	uint8_t * pdu;
	int pdu_len;
	int len;
	int frm_type;
	int src_addr;
	int dst_addr;
	int frame_count;
	int retry_count;
	int event_count;
	int token_count; 
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
		[MSTP_NO_TOKEN] = (T_NO_TOKEN + T_SLOT * lnk->ts),
		[MSTP_POLL_FOR_MASTER] = T_USAGE_TIMEOUT,
		[MSTP_PASS_TOKEN] = T_USAGE_TIMEOUT,
		[MSTP_USE_TOKEN] = 25,
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
	ns = lnk->ts;
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

	while (lnk->state == MSTP_INITIALIZE) {
		/* In this state, the device waits for the network layer to 
		   initiate a MSTP data link connection or for the physical 
		   layer to indicate the occurrence of a physical 
		   layer connection. */
		thinkos_sleep(100);
		clk = thinkos_clock();
	}

	DCC_LOG1(LOG_TRACE, "%6d:[INITIALIZE]  --> [IDLE]", clk);
	RESET_SILENCE_TIMER();
again:
	/* XXX: find the clock about to expire sooner  */

	dt = silence_clk - clk + timer_ms[lnk->state];
	DCC_LOG3(LOG_INFO, "state=%s tmr=%d dt=%d", state_nm[lnk->state],
			 timer_ms[lnk->state], dt);
	ret = bacnet_mstp_frame_recv(lnk, dt);
	event_count += lnk->rx.off;
	clk = thinkos_clock();

	switch (ret) {
	case MSTP_RCVD_VALID_FRAME:
		rcvd_valid_frm = true;
		frm_type = lnk->rx.hdr.type;
		dst_addr = lnk->rx.hdr.daddr;
		src_addr = lnk->rx.hdr.saddr;
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

//	if (clk >= 10000)
//		abort();
	
	switch (lnk->state) {
	case MSTP_IDLE:
		if (SILENCE_TIMER() >= T_NO_TOKEN) {
			event_count = 0; /* Addendum 135-2004d-8 */
			lnk->state = MSTP_NO_TOKEN;
			DCC_LOG1(LOG_TRACE, "%6d:[IDLE] LostToken --> [NO_TOKEN]", clk);
			rcvd_invalid_frm = false;
			rcvd_valid_frm = false;
			goto transition_now;
		} if (rcvd_invalid_frm) {
			rcvd_invalid_frm = false;
			DCC_LOG1(LOG_TRACE, "%6d:[IDLE] ReceivedInvalidFrame --> [IDLE]", 
					clk);
		} if (rcvd_valid_frm) {
			if (dst_addr == lnk->ts || dst_addr == MSTP_BCAST) {
				switch (frm_type) {
				case FRM_TOKEN:
				case FRM_REPLY_POLL_FOR_MASTER:
					if (dst_addr == MSTP_BCAST) {
						DCC_LOG(LOG_ERROR, "FRM_TOKEN, dst_addr==MSTP_BCAST");
						break;
					}
					rcvd_valid_frm = false;
					frame_count = 0;
					lnk->sole_master = false;
					lnk->state = MSTP_USE_TOKEN;
					DCC_LOG1(LOG_INFO, "%6d:[IDLE] ReceivedToekn"
							 " --> [USE_TOKEN]", clk);
					goto transition_now;
				case FRM_POLL_FOR_MASTER:
					bacnet_mstp_frame_send(lnk, 
										   REPLY_POLL_FOR_MASTER(src_addr, 
																 dst_addr), 
										   NULL, 0);
					RESET_SILENCE_TIMER();
					DCC_LOG1(LOG_TRACE, "%6d:[IDLE] ReceivedPFM --> [IDLE]", 
							clk);
					break;

				case FRM_BACNET_DATA_NO_REPLY:
				case FRM_TEST_RESPONSE:
					thinkos_flag_give(lnk->rx.flag);
					bacnet_dl_pdu_recv_notify(lnk->dln);
					DCC_LOG(LOG_TRACE, "[IDLE] ReceivedDataNoReply --> [IDLE]");
					break;

				case FRM_BACNET_DATA_XPCT_REPLY:
				case FRM_TEST_REQUEST:
					thinkos_flag_give(lnk->rx.flag);
					bacnet_dl_pdu_recv_notify(lnk->dln);
					if (dst_addr == MSTP_BCAST) {
						lnk->state = MSTP_ANSWER_DATA_REQUEST;
						DCC_LOG(LOG_TRACE, "[IDLE] ReceivedDataNeedingReply"
								" --> [ANSWER_DATA_REQUEST]");
					}
					break;
				default:
					rcvd_valid_frm = false;
					DCC_LOG(LOG_TRACE, "[IDLE] ReceivedUnwantedFrame"
							" --> [IDLE]");
					break;
				}
			} else {
				DCC_LOG1(LOG_INFO, "%6d:[IDLE] NotForMe --> [IDLE]", clk);
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
			DCC_LOG1(LOG_INFO, "%6d:[USE_TOKEN] NothingToSend"
					" --> [DONE_WITH_TOKEN]", clk);
			goto transition_now;
		} else {
			frm_type = lnk->tx.frm_type;
			dst_addr = lnk->tx.dst_addr;
			bacnet_mstp_frame_send(lnk, FRAME(frm_type, dst_addr, lnk->ts),
								   lnk->tx.pdu, lnk->tx.pdu_len);
			RESET_SILENCE_TIMER();
			lnk->tx.pdu_len = 0;
			thinkos_flag_give(lnk->tx.flag);
			frame_count++;
			switch (frm_type) {
			case FRM_BACNET_DATA_XPCT_REPLY:
				if (dst_addr == MSTP_BCAST)  {
					lnk->state = MSTP_DONE_WITH_TOKEN;
					DCC_LOG1(LOG_TRACE, "%6d:[USE_TOKEN] SendNoWait"
							" --> [DONE_WITH_TOKEN]", clk);
				} else {
					lnk->state = MSTP_WAIT_FOR_REPLY;
					DCC_LOG1(LOG_TRACE, "%6d:[USE_TOKEN] SendAndWait"
							" --> [WAIT_FOR_REPLY]", clk);
				}
				break;
			case FRM_TEST_REQUEST: 
				lnk->state = MSTP_WAIT_FOR_REPLY;
				DCC_LOG1(LOG_TRACE, "%6d:[USE_TOKEN] SendAndWait"
						" --> [WAIT_FOR_REPLY]", clk);
				break;
			case FRM_TEST_RESPONSE:
			case FRM_BACNET_DATA_NO_REPLY:
			default:
				lnk->state = MSTP_DONE_WITH_TOKEN;
				DCC_LOG1(LOG_TRACE, "%6d:[USE_TOKEN] SendNoWait"
						" --> [DONE_WITH_TOKEN]", clk);
				break;
			}
		}
		break;

	case MSTP_WAIT_FOR_REPLY:
		if (SILENCE_TIMER() >= T_REPLY_TIMEOUT) {
			frame_count = N_MAX_INFO_FRAMES;
			lnk->state = MSTP_DONE_WITH_TOKEN;
			DCC_LOG(LOG_TRACE, "[WAIT_FOR_REPLY] ReplyTimeout"
					" --> [DONE_WITH_TOKEN]");
			goto transition_now;
		} else {
			if (rcvd_invalid_frm == true) {
				rcvd_invalid_frm = false;
				lnk->state = MSTP_DONE_WITH_TOKEN;
				DCC_LOG(LOG_TRACE, "[WAIT_FOR_REPLY] InvalidFrame"
						" --> [DONE_WITH_TOKEN]");
				goto transition_now;
			} else if (rcvd_valid_frm == true) {
				if (dst_addr == lnk->ts) {
					switch (frm_type) {
					case FRM_REPLY_POSTPONED:
						lnk->state = MSTP_DONE_WITH_TOKEN;
						DCC_LOG(LOG_TRACE, "[WAIT_FOR_REPLY] ReceivedPostponed"
								" --> [DONE_WITH_TOKEN]");
						break;
					case FRM_TEST_RESPONSE:
					case FRM_BACNET_DATA_NO_REPLY:
						thinkos_flag_give(lnk->rx.flag);
						bacnet_dl_pdu_recv_notify(lnk->dln);
						lnk->state = MSTP_DONE_WITH_TOKEN;
						DCC_LOG(LOG_TRACE, "[WAIT_FOR_REPLY] ReceivedReply"
								" --> [DONE_WITH_TOKEN]");
						break;
					default:
						lnk->state = MSTP_IDLE;
						DCC_LOG(LOG_TRACE, "[WAIT_FOR_REPLY]"
								" ReceivedUnexpectedFrame --> [IDLE]");
						break;
					}
				} else {
					lnk->state = MSTP_IDLE;
					DCC_LOG(LOG_TRACE, "[WAIT_FOR_REPLY]"
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
			DCC_LOG1(LOG_TRACE, "%6d:[DONE_WITH_TOKEN] SendAnotherFrame"
					" --> [USE_TOKEN]", clk);
			goto transition_now;
		} else if (lnk->sole_master == false && ns == lnk->ts) {
			/* NextStationUnknown - added in Addendum 135-2008v-1 */
			/*  then the next station to which the token
				should be sent is unknown - so PollForMaster */
			ps = (lnk->ts + 1) % (N_MAX_MASTER + 1);
			bacnet_mstp_frame_send(lnk, POLL_FOR_MASTER(ps, lnk->ts),
								   NULL, 0);
			RESET_SILENCE_TIMER();
			retry_count = 0;
			lnk->state = MSTP_POLL_FOR_MASTER;
			DCC_LOG1(LOG_TRACE, "%6d:[DONE_WITH_TOKEN]"
					" NextStationUnknown --> [POLL_FOR_MASTER]", clk);
		} else if (token_count < (N_POLL - 1)) {
			if (lnk->sole_master == true &&
				ns != (lnk->ts + 1) % (N_MAX_MASTER + 1)) {
				frame_count = 0;
				token_count++;
				lnk->state = MSTP_USE_TOKEN;
				DCC_LOG1(LOG_TRACE, "%6d:[DONE_WITH_TOKEN] SoleMaster"
						" --> [USE_TOKEN]", clk);
				/* FIXME: test only ... */
				RESET_SILENCE_TIMER();
//				goto transition_now;
			} else { 
				token_count++;
				bacnet_mstp_frame_send(lnk, TOKEN(ns, lnk->ts), NULL, 0);
				RESET_SILENCE_TIMER();
				retry_count = 0;
				event_count = 0;
				lnk->state = MSTP_PASS_TOKEN;
				DCC_LOG1(LOG_INFO, "%6d:[DONE_WITH_TOKEN] SendToken"
						" --> [PASS_TOKEN]", clk);
			}
		} else if ((ps + 1) % (N_MAX_MASTER + 1) == ns) {
			if (lnk->sole_master == true) {
				ps = (ns + 1) % (N_MAX_MASTER + 1);
				bacnet_mstp_frame_send(lnk, POLL_FOR_MASTER(ps, lnk->ts),
									   NULL, 0);
				RESET_SILENCE_TIMER();
				ns = lnk->ts;
				retry_count = 0;
				/* changed in Errata SSPC-135-2004 */
				token_count = 1;
				/* event_count = 0; removed in Addendum 135-2004d-8 */
				lnk->state = MSTP_POLL_FOR_MASTER;
				DCC_LOG1(LOG_TRACE, "%6d:[DONE_WITH_TOKEN]"
						" SoleMasterRestartMaintenancePFM -->"
						" [POLL_FOR_MASTER]", clk);
			} else {
				ps = lnk->ts;
				bacnet_mstp_frame_send(lnk, POLL_FOR_MASTER(ns, lnk->ts),
									   NULL, 0);
				RESET_SILENCE_TIMER();
				retry_count = 0;
                /* changed in Errata SSPC-135-2004 */
				token_count = 1;
				event_count = 0;
				lnk->state = MSTP_PASS_TOKEN;
				DCC_LOG1(LOG_INFO, "%6d:[DONE_WITH_TOKEN] ResetMaintenancePFM"
						" --> [PASS_TOKEN]", clk);
			}
		} else {
			ps = (ps + 1) % (N_MAX_MASTER + 1);
			bacnet_mstp_frame_send(lnk, POLL_FOR_MASTER(ps, lnk->ts),
								   NULL, 0);
			RESET_SILENCE_TIMER();
			retry_count = 0;
			lnk->state = MSTP_POLL_FOR_MASTER;
			DCC_LOG1(LOG_INFO, "%6d:[DONE_WITH_TOKEN] SendMaintenancePFM"
					" --> [POLL_FOR_MASTER]", clk);
		}

		break;

	case MSTP_PASS_TOKEN:
		if (SILENCE_TIMER() <= T_USAGE_TIMEOUT) {
			if (event_count > N_MIN_OCTETS) {
                /* Enter the IDLE state to process the frame. */
				lnk->state = MSTP_IDLE;
				DCC_LOG1(LOG_INFO, "%6d:[PASS_TOKEN] SawTokenUser"
						" --> [IDLE]", clk);
				goto transition_now;
			}
		} else {
			if (retry_count < N_RETRY_TOKEN) {
				retry_count++;
				bacnet_mstp_frame_send(lnk, TOKEN(ns, lnk->ts), NULL, 0);
				RESET_SILENCE_TIMER();
				event_count = 0;
				DCC_LOG1(LOG_TRACE, "%6d:[PASS_TOKEN] RetrySendToken"
						" --> [PASS_TOKEN]", clk);
			} else {
				ps = (ns + 1) % (N_MAX_MASTER + 1);
				bacnet_mstp_frame_send(lnk, POLL_FOR_MASTER(ps, lnk->ts), 
									   NULL, 0);
				RESET_SILENCE_TIMER();
				ns = lnk->ts;
				retry_count = 0;
				token_count = 0;
				/* event_count = 0; removed in Addendum 135-2004d-8 */
				lnk->state = MSTP_POLL_FOR_MASTER;
				DCC_LOG1(LOG_TRACE, "%6d:[PASS_TOKEN] FindNewSuccessor"
						" --> [POLL_FOR_MASTER]", clk);
			}
		}
		break;

	case MSTP_NO_TOKEN:
		if (SILENCE_TIMER() < (T_NO_TOKEN + T_SLOT * lnk->ts)) {
			if (event_count > N_MIN_OCTETS) {
				lnk->state = MSTP_IDLE;
				DCC_LOG1(LOG_TRACE, "%6d:[NO_TOKEN] SawFRame --> [IDLE]", clk);
				goto transition_now;
			}
			DCC_LOG1(LOG_TRACE, "(event_count(%d) <= N_MIN_OCTETS!!", 
					 event_count);
		} else {
			int ns_tmo = T_NO_TOKEN + T_SLOT * (lnk->ts + 1);
			int mm_tmo = T_NO_TOKEN + T_SLOT * (N_MAX_MASTER  + 1);
			if (SILENCE_TIMER() < ns_tmo ||
				SILENCE_TIMER() > mm_tmo) {
				ps = (lnk->ts + 1) % (N_MAX_MASTER  + 1);
				bacnet_mstp_frame_send(lnk, POLL_FOR_MASTER(ps, lnk->ts), 
									   NULL, 0);
				RESET_SILENCE_TIMER();
				ns = lnk->ts;
				token_count = 0;
				/* event_count = 0; removed Addendum 135-2004d-8 */
				retry_count = 0;
				lnk->state = MSTP_POLL_FOR_MASTER;
				DCC_LOG1(LOG_TRACE, "%6d:[NO_TOKEN] GenerateToken "
						"--> [POLL_FOR_MASTER]", clk);
			} else {
				if (event_count > N_MIN_OCTETS) {
					lnk->state = MSTP_IDLE;
					DCC_LOG1(LOG_TRACE, "%6d:[NO_TOKEN] SawFRame "
							 "--> [IDLE]", clk);
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
			if (dst_addr == lnk->ts && frm_type == FRM_REPLY_POLL_FOR_MASTER) { 
				lnk->sole_master = false;
				ns = src_addr;
				event_count = 0;
				bacnet_mstp_frame_send(lnk, TOKEN(ns, lnk->ts), NULL, 0);
				RESET_SILENCE_TIMER();
				ps = lnk->ts;
				token_count = 0;
				retry_count = 0;
				rcvd_valid_frm = false;
				lnk->state = MSTP_PASS_TOKEN;
				DCC_LOG1(LOG_TRACE, "%6d:[POLL_FOR_MASTER] ReceivedReplyToPFM "
						"--> [PASS_TOKEN]", clk);
			} else {
				rcvd_valid_frm = false;
				lnk->state = MSTP_IDLE;
				DCC_LOG1(LOG_TRACE, "%6d:[POLL_FOR_MASTER] "
						 "ReceivedUnexpectedFrame --> [IDLE]", clk);
				goto transition_now;
			}
		} else if (SILENCE_TIMER() >= T_USAGE_TIMEOUT || 
				   rcvd_invalid_frm == true) { 
			if (lnk->sole_master == true) {
				frame_count = 0;
				lnk->state = MSTP_USE_TOKEN;
				DCC_LOG1(LOG_TRACE, "%6d:[POLL_FOR_MASTER] SoleMaster "
						"--> [USE_TOKEN]", clk);
			} else {
				if (ns != lnk->ts) {
					event_count = 0;
					bacnet_mstp_frame_send(lnk, TOKEN(ns, lnk->ts), NULL, 0);
					RESET_SILENCE_TIMER();
					retry_count = 0;
					lnk->state = MSTP_PASS_TOKEN;
					DCC_LOG1(LOG_INFO, "%6d:[POLL_FOR_MASTER] DoneWithPFM "
							"--> [PASS_TOKEN]", clk);
				} else {
					if ((ps + 1) % (N_MAX_MASTER + 1) != lnk->ts) {
						ps = (ps + 1) % (N_MAX_MASTER + 1);
						bacnet_mstp_frame_send(lnk, 
											   POLL_FOR_MASTER(ps, lnk->ts), 
											   NULL, 0);
						RESET_SILENCE_TIMER();
						retry_count = 0;
						DCC_LOG1(LOG_TRACE, "%6d:[POLL_FOR_MASTER] "
								 "SendNextPFM --> [POLL_FOR_MASTER]", clk);
					} else {
						lnk->sole_master = true;
						frame_count = 0;
						rcvd_invalid_frm = false;
						lnk->state = MSTP_USE_TOKEN;
						DCC_LOG1(LOG_TRACE, "%6d:[POLL_FOR_MASTER] "
								 "DeclareSoleMaster --> [USE_TOKEN]", clk);
						goto transition_now;
					}
				}
			}
			rcvd_invalid_frm = false;
		}
		break;


	case MSTP_ANSWER_DATA_REQUEST:
		if (SILENCE_TIMER() < T_REPLY_DELAY && lnk->tx.pdu_len > 0  &&
			(lnk->tx.frm_type == FRM_TEST_RESPONSE ||
			 lnk->tx.frm_type == FRM_BACNET_DATA_NO_REPLY || 
			 lnk->tx.frm_type > 127)) {
			bacnet_mstp_frame_send(lnk, FRAME(lnk->tx.frm_type,  
											  lnk->tx.dst_addr, lnk->ts),
								   lnk->tx.pdu, lnk->tx.pdu_len);
			RESET_SILENCE_TIMER();
			lnk->state = MSTP_IDLE;
			DCC_LOG(LOG_TRACE, "[ANSWER_DATA_REQUEST] Reply"
					"--> IDLE[]");
			rcvd_valid_frm = false;
		} else if (SILENCE_TIMER() >= T_REPLY_DELAY) {
			bacnet_mstp_frame_send(lnk, REPLY_POSTPONED(src_addr, lnk->ts),
								   NULL, 0);
			RESET_SILENCE_TIMER();
			lnk->state = MSTP_IDLE;
			DCC_LOG(LOG_TRACE, "[ANSWER_DATA_REQUEST] DeferredReply"
					"--> [IDLE]");
			rcvd_valid_frm = false;
		}
		break;
	}

	goto again;
}

int bacnet_mstp_recv(struct bacnet_mstp_lnk * lnk, uint8_t pdu[], 
					unsigned int max)
{
	int pdu_len;

	for(;;) {
		if (thinkos_flag_take(lnk->rx.flag) < 0) {
			DCC_LOG(LOG_ERROR, "thinkos_flag_take() failed!");
			abort();
		}

		/* check for a slot in the xmit queue ... */
		if (lnk->rx.pdu_len) 
			break;
	};

	pdu_len = lnk->rx.pdu_len;
	memcpy(pdu, lnk->rx.pdu, pdu_len);

	lnk->rx.pdu_len = 0;

	return pdu_len;
}

int bacnet_mstp_send(struct bacnet_mstp_lnk * lnk, const uint8_t pdu[], 
					unsigned int len)
{
	uint32_t clk;
	int saddr = lnk->ts;
	int daddr = 1;

	for(;;) {
		if (thinkos_flag_take(lnk->tx.flag) < 0) {
			DCC_LOG(LOG_ERROR, "thinkos_flag_take() failed!");
			abort();
		}

		/* check for a slot in the xmit queue ... */
		if (lnk->tx.pdu_len == 0) 
			break;
	}

	/* insert frame in the transmission queue ...  */
	memcpy(lnk->tx.pdu, pdu, len);
	lnk->tx.pdu_len = len;

	return len;
}

const struct bacnetdl_op bacnet_mstp_op = {
	.recv = (void *)bacnet_mstp_recv,
	.send = (void *)bacnet_mstp_send
};

int bacnet_mstp_init(struct bacnet_mstp_lnk * lnk, 
					 const char * name, unsigned int addr,
					 struct serial_dev * dev)
{
	memset(lnk, 0, sizeof(struct bacnet_mstp_lnk));

	lnk->dev = dev;
	lnk->state = MSTP_INITIALIZE;
	lnk->ts = addr;

	DCC_LOG(LOG_TRACE, "[MSTP_INITIALIZE]");
	lnk->rx.flag = thinkos_flag_alloc();
	lnk->tx.flag = thinkos_flag_alloc();

	DCC_LOG2(LOG_TRACE, "tx.flag=%d rx.flag=%d", lnk->rx.flag, lnk->tx.flag);

	lnk->dln = bacnet_dl_register(name, lnk, &bacnet_mstp_op);

	return 0;
}

int bacnet_mstp_start(struct bacnet_mstp_lnk * lnk)
{
	uint32_t clk;

	DCC_LOG(LOG_TRACE, "Starting BACnet MS/TP Data Link");

	lnk->state = MSTP_IDLE;
	DCC_LOG(LOG_TRACE, "[MSTP_IDLE]");

	return 0;
}


