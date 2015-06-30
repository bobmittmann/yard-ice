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
#include <trace.h>

#include "rtp.h"
#include "g711.h"
#include "wavetab.h"

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

void audio_alaw_enqueue(struct jitbuf * jb, uint32_t ts,
		uint8_t * data, unsigned int samples)
{
	int16_t * dst;
	uint8_t * src;
	struct sndbuf * pcm;
	int cnt = 0;
	int rem;
	int i;
	int n;
//	static int j = 0;

	rem = samples;
	while (rem > 0) {
		if ((pcm = sndbuf_alloc()) == NULL)
			return;

		src = (uint8_t *)&data[cnt++ * SNDBUF_LEN];
		dst = (int16_t *)pcm->data;
		n = rem > SNDBUF_LEN ? SNDBUF_LEN : rem;

		for (i = 0; i < n; ++i)
			dst[i] = ((int)alaw2linear(src[i]) << 3) + 0x8000;

#if 0
		for (i = 0; i < n; ++i) {
				dst[i] = (int)wave_a3[j++] + 0x8000;
				if (j == sizeof(wave_a3) / 2)
					j = 0;
		}
#endif

		jitbuf_enqueue(jb, pcm, ts);

		sndbuf_free(pcm);

		ts += n;
		rem -= n;
	}
}

int rtp_g711_recv(struct rtp_session * __rtp, struct sockaddr_in * __sin)
{
	uint32_t pkt_buf[256];
	struct rtp_packet * pkt = (struct rtp_packet *)pkt_buf;
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
		return -1;
	}

	if (pkt->payload != 8) {
		ERR("rtp payload != 8");
		return -1;
	}

	data_len = len - sizeof(struct rtp_packet);

	audio_alaw_enqueue(__rtp->jb, pkt->timestamp,
			pkt->data, data_len);

	return data_len;
}


