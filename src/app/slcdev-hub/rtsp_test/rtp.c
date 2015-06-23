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

int rtp_recv(struct rtp_client * __rtp, void * __buf, int __len)
{
	struct sockaddr_in sin;
	struct rtp_packet * pkt = (struct rtp_packet *)__buf;
	int len;

	if ((len = udp_recv(__rtp->udp, __buf, __len, &sin)) < 0) {
		if (len == -ECONNREFUSED) {
			ERR("udp_rcv ICMP error: ECONNREFUSED\n");
		}
		if (len == -EFAULT) {
			ERR("udp_rcv error: EFAULT\n");
		}
		if (len == -ENOTCONN) {
			ERR("udp_rcv error: ENOTCONN\n");
		}
		return -1;
	}

	if (pkt->version != 2) {
		ERR("rtp version != 2");
		return -1;
	}


	return 0;
}


