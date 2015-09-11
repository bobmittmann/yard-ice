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
 * but WITHOUT ANY WARRANTY; without even the implied warranty of * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */

/** 
 * @file rtp_test.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <errno.h>

#include <thinkos.h>
#include <trace.h>
#include <assert.h>

#include "rtp.h"
#include "audio.h"

struct rtp_packet {
    /* byte 0 */
#if (BYTE_ORDER == LITTLE_ENDIAN)
    uint8_t csrc_len:4;   /* expect 0 */
    uint8_t extension:1;  /* expect 1, see RTP_OP below */
    uint8_t padding:1;    /* expect 0 */
    uint8_t version:2;    /* expect 2 */
#elif (BYTE_ORDER == BIG_ENDIAN)
    uint8_t version:2;
    uint8_t padding:1;
    uint8_t extension:1;
    uint8_t csrc_len:4;
#else
#error "Adjust your <bits/endian.h> defines"
#endif
    /* byte 1 */
#if (BYTE_ORDER == LITTLE_ENDIAN)
    uint8_t payload:7;    /* RTP_PAYLOAD_RTSP */
    uint8_t marker:1;     /* expect 1 */
#elif (BYTE_ORDER == BIG_ENDIAN)
    uint8_t marker:1;
    uint8_t payload:7;
#endif
    /* bytes 2, 3 */
    uint16_t seq_no;
    /* bytes 4-7 */
    uint32_t timestamp;
    /* bytes 8-11 */
    uint32_t ssrc;    /* stream number is used here. */

    uint8_t data[]; /**< Variable-sized data payload */
};

int rtp_g711_recv(struct rtp_session * __rtp, struct sockaddr_in * __sin)
{
	uint32_t pkt_buf[1496 / 4];
	struct rtp_packet * pkt = (struct rtp_packet *)pkt_buf;
	uint8_t * data_buf;
	uint32_t ts;
	int data_len;
	int len;

	if ((len = udp_recv(__rtp->udp[0], pkt, sizeof(pkt_buf), __sin)) < 0) {
		if (len == -ECONNREFUSED) {
			ERR("udp_rcv ICMP error: ECONNREFUSED");
		}
		if (len == -EFAULT) {
			ERR("udp_rcv error: EFAULT");
		}
		if (len == -ENOTCONN) {
			ERR("udp_rcv error: ENOTCONN");
		}
		return -1;
	}

	if (pkt->version != 2) {
		ERR("rtp version != 2");
		return 0;
	}

	if ((pkt->payload != 8) && (pkt->payload != 96)) {

		ERR("rtp payload(%d) != 8 | 96", pkt->payload);
		return 0;
	}

	if (pkt->csrc_len != 0) {
		ERR("csrc_len (%d) != 0", pkt->csrc_len);
		return 0;
	}

	if (pkt->ssrc != __rtp->ssrc) {
		ERR("SSRC (%0x8) != %0x8 !", pkt->ssrc, __rtp->ssrc);
		return 0;
	}

	ts = ntohl(pkt->timestamp);
	data_len = len - sizeof(struct rtp_packet);

	data_buf = pkt->data;

	if (__rtp->rem) {
		int n = SNDBUF_LEN - __rtp->rem;

		if (n > data_len)
			n = data_len;
		memcpy(&__rtp->buf[__rtp->rem], data_buf, n);

		__rtp->rem += n;
		if (__rtp->rem < SNDBUF_LEN)
			return 0;

		ts = __rtp->ts;
		__rtp->ts = ts + SNDBUF_LEN;
//		DBG("%5d --> %d (buf)", ts, SNDBUF_LEN);
		audio_alaw_enqueue(ts, __rtp->buf, SNDBUF_LEN);
		data_buf += n;
		data_len -= n;
	}

	if ((__rtp->rem = (data_len % SNDBUF_LEN)) != 0) {
		data_len -= __rtp->rem;
		memcpy(__rtp->buf, &data_buf[data_len], __rtp->rem);
	}

	ts = __rtp->ts;
	__rtp->ts = ts + data_len;
//	DBG("%5d --> %d (pkt)", ts, data_len);
	audio_alaw_enqueue(ts, data_buf, data_len);

	return data_len;
}

int rtp_task(struct rtp_session * rtp_s)
{
	struct ntp_time ntp;
	struct sockaddr_in sin;
	struct udp_pcb * rtp_udp;
	struct udp_pcb * rtcp_udp;
	uint32_t clk;
	uint32_t now;
	int32_t dt;
	int len;

	INF("RTP client started (thread %d).", thinkos_thread_self());
	DBG("RTP port: %d", rtp_s->lport[0]);
	DBG("RTCP port: %d", rtp_s->lport[1]);

	if ((rtp_udp = udp_alloc()) == NULL) {
		ERR("udp_alloc() failed!");
		return -1;
	}

	if (udp_bind(rtp_udp, INADDR_ANY, htons(rtp_s->lport[0])) < 0) {
		ERR("udp_bind() failed!");
		return -1;
	}

	if ((rtcp_udp = udp_alloc()) == NULL) {
		ERR("udp_alloc() failed!");
		return -1;
	}

	if (udp_bind(rtcp_udp, INADDR_ANY, htons(rtp_s->lport[1])) < 0) {
		ERR("udp_bind() failed!");
		return -1;
	}

	rtp_s->udp[0] = rtp_udp;
	rtp_s->udp[1] = rtcp_udp;

	ntp.sec = 2208988800u;
	ntp.frac = 0;

	clk = thinkos_clock();
	for (;;) {
		len = rtp_g711_recv(rtp_s, &sin);

		if (len < 0) {
			ERR("rtp_g711_recv() failed!");
			udp_close(rtp_s->udp[0]);
			return -1;
		}

		now = thinkos_clock();
		if ((dt = (int32_t)(now - clk)) > 5000) {
			clk = now;

			DBG("RTCP report");

		    ntp.sec += dt / 1000;
		    ntp.frac = (((uint64_t)(dt % 1000)) << 32) / 1000u;

			sin.sin_family = AF_INET;
			sin.sin_addr.s_addr = rtp_s->faddr;
			sin.sin_port = htons(rtp_s->fport[1]);
			rtcp_send_sr_sdes(rtp_s, 0, &ntp, &sin);
		}

	}

	return 0;
}

void rtp_close_session(struct rtp_session * rtp)
{
	rtp->ssrc = 0;
	rtp->rem = 0;
}

void rtp_open_session(struct rtp_session * rtp, uint32_t ssrc)
{
	rtp->octet_count = 0;
	rtp->pkt_count = 0;
	rtp->seq_no = 0;
	rtp->start_seq = 0;
	rtp->ssrc = ntohl(ssrc);
}

uint32_t rtp_stack[1024];

const struct thinkos_thread_inf rtp_inf = {
	.stack_ptr = rtp_stack,
	.stack_size = sizeof(rtp_stack),
	.priority = 32,
	.thread_id = 8,
	.paused = 0,
	.tag = "RTP"
};

void rtp_g711_start(struct rtp_session * rtp)
{
	rtp->rem = 0;
	rtp->octet_count = 0;
	rtp->pkt_count = 0;
	rtp->seq_no = 0;
	rtp->start_seq = 0;
	rtp->ssrc = 0;

	thinkos_thread_create_inf((void *)rtp_task, (void *)rtp, &rtp_inf);
}

